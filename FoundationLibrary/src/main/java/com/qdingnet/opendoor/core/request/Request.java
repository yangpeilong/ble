package com.qdingnet.opendoor.core.request;

/**
 * 设备(蓝牙,wifi)操作请求
 * @author pipan (pipan@qding.me)
 * @date   19-1-15 
 */
public class Request {
    //当前操作的门禁设备ssid
    private String ssid;
    //当前操作的门禁设备地址
    private String address;

    public String getSsid() {
        return ssid;
    }

    public void setSsid(String ssid) {
        this.ssid = ssid;
    }

    public String getAddress() {
        return address;
    }

    public void setAddress(String address) {
        this.address = address;
    }
}
