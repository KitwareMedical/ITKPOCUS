package com.kitware.medical.clariusstream

import android.app.*
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.graphics.Color
import android.graphics.drawable.Icon
import android.net.ConnectivityManager
import android.net.LinkProperties
import android.net.Network
import android.os.*
import android.util.Log
import android.util.Size
import android.widget.Toast
import com.igtl.ImageServer
import me.clarius.mobileapi.MobileApi
import java.lang.Exception
import java.lang.ref.WeakReference
import java.net.Inet4Address

class StreamingService : Service() {

    companion object {
        const val NOTIFICATION_ID = 1
        const val TAG = "ClariusStream"
        const val IGTL_PORT = 18944

        init {
            System.loadLibrary("igtljava")
        }
    }

    // messenger to clarius service
    private var mService: Messenger? = null

    // messenger to handle replies from clarius service
    private var mClient: Messenger? = null

    // is clarius service bound
    private var mBound: Boolean = false

    // are we registered with the clarius service
    private var mRegistered: Boolean = false

    // thread to send images
    private var mImageWorkerThread: HandlerThread? = null

    // messenger to the image worker handler
    private var mImageWorker: Messenger? = null

    // the igtl image server
    private var mImageServer: ImageServer? = null

    private lateinit var mNotificationBuilder: Notification.Builder

    private val mNetworkCallback = object : ConnectivityManager.NetworkCallback() {
        override fun onLinkPropertiesChanged(network: Network, linkProperties: LinkProperties) {
            val addr = getWifiAddress(linkProperties)
            updateForegroundNotification(addr)
        }

        override fun onLost(network: Network) {
            updateForegroundNotification("")
        }
    }

    private val mObserver = object : ClariusMessageObserver {
        private var counter = 0

        override fun onConnected(connected: Boolean) {
            if (connected) {
                Toast.makeText(applicationContext, "Connected!", Toast.LENGTH_SHORT).show()
                sendImageConfig()
            }
        }

        override fun onConfiguredImage(configured: Boolean) {
            Log.d(TAG, "Configured image status: $configured")
        }

        override fun onNewProcessedImage(data: Bundle) {
            val msg = Message.obtain(null, Constants.ImageWorker.IMAGE_DATA)
            msg.data = data
            try {
                mImageWorker?.send(msg)
            } catch (e: RemoteException) {
                e.printStackTrace()
            }
        }
    }

    // connection to Clarius service
    private val mConn = object : ServiceConnection {
        override fun onServiceConnected(name: ComponentName?, service: IBinder?) {
            // wraps the raw IBinder to the service with the Messenger interface
            mService = Messenger(service)
            mClient = Messenger(ClariusHandler(mObserver))
            mBound = true
            registerWithClariusService()
        }

        override fun onServiceDisconnected(name: ComponentName?) {
            // unregisterFromService invoked in teardownService
            mService = null
            mClient = null
            mBound = false
        }

        override fun onNullBinding(name: ComponentName?) {
            Toast.makeText(
                applicationContext,
                "Clarius service refused to start; bad license?",
                Toast.LENGTH_SHORT
            ).show()
        }
    }

    /**
     * Callback parameters used to map outbound messages to return status messages from the service.
     *
     * How it works:
     *  1. When sending a message, set Message.arg1 to a unique value;
     *  2. Wait for a MSG_RETURN_STATUS from the service and check its Message.arg1 field:
     *      a. if it matches, it is the return status for our request;
     *      b. otherwise, it is the return status for another request;
     *  3. Use different codes to differentiate different requests.
     */
    class Requests {
        companion object {
            const val REGISTER = 1
            const val CONFIGURE_IMAGE = 2
        }
    }

    class ClariusHandler(
        observer: ClariusMessageObserver,
        private val mObserver: WeakReference<ClariusMessageObserver> = WeakReference(
            observer
        )
    ) : Handler(Looper.getMainLooper()) {
        override fun handleMessage(msg: Message) {
            when (msg.what) {
                MobileApi.MSG_RETURN_STATUS -> {
                    val param = msg.arg1
                    val status = msg.arg2
                    when (param) {
                        Requests.REGISTER -> {
                            mObserver.get()?.onConnected(status == 0)
                        }
                        Requests.CONFIGURE_IMAGE -> {
                            mObserver.get()?.onConfiguredImage(status == 0)
                        }
                        else -> {}
                    }
                }
                MobileApi.MSG_NEW_PROCESSED_IMAGE -> {
                    mObserver.get()?.onNewProcessedImage(msg.data)
                }
                else -> super.handleMessage(msg)
            }
        }
    }

    override fun onBind(intent: Intent?): IBinder? {
        return null
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        intent?.let {
            when (it.action) {
                Constants.START_SERVICE -> {
                    if (bindToClariusService()) {
                        setupService()
                        setupImageWorkerThread()
                        setupNetworkCallback()
                    }
                }
                Constants.STOP_SERVICE -> {
                    teardownNetworkCallback()
                    teardownImageWorkerThread()
                    teardownService(startId)
                }
                else -> {
                    throw Exception("Invalid command")
                }
            }
        }
        return START_STICKY
    }

    private fun bindToClariusService(): Boolean {
        Intent().also { intent ->
            intent.component =
                ComponentName(BuildConfig.CLARIUS_PACKAGE_NAME, BuildConfig.CLARIUS_SERVICE_NAME)
            if (bindService(intent, mConn, Context.BIND_AUTO_CREATE)) {
                return true
            }

            // failed to bind service
            unbindService(mConn)
            return false
        }
    }

