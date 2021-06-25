package com.qdingnet.qdaccess.msg;

/**
 * Created by kuangbiao on 17-5-11.
 */

/**
 * uint32_t app_user_id;
 * uint32_t passby_method_create_time;    //高4bit，通行方式，已知有蓝牙，wifi，键盘, 卡; 低28bit为开锁时间
 */
public class QdPassRecordItem {

    public int app_user_id;
    public int passby_method_create_time;

    public int getMinSize() {
        return 4 + 4;
    }
}
