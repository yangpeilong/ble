package com.qdingnet.provider.config.callback;

import com.qdingnet.provider.config.bean.QDAccessResult;

/**
 * 设备升级回调接口
 * @author pipan (pipan@qding.me)
 * @date   2019/5/28
 */
public interface IDeviceUpgradeCallback {
    /**
     * 设备升级结果
     * @param mac
     * @param result
     */
    void onDeviceUpgradeResult(String mac, QDAccessResult result);
}
