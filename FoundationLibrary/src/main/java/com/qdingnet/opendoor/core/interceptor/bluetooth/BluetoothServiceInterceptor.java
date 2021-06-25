package com.qdingnet.opendoor.core.interceptor.bluetooth;

import android.bluetooth.BluetoothGatt;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;

import com.qdingnet.opendoor.core.bluetooth.BluetoothLeClass;
import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.opendoor.core.handler.IGattServiceAuthHandler;
import com.qdingnet.opendoor.core.interceptor.AbstractInterceptor;
import com.qdingnet.opendoor.log.Logdeal;

/**
 * 蓝牙服务发现拦截器
 * @author pipan (pipan@qding.me)
 * @date   19-1-21
 */
@RequiresApi(18)
public class BluetoothServiceInterceptor extends AbstractInterceptor<BluetoothLeClass> implements BluetoothLeClass.OnServiceDiscoverListener{
    private static final String TAG = "BLEServiceInterceptor";
    //服务发现延时时间
    private static final int DISCOVER_SERVICE_DELAY = 500;
    //服务发现超时时间
    private static final int DISCOVER_SERVICE_TIMEOUT = 10_000;
    //服务验证接口
    private IGattServiceAuthHandler mIAuthHandler;
    //服务发现超时操作实例
    private Runnable mTimeoutRunnable;

    public BluetoothServiceInterceptor(@NonNull BluetoothLeClass bluetoothLeClass, IGattServiceAuthHandler authHandler) {
        super(bluetoothLeClass, IStateCallback.STATE_DISCOVER_SERVICE);
        this.mIAuthHandler = authHandler;
    }

    @Override
    public void intercept(Chain next) {
        super.intercept(next);
        //启动发现服务
        mDelegate.discoverServices();
        //设置蓝牙服务发现监听器
        mDelegate.setOnServiceDiscoverListener(this);
        //设置超时操作
        if(mTimeoutRunnable == null){
            mTimeoutRunnable = new Runnable() {
                @Override
                public void run() {
                    if(!mChain.isCanceled()){
                        onState(IStateCallback.CODE_TIMEOUT, "超时");
                    }
                }
            };
            timeoutExecute(mTimeoutRunnable, DISCOVER_SERVICE_TIMEOUT);
        }
    }

    @Override
    public void onServiceDiscover(final BluetoothGatt gatt) {
        //服务验证
        if(mIAuthHandler == null || mIAuthHandler.onGattServiceAuth(gatt)){
            //读取rssi
            gatt.readRemoteRssi();
            //下一步操作
            process(null);
            //取消超时操作
            cancelExecute(mTimeoutRunnable);
            //设置发现服务成功状态
            onState(IStateCallback.CODE_SUCCESS, "");
        }else{
            Logdeal.D(TAG, "onServiceDiscover...ReDiscoverService");
            //刷新设备服务
            mDelegate.refresh();
            //重新搜索服务
            timeoutExecute(new Runnable() {
                @Override
                public void run() {
                    gatt.discoverServices();
                }
            }, DISCOVER_SERVICE_DELAY);
        }
    }

    @Override
    public void onSerivceError(int status) {
        //no-op ignore
    }

    @Override
    public void cancel() {
        super.cancel();
        //注销回调
        mDelegate.setOnServiceDiscoverListener(null);
    }
}
