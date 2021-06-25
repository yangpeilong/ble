package com.qdingnet.provider.config.callback.impl;

import com.qdingnet.opendoor.core.callback.IStateCallback;
import com.qdingnet.provider.config.Logdeal;
import com.qdingnet.provider.config.bean.QDAccessResult;
import com.qdingnet.provider.config.bean.StateGuard;

import java.util.Locale;

/**
 * 设备操作状态回调实现基类
 * @author pipan (pipan@qding.me)
 * @date   2019/5/29
 */
public abstract class AbstractStateCallback implements IStateCallback {
    private static final String TAG = "AbstractStateCallback";

    //蓝牙开门方式
    public static final int MODE_BLE = 1;
    //WIFI开门方式
    public static final int MODE_WIFI = 2;
    //扫码开门方式
    public static final int MODE_SCAN_QRCODE = 3;

    //开门方式
    protected int mMode;
    //设备状态机管理器
    private final StateGuard mStateGuard;

    public AbstractStateCallback(int mode, StateGuard stateGuard) {
        this.mMode = mode;
        this.mStateGuard = stateGuard;
    }

    @Override
    public void onState(int state, int code, String ssid, String msg) {
        Logdeal.D(TAG, String.format(Locale.US,"onState...state:%d, code:%d, ssid:%s, msg:%s", state, code, ssid != null ? ssid : "NULL", msg != null ? msg : "NULL"));
        //操作执行成功则记录统计数据
        if(code == IStateCallback.CODE_SUCCESS){
            onStateSuccess();
        }
        //操作执行失败，则由各具体业务执行操作
        else{
            QDAccessResult result = handleFailureStateResult(state, code);
            //状态传播
            onStateFailure(ssid, result);
        }
    }

    /**
     * 成功状态回调
     */
    protected abstract void onStateSuccess();

    /**
     * 失败状态回调
     * @param result
     */
    protected abstract void onStateFailure(String ssid, QDAccessResult result);

    /**
     * 处理设备操作失败状态结果
     * @param state
     * @param code
     * @return
     */
    private QDAccessResult handleFailureStateResult(int state, int code){
        QDAccessResult result = null;
        //蓝牙
        if(mMode == MODE_BLE){
            //操作失败
            if(code == IStateCallback.CODE_FAILURE){
                if(state == IStateCallback.STATE_SCAN){
                    result = QDAccessResult.ERROR_NO_BLUETOOTH_PERMISSION;
                } else if(state == IStateCallback.STATE_ADVERTISE){
                    //ignore
                } else if(state == IStateCallback.STATE_CONNECT){
                    result = QDAccessResult.ERROR_CONNECT_BLE_FAILURE;
                } else if(state == IStateCallback.STATE_SEND_DATA){
                    result = QDAccessResult.ERROR_WRITE_BLE_DATA_EXCEPTION;
                } else if(state == IStateCallback.STATE_RECV_DATA){
                    result = QDAccessResult.ERROR_READ_BLE_DATA_EXCEPTION;
                } else {
                    result = QDAccessResult.ERROR_OPEN_DOOR_FAILURE;
                }
            }
            //操作超时
            else if(code == IStateCallback.CODE_TIMEOUT){
                if(state == IStateCallback.STATE_SCAN){
                    result = QDAccessResult.ERROR_NO_FIND_DEVICE;
                    if(mStateGuard != null){
                        if(mStateGuard.isStateScanFindOpenDoorDevice()){
                            result = QDAccessResult.ERROR_BLE_OPEN_DOOR_TIMEOUT;
                        }
                        else if(mStateGuard.isStateScanFindOperableDevice()){
                            result = QDAccessResult.ERROR_TOO_FAR_DEVICE;
                        }
                        else if(mStateGuard.isStateScanFindDevice()){
                            result = QDAccessResult.ERROR_NO_FIND_DEVICE_NOAUTHORIZE;
                        }
                    }
                } else if(state == IStateCallback.STATE_CONNECT){
                    result = QDAccessResult.ERROR_CONNECT_BLE_TIMEOUT;
                } else if(state == IStateCallback.STATE_DISCOVER_SERVICE){
                    result = QDAccessResult.ERROR_FIND_BLE_SERVICE_TIMEOUT;
                } else if(state == IStateCallback.STATE_SEND_DATA){
                    result = QDAccessResult.ERROR_SEND_DATA_TIMEOUT;
                } else if(state == IStateCallback.STATE_RECV_DATA){
                    result = QDAccessResult.ERROR_RECEIVE_DATA_TIMEOUT;
                } else {
                    result = QDAccessResult.ERROR_BLE_OPEN_DOOR_TIMEOUT;
                }
            }
            //连接断开
            else if(code == IStateCallback.CODE_DISCONNECT){
                result = QDAccessResult.ERROR_DISCONNECT_DEVICE_FAILURE;
            }
        }
        //wifi
        else if(mMode == MODE_WIFI){
            //操作失败
            if(code == IStateCallback.CODE_FAILURE){
                if(state == IStateCallback.STATE_CONNECT){
                    result = QDAccessResult.ERROR_CONNECT_WIFI_FAILURE;
                }else {
                    result = QDAccessResult.ERROR_OPEN_DOOR_FAILURE;
                }
            }
            //操作超时
            else if(code == IStateCallback.CODE_TIMEOUT){
                if(state == IStateCallback.STATE_SCAN){
                    result = QDAccessResult.ERROR_NO_FIND_DEVICE;
                    if(mStateGuard != null){
                        if(mStateGuard.isStateScanFindOpenDoorDevice()){
                            result = QDAccessResult.ERROR_WIFI_OPENDOOR_TIMEOUT;
                        }
                        else if(mStateGuard.isStateScanFindOperableDevice()){
                            result = QDAccessResult.ERROR_TOO_FAR_DEVICE;
                        }
                        else if(mStateGuard.isStateScanFindDevice()){
                            result = QDAccessResult.ERROR_NO_FIND_DEVICE_NOAUTHORIZE;
                        }
                    }
                } else if(state == IStateCallback.STATE_CONNECT){
                    result = QDAccessResult.ERROR_CONNECT_WIFI_TIMEOUT;
                } else {
                    result = QDAccessResult.ERROR_WIFI_OPENDOOR_TIMEOUT;
                }
            }
            //网络断开
            else if(code == IStateCallback.CODE_DISCONNECT){
                result = QDAccessResult.ERROR_DISCONNECT_DEVICE_FAILURE;
            }
        }
        return result;
    }

}
