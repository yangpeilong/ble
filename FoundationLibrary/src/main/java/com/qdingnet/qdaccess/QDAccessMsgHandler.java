package com.qdingnet.qdaccess;

import com.qdingnet.opendoor.log.Logdeal;

import java.util.ArrayList;
import java.util.concurrent.CopyOnWriteArraySet;

/**
 * Created by kuangbiao on 17-5-11.
 */

public class QDAccessMsgHandler {

    private static final String TAG = QDAccessMsgHandler.class.getSimpleName();

    public interface IQDAccessMsgCallback {

        void onReceiveAck(int result, byte cmdType);

        void onOpenDoorAck(int result);

        void onPassRecordsAck(ArrayList<QDPassRecordEntity> passRecordEntities);

        void onDistributeCardAck(int result, String cardNo);

        void onReadCardAck(int result, String cardNo);

        void onUpdateStateReportAck(String software_version, String hardware_version,
                                    int software_version_num, int current_update_file_pos, int current_update_version_num);
        //离线读卡回调
        void onStandaloneReadCardAck(int result, long cardNo);
        //离线加卡回调
        void onStandaloneAddCardAck(int result);
        //离线删卡回调
        void onStandaloneDelCardAck(int result);
    }

    private static CopyOnWriteArraySet<IQDAccessMsgCallback> sCallbacks = new CopyOnWriteArraySet<IQDAccessMsgCallback>();


    public static void registCallback(IQDAccessMsgCallback callBack) {
        sCallbacks.clear();
        sCallbacks.add(callBack);
    }

    public static void unregistCallback(IQDAccessMsgCallback callBack) {
        sCallbacks.remove(callBack);
    }

    public static native int QDAccessJNIModelInit();

    public static native int QDAccessJNIModelTest();

    public static native int QDAccessJNIModelExit();

    public static native int QDAccessJNIPushData(String targetDeviceMac, byte[] data);

    public static native int QDAccessJNIPushBroadcastData(String targetDeviceMac, byte[] data);

    //**************jni 回调 start****************//

    /**
     * 通用请求ack
     * 例如向设备发送写卡、读卡请求后，设备会立即回一个ack，表示已经收到请求
     *
     * @param result
     * @param cmdType
     */
    public static void onReceiveAck(int result, byte cmdType) {
        Logdeal.D(TAG, "onReceiveAck result == " + result + " cmdType == " + cmdType);
        for (IQDAccessMsgCallback callback : sCallbacks) {
            callback.onReceiveAck(result, cmdType);
        }
    }

    /**
     * 开门结果回调
     *
     * @param result
     */
    public static void onOpenDoorAck(int result) {
        Logdeal.D(TAG, "onOpenDoorAck result == " + result);
        for (IQDAccessMsgCallback callback : sCallbacks) {
            callback.onOpenDoorAck(result);
        }
    }

    /**
     * 通行记录回调
     *
     * @param passRecordEntities
     */
    public static void onPassRecordsAck(ArrayList<QDPassRecordEntity> passRecordEntities) {
        Logdeal.D(TAG, "onPassRecordsAck   == " + passRecordEntities);
        for (IQDAccessMsgCallback callback : sCallbacks) {
            callback.onPassRecordsAck(passRecordEntities);
        }
    }

    /**
     * 写卡结果回调
     *
     * @param result
     * @param cardNo
     */
    public static void onDistributeCardAck(int result, String cardNo) {
        Logdeal.D(TAG, "onDistributeCardAck  result == " + result + ", cardNo == " + cardNo);
        for (IQDAccessMsgCallback callback : sCallbacks) {
            callback.onDistributeCardAck(result, cardNo);
        }
    }

    /**
     * 读卡结果回调
     *
     * @param result
     * @param cardNo
     */
    public static void onReadCardAck(int result, String cardNo) {
        Logdeal.D(TAG, "onReadCardAck  result == " + result + ", cardNo == " + cardNo);
        for (IQDAccessMsgCallback callback : sCallbacks) {
            callback.onReadCardAck(result, cardNo);
        }
    }

    /**
     * 门禁设备当前升级状态回调
     *
     * @param software_version
     * @param hardware_version
     * @param software_version_num
     * @param current_update_file_pos
     * @param current_update_version_num
     */
    public static void onUpdateStateReportAck(String software_version, String hardware_version,
                                              int software_version_num, int current_update_file_pos, int current_update_version_num) {
        Logdeal.D(TAG, "onUpdateStateReportAck  software_version == " + software_version + ", hardware_version == " + hardware_version + ",software_version_num  = " + software_version_num);
        Logdeal.D(TAG, "onUpdateStateReportAck current_update_file_pos =  " + current_update_file_pos + " ,current_update_version_num = " + current_update_version_num);
        for (IQDAccessMsgCallback callback : sCallbacks) {
            callback.onUpdateStateReportAck(software_version, hardware_version, software_version_num, current_update_file_pos, current_update_version_num);
        }
    }

    /** 离线读卡回调
     * @param result
     * @param cardNo
     */
    public static void onStandaloneReadCardAck(int result, long cardNo){
        Logdeal.D(TAG, "onStandaloneReadCardAck  result == " + result + ", cardNo == " + cardNo);
        for (IQDAccessMsgCallback callback : sCallbacks) {
            callback.onStandaloneReadCardAck(result, cardNo);
        }
    }

    /**
     * 离线加卡回调
     * @param result
     */
    public static void onStandaloneAddCardAck(int result){
        Logdeal.D(TAG, "onStandaloneAddCardAck  result == " + result);
        for (IQDAccessMsgCallback callback : sCallbacks) {
            callback.onStandaloneAddCardAck(result);
        }
    }

    /**
     * 离线删卡回调
     * @param result
     */
    public static void onStandaloneDelCardAck(int result){
        Logdeal.D(TAG, "onStandaloneDelCardAck  result == " + result);
        for (IQDAccessMsgCallback callback : sCallbacks) {
            callback.onStandaloneDelCardAck(result);
        }
    }

    //**************jni 回调 end****************//
}
