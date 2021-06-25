package com.qdingnet.opendoor.core.interceptor.bluetooth;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

import androidx.annotation.NonNull;

import com.qdingnet.opendoor.core.bluetooth.BluetoothLeClass;
import com.qdingnet.opendoor.core.bluetooth.scan.AbstractBlueScanner;
import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.opendoor.core.handler.IAdvertiseHandler;
import com.qdingnet.opendoor.core.handler.IAuthHandler;
import com.qdingnet.opendoor.core.interceptor.AbstractInterceptor;
import com.qdingnet.opendoor.core.request.Request;
import com.qdingnet.opendoor.log.Logdeal;

import java.util.UUID;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * 蓝牙扫描拦截类
 * @author pipan (pipan@qding.me)
 * @date   19-1-15 
 */
public class BluetoothScanInterceptor extends AbstractInterceptor<BluetoothLeClass> implements AbstractBlueScanner.OnLeScanListener{
    private static final String TAG = "BLEScanInterceptor";
    //扫描超时时间
    private static final int SCAN_TIMEOUT = 5_000;

    //扫描过滤的uuid
    private final UUID[] filterUUIDS;
    //扫描结果延时时间(ms)
    private final int reportDelay;
    //是否启动连接设备
    private final boolean mEnableConnect;
    //是否已扫描到设备标志
    private final AtomicBoolean mDeviceScanned = new AtomicBoolean(false);
    //设备验证授权处理器
    private IAuthHandler mIAuthHandler;
    //广播数据处理接口
    private IAdvertiseHandler mIAdvertiseHandler;
    //服务发现超时操作实例
    private Runnable mTimeoutRunnable;
    //蓝牙广播注册器
    private BluetoothReceiver mBluetoothReceiver;
    //蓝牙广播注册标志
    private final AtomicBoolean mBluetoothReceiveMark = new AtomicBoolean(false);
    //蓝牙扫描标志
    private boolean mScanning = false;

    public BluetoothScanInterceptor(@NonNull BluetoothLeClass bluetoothLeClass, UUID[] filterUUIDS, int reportDelay, boolean enableConnect, @NonNull IAuthHandler authHandler, IAdvertiseHandler advertiseHandler) {
        super(bluetoothLeClass, IStateCallback.STATE_SCAN);
        this.filterUUIDS = filterUUIDS;
        this.reportDelay = reportDelay;
        this.mEnableConnect = enableConnect;
        this.mIAuthHandler = authHandler;
        this.mIAdvertiseHandler = advertiseHandler;
    }

    @Override
    public void onScanResult(BluetoothDevice device, String name, int rssi, byte[] scanRecord) {
        String deviceName = device.getName();
        Logdeal.D(TAG, "onScanResult...deviceName:" + deviceName + ", name:" + name + ", rssi:" + rssi);
        //mac校验，信号值比较
        Request request = mChain.request();
        if(mIAuthHandler.onAuth(deviceName, rssi)){
            //设备是否已扫描到
            if(mDeviceScanned.compareAndSet(false, true)){
                //设置需操作的设备ssid和address
                request.setSsid(deviceName);
                request.setAddress(device.getAddress());
                //执行正常流程操作下一步
                process(request);
                //设置扫描成功状态
                onState(IStateCallback.CODE_SUCCESS, "");
            }
            //解析广播开门结果
            else {
                if(mIAdvertiseHandler == null || mIAdvertiseHandler.analysisAdvertiseData(mDelegate, getDeviceName(device, name), scanRecord)){
                    //取消超时操作
                    cancelExecute(mTimeoutRunnable);
                    //停止扫描
                    stopScan();
                }
            }
        }
    }

    @Override
    public void intercept(Chain next) {
        super.intercept(next);
        //蓝牙是否已启动
        if(mDelegate.isEnabled()){
            startScan();
        }
        //启动蓝牙
        else {
            //注册广播接收器
            registerBluetoothReceiver();
            //启动蓝牙
            boolean enable = mDelegate.enable();
            Logdeal.D(TAG, "Bluetooth enable:" + enable);
            //启动失败
            if(!enable){
                unregisterBluetoothReceiver();
                onState(IStateCallback.CODE_FAILURE, "蓝牙启动失败");
                return;
            }
        }
        //执行超时操作
        mTimeoutRunnable = new Runnable() {
            @Override
            public void run() {
                //停止扫描
                stopScan();
                //回调上层
                if((!mDeviceScanned.get() || !mEnableConnect) && !mChain.isCanceled()){
                    onState(IStateCallback.CODE_TIMEOUT, "超时");
                }
            }
        };
        timeoutExecute(mTimeoutRunnable, SCAN_TIMEOUT);
    }

    /**
     * 开始扫描
     */
    private void startScan(){
        //扫描状态标志
        if(mScanning) return;
        mScanning = true;
        //启动扫描
        mDelegate.startLeScan(filterUUIDS, reportDelay);
        mDelegate.setOnLeScanListener(this);
    }

    /**
     * 停止扫描
     */
    private void stopScan(){
        //注销回调
        mDelegate.setOnLeScanListener(null);
        //注销监听器
        unregisterBluetoothReceiver();
        //停止扫描
        mDelegate.stopLeScan();
    }

    /**
     * 获取蓝牙设备名
     * @param device
     * @param deviceName
     * @return
     */
    private String getDeviceName(BluetoothDevice device, String deviceName){
        String name = device.getName();
        if (deviceName != null) {
            Pattern pattern = Pattern.compile("[_\\w\\d/\\+=]+");
            Matcher matcher = pattern.matcher(deviceName);
            if (matcher.find()) {
                name = matcher.group();
            }
        }
        return name;
    }

    @Override
    public void cancel() {
        super.cancel();
        //停止扫描
        stopScan();
    }

    /**
     * 注册蓝牙状态广播器
     */
    private void registerBluetoothReceiver(){
        if(mBluetoothReceiveMark.compareAndSet(false, true)){
            mBluetoothReceiver = new BluetoothReceiver();
            IntentFilter filter = new IntentFilter();
            filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
            Context context = mDelegate.getContext();
            if(context != null){
                context.registerReceiver(mBluetoothReceiver, filter);
            }
        }
    }

    /**
     * 注销蓝牙状态广播器
     */
    private void unregisterBluetoothReceiver(){
        if(mBluetoothReceiveMark.compareAndSet(true, false)){
            Context context = mDelegate.getContext();
            if(context != null){
                context.unregisterReceiver(mBluetoothReceiver);
            }
        }
    }


    /**
     * 蓝牙设备广播接收器
     */
    final class BluetoothReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Logdeal.D(TAG, "BluetoothReceiver...onReceive...action:" + action);
            if (BluetoothAdapter.ACTION_STATE_CHANGED.equals(action)) {
                final int status = intent.getExtras().getInt(BluetoothAdapter.EXTRA_STATE);
                if (status == BluetoothAdapter.STATE_ON) {
                    Logdeal.D(TAG, "Bluetooth STATE_ON");
                    //启动蓝牙扫描处理
                    startScan();
                } else if (status == BluetoothAdapter.STATE_OFF) {
                    Logdeal.D(TAG, "Bluetooth STATE_OFF");
                }
            }
        }
    }
}
