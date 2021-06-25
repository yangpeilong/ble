/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.qdingnet.opendoor.core.bluetooth;

import android.annotation.TargetApi;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.os.Build;

import com.qdingnet.opendoor.core.bluetooth.scan.AbstractBlueScanner;
import com.qdingnet.opendoor.core.bluetooth.scan.BlueScanForJellyBeanMR2;
import com.qdingnet.opendoor.core.interceptor.IDelegateClass;
import com.qdingnet.opendoor.log.Logdeal;

import java.lang.reflect.Method;
import java.util.List;
import java.util.UUID;

/**
 * Service for managing connection and data communication with a GATT server hosted on a
 * given Bluetooth LE device.
 */
@TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
public class BluetoothLeClass implements IDelegateClass{
    private final static String TAG = "BluetoothLeClass";

    private BluetoothManager mBluetoothManager;
    private BluetoothAdapter mBluetoothAdapter;
    private String mBluetoothDeviceAddress;
    private BluetoothGatt mBluetoothGatt;
    private int mErrorStatus = 0;
    private String originBLEName;
    private boolean isDestory = false;

    public interface OnConnectStateListener{
        void onConnected(BluetoothGatt gatt);
        void onDisconnected(BluetoothGatt gatt);
        void onFailureState(BluetoothGatt gatt, int state);
    }

    public interface OnServiceDiscoverListener {
        public void onServiceDiscover(BluetoothGatt gatt);

        public void onSerivceError(int status);
    }

    public interface OnDataAvailableListener extends OnDataNotifyListener{
        public void onCharacteristicRead(BluetoothGatt gatt,
                                         BluetoothGattCharacteristic characteristic,
                                         int status);

        public void onCharacteristicWrite(BluetoothGatt gatt,
                                          BluetoothGattCharacteristic characteristic,
                                          int status);

