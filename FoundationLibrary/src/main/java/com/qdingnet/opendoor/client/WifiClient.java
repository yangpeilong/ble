package com.qdingnet.opendoor.client;

import android.content.Context;

import com.qdingnet.opendoor.core.callback.ICloseable;
import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.opendoor.core.handler.IAuthHandler;
import com.qdingnet.opendoor.core.handler.IClient;
import com.qdingnet.opendoor.core.handler.IDataHandler;
import com.qdingnet.opendoor.core.handler.IWifiConnectHandler;
import com.qdingnet.opendoor.core.interceptor.Interceptor;
import com.qdingnet.opendoor.core.interceptor.RealInterceptorChain;
import com.qdingnet.opendoor.core.interceptor.wifi.RetryAndFollowUpInterceptor;
import com.qdingnet.opendoor.core.interceptor.wifi.WifiConnectInterceptor;
import com.qdingnet.opendoor.core.interceptor.wifi.WifiScanInterceptor;
import com.qdingnet.opendoor.core.request.Request;
import com.qdingnet.opendoor.core.wifi.WifiManagerClass;
import com.qdingnet.opendoor.log.Logdeal;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * wifi操作工具类v2版
 * @author pipan (pipan@qding.me)
 * @date   19-1-28
 */
public class WifiClient implements IClient{
    private static final String TAG = "WifiClient";

    //wifi操作客户端
    private WifiManagerClass mWifiManagerClass;
    //重试/取消操作拦截器
    private RealInterceptorChain mRealInterceptorChain;
    //wifi操作启动标志
    private AtomicBoolean mEnable = new AtomicBoolean(false);

    //连接wifi的ssid命名正则规则
    private String mSsidNameRegex;
    //连接wifi的密码
    private String mSsidPassword;
    //连接wifi超时时间
    private int mConnectTimeout;
    //设备验证授权处理器
    private IAuthHandler mIAuthHandler;
    //门禁数据处理器
    private IDataHandler mIDataHandler;
    //wifi连接处理器
    private IWifiConnectHandler mIWifiConnectHandler;
    //wifi状态回调接口
    private IStateCallback mIStateCallback;
    //上层触发设备客户端关闭接口
    private ICloseable.Factory mICloseableFactory;

    WifiClient(Builder builder){
        this(builder.mSsidNameRegex, builder.mSsidPassword, builder.mConnectTimeout, builder.mIAuthHandler, builder.mIWifiConnectHandler, builder.mIDataHandler, builder.mIStateCallback, builder.mICloseableFactory);
    }
    WifiClient() {}
    WifiClient(String ssidNameRegex, String ssidPassword, int connectTimeout, IAuthHandler authHandler, IWifiConnectHandler wifiConnectHandler, IDataHandler dataHandler, IStateCallback stateCallback, ICloseable.Factory factory) {
        this.mSsidNameRegex = ssidNameRegex;
        this.mSsidPassword = ssidPassword;
        this.mConnectTimeout = connectTimeout;
        this.mIAuthHandler = authHandler;
        this.mIDataHandler = dataHandler;
        this.mIWifiConnectHandler = wifiConnectHandler;
        this.mIStateCallback = stateCallback;
        this.mICloseableFactory = factory;
    }

    /**
     * 创建wifi拦截器处理链
     * @param wifiManagerClass
     * @param request
     * @return
     */
    private RealInterceptorChain createInterceptorChain(WifiManagerClass wifiManagerClass, Request request){
        //拦截器
        List<Interceptor> mInterceptors = new ArrayList<>();
        //重试/取消操作拦截器
        mInterceptors.add(new RetryAndFollowUpInterceptor(wifiManagerClass));
        //扫描
        mInterceptors.add(new WifiScanInterceptor(wifiManagerClass, mIAuthHandler, 10_000));
        //连接
        mInterceptors.add(new WifiConnectInterceptor(wifiManagerClass, mSsidPassword, mConnectTimeout, mIWifiConnectHandler));
        return new RealInterceptorChain(mInterceptors, 0, request, new StateCallbackAdapter(mIStateCallback, mICloseableFactory));
    }

