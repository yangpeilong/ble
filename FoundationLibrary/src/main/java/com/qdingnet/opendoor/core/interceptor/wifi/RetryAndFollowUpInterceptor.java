package com.qdingnet.opendoor.core.interceptor.wifi;


import androidx.annotation.NonNull;

import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.opendoor.core.interceptor.AbstractInterceptor;
import com.qdingnet.opendoor.core.wifi.WifiManagerClass;

/**
 * wifi重试处理拦截器
 * @author pipan (pipan@qding.me)
 * @date   19-1-22
 */
public class RetryAndFollowUpInterceptor extends AbstractInterceptor<WifiManagerClass> {
    private static final String TAG = "RetryAndFollowUpInterceptor";

    public RetryAndFollowUpInterceptor(@NonNull WifiManagerClass wifiManagerClass) {
        super(wifiManagerClass, IStateCallback.STATE_INIT);
    }

    /**
     * 重试操作
     */
    public void retry(){
        //no-op temporarily
    }

    @Override
    public void intercept(Chain next) {
        super.intercept(next);
        //设置初始化成功状态
        onState(IStateCallback.CODE_SUCCESS, "");
        next.process(null);
    }

}
