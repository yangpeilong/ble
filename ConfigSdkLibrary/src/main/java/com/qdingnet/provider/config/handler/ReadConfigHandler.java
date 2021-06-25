package com.qdingnet.provider.config.handler;

import com.qdingnet.opendoor.core.bean.DataPacket;
import com.qdingnet.provider.config.Logdeal;
import com.qdingnet.provider.config.bean.Contants;
import com.qdingnet.provider.config.bean.DeviceConfigMsg;
import com.qdingnet.provider.config.bean.QDAccessResult;
import com.qdingnet.provider.config.bundle.ReadConfigBundle;
import com.qdingnet.provider.config.callback.IReadConfigCallback;
import com.qdingnet.qdaccess.QDDeviceConfigEntity;

/**
 * 读取设备配置处理器
 * @author pipan (pipan@qding.me)
 * @date   2019/5/28
 */
public class ReadConfigHandler extends QdingDataHandler {
    private static final String TAG = "ReadConfigHandler";

    //读设备配置信息
    private final ReadConfigBundle mBundle;
    //读取设备回调接口
    private final IReadConfigCallback mReadConfigCallback;

    public ReadConfigHandler(ReadConfigBundle mBundle, IReadConfigCallback callback) {
        super(true);
        this.mBundle = mBundle;
        this.mReadConfigCallback = callback;
    }

    @Override
    DataPacket createSendPacketInfo(String ssid) {
        DataPacket info = null;
        //获取版本
        Contants.DeviceEdition edition = Contants.DeviceEdition.getEditionFromSSID(ssid);
        if(edition != null) {
            int frameDelay = 0;
            String mac = QdingDataPacketHandler.getDeviceMac(ssid);
            //一代设备
            if (Contants.DeviceEdition.V1 == edition) {
                //一代设备固定50ms发送间隔时间
                frameDelay = 50;
            }
            byte[] data = QdingDataPacketHandler.buildGetConfigMsg(edition, mac);
            if(data != null && data.length > 0){
                info = new DataPacket(Contants.QDCmdType.QD_CMD_GET_CONFIG, data, frameDelay);
            }
        }
        return info;
    }

    @Override
    protected void handleReadConfigAck(String ssid, QDDeviceConfigEntity entity) {
        Logdeal.D(TAG, "handleReadConfigAck...");
        //回调上层
        if(mReadConfigCallback != null){
            String mac = QdingDataPacketHandler.getDeviceMac(ssid);
            Contants.DeviceEdition edition = Contants.DeviceEdition.getEditionFromSSID(ssid);
            DeviceConfigMsg msg = QdingDataPacketHandler.parseDeviceConfig(edition, entity);
            QDAccessResult result = msg != null ? QDAccessResult.OK : QDAccessResult.ERROR_READ_CONFIG_FAILURE;
            mReadConfigCallback.onReadConfigResult(mac, result, msg);
        }
        //设置开门操作完成标志
        setStateFlag(FLAG_ALL_FINISH);
        //关闭设备
        deviceClose();
    }
}
