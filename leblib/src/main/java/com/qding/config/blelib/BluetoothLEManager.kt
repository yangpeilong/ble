package com.qding.config.blelib

import android.annotation.SuppressLint
import android.bluetooth.*
import android.content.ComponentName
import android.content.Context
import android.content.Intent
import android.content.ServiceConnection
import android.content.pm.PackageManager
import android.os.*
import android.util.Log
import androidx.annotation.RequiresApi
import com.qding.config.blelib.blescan.*
import java.util.*


/**
 *  Created by yangyeah on 2021/6/23
 *
 *  describe:
 */
class BluetoothLEManager private constructor(private val context: Context) :
    BluetoothLeService.OnBluetoothLEStatusChangedListener,
    BluetoothLeService.OnCharacteristicChangedListener {

    companion object {

        const val  TAG = "BluetoothLEManager"
        const val BLE_INIT_SUCCESS = 0
        const val BLE_INIT_FAIL = 1
        const val SCAN_PERIOD = 15000

        val UUID_UART_SERVICE = UUID.fromString("42880008-1129-484e-af8c-05609d2ec3d7")
        val UUID_NOTIFY_DATA = UUID.fromString("4288000a-1129-484e-af8c-05609d2ec3d7")
        val UUID_WRITE_DATA = UUID.fromString("4288000c-1129-484e-af8c-05609d2ec3d7")
        val UUID_READ_DATA = UUID.fromString("4288000b-1129-484e-af8c-05609d2ec3d7")
        val CLIENT_CHARACTERISTIC_CONFIG = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb")

        @SuppressLint("StaticFieldLeak")
        private var instance: BluetoothLEManager? = null
        fun getInstance(context: Context): BluetoothLEManager {
            return instance ?: synchronized(this) {
                instance ?: BluetoothLEManager(context).also { instance = it }
            }
        }
    }

    var mBluetoothLeService: BluetoothLeService? = null
    private var mbluetoothAdapter: BluetoothAdapter? = null
    private var bBLEScanning: Boolean = false
    var bConnectStatus: Boolean = false
    private var mScanner:AbstractBlueScanner?=null
    private lateinit var mDeviceAddress:String



    var onBluetoothLEStatusChangedListener: OnBluetoothLEStatusChangedListener? = null
    var onBluetoothLEReadDatasListener: OnBluetoothLEReadDatasListener? = null
    private lateinit var mWriteCharateristic:BluetoothGattCharacteristic
    private lateinit var mNotifyCharacteristic:BluetoothGattCharacteristic
    private lateinit var mReadCharacteristic:BluetoothGattCharacteristic

    fun initBluetoothLE(): Int {
        Log.i(TAG, "initBluetoothLE: do this")
        initScanner()
        if (!context.packageManager.hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Log.e(TAG, "System don't support bluetooth LE!")
            return BLE_INIT_FAIL
        }
        val bluetoothManager =
            context.getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        mbluetoothAdapter = bluetoothManager.adapter
        if (mbluetoothAdapter == null) {
            Log.e(TAG, "Get bluetooth adapter fail!")
            return BLE_INIT_FAIL
        }
        val gattServiceIntent = Intent(context, BluetoothLeService::class.java)
        context.bindService(gattServiceIntent, mServiceConnection, Context.BIND_AUTO_CREATE)
        return BLE_INIT_SUCCESS
    }

    private val mServiceConnection = object :ServiceConnection{
        override fun onServiceConnected(componentName: ComponentName?, iBinder: IBinder?) {
            Log.i(TAG, "onServiceConnected: connected success")
            mBluetoothLeService = (iBinder as BluetoothLeService.LocalBinder).service
            mBluetoothLeService!!.onBluetoothLEStatusChangedListener = this@BluetoothLEManager
            mBluetoothLeService!!.onCharacteristicChangedListener = this@BluetoothLEManager
        }

        override fun onServiceDisconnected(p0: ComponentName?) {
            Log.i(TAG, "onServiceDisconnected: disconnectd")
            mBluetoothLeService = null
        }

    }

    private fun initScanner() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            mScanner = BlueScanForLollipop()
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
            mScanner = BlueScanForJellyBeanMR2()
        } else {
            mScanner = null
        }
    }


    fun startScan(
        timeout: Int,
        callback: IScanDeviceCallback?
    ) {
        QDBluetoothScanner.getInstance().startScan(
            context,
            timeout,
            object : QDBluetoothScanner.OnScanListener {
               override fun onScanResult(result: ScanResult) {
                    if (callback != null && result != null) {
                        callback.onScanResult(result)
                    }
                }

                override fun onScanStop() {
                    callback?.onScanFinish()
                }
            })
    }

    fun stopScan() {
        QDBluetoothScanner.getInstance().stopScan()
    }


    fun connectDevice(address: String) {
        if(address.isNullOrEmpty()) return
        mDeviceAddress = address
        if (mBluetoothLeService == null) {
            Log.i(TAG, "connectDevice: mBluetoothLeService == null")
            initBluetoothLE()
            return
        }
        if (mbluetoothAdapter == null) {
            Log.i(TAG, "connectDevice: mbluetoothAdapter == null")
        }
        mBluetoothLeService!!.connect(mbluetoothAdapter!!,mDeviceAddress)
    }

    private fun disconnect() {
        if (mBluetoothLeService == null) return
        mBluetoothLeService!!.disconnect()
    }

    fun destory() {
        stopScan()
        if (mBluetoothLeService == null) {
            return
        }
        disconnect()
        mBluetoothLeService!!.close()
        context.unbindService(mServiceConnection)
        mBluetoothLeService = null
    }

    /**
     * BLE状态改变时监听回调
     */
    override fun onBluetoothLEStatusChanged(status: Int, info: Int) {
        when (status) {
            BluetoothLeService.STATE_CONNECT ->
                if (info === BluetoothProfile.STATE_CONNECTED) {
                    Log.i(TAG, "BluetoothLE has connected!")
                    onBluetoothLEStatusChangedListener?.onBluetoothLEStatusChanged(info)

                } else if (info === BluetoothProfile.STATE_DISCONNECTED) {
                    //BLE已断开
                    bConnectStatus = false
                    Log.i(TAG, "BluetoothLE has disconnected!")
                    onBluetoothLEStatusChangedListener?.onBluetoothLEStatusChanged(info)
                    mBluetoothLeService!!.close()
                }
            BluetoothLeService.STATE_DISCOVER_SERVICES -> {
                //轮询扫描到的服务和特征,查询是否有支持的UUID
                Log.i(TAG, "get support gatt services !")
                displayGattServices(mBluetoothLeService!!.getSupportGattServices())
            }
            else -> {
            }
        }
    }
    /**
     * BLE特征改变时监听回调
     */
    override fun onCharacteristicChanged(
        status: Int,
        characteristic: BluetoothGattCharacteristic?
    ) {
        when (status) {
            BluetoothLeService.STATE_CHARACTERISTIC_WRITE -> {
                Log.i(TAG, "characteristics write: " + characteristic?.value.toString())
            }
            BluetoothLeService.STATE_CHARACTERISTIC_READ -> {
                Log.i(TAG, "characteristics read: " + characteristic?.value.toString())
                if (onBluetoothLEReadDatasListener != null) {
                    onBluetoothLEReadDatasListener!!.onBluetoothLeVersionInfo(characteristic!!.value)
                }
            }
            BluetoothLeService.STATE_CHARACTERISTIC_CHANGED ->  {
                Log.i(TAG, "characteristics changed: " + characteristic?.value.toString())
                if (onBluetoothLEReadDatasListener != null) {
                    onBluetoothLEReadDatasListener!!.onBluetoothLEReadDatas(characteristic!!.value)
                }
            }
            else -> return
        }
    }

    /**
     * 写入数据
     */
    fun writeDataBytes(datas: ByteArray) {
        if (!this::mWriteCharateristic.isInitialized && mBluetoothLeService == null) {
            return
        }
        mWriteCharateristic.value = datas
        mBluetoothLeService!!.writeCharacteristic(mWriteCharateristic)
    }

    /**
     * 读设备数据
     */
    fun readCharacteristic() {
        if (!this::mWriteCharateristic.isInitialized && mBluetoothLeService == null) {
            return
        }
        mBluetoothLeService!!.readCharacteristic(mWriteCharateristic)
    }

    @RequiresApi(18)
    fun onGattServiceAuth(gatt: BluetoothGatt): Boolean {
        val gattService = gatt.getService(UUID_UART_SERVICE)
        if (gattService != null) {
            mWriteCharateristic =
                gattService.getCharacteristic(UUID_WRITE_DATA)
            mNotifyCharacteristic =
                gattService.getCharacteristic(UUID_NOTIFY_DATA)
            mReadCharacteristic = gattService.getCharacteristic(UUID_READ_DATA)
            if (mWriteCharateristic != null && mNotifyCharacteristic != null && mReadCharacteristic != null) {
                //设置开启设备的推送通道
                val isNotifyState = gatt.setCharacteristicNotification(mNotifyCharacteristic, true)
                if (isNotifyState) {
                    val descriptor: BluetoothGattDescriptor = mNotifyCharacteristic.getDescriptor(CLIENT_CHARACTERISTIC_CONFIG)
                    if (descriptor != null) {
                        descriptor.value = BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE
                        gatt.writeDescriptor(descriptor)
                    }
                }
                return true
            }
        }
        return false
    }

    private fun displayGattServices(gattServices: List<BluetoothGattService>?) {
        if (gattServices == null) return
         gattServices.forEachIndexed { index, bluetoothGattService ->
             Log.i(TAG, "displayGattServices: i = $index,UUID = ${bluetoothGattService.uuid}")
             val characteristics = bluetoothGattService.characteristics
             characteristics.forEachIndexed { index, bluetoothGattCharacteristic ->
                 Log.i(TAG, "displayGattServices: index = $index,UUID = ${bluetoothGattCharacteristic.uuid}")
             }
         }
        val isAuth = onGattServiceAuth(mBluetoothLeService!!.mBluetoothGatt!!)
        if (isAuth && onBluetoothLEStatusChangedListener != null) {
            bConnectStatus = true
            onBluetoothLEStatusChangedListener!!.onBluetoothLEStatusChanged(BluetoothProfile.STATE_CONNECTED)
        }
    }

    /**
     * BLE状态改变时监听回调
     */
    interface OnBluetoothLEStatusChangedListener{
        fun onBluetoothLEStatusChanged(status:Int)
    }

    /**
     * BLE收到数据后回调接口
     */
    interface OnBluetoothLEReadDatasListener{
        fun onBluetoothLEReadDatas(dataBytes: ByteArray?)
        fun onBluetoothLeVersionInfo(dataBytes: ByteArray?)
    }



}