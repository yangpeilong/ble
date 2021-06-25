package com.qdingnet.provider.config.upgrade;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;

import com.qdingnet.provider.config.Logdeal;
import com.qdingnet.provider.config.bean.Contants;

/**
 * web后台服务
 * @author pipan (pipan@qding.me)
 * @date   2019/5/29
 */
public class WebService extends Service {
	private final static String TAG = "WebService";

	private static final String WEBROOT = "/";
	private WebServer webServer;

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	@Override
	public void onCreate() {
		Logdeal.D(TAG, "onCreate...");
		super.onCreate();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		Logdeal.D(TAG, " onStartCommand...");
		if (webServer == null || (webServer.isClosed() && webServer.getState().equals(Thread.State.TERMINATED))) {
			webServer = new WebServer(Contants.QDWifi.LOCAL_FIRMWARE_DOWNLOAD_SERVER_PORT, WEBROOT);
		}
		if (webServer.getState().equals(Thread.State.NEW)) {
		    //不需要本地html,css,js相关文件
			//new CopyUtil(this).assetsCopy();
			webServer.setDaemon(true);
			webServer.start();
		} 
		return super.onStartCommand(intent, flags, startId);
	}

	@Override
	public void onDestroy() {
		webServer.close();
		super.onDestroy();
	}

	/**
	 * 启动web服务
	 * @param context
	 */
	public static void start(Context context){
		Intent intent = new Intent(context, WebService.class);
		context.startService(intent);
	}

	/**
	 * 关闭web服务
	 * @param context
	 */
	public static void stop(Context context){
		Intent intent = new Intent(context, WebService.class);
		context.stopService(intent);
	}
}
