package com.qding.myapp.ble

import android.content.Context
import android.os.Bundle
import android.os.Handler
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout.OnRefreshListener
import com.qding.config.blelib.BleConnManager
import com.qding.config.blelib.BleConnManager.Companion.getInstance
import com.qding.config.blelib.blescan.IScanDeviceCallback
import com.qding.config.blelib.blescan.ScanResult
import com.qding.myapp.App
import com.qding.myapp.R
import com.qdingnet.provider.config.ConfigManager
import com.qdingnet.provider.config.Logdeal
import kotlinx.android.synthetic.main.layout_config_sdk_main.*
import kotlinx.android.synthetic.main.layout_config_sdk_main_device_item.view.*
import java.util.*
import kotlin.concurrent.thread
import kotlin.experimental.and

/**
 *  Created by yangyeah on 2021/6/23
 *
 *  describe:
 */
class ScanDeviceActivity2 : BaseActivity2() {
    private val TAG = "ScanDeviceActivity"
    private val LOCATION_PERMISSION_REQUEST_CODE = 0x09

    private var mConfigManager: ConfigManager? = null
    private var mAdapter: DeviceAdapter? = null
    private val mHandler = Handler()

    //Views
    private lateinit var mSwipeRefreshLayout: SwipeRefreshLayout
    private lateinit var mListView: RecyclerView

    private lateinit var mBleConnManager :BleConnManager

    private var bleConnState:Boolean =false


