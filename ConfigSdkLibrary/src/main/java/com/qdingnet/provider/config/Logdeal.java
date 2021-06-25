package com.qdingnet.provider.config;

import android.util.Log;

public class Logdeal {

    private static String TAG = "CONFIG";
    private static boolean OPEN = true;

    public static void setSwitch(boolean open) {
        OPEN = open;
    }

    public static void D(String message) {
        if (OPEN) {
            Log.d(TAG, message);
        }
    }

    public static void D(String tag, String message) {
        if (OPEN) {
            Log.d(TAG, tag + ":" + message);
        }
    }

    public static void D(String tag, String message, Throwable throwable) {
        if (OPEN) {
            Log.d(TAG,  tag + ":" + message, throwable);
        }
    }

    public static void E(String tag, String message) {
        Log.e(TAG, tag + ":" + message);
    }

    public static void E(String tag, String message, Throwable throwable){
        Log.e(TAG, tag + ":" + message, throwable);
    }
}
