package com.qding.myapp.decoration

/**
 *  Created by yangyeah on 2021/4/13
 *
 *  describe:
 */
class Bird2(component: Component) : Decorator(component) {

    override fun operate() {
        println("Bird2----operate--before")
        super.operate()
        println("Bird2----operate--after")
    }

    override fun getDecorateName(): String {
        return "<<<bird2"
    }

    override fun getName(): String {
        return "Bird2"
    }
}