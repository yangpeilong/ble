package com.qding.config.blelib

import android.app.Service
import android.bluetooth.*
import android.content.Intent
import android.os.Binder
import android.os.IBinder
import android.util.Log

/**
 *  Created by yangyeah on 2021/6/18
 *
 *  describe:
 */
class BluetoothLeService : Service() {

    companion object{
        const val TAG = "BluetoothLeService"
        const val STATE_CONNECT = 0
        const val STATE_DISCOVER_SERVICES = 1
        const val STATE_CHARACTERISTIC_READ = 2
        const val STATE_CHARACTERISTIC_WRITE = 3
        const val STATE_CHARACTERISTIC_CHANGED = 4
    }

    private val localBinder by lazy {
        LocalBinder()
    }

    override fun onBind(p0: Intent?): IBinder{
        return localBinder
    }

    var mBluetoothGatt: BluetoothGatt? = null
    private var mBluetoothAdapter: BluetoothAdapter? = null
    private var mBluetoothDeviceAddress: String? = null
    var onBluetoothLEStatusChangedListener:OnBluetoothLEStatusChangedListener? = null
    var onCharacteristicChangedListener: OnCharacteristicChangedListener? = null



    val mGattCallback = object :BluetoothGattCallback(){
        /**
         * 连接状态改变时的回调
         */
        override fun onConnectionStateChange(gatt: BluetoothGatt?, status: Int, newState: Int) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.i(TAG, "BLE Connected!Attempting to start service discovery!")
                //开始扫描ble服务
                mBluetoothGatt?.discoverServices()
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED
                || newState == BluetoothProfile.STATE_DISCONNECTING){
                Log.i(TAG, "Disconnected from GATT server.")
            }
            onBluetoothLEStatusChangedListener?.onBluetoothLEStatusChanged(STATE_CONNECT,newState)
        }

        /**
         * UUID搜索成功回调
         */
        override fun onServicesDiscovered(gatt: BluetoothGatt?, status: Int) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                onBluetoothLEStatusChangedListener?.onBluetoothLEStatusChanged(
                    STATE_DISCOVER_SERVICES, status
                )
            } else {
                Log.w(TAG, "onServicesDiscovered received: $status")
            }
        }

        override fun onCharacteristicRead(
            gatt: BluetoothGatt?,
            characteristic: BluetoothGattCharacteristic?,
            status: Int
        ) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                onCharacteristicChangedListener?.onCharacteristicChanged(
                    STATE_CHARACTERISTIC_READ, characteristic
                )
            } else {
                Log.w(TAG, "onCharacteristicRead received: $status")
            }
        }

        override fun onCharacteristicWrite(
            gatt: BluetoothGatt?,
            characteristic: BluetoothGattCharacteristic?,
            status: Int
        ) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                onCharacteristicChangedListener?.onCharacteristicChanged(
                    STATE_CHARACTERISTIC_WRITE, characteristic
                )
            } else {
                Log.w(TAG, "onCharacteristicWrite received: $status")
            }
        }

        /**
         * 接收数据的回调
         */
        override fun onCharacteristicChanged(
            gatt: BluetoothGatt?,
            characteristic: BluetoothGattCharacteristic?
        ) {
            Log.w(TAG, "onCharacteristicChanged")
            onCharacteristicChangedListener?.onCharacteristicChanged(
                STATE_CHARACTERISTIC_CHANGED, characteristic
            )
        }
    }


    fun getSupportGattServices():List<BluetoothGattService>?{
        if(mBluetoothGatt == null )  return  null
        return mBluetoothGatt!!.services
    }


    fun setCharacteristicNotification(
        characteristic: BluetoothGattCharacteristic,
        enabled: Boolean
    ) {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "setCharacteristicNotification: BluetoothAdapter not initialized" )
            return
        }
        mBluetoothGatt?.setCharacteristicNotification(characteristic,enabled)

    }

    fun readCharacteristic(characteristic: BluetoothGattCharacteristic){
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "readCharacteristic: BluetoothAdapter not initialized" )
            return
        }
        mBluetoothGatt?.readCharacteristic(characteristic)
    }

    fun writeCharacteristic(characteristic: BluetoothGattCharacteristic) {
        Log.i(TAG, "writeCharacteristic: ")
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "writeCharacteristic: BluetoothAdapter not initialized" )
            return
        }
        mBluetoothGatt?.writeCharacteristic(characteristic)
    }


    fun connect(bluetoothAdapter: BluetoothAdapter, address: String):Boolean {
        if (bluetoothAdapter == null || address.isNullOrEmpty()) return  false
        mBluetoothAdapter = bluetoothAdapter
        if (mBluetoothDeviceAddress != null && address == mBluetoothDeviceAddress && mBluetoothGatt != null) {
            Log.i(TAG, "trying  to reconnect.")
            return mBluetoothGatt!!.connect()
        }
        val device = mBluetoothAdapter!!.getRemoteDevice(address)
        if (device == null) {
            Log.i(TAG, "Device not found.  Unable to connect.")
            return false
        }
        mBluetoothGatt = device.connectGatt(this, false, mGattCallback)
        Log.d(TAG, "Trying to create a new connection.");
        mBluetoothDeviceAddress = address
        return true
    }


    fun disconnect() {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "disconnect: BluetoothAdapter not initialized" )
            return
        }
        mBluetoothGatt?.disconnect()
    }

    fun close() {
        if (mBluetoothGatt == null) return
        mBluetoothGatt!!.close()
        mBluetoothGatt = null
    }


    override fun onUnbind(intent: Intent?): Boolean {
        close()
        return super.onUnbind(intent)
    }



    class LocalBinder : Binder() {
        val service: BluetoothLeService
            get() = BluetoothLeService()
    }

    interface OnBluetoothLEStatusChangedListener{
        /**
         * @param status 哪个阶段的状态发生改变
         * @param info   状态改变的信息
         */
        fun onBluetoothLEStatusChanged(status:Int,info:Int)
    }


    interface OnCharacteristicChangedListener {
        /**
         * @param status         哪个阶段的状态发生改变
         * @param characteristic 特征信息,可读写值
         */
        fun onCharacteristicChanged(status: Int, characteristic: BluetoothGattCharacteristic?)
    }


}