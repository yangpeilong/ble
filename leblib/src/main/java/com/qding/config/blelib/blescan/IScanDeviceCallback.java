package com.qding.config.blelib.blescan;

/**
  * 蓝牙扫描回调接口
  * @author pipan (pipan@qding.me)
  * @date   18-9-25 
 */
public interface IScanDeviceCallback {
    /**
     * 扫描设备信息
     * @param result
     */
    void onScanResult(ScanResult result);

    /**
     * 扫描完成
     */
    void onScanFinish();
}

