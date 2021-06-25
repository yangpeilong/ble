package com.qding.config.blelib.blescan;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Base64;
import android.util.Log;

import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * 通用蓝牙扫描器
 * @author pipan (pipan@qding.me)
 * @date   19-3-4
 */
public class QDBluetoothScanner implements Handler.Callback {
    private final static String TAG = "QC202/QDBluetoothScanner";

    private final static int MSG_SCAN_TIMEOUT = 9090;
    private final static int DEFAULT_SCAN_TIMEOUT = 2 * 1000;
    private static QDBluetoothScanner instance;
    private BluetoothAdapter mBluetoothAdapter;
    private Handler mHandler;
    private AbstractBlueScanner scanner;
    private OnScanListener mOnScanListener;
    private int mScanTimeout;
    private final AtomicBoolean isRegisterReceiver = new AtomicBoolean(false);
    private final AtomicBoolean mScanMark = new AtomicBoolean(false);
    private List<ScanResult> mScanResults = Collections.synchronizedList(new LinkedList<ScanResult>());

    private QDBluetoothScanner() {
        build();
    }

    public static synchronized QDBluetoothScanner getInstance() {
        if (instance == null) {
            synchronized (QDBluetoothScanner.class) {
                if (instance == null) {
                    instance = new QDBluetoothScanner();
                }
            }
        }
        return instance;
    }

    private void build() {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        mHandler = new Handler(Looper.getMainLooper(), this);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            scanner = new BlueScanForLollipop();
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
            scanner = new BlueScanForJellyBeanMR2();
        } else {
            scanner = null;
        }
    }

    private boolean startScan(final UUID[] uuids) {
        if (scanner == null) {
            if (mOnScanListener != null) {
                mOnScanListener.onScanStop();
                mOnScanListener = null;
            }
            return false;
        }
        //根据扫描标志判断是否在扫描
        if(mScanMark.compareAndSet(false, true)){
            mHandler.sendEmptyMessageDelayed(MSG_SCAN_TIMEOUT, mScanTimeout);
            mScanResults.clear();
            scanner.setOnLeScanListener(new AbstractBlueScanner.OnLeScanListener() {
                @Override
                public void onScanResult(BluetoothDevice device, String name, int rssi, byte[] scanRecord) {
                    Log.d(TAG, "onScanResult...name:" + name + ",rssi:" + rssi);
                    String mac = getBluedeviceMac(name);
                    if (mac != null) {
                        final ScanResult result = new ScanResult(mac, rssi, device.getAddress());
                        mHandler.post(new Runnable() {
                            @Override
                            public void run() {
                                if (mOnScanListener != null) {
                                    mOnScanListener.onScanResult(result);
                                }
                            }
                        });
                    }
                }
            });
            return scanner.startScan(mBluetoothAdapter, uuids, 0);
        }
        return true;
    }

    /**
     * scan bluetooth for 2000 milliseconds
     *
     * @param listener
     * @return
     */
    public boolean startScan(Context context, OnScanListener listener) {
        return startScan(context, DEFAULT_SCAN_TIMEOUT, listener);
    }

    /**
     * scan bluetooth for <param>timeout</param> milliseconds
     *
     * @param timeout
     * @param listener
     * @return
     */
    public boolean startScan(Context context, int timeout, OnScanListener listener) {
        if (mBluetoothAdapter == null) {
            Log.d(TAG, "startScan fail,mBluetoothAdapter == null");
            return false;
        }
        mOnScanListener = listener;
        mScanTimeout = timeout > 0 ? timeout : DEFAULT_SCAN_TIMEOUT;
        if (!mBluetoothAdapter.isEnabled()) {
            if(mBluetoothAdapter.enable()){
                registerBluetoothReceiver(context);
                Log.d(TAG, "bluetooth is not enable,enable now!");
            }else {
                if(listener != null) listener.onScanStop();
            }
            return true;
        } else {
            startScan((UUID[]) null);
        }
        return true;
    }

    /**
     * 注册蓝牙广播器
     * @param context
     */
    private void registerBluetoothReceiver(Context context) {
        if (isRegisterReceiver.compareAndSet(false, true)) {
            IntentFilter filter;
            filter = new IntentFilter(BluetoothDevice.ACTION_ACL_DISCONNECTED);
            filter.addAction(BluetoothDevice.ACTION_ACL_CONNECTED);
            filter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECT_REQUESTED);
            filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
            context.registerReceiver(mBluetoothReceiver, filter);
        }
    }
    /**
     * 注销蓝牙广播器
     * @param context
     */
    private void unregisterBluetoothReceiver(Context context){
        if (isRegisterReceiver.compareAndSet(true, false)) {
            context.unregisterReceiver(mBluetoothReceiver);
        }
    }

    private final BroadcastReceiver mBluetoothReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.d(TAG, "BluetoothReceiver...onReceive...action:" + action);
            if (BluetoothAdapter.ACTION_STATE_CHANGED.equals(action)) {
                final int status = intent.getExtras().getInt(BluetoothAdapter.EXTRA_STATE);
                if (status == BluetoothAdapter.STATE_ON) {
                    Log.d(TAG, "Bluetooth STATE_ON");
                    //启动蓝牙扫描处理
                    startScan((UUID[]) null);
                    //注销蓝牙监听
                    unregisterBluetoothReceiver(context);
                } else if (status == BluetoothAdapter.STATE_OFF) {
                    Log.d(TAG, "Bluetooth STATE_OFF");
                }
            }
        }

    };

    /**
     * stop scan bluetooth
     */
    public void stopScan() {
        Log.d(TAG, "bluetooth stopScan");
        if (scanner != null) {
            scanner.stopScan(mBluetoothAdapter);
            scanner.setOnLeScanListener(null);
        }
        if (mOnScanListener != null) {
            mOnScanListener.onScanStop();
            mOnScanListener = null;
        }
        //移除扫描超时锚
        mHandler.removeMessages(MSG_SCAN_TIMEOUT);
        //复位扫描标志
        mScanMark.set(false);
    }

    public interface OnScanListener {
        void onScanResult(ScanResult result);

        void onScanStop();
    }

    @Override
    public boolean handleMessage(Message msg) {
        switch (msg.what) {
            case MSG_SCAN_TIMEOUT:
                stopScan();
                break;
        }
        return false;
    }

    /**
     * 筛选符合QD/QD_的设备
     *
     * @param ssid
     * @return
     */
    private String getBluedeviceMac(String ssid) {
        String mac = null;
        try {
            final String name = ssid;
            if (name != null) {
                mac = name ;
            }
        } catch (Exception e) {
            Log.d(TAG, "getBluedeviceMac...Exception:", e);
        }
        return mac;
    }
}
