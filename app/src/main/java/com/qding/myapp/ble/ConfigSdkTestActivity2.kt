package com.qding.myapp.ble

import android.app.ProgressDialog
import android.graphics.Color
import android.os.Bundle
import android.os.Environment
import android.text.TextUtils
import android.view.View
import android.widget.TextView
import com.qding.config.blelib.BleConnManager
import com.qding.myapp.App
import com.qding.myapp.R
import com.qdingnet.provider.config.ConfigManager
import com.qdingnet.provider.config.Logdeal
import com.qdingnet.provider.config.bean.DeviceConfigMsg
import com.qdingnet.provider.config.bean.GateBuildUnitInfo
import com.qdingnet.provider.config.bundle.DeviceUpgradeBundle
import com.qdingnet.provider.config.bundle.OpenDoorBundle.OpenDoorBundleBuilder
import com.qdingnet.provider.config.bundle.ReadConfigBundle
import com.qdingnet.provider.config.bundle.WriteConfigBundle
import java.io.File
import java.text.SimpleDateFormat
import java.util.*
import kotlin.collections.HashMap

/**
 *  Created by yangyeah on 2021/6/15
 *
 *  describe:
 */
class ConfigSdkTestActivity2:BaseActivity2(), View.OnClickListener {

    companion object{
        const val EXTRA_DEVICE_SSID = "ssid"
    }

    //sdk管理类
    private var mConfigManager: ConfigManager? = null

    //设备操作进度框
    private var mProgressDialog: ProgressDialog? = null

    //测试设备的ssid
    private var mDeviceSsid: String? = null

    //测试中的卡号
    private val mCardNo: Long = 0

    //读取的设备配置信息
    private var mDeviceConfigMsg: DeviceConfigMsg? = null

    //设备可升级的固件版本路径
    private val mDeviceUpgradeFirmwarePaths = mutableMapOf<String,String>()

    //设备可升级的固件版本对应关系
    private val mDeviceUpgradeFirmwares= mutableMapOf<String,String>()

    //测试结果
    private var mTestResult: TextView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.layout_config_sdk_test)
        init()
        requestPermission()


