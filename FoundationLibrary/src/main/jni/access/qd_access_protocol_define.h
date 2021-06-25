/*
 * qd_access_protocol_define.h
 *
 *  Created on: Feb 2, 2016
 *      Author: hutao
 */

#ifndef SRC_BUSINESS_QD_ACCESS_PROTOCOL_QD_ACCESS_PROTOCOL_DEFINE_H_
#define SRC_BUSINESS_QD_ACCESS_PROTOCOL_QD_ACCESS_PROTOCOL_DEFINE_H_

/* >>>>>>>>>> 千丁蓝牙门禁控制协议说明 <<<<<<<<<<
 * 最后修订人：胡涛       最后修订时间：2016-03-18
 * 当前版本: V0.0.2
 * 修订记录:
 * 1.增加App用户位置解析命令
 *
 *
 * 最后修订人：胡涛       最后修订时间：2015-09-01
 * 当前版本: V0.0.1
 * 修订记录:
 * ============ V0.0.1 初始版本 ==============
 * 1.规定蓝牙模块服务UUID，读写的特征值UUID,具体数值见本文件宏定义
 * 2.规定蓝牙模块与app之间的通信数据链路层
 *   本协议采用类似LTV（Length-Tag-Value）通用变长结构来定义一个数据帧.一次通信的数据帧长度最长
 * 为1024个字节。
 *   数据帧由前导码，校验码，协议版本，消息长度和消息数据组成，如下图所示。消息数据由多个数据项组成。
 * 数据帧的发送顺序依次为前导码，校验码，数据长度和消息数据。对于每个由多个字节构成的数据项应该先发
 * 送最低有效字节；对每个字节应该发送最高有效位。
 *
 *    ------------------------------
 *    |         前导码 (1Byte)      |
 *    ------------------------------
 *    |         校验码 (2Byte)      |
 *    ------------------------------
 *    |         协议版本(2Byte)     |
 *    ------------------------------
 *    |         消息长度(2Byte)     |
 *    ------------------------------
 *    |         数据长度(2Byte)     |
 *    ------------------------------
 *    |         命令字 (1Byte)      |
 *    ------------------------------
 *    |         帧选项 (1Byte)      |
 *    ------------------------------
 *    |                            |
 *    |          附加数据           |
 *    |                            |
 *    ------------------------------
 *    具体的结构抽象见文件的消息结构体定义
 *  前导码：标示一个数据帧的开始，1个字节，暂定为0x24。
 *  校验码：crc16校验，校验内容为协议版本开始的所有数据，检验函数目前未定，调试阶段不进行数据校验。
 *  协议版本：用来标示当前通信的版本，设计此字段的目的，是为了未来协议更新，依然可以与旧设备进行通信。
 *  消息长度：整条消息的长度，包括所有内容在内的字节数。
 *  数据长度：一个数据项的长度。
 *  命令字：  当前数据项要执行的操作。
 *  帧选项：  标示当前数据项是否需要接收端应答，或是指示当前数据项为应答。
 *  附加数据： 当前数据项的附近数据。
 *
 *  如果一次要传递多个数据项的话，直接跟在上一个数据项后。注意一次消息数据总长度不能超过1024Byte。
 */

#include <stdint.h>

#include "qd_access_cmd_define.h"

//------------------- 协议版本 ----------------------------
#define QD_BLE_GATE_CTRL_VERSION 0100


/* 通信帧－－消息头 */
typedef struct t_qd_gatectrl_msg {
    uint8_t frame_head;
    uint16_t crc16_check;
    uint16_t protocol_version;
    uint16_t total_length;
    uint8_t content_start;

}__attribute__((packed())) t_qd_gatectrl_msg, *p_t_qd_gatectrl_msg;

/* 通信帧－－消息体 */
typedef struct t_qd_gatectrl_contentbody {
    uint16_t content_length; //数据项长度
    uint8_t command_type;    //命令字
    uint8_t ack_type;        //帧选项，用来指示此帧是否需要应答，或指示为应答帧
    uint8_t data_start;      //数据项内容开始，即第一个字节

}__attribute__((packed())) t_qd_gatectrl_contentbody, *p_t_qd_gatectrl_contentbody;

