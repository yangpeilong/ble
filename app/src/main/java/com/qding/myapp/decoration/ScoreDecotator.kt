package com.qding.myapp.decoration

/**
 *  Created by yangyeah on 2021/4/13
 *
 *  describe:
 */
class ScoreDecotator : DecoratorBuyPhone {

    constructor(abstractBuyPhone: AbstractBuyPhone):super(abstractBuyPhone)

    private var score:Int = 0

    override fun buyPhone() {
        if (score > 90) {
            println("成绩不错，同意买")
            super.buyPhone()
            agree()
        } else {
            println("成绩不达标，不同意买")
        }

    }

    override fun agree() {
        super.agree()
    }

    fun setScore(score: Int)  {
       this.score = score
    }

}