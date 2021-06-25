package com.qding.myapp

import com.qding.myapp.decoration.*
import com.qding.myapp.decoration.cake.EggCake
import com.qding.myapp.decoration.cake.FeatherCake
import com.qding.myapp.decoration.cake.NiuCake
import java.io.*
import java.util.*

/**
 *  Created by yangyeah on 2021/3/11
 *
 *  describe:
 */
object Test {
    @JvmStatic
    fun main(args: Array<String>) {
        val content = "hello world"
        val content2 = "一二三"

        val array = content.toByteArray()
        val array2 = content2.toByteArray()

        println("UTF-8===>${array.contentToString()}")
        println("UTF-8===>${array2.contentToString()}")


        val testarray = byteArrayOf(36, 18, 69, 65, 24, 0, 118, -5, 75, 83, -94, 46, -46, 82, -126, -123, 46, 46, 70, 21, 25, 50, -84, 15)

        //println("testarray=====${String(testarray,Charsets.UTF_8)}")





    }
}