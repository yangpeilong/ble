package com.qdingnet.opendoor.core.bluetooth.scan;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import com.qdingnet.opendoor.log.Logdeal;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.UUID;

/**
 * Created by pipan on 16-5-3.
 */
public abstract class AbstractBlueScanner {

    private static final String TAG = "AbstractBlueScanner";
    private static final int MSG_REPORT_SCAN_RESULT = 10001;

    protected List<ScanRes> scanResults;
    protected int mReportDelay;
    private long mScanReportStartTime;

    private OnLeScanListener mOnLeScanListener;

    private Handler mHandler = new Handler(Looper.getMainLooper()) {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_REPORT_SCAN_RESULT) {
                if (!scanResults.isEmpty()) {
                    Collections.sort(scanResults, new Comparator<ScanRes>() {
                        @Override
                        public int compare(ScanRes o1, ScanRes o2) {
                            return o2.rssi - o1.rssi;
                        }
                    });
                    for (ScanRes res : scanResults) {
                        if (mOnLeScanListener != null) {
                            mOnLeScanListener.onScanResult(res.device, res.name, res.rssi, res.scanRecord);
                        }
                    }
                }
            }
        }
    };

    //延时上报回调
    protected OnLeScanListener mReportDelayScanListener = new OnLeScanListener() {
        @Override
        public void onScanResult(BluetoothDevice device, String name, int rssi, byte[] scanRecord) {
            Logdeal.D(TAG, " onScanResult run on main thread " + (Thread.currentThread() == Looper.getMainLooper().getThread()));
            if (mReportDelay == 0) {
                if (mOnLeScanListener != null) {
                    mOnLeScanListener.onScanResult(device, name, rssi, scanRecord);
                }
            } else {
                if (scanResults == null) {
                    mScanReportStartTime = System.currentTimeMillis();
                    scanResults = new ArrayList<>();
                    mHandler.removeMessages(MSG_REPORT_SCAN_RESULT);
                    mHandler.sendEmptyMessageDelayed(MSG_REPORT_SCAN_RESULT, mReportDelay);
                }
                if ((System.currentTimeMillis() - mScanReportStartTime) < mReportDelay) {
                    // 添加到扫描结果list
                    ScanRes scanResult = new ScanRes(device, name, rssi, scanRecord);
                    scanResults.add(scanResult);
                } else {
                    if (mOnLeScanListener != null) {
                        mOnLeScanListener.onScanResult(device, name, rssi, scanRecord);
                    }
                }
            }
        }
    };

    public void setOnLeScanListener(final OnLeScanListener onLeScanListener) {
        this.mOnLeScanListener = onLeScanListener;
        if (mOnLeScanListener == null) {
            mHandler.removeMessages(MSG_REPORT_SCAN_RESULT);
        }
    }

    public boolean startScan(BluetoothAdapter adapter) {
        return startScan(adapter, null, 0);
    }

    public abstract boolean startScan(BluetoothAdapter adapter, UUID[] uuids, int reportDelay);

    public abstract void stopScan(BluetoothAdapter adapter);

    public interface OnLeScanListener {
        public void onScanResult(BluetoothDevice device, String name, int rssi, byte[] scanRecord);
    }

    public static class ScanRes {
        BluetoothDevice device;
        String name;
        int rssi;
        byte[] scanRecord;

        public ScanRes(BluetoothDevice device, String name, int rssi, byte[] scanRecord) {
            this.device = device;
            this.name = name;
            this.rssi = rssi;
            this.scanRecord = scanRecord;
        }
    }

}
