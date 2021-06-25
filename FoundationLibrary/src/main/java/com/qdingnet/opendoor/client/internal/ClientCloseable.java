package com.qdingnet.opendoor.client.internal;

import com.qdingnet.opendoor.core.callback.ICloseCallback;
import com.qdingnet.opendoor.core.callback.ICloseable;
import com.qdingnet.opendoor.core.handler.IClient;
import com.qdingnet.opendoor.log.Logdeal;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * 用于上层对设备客户端进行关闭操作类
 * @author pipan (pipan@qding.me)
 * @date   2019/5/31
 */
public class ClientCloseable implements ICloseable.Factory {
    private static final String TAG = "ClientCloseable";
    /**
     * 关闭操作是否触发
     */
    private final AtomicBoolean mInvoked = new AtomicBoolean(false);
    /**
     * 设备操作客户端
     */
    private IClient mIClient;
    /**
     * 设备关闭操作触发接口
     */
    private ICloseable mICloseable;
    /**
     * 设备关闭回调接口集
     */
    private final List<ICloseCallback> mICloseCallbackList = new ArrayList<>();

    @Override
    public synchronized ICloseable create() {
        if(mICloseable == null){
            mICloseable = new ICloseable() {
                @Override
                public void close() {
                    if(mInvoked.compareAndSet(false, true)){
                        Logdeal.D(TAG, "close...");
                        //分发关闭前通知
                        for(ICloseCallback callback: mICloseCallbackList){
                            callback.onBeforeClose();
                        }
                        //实际关闭操作
                        if(mIClient != null){
                            mIClient.close();
                            mIClient = null;
                        }
                        //分发关闭后通知
                        for(ICloseCallback callback: mICloseCallbackList){
                            callback.onAfterClose();
                        }
                        mICloseCallbackList.clear();
                    }
                }
            };
        }
        return mICloseable;
    }

    @Override
    public void setIClient(IClient client) {
        this.mIClient = client;
    }

    @Override
    public void registerCloseCallback(ICloseCallback callback) {
        mICloseCallbackList.add(callback);
    }
}
