package com.qdingnet.opendoor.core.handler;

import com.qdingnet.opendoor.core.bean.DataPacket;
import com.qdingnet.opendoor.core.callback.ICloseable;

/**
 * 数据处理器
 * @author pipan (pipan@qding.me)
 * @date   19-1-23
 */
public interface IDataHandler{
    /**
     * 注册设备关闭回调接口工厂
     * @param factory
     */
    void registerCloseableFactory(ICloseable.Factory factory);
    /**
     * 开始发送数据
     * @param ssid
     */
    DataPacket handleStartSendData(String ssid);

    /**
     * 数据已经发送完成
     */
    void handleSendDataFinish(String ssid, DataPacket packetInfo);

    /**
     * 上一帧数据发送完成回调
     * @param ssid
     * @param status
     */
    void handleDateWrite(String ssid, int status);

    /**
     * 设备推送过来的数据回调
     * @param ssid
     * @param data
     */
    void handleDataChanged(String ssid, byte[] data);

    /**
     * 读取设备数据的回调
     * @param ssid
     * @param data
     * @param status
     */
    void handleDataRead(String ssid, byte[] data, int status);

    /**
     * 整个操作是否已完成
     * @return
     */
    boolean isSuccess();

    /**
     * 写数据是否已完成
     * @return
     */
    boolean writeFinish();

    /**
     * 读数据是否已完成
     * @return
     */
    boolean readFinish();

}
