package com.qdingnet.provider.config.callback.impl;

import com.qdingnet.provider.config.Logdeal;
import com.qdingnet.provider.config.bean.Contants;
import com.qdingnet.provider.config.bean.QDAccessResult;
import com.qdingnet.provider.config.bean.StateGuard;
import com.qdingnet.provider.config.callback.IOpenDoorCallback;
import com.qdingnet.provider.config.callback.internal.IOpenDoorHandler;
import com.qdingnet.provider.config.callback.internal.ISocketHandler;
import com.qdingnet.provider.config.handler.QdingDataPacketHandler;
import com.qdingnet.qdaccess.QDPassRecordEntity;

import java.util.List;
import java.util.Locale;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * 开门接口适配器
 * @author pipan (pipan@qding.me)
 * @date   19-1-29
 */
public class OpenDoorCallbackAdapter extends AbstractStateCallback implements IOpenDoorHandler, ISocketHandler {
    private static final String TAG = "OpenDoorCallbackAdapter";

    //用户信息
    private long mUserId;
    //开门标志id
    private int mPassId;
    //开门上层回调
    private IOpenDoorCallback mOpenDoorCallBack;
    //回调上层锁
    private AtomicBoolean mReturnCallbackLock = new AtomicBoolean(false);

    public OpenDoorCallbackAdapter(int mode, long userId, int passId, StateGuard stateGuard, IOpenDoorCallback callback) {
        super(mode, stateGuard);
        this.mUserId = userId;
        this.mPassId = passId;
        this.mOpenDoorCallBack = callback;
    }

    @Override
    public void onOpenDoorResult(String mac, int passId, QDAccessResult result) {
        if(mReturnCallbackLock.compareAndSet(false, true)){
            //回调上层
            if(mOpenDoorCallBack != null){
                mOpenDoorCallBack.onOpenDoorResult(mac, result);
            }
        }
    }

    @Override
    public void onOpenDoorRecords(String mac, List<QDPassRecordEntity> records) {
    }

    @Override
    protected void onStateSuccess() {}

    @Override
    protected void onStateFailure(String ssid, QDAccessResult result) {
        //失败回调上层
        String mac = QdingDataPacketHandler.getDeviceMac(ssid);
        int passId = 0;
        Contants.DeviceEdition edition = Contants.DeviceEdition.getEditionFromSSID(ssid);
        if(edition == Contants.DeviceEdition.V4){
            passId = mPassId;
        }
        onOpenDoorResult(mac, passId, result);
    }

    @Override
    public void onSocketResult(String ssid, QDAccessResult result) {
        Logdeal.D(TAG, String.format(Locale.US,"onSocketResult...ssid:%s, result:%s", ssid, result));
        String mac = QdingDataPacketHandler.getDeviceMac(ssid);
        onOpenDoorResult(mac, 0, result);
    }
}
