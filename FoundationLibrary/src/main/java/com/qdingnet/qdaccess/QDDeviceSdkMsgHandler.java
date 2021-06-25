package com.qdingnet.qdaccess;

import com.qdingnet.opendoor.log.Logdeal;

import java.util.Arrays;
import java.util.concurrent.CopyOnWriteArraySet;

/**
 * 千丁设备端sdk处理器
 * @author pipan (pipan@qding.me)
 * @date   2020-09-01
 */
public class QDDeviceSdkMsgHandler {
    private static final String TAG = "QDDeviceSdkMsgHandler";
    /**
     * 设备sdk回调接口集合
     */
    private final static CopyOnWriteArraySet<IQDDeviceSdkHandler> sCallbacks = new CopyOnWriteArraySet<>();

    /**
     * 注册sdk回调接口
     * @param callback
     */
    public static void registerCallback(IQDDeviceSdkHandler callback) {
        if(callback != null){
            sCallbacks.add(callback);
        }
    }

    /**
     * 注销sdk回调接口
     * @param callback
     */
    public static void unregisterCallback(IQDDeviceSdkHandler callback) {
        sCallbacks.remove(callback);
    }

    /**
     * 设备端指令解析处理结果的上层回调
     */
    public static interface IQDDeviceSdkHandler{
        /**
         * 开门指令处理结果回调
         * @param operation
         * @param result
         * @param userId
         * @param keepTime
         * @param replayData
         */
        void onOpenDoorReply(int operation, boolean result, long userId, long keepTime, byte[] replayData);
    }

    //**************jni 回调 start****************//

    /**
     * 开门指令处理结果回调
     * @param result
     * @param replayData
     */
    public static void onOpenDoorReply(int operation, boolean result, long userId, long keepTime, byte[] replayData){
        Logdeal.D(TAG, "onOpenDoorReply...operation:" + operation +
                                                 ",result:" + result +
                                                 ",userId:" + userId +
                                               ",keepTime:" + keepTime +
                                             ",replayData:" + Arrays.toString(replayData));
        for(IQDDeviceSdkHandler handler: sCallbacks){
            handler.onOpenDoorReply(operation, result, userId, keepTime, replayData);
        }
    }


}
