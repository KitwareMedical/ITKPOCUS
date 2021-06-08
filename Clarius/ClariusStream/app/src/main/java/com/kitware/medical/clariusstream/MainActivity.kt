package com.kitware.medical.clariusstream

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        var launchBtn: Button = findViewById(R.id.launchBtn)
        launchBtn.setOnClickListener {
            sendServiceIntent(Constants.START_SERVICE)
        }

        var stopBtn: Button = findViewById(R.id.stopBtn)
        stopBtn.setOnClickListener {
            sendServiceIntent(Constants.STOP_SERVICE)
        }
    }

    private fun sendServiceIntent(action: String) {
        val serviceIntent = Intent(this, StreamingService::class.java).also {
            it.action = action
        }
        startService(serviceIntent)
    }
}