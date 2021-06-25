package com.qdingnet.provider.config.bean;

/**
 * Created by kuangbiao on 17-5-27.
 */

public enum QDAccessResult {

    OK(0, "success", "成功"),
    //common error
    ERROR_OPEN_DOOR_FAILURE(1000, "开门失败,请重试", "请靠近设备后再试"),
    ERROR_GET_DOOR_FAILURE(1001, "房屋未授权, 请联络物业中心", "房屋未授权, 请联络物业中心"),
    ERROR_BUILD_PACKET(1002, "生成开门数据失败", "请确保千丁App为最新版,再靠近门禁开门"),
    ERROR_OPEN_DOOR_BUSY(1003, "其他用户开门中，请稍后再试", "门禁繁忙,请再开门操作一次"),
    ERROR_DISCONNECT_DEVICE_FAILURE(1004, "设备异常断开连接", "设备异常断开连接,请靠近设备后再试"),
    ERROR_NO_FIND_DEVICE_NOAUTHORIZE(1005, "当前未发现您可通行的门禁设备:有设备,但无权限", "该位置没有开门权限"),
    ERROR_TOO_FAR_DEVICE(1006, "请靠近门重试，有效开门距离为1~2米", "开门信号较弱,请靠近门禁再进行开门操作"),
    ERROR_SEND_DATA_TIMEOUT(1007, "发送开门数据超时", "请重新开启手机蓝牙,再靠近门禁开门操作"),
    ERROR_RECEIVE_DATA_TIMEOUT(1008, "接收开门数据超时", "请重新开启手机蓝牙,再靠近门禁开门操作"),
    ERROR_CLOSE_DEVICE_TIMEOUT(1009, "等待设备关闭连接超时", "请重新开启手机蓝牙,再靠近门禁开门操作"),
    ERROR_NOT_COMPITABLE_DEVICE(1010, "请下载新版千丁App后使用", "请确保千丁App为最新版本再进行开门操作"),
    ERROR_NOT_SUPPORT(1011, "不支持当前手机型号", "不支持该机型开门,可使用访客通行密码开门"),
    ERROR_BLEDEVICE_NULL(1012, "设备列表为空:未绑定房屋,获取失败", "认证失败，请联网在主页下拉刷新后再试"),
    ERROR_NO_FIND_DEVICE(1013, "当前未发现您可通行的门禁设备:周围无设备", "当前未发现您可通行的门禁设备，再靠近设备后重试"),
    ERROR_BLEDEVICE_NULL_WITH_ROOM(1014, "设备列表为空:有绑定房屋,获取失败", "认证失败，请联网在主页下拉刷新后再试"),
    ERROR_SZ_APP_USER_ID_NULL(1015, "APP_USER_ID获取失败", "认证失败，请联网在主页下拉刷新后再试"),
    ERROR_NO_LOCATION_PERMISSION(1016, "没有获取位置信息权限", "请求权限失败，请求打开应用位置信息权限"),
    ERROR_DEVICE_ACK_ERROR(1017, "设备回复开门错误", "设备开门失败，请重试"),
    ERROR_OPEN_DOOR_FREQUENTLY(1018, "开门太频繁", "开门太频繁，请稍后再试"),
    ERROR_GOPEN_LOCK_AGCY_FAILED(1019, "客户信息识别不匹配，没有权限开门", "认证失败，没有开门权限"),

    //BLE error
    ERROR_NO_BLUETOOTH_PERMISSION(1100, "打开蓝牙失败", "请重新开启手机蓝牙,再靠近门禁开门操作"),
    ERROR_INIT_BLE_FAILURE(1101, "初始化蓝牙失败,关闭蓝牙后重试", "请重新开启手机蓝牙,再靠近门禁开门操作"),
    ERROR_CONNECT_BLE_FAILURE(1102, "连接设备蓝牙失败,请重试", "请重新开启手机蓝牙,再靠近门禁开门操作"),

    ERROR_CONNECT_BLE_TIMEOUT(1103, "连接设备蓝牙超时,请重试", "请重新开启手机蓝牙,再靠近门禁开门操作"),
    ERROR_FIND_BLE_SERVICE_TIMEOUT(1104, "发现蓝牙设备服务超时", "请重新开启手机蓝牙,再靠近门禁开门操作"),
    ERROR_WRITE_BLE_DATA_EXCEPTION(1105, "写入蓝牙设备数据异常", "请重新开启手机蓝牙,再靠近门禁开门操作"),
    ERROR_WRITE_BLE_CHANNEL_CLOSED(1106, "写入蓝牙设备数据的特征管道关闭", "请重新开启手机蓝牙,再靠近门禁开门操作"),
    ERROR_READ_BLE_DATA_EXCEPTION(1107, "读取蓝牙设备数据异常", "请重新开启手机蓝牙,再靠近门禁开门操作"),
    ERROR_BLE_INIT_EXCEPTION(1108, "初始化蓝牙异常", "请重新开启手机蓝牙,再靠近门禁开门操作"),
    ERROR_BLE_OPEN_DOOR_TIMEOUT(1109, "蓝牙开门超时", "请重新开启手机蓝牙,再靠近门禁开门操作"),
    ERROR_BLE_SCAN_TIMEOUT(1110, "扫描设备超时", "请重新开启手机蓝牙,再靠近门禁操作"),
    ERROR_NOT_SUPPORT_BLE(1111, "当前手机型号不支持低功耗蓝牙", "该机型不支持低功耗蓝牙操作,请更换手机再试"),

