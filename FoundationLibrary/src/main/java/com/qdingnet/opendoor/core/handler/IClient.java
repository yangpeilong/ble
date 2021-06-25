package com.qdingnet.opendoor.core.handler;

import android.content.Context;

import com.qdingnet.opendoor.core.request.Request;

/**
 * 智能设备客户端接口
 * @author pipan (pipan@qding.me)
 * @date   19-1-25
 */
public interface IClient {
    
    //启动成功状态
    int STATE_SUCCESS = 0;
    //启动进行中状态
    int STATE_WORKING = 1;
    //启动初始化失败状态
    int STATE_INIT_FAILURE = 2;

    /**
     * 启动
     * @param context
     * @param request
     * @return -- @see {@link #STATE_SUCCESS, #STATE_WORKING, #STATE_INIT_FAILURE}
     */
    public int start(Context context, Request request);

    /**
     * 取消
     */
    public void cancel();

    /**
     * 关闭
     */
    public void close();
}