    private fun setupService() {
        if (!this::mNotificationBuilder.isInitialized) {
            val notificationIntent = Intent(this, MainActivity::class.java)
            val pendingMainIntent = PendingIntent.getActivity(this, 0, notificationIntent, 0)

            val stopIntent = Intent(this, StreamingService::class.java).also {
                it.action = Constants.STOP_SERVICE
            }
            val pendingStopIntent = PendingIntent.getService(this, 0, stopIntent, 0)
            val stopAction = Notification.Action.Builder(
                Icon.createWithResource(
                    this,
                    R.drawable.ic_launcher_background
                ), "Stop", pendingStopIntent
            ).build()

            val connMgr = getSystemService(ConnectivityManager::class.java)
            val linkProps = connMgr.getLinkProperties(connMgr.activeNetwork)
            val addr = getWifiAddress(linkProps)
            val ipPort = if (addr.isEmpty()) {
                "No IP address"
            } else {
                "$addr:$IGTL_PORT"
            }

            val channelId =
                createNotificationChannel("clarius_stream_service", "Clarius Stream Service")
            mNotificationBuilder = Notification.Builder(this, channelId)
                .setContentTitle("Clarius IGTL Stream enabled")
                .setContentText("IP/Port: $ipPort")
                .setSmallIcon(R.drawable.ic_launcher_background)
                .setContentIntent(pendingMainIntent)
                .addAction(stopAction)
        }

        startForeground(NOTIFICATION_ID, mNotificationBuilder.build())
    }

    private fun createNotificationChannel(channelId: String, channelName: String): String {
        val chan = NotificationChannel(channelId, channelName, NotificationManager.IMPORTANCE_NONE)
        chan.lightColor = Color.GREEN
        chan.lockscreenVisibility = Notification.VISIBILITY_PRIVATE
        val service = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
        service.createNotificationChannel(chan)
        return channelId
    }

    private fun teardownService(startId: Int) {
        if (mBound) {
            unregisterFromClariusService()
            unbindService(mConn)
        }
        // will also invoke stopForeground(true)
        stopSelf(startId)
    }

    private fun setupImageWorkerThread() {
        mImageWorkerThread = HandlerThread("ImageWorkerThread").also { thread ->
            thread.start()
            mImageServer = ImageServer().also { server ->
                val result = server.CreateServer(IGTL_PORT)
                Log.d(TAG, "igtl server creation status: $result")
                mImageWorker = Messenger(ImageWorkerHandler(thread.looper, server))
            }
        }
    }

    private fun teardownImageWorkerThread() {
        mImageWorkerThread?.quit()
        mImageWorkerThread = null
        mImageWorker = null
        mImageServer?.delete()
        mImageServer = null
    }

    private fun setupNetworkCallback() {
        val connMgr = getSystemService(ConnectivityManager::class.java)
        connMgr.registerDefaultNetworkCallback(mNetworkCallback)
    }

    private fun teardownNetworkCallback() {
        val connMgr = getSystemService(ConnectivityManager::class.java)
        connMgr.unregisterNetworkCallback(mNetworkCallback)
    }

    private fun registerWithClariusService() {
        if (mBound) {
            val msg = Message.obtain(null, MobileApi.MSG_REGISTER_CLIENT)
            msg.replyTo = mClient
            // sets the callback param
            msg.arg1 = Requests.REGISTER
            try {
                mService?.send(msg)
                mRegistered = true
            } catch (e: RemoteException) {
                e.printStackTrace()
                Toast.makeText(this, "Failed to register with service", Toast.LENGTH_SHORT).show()
            }
        }
    }

    private fun unregisterFromClariusService() {
        if (mBound && mRegistered) {
            val msg = Message.obtain(null, MobileApi.MSG_UNREGISTER_CLIENT)
            try {
                mService?.send(msg)
                mRegistered = false
            } catch (e: RemoteException) {
                e.printStackTrace()
                Toast.makeText(this, "Failed to unregister from service", Toast.LENGTH_SHORT).show()
            }
        }
    }


    private fun makeImageConfig(): Bundle {
        return Bundle().also { bundle ->
            bundle.putSize(MobileApi.KEY_IMAGE_SIZE, Size(400, 400))
            bundle.putString(MobileApi.KEY_COMPRESSION_TYPE, MobileApi.COMPRESSION_TYPE_JPEG)
            bundle.putInt(MobileApi.KEY_COMPRESSION_QUALITY, 90)
        }
    }

    private fun sendImageConfig() {
        if (mBound && mRegistered) {
            val msg = Message.obtain(null, MobileApi.MSG_CONFIGURE_IMAGE)
            msg.replyTo = mClient
            msg.arg1 = Requests.CONFIGURE_IMAGE
            msg.data = makeImageConfig()
            try {
                mService?.send(msg)
            } catch (e: RemoteException) {
                e.printStackTrace()
                Toast.makeText(this, "Failed to send image config", Toast.LENGTH_SHORT).show()
            }
        }
    }

    private fun getWifiAddress(linkProps: LinkProperties?): String {
        return linkProps?.linkAddresses?.find { it.address is Inet4Address }
            ?.let { addr ->
            addr.address.hostAddress
        } ?: ""
    }

    private fun updateForegroundNotification(addr: String) {
        val ipPort = if (addr.isEmpty()) {
            "No IP address"
        } else {
            "$addr:$IGTL_PORT"
        }

        mNotificationBuilder.setContentText("IP/Port: $ipPort")

        val notificationMgr = getSystemService(NotificationManager::class.java) as NotificationManager
        notificationMgr.notify(NOTIFICATION_ID, mNotificationBuilder.build())
    }
}