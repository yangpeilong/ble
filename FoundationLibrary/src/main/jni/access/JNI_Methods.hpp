/*
 * NativeMethods.h
 *
 *  Created on: Apr 1, 2014
 *      Author: china
 */

#ifndef JNI_METHODS_H_
#define JNI_METHODS_H_
/*注意不要进行代码格式化，会搞乱下面的方法格式*/
#include <jni.h>

//V1
jbyteArray JNI_build_open_door_msg(JNIEnv * env, jobject obj, jint reported_record_index, jint cacahed_record_index,
		jstring mac_list, jint keep_seconds, jstring app_user_id, jstring room_id, jstring server_id);

jbyteArray JNI_build_open_door_msg_with_locations(JNIEnv * env, jobject obj, jint reported_record_index,
		jint cacahed_record_index, jstring mac_list, jint keep_seconds, jstring app_user_id, jstring room_id,
		jstring server_id, jstring location);

jint JNI_analyse_opendoor_ack_packet(JNIEnv * env, jobject obj, jbyteArray packet, jobjectArray record_array);

jbyteArray JNI_build_config_device_msg(JNIEnv * env, jobject obj, jint keep_time, jstring device_list,
		jint max_doors_size, jint max_counter_size);
jint JNI_analyse_config_ack_packet(JNIEnv * env, jobject obj, jbyteArray packet);

jint JNI_genarate_key(JNIEnv * env, jobject obj, jstring commnity, jstring bulid_uinit, jint max_doors_size,
		jint max_counter_size);

jbyteArray JNI_build_update_device_msg(JNIEnv * env, jobject obj, jstring pakage_download_path);
jint JNI_analyse_update_ack_packet(JNIEnv * env, jobject obj, jbyteArray packet);

jbyteArray JNI_build_get_device_version_msg(JNIEnv * env, jobject obj, jstring target_mac);
jobjectArray JNI_analyse_get_device_version_ack_packet(JNIEnv * env, jobject obj, jbyteArray packet);

jbyteArray JNI_build_get_device_config_msg(JNIEnv * env, jobject obj, jstring target_mac);
jint JNI_analyse_get_device_config_ack_packet(JNIEnv * env, jobject obj, jbyteArray packet, jobject config_entity);

jint JNI_PJWHash(JNIEnv * env, jobject obj, jbyteArray packet);
jint JNI_Do_Calc_CheckSum(JNIEnv * env, jobject obj, jbyteArray packet);

//V2
jbyteArray JNI_build_get_device_version_msg_v2(JNIEnv * env, jobject obj, jstring target_mac);
jobjectArray JNI_analyse_get_device_version_ack_packet_v2(JNIEnv * env, jobject obj, jstring target_mac, jbyteArray packet);

jbyteArray JNI_build_get_device_config_msg_v2(JNIEnv * env, jobject obj, jstring target_mac);
jint JNI_analyse_get_device_config_ack_packet_v2(JNIEnv * env, jobject obj, jstring target_mac, jbyteArray packet, jobject config_entity);

jbyteArray JNI_build_update_device_msg_v2(JNIEnv * env, jobject obj, jstring target_mac, jstring package_download_path);
jint JNI_analyse_update_ack_packet_v2(JNIEnv * env, jobject obj, jstring target_mac, jbyteArray packet);

jbyteArray JNI_build_open_door_msg_v2(JNIEnv * env, jobject obj, jstring target_mac, jbyte keep_time, jlong app_user_id, jstring auth_code);
jbyteArray JNI_build_open_door_msg_with_distribute_cards_info_v2(JNIEnv * env, jobject obj, jstring target_mac, jbyte keep_time, jlong app_user_id, jstring auth_code, jint wait_time_pre_card,jstring cards_info);
jbyteArray JNI_build_open_door_msg_with_locations_v2(JNIEnv * env, jobject obj, jstring target_mac, jbyte keep_time, jlong app_user_id, jstring auth_code, jobjectArray project_group_build_units);
jbyteArray JNI_build_open_door_msg_with_cards_v2(JNIEnv *env, jobject obj, jstring target_mac, jbyte keep_time, jlong app_user_id, jstring auth_code, jobjectArray cards);
jint JNI_analyse_opendoor_ack_packet_v2(JNIEnv * env, jobject obj, jstring target_mac, jbyteArray packet, jobjectArray record_array);

jbyteArray JNI_build_config_device_msg_v2(JNIEnv * env, jobject obj, jstring target_mac,jbyte keep_time, jobjectArray project_group_build_units, jbyte relay_state, jlong phone, jstring auth_code);
jint JNI_analyse_config_ack_packet_v2(JNIEnv * env, jobject obj, jstring target_mac, jbyteArray packet);

