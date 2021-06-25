package com.qdingnet.opendoor.core.interceptor.bluetooth;

import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;

import androidx.annotation.NonNull;

import com.qdingnet.opendoor.core.bluetooth.BluetoothLeClass;
import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.opendoor.core.handler.IDataHandler;
import com.qdingnet.opendoor.core.interceptor.AbstractInterceptor;
import com.qdingnet.opendoor.log.Logdeal;

/**
 * 蓝牙接收数据拦截器
 * @author pipan (pipan@qding.me)
 * @date   19-1-21 
 */
public class BluetoothReceiveDataInterceptor extends AbstractInterceptor<BluetoothLeClass> implements BluetoothLeClass.OnDataNotifyListener{
    private static final String TAG = "BLERecvInterceptor";
    //数据操作超时时间
    private static final int TRANSFER_DATA_TIMEOUT = 20_000;

    //数据处理器
    private IDataHandler mIDataHandler;
    //服务发现超时操作实例
    private Runnable mTimeoutRunnable;
    //读数据超时时间
    private int mReadTimeout;

    public BluetoothReceiveDataInterceptor(@NonNull BluetoothLeClass bluetoothLeClass, @NonNull IDataHandler handler){
        this(bluetoothLeClass, handler, TRANSFER_DATA_TIMEOUT);
    }
    public BluetoothReceiveDataInterceptor(@NonNull BluetoothLeClass bluetoothLeClass, @NonNull IDataHandler handler, int readTimeout) {
        super(bluetoothLeClass, IStateCallback.STATE_RECV_DATA);
        checkNull(handler);
        this.mIDataHandler = handler;
        this.mReadTimeout = readTimeout > 0 ? readTimeout : TRANSFER_DATA_TIMEOUT;
    }

    @Override
    public void intercept(Chain next) {
        super.intercept(next);
        //设置接收数据监听器
        mDelegate.setOnDataNotifyListener(this);
        //执行超时操作
        mTimeoutRunnable = new Runnable() {
            @Override
            public void run() {
                if(!mIDataHandler.isSuccess() && !mChain.isCanceled()){
                    onState(IStateCallback.CODE_TIMEOUT, "超时");
                }
            }
        };
        timeoutExecute(mTimeoutRunnable, mReadTimeout);
    }

    /**********************************数据通知*******************************/
    @Override
    public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
        Logdeal.D(TAG, "onCharacteristicChanged...thread:" + Thread.currentThread().getName());
        if (mIDataHandler != null) {
            mIDataHandler.handleDataChanged(gatt.getDevice().getName(), characteristic.getValue());
        }
    }

    @Override
    public void cancel() {
        super.cancel();
        //注销回调
        mDelegate.setOnDataNotifyListener(null);
    }
}
