package com.qdingnet.qdaccess.msg;

/**
 * Created by kuangbiao on 17-5-10.
 */

public class QDBuffer {
    private int currentReceiveLength = 0;
    private int size = 0;
    private byte[] buffer = null;


    public void init(int size) {
        this.size = size;
        buffer = new byte[size];

    }

    public void reset() {
        buffer = null;
        currentReceiveLength = 0;
        size = 0;
    }

    public boolean isEmpty() {
        return currentReceiveLength == 0;
    }

    public boolean isReceiveFinish() {
        return currentReceiveLength == size;
    }

    public synchronized int getCurrentReceiveLength() {
        return currentReceiveLength;
    }

    public synchronized void setCurrentReceiveLength(int currentReceiveLength) {
        this.currentReceiveLength = currentReceiveLength;
    }

    public synchronized int getSize() {
        return size;
    }

    public synchronized byte[] getBuffer() {
        return buffer;
    }

    public void addData(byte[] packet) {
        int size = packet.length;
        size = (getCurrentReceiveLength() + size) > getSize() ? getSize() - getCurrentReceiveLength() : size;
        if (size > 0) {
            System.arraycopy(packet, 0, getBuffer(), getCurrentReceiveLength(), size);
            setCurrentReceiveLength(getCurrentReceiveLength() + size);
        }
    }


}
