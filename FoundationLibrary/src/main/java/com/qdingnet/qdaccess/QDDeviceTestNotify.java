package com.qdingnet.qdaccess;

/**
 * 设备主动通知app相关测试信息
 * @author pipan (pipan@qding.me)
 * @date   18-9-29
 */
public class QDDeviceTestNotify {
    //开关按钮测试
    public static final int BUTTON_TEST = 3;
    //霍尔开关测试
    public static final int HALL_TEST = 4;
    //NFC开关测试
    public static final int NFC_TEST = 5;

    //测试类型
    public int type;
    //相关数据
    public String msg;

    public QDDeviceTestNotify(int type, String msg) {
        this.type = type;
        this.msg = msg;
    }
}
