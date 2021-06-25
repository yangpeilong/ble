package com.qding.myapp.decoration.cake

import android.icu.text.RelativeDateTimeFormatter

/**
 *  Created by yangyeah on 2021/4/13
 *
 *  describe:
 */
open class DecoratorCake(val cake: Cake) : Cake {


    override fun descName(): String {
       return cake.descName()
    }

    override fun money(): Double {
        return cake.money()
    }
}