    /**
     * @param savedInstanceState
     */
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.layout_config_sdk_main)
        requestPermission()
        init()
    }

    /**
     * 初始化
     */
    private fun init() {
        mConfigManager = App.mConfigManager
        //下拉刷新设置
        mSwipeRefreshLayout = findViewById(R.id.refresh_layout)
        mSwipeRefreshLayout.setProgressBackgroundColorSchemeResource(android.R.color.white)
        mSwipeRefreshLayout.setColorSchemeResources(
            R.color.colorAccent,
            R.color.colorPrimary,
            R.color.colorPrimaryDark
        )
        mSwipeRefreshLayout.setOnRefreshListener(OnRefreshListener { scanDevice2() })
        //设备列表
        mListView = findViewById(R.id.device_list)
        mListView.layoutManager = LinearLayoutManager(this)
        mAdapter = DeviceAdapter(this){

            Log.i(TAG, "init: mac:$it")
            mBleConnManager.startConnect(it)

        }
        mListView.adapter = mAdapter
//        mListView.setOnItemClickListener(AdapterView.OnItemClickListener { parent, view, position, id ->
//            val intent = Intent(
//                this@ScanDeviceActivity2,
//                ConfigSdkTestActivity2::class.java
//            )
//            intent.putExtra(
//                ConfigSdkTestActivity2.EXTRA_DEVICE_SSID,
//                mAdapter!!.getItem(position).ssid
//            )
//            startActivity(intent)
//        })
        //mAdapter.addDevice(new Device("883B8B017945", -60));
        //默认开始刷新
        mSwipeRefreshLayout.setRefreshing(true)
        scanDevice2()

        mBleConnManager.setDeviceConnListener(object : BleConnManager.IdeviceConnListener{
            override fun deviceConnStateChanged(isConn: Boolean) {
                bleConnState = isConn
                if (isConn) {
                    Log.i(TAG, "deviceConnStateChanged:  conn success ")
                }
            }

        })

        send.setOnClickListener{
            if (bleConnState) {
                thread {
                    sendData()
                }
            } else {
                Toast.makeText(this, "连接断开", Toast.LENGTH_SHORT).show()
            }
        }

        disconn.setOnClickListener{
            mBleConnManager.destory()
        }

        mBleConnManager.setBletoothReceiverDataListener(object :BleConnManager.IBletoothReceiverDataListener{
            override fun onBletoothReceiver(receiverByte: ByteArray?) {
                if (receiverByte == null) return
                val content = bytes2HexString(receiverByte)
                Log.i(TAG, "onBletoothReceiver: content = $content")
            }

        })
    }

    private fun sendData() {

        val sn = "12345678"
        val snToByteArray = sn.toByteArray(Charsets.UTF_8)
        val sendByteArray = byteArrayOf(0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08)

        val byteArray = ByteArray(9)
        byteArray[0]= 0x01
        var index = 1

        while (index < 9) {
            byteArray[index] = snToByteArray[index-1]
            index ++
        }

        Log.i(TAG, "sendData: byteArray = ${byteArray.contentToString()}")

        mBleConnManager.sendDatas(byteArray)
    }


    fun bytes2HexString(bytes: ByteArray): String? {
        var ret = ""
        for (i in bytes.indices) {
            var hex = Integer.toHexString((bytes[i] and 0xFF.toByte()).toInt())
            if (hex.length == 1) {
                hex = "0$hex"
            }
            ret += hex.toUpperCase()
        }
        return ret
    }


    private fun scanDevice2() {
        mBleConnManager = getInstance(this)
        Log.i(TAG, "scanDevice...")
        //1.异步扫描设备方式
        mAdapter!!.clear()
        mBleConnManager.bluetoothLEManager.startScan(3000, object : IScanDeviceCallback {
            override fun onScanResult(result: ScanResult) {
                //扫描结果
                Logdeal.D(
                    TAG,
                    "onScanResult...ssid:" + result.ssid + ", rssi:" + result.rssi+",address:${result.address}"
                )
                if (result.ssid != null) {
                    runOnUiThread {
                        mAdapter!!.addDevice(Device(
                                result.ssid,
                                result.rssi,
                                result.address
                            )
                        )
                    }
                }
            }

            override fun onScanFinish() {
                Logdeal.D(TAG, "onScanFinish")
                mSwipeRefreshLayout!!.isRefreshing = false
            }
        })
    }

    internal class DeviceAdapter(private val mContext: Context,private val callback: (String)->Unit) :
        RecyclerView.Adapter<DeviceAdapter.ViewHolder>() {
        private val mDataList: MutableList<Device> = ArrayList()
        @Synchronized
        fun clear() {
            mDataList.clear()
            notifyDataSetChanged()
        }

//        fun addDevices(devices: List<Device?>?) {
//            mDataList.clear()
//            mDataList.addAll(devices)
//            Collections.sort(mDataList)
//            notifyDataSetChanged()
//        }

        @Synchronized
        fun addDevice(device: Device) {
            val index = mDataList.indexOf(device)
            if (index != -1) {
                val cached = mDataList[index]
                cached.rssi = device.rssi
            } else {
                mDataList.add(device)
            }
            Collections.sort(mDataList)
            notifyDataSetChanged()
        }

        class ViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
            var deviceMac = itemView.device_mac
            var deviceRssi =itemView.device_rssi
            var deviceAddress = itemView.device_address
        }



        override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
            return ViewHolder(LayoutInflater.from(parent.context)
                .inflate(R.layout.layout_config_sdk_main_device_item,parent,false))
        }

        override fun onBindViewHolder(holder: ViewHolder, position: Int) {
            val bean = mDataList[position]
            holder.deviceMac.text = bean.ssid
            holder.deviceRssi.text = String.format("%ddBm", bean.rssi)
            holder.deviceAddress.text = bean.address

            holder.itemView.setOnClickListener {
                bean.address?.let { it1 ->
                    Log.i("", "onBindViewHolder: address = $it1 ")
                    callback(it1)
                }
            }


        }

        override fun getItemCount(): Int {
           return mDataList.size
        }
    }

    internal class Device(var ssid: String?, var rssi: Int,var address:String?) : Comparable<Device?> {


        override fun equals(o: Any?): Boolean {
            if (this === o) return true
            if (o == null || javaClass != o.javaClass) return false
            val device = o as Device
            return if (ssid != null) ssid == device.ssid else device.ssid == null
        }

        override fun hashCode(): Int {
            return if (ssid != null) ssid.hashCode() else 0
        }

        override fun compareTo(other: Device?): Int {
            return other!!.rssi - rssi
        }
    }
}