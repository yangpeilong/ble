package com.qding.config.blelib

import android.annotation.SuppressLint
import android.bluetooth.BluetoothProfile
import android.content.Context
import android.util.Log
import com.qding.config.blelib.blescan.IScanDeviceCallback
import com.qding.config.blelib.blescan.ScanResult
import java.util.concurrent.atomic.AtomicBoolean

/**
 *  Created by yangyeah on 2021/6/23
 *
 *  describe:
 */
class BleConnManager private constructor(private val context: Context) :
    BluetoothLEManager.OnBluetoothLEStatusChangedListener,
    BluetoothLEManager.OnBluetoothLEReadDatasListener {

    companion object {
        const val TAG = "BleConnManager"

        @SuppressLint("StaticFieldLeak")
        @Volatile
        var bleConnManager: BleConnManager? = null
        fun getInstance(context: Context): BleConnManager {
            if (bleConnManager == null) {
                synchronized(BleConnManager::class) {
                    if (bleConnManager == null) {
                        bleConnManager = BleConnManager(context)
                    }
                }
            }
            return bleConnManager!!
        }
    }

    init {
        init()
    }

    lateinit var bluetoothLEManager: BluetoothLEManager

    //同步扫描设备集
    private var mSyncScanResults: MutableList<ScanResult>? = null

    //同步扫描状态
    private val mSyncScanState = AtomicBoolean(false)
    private var mDeviceConnListener:IdeviceConnListener?=null
    private var mIBletoothReceiverDataListener:IBletoothReceiverDataListener?=null

    private fun init() {
        Log.i(TAG, "init: do this")
        bluetoothLEManager = BluetoothLEManager.getInstance(context)
        bluetoothLEManager.onBluetoothLEStatusChangedListener = this
        bluetoothLEManager.onBluetoothLEReadDatasListener = this
        if (bluetoothLEManager.mBluetoothLeService == null) {
            bluetoothLEManager.initBluetoothLE()
        }
    }

    fun setDeviceConnListener(deviceConnListener: IdeviceConnListener) {
        this.mDeviceConnListener = deviceConnListener
    }

    fun setBletoothReceiverDataListener(iBletoothReceiverDataListener: IBletoothReceiverDataListener) {
        this.mIBletoothReceiverDataListener =iBletoothReceiverDataListener
    }

    fun startScan(timeout: Int) {
        if (mSyncScanState.compareAndSet(false, true)) {
            if (mSyncScanResults == null) {
                mSyncScanResults = mutableListOf()
            } else {
                mSyncScanResults?.clear()
            }
            bluetoothLEManager.startScan(
                timeout,
                object : IScanDeviceCallback {
                    override fun onScanResult(result: ScanResult) {
                        Log.i(TAG, "onScanResult: ${result.toString()}")
                        val index: Int = mSyncScanResults!!.indexOf(result)
                        if (index != -1) {
                            val sr: ScanResult =
                                mSyncScanResults!![index]
                            sr.rssi = result.rssi
                        } else {
                            mSyncScanResults!!.add(result)
                        }
                    }

                    override fun onScanFinish() {
                        mSyncScanState.set(false)
                    }
                })
        }
    }

    fun stopScan() {
        bluetoothLEManager.stopScan()
    }


    fun getScanResults(): List<ScanResult?>? {
        return ArrayList<ScanResult>(mSyncScanResults)
    }

    fun startConnect(address:String) {
        bluetoothLEManager.connectDevice(address)
    }

    fun sendDatas(dataByte: ByteArray) {
        if (bluetoothLEManager.bConnectStatus) {
            bluetoothLEManager.writeDataBytes(dataByte)
        }
    }

    fun destory() {
        try {
            bleConnManager = null
            bluetoothLEManager.stopScan()
            bluetoothLEManager.destory()
        } catch (e: Exception) {
        }
    }

    interface IdeviceConnListener {
        fun deviceConnStateChanged(isConn: Boolean)
    }

    interface IBletoothReceiverDataListener {
        fun onBletoothReceiver(receiverByte: ByteArray?)
    }

    override fun onBluetoothLEStatusChanged(status: Int) {
        Log.i(TAG, "onBluetoothLEStatusChanged: state = $status")
        when (status) {
            BluetoothProfile.STATE_CONNECTED -> {
                mDeviceConnListener?.deviceConnStateChanged(true)
            }
            BluetoothProfile.STATE_DISCONNECTED -> {
                mDeviceConnListener?.deviceConnStateChanged(false)
            }
        }
    }

    override fun onBluetoothLEReadDatas(dataBytes: ByteArray?) {
        Log.i(TAG, "onBluetoothLEReadDatas: dataBytes: ${dataBytes?.contentToString()}")
        mIBletoothReceiverDataListener?.onBletoothReceiver(dataBytes)
    }

}