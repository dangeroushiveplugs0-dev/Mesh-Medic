package com.dangeroushive.meshmedic

import android.app.Activity
import android.os.Bundle
import android.widget.TextView

class MainActivity : Activity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        check(NativeBridge.initializeCore()) {
            "wesh-Seller native C++ core failed to initialize"
        }

        setContentView(
            TextView(this).apply {
                text = "wesh-Seller\nNative C++ core initialized"
                textSize = 22f
                setPadding(32, 32, 32, 32)
            }
        )
    }
}
