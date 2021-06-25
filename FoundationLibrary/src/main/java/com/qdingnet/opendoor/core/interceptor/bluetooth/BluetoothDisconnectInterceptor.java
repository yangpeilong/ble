package com.qdingnet.opendoor.core.interceptor.bluetooth;


import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.qdingnet.opendoor.core.bluetooth.BluetoothLeClass;
import com.qdingnet.opendoor.core.interceptor.AbstractInterceptor;

/**
 * 蓝牙断开连接拦截器
 * @author pipan (pipan@qding.me)
 * @date   19-1-22 
 */
public class BluetoothDisconnectInterceptor extends AbstractInterceptor<BluetoothLeClass> {
    private static final String TAG = "BLEDisconnInterceptor";

    public BluetoothDisconnectInterceptor(@NonNull BluetoothLeClass bluetoothLeClass, int state) {
        super(bluetoothLeClass, state);
    }

    @Override
    public void intercept(Chain next) {
        super.intercept(next);
    }
}
