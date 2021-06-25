package com.qdingnet.opendoor.log;

import android.util.Log;

public class Logdeal {

    public static String TAG = "QC202&QC204";
    public static boolean OPEN = true;

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
}
