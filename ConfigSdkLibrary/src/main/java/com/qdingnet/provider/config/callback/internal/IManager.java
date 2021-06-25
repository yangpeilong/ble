package com.qdingnet.provider.config.callback.internal;

import com.qdingnet.provider.config.bean.GateBuildUnitInfo;
import com.qdingnet.provider.config.bean.ScanResult;
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

import java.util.List;

/**
 * sdk对外接口
 * @author pipan (pipan@qding.me)
 * @date   19-1-29
 */
public interface IManager {
    /**
     * 设置日志是否打开
     * @param debug
     */
    void debugLog(boolean debug);
    /**
     * 获取sdk版本信息
     * @return
     */
    String getSDKVersion();

    /**
     * 获取sdk编译号
     * @return
     */
    String getSDKBuildNo();


    /*******************************门禁相关接口*********************************/
    /**
     * 启动设备扫描(同步方式)
     * @param timeout
     */
    void startScan(final int timeout);

    /**
     * 获取扫描设备列表(同步方式)
     * @return
     */
    List<ScanResult> getScanResults();

    /**
     * 启动设备扫描(异步方式)
     * @param timeout --- 扫描超时时间
     * @param callback --- 扫描结果回调
     */
    void startScan(final int timeout, final IScanDeviceCallback callback);

    /**
     * 停止设备扫描
     */
    void stopScan();

    /**
     * 开门接口
     * @param callback
     */
    void openDoor(OpenDoorBundle bundle, IOpenDoorCallback callback);

    /**
     * 配置设备接口
     * @param bundle
     * @param callback
     */
    void writeConfig(WriteConfigBundle bundle, IWriteConfigCallback callback);

    /**
     * 读取设备配置接口
     * @param bundle
     * @param callback
     */
    void readConfig(ReadConfigBundle bundle, IReadConfigCallback callback);

    /**
     * 读取设备版本接口
     * @param bundle
     * @param callback
     */
    void readVersion(ReadVersionBundle bundle, IReadVersionCallback callback);

    /**
     * 设备升级接口
     * @param bundle
     * @param callback
     */
    void deviceUpgrade(DeviceUpgradeBundle bundle, IDeviceUpgradeCallback callback);

    /**
     * 生成设备开门密码
     * @param isV1
     * @param buildUnitInfo
     * @return
     */
    int generatePassword(boolean isV1, GateBuildUnitInfo buildUnitInfo);
}