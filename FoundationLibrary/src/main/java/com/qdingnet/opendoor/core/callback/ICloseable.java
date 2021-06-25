package com.qdingnet.opendoor.core.callback;

import com.qdingnet.opendoor.core.handler.IClient;

/**
 * 设备关闭接口
 * @author pipan (pipan@qding.me)
 * @date   2019/5/30
 */
public interface ICloseable {
    /**
     * 设备关闭
     */
    void close();

    interface Factory{
        /**
         * 创建ICloseable
         * @return
         */
        ICloseable create();

        /**
         * 设置实体设备客户端
         * @param client
         */
        void setIClient(IClient client);

        /**
         * 注册设备关闭操作回调
         * @param callback
         */
        void registerCloseCallback(ICloseCallback callback);
    }
}
