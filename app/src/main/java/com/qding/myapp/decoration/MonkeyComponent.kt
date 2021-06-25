package com.qding.myapp.decoration

/**
 *  Created by yangyeah on 2021/4/13
 *
 *  describe:
 */
class MonkeyComponent :Component {

    override fun operate() {
        println("MonkeyComponent-----operate")
    }

    override fun getName(): String {
        return "MonkeyComponent"
    }
}