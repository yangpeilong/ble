package com.qding.myapp.decoration

/**
 *  Created by yangyeah on 2021/4/13
 *
 *  describe:
 */
abstract class Decorator : Component {

    private var component: Component? = null

    constructor(component: Component){
        this.component = component
    }


    override fun operate() {

        println("Decorator-----getDecorateName=${getDecorateName()}")
        println("Decorator-----getName=${component?.getName()}")
        component?.operate()
    }

    abstract fun getDecorateName():String
}