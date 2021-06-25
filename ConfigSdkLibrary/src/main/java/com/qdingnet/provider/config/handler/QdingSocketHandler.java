package com.qdingnet.provider.config.handler;


import androidx.annotation.NonNull;

import com.qdingnet.opendoor.core.bean.DataPacket;
import com.qdingnet.opendoor.core.handler.IDataHandler;
import com.qdingnet.opendoor.core.handler.IWifiConnectHandler;
import com.qdingnet.provider.config.Logdeal;
import com.qdingnet.provider.config.bean.QDAccessResult;
import com.qdingnet.provider.config.callback.internal.ISocketHandler;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.util.Arrays;

/**
 * 千丁wifi开门时socket处理器
 * @author pipan (pipan@qding.me)
 * @date   19-2-16
 */
public class QdingSocketHandler implements IWifiConnectHandler {
    private static final String TAG = "QdingSocketHandler";
    //连接wifi的IP
    private final static String OPEN_SERVER_IPADDRESS = "10.10.10.1";
    //连接wifi的端口
    private final static int OPEN_SERVER_PORT = 30000;
    //默认写超时时间
    private final static int WRITE_SOCKET_TIMEOUT = 15000;
    //默认收超时时间
    private final static int READ_SOCKET_TIMEOUT = 15000;
    //连接socket失败后重试次数
    private final static int CONNECT_RETRY_COUNT = 3;
    //UDP数据包重发次数
    private static final int SEND_DATA_RETRY_COUNT = 5;

    //写超时时间
    private int mWriteTimeout;
    //收超时时间
    private int mReadTimeout;
    //数据处理器
    private IDataHandler mIDataHandler;
    //上层回调接口
    private ISocketHandler mSocketCallback;

    //发送|接收socket
    private DatagramSocket mSenderSocket = null;
    private DatagramSocket mReceiveSocket = null;
    //接收到数据标志
    private volatile boolean mReceiveData = false;

    public QdingSocketHandler(@NonNull IDataHandler handler, ISocketHandler callback) {
        this(WRITE_SOCKET_TIMEOUT, READ_SOCKET_TIMEOUT, handler, callback);
    }

    public QdingSocketHandler(int writeTimeout, int readTimeout, @NonNull IDataHandler handler, ISocketHandler callback) {
        this.mWriteTimeout = writeTimeout > 0 ? writeTimeout : WRITE_SOCKET_TIMEOUT;
        this.mReadTimeout = readTimeout > 0 ? readTimeout : READ_SOCKET_TIMEOUT;
        this.mIDataHandler = handler;
        this.mSocketCallback = callback;
    }

    @Override
    public void onConnected(String ssid, String ip) {
        //初始化socket
        if(initUdpSocket(ssid)){
            send(ssid);
            receive(ssid);
        }
    }

    /**
     * 初始化udp socket
     * @return
     */
    private boolean initUdpSocket(String ssid) {
        Logdeal.D(TAG, "Init UDP Socket");
        boolean init = false;
        try {
            //发送端
            mSenderSocket = new DatagramSocket();
            mSenderSocket.setSoTimeout(mWriteTimeout);
            mReceiveSocket = new DatagramSocket(null);
            //接收端
            mReceiveSocket.setReuseAddress(true);
            mReceiveSocket.setSoTimeout(mReadTimeout);
            mReceiveSocket.bind(new InetSocketAddress(OPEN_SERVER_PORT));
            init = true;
        } catch (Exception e) {
            Logdeal.E(TAG,"initUdpSocket...Exception:",e);
            if(mSocketCallback != null){
                mSocketCallback.onSocketResult(ssid, QDAccessResult.ERROR_INIT_WIFI_FAILURE);
            }
        }
        return init;
    }

    /**
     * 连接socket
     * @return
     */
    private boolean connect(){
        boolean connected = false;
        for(int i=0;;i++){
            try {
                mSenderSocket.connect(new InetSocketAddress(OPEN_SERVER_IPADDRESS, OPEN_SERVER_PORT));
                connected = mSenderSocket.isConnected();
                break;
            } catch (Exception e) {
                if(i >= CONNECT_RETRY_COUNT) break;
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e1) {}
            }
        }
        return connected;
    }

    /**
     * 发送数据
     * @param ssid
     */
    private void send(final String ssid){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    if(connect()){
                        Logdeal.D(TAG,"mSenderSocket is connected ");
                        //构建数据
                        DataPacket packet = mIDataHandler.handleStartSendData(ssid);
                        //发送数据
                        byte[] data = packet.getData();
                        //重发机制
                        int count = SEND_DATA_RETRY_COUNT;
                        while (count-- > 0 && !mReceiveData){
                            DatagramPacket datagramPacket = new DatagramPacket(data, data.length);
                            mSenderSocket.send(datagramPacket);
                            Thread.sleep(1000);
                        }
                        Logdeal.D(TAG, "mSenderSocket...write data success");
                        mIDataHandler.handleSendDataFinish(ssid, packet);
                    }else{
                        if(mSocketCallback != null){
                            mSocketCallback.onSocketResult(ssid, QDAccessResult.ERROR_CONNECT_WIFI_FAILURE);
                        }
                    }
                } catch (Exception e) {
                    Logdeal.D(TAG,"send...Exception:",e);
                    if(mSocketCallback != null){
                        mSocketCallback.onSocketResult(ssid, QDAccessResult.ERROR_WRITE_WIFI_DATA_EXCEPTION);
                    }
                    close();
                }
            }
        }).start();
    }

    /**
     * 接收数据
     * @param ssid
     */
    private void receive(final String ssid){
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    Logdeal.D(TAG, "start receive");
                    byte[] receiveBytes = new byte[1024];
                    DatagramPacket receiveData = new DatagramPacket(receiveBytes,receiveBytes.length);
                    mReceiveSocket.receive(receiveData);
                    mReceiveData = true;
                    int length = receiveData.getLength();
                    Logdeal.D(TAG, "receive data success : length:" + length);
                    mIDataHandler.handleDataChanged(ssid, Arrays.copyOfRange(receiveBytes,0,length));
                } catch (Exception e) {
                    Logdeal.D(TAG,"receive...Exception:",e);
                    if(mSocketCallback != null){
                        mSocketCallback.onSocketResult(ssid, QDAccessResult.ERROR_READ_WIFI_DATA_EXCEPTION);
                    }
                } finally {
                    close();
                }
            }
        }).start();
    }

    /**
     * 关闭
     */
    public void close(){
        if(mSenderSocket != null){
            mSenderSocket.close();
            mSenderSocket = null;
        }
        if(mReceiveSocket != null){
            mReceiveSocket.close();
            mReceiveSocket = null;
        }
    }

}
