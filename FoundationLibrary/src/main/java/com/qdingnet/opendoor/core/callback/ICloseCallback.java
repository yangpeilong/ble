package com.qdingnet.opendoor.core.callback;

/**
 * 设备关闭回调
 */
public interface ICloseCallback {
    /**
     * 关闭前操作
     */
    void onBeforeClose();
    /**
     * 关闭后操作
     */
    void onAfterClose();
}
