package com.qdingnet.provider.config.bean;

import android.text.TextUtils;

public class Contants {

    public interface QDWifi{
        byte[] PW = {97, 98, 99, 49, 50, 51, 52, 53, 54};
        int LOCAL_FIRMWARE_DOWNLOAD_SERVER_PORT = 10080;
    }

    public static class OpenDoorResultType {
        public static final int OPEN_DOOR_SUCCESS = 0x71;
        public static final int OPEN_DOOR_FAILURE = 0x72;
        public static final int OPEN_DOOR_BUSY = 0x73;
        public static final int OPEN_DOOR_FREQUENT = 0x74;
        public static final int OPEN_DOOR_AGCY_ERROR = 0x76;
    }

    public static class QDCmdType {
        public static final byte QD_CMD_OPEN_DOOR = 0X05;
        public static final byte QD_CMD_DISTRIBUTE_CARD = 0x12;
        public static final byte QD_CMD_CANCEL_DISTRIBUTE_CARD = 0x13;
        public static final byte QD_CMD_READ_CARD_UID = 0X14;
        public static final byte QD_CMD_UPDATE_FILE_DESC = 0X15;
        public static final byte QD_CMD_UPDATE_FILE_TRANSPORT = 0X17;
        //离线读卡
        public static final byte QD_CMD_STANDALONE_READ_CARD = 0X30;
        //离线添卡
        public static final byte QD_CMD_STANDALONE_ADD_CARD = 0X31;
        //离线删卡
        public static final byte QD_CMD_STANDALONE_DEL_CARD = 0X32;

        ///////////////////////////设备配置/////////////////////////////
        public static final byte QD_CMD_CONFIG_DEVICE = 0x06;
        public static final byte QD_CMD_UPDATE_SYSTEM  = 0X07;
        public static final byte QD_CMD_GET_VERSION = 0x09;
        public static final byte QD_CMD_GET_CONFIG = 0x0A;
        public static final byte QD_CMD_CONFIG_DEVICE_AGCY = 0x2A;
    }

    /**
     * 门禁设备版本标示: 若无法找到相应的版本标示,需提示用户升级app以适应门禁新固件的升级
     */
    public enum DeviceEdition {
        V1("_"), V2("A"), V3("B"), V4("C"), V5("D");
        private String tag;
        private String firmware_version;

        DeviceEdition(String tag) {
            this.tag = tag;
        }

        public static DeviceEdition getEdition(String firmware_version) {
            DeviceEdition deviceEdition = null;
            if (firmware_version != null && firmware_version.matches("^[Vv]\\d+.\\d+.\\d+")) {
                try {
                    String major = firmware_version.substring(0, firmware_version.indexOf("."));
                    deviceEdition = valueOf(major.toUpperCase());
                    deviceEdition.firmware_version = firmware_version;
                } catch (Exception e) {
                }
            }
            return deviceEdition;
        }

        /**
         * 根据ssid获取设备版本
         * @param ssid
         * @return
         */
        public static DeviceEdition getEditionFromSSID(String ssid){
            if (ssid != null && ssid.matches("^QD[_A-Z].+")) {
                String tag = ssid.substring(2,3);
                for(DeviceEdition edition: values()){
                    if(edition.tag.equals(tag)){
                        return edition;
                    }
                }
            }
            return null;
        }

        public boolean isSupportWrtieCard() {
            //V2.3.0 以上版本支持写卡
            final String supportVersion = "V2.3.0";
            String[] supportSplits = supportVersion.split("\\.");
            if (!TextUtils.isEmpty(firmware_version)) {
                String[] splits = firmware_version.split("\\.");
                if (splits != null && splits.length == 3) {
                    String v = splits[0];
                    if (v.compareToIgnoreCase(supportSplits[0]) > 0) {
                        return true;
                    } else if (v.compareToIgnoreCase(supportSplits[0]) == 0) {
                        v = splits[1];
                        if (v.compareToIgnoreCase(supportSplits[1]) > 0) {
                            return true;
                        } else if (v.compareToIgnoreCase(supportSplits[1]) == 0) {
                            v = splits[2];
                            if (v.compareToIgnoreCase(supportSplits[2]) >= 0) {
                                return true;
                            }
                        }
                    }
                }
            }
            return false;
        }
    }
}
