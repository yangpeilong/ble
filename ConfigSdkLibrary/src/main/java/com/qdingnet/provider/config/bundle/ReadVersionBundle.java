package com.qdingnet.provider.config.bundle;

import com.qdingnet.provider.config.ConfigManager;
import com.qdingnet.provider.config.bean.QDevice;

/**
 * 读取设备版本数据集
 * @author pipan (pipan@qding.me)
 * @date   2019/5/28
 */
public class ReadVersionBundle {
    //读取配置的设备信息
    public QDevice device;

    ReadVersionBundle(QDevice device) {
        this.device = device;
    }

    @Override
    public String toString() {
        return "ReadVersionBundle{" +
                "device=" + device +
                '}';
    }

    /**
     * 读取设备版本信息操作的数据集构建器
     */
    public final static class Builder{
        //设备信息
        private QDevice device;

        /**
         * 设置设备信息
         * @param mac
         * @return
         */
        public Builder setDevice(String mac) {
            this.device = new QDevice(mac, ConfigManager.DEFAULT_RSSI);
            return this;
        }

        public ReadVersionBundle build(){
            if(device == null){
                throw new NullPointerException("No Set param <device>");
            }
            return new ReadVersionBundle(device);
        }
    }
}
