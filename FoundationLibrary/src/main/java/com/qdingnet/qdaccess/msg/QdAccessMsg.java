package com.qdingnet.qdaccess.msg;

/**
 * Created by kuangbiao on 17-5-11.
 */

import java.util.Arrays;

/**
 * uint8_t frame_head;         ///< 前导码
 * uint16_t decrypt_key;       ///< 解密密匙
 * uint8_t protocol_version;   ///< 协议版本
 * uint16_t total_length;      ///< 消息总长度
 * uint8_t content_start[0];   ///< 0长度数组,数据项的起始位置
 */
public class QdAccessMsg {
    public byte frame_head;
    public short decrypt_key;
    public byte protocol_version;
    public short total_length;
    public byte[] content;


    public static int getMinSize() {
        return 1 + 2 + 1 + 2;
    }

    @Override
    public String toString() {
        return "QdAccessMsg{" +
                "frame_head=" + Integer.toHexString(frame_head) +
                ", decrypt_key=" + decrypt_key +
                ", protocol_version=" + protocol_version +
                ", total_length=" + total_length +
                ", content=" + Arrays.toString(content) +
                '}';
    }
}
