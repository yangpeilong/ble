/*
 * qd_ble_hook_msg_define.h
 *
 *  Created on: Feb 22, 2016
 *      Author: hutao
 */

#ifndef SRC_BUSINESS_QD_BLE_READER_QD_BLE_HOOK_MSG_DEFINE_H_
#define SRC_BUSINESS_QD_BLE_READER_QD_BLE_HOOK_MSG_DEFINE_H_

#include <stdint.h>

#define QD_BLE_HOOK_PROTOCOL_VER    'A'
#define QD_BLE_HOOK_FRAMEHEAD        0x23

// 模块发给设备的数据类型，单向 Module To Device
#define MTD_ADVERTISE                0x10  //模块转发广播数据
#define MTD_FIRST_LINK_REAL_MAC      0x11  //模块由空闲状态接受收到第一个连接请求
#define MTD_SECOND_LINK_REAL_MAC     0x12  //模块在连接状态下，收到第二个连接连接请求，此次的MAC为真实MAC，模块会重启
#define MTD_FIRST_LINK_FAKE_MAC      0x14  //模块由空闲状态接受收到第一个连接请求
#define MTD_SECOND_LINK_FAKE_MAC     0x15  //模块在连接状态下，收到第二个连接连接请求，此次的MAC为随机MAC，模块会重启
#define MTD_TRANSPORT_ACK            0xC0  //数据已转发，可以发第二包数据
#define MTD_DEBUG                    0x20  //模块debug

// 设备发给模块的数据类型，单向 Device To Module
#define DTM_ADVERTISE_ACK            0x13  //广播数据处理应答
#define DTM_SET_MAC                  0x02  //设置模块MAC
#define DTM_DISCONNECT                 0x81

// 双向命令
#define MDDM_DATA                    0x80  //透传数据命令，每次透传的数据最大为20个字节
#define MDDM_HEARTBEAT               0x00  //双向心跳，互为看门狗
#define MDDM_FIRST_CMD               0x01  //模块或是设备启动完成后的第一个命令

//广播消息头,用于蓝牙模块和设备之间
typedef struct t_ble_hook_msg_header {
    char frame_head;
    uint8_t msg_len;
    uint8_t msg_type;
    uint8_t data_start[0];
}__attribute__((packed())) t_ble_hook_msg_header, *p_t_ble_hook_msg_header;

//门禁设备端广播出来的数据
typedef struct t_dev_adv_msg {
    char qd_prefix[2];
    char version;
    char base64_mac[8];
    char ack_id_start[0];
}__attribute__((packed())) t_dev_adv_msg;

// ack_id Item单元
typedef struct t_dev_ack_id {
    uint8_t open_reslut;
    uint8_t two_byte_app_user_id[2];
}__attribute__((packed())) t_dev_ack_id;

// 手机端广播的门禁数据
typedef struct t_phone_adv_msg {
    uint8_t version;       // 版本号,需为可见字符
    uint16_t key;          // 消息解密key
    uint32_t app_user_id;  // 经服务器端编号的用户ID
    uint32_t time;         // 手机的当前时间
    uint8_t reserved[7];   // 预留
}__attribute__((packed())) t_phone_adv_msg;

// 手机端广播的门禁数据
typedef struct t_phone_adv_msg_v4 {
    uint8_t version;       // 版本号,需为可见字符
    uint16_t key;          // 消息解密key
    uint32_t app_user_id;  // 经服务器端编号的用户ID
    uint32_t time;         // 手机的当前时间
    uint32_t pass_id;      // 通行ID
    //uint8_t reserved[3];   // 预留
}__attribute__((packed())) t_phone_adv_msg_v4;

// 手机端广播的门禁数据
typedef struct t_phone_adv_msg_v5 {
    uint8_t version;       // 版本号,需为可见字符
    uint16_t key;          // 消息解密key
    uint64_t app_user_id;  // 经服务器端编号的用户ID
    uint32_t time;         // 手机的当前时间
    uint8_t floors[3];         // 楼层
}__attribute__((packed())) t_phone_adv_msg_v5;

/*
 * --- Mon Feb 15 16:49:04 CST 2016

 1, 基本命令结构,  '#' + Len + Type + Data
 (命令完整帧长度不可超出32Byte)

 Len,1B,包含其自身的长度
 Type,1B,命令类型,
 Data,Type所含数据

 2, 目前定义的Type,

 0x10, 广播开门数据,由蓝牙模块发给CPU,
 '#' + 26 + 0x10 + App广播数据(24B)

 0x11, 第一次连接MAC,蓝牙模块发给CPU,
 0x12, 第二次连接MAC,蓝牙模块发给CPU,
 '#' + 8 + 0x11(12) + MAC(6B)

 0x13, 开门ID,由CPU发给蓝牙模块,注意这里ID是用ASCII格式
 '#' + 6 + 0x13 + ID(4B)

 0x14, 第二次连接,MAC是随机数,蓝牙模块发给CPU,(模块将重启)
 '#' + 8 + 0x14 + MAC随机数(6B)

 0x80, 连接数据,双向,蓝牙模块只管加命令头后透传
 '#' + Len + 0x80 + 透传数据(Max 20Byte)

 0xC0, 连接数据ACK,蓝牙模块发给CPU,
 '#' + 2 + 0xC0

 0x81, 连接断开, CPU发给蓝牙
 '#' + 2 + 0x81


 一些系统命令,Type,

 0x00, ECHO,心跳,双向
 '#' + 2 + 0x00

 0x01, First,第一条命令,双向
 '#' + 2 + 0x01

 0x02, 模块MAC,CPU发给蓝牙模块,
 '#' + 8 + 0x02 + MAC(6B)

 0x20, DBG, 调试, 发给CPU
 '#' + (2+N) + 0x20 + 调试字符(Nbyte)


 --- Wed Feb 17 16:41:05 CST 2016

 1, 蓝牙门禁广播格式,

 整个长度固定为26Byte,"确认ID"用于回应用户的开门请求,最多3个,为0值
 时无效,协议版本,("00"),

 "QD" + "A"(版本号) + MAC(8B,base64) + 0 , 0 , ... (13个0x00)
 或者是,
 确认ID1(4B) + ... + 确认ID3(4B) + 0x00

 2, 手机App广播格式

 固定长度为24Byte,协议版本号1B,解密密匙 2B, 其它共15B,进行Base64编码，18B 转换为 24B

 Version(1B) + Key(2B) + AppID(4B) + Time(4B) + Reserved(7B)



 History:

 +++ Tue Feb 16 09:16:37 CST 2016

 1, 添加Type, 0xC0, 连接数据ACK,蓝牙模块发给CPU,用于模块的缓冲管理


 +++ Wed Feb 17 16:46:01 CST 2016

 1, 添加蓝牙门禁广播格式

 2, 添加Type, 0x14, 第二次连接,MAC是随机数,(模块将重启)


 +++ Thu Feb 18 11:09:17 CST 2016

 1, 改蓝牙门禁广播为固定长度格式,26Byte,确认ID改为用4Byte

 2, 添加手机App广播格式,同时变更Type 0x10,广播开门数据格式
 */

#endif /* SRC_BUSINESS_QD_BLE_READER_QD_BLE_HOOK_MSG_DEFINE_H_ */