jint JNI_genarate_key_v2(JNIEnv * env, jobject obj, jlong unit_id);

jstring JNI_build_phone_advertise_msg_v2(JNIEnv * env, jobject obj, jstring target_mac, jlong app_user_id, jbyteArray floors, jstring auth_code);
jbyteArray JNI_build_protocol_get_card_uid_msg_v2(JNIEnv * env,jobject obj,jstring target_mac);
jbyteArray JNI_build_protocol_write_card_data_msg_v2(JNIEnv * env,jobject obj,jstring target_mac,jint wait_time_pre_card, jstring write_data);
jbyteArray JNI_build_protocol_cancel_write_card_data_msg_v2(JNIEnv * env,jobject obj,jstring target_mac);
jbyteArray  JNI_build_protocol_update_file_desc_msg_v2(JNIEnv *env,jobject obj,jstring target_device_mac,jint version_num,jint file_len,jstring md5);
jbyteArray  JNI_build_protocol_update_file_desc_msg_v3(JNIEnv *env,jobject obj,jstring target_device_mac,jint version_num,jint file_len,jint file_checksum_crc32);
jbyteArray  JNI_build_protocol_transport_update_file_msg_v2(JNIEnv *env,jobject obj,jstring target_mac,jint seek_index,jbyteArray data,jint data_len);
jbyteArray  JNI_build_update_state_request_msg_v3(JNIEnv *env,jobject obj,jstring target_mac);

jbyteArray JNI_build_protocol_standalone_read_cardno_msg_v2(JNIEnv *env,jobject obj,jstring target_mac, jint timeout);
jbyteArray JNI_build_protocol_standalone_add_cardno_msg_v2(JNIEnv *env,jobject obj,jstring target_mac, jlong cardNo, jlong validity, jbyteArray floors);
jbyteArray JNI_build_protocol_standalone_delete_cardno_msg_v2(JNIEnv *env,jobject obj,jstring target_mac, jlongArray cardNos);

//V4
jbyteArray JNI_build_open_door_msg_v4(JNIEnv * env, jobject obj, jstring target_mac, jbyte keep_time, jlong app_user_id, jint pass_id, jstring auth_code);
jstring JNI_build_phone_advertise_msg_v4(JNIEnv * env, jobject obj, jstring target_mac, jlong app_user_id, jint pass_id);
jbyteArray JNI_build_config_device_msg_v4(JNIEnv * env, jobject obj, jstring target_mac, jstring auth_code);

