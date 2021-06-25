package com.qdingnet.provider.config.callback;

import com.qdingnet.provider.config.bean.QDAccessResult;

/**
 * 配置门禁设备回调
 * @author pipan (pipan@qding.me)
 * @date   2019/5/16
 */
public interface IWriteConfigCallback {
    /**
     * 配置结果
     * @param mac
     * @param result
     */
    void onWriteConfigResult(String mac, QDAccessResult result);
}
