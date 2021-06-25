package com.qdingnet.provider.config.callback.impl;

import android.os.Build;

import com.qdingnet.opendoor.core.callback.ICloseable;
import com.qdingnet.opendoor.core.misc.MessageHandler;
import com.qdingnet.provider.config.ConfigManager;
import com.qdingnet.provider.config.bean.QDAccessResult;
import com.qdingnet.provider.config.bean.StateGuard;
import com.qdingnet.provider.config.bundle.DeviceUpgradeBundle;
import com.qdingnet.provider.config.bundle.ReadVersionBundle;
import com.qdingnet.provider.config.callback.IDeviceUpgradeCallback;
import com.qdingnet.provider.config.callback.IReadVersionCallback;
import com.qdingnet.provider.config.callback.internal.ISocketHandler;
import com.qdingnet.provider.config.handler.QdingDataPacketHandler;

import java.util.concurrent.atomic.AtomicBoolean;

/**
 * 设备升级回调适配器
 * @author pipan (pipan@qding.me)
 * @date   2019/5/28
 */
public class DeviceUpgradeCallbackAdapter extends AbstractStateCallback implements ISocketHandler, IReadVersionCallback, IDeviceUpgradeCallback {
    private static final String TAG = "DeviceUpgradeCallbackAdapter";

    //轮询读取设备版本信息间隔时长
    private static final int READ_VERSION_ROUND_ROBIN_SLOT = 10_000;

    //设备配置管理类
    private final ConfigManager mConfigManager;
    //设备升级参数集
    private final DeviceUpgradeBundle mBundle;
    //回调接口
    private final IDeviceUpgradeCallback mDeviceUpgradeCallback;
    //回调上层锁
    private final AtomicBoolean mReturnCallbackLock = new AtomicBoolean(false);
    //轮询读取设备版本信息消息处理器
    private final MessageHandler mHandler = new MessageHandler(TAG);
    //开始读取设备版本信息开始时间
    private long mStartReadVersionTime = 0;
    //设备关闭操作工厂接口
    private ICloseable.Factory mICloseableFactory;

    public DeviceUpgradeCallbackAdapter(ConfigManager manager, StateGuard stateGuard, DeviceUpgradeBundle bundle, IDeviceUpgradeCallback callback, ICloseable.Factory factory) {
        super(MODE_WIFI, stateGuard);
        this.mBundle = bundle;
        this.mConfigManager = manager;
        this.mDeviceUpgradeCallback = callback;
        this.mICloseableFactory = factory;
    }

    @Override
    protected void onStateSuccess() {}

    @Override
    protected void onStateFailure(String ssid, QDAccessResult result) {
        onDeviceUpgradeResult(QdingDataPacketHandler.getDeviceMac(ssid), result);
    }

    @Override
    public void onSocketResult(String ssid, QDAccessResult result) {
        String mac = QdingDataPacketHandler.getDeviceMac(ssid);
        //升级指令发送成功或者读取返回结果超时
        if(result.isSuccess() || result == QDAccessResult.ERROR_READ_WIFI_DATA_EXCEPTION){
            //准备轮询扫描读取设备版本
            mStartReadVersionTime = System.currentTimeMillis();
            roundRobinReadVersion(mac);
        }
        //升级指令发送失败
        else {
            onDeviceUpgradeResult(mac, result);
        }
    }

    /**
     * 轮询读取版本信息
     * @param mac
     */
    private void roundRobinReadVersion(String mac){
        QDAccessResult result = null;
        if(Build.VERSION.SDK_INT > 17){
            long costTime = mStartReadVersionTime + mBundle.upgradeTimeout - System.currentTimeMillis();
            if(costTime >= 0) {
                mHandler.execute(new Runnable() {
                    @Override
                    public void run() {
                        ReadVersionBundle bundle = new ReadVersionBundle.Builder().setDevice(mBundle.device.mac).build();
                        mConfigManager.readVersion(bundle, DeviceUpgradeCallbackAdapter.this);
                    }
                }, READ_VERSION_ROUND_ROBIN_SLOT);
            }else {
                result = QDAccessResult.ERROR_UPDATE_DEVICE_FAILURE;
            }
        }else {
            result = QDAccessResult.ERROR_NOT_SUPPORT_BLE;
        }
        //回调上层
        if(result != null){
            onDeviceUpgradeResult(mac, result);
        }
    }

    @Override
    public void onReadVersionResult(String mac, QDAccessResult result, String softwareVersion, String hardwareVersion) {
        if(result.isSuccess() && mBundle.firmwareVersion.equals(softwareVersion)){
            onDeviceUpgradeResult(mac, result);
        }else {
            roundRobinReadVersion(mac);
        }
    }

    @Override
    public void onDeviceUpgradeResult(String mac, QDAccessResult result) {
        if(mReturnCallbackLock.compareAndSet(false, true)){
            //回调上层
            if(mDeviceUpgradeCallback != null){
                mDeviceUpgradeCallback.onDeviceUpgradeResult(mac, result);
            }
            //清除消息队列
            mHandler.cancel();
            //关闭设备
            if(mICloseableFactory != null){
                mICloseableFactory.create().close();
                mICloseableFactory = null;
            }
        }
    }

}
