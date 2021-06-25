package com.qdingnet.provider.config.callback;

import com.qdingnet.provider.config.bean.QDAccessResult;

/**
 * 读取设备版本信息回调
 * @author pipan (pipan@qding.me)
 * @date   2019/5/28
 */
public interface IReadVersionCallback {
    /**
     * 读取设备版本信息结果
     * @param mac
     * @param result
     * @param softwareVersion
     * @param hardwareVersion
     */
    void onReadVersionResult(String mac, QDAccessResult result, String softwareVersion, String hardwareVersion);
}
