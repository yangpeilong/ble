package com.qdingnet.opendoor.core.misc;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;

/**
 * 拦截器消息处理器
 * @author pipan (pipan@qding.me)
 * @date   19-1-15
 */
public class MessageHandler implements Handler.Callback{
    private static final String TAG = "MessageHandler";

    //处理器名称标志
    private String mName;
    private HandlerThread mThread;
    private Handler mHandler;

    public MessageHandler(String name) {
        this.mName = name;
        this.mThread = new HandlerThread(name);
        this.mThread.start();
        this.mHandler = new Handler(mThread.getLooper(), this);
    }


    /**
     * 启动操作
     * @param timeout
     */
    public void execute(Runnable runnable, int timeout){
        if(mThread != null && mThread.isAlive()){
            mHandler.postDelayed(runnable, timeout);
        }else {
            Log.e(TAG, "mThread is null or not alive");
        }
    }

    /**
     * 取消Runnable操作
     */
    public void cancel(Runnable runnable){
        if(mHandler != null){
            mHandler.removeCallbacks(runnable);
        }
    }

    /**
     * 取消操作
     */
    public void cancel(){
        if(mHandler != null){
            mHandler.removeCallbacksAndMessages(null);
        }
    }

    /**
     * 关闭操作
     */
    public void close(){
        cancel();
        if(mThread != null){
            mThread.quit();
            mThread = null;
        }
    }

    @Override
    public boolean handleMessage(Message msg) {

        return false;
    }
}
