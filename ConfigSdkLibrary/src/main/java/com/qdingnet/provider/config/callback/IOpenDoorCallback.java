package com.qdingnet.provider.config.callback;

import com.qdingnet.provider.config.bean.QDAccessResult;

/**
 * 开门回调接口
 */
public interface IOpenDoorCallback {
   void onOpenDoorResult(String mac, QDAccessResult result);
}
