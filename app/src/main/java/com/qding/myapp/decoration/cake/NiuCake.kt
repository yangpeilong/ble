package com.qding.myapp.decoration.cake

/**
 *  Created by yangyeah on 2021/4/13
 *
 *  describe:
 */
class NiuCake(val ncake: Cake) : DecoratorCake(ncake) {

    override fun money(): Double {
        return super.money() + 3.0
    }

    override fun descName(): String {
        return "牛肉" + super.descName()
    }
}