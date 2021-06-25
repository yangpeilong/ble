package com.qdingnet.provider.config.bean;

/**
 * 用于设备操作（请求->响应）整个闭环的状态守护
 * @author pipan (pipan@qding.me)
 * @date   19-2-28
 */
public class StateGuard {
    //扫描初始状态
    private static final int STATE_SCAN_INIT = 0;
    //扫描到千丁设备状态
    private static final int STATE_SCAN_FIND_DEVICE = 0x01;
    //扫描到千丁且可操作的设备状态
    private static final int STATE_SCAN_FIND_OPERABLE_DEVICE = 0x02;
    //扫描到千丁设备且可开门的状态
    private static final int STATE_SCAN_FIND_OPEN_DOOR_DEVICE = 0x04;
    //扫描设备的具体状态
    private int mState = STATE_SCAN_INIT;
    //获取设备蓝牙写通道是否具有读权限
    private boolean isReadableOfWriteBluetoothGattCharacteristic;

    /**
     * 设置扫描到千丁设备状态
     */
    public void setStateScanFindDevice(){
        mState |= STATE_SCAN_FIND_DEVICE;
    }

    /**
     * 是否扫描到千丁设备状态
     * @return
     */
    public boolean isStateScanFindDevice(){
        return (mState & STATE_SCAN_FIND_DEVICE) > 0;
    }

    /**
     * 设置扫描到千丁且可操作的设备状态
     */
    public void setStateScanFindOperableDevice(){
        mState |= STATE_SCAN_FIND_OPERABLE_DEVICE;
    }

    /**
     * 是否扫描到千丁且可操作的设备状态
     * @return
     */
    public boolean isStateScanFindOperableDevice(){
        return (mState & STATE_SCAN_FIND_OPERABLE_DEVICE) > 0;
    }

    /**
     * 设置扫描到千丁且可开门的设备状态
     */
    public void setStateScanFindOpenDoorDevice(){
        mState |= STATE_SCAN_FIND_OPEN_DOOR_DEVICE;
    }

    /**
     * 是否扫描到千丁且可开门的设备状态
     * @return
     */
    public boolean isStateScanFindOpenDoorDevice(){
        return (mState & STATE_SCAN_FIND_OPEN_DOOR_DEVICE) > 0;
    }

    /**
     * 是否有读权限
     * @return
     */
    public boolean isReadableOfWriteBluetoothGattCharacteristic() {
        return isReadableOfWriteBluetoothGattCharacteristic;
    }

    /**
     * 设置是否有读权限
     * @param readableOfWriteBluetoothGattCharacteristic
     */
    public void setReadableOfWriteBluetoothGattCharacteristic(boolean readableOfWriteBluetoothGattCharacteristic) {
        isReadableOfWriteBluetoothGattCharacteristic = readableOfWriteBluetoothGattCharacteristic;
    }
}
