package com.qdingnet.provider.config.callback.impl;

import com.qdingnet.provider.config.bean.QDAccessResult;
import com.qdingnet.provider.config.bean.StateGuard;
import com.qdingnet.provider.config.callback.IWriteConfigCallback;
import com.qdingnet.provider.config.handler.QdingDataPacketHandler;

import java.util.concurrent.atomic.AtomicBoolean;

public class WriteConfigCallbackAdapter extends AbstractStateCallback implements IWriteConfigCallback {
    private static final String TAG = "WriteConfigCallbackAdapter";
    //写配置结果回调
    private final IWriteConfigCallback mWriteConfigCallback;
    //回调上层锁
    private final AtomicBoolean mReturnCallbackLock = new AtomicBoolean(false);

    public WriteConfigCallbackAdapter(StateGuard stateGuard, IWriteConfigCallback callback) {
        super(MODE_BLE, stateGuard);
        this.mWriteConfigCallback = callback;
    }

    @Override
    protected void onStateSuccess() {}

    @Override
    protected void onStateFailure(String ssid, QDAccessResult result) {
        onWriteConfigResult(QdingDataPacketHandler.getDeviceMac(ssid), result);
    }

    @Override
    public void onWriteConfigResult(String mac, QDAccessResult result) {
        if(mWriteConfigCallback != null && mReturnCallbackLock.compareAndSet(false, true)){
            mWriteConfigCallback.onWriteConfigResult(mac, result);
        }
    }
}
