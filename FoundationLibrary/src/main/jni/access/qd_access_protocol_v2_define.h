//
// Created by 胡涛 on 16/6/7.
//


#ifndef QC202_QD_ACCESS_PROTOCOL_DEFINE_V2_H
#define QC202_QD_ACCESS_PROTOCOL_DEFINE_V2_H
#include <stdint.h>

#include "qd_access_cmd_define.h"


// ===========  当前协议版本 =================
#define QD_BLE_GATE_CTRL_VERSION_V2 'A'
#define QD_BLE_GATE_CTRL_VERSION_V4 'C'
#define QD_BLE_GATE_CTRL_VERSION_V5 'D'

/* >>>>>>>>>> 千丁蓝牙门禁控制协议说明 <<<<<<<<<<
 * 最后修订人：胡涛       最后修订时间：2016-06-11
 * 当前版本: V2.0.0
 *
 *
 * 修订记录:
 * ============ V2.0.0 初始版本 ==============
 * 1.规定蓝牙模块服务UUID，读写的特征值UUID,具体数值见本文件宏定义
 * 2.规定蓝牙模块与app之间的通信数据链路层
 *   本协议采用类似LTV（Length-Tag-Value）通用变长结构来定义一个数据帧.一次通信的数据帧长度最长
 * 为64K(受限于设备的buffer)。
 *   数据帧由前导码，解密密匙，协议版本，消息长度和消息数据组成，如下图所示。消息数据由多个数据项组成。
 * 数据帧的发送顺序依次为前导码，校验码，数据长度和消息数据。对于每个由多个字节构成的数据项应该先发
 * 送最低有效字节；对每个字节应该发送最高有效位。
 *
 *    ============ 帧头 ==============
 *    |         前导码 (1Byte)      |
 *    ------------------------------
 *    |         解密密匙(2Byte)      |
 *    ------------------------------
 *    |         协议版本(1Byte)     |
 *    ------------------------------
 *    |         消息长度(2Byte)     |
 *    ============ 数据项1 =============
 *    |         数据长度(2Byte)     |
 *    ------------------------------
 *    |         命令字 (1Byte)      |
 *    ------------------------------
 *    |         帧选项 (1Byte)      |
 *    ------------------------------
 *    |                           |
 *    |          附加数据(N Byte)    |
 *    |                           |
 *    ------------------------------
 *    =========== 数据项2 =============
 *                 ~
 *                 ~
 *                 ~
 *                 ~
 *    ========== 数据项N ===============
 *
 *  具体的结构抽象见文件的消息结构体定义
 *
 *  前导码:    标示一个数据帧的开始，1个字节，暂定为0x24。
 *  解密密匙：  老宋编写的加密算法,可起到校验数据的合法性和完整性
 *  协议版本：  用来标示当前通信的版本，设计此字段的目的，是为了未来协议更新，依然可以与旧设备进行通信。
 *  消息长度：  整条消息的长度，包括前导码在内的所有有内容字节数。
 *  数据长度：  一个数据项的长度。
 *  命令字：    当前数据项要执行的操作。
 *  帧选项：    标示当前数据项是否需要接收端应答，或是指示当前数据项的应答结果。
 *  附加数据：  当前数据项的附加数据。
 *
 *  如果一次要传递多个数据项的话，直接跟在上一个数据项后。注意一次消息数据总长度不能超过65535个Byte。
 */

/**
 * @brief 固定消息头
 *
 * 通信帧－－消息头 合计 6B
 *
 */

typedef struct t_qd_access_msg {
    uint8_t frame_head;         ///< 前导码
    uint16_t decrypt_key;       ///< 解密密匙
    uint8_t protocol_version;   ///< 协议版本
    uint16_t total_length;      ///< 消息总长度
    uint8_t content_start[0];   ///< 0长度数组,数据项的起始位置

}__attribute__((packed())) t_qd_access_msg, *p_t_qd_access_msg;

/* 通信帧－－消息体 4B */
typedef struct t_qd_access_content {
    uint16_t content_length; ///< 数据项长度
    uint8_t command_type;    ///< 命令字
    uint8_t ack_type;        ///< 帧选项，用来指示此帧是否需要应答，或指示该命令应答结果
    uint8_t data_start[0];   ///< 数据项内容开始，即第一个字节

}__attribute__((packed())) t_qd_access_content, *p_t_qd_access_content;

