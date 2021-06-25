package com.qdingnet.provider.config.handler;

import android.util.Log;

import com.qdingnet.opendoor.core.handler.IAuthHandler;
import com.qdingnet.provider.config.bean.QDevice;
import com.qdingnet.provider.config.bean.StateGuard;

import java.util.List;

/**
 * 千丁智能门禁验证授权处理器
 * @author pipan (pipan@qding.me)
 * @date   19-1-23
 */

public class QdingAuthHandler implements IAuthHandler {
    private static final String TAG = "QdingAuthHandler";

    //授权执行操作的门禁设备列表
    private List<QDevice> mDeviceList;
    //设备状态机管理器
    private StateGuard mStateGuard;

    public QdingAuthHandler(List<QDevice> deviceList, StateGuard stateGuard) {
        this.mDeviceList = deviceList;
        this.mStateGuard = stateGuard;
    }

    @Override
    public boolean onAuth(String ssid, int rssi) {
        //检验mac值
        String mac = QdingDataPacketHandler.getDeviceMac(ssid);
        Log.i(TAG, "onAuth mac = " + mac + "deviceList = " + mDeviceList.toString());
        if(mac == null) return false;
        //设置扫描到千丁设备
        if(mStateGuard != null) mStateGuard.setStateScanFindDevice();
        if(mDeviceList == null) return false;
        //检验是否有可操作的设备
        int index = mDeviceList.indexOf(new QDevice(mac));
        if(index == -1) return false;
        //设置扫描到千丁且可操作的设备
        if(mStateGuard != null) mStateGuard.setStateScanFindOperableDevice();
        QDevice device = mDeviceList.get(index);
        boolean authed = rssi >= device.rssi;
        //设置扫描到千丁可开门的设备
        if(authed && mStateGuard != null) mStateGuard.setStateScanFindOpenDoorDevice();
        Log.i(TAG, "onAuth authed = "+authed);
        return authed;
    }

}
