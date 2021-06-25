package com.qding.myapp

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import me.yokeyword.fragmentation.SupportFragment

/**
 *  Created by yangyeah on 2021/2/2
 *
 *  describe:
 */
class LeftFrag : SupportFragment() {
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.f_left,container,false)
    }
}