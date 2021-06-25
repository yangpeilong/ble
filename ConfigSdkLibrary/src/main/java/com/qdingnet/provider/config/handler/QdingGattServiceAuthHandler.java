package com.qdingnet.provider.config.handler;

import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.util.Log;

import androidx.annotation.RequiresApi;

import com.qdingnet.opendoor.core.handler.IGattServiceAuthHandler;
import com.qdingnet.provider.config.bean.StateGuard;

import java.util.UUID;

/**
 * 千丁智能门禁验证授权处理器
 * @author pipan (pipan@qding.me)
 * @date   19-1-23
 */

public class QdingGattServiceAuthHandler implements IGattServiceAuthHandler {
    private static final String TAG = "QdingGattServiceAuthHandler";

    //相关服务/特征通道对应标示
    public  final static UUID UUID_UART_SERVICE = UUID.fromString("0000ffe0-0000-1000-8000-00805f9b34fb");
    private final static UUID UUID_NOTIFY_DATA = UUID.fromString("0000ffe4-0000-1000-8000-00805f9b34fb");
    private final static UUID UUID_WRITE_DATA = UUID.fromString("0000ffe9-0000-1000-8000-00805f9b34fb");
    private final static UUID UUID_WRITE_DISCONNECT_DATA = UUID.fromString("0000ff89-0000-1000-8000-00805f9b34fb");

    //设备状态机管理器
    private StateGuard mStateGuard;

    public QdingGattServiceAuthHandler(){}
    public QdingGattServiceAuthHandler(StateGuard guard){
        this.mStateGuard = guard;
    }

    @RequiresApi(18)
    @Override
    public boolean onGattServiceAuth(BluetoothGatt gatt) {
        BluetoothGattService gattService = gatt.getService(UUID_UART_SERVICE);
        if (gattService != null) {
            BluetoothGattCharacteristic writeCharacteristic = gattService.getCharacteristic(UUID_WRITE_DATA);
            BluetoothGattCharacteristic notifyCharacteristic = gattService.getCharacteristic(UUID_NOTIFY_DATA);
            if (writeCharacteristic != null && notifyCharacteristic != null) {
                //设置开启设备的推送通道
                gatt.setCharacteristicNotification(notifyCharacteristic, true);
                //设置写通道是否有读权限
                if(mStateGuard != null){
                    Log.i(TAG, "writeCharacteristic.getProperties:" + writeCharacteristic.getProperties());
                    mStateGuard.setReadableOfWriteBluetoothGattCharacteristic((writeCharacteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_READ) > 0);
                }
                return true;
            }
        }
        return false;
    }
}
