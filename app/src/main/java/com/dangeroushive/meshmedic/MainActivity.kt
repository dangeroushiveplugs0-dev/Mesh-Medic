package com.dangeroushive.meshmedic

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.ui.Modifier
import androidx.compose.ui.viewinterop.AndroidView

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        check(NativeBridge.initializeCore())

        setContent {
            AndroidView(
                modifier = Modifier.fillMaxSize(),
                factory = { FilamentViewport(it) }
            )
        }
    }
}
