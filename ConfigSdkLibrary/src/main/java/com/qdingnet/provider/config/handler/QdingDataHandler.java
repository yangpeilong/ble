package com.qdingnet.provider.config.handler;

import com.qdingnet.opendoor.core.bean.DataPacket;
import com.qdingnet.opendoor.core.callback.ICloseCallback;
import com.qdingnet.opendoor.core.callback.ICloseable;
import com.qdingnet.opendoor.core.handler.IDataHandler;
import com.qdingnet.provider.config.Logdeal;
import com.qdingnet.qdaccess.QDAccessMsgHandler;
import com.qdingnet.qdaccess.QDConfigMsgHandler;
import com.qdingnet.qdaccess.QDDeviceConfigEntity;
import com.qdingnet.qdaccess.QDPassRecordEntity;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.UUID;

/**
 * 千丁蓝牙数据处理器
 * @author pipan (pipan@qding.me)
 * @date   19-1-23
 */
public abstract class QdingDataHandler implements IDataHandler, ICloseCallback {
    private static final String TAG = "QdingDataHandler";
    //相关服务/特征通道对应标示
    private final static UUID UUID_WRITE_DATA = UUID.fromString("0000ffe9-0000-1000-8000-00805f9b34fb");

    //写数据是否完成
    static final int FLAG_WRITE_FINISH = 0x01;
    //读数据是否完成
    static final int FLAG_READ_FINISH = 0x02;
    //整个流程操作是否已完成
    static final int FLAG_ALL_FINISH = 0x04;

    //设备关闭接口工厂
    private ICloseable.Factory mICloseableFactory;
    //jni数据处理器
    private QDJniAccessMsgHandler mQDJniAccessMsgHandler;
    //是否开启配置操作
    private final boolean mEnableConfig;
    //设备配置jni数据处理器
    private QDJniConfigHandler mQDJniConfigHandler;
    //上一帧缓存数据
    private byte[] mLastFrameData;
    //操作状态
    private int mState;

    public QdingDataHandler(){
        this(false);
    }
    public QdingDataHandler(boolean mEnableConfig) {
        this.mEnableConfig = mEnableConfig;
    }

    /**
     * 设备操作关闭
     */
    protected void deviceClose() {
        if(mICloseableFactory != null){
            mICloseableFactory.create().close();
        }
    }

    @Override
    public void registerCloseableFactory(ICloseable.Factory factory) {
        if(factory != null){
            factory.registerCloseCallback(this);
            this.mICloseableFactory = factory;
        }
    }

    @Override
    public void onBeforeClose() {
        Logdeal.D(TAG, "onBeforeClose");
        removeJniCallback();
    }

    @Override
    public void onAfterClose() {
        Logdeal.D(TAG, "onAfterClose");
        mICloseableFactory = null;
    }

    /**
     * 检测空指针
     * @param target
     */
    protected void checkNull(Object target){
        if(target == null) throw new NullPointerException("The target is Null");
    }

    @Override
    public DataPacket handleStartSendData(String ssid) {
        DataPacket info = createSendPacketInfo(ssid);
        if(info != null){
            info.setServiceUUID(QdingGattServiceAuthHandler.UUID_UART_SERVICE);
            info.setCharacteristicUUID(UUID_WRITE_DATA);
            //注册jni数据处理器
            mQDJniAccessMsgHandler = new QDJniAccessMsgHandler(ssid);
            QDAccessMsgHandler.registCallback(mQDJniAccessMsgHandler);
            //注册设备配置jni数据处理器
            if(mEnableConfig){
                mQDJniConfigHandler = new QDJniConfigHandler(ssid);
                QDConfigMsgHandler.registerCallback(mQDJniConfigHandler);
            }
        }
        return info;
    }

    /**
     * 创建需发送的数据
     * @param ssid
     * @return
     */
    abstract DataPacket createSendPacketInfo(String ssid);

