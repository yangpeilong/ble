package com.qdingnet.opendoor.core.interceptor.bluetooth;


import android.util.Log;

import androidx.annotation.NonNull;

import com.qdingnet.opendoor.core.bluetooth.BluetoothLeClass;
import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.opendoor.core.interceptor.AbstractInterceptor;

/**
 * 蓝牙重试处理拦截器
 * @author pipan (pipan@qding.me)
 * @date   19-1-22
 */
public class RetryAndFollowUpInterceptor extends AbstractInterceptor<BluetoothLeClass> {
    private static final String TAG = "RetryAndFollowUpInterceptor";

    public RetryAndFollowUpInterceptor(@NonNull BluetoothLeClass bluetoothLeClass) {
        super(bluetoothLeClass, IStateCallback.STATE_INIT);
    }

    /**
     * 重试操作
     */
    public void retry(){
        //no-op temporarily
    }

    @Override
    public void intercept(Chain next) {
        super.intercept(next);
        Log.i(TAG, "intercept next = "+next.request().toString());
        //设置初始化成功状态
        onState(IStateCallback.CODE_SUCCESS, "");
        next.process(null);
    }

}