    @Override
    public int start(Context context, Request request) {
        if(mEnable.compareAndSet(false, true)){
            Logdeal.D(TAG, "start");
            mWifiManagerClass = new WifiManagerClass(context);
            //初始化
            if(!mWifiManagerClass.initialize()){
                return IClient.STATE_INIT_FAILURE;
            }
            mWifiManagerClass.storeOriginWifi(mSsidNameRegex);
            //开始处理
            RealInterceptorChain chain = createInterceptorChain(mWifiManagerClass, request);
            chain.process(request);
            mRealInterceptorChain = chain;
            return IClient.STATE_SUCCESS;
        }else {
            return IClient.STATE_WORKING;
        }
    }

    @Override
    public void cancel() {
        if(mRealInterceptorChain != null){
            mRealInterceptorChain.cancel();
        }
    }

    @Override
    public void close() {
        if(mEnable.compareAndSet(true, false)){
            cancel();
            Logdeal.D(TAG, "close");
            mWifiManagerClass.resetOriginWifi(mSsidNameRegex);
        }
    }

    /**
     * wifi内部状态回调适配器
     */
    final class StateCallbackAdapter implements IStateCallback{
        IStateCallback mIStateCallback;
        ICloseable.Factory mCloseableFactory;
        public StateCallbackAdapter(IStateCallback callback, ICloseable.Factory factory) {
            this.mIStateCallback = callback;
            this.mCloseableFactory = factory;
        }

        @Override
        public void onState(int state, int code, String ssid, String msg) {
            Logdeal.D(TAG, String.format("state:%d, code:%d, ssid:%s, msg:%s", state, code, ssid != null ? ssid : "NULL", msg));
            if(mIStateCallback != null){
                mIStateCallback.onState(state, code, ssid, msg);
            }
            //wifi操作超过/被取消/
            if(code == CODE_CANCELED || code == CODE_DISCONNECT || code == CODE_TIMEOUT){
                if(mCloseableFactory != null){
                    mCloseableFactory.create().close();
                }
            }
        }
    }

    public static class Builder{
        //连接wifi的ssid命名正则规则
        private String mSsidNameRegex;
        //连接wifi的密码
        private String mSsidPassword;
        //连接wifi超时时间
        private int mConnectTimeout;
        //连接wifi验证处理器
        private IAuthHandler mIAuthHandler;
        //门禁数据处理器
        private IDataHandler mIDataHandler;
        //wifi连接处理器
        private IWifiConnectHandler mIWifiConnectHandler;
        //wifi状态回调接口
        private IStateCallback mIStateCallback;
        //上层触发设备客户端关闭接口
        private ICloseable.Factory mICloseableFactory;

        public Builder setSsidNameRegex(String regex) {
            this.mSsidNameRegex = regex;
            return this;
        }

        public Builder setSsidPassword(String password){
            this.mSsidPassword = password;
            return this;
        }

        public Builder setConnectTimeout(int connectTimeout){
            this.mConnectTimeout = connectTimeout;
            return this;
        }

        public Builder setIAuthHandler(IAuthHandler handler) {
            this.mIAuthHandler = handler;
            return this;
        }

        public Builder setIDataHandler(IDataHandler handler) {
            this.mIDataHandler = handler;
            return this;
        }

        public Builder setIWifiConnectHandler(IWifiConnectHandler handler) {
            this.mIWifiConnectHandler = handler;
            return this;
        }

        public Builder setIStateCallback(IStateCallback callback) {
            this.mIStateCallback = callback;
            return this;
        }

        public Builder setICloseableFactory(ICloseable.Factory factory) {
            this.mICloseableFactory = factory;
            return this;
        }

        public WifiClient build(){
            WifiClient client = new WifiClient(this);
            //注册实际设备客户端
            if(mICloseableFactory != null){
                mICloseableFactory.setIClient(client);
                //数据处理器中注册设备关闭回调接口
                if(mIDataHandler != null){
                    mIDataHandler.registerCloseableFactory(mICloseableFactory);
                }
            }
            return client;
        }
    }

}
