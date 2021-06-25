package com.qdingnet.opendoor.core.bluetooth.advertise;

import android.annotation.TargetApi;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.le.AdvertiseCallback;
import android.bluetooth.le.AdvertiseData;
import android.bluetooth.le.AdvertiseSettings;
import android.bluetooth.le.BluetoothLeAdvertiser;
import android.os.Build;
import android.os.ParcelUuid;

import com.qdingnet.opendoor.log.Logdeal;

import java.util.Map;

/**
 * Created by pipan on 16-5-18.
 */
public class BluetoothAdvertiser {
    private final static String TAG = "QC202/BluetoothAdvertiser";

    private final static int ADVERTISER_TIMEOUT = 5 * 1000;

    private Object mAdvertiserCallback;

    public boolean supportAdvertiser() {
        return getAdvertiser() != null;
    }

    private BluetoothLeAdvertiser getAdvertiser() {
        BluetoothLeAdvertiser advertiser = null;
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            advertiser = adapter.getBluetoothLeAdvertiser();
        }
        return advertiser;
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private AdvertiseCallback getAdveriseCallback() {
        if (mAdvertiserCallback == null) {
            mAdvertiserCallback = new AdvertiseCallback() {
                @Override
                public void onStartFailure(int errorCode) {
                    super.onStartFailure(errorCode);
                    if (mOnAdvertiserCallback != null) {
                        mOnAdvertiserCallback.onFailure();
                    }
                    Logdeal.D(TAG, "onStartFailure...errorCode:" + errorCode);
                }

                @Override
                public void onStartSuccess(AdvertiseSettings settingsInEffect) {
                    super.onStartSuccess(settingsInEffect);
                    if (mOnAdvertiserCallback != null) {
                        mOnAdvertiserCallback.onSuccess();
                    }
                    Logdeal.D(TAG, "onStartSuccess...");
                }
            };
        }
        return (AdvertiseCallback) mAdvertiserCallback;
    }

    public void startAdertiser(boolean includeName){
        startAdvertiser(includeName, ADVERTISER_TIMEOUT, null);
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public void startAdvertiser(boolean includeName, int timeout, Map<String, byte[]> datas) {
        Logdeal.D(TAG, "startAdvertiser...");
        BluetoothLeAdvertiser advertiser = getAdvertiser();
        if (advertiser != null) {
            AdvertiseSettings settings = new AdvertiseSettings.Builder()
                    .setTimeout(ADVERTISER_TIMEOUT)
                    .setAdvertiseMode(AdvertiseSettings.ADVERTISE_MODE_LOW_LATENCY)
                    .setConnectable(true)
                    .setTxPowerLevel(AdvertiseSettings.ADVERTISE_TX_POWER_MEDIUM)
                    .build();
            AdvertiseData.Builder builder = new AdvertiseData.Builder();
            if (datas != null) {
                for (String uuid : datas.keySet()) {
                    builder.addServiceData(ParcelUuid.fromString(uuid), datas.get(uuid));
                }
            }
            builder.setIncludeDeviceName(includeName);
            AdvertiseData data = builder.build();
            Logdeal.D(TAG, "startAdvertiser...uuid:" + data.getServiceUuids());
            advertiser.startAdvertising(settings, data, getAdveriseCallback());
        } else {
            Logdeal.D(TAG, "startAdvertiser...Not support Advertise");
            if(mOnAdvertiserCallback != null){
                mOnAdvertiserCallback.onFailure();
            }
        }
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public void stopAdvertiser() {
        Logdeal.D(TAG, "stopAdvertiser...");
        BluetoothLeAdvertiser advertiser = getAdvertiser();
        if (advertiser != null && mAdvertiserCallback != null) {
            advertiser.stopAdvertising(getAdveriseCallback());
        }
    }


    public OnAdvertiserCallback mOnAdvertiserCallback;

    public void setOnAdvertiserCallback(OnAdvertiserCallback l) {
        this.mOnAdvertiserCallback = l;
    }

    public static interface OnAdvertiserCallback {
        void onFailure();

        void onSuccess();
    }

}
