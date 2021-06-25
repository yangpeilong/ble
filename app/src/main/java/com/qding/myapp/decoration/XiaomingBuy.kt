package com.qding.myapp.decoration

/**
 *  Created by yangyeah on 2021/4/13
 *
 *  describe:
 */
class XiaomingBuy : AbstractBuyPhone() {

    override fun buyPhone() {
        println("XiaomingBuy---buyPhone")
    }

    override fun agree() {
      println("XiaomingBuy---agree")
    }
}