package com.qding.myapp

import android.content.Context
import android.content.SharedPreferences

/**
 *  Created by yangyeah on 2021/3/29
 *
 *  describe:
 */
object TextUtil {

    var sp:SharedPreferences?=null

    fun init(context: Context) {
        sp = context.getSharedPreferences("test_xml", Context.MODE_PRIVATE)
    }

    fun getInt(): Int {
        return sp!!.getInt("key_int", -1)
    }



}