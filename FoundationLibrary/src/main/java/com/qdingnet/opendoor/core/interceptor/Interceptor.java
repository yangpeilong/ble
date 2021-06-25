package com.qdingnet.opendoor.core.interceptor;

import com.qdingnet.opendoor.core.request.Request;

/**
 * 蓝牙操作拦截接口
 * @author pipan (pipan@qding.me)
 * @date   19-1-15
 */
public interface Interceptor {
    /**
     * 拦截操作
     * @param next
     */
    void intercept(Interceptor.Chain next);

    /**
     * 主动取消操作
     */
    void cancel();

    /**
     * 蓝牙操作拦截链
     */
    interface Chain {
        /**
         * 执行下一个拦截操作
         */
        void process(Request request);

        /**
         * 获取当前请求参数
         * @return
         */
        Request request();

    }
}
