package com.qdingnet.provider.config.handler;

import android.os.ParcelUuid;
import android.util.Base64;

import com.qdingnet.opendoor.core.bluetooth.BluetoothLeClass;
import com.qdingnet.opendoor.core.bluetooth.scan.ScanRecord;
import com.qdingnet.opendoor.core.handler.IAdvertiseHandler;
import com.qdingnet.provider.config.Logdeal;
import com.qdingnet.provider.config.bean.Contants;
import com.qdingnet.provider.config.bean.QDAccessResult;
import com.qdingnet.provider.config.bean.QDevice;
import com.qdingnet.provider.config.callback.internal.IOpenDoorHandler;

import java.util.List;
import java.util.Map;
import java.util.UUID;

/**
 * 千丁智能门禁广播数据处理器
 * @author pipan (pipan@qding.me)
 * @date   19-1-23
 */
public class QdingAdvertiseHandler implements IAdvertiseHandler {
    private static final String TAG = "QdingAdvertiseHandler";
    //广播结果服务uuid
    public final static UUID UUID_ADVERTISER_RESULT_SERVICE = UUID.fromString("0000ffe8-0000-1000-8000-00805f9b34fb");
    //门禁设备广播成功后处理下一操作流程（连接）的延时时间
    private final static int PROCESS_NEXT_DELAY_TIME = 1500;

    //用户信息
    private long mUserId;
    //开门结果上层回调
    private IOpenDoorHandler mOpenDoorCallback;
    //保存当前发送广播给目标设备的mac
    private String mMac;
    //执行操作的门禁设备列表
    private List<QDevice> mDeviceList;
    //缓存当前开门标示id
    private int mPassId;

    public QdingAdvertiseHandler(long userId, int passId, List<QDevice> deviceList, IOpenDoorHandler callback) {
        this.mUserId = userId;
        this.mPassId = passId;
        this.mDeviceList = deviceList;
        this.mOpenDoorCallback = callback;
    }

    @Override
    public Map<String, byte[]> buildAdvertiseData(BluetoothLeClass sender, String ssid) {
        //解析mac值
        String mac = QdingDataPacketHandler.getDeviceMac(ssid);
        if(mac != null){
            mMac = mac;
            //获取版本
            Contants.DeviceEdition edition = Contants.DeviceEdition.getEditionFromSSID(ssid);
            //非四代设备则复位passId值
            if(edition != Contants.DeviceEdition.V4){
                mPassId = 0;
            }
            //获取设备识别码
            int index = mDeviceList != null ? mDeviceList.indexOf(new QDevice(mac)) : -1;
            QDevice deivce = index != -1 ? mDeviceList.get(index) : null;
            String deviceAuthCode = deivce != null ? deivce.deviceAuthCode : null;
            String name = QdingDataPacketHandler.getAdvertiseOpendoorDataPacket(edition, mac, mPassId, mUserId, deviceAuthCode);
            Logdeal.D(TAG, "buildAdvertiseData...name:" + name);
            sender.setBluetoothName(name);
        }
        return null;
    }

    @Override
    public int processNextDelayTime(String ssid) {
        //获取版本
        Contants.DeviceEdition edition = Contants.DeviceEdition.getEditionFromSSID(ssid);
        return edition == Contants.DeviceEdition.V4 ? 0 : PROCESS_NEXT_DELAY_TIME;
    }

    @Override
    public boolean analysisAdvertiseData(BluetoothLeClass receiver, String ssid, byte[] record) {
        //解析广播数据
        boolean result = false;
        String mac = QdingDataPacketHandler.getDeviceMac(ssid);
        //比较mac
        if(mac != null && mac.equals(mMac)){
            //解析广播数据
            result = analysisAdvertiseData(ssid, record);
            //开门成功处理
            if(result){
                //将蓝牙名称复原
                receiver.resetBluetoothName();
                //回调上传
                if(mOpenDoorCallback != null){
                    mOpenDoorCallback.onOpenDoorResult(mac, mPassId, QDAccessResult.OK);
                }
            }
        }
        Logdeal.D(TAG, String.format("analysisAdvertiseData...ssid:%s, result=%b", ssid, result));
        return result;
    }

    /**
     * 解析广播数据
     * @param ssid
     * @param record
     * @return
     */
    private boolean analysisAdvertiseData(String ssid, byte[] record){
        boolean result = false;
        Contants.DeviceEdition edition = Contants.DeviceEdition.getEditionFromSSID(ssid);
        byte[] data = null;
        long appUserId = 0;
        if(edition != null){
            switch (edition){
                case V1:
                case V2:
                case V3:
                case V5:
                    {
                        String baseMac = ssid.substring(3);
                        int length = baseMac.length();
                        if (length >= 12) {
                            data = baseMac.substring(8).getBytes();
                            appUserId = mUserId;
                        }
                    }
                    break;
                case V4:
                    {
                        ScanRecord scanRecord = ScanRecord.parseFromBytes(record);
                        if(scanRecord != null){
                            data = scanRecord.getServiceData(new ParcelUuid(UUID_ADVERTISER_RESULT_SERVICE));
                            appUserId = mUserId;
                        }
                    }
                    break;
            }
        }
        if(data != null){
            for (int i = 0, size = data.length / 4; i < size; i += 4) {
                byte[] byteRc = Base64.decode(data, Base64.NO_PADDING);
                if (byteRc.length == 3
                        && (appUserId & 0xff) == (byteRc[1] & 0xff)
                        && ((appUserId >>> 8) & 0xff) == (byteRc[2] & 0xff)) {
                    result = Contants.OpenDoorResultType.OPEN_DOOR_SUCCESS == byteRc[0] || Contants.OpenDoorResultType.OPEN_DOOR_FREQUENT == byteRc[0];
                    break;
                }
            }
        }
        return result;
    }
}
