package com.qdingnet.opendoor.core.handler;

import com.qdingnet.opendoor.core.bluetooth.BluetoothLeClass;

import java.util.Map;

/**
 * 广播收发数据接口
 * @author pipan (pipan@qding.me)
 * @date   19-1-23 
 */
public interface IAdvertiseHandler {
    //构造广播发送的数据
    Map<String, byte[]> buildAdvertiseData(BluetoothLeClass sender, String ssid);
    //发送广播成功后开始执行下一步骤(连接)的延时时长
    int processNextDelayTime(String ssid);
    //解析广播接收的数据
    boolean   analysisAdvertiseData(BluetoothLeClass receiver, String ssid, byte[] record);
}
