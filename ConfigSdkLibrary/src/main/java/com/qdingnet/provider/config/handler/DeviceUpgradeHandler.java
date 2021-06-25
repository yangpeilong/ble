package com.qdingnet.provider.config.handler;

import android.content.Context;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;

import com.qdingnet.opendoor.core.bean.DataPacket;
import com.qdingnet.provider.config.Logdeal;
import com.qdingnet.provider.config.bean.Contants;
import com.qdingnet.provider.config.bean.QDAccessResult;
import com.qdingnet.provider.config.bundle.DeviceUpgradeBundle;
import com.qdingnet.provider.config.callback.internal.ISocketHandler;

/**
 * 设备升级处理器
 * @author pipan (pipan@qding.me)
 * @date   2019/5/28
 */
public class DeviceUpgradeHandler extends QdingDataHandler{
    private static final String TAG = "DeviceUpgradeHandler";

    //应用上下文
    private Context mContext;
    //设备升级信息
    private final DeviceUpgradeBundle mBundle;
    //升级内部回调接口
    private final ISocketHandler mSocketCallback;

    public DeviceUpgradeHandler(Context context, DeviceUpgradeBundle bundle, ISocketHandler callback) {
        super(true);
        this.mContext = context;
        this.mBundle = bundle;
        this.mSocketCallback = callback;
    }

    @Override
    DataPacket createSendPacketInfo(String ssid) {
        DataPacket info = null;
        //获取版本
        Contants.DeviceEdition edition = Contants.DeviceEdition.getEditionFromSSID(ssid);
        if(edition != null) {
            String mac = QdingDataPacketHandler.getDeviceMac(ssid);
            String path = String.format("http://%s:%d%s", getIpAddress(), Contants.QDWifi.LOCAL_FIRMWARE_DOWNLOAD_SERVER_PORT, mBundle.firmwarePath);
            byte[] data = QdingDataPacketHandler.buildGetUpgradeMsg(edition, mac, path);
            if(data != null && data.length > 0){
                info = new DataPacket(Contants.QDCmdType.QD_CMD_UPDATE_SYSTEM, data, 0);
            }
        }
        return info;
    }

    private String getIpAddress(){
        WifiManager wifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
        WifiInfo info = wifiManager.getConnectionInfo();
        String stringIp = "";
        if(info != null){
            int ip = info.getIpAddress();
            stringIp = (ip & 0xFF) + "." + ((ip >> 8) & 0xFF) + "."+ ((ip >> 16) & 0xFF) + "." + ((ip >> 24) & 0xFF);
        }
        return stringIp;
    }

    @Override
    protected void handleUpdateDeviceAck(String ssid, int resultCode) {
        Logdeal.D(TAG, "handleUpdateDeviceAck...");
        QDAccessResult result = resultCode == Contants.OpenDoorResultType.OPEN_DOOR_SUCCESS ? QDAccessResult.OK : QDAccessResult.ERROR_WRITE_WIFI_DATA_EXCEPTION;
        if(mSocketCallback != null){
            mSocketCallback.onSocketResult(ssid, result);
        }
        //设置开门操作完成标志
        setStateFlag(FLAG_ALL_FINISH);
        //a.发送指令失败关闭设备
        //b.发送指令成功不能关闭设备，需保持wifi等待设备下载固件
        if(!result.isSuccess()){
            deviceClose();
        }
    }

    @Override
    protected void deviceClose() {
        super.deviceClose();
        //释放context
        mContext = null;
    }

}
