package com.qdingnet.opendoor.client;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;


import com.qdingnet.opendoor.core.bluetooth.BluetoothLeClass;
import com.qdingnet.opendoor.core.callback.ICloseable;
import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.opendoor.core.handler.IAdvertiseHandler;
import com.qdingnet.opendoor.core.handler.IAuthHandler;
import com.qdingnet.opendoor.core.handler.IClient;
import com.qdingnet.opendoor.core.handler.IDataHandler;
import com.qdingnet.opendoor.core.handler.IGattServiceAuthHandler;
import com.qdingnet.opendoor.core.interceptor.Interceptor;
import com.qdingnet.opendoor.core.interceptor.RealInterceptorChain;
import com.qdingnet.opendoor.core.interceptor.bluetooth.BluetoothAdvertiseInterceptor;
import com.qdingnet.opendoor.core.interceptor.bluetooth.BluetoothConnectInterceptor;
import com.qdingnet.opendoor.core.interceptor.bluetooth.BluetoothReceiveDataInterceptor;
import com.qdingnet.opendoor.core.interceptor.bluetooth.BluetoothScanInterceptor;
import com.qdingnet.opendoor.core.interceptor.bluetooth.BluetoothSendDataInterceptor;
import com.qdingnet.opendoor.core.interceptor.bluetooth.BluetoothServiceInterceptor;
import com.qdingnet.opendoor.core.interceptor.bluetooth.RetryAndFollowUpInterceptor;
import com.qdingnet.opendoor.core.request.Request;
import com.qdingnet.opendoor.log.Logdeal;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * 蓝牙操作工具类v2版
 * 执行流程:
 *
 *       扫描
 *        |
 *       广播（可选项）
 *        |
 *       连接
 *        |
 *     发现服务
 *        |
 *     发送数据
 *        |
 *     接收数据
 *        |
 *     断开连接
 *
 * @author pipan (pipan@qding.me)
 * @date   19-1-15
 */

public class BluetoothClient implements IClient {
    private static final String TAG = "BluetoothClient";

    //BLE操作客户端
    private BluetoothLeClass mBleClient;
    //重试/取消操作拦截器
    private RealInterceptorChain mRealInterceptorChain;
    //蓝牙操作启动标志
    private AtomicBoolean mEnable = new AtomicBoolean(false);

    //写数据超时时间
    private int mReadTimeout;
    //读数据超时时间
    private int mWriteTimeout;
    //扫描过滤服务
    private UUID[] mFilterUUIDs;
    //设备验证授权处理器
    private IAuthHandler mIAuthHandler;
    //广播数据处理器
    private IAdvertiseHandler mIAdvertiseHandler;
    //门禁数据处理器
    private IDataHandler mIDataHandler;
    //蓝牙服务验证处理器
    private IGattServiceAuthHandler mIGattServiceAuthHandler;
    //蓝牙状态回调接口
    private IStateCallback mIStateCallback;
    //上层触发设备客户端关闭接口
    private ICloseable.Factory mICloseableFactory;

    public BluetoothClient(Builder builder){
        this(builder.mWriteTimeout, builder.mReadTimeout, builder.mFilterUUIDs, builder.mIAuthHandler, builder.mIAdvertiseHandler, builder.mIGattServiceAuthHandler, builder.mIDataHandler, builder.mIStateCallback, builder.mICloseableFactory);
    }
    BluetoothClient(){}
    BluetoothClient(int writeTimeout, int readTimeout, UUID[] uuids, IAuthHandler authHandler, IAdvertiseHandler advertiseHandler, IGattServiceAuthHandler gattServiceAuthHandler, IDataHandler dataHandler, IStateCallback stateCallback, ICloseable.Factory factory) {
        this();
        this.mWriteTimeout = writeTimeout;
        this.mReadTimeout = readTimeout;
        this.mFilterUUIDs = uuids;
        this.mIAuthHandler = authHandler;
        this.mIAdvertiseHandler = advertiseHandler;
        this.mIGattServiceAuthHandler = gattServiceAuthHandler;
        this.mIDataHandler = dataHandler;
        this.mIStateCallback = stateCallback;
        this.mICloseableFactory = factory;
    }

    /**
     * 创建蓝牙拦截器处理链
     * @param bluetoothLeClass
     * @param request
     * @return
     */
    private RealInterceptorChain createInterceptorChain(BluetoothLeClass bluetoothLeClass, Request request){
        //拦截器
        List<Interceptor> mInterceptors = new ArrayList<>();
        //重试/取消操作拦截器
        mInterceptors.add(new RetryAndFollowUpInterceptor(bluetoothLeClass));
        //扫描
        mInterceptors.add(new BluetoothScanInterceptor(bluetoothLeClass, mFilterUUIDs, 200, mIDataHandler != null, mIAuthHandler, mIAdvertiseHandler));
        //广播收发数据
        if(mIAdvertiseHandler != null){
            mInterceptors.add(new BluetoothAdvertiseInterceptor(bluetoothLeClass, mIAdvertiseHandler));
        }
        //连接收发数据
        if(mIDataHandler != null){
            //连接
            mInterceptors.add(new BluetoothConnectInterceptor(bluetoothLeClass));
            //发现服务
            mInterceptors.add(new BluetoothServiceInterceptor(bluetoothLeClass, mIGattServiceAuthHandler));
            //发送数据
            mInterceptors.add(new BluetoothSendDataInterceptor(bluetoothLeClass, mIDataHandler, mWriteTimeout));
            //接收数据
            mInterceptors.add(new BluetoothReceiveDataInterceptor(bluetoothLeClass, mIDataHandler, mReadTimeout));
        }
        return new RealInterceptorChain(mInterceptors, 0, request, new StateCallbackAdapter(mIStateCallback, mICloseableFactory));
    }

