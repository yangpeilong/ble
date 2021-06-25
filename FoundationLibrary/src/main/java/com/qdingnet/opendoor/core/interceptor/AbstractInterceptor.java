package com.qdingnet.opendoor.core.interceptor;

import com.qdingnet.opendoor.log.Logdeal;
import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.opendoor.core.misc.MessageHandler;
import com.qdingnet.opendoor.core.request.Request;

/**
 * 拦截器基类
 * @author pipan (pipan@qding.me)
 * @date   19-1-15
 */
public abstract class AbstractInterceptor<T extends IDelegateClass> implements Interceptor {
    private static final String TAG = "AbstractInterceptor";

    //设备操作类
    protected T mDelegate;
    //实际拦截处理链
    protected RealInterceptorChain mChain;
    //超时处理器
    private MessageHandler mInterceptorTimeoutHandler;
    /**
     * 拦截器所处状态
     * @see {@link # IStateCallback.STATE_* }
     */
    private int mState;

    public AbstractInterceptor(T deviceDelegate, int state) {
        checkNull(deviceDelegate);
        this.mDelegate = deviceDelegate;
        this.mState = state;
    }

    public void setState(int state) {
        this.mState = state;
    }

    @Override
    public void intercept(Chain next) {
        Logdeal.D(TAG, "intercept...class:" + getClass().getSimpleName());
        mChain = (RealInterceptorChain) next;
    }

    /**
     * 检测空指针
     * @param target
     */
    protected void checkNull(Object target){
        if(target == null) throw new NullPointerException("The target is Null");
    }

    /**
     * 设置超时操作
     * @param runnable
     * @param timeout
     */
    protected void timeoutExecute(Runnable runnable, int timeout){
        if(mInterceptorTimeoutHandler == null){
            mInterceptorTimeoutHandler = new MessageHandler(getClass().getSimpleName());
        }
        mInterceptorTimeoutHandler.execute(runnable, timeout);
    }

    /**
     * 取消超时操作
     * @param runnable
     */
    protected void cancelExecute(Runnable runnable){
        if(mInterceptorTimeoutHandler != null){
            mInterceptorTimeoutHandler.cancel(runnable);
        }
    }

    /**
     * 执行下一个拦截器
     */
    protected void process(Request request){
        if(mChain.isCanceled()) {
            Logdeal.D(TAG, "process...canceled");
            onState(IStateCallback.CODE_CANCELED, "操作被取消");
        }else {
            Request r = request != null ? request : mChain.request();
            mChain.process(r);
        }
    }

    /**
     * 蓝牙操作各状态回调
     * @param code
     * @param msg
     */
    protected void onState(int code, String msg){
        mChain.onState(mState, code, mChain.request().getSsid(), msg);
    }

    @Override
    public void cancel() {
        if(mInterceptorTimeoutHandler != null){
            mInterceptorTimeoutHandler.close();
        }
    }

}
