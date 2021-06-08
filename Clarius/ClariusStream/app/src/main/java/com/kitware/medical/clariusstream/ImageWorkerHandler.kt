package com.kitware.medical.clariusstream

import android.graphics.BitmapFactory
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.util.Log
import com.igtl.ImageServer
import me.clarius.mobileapi.MobileApi
import me.clarius.mobileapi.ProcessedImageInfo
import java.lang.AssertionError
import java.nio.ByteBuffer
import kotlin.experimental.and

class ImageWorkerHandler(looper: Looper, private val imageServer: ImageServer) : Handler(looper) {
    companion object {
        const val TAG = "ImageWorkerHandler"
    }

    override fun handleMessage(msg: Message) {
        when (msg.what) {
            Constants.ImageWorker.IMAGE_DATA -> {
                val data = msg.data
                data.classLoader = ProcessedImageInfo::class.java.classLoader
                val info = data.getParcelable(MobileApi.KEY_IMAGE_INFO) as ProcessedImageInfo?
                    ?: throw AssertionError("Image info missing")

                val imageData = data.getByteArray(MobileApi.KEY_IMAGE_DATA)
                    ?: throw AssertionError("image data missing")
                val bitmap = BitmapFactory.decodeByteArray(imageData, 0, imageData.size)
                    ?: throw AssertionError("Bad image data")

                // Log.d(TAG, "Received image ${info.width}x${info.height} @${info.tm}")

                val size = info.width * info.height
                val pixels = IntArray(size)
                bitmap.getPixels(pixels, 0, info.width, 0, 0, info.width, info.height)

                val grayscale = ShortArray(size)
                for (i in pixels.indices) {
                    grayscale[i] = (pixels[i] and 0xff).toShort()
                }

                val byteBuffer = ByteBuffer.allocate(grayscale.size * 2)
                val shortBuffer = byteBuffer.asShortBuffer()
                shortBuffer.put(grayscale)

                val dims = intArrayOf(info.width, info.height, 1)
                val spacing = floatArrayOf(1f, 1f, 1f)
                imageServer.SendImageBasic(dims, spacing, ImageServer.ImgMsg.TYPE_INT16, 1, "ClariusImage", byteBuffer.array())
                Log.d(TAG, "-- Image sent")
            }
        }
    }
}