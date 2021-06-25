/*
 * qd_access_protocol_helper.h
 *
 *  Created on: Feb 15, 2016
 *      Author: hutao
 */

#ifndef SRC_BUSINESS_QD_ACCESS_PROTOCOL_QD_ACCESS_PROTOCOL_HELPER_H_
#define SRC_BUSINESS_QD_ACCESS_PROTOCOL_QD_ACCESS_PROTOCOL_HELPER_H_

#include "common_struct.h"

/*
 ** Make sure we can call this stuff from C++.
 */
#ifdef __cplusplus
extern "C" {
#endif

uint16_t Crc16_DATAs(uint8_t *ptr, uint16_t len);

/**
 *  加密内容
 *
 *  @param p_content      内容的起始指针
 *  @param content_len    内容的长度
 *  @param p_out_holder   加密输出位置起始指针
 *  @param holder_max_len 加密输出位置能够提供的最大数据容量
 *  @param p_dec_key      与本次加密配套输出的解密key。
 *
 *  @return  负数表示加密失败，正数代表加密后的数据长度。
 */
int encrypt_content(const char *const p_content, int content_len, char *const p_out_holder, int holder_max_len,
                    uint32_t *p_dec_key);

/**
 *  解密内容
 *
 *  @param p_content      内容的起始指针
 *  @param content_len    内容的长度
 *  @param p_out_holder   解密输出位置起始指针
 *  @param holder_max_len 解密输出位置能够提供的最大数据容量
 *  @param dec_key        解密key
 *
 *  @return 负数表示解密失败，正数代表解密后的数据长度
 */
int decrypt_content(const char *const p_content, int content_len, char *const p_out_holder, int holder_max_len,
                    uint32_t dec_key);

t_buffer build_open_door_msg(uint32_t reported_record_index, uint32_t cached_record_index,
                             const char *const p_mac_list, int keep_time, const char *p_app_user_id,
                             const char *p_room_id,
                             const char *p_server_id, uint8_t phone_type);
t_buffer build_open_door_msg_with_locations(uint32_t reported_record_index, uint32_t cached_record_index,
                                            const char *const p_mac_list, int keep_time, const char *p_app_user_id,
                                            const char *p_room_id, const char *p_server_id, uint8_t phone_type,
                                            const char *p_location);
t_buffer build_open_door_ack(uint8_t open_result, void *const p_data, int data_len);

t_buffer build_config_device_msg(const char *const p_related_doors, int open_time, int max_doors_size,
                                 int max_counter_size);
t_buffer build_config_device_ack(uint8_t config_result);

t_buffer build_update_device_msg(uint8_t update_type, const char *const p_download_addr);
t_buffer build_update_device_ack(uint8_t update_result);

t_buffer build_get_device_version_msg(const char *const p_target_mac_addr);
t_buffer build_get_device_version_ack(const char *const p_software_version, const char *const p_hardware_version);

t_buffer build_get_device_config_msg(const char *const target_mac_addr);
t_buffer build_get_device_config_ack(uint32_t open_keep_time, const char *const p_related_doors, int max_doors_size,
                                     int max_counter_size, const char *const p_software_version,
                                     const char *const p_hardware_version);

t_buffer build_protocol_error_ack(uint16_t current_protocol_version);
t_buffer build_protocol_set_relay_state_msg(uint8_t default_state);


#ifdef __cplusplus
} /* end of the 'extern "C"' block */
#endif

#endif /* SRC_BUSINESS_QD_ACCESS_PROTOCOL_QD_ACCESS_PROTOCOL_HELPER_H_ */
