package com.qdingnet.opendoor.core.interceptor;

import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.opendoor.core.request.Request;
import com.qdingnet.opendoor.log.Logdeal;

import java.util.List;

/**
 * 处理蓝牙/wifi各拦截操作器链
 * @author pipan (pipan@qding.me)
 * @date   19-1-15 
 */
public class RealInterceptorChain implements Interceptor.Chain, IStateCallback{
    private static final String TAG = "RealChain";
    //设备操作拦截器列表
    private List<Interceptor> mInterceptors;
    //设备操作请求参数
    private Request mRequest;
    //设备当前操作拦截器索引
    private int mIndex;
    //各拦截器状态回调接口
    private IStateCallback mIStateCallback;
    //是否取消操作的状态
    private boolean canceled;

    public RealInterceptorChain(List<Interceptor> interceptors, int index, Request request, IStateCallback callback) {
        this.mInterceptors = interceptors;
        this.mRequest = request;
        this.mIndex = index;
        this.mIStateCallback = callback;
    }

    /**
     * 取消操作
     */
    public void cancel(){
        this.canceled = true;
        //所有拦截器执行取消操作，并清空拦截器资源
        if(mInterceptors != null){
            for(Interceptor interceptor: mInterceptors){
                interceptor.cancel();
            }
            mInterceptors.clear();
        }
    }

    /**
     * 蓝牙操作是否已取消
     * @return
     */
    public boolean isCanceled(){
        return canceled;
    }

    /**
     * 获取当前操作拦截器索引
     * @return
     */
    public int getIndex() {
        return mIndex;
    }

    /**
     * 获取请求参数
     * @return
     */
    @Override
    public Request request() {
        return mRequest;
    }

    @Override
    public void process(Request request) {
        int size = mInterceptors != null ? mInterceptors.size() : 0;
        if (size == 0) {
            Logdeal.D(TAG, "The interceptors is null or blank. Finish Process!!!");
            return;
        }
        if (mIndex < 0 || mIndex >= size) {
            Logdeal.D(TAG, String.format("The interceptors index [%d] is smaller than 0 or greater than %d. Finish Process!!!", mIndex, size));
            return;
        }
        Logdeal.D(TAG, "process...interceptor index:" + mIndex);
        //获取当前操作的拦截器
        Interceptor interceptor = mInterceptors.get(mIndex);
        RealInterceptorChain chain = new RealInterceptorChain(mInterceptors, mIndex+1, mRequest, mIStateCallback);
        interceptor.intercept(chain);
    }

    @Override
    public void onState(int state, int code, String ssid, String msg) {
        if(mIStateCallback != null){
            mIStateCallback.onState(state, code, ssid, msg);
        }
    }
}