    @Override
    public void handleSendDataFinish(String ssid, DataPacket packetInfo) {
        setStateFlag(FLAG_WRITE_FINISH);
    }

    /**
     * 删除jni数据处理器
     */
    private void removeJniCallback(){
        if(mQDJniAccessMsgHandler != null){
            QDAccessMsgHandler.unregistCallback(mQDJniAccessMsgHandler);
        }
        if(mQDJniConfigHandler != null){
            QDConfigMsgHandler.unregisterCallback(mQDJniConfigHandler);
        }
    }

    @Override
    public void handleDateWrite(String ssid, int status) {

    }

    @Override
    public void handleDataChanged(String ssid, final byte[] data) {
        //保证处理数据的线程安全
        synchronized (QDAccessMsgHandler.class){
            if(mLastFrameData == null || !Arrays.equals(mLastFrameData, data)){
                mLastFrameData = data;
                final String mac = QdingDataPacketHandler.getDeviceMac(ssid);
                QDAccessMsgHandler.QDAccessJNIPushData(mac, data);
            }
        }
    }

    @Override
    public void handleDataRead(String ssid, byte[] data, int status) {

    }

    /**
     * 设置完成状态标志
     * @param flag @see {@link #FLAG_READ_FINISH}, {@link #FLAG_WRITE_FINISH}, {@link #FLAG_ALL_FINISH}
     */
    protected void setStateFlag(int flag){
        mState |= flag;
    }

    @Override
    public boolean writeFinish() {
        return (mState & FLAG_WRITE_FINISH) > 0;
    }

    @Override
    public boolean readFinish() {
        return (mState & FLAG_READ_FINISH) > 0;
    }

    @Override
    public boolean isSuccess() {
        return (mState & FLAG_ALL_FINISH) > 0;
    }

    /*********************************************************************
     *************************蓝牙回复数据jni的回调处理************************
     **********************************************************************/
    private final class QDJniAccessMsgHandler implements QDAccessMsgHandler.IQDAccessMsgCallback{
        private String ssid;

        public QDJniAccessMsgHandler(String ssid) {
            this.ssid = ssid;
        }

        @Override
        public void onReceiveAck(int result, byte cmdType) {
            handleReceiveAck(ssid, result, cmdType);
        }

        @Override
        public void onOpenDoorAck(int result) {
            Logdeal.D(TAG, "onOpenDoorAck result == " + result);
            handleOpenDoorAck(ssid, result);
        }

        @Override
        public void onPassRecordsAck(final ArrayList<QDPassRecordEntity> passRecordEntities) {
            Logdeal.D(TAG, "onPassRecordsAck size == " + passRecordEntities.size());
            handlePassRecordsAck(ssid, passRecordEntities);
        }

        @Override
        public void onDistributeCardAck(final int result, final String cardNo) {
            Logdeal.D(TAG, "onDistributeCardAck result == " + result + " cardNo = " + cardNo);
            handleDistributeCardAck(ssid, result, cardNo);
        }

        @Override
        public void onReadCardAck(final int result, final String cardNo) {
            Logdeal.D(TAG, "onReadCardAck result == " + result + " cardNo = " + cardNo);
            handleReadCardAck(ssid, result, cardNo);
        }

        @Override
        public void onUpdateStateReportAck(String software_version, String hardware_version, int software_version_num, int current_update_file_pos, int current_update_version_num) {
            Logdeal.D(TAG, "onUpdateStateReportAck software_version == " + software_version + " hardware_version = " + hardware_version
                    + " current_update_file_pos = " + current_update_file_pos + " current_update_version_num = " + current_update_version_num);
            handleUpdateStateReportAck(ssid, software_version, hardware_version, software_version_num, current_update_file_pos, current_update_version_num);
        }

        @Override
        public void onStandaloneReadCardAck(int result, long cardNo) {
            Logdeal.D(TAG, "onStandaloneReadCardAck result == " + result + " cardNo = " + cardNo);
            handleStandaloneCardReadAck(ssid, result, cardNo);
        }