/* 消息数据－－开锁 命令数据项 */
typedef struct t_qd_opendoor_data {
    uint32_t reported_record_index;    //当前app用户已上报的开锁记录index，此index由终端设备生成，并保证唯一
    uint32_t cached_record_index;       //当前app用户已经缓存的开锁记录index,设备端将从此index之后取数据发给app
    uint8_t app_user_id[33];           // app用户ID, 由北京服务器提供, char * 字符串
    uint8_t room_id[33];               // 当前用户使用的房间编号，自增长，北京服务器生成，保证全局唯一
    uint8_t server_id[33];             //服务器serverID，由深圳服务器提供
    uint32_t create_time;             // 消息创建时间，蓝牙门禁模块以此时间作为开锁时间
    uint16_t keep_time;               // 开锁保持时间，单位ms为单位
    uint32_t decrypt_key;             //解密密匙
    uint8_t phone_type;             //手机类型，1 Android 2 iPhone
    uint8_t encrypted_mac_list_start; //加密的MAC地址列表第一字节开始，收到后使用解密密匙加密。
//每个MAC地址为12字节的ASCII字符，至少会有一个MAC地址

}__attribute__((packed())) t_qd_opendoor_data, *p_t_qd_opendoor_data;

//typedef struct t_qd_opendoor_data_v2 {
//	uint32_t create_time;  //手机当前时间，以秒为单位
//	uint8_t keep_time; //锁开保持时间，以500毫秒为单位，255 ＊ 500 = 127.5秒
//	uint8_t CA_flag;	     // 针对本地UUID列表操作的一些标示，及最大密码个数等
//	uint32_t app_user_id;    //后台对现有北京过来的AppUserID进行重新编号，4B 支持约 42亿用户
//	uint8_t location_uuid[0]; //预留
//};

/* 消息数据 -- 开锁 应答数据项*/
typedef struct t_qd_opendoor_ack_data {
    uint8_t open_result;             // 开锁成功为1，开锁失败为 2
    uint8_t record_item_start;      // 开锁记录的起始

}__attribute__((packed())) t_qd_opendoor_ack_data, *p_t_qd_opendoor_ack_data;

/* 消息数据 -- 开锁 应答数据项 record_item
 * 该数据项可以使多个Item，每个Item的长度固定,当蓝牙门禁收到开锁命令后解析
 * 如果app端服务器可达且自身还有数据需要传输出去，则附加到数据项中。
 * App端收到后需根据content长度，对消息按Item结构解析
 *
 * 这个结构只是一个Item 的定义
 */

typedef struct t_qd_passby_record_item {
    uint32_t record_index; //设备端记录的自增长ID
    uint8_t app_user_id[33]; // app用户ID, char * 字符串
    uint8_t room_id[33];     // 当前用户使用的房间编号，c 字符串
    uint8_t server_id[33];      //服务器serverID
    uint8_t mac_addr[13];    //门禁设备MAC, char * 字符串
    uint32_t create_time;    //开锁时间
    uint8_t passby_method;   //通过方式，已知有蓝牙，wifi，键盘

}__attribute__((packed())) t_qd_passby_record_item, *p_t_qd_passby_record_item;
//
//typedef struct t_qd_passby_record_item_v2 {
//	uint32_t app_user_id;
//	uint32_t passby_method_create_time;    //高1B，通行方式，已知有蓝牙，wifi，键盘, 卡; 低3B为开锁时间
//};

/*
 * UUID 定义
 * 省-- 市 -- 项目 -- 组团 -- 楼栋 -- 单元
 *
 * 设备端：项目 --      组团    --     楼栋 --单元  控制在4B以内
 *        1023(10b)  127(7b)         4095(12b)  = 29bit
 * 重新编排 ID
 */

/* 消息数据－－配置设备 命令数据项 */
typedef struct t_qd_config_device_data {
    uint16_t keep_time;                            // 开锁保持时间，单位ms为单位
    uint32_t decrypt_key;                            //解密密匙
    uint32_t setting_time;                            //设置时间
    uint32_t max_doors_size;                        //最大单元门数量
    uint32_t max_counter_size;                        //单个单元最大有效访客数量计数
    uint8_t encrypted_related_devices_list_start;    //关联设备列表
}__attribute__((packed())) t_qd_config_device_data, *p_t_qd_config_device_data;

/* 消息数据 -- 配置设备 应答数据项*/
typedef struct t_qd_config_device_ack_data {
    uint8_t config_result;             // 开锁成功为1，开锁失败为 2
}__attribute__((packed())) t_qd_config_device_ack_data, *p_t_qd_config_device_ack_data;

