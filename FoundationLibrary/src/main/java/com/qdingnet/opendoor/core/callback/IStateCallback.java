package com.qdingnet.opendoor.core.callback;

/**
 * 处理蓝牙各拦截器中状态回调接口
 * @author pipan (pipan@qding.me)
 * @date   19-1-22
 */
public interface IStateCallback {
    //初始化状态
    int STATE_INIT = 0;
    //扫描设备
    int STATE_SCAN = 1;
    //发送广播开门
    int STATE_ADVERTISE = 2;
    //连接设备
    int STATE_CONNECT = 3;
    //发现服务
    int STATE_DISCOVER_SERVICE = 4;
    //发送数据到设备
    int STATE_SEND_DATA = 5;
    //从设备接收数据
    int STATE_RECV_DATA = 6;

    //成功
    int CODE_SUCCESS = 0;
    //失败
    int CODE_FAILURE = 1;
    //超时
    int CODE_TIMEOUT = 2;
    //操作被取消
    int CODE_CANCELED = 3;
    //蓝牙连接已断开
    int CODE_DISCONNECT = 4;

    /**
     * 状态回调
     * @param state -- 操作状态字 @see {@link # STATE_*}
     * @param code  -- 操作状态码 @see {@link # CODE_* }
     * @param ssid  -- 操作设备的ssid
     * @param msg   -- 操作失败信息
     */
    void onState(int state, int code, String ssid, String msg);
}
