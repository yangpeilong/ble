package com.qdingnet.provider.config.callback.impl;

import com.qdingnet.provider.config.bean.DeviceConfigMsg;
import com.qdingnet.provider.config.bean.QDAccessResult;
import com.qdingnet.provider.config.bean.StateGuard;
import com.qdingnet.provider.config.callback.IReadConfigCallback;
import com.qdingnet.provider.config.handler.QdingDataPacketHandler;

import java.util.concurrent.atomic.AtomicBoolean;

/**
 * 读取设备配置回调适配器
 * @author pipan (pipan@qding.me)
 * @date   2019/5/28
 */
public class ReadConfigCallbackAdapter extends AbstractStateCallback implements IReadConfigCallback {
    private static final String TAG = "ReadConfigCallbackAdapter";
    //写配置结果回调
    private final IReadConfigCallback mReadConfigCallback;
    //回调上层锁
    private final AtomicBoolean mReturnCallbackLock = new AtomicBoolean(false);

    public ReadConfigCallbackAdapter(StateGuard stateGuard, IReadConfigCallback callback) {
        super(MODE_BLE, stateGuard);
        this.mReadConfigCallback = callback;
    }

    @Override
    protected void onStateSuccess() {}

    @Override
    protected void onStateFailure(String ssid, QDAccessResult result) {
        onReadConfigResult(QdingDataPacketHandler.getDeviceMac(ssid), result, null);
    }

    @Override
    public void onReadConfigResult(String mac, QDAccessResult result, DeviceConfigMsg msg) {
        if(mReadConfigCallback != null && mReturnCallbackLock.compareAndSet(false, true)){
            mReadConfigCallback.onReadConfigResult(mac, result, msg);
        }
    }
}
