package com.qdingnet.opendoor.core.handler;

/**
 * 门禁设备验证授权接口
 * @author pipan (pipan@qding.me)
 * @date   19-1-23
 */
public interface IAuthHandler {
    /**
     * 根据设备ssid和rssi验证结果
     * @param ssid -- 设备ssid
     * @param rssi -- 设备信号值
     * @return
     */
    boolean onAuth(String ssid, int rssi);

}
