package com.qdingnet.provider.config.bean;

/**
 * 用于各业务操作的门禁设备模型
 * @author pipan (pipan@qding.me)
 * @date   19-2-28
 */
public class QDevice {
    //设备mac值
    public String mac;
    //设备开门最小信号值
    public int rssi;
    //设备开门时长(ms)
    public int openKeepTime;
    //设备来源
    public String deviceSource;
    //设备识别码
    public String deviceAuthCode;

    public QDevice(String mac) {
        this.mac = mac;
    }

    public QDevice(String mac, int rssi) {
        this(mac, rssi, 0);
    }
    public QDevice(String mac, int rssi, int openKeepTime) {
        this(mac, rssi, openKeepTime, null);
    }

    public QDevice(String mac, int rssi, int openKeepTime, String deviceAuthCode) {
        this.mac = mac;
        this.rssi = rssi;
        this.openKeepTime = openKeepTime;
        this.deviceAuthCode = deviceAuthCode;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        QDevice qDevice = (QDevice) o;
        return mac != null ? mac.equals(qDevice.mac) : qDevice.mac == null;
    }

    @Override
    public int hashCode() {
        return mac != null ? mac.hashCode() : 0;
    }

    @Override
    public String toString() {
        return "QDevice{" +
                "mac='" + mac + '\'' +
                ", rssi=" + rssi +
                ", openKeepTime=" + openKeepTime +
                ", deviceSource='" + deviceSource + '\'' +
                ", deviceAuthCode='" + deviceAuthCode + '\'' +
                '}';
    }
}
