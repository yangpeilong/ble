package com.qding.myapp

import android.os.Bundle
import android.os.Handler
import android.util.Log
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import com.qding.myapp.decoration.MonkeyComponent
import com.ys.rkapi.MyManager
import me.yokeyword.fragmentation.SupportActivity

class MainActivity : SupportActivity() {
    private var manager: MyManager? = null

    val TAG = "MainActivity"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)








    }


}