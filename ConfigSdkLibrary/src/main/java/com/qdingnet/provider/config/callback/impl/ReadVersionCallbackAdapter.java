package com.qdingnet.provider.config.callback.impl;

import com.qdingnet.provider.config.bean.QDAccessResult;
import com.qdingnet.provider.config.bean.StateGuard;
import com.qdingnet.provider.config.callback.IReadVersionCallback;
import com.qdingnet.provider.config.handler.QdingDataPacketHandler;

import java.util.concurrent.atomic.AtomicBoolean;

/**
 * 读取设备版本信息回调适配器
 * @author pipan (pipan@qding.me)
 * @date   2019/5/28
 */
public class ReadVersionCallbackAdapter extends AbstractStateCallback implements IReadVersionCallback {
    private static final String TAG = "ReadVersionCallbackAdapter";
    //写配置结果回调
    private final IReadVersionCallback mReadVersionCallback;
    //回调上层锁
    private final AtomicBoolean mReturnCallbackLock = new AtomicBoolean(false);

    public ReadVersionCallbackAdapter(StateGuard stateGuard, IReadVersionCallback callback) {
        super(MODE_BLE, stateGuard);
        this.mReadVersionCallback = callback;
    }

    @Override
    protected void onStateSuccess() {}

    @Override
    protected void onStateFailure(String ssid, QDAccessResult result) {
        onReadVersionResult(QdingDataPacketHandler.getDeviceMac(ssid), result, "", "");
    }

    @Override
    public void onReadVersionResult(String mac, QDAccessResult result, String softwareVersion, String hardwareVersion) {
        if(mReadVersionCallback != null && mReturnCallbackLock.compareAndSet(false, true)){
            mReadVersionCallback.onReadVersionResult(mac, result, softwareVersion, hardwareVersion);
        }
    }
}
