package com.qdingnet.qdaccess.msg;

/**
 * Created by kuangbiao on 17-5-11.
 */


import java.util.Arrays;

/**
 * uint16_t content_length; ///< 数据项长度
 * uint8_t command_type;    ///< 命令字
 * uint8_t ack_type;        ///< 帧选项，用来指示此帧是否需要应答，或指示该命令应答结果
 * uint8_t data_start[0];   ///< 数据项内容开始，即第一个字节
 */
public class QdAccessContent {

    public short content_length;
    public byte command_type;
    public byte ack_type;
    public byte[] data_start;


    public static int getMinSize() {
        return 2 + 1 + 1;
    }

    @Override
    public String toString() {
        return "QdAccessContent{" +
                "content_length=" + content_length +
                ", command_type=" + Integer.toHexString(command_type) +
                ", ack_type=" + Integer.toHexString(ack_type) +
                ", data_start=" + Arrays.toString(data_start) +
                '}';
    }
}
