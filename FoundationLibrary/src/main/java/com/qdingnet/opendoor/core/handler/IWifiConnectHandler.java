package com.qdingnet.opendoor.core.handler;

/**
 * wifi连接处理接口
 * @author pipan (pipan@qding.me)
 * @date   19-2-16
 */
public interface IWifiConnectHandler {
    void onConnected(String ssid, String ip);
}