        @Override
        public void onStandaloneAddCardAck(int result) {
            Logdeal.D(TAG, "onStandaloneAddCardAck result == " + result);
            handleStandaloneCardAddAck(ssid, result);
        }

        @Override
        public void onStandaloneDelCardAck(int result) {
            Logdeal.D(TAG, "onStandaloneDelCardAck result == " + result);
            handleStandaloneCardDelAck(ssid, result);
        }
    }

    /**
     * 操作结果成功与否回调
     * @param ssid
     * @param result
     * @param cmdType
     */
    protected void handleReceiveAck(String ssid, int result, byte cmdType) {

    }
    /**
     * 开门结果回调
     * @param ssid
     * @param result
     */
    protected void handleOpenDoorAck(String ssid, int result) {

    }

    /**
     * 开门记录结果回调
     * @param ssid
     * @param passRecordEntities
     */
    protected void handlePassRecordsAck(String ssid, final ArrayList<QDPassRecordEntity> passRecordEntities) {
    }

    /**
     * 设备固件升级结果回调
     * @param ssid
     * @param software_version
     * @param hardware_version
     * @param software_version_num
     * @param current_update_file_pos
     * @param current_update_version_num
     */
    protected void handleUpdateStateReportAck(String ssid, String software_version, String hardware_version, int software_version_num, int current_update_file_pos, int current_update_version_num) {

    }

    /**
     * 在线发卡结果回调
     * @param ssid
     * @param result
     * @param cardNo
     */
    protected void handleDistributeCardAck(String ssid, final int result, final String cardNo) {

    }

    /**
     * 在线读卡结果回调
     * @param ssid
     * @param result
     * @param cardNo
     */
    protected void handleReadCardAck(String ssid, final int result, final String cardNo) {

    }

    /**
     * 离线读卡结果回调
     * @param ssid
     * @param result
     * @param cardNo
     */
    protected void handleStandaloneCardReadAck(String ssid, final int result, final long cardNo){

    }

    /**
     * 离线添加卡结果回调
     * @param ssid
     * @param result
     */
    protected void handleStandaloneCardAddAck(String ssid, final int result){

    }

    /**
     * 离线删卡结果回调
     * @param ssid
     * @param result
     */
    protected void handleStandaloneCardDelAck(String ssid, final int result){

    }


    /*********************************************************************
     *************************设备配置jni的回调处理************************
     **********************************************************************/
    private final class QDJniConfigHandler implements QDConfigMsgHandler.IQDConfigMsgHandler{
        private String ssid;

        public QDJniConfigHandler(String ssid) {
            this.ssid = ssid;
        }
        @Override
        public void onWriteConfigAck(int result) {
            handleWriteConfigAck(ssid, result);
        }

        @Override
        public void onReadConfigAck(QDDeviceConfigEntity entity) {
            handleReadConfigAck(ssid, entity);
        }

        @Override
        public void onGetVersionAck(String softwareVersion, String hardwareVersion) {
            handleGetVersionAck(ssid, softwareVersion, hardwareVersion);
        }

        @Override
        public void onUpdateDeviceAck(int result) {
            handleUpdateDeviceAck(ssid, result);
        }
    }

    /**
     * 设备写配置回调
     * @param result
     */
    protected void handleWriteConfigAck(String ssid, int result) {

    }

    /**
     * 读设备配置回调
     * @param entity
     */
    protected void handleReadConfigAck(String ssid, QDDeviceConfigEntity entity) {

    }

    /**
     * 获取设备版本信息回调
     * @param softwareVersion
     * @param hardwareVersion
     */
    protected void handleGetVersionAck(String ssid, String softwareVersion, String hardwareVersion) {

    }

    /**
     * 发送设备升级指令后回调
     * @param result
     */
    protected void handleUpdateDeviceAck(String ssid, int result) {

    }
}
