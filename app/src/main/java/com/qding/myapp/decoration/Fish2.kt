package com.qding.myapp.decoration

/**
 *  Created by yangyeah on 2021/4/13
 *
 *  describe:
 */
class Fish2 : Decorator {

    constructor(component: Component):super(component)

    override fun operate() {
        println("Fish2-----opetate--before")
        super.operate()
        println("Fish2-----opetate--after")
    }

    override fun getDecorateName(): String {
        return "<<<Fish2"
    }

    override fun getName(): String {
        return "Fish2"
    }
}