    //wifi error
    ERROR_NO_WIFI_PERMISSION(1200, "请打开WiFi后再试", "请重新开启手机WiFi,再靠近门禁开门操作"),
    ERROR_INIT_WIFI_FAILURE(1201, "初始化WiFi失败", "请重新开启手机WiFi,再靠近门禁开门操作"),
    ERROR_CONNECT_WIFI_FAILURE(1202, "连接设备wifi失败,请重试", "请重新开启手机WiFi,再靠近门禁开门操作"),
    ERROR_CONNECT_WIFI_TIMEOUT(1203, "连接设备wifi超时,请重试", "请重新开启手机WiFi,再靠近门禁开门操作"),
    ERROR_WRITE_WIFI_DATA_EXCEPTION(1204, "写入wifi设备数据异常", "请重新开启手机WiFi,再靠近门禁开门操作"),
    ERROR_READ_WIFI_DATA_EXCEPTION(1205, "读取wifi设备数据异常", "请重新开启手机WiFi,再靠近门禁开门操作"),
    ERROR_WIFI_OPENDOOR_TIMEOUT(1206, "WiFi开门超时", "WiFi开门超时,请打开WiFi后重试"),
    ERROR_WIFI_OPENDOOR_FAILUER(1207, "WiFi开门失败", "WiFi开门失败,请打开WiFi后重试"),

    //扫描开门 error
    ERROR_QRCODE_OPENDOOR_FAILURE(1800, "扫码开门失败", "扫码开门失败，请重试"),
    ERROR_INVALID_QRCODE(1801, "无法识别二维码", "无法识别二维码，请重试"),
    ERROR_NETWORK_OPENDOOR_FAILURE(1802, "网络开门失败", "网络开门失败，请重试"),

    //写卡
    WRITE_CARD_UNSUPPORT(1301, "门禁设备版本过低,不支持写卡", "门禁设备版本过低,不支持写卡"),
    WRITE_CARD_RECEIVE_ACK_TIMEOUT(1302, "接收设备响应超时", "请确认设备是否支持读写卡"),
    WRITE_CARD_BUILD_DATA_FIAL(1303, "构建写卡数据失败", "请重试"),
    WRITE_CARD_TIMEOUT(1304, "写卡超时", "请靠近门禁设备重试"),
    WRITE_CARD_FAIL(1305, "刷卡太快，请保持刷卡时间3秒以上", "写卡失败，请重试"),
    WRITE_CARD_NO_TARGET_CARD(1306, "刷的卡和要写的卡不是一张卡", "请确保刷的是您要开卡的那张卡"),
    //读卡
    READ_CARD_RESULT_TIMEOUT(1401, "读卡超时", "请靠近门禁设备重试"),
    READ_CARD_FAIL(1402, "刷卡太快，请保持刷卡停留时间3秒以上", "读卡失败，请重试"),
    READ_CARD_BUILD_DATA_FIAL(1403, "构建读卡数据失败", "请重试"),
    //删卡
    DELETE_CARD_RESULT_TIMEOUT(1501, "删卡超时", "请靠近门禁设备重试"),
    DELETE_CARD_FAIL(1502, "刷卡太快，请保持刷卡停留时间3秒以上", "删卡失败，请重试"),
    DELETE_CARD_BUILD_DATA_FAIL(1503, "构建删卡数据失败", "请重试"),

    //门禁配置相关错误码
    ERROR_CONFIG_FAILURE(2000, "配置失败", "配置失败"),
    ERROR_READ_CONFIG_FAILURE(2001, "读取配置失败", "读取配置失败"),
    ERROR_READ_VERSION_FAILURE(2002, "读取版本失败", "读取版本失败"),
    ERROR_UPDATE_DEVICE_FAILURE(2003, "设备升级失败", "设备升级失败"),
    ;

    String errmsg;
    String user_tips;
    int errcode;

    QDAccessResult(int code, String msg, String user_tips) {
        this.errcode = code;
        this.errmsg = msg;
        this.user_tips = user_tips;
    }

    public int getErrCode() {
        return errcode;
    }

    public String getErrMsg() {
        return errmsg;
    }

    public String getUserTips() {
        return String.format(user_tips + "[%s]", errcode);
    }

    public boolean isSuccess() {
        return errcode == 0;
    }
}