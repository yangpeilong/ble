package com.qdingnet.opendoor.core.bluetooth.scan;

import android.annotation.TargetApi;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.os.Build;
import android.os.ParcelUuid;

import com.qdingnet.opendoor.log.Logdeal;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import static android.bluetooth.le.ScanSettings.SCAN_MODE_BALANCED;

/**
 * android version >=5.0 使用的蓝牙扫描监听器
 * Created by pipan on 16-5-3.
 */
@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class BlueScanForLollipop extends AbstractBlueScanner {
    private final static String TAG = "BlueScanForLollipop";

    private ScanCallback mScanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            if (result != null && result.getDevice() != null && result.getScanRecord() != null) {
                if (mReportDelayScanListener != null) {
                    mReportDelayScanListener.onScanResult(result.getDevice(), result.getScanRecord().getDeviceName(), result.getRssi(), result.getScanRecord().getBytes());
                }
            }
        }
        @Override
        public void onScanFailed(int errorCode) {
            super.onScanFailed(errorCode);
            Logdeal.D(TAG, "...onScanFailed...errorCode:" + errorCode);
        }
    };

    @Override
    public boolean startScan(BluetoothAdapter adapter, UUID[] uuids, int reportDelay) {
        scanResults = null;
        mReportDelay = reportDelay;
        boolean result = false;
        try {
            if (adapter != null) {
                BluetoothLeScanner scanner = adapter.getBluetoothLeScanner();
                if (scanner != null) {
                    List<ScanFilter> filters = null;
                    if (uuids != null && uuids.length > 0) {
                        filters = new ArrayList<ScanFilter>();
                        for (UUID uuid : uuids) {
                            ScanFilter filter = new ScanFilter.Builder()
                                    .setServiceUuid(new ParcelUuid(uuid))
                                    .build();
                            filters.add(filter);
                        }
                    }
                    ScanSettings scanSettings = new ScanSettings.Builder()
                            .setScanMode(SCAN_MODE_BALANCED)
                            .setReportDelay(0)
                            .build();
                    scanner.startScan(filters, scanSettings, mScanCallback);
                    result = true;
                }
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
                BluetoothLeScanner scanner = adapter.getBluetoothLeScanner();
                if (scanner != null)
                    scanner.stopScan(mScanCallback);
            } else {
                Logdeal.D(TAG, "stopScan...BluetoothAdapter is null");
            }
        } catch (Exception e) {
            Logdeal.D(TAG, "stopScan...Exception:", e);
        }
    }
}
