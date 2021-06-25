package com.qdingnet.provider.config;

import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Build;

import androidx.annotation.RequiresApi;

import com.qdingnet.opendoor.client.BluetoothClient;
import com.qdingnet.opendoor.client.WifiClient;
import com.qdingnet.opendoor.client.internal.ClientCloseable;
import com.qdingnet.opendoor.core.callback.ICloseable;
import com.qdingnet.opendoor.core.handler.IAdvertiseHandler;
import com.qdingnet.opendoor.core.request.Request;
import com.qdingnet.opendoor.log.Logdeal;
import com.qdingnet.provider.config.bean.Contants;
import com.qdingnet.provider.config.bean.GateBuildUnitInfo;
import com.qdingnet.provider.config.bean.QDevice;
import com.qdingnet.provider.config.bean.ScanResult;
import com.qdingnet.provider.config.bean.StateGuard;
import com.qdingnet.provider.config.bluetooth.QDBluetoothScanner;
import com.qdingnet.provider.config.bundle.DeviceUpgradeBundle;
import com.qdingnet.provider.config.bundle.ReadConfigBundle;
import com.qdingnet.provider.config.bundle.ReadVersionBundle;
import com.qdingnet.provider.config.bundle.WriteConfigBundle;
import com.qdingnet.provider.config.bundle.OpenDoorBundle;
import com.qdingnet.provider.config.callback.IDeviceUpgradeCallback;
import com.qdingnet.provider.config.callback.IReadConfigCallback;
import com.qdingnet.provider.config.callback.IReadVersionCallback;
import com.qdingnet.provider.config.callback.IWriteConfigCallback;
import com.qdingnet.provider.config.callback.IOpenDoorCallback;
import com.qdingnet.provider.config.callback.IScanDeviceCallback;
import com.qdingnet.provider.config.callback.impl.DeviceUpgradeCallbackAdapter;
import com.qdingnet.provider.config.callback.impl.OpenDoorCallbackAdapter;
import com.qdingnet.provider.config.callback.impl.ReadConfigCallbackAdapter;
import com.qdingnet.provider.config.callback.impl.ReadVersionCallbackAdapter;
import com.qdingnet.provider.config.callback.impl.WriteConfigCallbackAdapter;
import com.qdingnet.provider.config.callback.internal.IManager;
import com.qdingnet.provider.config.handler.DeviceUpgradeHandler;
import com.qdingnet.provider.config.handler.OpendoorHandler;
import com.qdingnet.provider.config.handler.QdingAdvertiseHandler;
import com.qdingnet.provider.config.handler.QdingAuthHandler;
import com.qdingnet.provider.config.handler.QdingDataPacketHandler;
import com.qdingnet.provider.config.handler.QdingGattServiceAuthHandler;
import com.qdingnet.provider.config.handler.QdingSocketHandler;
import com.qdingnet.provider.config.handler.ReadConfigHandler;
import com.qdingnet.provider.config.handler.ReadVersionHandler;
import com.qdingnet.provider.config.handler.WriteConfigHandler;
import com.qdingnet.provider.config.secure.SecureProvider;
import com.qdingnet.provider.config.upgrade.WebService;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.atomic.AtomicBoolean;

public class ConfigManager implements IManager {
    private static final String TAG = "ConfigManager";

    //sdk版本及编译号对应的关键字
    private static final String KEY_SDK_VERSION = "SDK_VERSION";
    private static final String KEY_SDK_BUILDNO = "BUILD_NO";
    //默认开门最小信号值
    public static final int DEFAULT_RSSI = -90;

    //应用上下文
    private Context mContext;
    //同步扫描状态
    private AtomicBoolean mSyncScanState = new AtomicBoolean(false);
    //同步扫描设备集
    private List<ScanResult> mSyncScanResults;

    public ConfigManager(Context context){
        this.mContext = context.getApplicationContext();
        SecureProvider.register(context);
    }

