package com.qding.myapp.decoration

/**
 *  Created by yangyeah on 2021/4/13
 *
 *  describe:
 */
open class DecoratorBuyPhone : AbstractBuyPhone {

    private var abstractBuyPhone: AbstractBuyPhone? = null

    constructor(abstractBuyPhone: AbstractBuyPhone) {
        this.abstractBuyPhone = abstractBuyPhone
    }


    override fun buyPhone() {
        abstractBuyPhone?.buyPhone()
    }

    override fun agree() {
        abstractBuyPhone?.agree()
    }
}