package com.qdingnet.qdaccess;

import java.util.concurrent.CopyOnWriteArraySet;

/**
 * 设备工厂测试jni回调接口
 * @author pipan (pipan@qding.me)
 * @date   2019/6/17
 */
public class QDFactoryTestMsgHandler {
    private static CopyOnWriteArraySet<IQDFactoryTestMsgHandler> sCallbacks = new CopyOnWriteArraySet<>();

    public static void registerCallback(IQDFactoryTestMsgHandler callback) {
        sCallbacks.clear();
        sCallbacks.add(callback);
    }

    public static void unregisterCallback(IQDFactoryTestMsgHandler callback) {
        sCallbacks.remove(callback);
    }

    public static interface IQDFactoryTestMsgHandler{
        /***********二代测试回调*************/
        /**
         * 读取设备版本结果
         * @param softwareVersion
         * @param hardwareVersion
         */
        void onGetVersionAck(String softwareVersion, String hardwareVersion);
        /**
         * 使能433测试结果
         * @param result_no
         */
        void onEnable433TestAck(int result_no);

        /**
         * 获取霍尔开关状态
         * @param state
         */
        void onGetHalStateAck(int state);

        /***********四代测试回调*************/
        /**
         * app主动获取设备信息
         * @param info
         */
        void onReceiveDeviceInfoAck(QDDeviceTestInfo info);

        /**
         * 设备主动通知app相关信息
         */
        void onDeviceNotifyEventAck(QDDeviceTestNotify notify);

        /***********五代测试回调*************/
        /**
         * 获取门磁状态
         * @param state
         */
        void onGetGateMagnetismStateAck(int state);
    }

    /**
     * 读取设备版本结果
     * @param softwareVersion
     * @param hardwareVersion
     */
    public static void onGetVersionAck(String softwareVersion, String hardwareVersion){
        for(IQDFactoryTestMsgHandler handler: sCallbacks){
            handler.onGetVersionAck(softwareVersion, hardwareVersion);
        }
    }
    /**
     * 使能433测试结果
     * @param result_no
     */
    public static void onEnable433TestAck(int result_no){
        for(IQDFactoryTestMsgHandler handler: sCallbacks){
            handler.onEnable433TestAck(result_no);
        }
    }

    /**
     * 获取霍尔开关状态
     * @param state
     */
    public static void onGetHalStateAck(int state){
        for(IQDFactoryTestMsgHandler handler: sCallbacks){
            handler.onGetHalStateAck(state);
        }
    }

    /***********四代测试回调*************/
    /**
     * app主动获取设备信息
     * @param info
     */
    public static void onReceiveDeviceInfoAck(QDDeviceTestInfo info){
        for(IQDFactoryTestMsgHandler handler: sCallbacks){
            handler.onReceiveDeviceInfoAck(info);
        }
    }

    /**
     * 设备主动通知app相关信息
     */
    public static void onDeviceNotifyEventAck(QDDeviceTestNotify notify){
        for(IQDFactoryTestMsgHandler handler: sCallbacks){
            handler.onDeviceNotifyEventAck(notify);
        }
    }

    /***********五代测试回调*************/
    /**
     * 获取门磁状态
     * @param state
     */
    public static void onGetGateMagnetismStateAck(int state){
        for(IQDFactoryTestMsgHandler handler: sCallbacks){
            handler.onGetGateMagnetismStateAck(state);
        }
    }

}
