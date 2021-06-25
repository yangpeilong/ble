package com.qdingnet.opendoor.core.interceptor.bluetooth;


import androidx.annotation.NonNull;

import com.qdingnet.opendoor.core.bluetooth.BluetoothLeClass;
import com.qdingnet.opendoor.core.bluetooth.advertise.BluetoothAdvertiser;
import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.opendoor.core.handler.IAdvertiseHandler;
import com.qdingnet.opendoor.core.interceptor.AbstractInterceptor;

import java.util.Map;

/**
 * 蓝牙广播拦截类
 * @author pipan (pipan@qding.me)
 * @date   19-1-16 
 */
public class BluetoothAdvertiseInterceptor extends AbstractInterceptor<BluetoothLeClass> implements BluetoothAdvertiser.OnAdvertiserCallback{
    private static final String TAG = "AdvertiseInterceptor";
    //广播超时时间
    private static final int ADVERTISE_TIMEOUT = 3_000;
    //设置蓝牙广播名字后开启广播的延时时间
    private static final int ADVERTISE_AFTER_SET_BLUETOOTH_NAME = 50;
    //广播操作实例
    private BluetoothAdvertiser mBluetoothAdvertiser;
    //广播数据处理接口
    private IAdvertiseHandler mIAdvertiseHandler;
    //广播超时操作实例
    private Runnable mTimeoutRunnable;

    public BluetoothAdvertiseInterceptor(@NonNull BluetoothLeClass bluetoothLeClass, @NonNull IAdvertiseHandler handler) {
        super(bluetoothLeClass, IStateCallback.STATE_ADVERTISE);
        checkNull(handler);
        mBluetoothAdvertiser = new BluetoothAdvertiser();
        mIAdvertiseHandler = handler;
    }

    @Override
    public void intercept(Chain next) {
        super.intercept(next);
        //设置广播操作回调
        mBluetoothAdvertiser.setOnAdvertiserCallback(this);
        //构造广播数据
        final Map<String,byte[]> advertiseData = mIAdvertiseHandler.buildAdvertiseData(mDelegate, next.request().getSsid());
        //启动广播
        timeoutExecute(new Runnable() {
            @Override
            public void run() {
                mBluetoothAdvertiser.startAdvertiser(true, ADVERTISE_TIMEOUT, advertiseData);
            }
        }, ADVERTISE_AFTER_SET_BLUETOOTH_NAME);
    }

    @Override
    public void onSuccess() {
        //延时启动下一个流程操作
        int delay = mIAdvertiseHandler.processNextDelayTime(mChain.request().getSsid());
        timeoutExecute(new Runnable() {
            @Override
            public void run() {
                //启动下一流程操作
                process(null);
                //设置广播成功状态
                onState(IStateCallback.CODE_SUCCESS, "");
            }
        }, delay > 0 ? delay : 0);
        //执行超时操作
        mTimeoutRunnable = new Runnable() {
            @Override
            public void run() {
                //停止广播
                mBluetoothAdvertiser.stopAdvertiser();
            }
        };
        timeoutExecute(mTimeoutRunnable, ADVERTISE_TIMEOUT);
    }

    @Override
    public void onFailure() {
        //取消超时操作
        cancelExecute(mTimeoutRunnable);
        //广播失败
        onState(IStateCallback.CODE_FAILURE, "广播失败");
        //启动下一流程操作
        process(null);
    }

    @Override
    public void cancel() {
        super.cancel();
        //将蓝牙名称复原
        mDelegate.resetBluetoothName();
        //停止广播
        mBluetoothAdvertiser.stopAdvertiser();
        //注销回调
        mBluetoothAdvertiser.setOnAdvertiserCallback(null);
    }
}