    private boolean supportBLE() {
        return mContext.getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE);
    }

    private boolean supportBLEAdvertiser(){
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        return adapter != null && Build.VERSION.SDK_INT >= 21 && adapter.isMultipleAdvertisementSupported();
    }

    /**
     * 获取Manifest.xml中元数据
     * @param context
     * @param appKey
     * @return
     */
    private String getMetaData(Context context, String appKey) {
        String value = "";
        try {
            ApplicationInfo ai = context.getPackageManager().getApplicationInfo(context.getPackageName(), PackageManager.GET_META_DATA);
            Object metaValue = ai.metaData.get(appKey);
            if(metaValue != null){
                if (metaValue instanceof Integer) {
                    long longValue = ((Integer) metaValue).longValue();
                    value = String.valueOf(longValue);
                } else if (metaValue instanceof String) {
                    value = String.valueOf(metaValue);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return value;

    }

    @Override
    public void debugLog(boolean debug) {
        Logdeal.setSwitch(debug);
        com.qdingnet.provider.config.Logdeal.setSwitch(debug);
    }

    @Override
    public String getSDKVersion() {
        return getMetaData(mContext, KEY_SDK_VERSION);
    }

    @Override
    public String getSDKBuildNo() {
        return getMetaData(mContext, KEY_SDK_BUILDNO);
    }

    @Override
    public void startScan(int timeout) {
        if(mSyncScanState.compareAndSet(false, true)) {
            //初始化数据集合
            if (mSyncScanResults == null) {
                mSyncScanResults = new ArrayList<>();
            } else {
                mSyncScanResults.clear();
            }
            startScan(timeout, new IScanDeviceCallback() {
                @Override
                public void onScanResult(ScanResult result) {
                    //去重
                    int index = mSyncScanResults.indexOf(result);
                    if(index != -1){
                        ScanResult sr = mSyncScanResults.get(index);
                        sr.rssi = result.rssi;
                    }else {
                        mSyncScanResults.add(result);
                    }
                }
                @Override
                public void onScanFinish() {
                    mSyncScanState.set(false);
                }
            });
        }
    }

    @Override
    public List<ScanResult> getScanResults() {
        return new ArrayList<>(mSyncScanResults);
    }

    @Override
    public void startScan(int timeout, final IScanDeviceCallback callback) {
        QDBluetoothScanner.getInstance().startScan(mContext, timeout, new QDBluetoothScanner.OnScanListener() {
            @Override
            public void onScanResult(ScanResult result) {
                if(callback != null && result != null){
                    callback.onScanResult(result);
                }
            }
            @Override
            public void onScanStop() {
                if(callback != null){
                    callback.onScanFinish();
                }
            }
        });
    }

    @Override
    public void stopScan() {
        QDBluetoothScanner.getInstance().stopScan();
    }

    @Override
    public void openDoor(OpenDoorBundle bundle, IOpenDoorCallback callback) {
        if(Build.VERSION.SDK_INT > 17 && supportBLE()){
            bleOpenDoor(bundle, callback);
        }else {
            wifiOpenDoor(bundle, callback);
        }
    }

    /**
     * 蓝牙开门
     * @param callback
     */
    @RequiresApi(18)
    private void bleOpenDoor(OpenDoorBundle bundle, final IOpenDoorCallback callback){
        //开门操作
        List<QDevice> deviceList = bundle.getOpenDoorDeviceInfo();
        Logdeal.D(TAG, "bleOpenDoor...OpenDoorBundle:" + bundle);
        final int passId = QdingDataPacketHandler.createPassId();
        StateGuard stateGuard = new StateGuard();
        OpenDoorCallbackAdapter delegate = new OpenDoorCallbackAdapter(OpenDoorCallbackAdapter.MODE_BLE, bundle.getAppUserId(), passId, stateGuard, callback);
        IAdvertiseHandler advertiseHandler = supportBLEAdvertiser() && bundle.isSupportAdvertise() ? new QdingAdvertiseHandler(bundle.getAppUserId(), passId, deviceList, delegate) : null;
        BluetoothClient client = new BluetoothClient.Builder()
                .setTimeout(10_000, 10_000)
                .setFilterUUIDs(new UUID[]{QdingGattServiceAuthHandler.UUID_UART_SERVICE, QdingAdvertiseHandler.UUID_ADVERTISER_RESULT_SERVICE})
                .setIAuthHandler(new QdingAuthHandler(deviceList, stateGuard))
                .setIAdvertiseHandler(advertiseHandler)
                .setIGattServiceAuthHandler(new QdingGattServiceAuthHandler())
                .setIDataHandler(new OpendoorHandler(OpendoorHandler.MODE_BLE, bundle.getAppUserId(), passId, deviceList, delegate))
                .setIStateCallback(delegate)
                .setICloseableFactory(new ClientCloseable())
                .build();
        Request request = new Request();
        int result = client.start(mContext, request);
        Logdeal.D(TAG, "bleOpenDoor...start result:" + result);
    }

    /**
     * wifi开门
     * @param callback
     */
    private void wifiOpenDoor(OpenDoorBundle bundle, final IOpenDoorCallback callback){
        List<QDevice> deviceList = bundle.getOpenDoorDeviceInfo();
        Logdeal.D(TAG, "wifiOpenDoor...OpenDoorBundle:" + bundle);
        StateGuard stateGuard = new StateGuard();
        OpenDoorCallbackAdapter delegate = new OpenDoorCallbackAdapter(OpenDoorCallbackAdapter.MODE_WIFI, bundle.getAppUserId(),0, stateGuard, callback);
        OpendoorHandler handler = new OpendoorHandler(OpendoorHandler.MODE_WIFI, bundle.getAppUserId(), 0, deviceList, delegate);
        WifiClient client = new WifiClient.Builder()
                .setSsidNameRegex("^QD[_A-Z].+")
                .setSsidPassword(new String(Contants.QDWifi.PW))
                .setConnectTimeout(10_000)
                .setIAuthHandler(new QdingAuthHandler(deviceList, stateGuard))
                .setIDataHandler(handler)
                .setIWifiConnectHandler(new QdingSocketHandler(15_000, 15_000, handler, delegate))
                .setIStateCallback(delegate)
                .setICloseableFactory(new ClientCloseable())
                .build();
        Request request = new Request();
        int result = client.start(mContext, request);
        Logdeal.D(TAG, "wifiOpenDoor...start result:" + result);
    }

    @RequiresApi(18)
    @Override
    public void writeConfig(WriteConfigBundle bundle, IWriteConfigCallback callback) {
        List<QDevice> deviceList = Arrays.asList(bundle.device);
        Logdeal.D(TAG, "writeConfig...WriteConfigBundle:" + bundle);
        StateGuard stateGuard = new StateGuard();
        WriteConfigCallbackAdapter delegate = new WriteConfigCallbackAdapter(stateGuard, callback);
        //计算超时时间
        int size = bundle.buildUnitList != null ? bundle.buildUnitList.size() : 0;
        int timeout = Math.max((size * 8 / 20 + 1) * 50 + 5000, 10_000);
        BluetoothClient client = new BluetoothClient.Builder()
                .setTimeout(timeout, timeout)
                .setFilterUUIDs(new UUID[]{QdingGattServiceAuthHandler.UUID_UART_SERVICE, QdingAdvertiseHandler.UUID_ADVERTISER_RESULT_SERVICE})
                .setIAuthHandler(new QdingAuthHandler(deviceList, stateGuard))
                .setIGattServiceAuthHandler(new QdingGattServiceAuthHandler(stateGuard))
                .setIDataHandler(new WriteConfigHandler(bundle, stateGuard, delegate))
                .setIStateCallback(delegate)
                .setICloseableFactory(new ClientCloseable())
                .build();
        Request request = new Request();
        int result = client.start(mContext, request);
        Logdeal.D(TAG, "writeConfig...start result:" + result);
    }

    @RequiresApi(18)
    @Override
    public void readConfig(ReadConfigBundle bundle, IReadConfigCallback callback) {
        List<QDevice> deviceList = Arrays.asList(bundle.device);
        Logdeal.D(TAG, "readConfig...ReadConfigBundle:" + bundle);
        StateGuard stateGuard = new StateGuard();
        ReadConfigCallbackAdapter delegate = new ReadConfigCallbackAdapter(stateGuard, callback);
        BluetoothClient client = new BluetoothClient.Builder()
                .setTimeout(10_000, 10_000)
                .setFilterUUIDs(new UUID[]{QdingGattServiceAuthHandler.UUID_UART_SERVICE, QdingAdvertiseHandler.UUID_ADVERTISER_RESULT_SERVICE})
                .setIAuthHandler(new QdingAuthHandler(deviceList, stateGuard))
                .setIGattServiceAuthHandler(new QdingGattServiceAuthHandler())
                .setIDataHandler(new ReadConfigHandler(bundle, delegate))
                .setIStateCallback(delegate)
                .setICloseableFactory(new ClientCloseable())
                .build();
        Request request = new Request();
        int result = client.start(mContext, request);
        Logdeal.D(TAG, "readConfig...start result:" + result);
    }

    @RequiresApi(18)
    @Override
    public void readVersion(ReadVersionBundle bundle, IReadVersionCallback callback) {
        List<QDevice> deviceList = Arrays.asList(bundle.device);
        Logdeal.D(TAG, "readVersion...ReadVersionBundle:" + bundle);
        StateGuard stateGuard = new StateGuard();
        ReadVersionCallbackAdapter delegate = new ReadVersionCallbackAdapter(stateGuard, callback);
        BluetoothClient client = new BluetoothClient.Builder()
                .setTimeout(10_000, 10_000)
                .setFilterUUIDs(new UUID[]{QdingGattServiceAuthHandler.UUID_UART_SERVICE, QdingAdvertiseHandler.UUID_ADVERTISER_RESULT_SERVICE})
                .setIAuthHandler(new QdingAuthHandler(deviceList, stateGuard))
                .setIGattServiceAuthHandler(new QdingGattServiceAuthHandler())
                .setIDataHandler(new ReadVersionHandler(bundle, delegate))
                .setIStateCallback(delegate)
                .setICloseableFactory(new ClientCloseable())
                .build();
        Request request = new Request();
        int result = client.start(mContext, request);
        Logdeal.D(TAG, "readVersion...start result:" + result);
    }

    @RequiresApi(18)
    @Override
    public void deviceUpgrade(DeviceUpgradeBundle bundle, IDeviceUpgradeCallback callback) {
        //先启动web服务
        WebService.start(mContext);
        //构造升级设备数据
        List<QDevice> deviceList = Arrays.asList(bundle.device);
        Logdeal.D(TAG, "deviceUpgrade...DeviceUpgradeBundle:" + bundle);
        StateGuard stateGuard = new StateGuard();
        ICloseable.Factory closeableFactory = new ClientCloseable();
        DeviceUpgradeCallbackAdapter delegate = new DeviceUpgradeCallbackAdapter(this, stateGuard, bundle, callback, closeableFactory);
        DeviceUpgradeHandler handler = new DeviceUpgradeHandler(mContext, bundle, delegate);
        WifiClient client = new WifiClient.Builder()
                .setSsidNameRegex("^QD[_A-Z].+")
                .setSsidPassword(new String(Contants.QDWifi.PW))
                .setConnectTimeout(15_000)
                .setIAuthHandler(new QdingAuthHandler(deviceList, stateGuard))
                .setIDataHandler(handler)
                .setIWifiConnectHandler(new QdingSocketHandler(15_000, 15_000, handler, delegate))
                .setIStateCallback(delegate)
                .setICloseableFactory(closeableFactory)
                .build();
        Request request = new Request();
        int result = client.start(mContext, request);
        Logdeal.D(TAG, "deviceUpgrade...start result:" + result);
    }

    @Override
    public int generatePassword(boolean isV1, GateBuildUnitInfo buildUnitInfo) {
        return QdingDataPacketHandler.generateKey(isV1 ? Contants.DeviceEdition.V1 : Contants.DeviceEdition.V2, buildUnitInfo);
    }
}
