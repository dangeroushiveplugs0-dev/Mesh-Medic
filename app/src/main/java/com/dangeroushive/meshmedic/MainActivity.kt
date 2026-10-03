package com.dangeroushive.meshmedic

import android.app.Activity
import android.os.Bundle

class MainActivity : Activity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        check(NativeBridge.initializeCore()) {
            "wesh-Seller native C++ core failed to initialize"
        }

        setContentView(NativeViewport(this))
    }
}
