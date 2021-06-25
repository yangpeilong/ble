package com.qdingnet.provider.config.handler;

import com.qdingnet.opendoor.core.bean.DataPacket;
import com.qdingnet.provider.config.Logdeal;
import com.qdingnet.provider.config.bean.Contants;
import com.qdingnet.provider.config.bean.QDAccessResult;
import com.qdingnet.provider.config.bean.StateGuard;
import com.qdingnet.provider.config.bundle.WriteConfigBundle;
import com.qdingnet.provider.config.callback.IWriteConfigCallback;

/**
 * 设备写配置处理器
 * @author pipan (pipan@qding.me)
 * @date   2019/5/16
 */
public class WriteConfigHandler extends QdingDataHandler {
    private static final String TAG = "WriteConfigHandler";

    //写配置信息
    private final WriteConfigBundle mBundle;
    //设备状态机管理器
    private StateGuard mStateGuard;
    //写配置结果回调
    private final IWriteConfigCallback mWriteConfigCallback;

    public WriteConfigHandler(WriteConfigBundle bundle, StateGuard stateGuard, IWriteConfigCallback callback) {
        super(true);
        this.mBundle = bundle;
        this.mStateGuard = stateGuard;
        this.mWriteConfigCallback = callback;
    }

    @Override
    DataPacket createSendPacketInfo(String ssid) {
        DataPacket info = null;
        //获取版本
        Contants.DeviceEdition edition = Contants.DeviceEdition.getEditionFromSSID(ssid);
        if(edition != null) {
            int frameDelay = 0;
            String mac = QdingDataPacketHandler.getDeviceMac(ssid);
            int cmd = Contants.QDCmdType.QD_CMD_CONFIG_DEVICE;
            //一代设备
            if (Contants.DeviceEdition.V1 == edition) {
                if(!mStateGuard.isReadableOfWriteBluetoothGattCharacteristic()){
                    //一代设备固定50ms发送间隔时间
                    frameDelay = 50;
                }else {
                    //一代设备已升级到二代设备
                    edition = Contants.DeviceEdition.V2;
                }
            }
            //二代设备
            else if (Contants.DeviceEdition.V2 == edition) {
                //no-op
            }
            //四代设备
            else if (Contants.DeviceEdition.V4 == edition) {
                cmd = Contants.QDCmdType.QD_CMD_CONFIG_DEVICE_AGCY;
            }
            byte[] data = QdingDataPacketHandler.buildDeviceConfigMsg(edition, mBundle.account, mac, mBundle.device.openKeepTime, mBundle.buildUnitList, mBundle.relayOn, mBundle.device.deviceAuthCode);
            if(data != null && data.length > 0){
                info = new DataPacket(cmd, data, frameDelay);
            }
        }
        return info;
    }

    @Override
    protected void handleWriteConfigAck(String ssid, int resultCode) {
        Logdeal.D(TAG, "handleWriteConfigAck...resultCode:" + resultCode);
        QDAccessResult result = resultCode == Contants.OpenDoorResultType.OPEN_DOOR_SUCCESS ? QDAccessResult.OK : QDAccessResult.ERROR_CONFIG_FAILURE;
        String mac = QdingDataPacketHandler.getDeviceMac(ssid);
        //回调上层
        if(mWriteConfigCallback != null){
            mWriteConfigCallback.onWriteConfigResult(mac, result);
        }
        //设置开门操作完成标志
        setStateFlag(FLAG_ALL_FINISH);
        //关闭设备
        deviceClose();
    }
}
