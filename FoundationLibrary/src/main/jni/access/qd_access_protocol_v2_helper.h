//
// Created by 胡涛 on 16/6/11.
//

#ifndef QC202_QD_ACCESS_PROTOCOL_V2_HELPER_H
#define QC202_QD_ACCESS_PROTOCOL_V2_HELPER_H
#include "common_struct.h"

/*
 ** Make sure we can call this stuff from C++.
 */
#ifdef __cplusplus
extern "C" {
#endif

//开门指令
t_buffer build_open_door_msg_v2(const char *const p_target_device_mac,
                                const uint8_t keep_time,
                                const uint32_t app_user_id,
                                const uint8_t phone_type,
                                const char *const agcy);

t_buffer build_open_door_msg_v4(const char *const p_target_device_mac,
                                const uint8_t keep_time,
                                const uint32_t app_user_id,
                                const uint8_t phone_type,
                                const uint32_t pass_id,
                                const char *const agcy);

t_buffer build_open_door_msg_with_locations_v2(const char *const p_target_device_mac,
                                               const uint8_t keep_time,
                                               const uint32_t app_user_id,
                                               const uint8_t phone_type,
                                               const char *const agcy,
                                               const uint8_t *const p_related_unit_itmes,
                                               const int sum_of_unit_items);

t_buffer build_open_door_msg_with_all_locations_v2(const char *const p_target_device_mac,
                                                   const uint8_t keep_time,
                                                   const uint32_t app_user_id,
                                                   const uint8_t phone_type,
                                                   const char *const agcy,
                                                   const uint8_t *const p_related_unit_items,
                                                   const int sum_of_unit_items,
                                                   const uint32_t *const p_history_units,
                                                   const int sum_of_history_units);

t_buffer build_open_door_msg_with_black_cards_v2(const char *const p_target_device_mac,
                                                 const uint8_t keep_time,
                                                 const uint32_t app_user_id,
                                                 const uint8_t phone_type,
                                                 const char *const agcy,
                                                 const uint8_t *const p_black_card_items,
                                                 const int sum_of_card_item);

t_buffer build_open_door_msg_with_distribute_cards_info_v2(const char *const p_target_device_mac,
                                                 const uint8_t keep_time,
                                                 const uint32_t app_user_id,
                                                 const uint8_t phone_type,
                                                 const char *const agcy,
                                                 int wait_time_pre_card,
                                                 const char *const p_cards_info);

t_buffer build_open_door_ack_v2(const char *const p_target_device_mac,
                                const uint8_t open_result,
                                void *const p_data,
                                const int data_len);

//配置指令
t_buffer build_config_device_msg_v2(const char *const p_target_device_mac,
                                    const uint8_t open_time,
                                    const uint8_t *const p_related_unit_items,
                                    const int sum_of_unit_items);

t_buffer build_config_device_msg_with_relay_state_and_phone_v2(const char *const p_target_device_mac,
                                                     const uint8_t open_time,
                                                     const uint8_t *const p_related_unit_items,
                                                     const int sum_of_unit_items,
                                                     const uint8_t relay_state,
                                                     const uint32_t phone,
                                                     const char* const agcy);

t_buffer build_config_device_msg_with_relay_state_all_locations_v2(const char *const p_target_device_mac,
                                                          const uint8_t open_time,
                                                          const uint8_t *const p_related_unit_items,
                                                          const int sum_of_unit_items,
                                                          uint8_t relay_state,
                                                          const uint32_t *const p_history_units,
                                                          const int sum_of_history_units);

t_buffer build_config_device_ack_v2(const char *const p_target_device_mac,
                                    const uint8_t config_result);

//升级指令
t_buffer build_update_device_msg_v2(const char *const p_target_device_mac,
                                    const uint8_t update_type,
                                    const char *const p_download_addr);

t_buffer build_update_device_ack_v2(const char *const p_target_device_mac,
                                    const uint8_t update_result);

//获取版本信息指令
t_buffer build_get_device_version_msg_v2(const char *const p_target_device_mac);

t_buffer build_get_device_version_ack_v2(const char *const p_target_device_mac,
                                         const char *const p_software_version,
                                         const char *const p_hardware_version);

//获取配置信息指令
t_buffer build_get_device_config_msg_v2(const char *const p_target_device_mac);

t_buffer build_get_device_config_ack_v2(const char *const p_target_device_mac,
                                        const uint8_t open_keep_time,
                                        const char *const p_software_version,
                                        const char *const p_hardware_version,
                                        const uint8_t *const p_related_units,
                                        const int sum_of_units);

//协议不支持应答
t_buffer build_protocol_error_ack_v2(const char *const p_target_device_mac);

//设置继电器默认状态指令
t_buffer build_set_relay_state_msg_v2(const char *const p_target_device_mac,
                                      const uint8_t default_state);

t_buffer build_set_relay_state_ack_v2(const char *const p_target_device_mac,
                                      const uint8_t set_state);

// 启用433测试模式
t_buffer build_protocol_enable_433_test_msg_v2(const char *const p_target_device_mac);
t_buffer build_protocol_enable_433_test_ack_v2(const char *const p_target_device_mac);

// 获取霍尔开关的状态
t_buffer build_protocol_get_hal_state_msg_v2(const char *const p_target_device_mac);
t_buffer build_protocol_get_hal_state_ack_v2(const char *const p_target_device_mac, const uint8_t state);


// 读取卡片UID 和 写卡
t_buffer build_protocol_get_card_uid_msg_v2(const char *const p_target_device_mac);
t_buffer build_protocol_get_card_uid_ack_v2(const char *const p_target_device_mac, const uint8_t state);
t_buffer build_protocol_report_card_uid_ack_v2(const char *const p_target_device_mac, const uint8_t state, const uint8_t *const p_data, uint32_t data_len);

t_buffer build_protocol_write_card_data_msg_v2(const char *const p_target_device_mac, int wait_time_pre_card, const char *const p_write_data);
t_buffer build_protocol_cancel_write_card_data_msg_v2(const char *const p_target_device_mac);


t_buffer build_protocol_write_card_data_ack_v2(const char *const p_target_device_mac, const uint8_t state);
t_buffer build_protocol_report_write_card_result_msg_v2 (const char *const p_target_device_mac, const uint8_t state, const uint8_t *const p_data, uint32_t data_len);

//构建升级描述信息
t_buffer build_protocol_update_file_desc_msg_v2(const char *const p_target_device_mac, int version_num, int file_len, const char md5[33]);
//构建升级描述信息
t_buffer build_protocol_update_file_desc_msg_v3(const char *const p_target_device_mac, int version_num, int file_len, const uint32_t const checksum_crc32);

// 发送升级包数据
t_buffer build_protocol_transport_update_file_msg_v2(const char *const p_target_device_mac, int seek_index, const uint8_t * const p_data, int data_len);
//构建请求设备当前版本以及当前接收到的升级文件位置信息
t_buffer build_update_state_request_msg_v3(const char *const p_target_device_mac);


//---------- 分割线 ---------
// 快速开门使用
t_buffer build_phone_advertise_msg_with_floors_v2(const char *p_target_dev_mac, uint8_t version,
                                                  uint32_t app_user_id,
                                                  const uint8_t *const p_floors,
                                                  const uint8_t sum_of_floors,
                                                  const char *const agcy_code);

t_buffer
build_phone_advertise_msg_v4(const char *p_target_dev_mac, uint8_t version, uint32_t app_user_id,
                             uint32_t pass_id);

//配置指令
t_buffer build_config_device_msg_v4(const char *const p_target_device_mac, const char *const agcy);

t_buffer build_protocol_get_device_info_msg_v4(const char *const p_target_device_mac);

// 获取门磁的状态
t_buffer build_protocol_get_gate_magnetism_state_msg_v5(const char *const p_target_device_mac);

t_buffer build_protocol_get_gate_magnetism_state_ack_v5(const char *const p_target_device_mac,
                                                        const uint8_t state);

//---------------------------------- v5 --------------------------------
t_buffer build_open_door_msg_v5(const char *const p_target_device_mac,
                                const uint8_t keep_time,
                                const uint64_t app_user_id,
                                const uint8_t phone_type);

t_buffer build_phone_advertise_msg_with_floors_v5(const char *p_target_dev_mac,
                                                  uint8_t version,
                                                  uint64_t app_user_id,
                                                  const uint8_t *const p_floors,
                                                  const uint8_t sum_of_floors);


#ifdef __cplusplus
} /* end of the 'extern "C"' block */
#endif

#endif //QC202_QD_ACCESS_PROTOCOL_V2_HELPER_H
