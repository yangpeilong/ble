package com.qdingnet.provider.config.callback;

import com.qdingnet.provider.config.bean.DeviceConfigMsg;
import com.qdingnet.provider.config.bean.QDAccessResult;

/**
 * 读取门禁设备配置回调
 * @author pipan (pipan@qding.me)
 * @date   2019/5/28
 */
public interface IReadConfigCallback {
    /**
     * 读取结果
     * @param mac
     * @param result
     * @param msg
     */
    void onReadConfigResult(String mac, QDAccessResult result, DeviceConfigMsg msg);
}
