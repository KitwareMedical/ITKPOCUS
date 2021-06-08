package com.kitware.medical.clariusstream

import android.os.Bundle

interface ClariusMessageObserver {
    fun onConnected(connected: Boolean)
    fun onConfiguredImage(configured: Boolean)
    fun onNewProcessedImage(data: Bundle)
}