//        val instance = BleConnManager.getInstance(this)
//         instance.startScan(3000)

    }


    fun init() {
        mConfigManager = App.mConfigManager
        //开门测试按键
        //开门测试按键
        val openDoorTest = findViewById<TextView>(R.id.open_door_test)
        openDoorTest.setOnClickListener(this)
        //读卡测试按钮
        //读卡测试按钮
        val configDeviceTV = findViewById<TextView>(R.id.config_device)
        configDeviceTV.setOnClickListener(this)
        //写卡测试按钮
        //写卡测试按钮
        val readConfigTV = findViewById<TextView>(R.id.read_config)
        readConfigTV.setOnClickListener(this)
        //删卡测试按钮
        //删卡测试按钮
        val deviceUpgradeTV = findViewById<TextView>(R.id.device_upgrade)
        deviceUpgradeTV.setOnClickListener(this)
        //测试结果
        //测试结果
        mTestResult = findViewById(R.id.test_result)

        //创建进度框

        //创建进度框
        createProgressDialog()
        //获取设备ssid
        //获取设备ssid
        mDeviceSsid = intent.getStringExtra(EXTRA_DEVICE_SSID)
        if (TextUtils.isEmpty(mDeviceSsid)) {
            throw NullPointerException("Please set <EXTRA_DEVICE_SSID> params in intent")
        }
        //添加固件可升级版本及路径
        //添加固件可升级版本及路径
        val path =
            Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS).absolutePath
        mDeviceUpgradeFirmwarePaths["V2.3.6"] = path + File.separator + "upgrade.V2.3.6.zip"
        mDeviceUpgradeFirmwarePaths["V2.3.7"] = path + File.separator + "upgrade.V2.3.7.zip"

        mDeviceUpgradeFirmwares["V2.3.6"] = "V2.3.7"
        mDeviceUpgradeFirmwares["V2.3.7"] = "V2.3.6"



        Logdeal.D(
           "ConfigSdkTestActivity2",
            "init...firmwarePaths:$mDeviceUpgradeFirmwarePaths\r\nfirmwares:$mDeviceUpgradeFirmwares"
        )
    }

    private fun createProgressDialog() {
        mProgressDialog = ProgressDialog(this)
        mProgressDialog?.isIndeterminate = true
    }

    override fun onClick(v: View?) {
        val id = v!!.id
        when (id) {
            R.id.open_door_test -> openDoorTest(mDeviceSsid!!)
            R.id.config_device -> configDevice(mDeviceSsid!!)
            R.id.read_config -> readConfig(mDeviceSsid!!)
            R.id.device_upgrade -> deviceUpgrade(mDeviceSsid!!)
            else -> {
            }
        }
    }

    private fun openDoorTest(ssid: String) {
        mProgressDialog!!.setMessage("开门中...")
        mProgressDialog!!.show()
        val bundle = OpenDoorBundleBuilder() //自定义用户标示ID
            .setAppUserId(12345678901) //是否开通广播开门
            .setSupportAdvertise(true) //添加需开门门禁设备的ssid
            .addDeviceBundle(ssid)
            .build()
        mConfigManager!!.openDoor(
            bundle
        ) { mac, result -> setTestResult(result.userTips, result.isSuccess) }
    }

    /**
     * 设置测试结果
     * @param result
     */
    fun setTestResult(result: String?, success: Boolean) {
        runOnUiThread {
            mTestResult!!.setTextColor(if (success) Color.GREEN else Color.RED)
            mTestResult!!.text = result
            mProgressDialog!!.dismiss()
        }
    }

    /**
     * 配置设备测试
     * @param ssid
     */
    private fun configDevice(ssid: String) {
        mProgressDialog!!.setMessage("写配置中...")
        mProgressDialog?.show()
        val bundle = WriteConfigBundle.Builder() //设置操作账号
            .setAccount("12345678901") //设置配置的设备 QD="EyhtEkuifB-7mwAru8qU5A==",MH="8Ab6kFplhO9O1fXHkpyoxg=="
            .setDevice(ssid, 5000, "EyhtEkuifB-7mwAru8qU5A==") //设置设备继电器状态
            .setRelayOn(true) //添加配置单元信息
            .addBuildUnit(GateBuildUnitInfo("", "", "", "", "", 123, 1500))
            .build()
        mConfigManager!!.writeConfig(
            bundle
        ) { mac, result -> setTestResult(result.userTips, result.isSuccess) }
    }

    /**
     * 读取配置测试
     * @param ssid
     */
    private fun readConfig(ssid: String) {
        mProgressDialog!!.setMessage("读配置中...")
        mProgressDialog?.show()
        val bundle = ReadConfigBundle.Builder()
            .setDevice(ssid)
            .build()
        mConfigManager!!.readConfig(
            bundle
        ) { mac, result, msg ->
            val tips: String
            if (result.isSuccess) {
                tips = String.format(
                    Locale.US,
                    "开门时长:%d\r\n设备时间:%s\r\n软件版本:%s\r\n硬件版本:%s\r\n配置单元[0]:%s\r\n",
                    msg.openKeepTime,
                    SimpleDateFormat("yyyy-MM-dd HH:mm:ss")
                        .format(Date(msg.deviceTime * 1000)),
                    msg.softwareVersion,
                    msg.hardwareVersion,
                    if (msg.configUnitInfoList != null) msg.configUnitInfoList[0] else ""
                )
                mDeviceConfigMsg = msg
            } else {
                tips = result.userTips
            }
            setTestResult(tips, result.isSuccess)
        }
        //readVersion(ssid);
    }

    /**
     * 设备升级测试
     * @param ssid
     */
    private fun deviceUpgrade(ssid: String) {
        if (mDeviceConfigMsg == null) {
            setTestResult("请先进行<读取配置>操作", false)
            return
        }
        val upgradeFirmware = mDeviceUpgradeFirmwares[mDeviceConfigMsg!!.softwareVersion]
        val upgradeFirmwarePath = mDeviceUpgradeFirmwarePaths[upgradeFirmware]
        if (upgradeFirmwarePath == null) {
            setTestResult(
                String.format(
                    "未找到当前版本「%s」对应的可升级版本，请配置好相应的版本和路径",
                    mDeviceConfigMsg!!.softwareVersion
                ), false
            )
            return
        }
        mProgressDialog!!.setMessage("设备升级中...")
        mProgressDialog?.show()
        val bundle = DeviceUpgradeBundle.Builder()
            .setDevice(ssid) //升级超时时间
            .setUpgradeTimeout(3 * 60000) //升级固件本地存储路径
            .setFirmwarePath(upgradeFirmwarePath) //升级固件的版本
            .setFirmwareVersion(upgradeFirmware)
            .build()
        mConfigManager!!.deviceUpgrade(
            bundle
        ) { mac, result -> setTestResult(result.userTips, result.isSuccess) }
    }

}