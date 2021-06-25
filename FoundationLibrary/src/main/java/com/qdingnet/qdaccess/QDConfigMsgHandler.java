package com.qdingnet.qdaccess;

import java.util.concurrent.CopyOnWriteArraySet;

/**
 * 设备配置jni回调接口
 * @author pipan (pipan@qding.me)
 * @date   2019/5/16
 */
public class QDConfigMsgHandler {

    private static CopyOnWriteArraySet<IQDConfigMsgHandler> sCallbacks = new CopyOnWriteArraySet<>();

    public static void registerCallback(IQDConfigMsgHandler callBack) {
        sCallbacks.clear();
        sCallbacks.add(callBack);
    }

    public static void unregisterCallback(IQDConfigMsgHandler callBack) {
        sCallbacks.remove(callBack);
    }

    public static interface IQDConfigMsgHandler{
        /**
         * 写配置结果
         * @param result
         */
        void onWriteConfigAck(int result);

        /**
         * 读取配置结果
         * @param entity
         */
        void onReadConfigAck(QDDeviceConfigEntity entity);

        /**
         * 读取设备版本结果
         * @param softwareVersion
         * @param hardwareVersion
         */
        void onGetVersionAck(String softwareVersion, String hardwareVersion);

        /**
         * 设备升级结果
         * @param result
         */
        void onUpdateDeviceAck(int result);
    }

    /**
     * 写配置结果回调
     * @param result
     */
    public static void onWriteConfigAck(int result){
        for(IQDConfigMsgHandler handler: sCallbacks){
            handler.onWriteConfigAck(result);
        }
    }

    /**
     * 读配置结果回调
     * @param entity
     */
    public static void onReadConfigAck(QDDeviceConfigEntity entity){
        for(IQDConfigMsgHandler handler: sCallbacks){
            handler.onReadConfigAck(entity);
        }
    }

    /**
     * 读取设备版本结果回调
     * @param softwareVersion
     * @param hardwareVersion
     */
    public static void onGetVersionAck(String softwareVersion, String hardwareVersion){
        for(IQDConfigMsgHandler handler: sCallbacks){
            handler.onGetVersionAck(softwareVersion, hardwareVersion);
        }
    }

    /**
     * 升级设备结果回调
     * @param result
     */
    public static void onUpdateDeviceAck(int result){
        for(IQDConfigMsgHandler handler: sCallbacks){
            handler.onUpdateDeviceAck(result);
        }
    }
}