        public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status);
    }

    public interface OnDataNotifyListener{
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic);
    }

    private OnConnectStateListener mOnConnectStateListener;
    private OnServiceDiscoverListener mOnServiceDiscoverListener;
    private OnDataAvailableListener mOnDataAvailableListener;
    private OnDataNotifyListener mOnDataNotifyListener;
    private Context mContext;

    public void setOnConnectStateListener(OnConnectStateListener listener) {
        this.mOnConnectStateListener = listener;
    }

    public void setOnServiceDiscoverListener(OnServiceDiscoverListener l) {
        mOnServiceDiscoverListener = l;
    }

    public void setOnDataAvailableListener(OnDataAvailableListener l) {
        mOnDataAvailableListener = l;
    }

    public void setOnDataNotifyListener(OnDataNotifyListener l){
        mOnDataNotifyListener = l;
    }

    public BluetoothLeClass(Context c) {
        mContext = c;
        initScanBluethoothCallback();
    }

    /**
     * 获取应用上下文
     * @return
     */
    public Context getContext(){
        return mContext;
    }

    //扫描器
    private AbstractBlueScanner mAbstractBlueScanner;

    public void setOnLeScanListener(AbstractBlueScanner.OnLeScanListener mOnLeScanListener) {
        if (mAbstractBlueScanner != null){
            mAbstractBlueScanner.setOnLeScanListener(mOnLeScanListener);
        }
    }

    private void initScanBluethoothCallback() {
        /*if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            mAbstractBlueScanner = new BlueScanForLollipop();
        } else {
            mAbstractBlueScanner = new BlueScanForJellyBeanMR2();
        }*/
        if (mAbstractBlueScanner == null) {
            mAbstractBlueScanner = new BlueScanForJellyBeanMR2();
        }
    }

    // Implements callback methods for GATT events that the app cares about.  For example,
    // connection change and services discovered.
    private final BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            Logdeal.D(TAG, "onConnectionStateChange. status:" + status + " newState:" + newState + " timestamp: " + String.valueOf(System.currentTimeMillis()));
            if(status == BluetoothGatt.GATT_SUCCESS){
                if (newState == BluetoothProfile.STATE_CONNECTED) {
                    Logdeal.D(TAG, "Connected to GATT server.");
                    if(mOnConnectStateListener != null){
                        mOnConnectStateListener.onConnected(gatt);
                    }
                }else {
                    Logdeal.D(TAG, "Disconnected from GATT server.");
                    if(mOnConnectStateListener != null){
                        mOnConnectStateListener.onDisconnected(gatt);
                    }
                }
            }else {
                if(mOnConnectStateListener != null){
                    mOnConnectStateListener.onFailureState(gatt, status);
                }
                mErrorStatus = status;
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            Logdeal.D(TAG, "onServicesDiscovered status: " + status);
            if (status == BluetoothGatt.GATT_SUCCESS) {
                if (mOnServiceDiscoverListener != null)
                    mOnServiceDiscoverListener.onServiceDiscover(gatt);
            } else {
                mErrorStatus = status;
                Logdeal.D(TAG, "onServicesDiscovered received: " + mErrorStatus);
                try {
                    Thread.sleep(200);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                discoverServices();
            }
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt,
                                         BluetoothGattCharacteristic characteristic,
                                         int status) {
            if (mOnDataAvailableListener != null)
                mOnDataAvailableListener.onCharacteristicRead(gatt, characteristic, status);
        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt,
                                          BluetoothGattCharacteristic characteristic,
                                          int status) {
            if (mOnDataAvailableListener != null)
                mOnDataAvailableListener.onCharacteristicWrite(gatt, characteristic, status);
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt,
                                            BluetoothGattCharacteristic characteristic) {
            if (mOnDataAvailableListener != null)
                mOnDataAvailableListener.onCharacteristicChanged(gatt, characteristic);

            if (mOnDataNotifyListener != null){
                mOnDataNotifyListener.onCharacteristicChanged(gatt, characteristic);
            }
        }

        public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
            if (mOnDataAvailableListener != null)
                mOnDataAvailableListener.onReadRemoteRssi(gatt, rssi, status);
        }

        public void onDescriptorRead(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {

        }

        ;
    };

    /**
     * Initializes a reference to the local Bluetooth adapter.
     *
     * @return Return true if the initialization is successful.
     */
    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
    public boolean initialize() {
        // For API level 18 and above, get a reference to BluetoothAdapter through
        // BluetoothManager.
        mErrorStatus = 0;
        isDestory = false;
        if (mBluetoothManager == null) {
            mBluetoothManager = (BluetoothManager) mContext.getSystemService(Context.BLUETOOTH_SERVICE);
            if (mBluetoothManager == null) {
                Logdeal.D(TAG, "Unable to initialize BluetoothManager.");
                return false;
            }
        }
        if (mBluetoothAdapter == null) {
            mBluetoothAdapter = mBluetoothManager.getAdapter();
            if (mBluetoothAdapter == null) {
                Logdeal.D(TAG, "Unable to obtain a BluetoothAdapter.");
                return false;
            }
        }
        originBLEName = mBluetoothAdapter.getName();
        return true;
    }

    public int getErrorStatus() {
        return mErrorStatus;
    }

    /**
     * Connects to the GATT server hosted on the Bluetooth LE device.
     *
     * @param address The device address of the destination device.
     * @return Return true if the connection is initiated successfully. The connection result
     * is reported asynchronously through the
     * {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
     * callback.
     */
    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
    public boolean connect(final String address) {
        Logdeal.D(TAG, "connect connect .... " + address + " Gatt " + mBluetoothGatt);
        if (mBluetoothAdapter == null || address == null) {
            Logdeal.D(TAG, "BluetoothAdapter not initialized or unspecified address.");
            return false;
        }

        // Previously connected device.  Try to reconnect.
        if (address.equals(mBluetoothDeviceAddress) && mBluetoothGatt != null) {
            Logdeal.D(TAG, "Trying to use an existing mBluetoothGatt for connection.");
            return mBluetoothGatt.connect();
        }

        final BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
        if (device == null) {
            Logdeal.D(TAG, "Device not found.  Unable to connect.");
            return false;
        }
        try {
            mBluetoothGatt = device.connectGatt(mContext, false, mGattCallback);
            Logdeal.D(TAG, "Trying to create a new connection." + mBluetoothGatt);
        } catch (Exception e) {
            Logdeal.D(TAG, " connectOnMainThread ...Exception:", e);
        }
        mBluetoothDeviceAddress = address;
        return mBluetoothGatt != null;
    }

    /**
     * 重新连接
     */
    public boolean reconnect() {
        boolean connect = false;
        try {
            Logdeal.D(TAG, "reconnect...");
            connect = mBluetoothGatt != null && mBluetoothGatt.connect();
        } catch (Exception e) {
            Logdeal.D(TAG, " reconnect ...Exception:", e);
        }
        return connect;
    }

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
    public void discoverServices() {
        try {
            if (mBluetoothGatt != null) {
                Logdeal.D(TAG, "Attempting to start service discovery:" + mBluetoothGatt.discoverServices());
            }
        } catch (Exception e) {
            Logdeal.D(TAG, " discoverServices...Exception:", e);
        }
    }

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
    public void readRemoteRssi() {
        try {
            if (mBluetoothGatt != null) {
                Logdeal.D(TAG, "Attempting to start readRemoteRssi :" + mBluetoothGatt.readRemoteRssi());
            }
        } catch (Exception e) {
            Logdeal.D(TAG, " readRemoteRssi...Exception:", e);
        }
    }

    /**
     * Disconnects an existing connection or cancel a pending connection. The disconnection result
     * is reported asynchronously through the
     * {@code BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)}
     * callback.
     */
    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
    public void disconnect() {
        try {
            if (mBluetoothGatt != null) {
                mBluetoothGatt.disconnect();
            }
            mBluetoothDeviceAddress = null;
            Logdeal.D(TAG, "GATT disconnect");
        } catch (Exception e) {
            Logdeal.D(TAG, " disconnect...Exception:", e);
        }
    }

    /**
     * clear cache and refresh
     * @return
     */
    public boolean refresh(){
        boolean result = false;
        if(mBluetoothGatt != null){
            try {
                Class clazz = BluetoothGatt.class;
                Method method = clazz.getMethod("refresh");
                Object object = method.invoke(mBluetoothGatt);
                if(object != null && object instanceof Boolean){
                    result = (Boolean) object;
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        Logdeal.D(TAG, "refresh...result:" + result);
        return result;
    }


    /**
     * After using a given BLE device, the app must call this method to ensure resources are
     * released properly.
     */
    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
    public void close() {
        try {
            refresh();
            if (mBluetoothGatt != null) {
                mBluetoothGatt.close();
                mBluetoothGatt = null;
            }
            mBluetoothDeviceAddress = null;
            Logdeal.D(TAG, "GATT close");
        } catch (Exception e) {
            Logdeal.D(TAG, " close...Exception:", e);
        }
    }

    public void destory() {
        stopLeScan();
        close();
        isDestory = true;
    }

    public boolean isDestory() {
        return isDestory;
    }

    /**
     * Request a read on a given {@code BluetoothGattCharacteristic}. The read result is reported
     * asynchronously through the {@code BluetoothGattCallback#onCharacteristicRead(android.bluetooth.BluetoothGatt, android.bluetooth.BluetoothGattCharacteristic, int)}
     * callback.
     *
     * @param characteristic The characteristic to read from.
     */
    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
    public boolean readCharacteristic(BluetoothGattCharacteristic characteristic) {
        try {
            if (mBluetoothGatt != null) {
                return mBluetoothGatt.readCharacteristic(characteristic);
            }
        } catch (Exception e) {
            Logdeal.D(TAG, " readCharacteristic...Exception:", e);
        }
        return false;
    }


    /**
     * Enables or disables notification on a give characteristic.
     *
     * @param characteristic Characteristic to act on.
     * @param enabled        If true, enable notification. False otherwise.
     */
    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
    public boolean setCharacteristicNotification(
            BluetoothGattCharacteristic characteristic, boolean enabled) {
        try {
            if (mBluetoothGatt != null) {
                return mBluetoothGatt.setCharacteristicNotification(characteristic, enabled);
            }
        } catch (Exception e) {
            Logdeal.D(TAG, " setCharacteristicNotification...Exception:", e);
        }
        return false;
    }

    /**
     * write descriptor
     *
     * @param descriptor
     * @return
     */
    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
    public boolean writeDescriptor(BluetoothGattDescriptor descriptor) {
        try {
            if (mBluetoothGatt != null) {
                return mBluetoothGatt.writeDescriptor(descriptor);
            }
        } catch (Exception e) {
            Logdeal.D(TAG, " writeDescriptor...Exception:", e);
        }
        return false;
    }

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
    public boolean writeCharacteristic(BluetoothGattCharacteristic characteristic) {
        try {
            if (mBluetoothGatt != null)
                return mBluetoothGatt.writeCharacteristic(characteristic);
        } catch (Exception e) {
            Logdeal.D(TAG, " writeCharacteristic...Exception:", e);
        }
        return false;
    }

    /**
     * Retrieves a list of supported GATT services on the connected device. This should be
     * invoked only after {@code BluetoothGatt#discoverServices()} completes successfully.
     *
     * @return A {@code List} of supported services.
     */
    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
    public List<BluetoothGattService> getSupportedGattServices() {
        if (mBluetoothGatt == null) return null;
        return mBluetoothGatt.getServices();
    }

    /**
     * Retrieves service according to uuid
     * @param uuid
     * @return
     */
    public BluetoothGattService getGattService(UUID uuid){
        if (mBluetoothGatt == null) return null;
        return mBluetoothGatt.getService(uuid);
    }

    public void stopLeScan() {
        Logdeal.D(TAG, " stopLeScan...");
        if (mAbstractBlueScanner != null) {
            mAbstractBlueScanner.stopScan(mBluetoothAdapter);
        }
    }

    public boolean startLeScan(UUID[] uuids,int reportDelay) {
        boolean result = true;
        Logdeal.D(TAG, " startLeScan..." + uuids);
        if (mAbstractBlueScanner != null) {
            result = mAbstractBlueScanner.startScan(mBluetoothAdapter, null, reportDelay);
        }
        return result;
    }

    /**
     * 判断蓝牙是否打开
     *
     * @return
     */
    public boolean isEnabled() {
        try {
            if (mBluetoothAdapter != null) {
                return mBluetoothAdapter.getState() == BluetoothAdapter.STATE_ON;
            }
        } catch (Exception e) {
            Logdeal.D(TAG, " isEnabled...Exception:", e);
        }
        return false;
    }

    /**
     * 打开蓝牙按钮
     *
     * @return
     */
    public boolean enable() {
        try {
            if (mBluetoothAdapter != null) {
                return mBluetoothAdapter.enable();
            }
        } catch (Exception e) {
            Logdeal.D(TAG, " enable...Exception:", e);
        }
        return false;
    }

    /**
     * 关闭蓝牙按钮
     *
     * @return
     */
    public boolean disable() {
        try {
            if (mBluetoothAdapter != null) {
                return mBluetoothAdapter.disable();
            }
        } catch (Exception e) {
            Logdeal.D(TAG, " isEnabled...Exception:", e);
        }
        return false;
    }

    /**
     * 设置蓝牙名称
     *
     * @param name
     * @return
     */
    public boolean setBluetoothName(String name) {
        try {
            if (mBluetoothAdapter != null) {
                return mBluetoothAdapter.setName(name);
            }
        } catch (Exception e) {
            Logdeal.D(TAG, " setBluetoothName...Exception:", e);
        }
        return false;
    }

    public void resetBluetoothName() {
        if (originBLEName != null) {
            setBluetoothName(originBLEName);
        }
    }

    /**
     * 获取蓝牙物理地址mac
     *
     * @return
     */
    public String getBluetoothAddress() {
        try {
            if (mBluetoothAdapter != null) {
                return mBluetoothAdapter.getAddress();
            }
        } catch (Exception e) {
            Logdeal.D(TAG, " setBluetoothName...Exception:", e);
        }
        return null;
    }


}