/* 消息数据－－更新设备 命令数据项 */
typedef struct t_qd_update_device_data {
    uint8_t update_type;                            //升级类型
    uint32_t decrypt_key;                            //解密密匙
    uint8_t encrypted_download_addr_start;            //更新包下载地址
}__attribute__((packed())) t_qd_update_device_data, *p_t_qd_update_device_data;

/* 消息数据 -- 更新设备 应答数据项*/
typedef struct t_qd_update_device_ack_data {
    uint8_t update_result;             // 收到下载命令成功 为1，收到下载命令失败为 2
}__attribute__((packed())) t_qd_update_device_ack_data, *p_t_qd_update_device_ack_data;

/* 消息数据－－获取版本信息 命令数据项 */
typedef struct t_qd_get_device_version_data {
    uint32_t decrypt_key;                            //解密密匙
    uint8_t encrypted_mac_addr_start;                //加密的目标MAC地址
}__attribute__((packed())) t_qd_get_device_version_data, *p_t_qd_get_device_version_data;

/* 消息数据－－获取版本信息 应答数据项 */
typedef struct t_qd_get_device_version_ack_data {
    uint8_t software_version[30];
    uint8_t hardware_version[50];
}__attribute__((packed())) t_qd_get_device_version_ack_data, *p_t_qd_get_device_version_ack_data;

/* 消息数据－－获取配置信息 命令数据项 */
typedef struct t_qd_get_device_config_data {
    uint32_t decrypt_key;                            //解密密匙
    uint8_t encrypted_mac_addr_start;                //加密的目标MAC地址
}__attribute__((packed())) t_qd_get_device_config_data, *p_t_qd_get_device_config_data;

/* 消息数据－－获取配置信息 应答数据项 */
typedef struct t_qd_get_device_config_ack_data {
    uint32_t open_keep_time;         // 开锁保持时间
    uint32_t max_doors_size;         // 最大单元门数量
    uint32_t max_counter_size;        //  单个单元最大有效访客数量计数
    uint32_t device_time;           //
    uint8_t software_version[30];
    uint8_t hardware_version[50];
    uint8_t related_doors_start;   //相关设备C 字符串起始字节
}__attribute__((packed())) t_qd_get_device_config_ack_data, *p_t_qd_get_device_config_ack_data;

/* 消息数据－－协议错误，应答数据项 */
typedef struct t_qd_protocol_error_ack_data {
    uint16_t current_version;
}__attribute__((packed())) t_qd_protocol_error_ack_data, *p_t_qd_protocol_error_ack_data;

/* 消息数据 -- 用户位置信息，命令数据 */
typedef struct t_qd_protocol_location_data {
    uint8_t location_start; //用户所属位置起始，c字符串，多个位置已@分割
}__attribute__((packed())) t_qd_protocol_location_data, *p_t_qd_protocol_location_data;

typedef struct t_qd_protocol_set_relay_data {
    uint8_t default_state;  //继电器默认状态
}__attribute__((packed())) t_qd_protocol_set_relay_data, *p_t_qd_protocol_set_relay_data;

/*
 *   校验函数 （借鉴秀派的校验函数）

 本协议采用CRC-16校验方式，校验结果为两个字节，CRC-16根据多项式X^16 + X^12 + X^5 + 1计算，计算的初始值为0000


 const unsigned short crc_ta[16] = { 0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129,0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef };

 uint16_t CShowPieMsgHelper::Crc16_DATAs(uint8_t *ptr, uint16_t len) {

 if (ptr == NULL) {
 return 0;
 }
 unsigned char da;
 unsigned short CRC_16_Data = 0;
 while (len-- != 0) {
 da = CRC_16_Data >> 12;
 CRC_16_Data <<= 4;
 CRC_16_Data ^= crc_ta[da ^ (*ptr / 16)];
 da = CRC_16_Data >> 12;
 CRC_16_Data <<= 4;
 CRC_16_Data ^= crc_ta[da ^ (*ptr & 0x0f)];
 ptr++;
 }
 return CRC_16_Data;
 }
 */

#endif /* SRC_BUSINESS_QD_ACCESS_PROTOCOL_QD_ACCESS_PROTOCOL_DEFINE_H_ */
