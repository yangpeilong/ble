package com.qdingnet.provider.config.callback.internal;

import com.qdingnet.provider.config.bean.QDAccessResult;

/**
 * socket操作回调
 * @author pipan (pipan@qding.me)
 * @date   2019/5/28
 */
public interface ISocketHandler {
    /**
     * 回调结果
     * @param ssid
     * @param result
     */
    void onSocketResult(String ssid, QDAccessResult result);
}