/* 开门命令数据 最小10B */
typedef struct t_qd_opendoor_data_v2 {
    uint32_t create_time;     //手机当前时间，以秒为单位
    uint8_t keep_time;        //锁开保持时间，以500毫秒为单位，255 ＊ 500 = 127.5秒
    uint8_t CA_flag;          // 针对本地UUID列表操作的一些标示，及最大密码个数等
    uint32_t app_user_id;     //后台对现有北京过来的AppUserID进行重新编号，4B 支持约 42亿用户
    uint8_t location_uuid[0]; //预留
}__attribute__((packed())) t_qd_opendoor_data_v2, *p_t_qd_opendoor_data_v2;

typedef struct t_qd_opendoor_data_v4 {
    uint32_t create_time;     //手机当前时间，以秒为单位
    uint8_t keep_time;        //锁开保持时间，以500毫秒为单位，255 ＊ 500 = 127.5秒
    uint8_t CA_flag;          // 针对本地UUID列表操作的一些标示，及最大密码个数等
    uint32_t app_user_id;     //后台对现有北京过来的AppUserID进行重新编号，4B 支持约 42亿用户
    uint32_t pass_id;
    uint8_t location_uuid[0]; //预留
}__attribute__((packed())) t_qd_opendoor_data_v4, *p_t_qd_opendoor_data_v4;

/* 开门命令数据 最小14B */
typedef struct t_qd_opendoor_data_v5 {
    uint32_t create_time;     //手机当前时间，以秒为单位
    uint8_t keep_time;        //锁开保持时间，以500毫秒为单位，255 ＊ 500 = 127.5秒
    uint8_t CA_flag;          // 针对本地UUID列表操作的一些标示，及最大密码个数等
    uint64_t app_user_id;     //后台对现有北京过来的AppUserID进行重新编号，4B 支持约 42亿用户
    uint8_t location_uuid[0]; //预留
}__attribute__((packed())) t_qd_opendoor_data_v5, *p_t_qd_opendoor_data_v5;


/* 发卡数据 */
typedef struct t_qd_write_card_data_v2 {
    int wait_mseconds_pre_card;
    uint8_t card_item[0];
}__attribute__((packed())) t_qd_write_card_data_v2, *p_t_qd_write_card_data_v2;

/* 卡item，55个B */
typedef struct t_qd_write_card_item_v2 {
    uint8_t target_card_uid[7];
    uint8_t write_data[48];
}__attribute__((packed())) t_qd_write_card_item_v2, *p_t_qd_write_card_item_v2;

/* 通行记录 item */
typedef struct t_qd_passby_record_item_v2 {
    uint32_t app_user_id;
    uint32_t passby_method_create_time;    //高4bit，通行方式，已知有蓝牙，wifi，键盘, 卡; 低28bit为开锁时间
}__attribute__((packed())) t_qd_passby_record_item_v2, *p_t_qd_passby_record_item_v2;

/* 通行记录 item */
typedef struct t_qd_passby_record_item_v5 {
    uint8_t passby_method_result; //高5bit：通行方式，低3bit:通行结果
    uint64_t app_user_id;
    uint32_t create_time;    //记录创建时间
}__attribute__((packed())) t_qd_passby_record_item_v5, *p_t_qd_passby_record_item_v5;

/* 消息数据－－配置设备 命令数据项 */
typedef struct t_qd_config_device_data_v2 {
    uint8_t keep_time;                                 //锁开保持时间，以500毫秒为单位，255 ＊ 500 = 127.5秒
    uint32_t setting_time;                             //设置时间
    uint8_t related_unit_items[0];                     //关联单元Item起始
}__attribute__((packed())) t_qd_config_device_data_v2, *p_t_qd_config_device_data_v2;

/* 单元信息 item */
typedef struct t_qd_unit_item {
    uint32_t unit_num;             //服务器端使用 “项目＋组团＋楼栋＋单元” 进行的hash编号
    uint16_t multi_time_counter;   //多次有效的密码总数
    uint16_t single_time_counter;  //单次有效的密码总数
}__attribute__((packed())) t_qd_unit_item, *p_t_qd_unit_item;

