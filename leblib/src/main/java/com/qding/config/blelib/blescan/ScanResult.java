package com.qding.config.blelib.blescan;

/**
 * 蓝牙扫描结果
 * @author pipan (pipan@qding.me)
 * @date   19-3-4
 */
public class ScanResult {
    public String ssid;
    public int rssi;
    public String address;

    public ScanResult(String ssid, int rssi, String address) {
        this.ssid = ssid;
        this.rssi = rssi;
        this.address = address;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        ScanResult that = (ScanResult) o;

        if (ssid != null ? !ssid.equals(that.ssid) : that.ssid != null) return false;
        return address != null ? address.equals(that.address) : that.address == null;
    }

    @Override
    public int hashCode() {
        int result = ssid != null ? ssid.hashCode() : 0;
        result = 31 * result + (address != null ? address.hashCode() : 0);
        return result;
    }

    @Override
    public String toString() {
        return "{" +
                "ssid='" + ssid + '\'' +
                ", rssi=" + rssi +
                '}';
    }
}
