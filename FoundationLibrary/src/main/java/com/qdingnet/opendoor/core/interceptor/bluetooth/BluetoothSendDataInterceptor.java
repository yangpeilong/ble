package com.qdingnet.opendoor.core.interceptor.bluetooth;

import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;

import com.qdingnet.opendoor.core.bean.DataPacket;
import com.qdingnet.opendoor.core.bluetooth.BluetoothLeClass;
import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.opendoor.core.handler.IDataHandler;
import com.qdingnet.opendoor.core.interceptor.AbstractInterceptor;
import com.qdingnet.opendoor.log.Logdeal;

import java.util.UUID;

/**
 * 蓝牙发送数据拦截器
 * @author pipan (pipan@qding.me)
 * @date   19-1-21 
 */
@RequiresApi(18)
public class BluetoothSendDataInterceptor extends AbstractInterceptor<BluetoothLeClass> implements BluetoothLeClass.OnDataAvailableListener{
    private static final String TAG = "BLESendDataInterceptor";
    //默认数据写超时时间
    private static final int WRITE_TIMEOUT = 10_000;
    //向设备端写数据通道
    private BluetoothGattCharacteristic mWriteCharacteristic;
    //需发送的数据
    private DataPacket mDataPacket;
    //数据处理器
    private IDataHandler mIDataHandler;
    //数据处理超时操作实例
    private Runnable mTimeoutRunnable;
    //写数据超时时间
    private int mWriteTimeout;

    public BluetoothSendDataInterceptor(@NonNull BluetoothLeClass bluetoothLeClass, @NonNull IDataHandler handler){
        this(bluetoothLeClass, handler, WRITE_TIMEOUT);
    }
    public BluetoothSendDataInterceptor(@NonNull BluetoothLeClass bluetoothLeClass, @NonNull IDataHandler handler, int writeTimeout) {
        super(bluetoothLeClass, IStateCallback.STATE_SEND_DATA);
        checkNull(handler);
        this.mIDataHandler = handler;
        this.mWriteTimeout = writeTimeout > 0 ? writeTimeout : WRITE_TIMEOUT;
    }

    @Override
    public void intercept(Chain next) {
        super.intercept(next);
        //设置发送数据监听器
        mDelegate.setOnDataAvailableListener(this);
        //获取需发送的数据
        mDataPacket = mIDataHandler.handleStartSendData(next.request().getSsid());
        if(mDataPacket == null){
            onState(IStateCallback.CODE_FAILURE, "获取发送的数据为空");
            return;
        }
        //获取写通道
        if(extractWriteCharacteristic(mDataPacket.getServiceUUID(), mDataPacket.getCharacteristicUUID())){
            //开始发送数据
            writeData();
            //执行超时操作
            mTimeoutRunnable = new Runnable() {
                @Override
                public void run() {
                    if(!mChain.isCanceled()){
                        onState(IStateCallback.CODE_TIMEOUT, "超时");
                    }
                }
            };
            timeoutExecute(mTimeoutRunnable, mWriteTimeout);
            //并行启动下一个接收拦截器
            mChain.process(null);
        }else {
            onState(IStateCallback.CODE_FAILURE, "获取写数据通道失败");
        }
    }

    /**
     * 解析提取服务中写特征通道
     * @param serviceUUID
     * @param characteristicUUID
     * @return
     */
    private boolean extractWriteCharacteristic(UUID serviceUUID, UUID characteristicUUID) {
        Logdeal.D(TAG, "extractWriteCharacteristic....");
        BluetoothGattService gattService = mDelegate.getGattService(serviceUUID);
        if (gattService != null) {
            mWriteCharacteristic = gattService.getCharacteristic(characteristicUUID);
        }
        return mWriteCharacteristic != null;
    }

    /**********************************信号值读取****************************/
    @Override
    public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
        Logdeal.D(TAG, "onReadRemoteRssi...rssi:" + rssi + ",status:" + status);
    }

    /*********************************数据读写********************************/

    @Override
    public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
        Logdeal.D(TAG, "onCharacteristicRead...status:" + status);
        if (mIDataHandler != null) {
            mIDataHandler.handleDataRead(gatt.getDevice().getName(), characteristic.getValue(), status);
        }
    }

    @Override
    public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
        Logdeal.D(TAG, "onCharacteristicWrite...status:" + status);
        if (status == 0) {
            final int writeDelay = mDataPacket.getFrameDelay();
            timeoutExecute(new Runnable() {
                @Override
                public void run() {
                    writeData();
                }
            }, writeDelay);
        }
        if (mIDataHandler != null) {
            mIDataHandler.handleDateWrite(gatt.getDevice().getName(), status);
        }
    }

    /**
     * 写数据
     */
    private void writeData(){
        if(mDataPacket != null && mWriteCharacteristic != null){
            byte[] frame = mDataPacket.getNextFrame();
            //写下一帧数据
            if (frame != null) {
                mWriteCharacteristic.setValue(frame);
                boolean result = mDelegate.writeCharacteristic(mWriteCharacteristic);
                if (result)
                    Logdeal.D(TAG, " writeData is success length:" + mWriteCharacteristic.getValue().length);
            } else {
                //设置发送数据成功状态
                onState(IStateCallback.CODE_SUCCESS, "");
                //取消超时操作
                cancelExecute(mTimeoutRunnable);
                //回调上层
                if (mIDataHandler != null) {
                    mIDataHandler.handleSendDataFinish(mChain.request().getSsid(), mDataPacket);
                }
            }
        }
    }
    /**********************************数据通知*******************************/
    @Override
    public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
        //no-op
    }


    @Override
    public void cancel() {
        super.cancel();
        //注销回调
        mDelegate.setOnDataAvailableListener(null);
    }
}
