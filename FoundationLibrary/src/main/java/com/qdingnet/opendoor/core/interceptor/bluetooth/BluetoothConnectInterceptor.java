package com.qdingnet.opendoor.core.interceptor.bluetooth;

import android.bluetooth.BluetoothGatt;
import android.os.Handler;
import android.os.Looper;

import androidx.annotation.NonNull;

import com.qdingnet.opendoor.core.bluetooth.BluetoothLeClass;
import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.opendoor.core.interceptor.AbstractInterceptor;
import com.qdingnet.opendoor.core.request.Request;
import com.qdingnet.opendoor.log.Logdeal;

import java.util.concurrent.atomic.AtomicBoolean;

/**
 * 蓝牙连接拦截器
 * @author pipan (pipan@qding.me)
 * @date   19-1-16
 */
public class BluetoothConnectInterceptor extends AbstractInterceptor<BluetoothLeClass> implements BluetoothLeClass.OnConnectStateListener{
    private static final String TAG = "BLEConnectInterceptor";
    //连接超时时间
    private static final int CONNECT_TIMEOUT = 10_000;
    //连接并发锁
    private final AtomicBoolean mConnectLock = new AtomicBoolean(false);
    //连接超时操作实例
    private Runnable mTimeoutRunnable;

    public BluetoothConnectInterceptor(@NonNull BluetoothLeClass bluetoothLeClass) {
        super(bluetoothLeClass, IStateCallback.STATE_CONNECT);
    }

    @Override
    public void intercept(Chain next) {
        super.intercept(next);
        //连接设备
        final Request request = next.request();
        mDelegate.setOnConnectStateListener(this);
        //在主线程进行连接,以优化部分机型(huawei-mate7)无法连接成功
        Handler handler = new Handler(Looper.getMainLooper());
        handler.post(new Runnable() {
            @Override
            public void run() {
                mDelegate.connect(request.getAddress());
            }
        });
        //设置超时时间
        mTimeoutRunnable = new Runnable() {
            @Override
            public void run() {
                if(!mChain.isCanceled()){
                    onState(IStateCallback.CODE_TIMEOUT, "超时");
                }
            }
        };
        timeoutExecute(mTimeoutRunnable, CONNECT_TIMEOUT);
    }

    @Override
    public void onConnected(BluetoothGatt gatt) {
        Logdeal.D(TAG, "onConnected");
        //限制单连接操作
        if(mConnectLock.compareAndSet(false, true)){
            //取消超时操作
            cancelExecute(mTimeoutRunnable);
            //执行下一步
            process(null);
            //设置连接成功状态
            onState(IStateCallback.CODE_SUCCESS, "");
        }
    }

    @Override
    public void onDisconnected(BluetoothGatt gatt) {
        Logdeal.D(TAG, "onDisconnected...reconnect");
        reconnect();
    }

    @Override
    public void onFailureState(BluetoothGatt gatt, int state) {
        Logdeal.D(TAG, "onFailureState...reconnect");
        reconnect();
    }

    private void reconnect(){
        //重置可连接操作
        mConnectLock.set(false);
        //关闭设备
        mDelegate.close();
        //设备重连
        mDelegate.connect(mChain.request().getAddress());
    }

    @Override
    public void cancel() {
        super.cancel();
        //注销回调
        mDelegate.setOnConnectStateListener(null);
    }
}
