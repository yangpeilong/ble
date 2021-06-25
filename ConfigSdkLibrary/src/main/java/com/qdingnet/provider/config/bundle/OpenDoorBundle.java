package com.qdingnet.provider.config.bundle;

import android.util.Log;

import com.qdingnet.provider.config.bean.QDevice;

import java.util.ArrayList;
import java.util.List;

/**
 * 开门操作的数据集
 * @author pipan (pipan@qding.me)
 * @date   19-2-14
 */
public class OpenDoorBundle {
    //用户id
    private final long mAppUserId;
    //是否开通广播开门
    private final boolean mSupportAdvertise;
    //开门设备信息集
    private final List<QDevice> mDeviceBundleList;

    OpenDoorBundle(long appUserId, boolean supportAdvertise, List<QDevice> deviceBundles){
        this.mAppUserId = appUserId;
        this.mSupportAdvertise = supportAdvertise;
        this.mDeviceBundleList = deviceBundles;
    }

    /**
     * 获取用户id
     * @return
     */
    public long getAppUserId() {
        return mAppUserId;
    }

    /**
     * 是否支持广播
     * @return
     */
    public boolean isSupportAdvertise() {
        return mSupportAdvertise;
    }

    /**
     * 获取可开门设备列表
     * @return
     */
    public List<QDevice> getOpenDoorDeviceInfo(){
        return mDeviceBundleList;
    }

    @Override
    public String toString() {
        return "OpenDoorBundle{" +
                "mAppUserId=" + mAppUserId +
                ", mDeviceBundleList=" + mDeviceBundleList +
                '}';
    }

    /**
     * 开门操作的数据集构建器
     */
    public static class OpenDoorBundleBuilder{
        //默认开门最小信号值
        private static final int DEFAULT_RSSI = -90;
        //默认开门时长
        private static final int DEFAULT_OPEN_KEEP_TIME = 5_000;
        //用户id
        private long mAppUserId;
        //是否开通广播开门(默认开启)
        private boolean mSupportAdvertise = true;
        //开门设备信息集
        private final List<QDevice> mDeviceList = new ArrayList<>();

        /**
         * 设置用户id
         * @param app_user_id
         * @return
         */
        public OpenDoorBundleBuilder setAppUserId(long app_user_id){
            this.mAppUserId = app_user_id;
          Log.i("OpenDoorBundleBuilder", "setAppUserId ="+mAppUserId);
            return this;
        }

        /**
         * 设置是否广播开门
         * @param supportAdvertise
         * @return
         */
        public OpenDoorBundleBuilder setSupportAdvertise(boolean supportAdvertise){
            this.mSupportAdvertise = supportAdvertise;
            return this;
        }

        /**
         * 添加可开门设备mac及自定义最小信号值,开门时长
         * @param mac
         * @param minRssi
         * @param openDoorKeepTime
         * @return
         */
        public OpenDoorBundleBuilder addDeviceBundle(String mac, int minRssi, int openDoorKeepTime){
            return addDeviceBundle(mac, minRssi, openDoorKeepTime, null);
        }

        /**
         * 添加可开门设备mac及自定义最小信号值,开门时长,设备识别码
         * @param mac
         * @param minRssi
         * @param openDoorKeepTime
         * @param deviceAuthCode
         * @return
         */
        public OpenDoorBundleBuilder addDeviceBundle(String mac, int minRssi, int openDoorKeepTime, String deviceAuthCode){
            mDeviceList.add(new QDevice(mac, minRssi, openDoorKeepTime, deviceAuthCode));
            return this;
        }

        /**
         * 添加可开门设备mac及默认最小信号值
         * @param mac
         * @return
         */
        public OpenDoorBundleBuilder addDeviceBundle(String mac){
            return addDeviceBundle(mac, DEFAULT_RSSI, DEFAULT_OPEN_KEEP_TIME);
        }

        /**
         * 添加可开门设备mac及默认最小信号值,默认开门时长,设备识别码
         * @param mac
         * @return
         */
        public OpenDoorBundleBuilder addDeviceBundle(String mac, String deviceAuthCode){
            return addDeviceBundle(mac, DEFAULT_RSSI, DEFAULT_OPEN_KEEP_TIME, deviceAuthCode);
        }

        public OpenDoorBundle build(){
            if(mAppUserId == 0){
                throw new NullPointerException("No Set param <mAppUserId>");
            }
            if(mDeviceList.isEmpty()){
                throw new NullPointerException("No Add Device Bundle");
            }
            return new OpenDoorBundle(mAppUserId, mSupportAdvertise, mDeviceList);
        }
    }
}
