package com.qdingnet.provider.config.handler;

import android.util.Base64;

import com.qdingnet.opendoor.utils.Hex;
import com.qdingnet.provider.config.Logdeal;
import com.qdingnet.provider.config.bean.Contants.*;
import com.qdingnet.provider.config.bean.DeviceConfigMsg;
import com.qdingnet.provider.config.bean.GateBuildUnitInfo;
import com.qdingnet.provider.config.secure.SecureProvider;
import com.qdingnet.qdaccess.ProtocolHelper;
import com.qdingnet.qdaccess.QDDeviceConfigEntity;
import com.qdingnet.qdaccess.QDRFCardItem;
import com.qdingnet.qdaccess.QDUnitItem;

import java.util.ArrayList;
import java.util.List;

/**
 * 千丁智能门禁数据处理器
 * @author pipan (pipan@qding.me)
 * @date   19-1-23
 */
public class QdingDataPacketHandler {
    private final static String TAG = "QdingDataPacketHandler";
    //门禁设备ssid的匹配规则
    private final static String DEVICE_SSID_REGEX = "^QD[_A-Z].+";
    //默认开门时长(ms)
    private static final int DEFAULT_OPENDOOR_KEEP_TIME = 3000;
    //最小开门时长(ms)
    private static final int MIN_OPENDOOR_KEEP_TIME = 1_000;
    //最大开门时长(ms)
    private static final int MAX_OPENDOOR_KEEP_TIME = 30_000;

    private QdingDataPacketHandler() {}

    /**
     * 创建passId
     * @return
     */
    public static int createPassId(){
        return (int)(System.currentTimeMillis() / 1000);
    }

    /**
     * 获取门禁设备的mac地址
     * @param ssid
     * @return
     */
    public static String getDeviceMac(String ssid) {
        String mac = null;
        if (ssid != null && ssid.matches(DEVICE_SSID_REGEX)) {
            char tag = ssid.charAt(2);
            //一代设备
            if ('_' == tag) {
                mac = ssid.substring(3);
            }
            //二代及以上门禁
            else {
                String baseMac = ssid.substring(3);
                int length = baseMac.length();
                if (length >= 8) {
                    byte[] byteMac = Base64.decode(baseMac.substring(0, 8), Base64.NO_PADDING);
                    mac = new String(Hex.encodeHex(byteMac)).toUpperCase();
                }
            }
        }
        return mac;
    }

    /**
     * 构建开门数据包
     * @param userId
     * @param mRFCards
     * @return
     */
    public static byte[] getOpendoorDataPacket(final DeviceEdition deviceEdition, final String mac, final int passId, final long userId, final List<QDRFCardItem> mRFCards, final String authCode) {
        return getOpendoorDataPacket(deviceEdition, mac, passId, userId, 0, mRFCards, authCode);
    }

