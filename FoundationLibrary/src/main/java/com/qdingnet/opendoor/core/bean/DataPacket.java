package com.qdingnet.opendoor.core.bean;

import java.util.Arrays;
import java.util.UUID;

/**
 * 数据封装包
 * @author pipan (pipan@qding.me)
 * @date   19-1-28
 */
public class DataPacket {
    //蓝牙每帧数据大小
    private final static int BYTE_OF_FRAME = 20;

    //蓝牙发送数据的服务UUID
    private UUID mServiceUUID;
    //蓝牙发送数据的特征UUID
    private UUID mCharacteristicUUID;

    //命令字
    private int cmd;
    //当前帧
    private int currentFrame;
    //帧长度
    private int frameLen;
    //帧数，即总共有多少帧
    private int frameSize;
    //源数据
    private byte[] data;
    //发送一帧数据间隔时长(单位：秒）
    private int frameDelay;

    public DataPacket(int cmd, byte[] data) {
        this(cmd, data,0);
    }
    public DataPacket(int cmd, byte[] data, int frameDelay) {
        this(cmd, data, BYTE_OF_FRAME, frameDelay);
    }

    public DataPacket(int cmd, byte[] data, int frameLen, int frameDelay) {
        this.cmd = cmd;
        this.data = data;
        this.frameDelay = frameDelay;
        this.frameLen = frameLen > 0 ? frameLen : BYTE_OF_FRAME;
        frameSize = data.length / frameLen + 1;
        currentFrame = 0;
    }


    public int getCmd() {
        return cmd;
    }

    public byte[] getData() {
        return data;
    }

    public byte[] getNextFrame() {
        int start = currentFrame * frameLen;
        int end = (start + frameLen) > data.length ? data.length : (start + frameLen);
        if (currentFrame < frameSize && start < data.length) {
            byte[] buffer = Arrays.copyOfRange(data, start, end);
            currentFrame++;
            return buffer;
        } else {
            return null;
        }
    }

    public UUID getServiceUUID() {
        return mServiceUUID;
    }

    public void setServiceUUID(UUID mServiceUUID) {
        this.mServiceUUID = mServiceUUID;
    }

    public UUID getCharacteristicUUID() {
        return mCharacteristicUUID;
    }

    public void setCharacteristicUUID(UUID mCharacteristicUUID) {
        this.mCharacteristicUUID = mCharacteristicUUID;
    }

    public int getFrameDelay() {
        return frameDelay;
    }
}
