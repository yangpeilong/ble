package com.qding.myapp.decoration.cake

/**
 *  Created by yangyeah on 2021/4/13
 *
 *  describe:
 */
class EggCake( egg: Cake):DecoratorCake(egg){

    override fun descName(): String {
        return "鸡蛋" + super.descName()
    }

    override fun money(): Double {
        return super.money() + 2.0
    }


}