    /**
     * 构建开门数据包
     * @param deviceEdition
     * @param mac
     * @param passId
     * @param userId
     * @param openDoorKeepTime
     * @param mRFCards
     * @return
     */
    public static byte[] getOpendoorDataPacket(final DeviceEdition deviceEdition, final String mac, final int passId, final long userId, final int openDoorKeepTime, final List<QDRFCardItem> mRFCards, final String authCode) {
        byte[] packet = null;
        try {
            //开门时长
            int open_keep_timer = DEFAULT_OPENDOOR_KEEP_TIME;
            if(openDoorKeepTime >= MIN_OPENDOOR_KEEP_TIME && openDoorKeepTime <= MAX_OPENDOOR_KEEP_TIME){
                open_keep_timer = openDoorKeepTime;
            }
            if(deviceEdition != null){
                //1代设备
                if (DeviceEdition.V1 == deviceEdition) {
                    Logdeal.D(TAG, "getOpendoorDataPacket...DeviceEdition:V1,open_keep_timer :" + open_keep_timer);
                    packet = ProtocolHelper.build_open_door_msg(0, 0, mac, open_keep_timer, String.valueOf(userId), "0", "0");
                }
                //2，3代设备
                else if (DeviceEdition.V2 == deviceEdition || DeviceEdition.V3 == deviceEdition) {
                    byte keep_time = (byte) (open_keep_timer / 500);
                    long app_user_id = userId;
                    Logdeal.D(TAG, "getOpendoorDataPacket...DeviceEdition:V2,keep_time:" + keep_time + ",app_user_id:" + app_user_id);
                    String decodeAuthCode = SecureProvider.decrypt(authCode);
                    if (mRFCards != null && mRFCards.size() > 0) {
                        packet = ProtocolHelper.build_open_door_msg_with_cards_v2(mac, keep_time, app_user_id, decodeAuthCode, mRFCards.toArray(new QDRFCardItem[0]));
                    } else {
                        packet = ProtocolHelper.build_open_door_msg_v2(mac, keep_time, app_user_id, decodeAuthCode);
                    }
                }
                //4代设备
                else if (DeviceEdition.V4 == deviceEdition) {
                    byte keep_time = (byte) (open_keep_timer / 500);
                    long app_user_id = userId;
                    Logdeal.D(TAG, "getOpendoorDataPacket...DeviceEdition:V4, open_keep_timer :" + open_keep_timer + ",app_user_id:" + app_user_id + ",passId:" + passId);
                    packet = ProtocolHelper.build_open_door_msg_v4(mac, keep_time, app_user_id, passId, SecureProvider.decrypt(authCode));
                }
                //5代设备
                else if (DeviceEdition.V5 == deviceEdition) {
                    byte keep_time = (byte) (open_keep_timer / 500);
                    long app_user_id = userId;
                    Logdeal.D(TAG, "getOpendoorDataPacket...DeviceEdition:V5, open_keep_timer :" + open_keep_timer + ",app_user_id:" + app_user_id);
                    packet = ProtocolHelper.build_open_door_msg_v5(mac, keep_time, app_user_id);
                }
            }
        } catch (Exception e) {
            Logdeal.D(TAG, " getOpendoorDataPacket...Exception:", e);
        }
        return packet;
    }

    /**
     * 获取手机端广播出去的数据包
     * @param deviceEdition
     * @param device_mac
     * @param passId
     * @param userId
     * @return
     */
    public static String getAdvertiseOpendoorDataPacket(final DeviceEdition deviceEdition, final String device_mac, final int passId, final long userId, final String authCode) {
        String packet = null;
        try {
            Logdeal.D(TAG, " getAdvertiseOpendoorDataPacket deviceEdition:" + deviceEdition + ",mac:" + device_mac + ",passId:" + passId + ",userId:" + userId);
            //一二三代设备
            if(deviceEdition.compareTo(DeviceEdition.V4) < 0){
                packet = ProtocolHelper.build_phone_advertise_msg_v2(device_mac, userId, null, SecureProvider.decrypt(authCode));
            }
            //四代设备
            else if (deviceEdition == DeviceEdition.V4) {
                packet = ProtocolHelper.build_phone_advertise_msg_v4(device_mac, userId, passId);
                if (packet != null) {
                    packet = "@#$" + packet;
                }
            }
            //五代设备
            else if (deviceEdition == DeviceEdition.V5) {
                packet = ProtocolHelper.build_phone_advertise_msg_v5(device_mac, userId, null);
            }
        } catch (Exception e) {
            Logdeal.D(TAG, " getAdvertiseOpendoorDataPacket...Exception:", e);
        }
        return packet;
    }

    /*****************************在线读写卡功能********************************/

    public static byte[] buildProtocolWriteCardDataMsgV2(String target_mac, int wait_time_pre_card, String write_data) {
        try {
            return ProtocolHelper.build_protocol_write_card_data_msg_v2(target_mac, wait_time_pre_card, write_data);
        } catch (Exception e) {
            Logdeal.D(TAG, " buildProtocolUpdateFileDescMsgV2...Exception:", e);
        }
        return null;
    }


    public static byte[] buildProtocolCancelWriteCardDataMsgV2(String target_mac) {
        try {
            return ProtocolHelper.build_protocol_cancel_write_card_data_msg_v2(target_mac);
        } catch (Exception e) {
            Logdeal.D(TAG, " buildProtocolUpdateFileDescMsgV2...Exception:", e);
        }
        return null;
    }