    /**
     * 启动蓝牙操作
     * @param context
     * @param request
     * @return -- 启动状态
     */
    public int start(Context context, Request request){
        Log.i(TAG, "start request ="+request.toString());
        if(mEnable.compareAndSet(false, true)){
            mBleClient = new BluetoothLeClass(context);
            if(!mBleClient.initialize()){
                return IClient.STATE_INIT_FAILURE;
            }
            //开始处理
            RealInterceptorChain chain = createInterceptorChain(mBleClient, request);
            chain.process(request);
            mRealInterceptorChain = chain;
            return IClient.STATE_SUCCESS;
        }else {
            return IClient.STATE_WORKING;
        }
    }

    /**
     * 取消蓝牙操作
     */
    public void cancel(){
        if(mRealInterceptorChain != null){
            mRealInterceptorChain.cancel();
            mRealInterceptorChain = null;
        }
    }

    /**
     * 关闭蓝牙操作
     */
    public void close(){
        if(mEnable.compareAndSet(true, false)){
            cancel();
            Logdeal.D(TAG, "close");
            mBleClient.disconnect();
            //考虑部分手机在断开连接后,直接调用关闭操作会导致系统的回调调用出现异常信息
            //所以延时500ms后进行关闭操作
            Handler handler = new Handler(Looper.getMainLooper());
            handler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    mBleClient.close();
                }
            }, 500);
        }
    }

    /**
     * 蓝牙内部状态回调适配器
     */
    final class StateCallbackAdapter implements IStateCallback{
        IStateCallback mIStateCallback;
        ICloseable.Factory mCloseableFactory;
        public StateCallbackAdapter(IStateCallback callback, ICloseable.Factory factory) {
            this.mIStateCallback = callback;
            this.mCloseableFactory = factory;
        }

        @Override
        public void onState(int state, int code, String ssid, String msg) {
            Logdeal.D(TAG, String.format("state:%d, code:%d, ssid:%s, msg:%s", state, code, ssid != null ? ssid : "NULL", msg));
            if(mIStateCallback != null){
                mIStateCallback.onState(state, code, ssid, msg);
            }
            //蓝牙操作超过/被取消/连接断开
            if((code == CODE_FAILURE && state != STATE_ADVERTISE) || code > CODE_FAILURE){
                if(mCloseableFactory != null){
                    mCloseableFactory.create().close();
                }
            }
        }
    }

    /**
     * BluetoothClient构造器
     */
    public final static class Builder{
        //写数据超时时间
        private int mReadTimeout;
        //读数据超时时间
        private int mWriteTimeout;
        //扫描过滤服务
        private UUID[] mFilterUUIDs;
        //设备验证授权处理器
        private IAuthHandler mIAuthHandler;
        //广播数据处理器
        private IAdvertiseHandler mIAdvertiseHandler;
        //门禁数据处理器
        private IDataHandler mIDataHandler;
        //蓝牙服务验证处理器
        private IGattServiceAuthHandler mIGattServiceAuthHandler;
        //蓝牙状态回调接口
        private IStateCallback mIStateCallback;
        //上层触发设备客户端关闭接口
        private ICloseable.Factory mICloseableFactory;

        public Builder setTimeout(int writeTimeout, int readTimeout){
            this.mWriteTimeout = writeTimeout;
            this.mReadTimeout = readTimeout;
            return this;
        }

        public Builder setFilterUUIDs(UUID[] uuids) {
            this.mFilterUUIDs = uuids;
            return this;
        }

        public Builder setIAuthHandler(IAuthHandler authHandler) {
            this.mIAuthHandler = authHandler;
            return this;
        }

        public Builder setIAdvertiseHandler(IAdvertiseHandler advertiseHandler) {
            this.mIAdvertiseHandler = advertiseHandler;
            return this;
        }

        public Builder setIDataHandler(IDataHandler dataHandler) {
            this.mIDataHandler = dataHandler;
            return this;
        }

        public Builder setIGattServiceAuthHandler(IGattServiceAuthHandler gattServiceAuthHandler) {
            this.mIGattServiceAuthHandler = gattServiceAuthHandler;
            return this;
        }

        public Builder setIStateCallback(IStateCallback stateCallback) {
            this.mIStateCallback = stateCallback;
            return this;
        }

        public Builder setICloseableFactory(ICloseable.Factory factory) {
            this.mICloseableFactory = factory;
            return this;
        }

        public BluetoothClient build(){
            BluetoothClient client = new BluetoothClient(this);
            //注册实际设备客户端
            if(mICloseableFactory != null){
                mICloseableFactory.setIClient(client);
                //数据处理器中注册设备关闭回调接口
                if(mIDataHandler != null){
                    mIDataHandler.registerCloseableFactory(mICloseableFactory);
                }
            }
            return client;
        }
    }

}
