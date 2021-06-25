/*
 * qd_access_protocol_helper.h
 *
 *  Created on: Feb 2, 2016
 *      Author: hutao
 */

#ifndef SRC_BUSINESS_QD_ACCESS_PROTOCOL_QD_ACCESS_PROTOCOL_HELPER_PRIVATE_H_
#define SRC_BUSINESS_QD_ACCESS_PROTOCOL_QD_ACCESS_PROTOCOL_HELPER_PRIVATE_H_

#include "common_struct.h"

/*
 ** Make sure we can call this stuff from C++.
 */
#ifdef __cplusplus
extern "C" {
#endif

//============ 通用数据结构 ==============
t_buffer build_msg(int sum_of_content, ...);

t_buffer build_content(uint8_t cmd, uint8_t ack_type, uint8_t *p_data, uint32_t data_len);

//============ 相关业务数据 ==============
//开门命令
t_buffer build_open_door_data(uint32_t reported_record_index, uint32_t cached_record_index,
                              const char *const p_mac_list, int keep_time, const char *p_app_user_id,
                              const char *p_room_id,
                              const char *p_server_id, uint8_t phone_type);

t_buffer build_open_door_ack_data(uint8_t open_result, void *const p_data, int data_len);

//设备配置命令
t_buffer build_config_data(const char *const p_related_doors, int open_time, int max_doors_size, int max_counter_size);

//设备升级命令
t_buffer build_update_device_data(uint8_t update_type, const char *const p_download_addr);

//获取设备版本信息
t_buffer build_get_device_version_data(const char *const p_target_mac_addr);

t_buffer build_get_device_version_ack_data(const char *const p_software_version, const char *const p_hardware_version);

//获取设备的配置信息
t_buffer build_get_device_config_data(const char *const p_target_mac_addr);

t_buffer build_get_device_config_ack_data(uint32_t open_keep_time, const char *const p_related_doors,
                                          int max_doors_size, int max_counter_size,
                                          const char *const p_software_version,
                                          const char *const p_hardware_version);

#ifdef __cplusplus
} /* end of the 'extern "C"' block */
#endif

#endif /* SRC_BUSINESS_QD_ACCESS_PROTOCOL_QD_ACCESS_PROTOCOL_HELPER_PRIVATE_H_ */