    public static byte[] buildProtocolGetCardUidMsgV2(String target_device_mac) {
        try {
            return ProtocolHelper.build_protocol_get_card_uid_msg_v2(target_device_mac);
        } catch (Exception e) {
            Logdeal.D(TAG, " buildProtocolUpdateFileDescMsgV2...Exception:", e);
        }
        return null;
    }

    /*****************************离线发卡功能********************************/
    public static byte[] buildStandaloneReadCardNoMsg(String mac, int timeout){
        try {
            return ProtocolHelper.build_protocol_standalone_read_cardno_msg_v2(mac, timeout);
        } catch (Exception e) {
            Logdeal.D(TAG, " buildStandaloneReadCardNoMsg...Exception:", e);
        }
        return null;
    }

    public static byte[] buildStandaloneAddCardNoMsg(String mac, long cardNo, long validity, int[] floors){
        try {
            int size = floors != null ? floors.length : 0;
            byte[] bFloors = new byte[size];
            for(int i=0; i<size; i++){
                bFloors[i] = (byte)floors[i];
            }
            return ProtocolHelper.build_protocol_standalone_add_cardno_msg_v2(mac, cardNo, validity, bFloors);
        } catch (Exception e) {
            Logdeal.D(TAG, " buildStandaloneAddCardNoMsg...Exception:", e);
        }
        return null;
    }

    public static byte[] buildStandaloneDelCardNoMsg(String mac, long[] cardNos){
        try {
            return ProtocolHelper.build_protocol_standalone_delete_cardno_msg_v2(mac, cardNos);
        } catch (Exception e) {
            Logdeal.D(TAG, " buildStandaloneDelCardNoMsg...Exception:", e);
        }
        return null;
    }

    /*****************************设备配置功能********************************/

    /**
     * 构建配置设备消息
     * @param deviceEdition
     * @param account
     * @param mac
     * @param openDoorKeepTime
     * @param gateBuildUnitInfoList
     * @param relayOn
     * @param authcode
     * @return
     */
    public static byte[] buildDeviceConfigMsg(DeviceEdition deviceEdition, String account, String mac, int openDoorKeepTime, List<GateBuildUnitInfo> gateBuildUnitInfoList, boolean relayOn, String authcode){
        byte[] packet = null;
        try {
            //开门时长
            int open_keep_timer = DEFAULT_OPENDOOR_KEEP_TIME;
            if(openDoorKeepTime >= MIN_OPENDOOR_KEEP_TIME && openDoorKeepTime <= MAX_OPENDOOR_KEEP_TIME){
                open_keep_timer = openDoorKeepTime;
            }
            if(deviceEdition != null){
                //1代设备
                if (DeviceEdition.V1 == deviceEdition) {
                    String configUnitMsg = buildConfigUnitMsgV1(gateBuildUnitInfoList);
                    Logdeal.D(TAG, "buildDeviceConfigMsg...DeviceEdition:V1,open_keep_timer :" + open_keep_timer);
                    int size = gateBuildUnitInfoList != null ? gateBuildUnitInfoList.size() : 0;
                    packet = ProtocolHelper.build_config_device_msg(open_keep_timer, configUnitMsg, size, 100);
                }
                //2，3，5代设备
                else if (DeviceEdition.V2 == deviceEdition || DeviceEdition.V5 == deviceEdition) {
                    //开门时长
                    byte keep_time = (byte) (open_keep_timer / 500);
                    //单元信息
                    QDUnitItem[] unitItems = buildConfigUnitV2(gateBuildUnitInfoList);
                    //继电器状态
                    byte relayState = relayOn ? (byte) 2 : (byte) 1;
                    Logdeal.D(TAG, "buildDeviceConfigMsg...DeviceEdition:" + deviceEdition + ",keep_time:" + keep_time + ",relayState:" + relayState + ",authcode:" + authcode);
                    //设备识别码
                    String decodeAuthCode = SecureProvider.decrypt(authcode);
                    //设备配置用户
                    long phone = transformMobile(account);
                    //构建数据包
                    packet = ProtocolHelper.build_config_device_msg_v2(mac, keep_time, unitItems, relayState, phone, decodeAuthCode);
                }
                //4代设备
                else if (DeviceEdition.V4 == deviceEdition) {
                    Logdeal.D(TAG, "buildDeviceConfigMsg...DeviceEdition:V4, authcode:" + authcode);
                    packet = ProtocolHelper.build_config_device_msg_v4(mac,SecureProvider.decrypt(authcode));
                }
            }
        } catch (Exception e) {
            Logdeal.D(TAG, " buildDeviceConfigMsg...Exception:", e);
        }
        return packet;
    }

