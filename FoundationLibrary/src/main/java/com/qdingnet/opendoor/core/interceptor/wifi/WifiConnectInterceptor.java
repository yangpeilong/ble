package com.qdingnet.opendoor.core.interceptor.wifi;


import androidx.annotation.NonNull;

import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.opendoor.core.handler.IWifiConnectHandler;
import com.qdingnet.opendoor.core.interceptor.AbstractInterceptor;
import com.qdingnet.opendoor.core.wifi.WifiManagerClass;
import com.qdingnet.opendoor.log.Logdeal;

/**
 * wifi连接拦截器
 * @author pipan (pipan@qding.me)
 * @date   19-2-16
 */
public class WifiConnectInterceptor extends AbstractInterceptor<WifiManagerClass> {
    private static final String TAG = "WifiConnectInterceptor";

    //wifi默认连接时间
    private static final int DEFAULT_TIMEOUT = 10_000;
    //查看连接结果间隔时间
    private static final int CONNECT_RESULT_TIMEOUT = 1000;
    //wifi密码
    private final String mPassword;
    //wifi连接超时时间
    private final int mConnectTimeout;

    //wifi连接处理接口
    private IWifiConnectHandler mIWifiConnectHandler;

    public WifiConnectInterceptor(@NonNull WifiManagerClass deviceDelegate, @NonNull String password, IWifiConnectHandler handler) {
        this(deviceDelegate, password, 0, handler);
    }
    public WifiConnectInterceptor(@NonNull WifiManagerClass deviceDelegate, @NonNull String password, int connectTimeout, IWifiConnectHandler handler) {
        super(deviceDelegate, IStateCallback.STATE_CONNECT);
        checkNull(password);
        this.mPassword = password;
        this.mConnectTimeout = connectTimeout > 0 ? connectTimeout : DEFAULT_TIMEOUT;
        this.mIWifiConnectHandler = handler;
    }

    @Override
    public void intercept(Chain next) {
        super.intercept(next);
        //开始连接
        connect(next.request().getSsid());
    }

    private void connect(final String ssid){
        mDelegate.connection(ssid, mPassword, WifiManagerClass.WifiConnect.WifiCipherType.WIFICIPHER_WPA);
        //查看连接是否成功任务
        final Runnable connectRunnable = new Runnable() {
            @Override
            public void run() {
                Logdeal.D(TAG, "connect...check");
                if(mDelegate.isConnected(ssid)){
                    //回调处理
                    if(mIWifiConnectHandler != null){
                        mIWifiConnectHandler.onConnected(ssid, mDelegate.getIpAddress());
                    }
                    //设置连接成功状态
                    onState(IStateCallback.CODE_SUCCESS, "");
                    //取消超时操作
                    cancel();
                }else {
                    timeoutExecute(this, CONNECT_RESULT_TIMEOUT);
                }
            }
        };
        timeoutExecute(connectRunnable, CONNECT_RESULT_TIMEOUT);
        //连接超时任务
        final Runnable timeoutRunnable = new Runnable() {
            @Override
            public void run() {
                cancel();
                //回调上层
                if(!mChain.isCanceled()){
                    onState(IStateCallback.CODE_TIMEOUT, "超时");
                }
            }
        };
        timeoutExecute(timeoutRunnable, mConnectTimeout);
    }
}
