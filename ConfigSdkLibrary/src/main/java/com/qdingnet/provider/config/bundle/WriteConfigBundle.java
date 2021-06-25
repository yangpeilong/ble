package com.qdingnet.provider.config.bundle;

import com.qdingnet.provider.config.ConfigManager;
import com.qdingnet.provider.config.bean.GateBuildUnitInfo;
import com.qdingnet.provider.config.bean.QDevice;

import java.util.ArrayList;
import java.util.List;

/**
 * 配置门禁设备数据集
 * @author pipan (pipan@qding.me)
 * @date   2019/5/16
 */
public class WriteConfigBundle {
    //配置账号
    public String account;
    //配置的设备信息
    public QDevice device;
    //配置设备的继电器状态
    public boolean relayOn;
    //配置设备的配置单元信息
    public List<GateBuildUnitInfo> buildUnitList;


    WriteConfigBundle(String account, QDevice device, boolean relayOn, List<GateBuildUnitInfo> buildUnitList) {
        this.account = account;
        this.device = device;
        this.relayOn = relayOn;
        this.buildUnitList = buildUnitList;
    }

    @Override
    public String toString() {
        return "WriteConfigBundle{" +
                "account='" + account + '\'' +
                ", device=" + device +
                ", relayOn=" + relayOn +
                ", buildUnitList=" + buildUnitList +
                '}';
    }

    /**
     * 设备配置操作的数据集构建器
     */
    public final static class Builder{
        //配置账号
        private String account;
        //配置的设备信息
        private QDevice device;
        //配置设备的继电器状态
        private boolean relayOn;
        //配置设备的配置单元信息
        private List<GateBuildUnitInfo> buildUnitList = new ArrayList<>();

        /**
         * 设置配置账号
         * @param account
         * @return
         */
        public Builder setAccount(String account){
            this.account = account;
            return this;
        }

        /**
         * 设置配置设备信息
         * @param mac
         * @param openKeepTime
         * @param authcode
         * @return
         */
        public Builder setDevice(String mac, int openKeepTime, String authcode) {
            this.device = new QDevice(mac, ConfigManager.DEFAULT_RSSI, openKeepTime, authcode);
            return this;
        }

        /**
         * 设置继电器开闭状态
         * @param relayOn
         * @return
         */
        public Builder setRelayOn(boolean relayOn) {
            this.relayOn = relayOn;
            return this;
        }

        /**
         * 添加设备配置位置信息
         * @param info
         * @return
         */
        public Builder addBuildUnit(GateBuildUnitInfo info) {
            this.buildUnitList.add(info);
            return this;
        }

        public WriteConfigBundle build(){
            if(device == null){
                throw new NullPointerException("No Set param <device>");
            }
            return new WriteConfigBundle(account, device, relayOn, buildUnitList);
        }
    }
}
