package com.qdingnet.provider.config.handler;

import com.qdingnet.opendoor.core.bean.DataPacket;
import com.qdingnet.provider.config.Logdeal;
import com.qdingnet.provider.config.bean.Contants;
import com.qdingnet.provider.config.bean.QDAccessResult;
import com.qdingnet.provider.config.bean.QDevice;
import com.qdingnet.provider.config.callback.internal.IOpenDoorHandler;
import com.qdingnet.qdaccess.QDPassRecordEntity;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;


/**
 * 千丁开门数据处理器
 * @author pipan (pipan@qding.me)
 * @date   19-1-24
 */
public class OpendoorHandler extends QdingDataHandler {
    private static final String TAG = "OpendoorHandler";
    //开门方式类型（蓝牙）
    public static final int MODE_BLE = 1;
    //开门方式类型（WIFI）
    public static final int MODE_WIFI = 2;

    //开门方式
    private int mMode;
    //用户数据
    private long mUserId;
    //执行操作的门禁设备列表
    private List<QDevice> mDeviceList;
    //上层结果回调
    private IOpenDoorHandler mOpenDoorHandler;
    //开门标志id
    private int mPassId;

    public OpendoorHandler(int mode, long userId, int passId, List<QDevice> deviceList, IOpenDoorHandler handler) {
        this.mMode = mode;
        this.mUserId = userId;
        this.mPassId = passId;
        this.mDeviceList = deviceList;
        this.mOpenDoorHandler = handler;
    }

    @Override
    DataPacket createSendPacketInfo(String ssid) {
        DataPacket info = null;
        //获取版本
        Contants.DeviceEdition edition = Contants.DeviceEdition.getEditionFromSSID(ssid);
        if(edition != null){
            int frameDelay = 0;
            String mac = QdingDataPacketHandler.getDeviceMac(ssid);
            //一代设备
            if (Contants.DeviceEdition.V1 == edition) {
                //一代设备固定50ms发送间隔时间
                frameDelay = 50;
                //passId置为0
                mPassId = 0;
            }
            //二/五代设备的在线卡数据
            else if (Contants.DeviceEdition.V2 == edition || Contants.DeviceEdition.V5 == edition) {
                mPassId = 0;
            }
            //四代设备
            else if (Contants.DeviceEdition.V4 == edition) {
                //no-op
            }
            //获取开门时长
            int index = mDeviceList != null ? mDeviceList.indexOf(new QDevice(mac)) : -1;
            QDevice deivce = index != -1 ? mDeviceList.get(index) : null;
            int openKeepTime = deivce != null ? deivce.openKeepTime : 0;
            //获取设备识别码
            String deviceAuthCode = deivce != null ? deivce.deviceAuthCode : null;
            //构建开门数据
            byte[] data = QdingDataPacketHandler.getOpendoorDataPacket(edition, mac, mPassId, mUserId, openKeepTime, null, deviceAuthCode);
            if(data != null && data.length > 0){
                Logdeal.D(TAG, "handleOpenDoorAck...result:" + Arrays.toString(data));
                info = new DataPacket(Contants.QDCmdType.QD_CMD_OPEN_DOOR, data, frameDelay);
            }
        }
        return info;
    }

    @Override
    public void handleSendDataFinish(String ssid, DataPacket packetInfo) {
        super.handleSendDataFinish(ssid, packetInfo);
    }

    @Override
    protected void handleOpenDoorAck(String ssid, int resultCode) {
        Logdeal.D(TAG, "handleOpenDoorAck...result:" + resultCode);
        QDAccessResult result;
        switch (resultCode){
            case Contants.OpenDoorResultType.OPEN_DOOR_SUCCESS:
            case Contants.OpenDoorResultType.OPEN_DOOR_FREQUENT:
                 result = QDAccessResult.OK;
                 break;
            case Contants.OpenDoorResultType.OPEN_DOOR_AGCY_ERROR:
                result = QDAccessResult.ERROR_GOPEN_LOCK_AGCY_FAILED;
                break;
            default:
                result = QDAccessResult.ERROR_DEVICE_ACK_ERROR;
                break;
        }
        String mac = QdingDataPacketHandler.getDeviceMac(ssid);
        response(mac, result);
        //获取版本
        Contants.DeviceEdition edition = Contants.DeviceEdition.getEditionFromSSID(ssid);
        //四/五代设备直接关闭设备
        if (Contants.DeviceEdition.V4 == edition || Contants.DeviceEdition.V5 == edition) {
            //设置开门操作完成标志
            setStateFlag(FLAG_ALL_FINISH);
            //关闭设备
            deviceClose();
        }
    }

    @Override
    protected void handlePassRecordsAck(String ssid, ArrayList<QDPassRecordEntity> passRecordEntities) {
        Logdeal.D(TAG, "handlePassRecordsAck...");
        //设置开门操作完成标志
        setStateFlag(FLAG_ALL_FINISH);
        //关闭设备
        deviceClose();
        //wifi方式下回调上册开门结果
        String mac = QdingDataPacketHandler.getDeviceMac(ssid);
        if(mMode == MODE_WIFI){
            response(mac, QDAccessResult.OK);
        }
        //回调上层开门记录
        if (mOpenDoorHandler != null) {
            mOpenDoorHandler.onOpenDoorRecords(mac, passRecordEntities);
        }
    }

    /**
     * 开门结果回调上层
     * @param mac
     * @param result
     */
    void response(String mac, QDAccessResult result){
        if (mOpenDoorHandler != null) {
            mOpenDoorHandler.onOpenDoorResult(mac, mPassId, result);
        }
    }
}
