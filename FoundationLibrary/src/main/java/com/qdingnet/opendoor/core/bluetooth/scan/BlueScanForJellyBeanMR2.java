package com.qdingnet.opendoor.core.bluetooth.scan;

import android.annotation.TargetApi;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.os.Build;

import com.qdingnet.opendoor.log.Logdeal;

import java.util.UUID;

/**
 * android version >=4.3 && < 5.0 使用的蓝牙扫描监听器
 * Created by pipan on 16-5-3.
 */
@TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
public class BlueScanForJellyBeanMR2 extends AbstractBlueScanner {
    private final static String TAG = "BlueScanForJellyBeanMR2";

    private BluetoothAdapter.LeScanCallback mLeScanCallback = new BluetoothAdapter.LeScanCallback() {
        @Override
        public void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord) {
            Logdeal.D(TAG, "...onLeScan...device:" + device.getName() + " rssi == " + rssi);
            if (mReportDelayScanListener != null) {
                ScanRecord record = ScanRecord.parseFromBytes(scanRecord);
                mReportDelayScanListener.onScanResult(device, record != null ? record.getDeviceName() : null, rssi, scanRecord);
            }
        }
    };

    @Override
    public boolean startScan(BluetoothAdapter adapter, UUID[] uuids,int reportDelay) {
        scanResults = null;
        mReportDelay = reportDelay;
        boolean result = false;
        try {
            if (adapter != null) {
                result = adapter.startLeScan(uuids, mLeScanCallback);
            } else {
                Logdeal.D(TAG, "startScan...BluetoothAdapter is null");
            }
        } catch (Exception e) {
            Logdeal.D(TAG, "startScan...Exception:", e);
        }
        return result;
    }

    @Override
    public void stopScan(BluetoothAdapter adapter) {
        try {
            if (adapter != null) {
                adapter.stopLeScan(mLeScanCallback);
            } else {
                Logdeal.D(TAG, "stopScan...BluetoothAdapter is null");
            }
        } catch (Exception e) {
            Logdeal.D(TAG, "stopScan...Exception:", e);
        }
    }

}
