package com.qdingnet.qdaccess;

public class ProtocolHelper {
    private final static String TAG = "ProtocolHelper";

    static {
        try {
            System.loadLibrary("qdaccess");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    //////////////////////////////////////////安全数据加解密/////////////////////////////////////////

    /**
     * 注册控件
     * @param context
     * @return -1:failure,0:no auth,1:ok
     */
    public static native int register(Object context);

    /**
     * 注册控件
     * @param context
     * @return -1:failure,0:no auth,1:ok
     */
    public static native int register(Object context, byte[] authMessage);

    /**
     * 加密
     * @param src
     * @return
     */
    public static native byte[] encrypt(byte[] src);

    /**
     * 解密
     * @param target
     * @return
     */
    public static native byte[] decrypt(byte[] target);

    //////////////////////////////////////////门禁数据解析/////////////////////////////////////////

    /**
     * 初始化
     */
    public static void init() {
        QDAccessMsgHandler.QDAccessJNIModelInit();
    }

    /**
     * 将数据推入门禁so库进行解析
     * @param broadcast
     * @param targetDeviceMac
     * @param data
     */
    public static void pushRawData(boolean broadcast, String targetDeviceMac, byte[] data){
        if(broadcast){
            QDAccessMsgHandler.QDAccessJNIPushBroadcastData(targetDeviceMac, data);
        }else {
            QDAccessMsgHandler.QDAccessJNIPushData(targetDeviceMac, data);
        }
    }


    //////////////////////////////////////////门禁指令构造/////////////////////////////////////////

    /**
     * 彼得J温伯格 HASH算法
     *
     * @param packet
     * @return
     */
    public static native int PJWHash(byte[] packet);

    public static native int Do_Calc_CheckSum(byte[] packet);

    ////////////////////////////////////////// JNI V1.O /////////////////////////////////////////

    /**
     * 构建获取设备版本信息数据包
     *
     * @param target_mac 目标设备的mac地址
     * @return 升级消息数据包
     */
    public static native byte[] build_get_device_version_msg(String target_mac);

    /**
     * 构建获取设备版本信息数据包
     *
     * @param target_mac 目标设备的mac地址
     * @return 升级消息数据包
     */
    public static native byte[] build_get_device_config_msg(String target_mac);

    /**
     * 构建升级设备息数据包
     *
     * @param package_download_path 升级包下载位置
     * @return 升级消息数据包
     */
    public static native byte[] build_update_device_msg(String package_download_path);

    /**
     * 构建开锁消息数据包
     *
     * @param reported_record_index 本机记录的，当前通信设备通行记录，已上传到服务器index
     * @param cacahed_record_index  本机已缓存的，当前通信设备通行记录index
     * @param mac_list              可以通行的设备mac列表
     * @param keep_seconds          开锁持续时间
     * @param app_user_id           用户的appID
     * @param room_id               所开设备所属的房间ID
     * @param server_id             服务器ID
     * @param locations             用户所绑定的位置，只需到单元
     * @return 开锁数据包
     */
    public static native byte[] build_open_door_msg_with_locations(int reported_record_index, int cacahed_record_index,
                                                                   String mac_list, int keep_seconds, String app_user_id, String room_id, String server_id, String locations);

    /**
     * 构建开锁消息数据包
     *
     * @param reported_record_index 本机记录的，当前通信设备通行记录，已上传到服务器index
     * @param cacahed_record_index  本机已缓存的，当前通信设备通行记录index
     * @param mac_list              可以通行的设备mac列表
     * @param keep_seconds          开锁持续时间
     * @param app_user_id           用户的appID
     * @param room_id               所开设备所属的房间ID
     * @param server_id             服务器ID
     * @return 开锁数据包
     */
    public static native byte[] build_open_door_msg(int reported_record_index, int cacahed_record_index,
                                                    String mac_list, int keep_seconds, String app_user_id, String room_id, String server_id);

    /**
     * 构建配置设备息数据包
     * @param keep_time
     * @param device_list
     * @param max_doors_size
     * @param max_counter_size
     * @return
     */
    public static native byte[] build_config_device_msg(int keep_time, String device_list, int max_doors_size, int max_counter_size);

    /**
     * 生成密码
     *
     * @param community
     * @param build_unit
     * @param max_doors_size
     * @param max_counter_size
     * @return
     */
    public static native int genarate_key(String community, String build_unit, int max_doors_size,
                                          int max_counter_size);

    ////////////////////////////////////////// JNI V2.O /////////////////////////////////////////

    /**
     * 构建配置设备息数据包
     * @param target_mac
     * @param keep_time
     * @param project_group_build_units
     * @param relay_state
     * @param auth_code
     * @return
     */
    public static native byte[] build_config_device_msg_v2(String target_mac, byte keep_time, QDUnitItem[] project_group_build_units, byte relay_state, long phone, String auth_code);

    /**
     * 构建获取设备版本信息数据包
     *
     * @param target_mac 目标设备的mac地址
     * @return 升级消息数据包
     */
    public static native byte[] build_get_device_version_msg_v2(String target_mac);

    /**
     * 构建获取设备版本信息数据包
     *
     * @param target_mac 目标设备的mac地址
     * @return 升级消息数据包
     */
    public static native byte[] build_get_device_config_msg_v2(String target_mac);

    /**
     * 构建升级设备息数据包
     *
     * @param package_download_path 升级包下载位置
     * @return 升级消息数据包
     */
    public static native byte[] build_update_device_msg_v2(String target_mac, String package_download_path);

    /**
     * 构建开锁消息数据包(带需注销卡的信息)
     *
     * @param target_mac
     * @param keep_time
     * @param app_user_id
     * @return
     */
    public static native byte[] build_open_door_msg_with_cards_v2(String target_mac, byte keep_time, long app_user_id, String auth_code, QDRFCardItem[] cards);

    /**
     * 构建开锁消息数据包(带用户的位置信息)
     * @param target_mac
     * @param keep_time
     * @param app_user_id
     * @param project_group_build_units
     * @return
     */
    public static native byte[] build_open_door_msg_with_locations_v2(String target_mac, byte keep_time, long app_user_id, String auth_code, QDUnitItem[] project_group_build_units);

    /**
     * 构建开锁消息数据包
     * @param target_mac
     * @param keep_time
     * @param app_user_id
     * @return
     */
    public static native byte[] build_open_door_msg_v2(String target_mac, byte keep_time, long app_user_id, String auth_code);

    /**
     * 构建带写卡信息的开门数据包
     *
     * @param target_mac
     * @param keep_time
     * @param app_user_id
     * @param cards_info
     * @return
     */
    public static native byte[] build_open_door_msg_with_distribute_cards_info_v2(String target_mac, byte keep_time, long app_user_id, String auth_code, int wait_time_pre_card, String cards_info);

    /**
     * 构建广播消息
     * @param target_mac
     * @param app_user_id
     * @return
     */
    public static native String build_phone_advertise_msg_v2(String target_mac, long app_user_id, byte[] floors, String auth_code);


    /**
     * 生成密码
     * @param unit_id
     * @return
     */
    public static native int generate_key_v2(long unit_id);

    /**
     * 在线发卡模块: 构建读卡数据
     *
     * @param target_mac
     * @return
     */
    public static native byte[] build_protocol_get_card_uid_msg_v2(String target_mac);


    /**
     * 在线发卡模块: 构建写卡数据
     *
     * @param target_mac
     * @param write_data
     * @return
     */
    public static native byte[] build_protocol_write_card_data_msg_v2(String target_mac, int wait_time_pre_card, String write_data);


    /**
     * 在线发卡模块: 构建取消写卡数据包
     *
     * @param target_mac
     * @return
     */
    public static native byte[] build_protocol_cancel_write_card_data_msg_v2(String target_mac);

    /**************************************离线发卡功能****************************************************/
    /**
     * 离线发卡模块: 读取卡号
     * @param target_mac
     * @param timeout
     * @return
     */
    public static native byte[] build_protocol_standalone_read_cardno_msg_v2(String target_mac, int timeout);

    /**
     * 离线发卡模块: 添加卡号
     * @param target_mac
     * @param cardNo
     * @param validity
     * @param floors
     * @return
     */
    public static native byte[] build_protocol_standalone_add_cardno_msg_v2(String target_mac, long cardNo, long validity, byte[] floors);


    /**
     * 离线发卡模块: 删除卡号
     * @param target_mac
     * @param cardNos
     * @return
     */
    public static native byte[] build_protocol_standalone_delete_cardno_msg_v2(String target_mac, long[] cardNos);

    /**********************************二代设备工厂测试相关接口****************************************/
    /**
     * 构建433测试消息
     * @param target_mac
     * @return
     */
    public static native byte[] build_enable_433_test_msg_v2(String target_mac);
    /**
     * 构建获取霍尔开门状态消息
     * @param target_mac
     * @return
     */
    public static native byte[] build_get_hal_state_msg_v2(String target_mac);


    ////////////////////////////////////////// JNI V4.O /////////////////////////////////////////

    /**
     * 构建广播消息
     * @param target_mac
     * @param app_user_id
     * @param pass_id
     * @return
     */
    public static native String build_phone_advertise_msg_v4(String target_mac, long app_user_id, int pass_id);

    /**
     * 构建开锁消息
     *
     * @param target_mac
     * @param keep_time
     * @param app_user_id
     * @param pass_id
     * @return
     */
    public static native byte[] build_open_door_msg_v4(String target_mac, byte keep_time, long app_user_id, int pass_id, String auth_code);

    /**
     * 构建配置设备息数据包
     *
     * @param target_mac
     * @param auth_code
     * @return
     */
    public static native byte[] build_config_device_msg_v4(String target_mac, String auth_code);

    ////////////////////////////////////////// JNI V5.O /////////////////////////////////////////

    /**
     * 构建开锁消息
     *
     * @param target_mac
     * @param keep_time
     * @param app_user_id
     * @return
     */
    public static native byte[] build_open_door_msg_v5(String target_mac, byte keep_time, long app_user_id);

    /**
     * 构建广播消息
     *
     * @param target_mac
     * @param app_user_id
     * @param floors
     * @return
     */
    public static native String build_phone_advertise_msg_v5(String target_mac, long app_user_id, byte[] floors);


    /**********************************四代设备工厂测试相关接口****************************************/
    /**
     * 获取设备
     *
     * @param target_mac
     * @return
     */
    public static native byte[] build_get_device_info_msg_v4(String target_mac);

    /**********************************五代设备工厂测试相关接口****************************************/
    /**
     * 构建获取门磁状态消息
     * @param target_mac
     * @return
     */
    public static native byte[] build_get_gate_magnetism_state_msg_v5(String target_mac);
}
