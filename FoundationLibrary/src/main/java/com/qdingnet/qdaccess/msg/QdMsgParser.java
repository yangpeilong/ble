package com.qdingnet.qdaccess.msg;

import com.qdingnet.opendoor.log.Logdeal;
import com.qdingnet.qdaccess.QDAccessMsgHandler;

import java.util.Arrays;

/**
 *
 *未使用
 * Created by kuangbiao on 17-5-11.
 */

public final class QdMsgParser {

    private static final String TAG = "QdMsgParser";
    private static final byte QD_CMD_OPEN_DOOR = 0x05;
    private static final byte QD_CMD_DISTRIBUTE_CARD = 0x12;
    private static final byte QD_MSG_FRAMEHEAD = 0x24;

    private static final byte QD_ACCESS_SUCCESS = 0x71;
    private static final byte OPEN_LOCK_FREQUENT = 0x74;

    private static QDBuffer mBuffer = new QDBuffer();

    /**
     * // --------- 通用状态 ---------------
     * #define QD_ACCESS_SUCCESS       0x71
     * #define QD_ACCESS_FAILED        0x72
     * <p>
     * <p>
     * //------------ 开锁异常状态 ---------------
     * #define OPEN_LOCK_DEV_BUSY      0x73
     * #define OPEN_LOCK_FREQUENT      0x74
     *
     * @param packet
     * @param callback
     * @return
     */


    public static void pushData(byte[] packet, QDAccessMsgHandler.IQDAccessMsgCallback callback) {
        if (mBuffer.isEmpty()) {
            parse(packet, callback);
        } else if (mBuffer.isReceiveFinish()) {
            parse(packet, callback);
        } else {
            mBuffer.addData(packet);
        }
    }

    public static boolean parse(byte[] packet, QDAccessMsgHandler.IQDAccessMsgCallback callback) {
        Logdeal.D(TAG, "packet == " + Arrays.toString(packet));
        if (packet.length < (QdAccessMsg.getMinSize() - 1 + QdAccessContent.getMinSize())) {
            return false;
        }
        int i = 0;
        QdAccessMsg accessMsg = new QdAccessMsg();
        accessMsg.frame_head = packet[i++];

        //1.int a=(byte[0]<<8)|(byte[1]&0xff)
        accessMsg.decrypt_key = (short) ((packet[i++] << 8) | (packet[i++] & 0xff));
        accessMsg.protocol_version = packet[i++];
        byte h = packet[i++], l = packet[i++];
        accessMsg.total_length = (short) ((((l & 0x080) + (l & 0x7f)) << 8) + (h & 0x80) + (h & 0x7f));

        //(short) ((packet[i++] << 8) | (packet[i++] & 0xff));
        if (mBuffer.isEmpty()) {
            mBuffer.init(accessMsg.total_length);
            mBuffer.addData(packet);
        }
        Logdeal.D(TAG, " accessMsg == " + accessMsg.toString());
        if (accessMsg.frame_head == QD_MSG_FRAMEHEAD) {
            for (; i < packet.length; ) {
                QdAccessContent content = new QdAccessContent();
                int start = i;
                h = packet[start++];
                l = packet[start++];
                content.content_length = (short) ((((l & 0x080) + (l & 0x7f)) << 8) + (h & 0x80) + (h & 0x7f));
                //(short) ((packet[start++] << 8) | (packet[start++] & 0xff));
                content.command_type = packet[start++];
                content.ack_type = packet[start++];
                if (content.content_length == 0) {
                    break;
                }
                switch (content.command_type) {
                    case QD_CMD_OPEN_DOOR:
                        parseOpenDoorAck(content, callback);
                        break;
                    case QD_CMD_DISTRIBUTE_CARD:
                        break;
                }

                Logdeal.D(TAG, " QdAccessContent == " + content.toString());
                i += i + content.content_length;
//                t_qd_access_content * p_qd_content = (t_qd_access_content *) ((char*)
//                p_data + processed_buf_index);
//                LOGD("processed_buf_index =%u ,p_qd_content->content_length = %u , data_len =%u\n",
//                        processed_buf_index, p_qd_content -> content_length, data_len);
//
//                if (p_qd_content -> content_length == 0) {
//                    break;
//                }
//                //保证content的数据完整性
//                if (processed_buf_index + p_qd_content -> content_length <= (uint16_t) data_len) {
//                    process_msg_content_v2((void*)p_qd_content);
//                    processed_buf_index += p_qd_content -> content_length;
//                } else {
//                    break;
//                }
            }
        }


        return true;
    }

    private static void parseOpenDoorAck(QdAccessContent content, QDAccessMsgHandler.IQDAccessMsgCallback callback) {
        callback.onOpenDoorAck(content.ack_type);
    }
}
