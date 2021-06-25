package com.qdingnet.provider.config.bundle;

import com.qdingnet.provider.config.ConfigManager;
import com.qdingnet.provider.config.bean.QDevice;

/**
 * 设备升级数据集
 * @author pipan (pipan@qding.me)
 * @date   2019/5/28
 */
public class DeviceUpgradeBundle {
    //需升级的设备信息
    public QDevice device;
    //需升级的固件路径
    public String  firmwarePath;
    //需升级的固件版本
    public String  firmwareVersion;
    //升级固件超时时长
    public int     upgradeTimeout;

    DeviceUpgradeBundle(QDevice device, String firmwarePath, String firmwareVersion, int upgradeTimeout) {
        this.device = device;
        this.firmwarePath = firmwarePath;
        this.firmwareVersion = firmwareVersion;
        this.upgradeTimeout = upgradeTimeout;
    }

    /**
     * 设备升级操作的数据集构建器
     */
    public static final class Builder{
        //需升级的设备信息
        private QDevice device;
        //需升级的固件路径
        private String  firmwarePath;
        //需升级的固件版本
        private String  firmwareVersion;
        //升级固件超时时长
        private int     upgradeTimeout;

        /**
         * 设置设备信息
         * @param mac
         * @return
         */
        public Builder setDevice(String mac) {
            this.device = new QDevice(mac, ConfigManager.DEFAULT_RSSI);
            return this;
        }

        /**
         * 设置固件路径
         * @param path
         * @return
         */
        public Builder setFirmwarePath(String path) {
            this.firmwarePath = path;
            return this;
        }

        /**
         * 设置固件需升级的版本
         * @param version
         * @return
         */
        public Builder setFirmwareVersion(String version) {
            this.firmwareVersion = version;
            return this;
        }

        /**
         * 设置固件需升级的超时时长
         * @param timeout
         * @return
         */
        public Builder setUpgradeTimeout(int timeout) {
            this.upgradeTimeout = timeout;
            return this;
        }

        public DeviceUpgradeBundle build(){
            if(device == null){
                throw new NullPointerException("No Set param <device>");
            }
            if(firmwarePath == null){
                throw new NullPointerException("No Set param <firmwarePath>");
            }
            if(firmwareVersion == null){
                throw new NullPointerException("No Set param <firmwareVersion>");
            }
            return new DeviceUpgradeBundle(device, firmwarePath, firmwareVersion, upgradeTimeout);
        }
    }
}
