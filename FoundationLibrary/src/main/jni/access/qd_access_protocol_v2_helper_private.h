//
// Created by 胡涛 on 16/6/11.
//

/**
 * @file qd_access_protocol_v2_helper_private.h
 * @brief 协议构建代码，内部使用
 * @author hutoa
 * @date 2016年09月27日18:24:08
 */
#ifndef QC202_QD_ACCESS_PROTOCOL_V2_HELPER_PRIVATE_H
#define QC202_QD_ACCESS_PROTOCOL_V2_HELPER_PRIVATE_H

#include "common_struct.h"
#include "qd_access_protocol_v2_define.h"

/*
 ** Make sure we can call this stuff from C++.
 */
#ifdef __cplusplus
extern "C" {
#endif

//============ 通用数据结构 ==============

/**
 * @brief 构建完整的通信消息
 * @param p_target_device_mac 目标设备的MAC地址字符串
 * @param sum_of_content 消息包含的content个数
 * @return 消息数据
 */

t_buffer build_msg_v2(const char *const p_target_device_mac, int sum_of_content, ...);
t_buffer build_msg_v4(const char *const p_target_device_mac, int sum_of_content, ...);

t_buffer build_msg_v5(const char *const p_target_device_mac, int sum_of_content, ...);

t_buffer build_content_v2(const uint8_t cmd, const uint8_t ack_type, const uint8_t *p_data, const uint32_t data_len);

//============ 相关业务数据 ==============
//开门命令
t_buffer build_open_door_data_v2(uint8_t keep_time, uint32_t app_usr_id, uint8_t phone_type,
                                 const char *const agcy);

t_buffer build_open_door_content_v2(const uint8_t keep_time, const uint32_t app_user_id,
                                    const uint8_t phone_type, const char *const agcy);

t_buffer build_open_door_data_v4(uint8_t keep_time, uint32_t app_usr_id, uint8_t phone_type,
                                 uint32_t pass_id, const char *const agcy);

t_buffer build_open_door_content_v4(const uint8_t keep_time, const uint32_t app_user_id,
                                    const uint8_t phone_type, const uint32_t pass_id,
                                    const char *const agcy);

t_buffer build_open_door_data_v5(uint8_t keep_time, uint64_t app_usr_id, uint8_t phone_type);

t_buffer build_open_door_content_v5(const uint8_t keep_time, const uint64_t app_user_id,
                                    const uint8_t phone_type);

//设备配置命令
t_buffer build_config_data_v2(const uint8_t open_time, const uint8_t *const p_related_unit_items,
                              const int sum_of_unit_items);

t_buffer build_config_data_v4(const char *const agcy);

//设备升级命令
t_buffer build_update_device_data_v2(const uint8_t update_type, const char *const p_download_addr);

//获取设备版本信息

t_buffer build_get_device_version_ack_data_v2(const char *const p_software_version,
                                              const char *const p_hardware_version);

t_buffer build_get_device_config_ack_data_v2(const uint8_t open_keep_time,
                                             const char *const p_software_version,
                                             const char *const p_hardware_version,
                                             const uint8_t *const p_related_units,
                                             const int sum_of_units);

// 构建发卡数
t_buffer build_distribute_cards_data_v2(int wait_time_pre_card, const char *const p_cards_info);

// 构建当前设备版本信息
t_buffer build_update_state_report_data_v2(const char * const p_software_version, const char * const p_hardware_version,
                                      int download_index, uint32_t p_update_version_num);

// 构建升级数据包
t_buffer build_update_transport_fragment_data_v2(int seek_index, const uint8_t *const p_data, int data_len);


#ifdef __cplusplus
} /* end of the 'extern "C"' block */
#endif

#endif //QC202_QD_ACCESS_PROTOCOL_V2_HELPER_PRIVATE_H