    /**
     * 手机号转换
     * @param mobile
     * @return
     */
    private static long transformMobile(String mobile){
        long phone = 0;
        if(mobile != null && mobile.matches("\\d{11}")){
            if(mobile.equals("12345678901")){
                phone = 0xffffffff;
            }else {
                long head = Long.valueOf(mobile.substring(0,2));
                long tail = Long.valueOf(mobile.substring(2));
                if(head == 13){
                    head = 0;
                }else if(head == 15){
                    head = 1;
                }else if(head == 17){
                    head = 2;
                }else if(head == 18){
                    head = 3;
                }else {
                    head = 0;
                }
                phone = (head << 30) + tail;
            }
        }
        return phone;
    }

    /**
     * 构建1代配置单元信息
     * @param gateBuildUnitInfoList
     * @return
     */
    private static String buildConfigUnitMsgV1(List<GateBuildUnitInfo> gateBuildUnitInfoList){
        StringBuilder msgBuilder = new StringBuilder();
        if(gateBuildUnitInfoList != null){
            for (int i = 0, size=gateBuildUnitInfoList.size(); i < size; i++) {
                GateBuildUnitInfo gbui = gateBuildUnitInfoList.get(i);
                //第一部分为社区信息，是公共部分
                if (i == 0) {
                    msgBuilder.append(gbui.province)
                              .append(gbui.city)
                              .append(gbui.community)
                              .append("@");
                }
                //第二部分楼栋单元信息
                msgBuilder.append(gbui.build)
                          .append("@")
                          .append(gbui.unit)
                          .append("@");
            }
        }
        return msgBuilder.toString();
    }

    /**
     * 构建2代配置单元信息
     * @param gateBuildUnitInfoList
     * @return
     */
    private static QDUnitItem[] buildConfigUnitV2(List<GateBuildUnitInfo> gateBuildUnitInfoList){
        int size = gateBuildUnitInfoList != null ? gateBuildUnitInfoList.size() : 0;
        QDUnitItem[] unitItems = new QDUnitItem[size];
        for(int i=0 ; i<size; i++) {
            GateBuildUnitInfo gbui = gateBuildUnitInfoList.get(i);
            unitItems[i] = new QDUnitItem(gbui.unit_id, gbui.password_num / 2, gbui.password_num / 2);
        }
        return unitItems;
    }

    /**
     * 解析设备配置单元信息
     * @param edition
     * @param entity
     * @return
     */
    public static DeviceConfigMsg parseDeviceConfig(DeviceEdition edition, QDDeviceConfigEntity entity){
        DeviceConfigMsg configMsg = null;
        if(entity != null){
            List<GateBuildUnitInfo> gateBuildUnitInfoList = new ArrayList<>();
            int openKeepTime = entity.open_keep_time;
            //1代设备
            if (DeviceEdition.V1 == edition) {
                final String msg = entity.related_doors;
                if(msg != null && msg.contains("@")){
                    //v1.1.0版本前配置位置信息组织方式
                    if("V1.1.0".compareTo(entity.software_version) < 0){
                        //截取公共部分
                        int index = msg.indexOf("@");
                        String commonMsg = msg.substring(0, index-1);
                        //截取楼栋单元部分
                        String subMsg = msg.substring(index+1);
                        String[] buildUnits = subMsg.split("@");
                        for(int i=0,size=buildUnits.length/2; i<size; i+=2){
                            gateBuildUnitInfoList.add(new GateBuildUnitInfo(commonMsg, commonMsg, commonMsg, buildUnits[i], buildUnits[i+1]));
                        }
                    }
                    //v1.1.0版本及以后配置位置信息组织方式
                    else {
                        String[] buildUnits = msg.split("@");
                        for (int i = 0, size = buildUnits.length / 3; i < size; i += 3) {
                            gateBuildUnitInfoList.add(new GateBuildUnitInfo(buildUnits[i], buildUnits[i], buildUnits[i], buildUnits[i + 1], buildUnits[i + 2]));
                        }
                    }
                }
            }
            //2代/5代设备
            else if (DeviceEdition.V2 == edition || DeviceEdition.V5 == edition) {
                //开门时长转换
                openKeepTime *= 500;
                final List<QDUnitItem> unitItems = entity.related_units;
                if (unitItems != null) {
                    for (QDUnitItem item : unitItems) {
                        gateBuildUnitInfoList.add(new GateBuildUnitInfo(item.unit_id, item.multi_time_counter * 2));
                    }
                }
            }
            configMsg = new DeviceConfigMsg(openKeepTime, entity.device_time, entity.software_version, entity.hardware_version, gateBuildUnitInfoList);
        }
        return configMsg;
    }