static JNINativeMethod methods[] =
		{
		  { "PJWHash", "([B)I" , (void*)JNI_PJWHash},
		  { "Do_Calc_CheckSum", "([B)I" , (void*)JNI_Do_Calc_CheckSum},
		  //V1
		  { "build_open_door_msg", "(IILjava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)[B",(void*) JNI_build_open_door_msg },
		  { "build_open_door_msg_with_locations","(IILjava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)[B",(void*) JNI_build_open_door_msg_with_locations },
		  { "analyse_opendoor_ack_packet","([BLjava/util/ArrayList;)I", (void*) JNI_analyse_opendoor_ack_packet },
		  { "build_config_device_msg", "(ILjava/lang/String;II)[B", (void*) JNI_build_config_device_msg },
		  { "analyse_config_device_ack_packet", "([B)I", (void*) JNI_analyse_config_ack_packet },
		  { "genarate_key", "(Ljava/lang/String;Ljava/lang/String;II)I", (void*) JNI_genarate_key },
		  { "build_update_device_msg", "(Ljava/lang/String;)[B", (void*) JNI_build_update_device_msg },
		  { "analyse_update_device_ack_packet", "([B)I", (void*) JNI_analyse_update_ack_packet },
		  { "build_get_device_version_msg", "(Ljava/lang/String;)[B",(void*) JNI_build_get_device_version_msg },
		  { "analyse_get_device_version_ack_packet", "([B)[Ljava/lang/String;", (void*) JNI_analyse_get_device_version_ack_packet },
		  { "build_get_device_config_msg", "(Ljava/lang/String;)[B", (void*) JNI_build_get_device_config_msg },
		  { "analyse_get_device_config_ack_packet", "([BLcom/qdingnet/qdaccess/QDDeviceConfigEntity;)I",(void*) JNI_analyse_get_device_config_ack_packet },
          //V2
		  { "build_open_door_msg_v2", "(Ljava/lang/String;BJLjava/lang/String;)[B",(void*) JNI_build_open_door_msg_v2 },
		  { "build_open_door_msg_with_distribute_cards_info_v2", "(Ljava/lang/String;BJLjava/lang/String;ILjava/lang/String;)[B",(void*) JNI_build_open_door_msg_with_distribute_cards_info_v2},
		  { "build_open_door_msg_with_locations_v2","(Ljava/lang/String;BJLjava/lang/String;[Lcom/qdingnet/qdaccess/QDUnitItem;)[B",(void*) JNI_build_open_door_msg_with_locations_v2 },
		  { "build_open_door_msg_with_cards_v2","(Ljava/lang/String;BJLjava/lang/String;[Lcom/qdingnet/qdaccess/QDRFCardItem;)[B",(void*)JNI_build_open_door_msg_with_cards_v2},
		  { "analyse_opendoor_ack_packet_v2","(Ljava/lang/String;[BLjava/util/ArrayList;)I", (void*) JNI_analyse_opendoor_ack_packet_v2 },
		  { "build_config_device_msg_v2", "(Ljava/lang/String;B[Lcom/qdingnet/qdaccess/QDUnitItem;BJLjava/lang/String;)[B", (void*) JNI_build_config_device_msg_v2 },
		  { "analyse_config_device_ack_packet_v2", "(Ljava/lang/String;[B)I", (void*) JNI_analyse_config_ack_packet_v2 },
		  { "build_get_device_version_msg_v2", "(Ljava/lang/String;)[B", (void*) JNI_build_get_device_version_msg_v2 },
		  { "analyse_get_device_version_ack_packet_v2", "(Ljava/lang/String;[B)[Ljava/lang/String;", (void*) JNI_analyse_get_device_version_ack_packet_v2 },
		  { "build_get_device_config_msg_v2", "(Ljava/lang/String;)[B", (void*) JNI_build_get_device_config_msg_v2 },
		  { "analyse_get_device_config_ack_packet_v2", "(Ljava/lang/String;[BLcom/qdingnet/qdaccess/QDDeviceConfigEntity;)I",(void*) JNI_analyse_get_device_config_ack_packet_v2 },
		  { "build_update_device_msg_v2", "(Ljava/lang/String;Ljava/lang/String;)[B", (void*) JNI_build_update_device_msg_v2 },
		  { "analyse_update_device_ack_packet_v2", "(Ljava/lang/String;[B)I", (void*) JNI_analyse_update_ack_packet_v2 },
		  { "generate_key_v2", "(J)I", (void*) JNI_genarate_key_v2 },
		  { "build_phone_advertise_msg_v2", "(Ljava/lang/String;J[BLjava/lang/String;)Ljava/lang/String;", (void*)JNI_build_phone_advertise_msg_v2},
		  { "build_protocol_get_card_uid_msg_v2", "(Ljava/lang/String;)[B", (void*)JNI_build_protocol_get_card_uid_msg_v2},
		  { "build_protocol_write_card_data_msg_v2", "(Ljava/lang/String;ILjava/lang/String;)[B", (void*)JNI_build_protocol_write_card_data_msg_v2},
		  { "build_protocol_cancel_write_card_data_msg_v2", "(Ljava/lang/String;)[B", (void*)JNI_build_protocol_cancel_write_card_data_msg_v2},
		  { "build_protocol_update_file_desc_msg_v2", "(Ljava/lang/String;IILjava/lang/String;)[B", (void*)JNI_build_protocol_update_file_desc_msg_v2},
		  { "build_protocol_update_file_desc_msg_v3", "(Ljava/lang/String;III)[B", (void*)JNI_build_protocol_update_file_desc_msg_v3},
		  { "build_protocol_transport_update_file_msg_v2", "(Ljava/lang/String;I[BI)[B", (void*)JNI_build_protocol_transport_update_file_msg_v2},
		  { "build_update_state_request_msg_v3", "(Ljava/lang/String;)[B", (void*)JNI_build_update_state_request_msg_v3},
		  { "build_protocol_standalone_read_cardno_msg_v2", "(Ljava/lang/String;I)[B", (void*)JNI_build_protocol_standalone_read_cardno_msg_v2},
		  { "build_protocol_standalone_add_cardno_msg_v2", "(Ljava/lang/String;JJ[B)[B", (void*)JNI_build_protocol_standalone_add_cardno_msg_v2},
		  { "build_protocol_standalone_delete_cardno_msg_v2", "(Ljava/lang/String;[J)[B", (void*)JNI_build_protocol_standalone_delete_cardno_msg_v2},

		  //v4
		  { "build_phone_advertise_msg_v4", "(Ljava/lang/String;JI)Ljava/lang/String;", (void*)JNI_build_phone_advertise_msg_v4},
		  { "build_open_door_msg_v4", "(Ljava/lang/String;BJILjava/lang/String;)[B",(void*) JNI_build_open_door_msg_v4 },
		  { "build_config_device_msg_v4", "(Ljava/lang/String;Ljava/lang/String;)[B",(void*) JNI_build_config_device_msg_v4},
		};
#endif /* JNI_METHODS_H_ */
