package com.qdingnet.qdaccess;

public class QDPassRecordEntity {

    public String record_index; // 设备端记录的自增长ID
    public String app_user_id; // app用户ID
    public String room_id; // 开锁房间ID
    public String server_id; // 服务器serverID
    public String mac_addr; // 门禁设备MAC
    public String create_time; // 开锁时间
    public String passby_method; // 通行方式，已知有蓝牙，wifi，密码
    public String pass_result_code; // 通行结果
    public int pass_id; // 通行id（v4专用）
    //v4开门协议中暂未添加此字段，所以采用默认值：0
    public String auto_switch = "0"; // 是否开启无障碍通行模式：0-未开启，1=已开启（v4专用）
    //v4开门协议中暂未添加此字段，所以采用默认值：手动
    public String pass_mode = "100"; //100-手动, 101-自动 （v4专用）

    public QDPassRecordEntity() {
    }

    public QDPassRecordEntity(String record_index,
                              String app_user_id,
                              String server_id,
                              String room_id,
                              String mac_addr,
                              String create_time,
                              String passby_method,
                              String pass_result_code) {
        this.record_index = record_index;
        this.app_user_id = app_user_id;
        this.room_id = room_id;
        this.server_id = server_id;
        this.mac_addr = mac_addr;
        this.create_time = create_time;
        this.passby_method = passby_method;
        this.pass_result_code = pass_result_code;
    }

    @Override
    public String toString() {
        return "QDPassRecordEntity{" +
                "record_index='" + record_index + '\'' +
                ", app_user_id='" + app_user_id + '\'' +
                ", room_id='" + room_id + '\'' +
                ", server_id='" + server_id + '\'' +
                ", mac_addr='" + mac_addr + '\'' +
                ", create_time='" + create_time + '\'' +
                ", passby_method='" + passby_method + '\'' +
                ", pass_result_code=" + pass_result_code +
                '}';
    }
}
