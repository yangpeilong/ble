package com.qdingnet.qdaccess;

/**
 * 设备测试获取的设备信息数据
 * @author pipan (pipan@qding.me)
 * @date   18-9-29
 */
public class QDDeviceTestInfo {
    //网络断开
    public static final int NETWORK_OFF = 0;
    //网络连接
    public static final int NETWORK_ON = 1;


    //软件版本
    public String softwareVersion;
    //硬件版本
    public String hardwareVersion;
    //序列号
    public String serialNo;
    //IMEI号
    public String imeiNo;
    //设备mac
    public String mac;
    //网络状态
    public int networkState;

    public QDDeviceTestInfo(String softwareVersion, String hardwareVersion, String serialNo, String imeiNo, String mac, int networkState) {
        this.softwareVersion = softwareVersion;
        this.hardwareVersion = hardwareVersion;
        this.serialNo = serialNo;
        this.imeiNo = imeiNo;
        this.mac = mac;
        this.networkState = networkState;
    }

    @Override
    public String toString() {
        return "QDDeviceTestInfo{" +
                "softwareVersion='" + softwareVersion + '\'' +
                ", hardwareVersion='" + hardwareVersion + '\'' +
                ", serialNo='" + serialNo + '\'' +
                ", imeiNo='" + imeiNo + '\'' +
                ", mac='" + mac + '\'' +
                ", networkState=" + networkState +
                '}';
    }
}
