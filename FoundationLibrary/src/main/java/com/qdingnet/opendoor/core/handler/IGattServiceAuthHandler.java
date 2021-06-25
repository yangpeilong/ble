package com.qdingnet.opendoor.core.handler;

import android.bluetooth.BluetoothGatt;

/**
 * 门禁设备验证授权接口
 * @author pipan (pipan@qding.me)
 * @date   19-1-23
 */
public interface IGattServiceAuthHandler {
    /**
     * 门禁设备发现的服务验证
     * @return
     */
    boolean onGattServiceAuth(BluetoothGatt gatt);
}
