package com.qding.myapp.decoration.cake

/**
 *  Created by yangyeah on 2021/4/13
 *
 *  describe:
 */
class FeatherCake : Cake {
    override fun descName(): String {
        return "原味手抓饼"
    }

    override fun money(): Double {
        return 3.0
    }
}