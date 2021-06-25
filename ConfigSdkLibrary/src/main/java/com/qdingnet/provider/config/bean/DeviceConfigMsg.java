package com.qdingnet.provider.config.bean;

import java.util.List;

/**
 * 设备配置信息
 * @author pipan (pipan@qding.me)
 * @date   2019/5/28
 */
public class DeviceConfigMsg {
    //设备开门时长(毫秒）
    public int openKeepTime;
    //设备当前时间(秒）
    public long deviceTime;
    //设备软件版本
    public String softwareVersion;
    //设备硬件版本
    public String hardwareVersion;
    //配置的楼栋单元数据
    public List<GateBuildUnitInfo> configUnitInfoList;

    public DeviceConfigMsg(int openKeepTime, long deviceTime, String softwareVersion, String hardwareVersion, List<GateBuildUnitInfo> configUnitInfoList) {
        this.openKeepTime = openKeepTime;
        this.deviceTime = deviceTime;
        this.softwareVersion = softwareVersion;
        this.hardwareVersion = hardwareVersion;
        this.configUnitInfoList = configUnitInfoList;
    }

}