/* 消息数据－－更新设备 命令数据项 */
typedef struct t_qd_update_device_data_v2 {
    uint8_t update_type;                           //升级类型
    uint8_t encrypted_download_addr_start[0];      //更新包下载地址
}__attribute__((packed())) t_qd_update_device_data_v2, *p_t_qd_update_device_data_v2;

typedef struct t_qd_version_ack_data_v2 {
    uint8_t software_version[10];
    uint8_t hardware_version[20];
}__attribute__((packed())) t_qd_version_ack_data_v2, *p_t_qd_version_ack_data_v2;

typedef struct t_qd_config_ack_data_v2 {
    uint8_t open_keep_time;           // 开锁保持时间
    uint32_t device_time;             //设备当前时间
    uint8_t software_version[10];     //设备软件版本号
    uint8_t hardware_version[20];     //设备硬件版本号
    uint8_t related_doors_start[0];   //相关设备C 字符串起始字节
}__attribute__((packed())) t_qd_config_ack_data_v2, *p_t_qd_config_ack_data_v2;

typedef struct t_qd_card_item {
    uint32_t card_uid;  //卡号,由7byte SN进行hash处理得到的 4B编号
    uint32_t time_stamp; // 操作的时间戳
    uint8_t operate_type; // 操作类型, 包括 注销/激活 两种操作
}__attribute__((packed())) t_qd_card_item, *p_t_qd_card_item;


// 升级文件描述，用于设备端于App端
typedef struct t_qd_update_file_desc_data_v2 {
    uint32_t file_len; // 文件长度
    uint8_t file_md5[33]; //文件md5校验,C字符串
    uint32_t update_file_version_num;   // 当前升级文件的
    uint32_t mini_compatible_software_version;
}__attribute__((packed())) t_qd_update_file_desc_data_v2,*p_t_qd_update_file_desc_data_v2 ;

// 升级文件描述，用于设备端于App端
typedef struct t_qd_update_file_desc_data_v3 {
    uint32_t file_len; // 文件长度
    uint32_t file_checksum_crc32; // file check sum with crc32
    uint32_t update_file_version_num; // 当前升级文件的
    uint32_t mini_compatible_software_version;
}__attribute__((packed())) t_qd_update_file_desc_data_v3,*p_t_qd_update_file_desc_data_v3 ;

// 文件当前更新状态
typedef struct t_qd_update_file_state_data_v2 {
    uint8_t software_version_name[10];     //设备软件版本号
    uint32_t software_version_num;
    uint8_t hardware_version_name[20];     //设备硬件版本号
    uint32_t current_update_file_pos;      //文件的最后更新位置
    uint32_t  current_update_version_num;  //当前升级的文件版本
}__attribute__((packed()))t_qd_update_file_state_data_v2,p_t_qd_update_file_state_data_v2 ;

// 升级文件传输块
typedef struct t_qd_update_file_transport_data_v2 {
    int32_t file_pos_index; //此包的数据在文件的位置
    uint32_t crc32_check;  //数据的CRC32校验
    uint8_t file_data[0];  //文件数据的起始位置，每次最多传输900个字节，耗时约0.64秒
}__attribute__((packed())) t_qd_update_file_transport_data_v2, p_t_qd_update_file_transport_data_v2 ;

//配置设备/设备开门携带的经销商CODE
typedef struct t_qd_agcy_data {
    unsigned char code[2];
}__attribute__((packed)) t_qd_agcy_data, *p_t_qd_agcy_data;

/****************************************四代测试使用的数据结构***********************************************************/
//设备相关信息
typedef struct t_qd_devinfo_ack_data_v4 {
    uint8_t sw_ver[16];     //设备软件版本号
    uint8_t hw_ver[16];     //设备硬件版本号
    uint8_t serial_no[16];			  //设备序列号
    uint8_t imei[16];				  //设备IMEI号
    uint8_t mac[16];				  //设备MAC
    uint8_t network_status;			 //网络状态
}__attribute__((packed)) t_qd_devinfo_ack_data_v4, *p_t_qd_devinfo_ack_data_v4;

//设备主动通知测试结果信息（开关测试，霍尔开关测试，nfc测试）
typedef struct t_qd_notify_data_v4 {
    uint8_t type;
    uint64_t data;
}__attribute__((packed)) t_qd_notify_data_v4, *p_t_qd_notify_data_v4;

#endif //QC202_QD_ACCESS_PROTOCOL_DEFINE_V2_H
