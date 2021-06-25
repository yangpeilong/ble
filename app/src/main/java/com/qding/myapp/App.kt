package com.qding.myapp

import android.app.Application
import android.util.Log
import com.qdingnet.provider.config.ConfigManager

/**
 *  Created by yangyeah on 2021/3/29
 *
 *  describe:
 */
class App :Application() {

    companion object {
        var mConfigManager: ConfigManager? = null
    }

    override fun onCreate() {
        super.onCreate()

       mConfigManager = ConfigManager(this)

        Log.d(
            "LibraryApplication",
            "onCreate...ConfigManager...sdk_version:" + mConfigManager!!.sdkVersion + ",build_no:" + mConfigManager!!.sdkBuildNo
        )
    }
}