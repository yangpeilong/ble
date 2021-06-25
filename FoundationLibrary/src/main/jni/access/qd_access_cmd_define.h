//
// Created by 胡涛 on 16/6/20.
//

#ifndef QC202_QD_ACCESS_CMD_DEFINE_H
#define QC202_QD_ACCESS_CMD_DEFINE_H

//================= 命令字 V1.0 和 V2.0 共用 =========================

//------------------- 蓝牙UUID ----------------------------
#define QD_BTGATECTRL_ACCESS_UART_SERVICE_UUID              @"FFE0"      // 模块串口透传服务，广播可见
#define QD_BTGATECTRL_CHARACTERISTIC_UART_NOTIFY_UUID       @"FFE4"
#define QD_BTGATECTRL_CHARACTERISTIC_UART_WRITE_UUID        @"FFE9"

#define QD_BTGATECTRL_ACCESS_RSSI_REPORT_SERVICE_UUID       @"FFA0"     // 模块RSSI报告服务，广播可见
#define QD_BTGATECTRL_CHARACTERISTIC_RSSI_NOTIFY_UUID       @"FFA1"
#define QD_BTGATECTRL_CHARACTERISTIC_RSSI_WRITE_UUID        @"FFA2"

#define QD_BTGATECTRL_ACCESS_FAST_DISCONNECT_SERVICE_UUID   @"FF80"    // 模块快速断开服务
#define QD_BTGATECTRL_CHARACTERISTIC_FAST_DISCONNECT_UUID   @"FF89"

//----------------- WIFI通信端口 -------------------------
#define QD_WIFI_READER_UDP_PORT     30000

//------------ 前导码 ---------------
#define QD_MSG_FRAMEHEAD        0X24

//------------ 命令字 ---------------
#define QD_CMD_OPEN_DOOR                0X05
#define QD_CMD_CONFIG_DEVICE            0X06
#define QD_CMD_UPDATE_SYSTEM            0X07
#define QD_CMD_SYNC_TIME                0x08
#define QD_CMD_GET_VERSION              0x09
#define QD_CMD_GET_CONFIG               0x0A
#define QD_CMD_PUSH_LOCATION            0x0B
#define QD_CMD_SET_RELAY_STATE          0x0C
#define QD_CMD_PUSH_BLACK_CARD          0x0D
#define QD_CMD_ENABLE_433_TEST          0x0E
#define QD_CMD_GET_HAL_STATE            0x0F
#define QD_CMD_SET_BUZZER_MODE          0x10
#define QD_CMD_PUSH_HISTORY_LOCATION    0x11
#define QD_CMD_OPEN_DOOR_AND_RECORDS_V5 0X50
#define QD_CMD_OPEN_DOOR_V5             0X51

// 门禁发卡操作 Add by hutao 20170418
#define QD_CMD_DISTRIBUTE_CARD          0x12
#define QD_CMD_CANCEL_DISTRIBUTE_CARD   0x13
#define QD_CMD_READ_CARD_UID            0x14

//  门禁分片升级操作
#define QD_CMD_UPDATE_FILE_DESC         0x15
#define QD_CMD_UPDATE_STATE_REPORT      0x16
#define QD_CMD_UPDATE_FILE_TRANSPORT    0x17

#define QD_CMD_RECORD_CONFIG_PHONENO    0x18
#define QD_CMD_UPDATE_REQUEST           0X19 //upgrade from configure tools

//配置设备经销商CODE
#define QD_CMD_CONFIG_DEVICE_AGCY       0x2A

//开门与通行记录分离指令
//携带正常开门请求数据即可， 会收到QD_CMD_OPEN_DOOR_NO_RECORD的开门结果应答
#define QD_CMD_OPEN_DOOR_NO_RECORD      0x2B
//无需携带数据，会收到QD_CMD_GET_ACCESS_RECORD的带通行记录数据的应答
#define QD_CMD_GET_ACCESS_RECORD        0x2C
//获取门磁状态
#define QD_CMD_GET_DOORSENSOR_STATE     0x2D

//门禁四代产测指令
#define QD_CMD_GET_DEVINFO				0x40
#define QD_CMD_NOTIFY_EVENT			    0x41


//============ 帧选项 ================
//----------- 通用状态 ---------------
#define QD_REQUEST              0x61
#define QD_ACK_OK               0x62
#define QD_ACK_ERR              0x63

// --------- 通用状态 ---------------
#define QD_ACCESS_SUCCESS       0x71
#define QD_ACCESS_FAILED        0x72


//------------ 开锁异常状态 ---------------
#define OPEN_LOCK_DEV_BUSY      0x73
#define OPEN_LOCK_FREQUENT      0x74
//开门鉴权失败结果：
#define OPEN_LOCK_AGCY_ERROR    0x76

//------------ 默认继电器状态 ----------
#define RELAY_DEFAULT_CLOSE     0x01
#define RELAY_DEFAULT_OPEN      0x02

//------------ 蜂鸣器工作模式 ----------
#define BUZZER_TTL_MODE         0x01
#define BUZZER_PWM_MODE         0x02

//------------ 通行方法,4个b 最大16种 ---------
#define PASSBY_BLUETOOTH            0x00
#define PASSBY_WIFI                 0x01
#define PASSBY_KEYBOARD             0x02
#define PASSBY_QRCODE               0x03
#define PASSBY_CARD                 0x04
#define PASSBY_EXIT_BUTTON          0x05
#define PASSBY_HAL_SENSOR_WARNING   0x06
#define PASSBY_KEY_TEST             0x07
#define PASSBY_433_TEST             0x08

//----------- 手机类型 ---------------
#define OS_ANDROID              0x01
#define OS_IOS                  0x02

//----------- 升级类型 ---------------
#define UPDATE_NORMAL           0x81
#define UPDATE_SYSTEM           0x82

//----------- 卡状态 -------------
#define CARD_NEED_DISABLE       0x83
#define CARD_DISABLED           0x84
#define CARD_NEED_ACTIVATE      0x85
#define CARD_ACTIVATED          0x86
#define CARD_NEED_WRITE         0x87
#define CARD_NEED_READ          0x88


#endif //QC202_QD_ACCESS_CMD_DEFINE_H