    /**
     * 构建读取设备配置消息
     * @param deviceEdition
     * @param mac
     * @return
     */
    public static byte[] buildGetConfigMsg(DeviceEdition deviceEdition, String mac){
        byte[] packet = null;
        if(deviceEdition != null) {
            //1代设备
            if (DeviceEdition.V1 == deviceEdition) {
                packet = ProtocolHelper.build_get_device_config_msg(mac);
            }
            //2代/5代设备
            else if (DeviceEdition.V2 == deviceEdition || DeviceEdition.V5 == deviceEdition) {
                packet = ProtocolHelper.build_get_device_config_msg_v2(mac);
            }
        }
        return packet;
    }

    /**
     * 构建读取设备版本消息
     * @param deviceEdition
     * @param mac
     * @return
     */
    public static byte[] buildGetVersionMsg(DeviceEdition deviceEdition, String mac){
        byte[] packet = null;
        if(deviceEdition != null) {
            //1代设备
            if (DeviceEdition.V1 == deviceEdition) {
                packet = ProtocolHelper.build_get_device_version_msg(mac);
            }
            //2代/5代设备
            else if (DeviceEdition.V2 == deviceEdition || DeviceEdition.V5 == deviceEdition) {
                packet = ProtocolHelper.build_get_device_version_msg_v2(mac);
            }
        }
        return packet;
    }

    /**
     * 构建设备升级消息
     * @param deviceEdition
     * @param mac
     * @param path
     * @return
     */
    public static byte[] buildGetUpgradeMsg(DeviceEdition deviceEdition, String mac, String path){
        byte[] packet = null;
        if(deviceEdition != null) {
            //1代设备
            if (DeviceEdition.V1 == deviceEdition) {
                packet = ProtocolHelper.build_update_device_msg(path);
            }
            //2代/5代设备
            else if (DeviceEdition.V2 == deviceEdition || DeviceEdition.V5 == deviceEdition) {
                packet = ProtocolHelper.build_update_device_msg_v2(mac, path);
            }
        }
        return packet;
    }

    /**
     * 生成设备开门密码
     * @param deviceEdition
     * @param buildUnitInfo
     * @return
     */
    public static int generateKey(DeviceEdition deviceEdition, GateBuildUnitInfo buildUnitInfo){
        //1代设备
        if (DeviceEdition.V1 == deviceEdition) {
            String community = (buildUnitInfo.province != null ? buildUnitInfo.province : "") + (buildUnitInfo.city != null ? buildUnitInfo.city : "") + (buildUnitInfo.community != null ? buildUnitInfo.community : "");
            String unit = (buildUnitInfo.build != null ? buildUnitInfo.build : "") + (buildUnitInfo.unit != null ? buildUnitInfo.unit : "");
            return ProtocolHelper.genarate_key(community, unit, 100, 100);
        }
        //2代/5代设备
        else if (DeviceEdition.V2 == deviceEdition || DeviceEdition.V5 == deviceEdition) {
            return ProtocolHelper.generate_key_v2(buildUnitInfo.unit_id);
        }
        return 0;
    }
}
