package com.qdingnet.opendoor.core.interceptor.wifi;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;

import androidx.annotation.NonNull;

import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.opendoor.core.handler.IAuthHandler;
import com.qdingnet.opendoor.core.interceptor.AbstractInterceptor;
import com.qdingnet.opendoor.core.request.Request;
import com.qdingnet.opendoor.core.wifi.WifiManagerClass;
import com.qdingnet.opendoor.log.Logdeal;

import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * wifi扫描拦截类
 * @author pipan (pipan@qding.me)
 * @date   19-2-16
 */
public class WifiScanInterceptor extends AbstractInterceptor<WifiManagerClass>{
    private static final String TAG = "WifiScanInterceptor";
    //默认扫描超时时间
    private static final int DEFAULT_TIMEOUT = 10_000;
    //查看扫描结果间隔时间
    private static final int SCAN_RESULT_TIMEOUT = 1000;

    //设备验证授权处理器
    private IAuthHandler mIAuthHandler;
    //扫描超时时间
    private int mTimeout;
    //wifi广播接收器
    private WifiReceiver mWifiReceiver;
    //wifi广播注册标志
    private AtomicBoolean mWifiRegisterMark = new AtomicBoolean(false);

    public WifiScanInterceptor(@NonNull WifiManagerClass deviceDelegate, @NonNull IAuthHandler authHandler, int timeout) {
        super(deviceDelegate, IStateCallback.STATE_SCAN);
        this.mIAuthHandler = authHandler;
        this.mTimeout = timeout > 0 ? timeout : DEFAULT_TIMEOUT;
    }

    @Override
    public void intercept(Chain next) {
        super.intercept(next);
        if(mDelegate.wifiEnabled()){
            //启动扫描
            startScan();
        }
        else {
            //打开wifi
            mDelegate.enableWifi();
            //注册wifi广播接收器
            registerWifiReceiver();
        }
    }

    private void startScan(){
        mDelegate.startScan();
        //添加扫描结果处理任务
        final Request request = mChain.request();
        final Runnable scanRunnable = new Runnable() {
            @Override
            public void run() {
                List<ScanResult> scanResults = mDelegate.getScanResults();
                //查找最近设备
                ScanResult scanResult = null;
                if(scanResults != null && scanResults.size() > 0){
                    for(ScanResult sr: scanResults){
                        if(mIAuthHandler.onAuth(sr.SSID, sr.level)
                                && (scanResult == null || scanResult.level < sr.level)){
                            scanResult = sr;
                        }
                    }
                }
                //是否有符合设备
                if(scanResult != null){
                    Logdeal.D(TAG, "scan result:" + scanResult);
                    request.setSsid(scanResult.SSID);
                    request.setAddress(scanResult.BSSID);
                    //取消队列任务
                    cancel();
                    //跳转下一拦截器
                    process(request);
                    //设置扫描成功状态
                    onState(IStateCallback.CODE_SUCCESS, "");
                }else {
                    //重新扫描
                    mDelegate.startScan();
                    timeoutExecute(this, SCAN_RESULT_TIMEOUT);
                }
            }
        };
        timeoutExecute(scanRunnable, SCAN_RESULT_TIMEOUT);
        //添加超时处理任务
        final Runnable timeoutRunnable = new Runnable() {
            @Override
            public void run() {
                //取消队列任务
                cancel();
                //回调上层
                if(!mChain.isCanceled()){
                    onState(IStateCallback.CODE_TIMEOUT, "超时");
                }
            }
        };
        timeoutExecute(timeoutRunnable, mTimeout);
    }

    @Override
    public void cancel() {
        super.cancel();
        //注销监听器
        unregisterWifiReceiver();
    }

    /**
     * 注册wifi广播监听器
     */
    private void registerWifiReceiver() {
        if (mWifiRegisterMark.compareAndSet(false, true)) {
            mWifiReceiver = new WifiReceiver();
            IntentFilter filter = new IntentFilter();
            filter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
            Context context = mDelegate.getContext();
            if(context != null){
                context.registerReceiver(mWifiReceiver, filter);
            }
        }
    }

    /**
     * 注销wifi广播监听器
     */
    private synchronized void unregisterWifiReceiver() {
        if (mWifiRegisterMark.compareAndSet(true, false)) {
            Context context = mDelegate.getContext();
            if(context != null){
                context.unregisterReceiver(mWifiReceiver);
            }
        }
    }

    /**
     * wifi开关,扫描,连接状态receiver
     */
    protected final class WifiReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Logdeal.D(TAG, "WifiReceiver...onReceive...action:" + action);
            if (WifiManager.WIFI_STATE_CHANGED_ACTION.equals(action)) {
                int state = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, -1);
                switch (state) {
                    case WifiManager.WIFI_STATE_ENABLED:
                        startScan();
                        break;
                    case WifiManager.WIFI_STATE_DISABLED:
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
