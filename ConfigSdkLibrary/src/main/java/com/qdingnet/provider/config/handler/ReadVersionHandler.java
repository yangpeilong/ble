package com.qdingnet.provider.config.handler;

import com.qdingnet.opendoor.core.bean.DataPacket;
import com.qdingnet.provider.config.Logdeal;
import com.qdingnet.provider.config.bean.Contants;
import com.qdingnet.provider.config.bean.QDAccessResult;
import com.qdingnet.provider.config.bundle.ReadVersionBundle;
import com.qdingnet.provider.config.callback.IReadVersionCallback;

/**
 * 读取设备版本处理器
 * @author pipan (pipan@qding.me)
 * @date   2019/5/28
 */
public class ReadVersionHandler extends QdingDataHandler {
    private static final String TAG = "ReadVersionHandler";

    //读设备版本信息
    private final ReadVersionBundle mBundle;
    //读取设备版本回调接口
    private final IReadVersionCallback mReadVersionCallback;

    public ReadVersionHandler(ReadVersionBundle mBundle, IReadVersionCallback callback) {
        super(true);
        this.mBundle = mBundle;
        this.mReadVersionCallback = callback;
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
            byte[] data = QdingDataPacketHandler.buildGetVersionMsg(edition, mac);
            if(data != null && data.length > 0){
                info = new DataPacket(Contants.QDCmdType.QD_CMD_GET_VERSION, data, frameDelay);
            }
        }
        return info;
    }

    @Override
    protected void handleGetVersionAck(String ssid, String softwareVersion, String hardwareVersion) {
        Logdeal.D(TAG, "handleGetVersionAck...");
        if(mReadVersionCallback != null){
            String mac = QdingDataPacketHandler.getDeviceMac(ssid);
            QDAccessResult result = softwareVersion != null ? QDAccessResult.OK : QDAccessResult.ERROR_READ_VERSION_FAILURE;
            mReadVersionCallback.onReadVersionResult(mac, result, softwareVersion, hardwareVersion);
        }
        //设置开门操作完成标志
        setStateFlag(FLAG_ALL_FINISH);
        //关闭设备
        deviceClose();
    }

}
