package com.qdingnet.opendoor.core.wifi;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.DhcpInfo;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkInfo;
import android.net.NetworkRequest;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.text.TextUtils;
import android.util.Pair;

import com.qdingnet.opendoor.core.interceptor.IDelegateClass;
import com.qdingnet.opendoor.log.Logdeal;

import java.util.List;

/**
 * wifi设备操作管理类
 * @author pipan (pipan@qding.me)
 * @date   2019/6/3
 */
public class WifiManagerClass implements IDelegateClass{
	private final static String TAG = "QC202/WifiConnectionManager";

	//应用上下文
	private Context mContext;
	//wifi管理器
	private WifiManager mWifiManager;
	//wifi连接管理器
	private ConnectivityManager mConnectivityManager;
	//手机默认连接wifi的SSID
	private String mSSID = null;
	//手机默认连接wifi的newId
	private int mNetID = -1;

	public WifiManagerClass(Context context){
		this.mContext = context;
	}

	@Override
	public boolean initialize() {
		mWifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
		mConnectivityManager = (ConnectivityManager)mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
		return mWifiManager != null && mConnectivityManager != null;
	}

	/**
	 * 获取应用上下文
	 * @return
	 */
	public Context getContext(){
		return mContext;
	}
	/**
	 * wifi是否打开
	 * @return
	 */
	public boolean wifiEnabled(){
		return mWifiManager.isWifiEnabled();
	}
	/**
	 * 打开wifi
	 * @return
	 */
	public boolean enableWifi(){
		Logdeal.D(TAG, "enableWifi");
		return mWifiManager.setWifiEnabled(true);
	}

	/**
	 * 扫描wifi
	 * @return
	 */
	public boolean startScan(){
		Logdeal.D(TAG, "startScan");
		return mWifiManager.startScan();
	}

	/**
	 * 获取wifi扫描结果
	 * @return
	 */
	public List<ScanResult> getScanResults(){
		return mWifiManager.getScanResults();
	}

	/**
	 * 连接指定wifi
	 * @param ssid
	 * @param password
	 * @param Type
	 * @return
	 */
	public boolean connection(String ssid, String password, WifiConnect.WifiCipherType Type) {
		WifiConnect connect = new WifiConnect(mWifiManager, mConnectivityManager);
		return connect.connect(ssid.trim(), password.trim(), Type);
	}

	/**
	 * 判断是否已连接指定的wifi
	 * @param ssid
	 * @return
	 */
	public boolean isConnected(String ssid){
		NetworkInfo mWifiInfo = mConnectivityManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
		if(mWifiInfo.isConnected()){
			WifiInfo info = mWifiManager.getConnectionInfo();
			if(info != null){
				String currentSSID = info.getSSID();
				Logdeal.D(TAG, " connectedSSID: " + currentSSID + ",BSSID:" + info.getBSSID() + ",name:" + ssid +",ip:" + info.getIpAddress());
				return currentSSID != null && currentSSID.contains(ssid) && info.getIpAddress() != 0;
			}
		}
		return false;
	}

	/**
	 * 重连wifi
	 * @return
	 */
	public boolean reconnect(){
		Logdeal.D(TAG, " reconnect...");
		return mWifiManager.reconnect();
	}

	/**
	 * 断开wifi连接
	 * @return
	 */
	public boolean disconnect(){
		WifiInfo info = mWifiManager.getConnectionInfo();
        if(info != null){
        	mWifiManager.disableNetwork(info.getNetworkId());
        }
        return mWifiManager.disconnect();
	}
	

	/**
	 * 获取当前连接的wifi的信号值rssi
	 * @return
	 */
	public int getRSSI(){
		WifiInfo info = mWifiManager.getConnectionInfo();
		return info != null ? info.getRssi():0;
	}

	/**
	 * 获取当前连接的wifi的SSID
	 * @return
	 */
	public String getSSID(){
		WifiInfo info = mWifiManager.getConnectionInfo();
		return info!=null ? info.getSSID():null;
	}

	/**
	 * 获取当前连接的wifi的IP
	 * @return
	 */
	public String getIpAddress(){
		WifiInfo info = mWifiManager.getConnectionInfo();
		if(info != null){
			int ip = info.getIpAddress();
			return (ip & 0xFF) + "." + ((ip >> 8) & 0xFF) + "."+ ((ip >> 16) & 0xFF) + "." + ((ip >> 24) & 0xFF);
		}else{
			return null;
		}
	}

	/**
	 * wifi是否DHCP成功与否
	 * @return
	 */
	public boolean isDHCPSuccess(){
		DhcpInfo info = mWifiManager.getDhcpInfo();
		return info != null && info.serverAddress == 0x010a0a0a;
	}

	/**
	 * 保存当前wifi信息
	 * @filterSsidNameRegex --过滤的ssid名称，符合规则不保存
	 */
	public void storeOriginWifi(String filterSsidNameRegex){
		WifiInfo info = mWifiManager.getConnectionInfo();
		if(info != null){
			String ssid =  info.getSSID();
			int    nid  =  info.getNetworkId();
			if(ssid != null && filterSsidNameRegex != null && !ssid.matches(filterSsidNameRegex) && nid != -1){
				mSSID = ssid;
				mNetID = nid;
				Logdeal.D(TAG, " storeOriginWifi... mSSID:" + mSSID + ", mNetID:" + mNetID);
			}
		}
	}

	/**
	 * 复原原来连接的wifi信息
	 * @filterSsidNameRegex --过滤的ssid名称，符合规则则需断开连接
	 */
	public void resetOriginWifi(String filterSsidNameRegex) {
		//ssid校验成功后断开当前连接的ap
		WifiInfo info = mWifiManager.getConnectionInfo();
		if(info != null){
			String cSSID = info.getSSID();
			if(cSSID != null && filterSsidNameRegex != null && cSSID.matches(filterSsidNameRegex)){
				mWifiManager.disableNetwork(info.getNetworkId());
				mWifiManager.removeNetwork(info.getNetworkId());
				mWifiManager.disconnect();
			}
		}
		if(mNetID != -1){
			Logdeal.D(TAG, " resetOriginWifi ssid=" + mSSID);
			mWifiManager.enableNetwork(mNetID, true);
			mWifiManager.reconnect();
		}
    }

	/**
	 * wifi连接操作类
	 */
	public static class WifiConnect {
		private final static String TAG = "WifiConnect";

		//wifi管理器
		private WifiManager wifiManager;
		//wifi连接管理器
		private ConnectivityManager mConnectivityManager;

		// 定义几种加密方式，一种是WEP，一种是WPA，还有没有密码的情况
		public enum WifiCipherType {
			WIFICIPHER_WEP, WIFICIPHER_WPA, WIFICIPHER_NOPASS, WIFICIPHER_INVALID
		}

		// 构造函数
		public WifiConnect(WifiManager wifiManager, ConnectivityManager connectivityManager) {
			this.wifiManager = wifiManager;
			this.mConnectivityManager = connectivityManager;
		}


		// 提供一个外部接口，传入要连接的无线网
		public boolean connect(String SSID, String Password, WifiCipherType Type) {
			Pair<WifiConfiguration, Integer> tempConfigPair = IsExist(SSID, Password, Type);
			WifiConfiguration tempConfig = tempConfigPair.first;
			int hightestPriority = tempConfigPair.second;
			int netID;
			if (tempConfig != null) {
				Logdeal.D(TAG, "connect...configuredSSID...state:" + tempConfig.status);
				//Maybe use DetailState
				//NetworkInfo.DetailedState currentState = WifiInfo.getDetailedStateOf(wifiManager.getConnectionInfo().getSupplicantState());
				if(tempConfig.status == WifiConfiguration.Status.CURRENT){
					return true;
				}
				netID = tempConfig.networkId;
				tempConfig.priority = hightestPriority + 1;
				wifiManager.updateNetwork(tempConfig);
			}else{
				WifiConfiguration wifiConfig = CreateWifiInfo(SSID, Password, Type);
				wifiConfig.priority = hightestPriority;
				netID = wifiManager.addNetwork(wifiConfig);
			}
			wifiManager.saveConfiguration();
			//关闭当前连接wifi
			WifiInfo wifiInfo = wifiManager.getConnectionInfo();
			if(wifiInfo != null){
				wifiManager.disableNetwork(wifiInfo.getNetworkId());
			}
			wifiManager.disconnect();
			wifiManager.enableNetwork(netID, true);
			boolean result = wifiManager.reconnect();
			//bindProcessToWifi();
			Logdeal.D(TAG," Connect manager:" + result + ",netID:" + netID + ",SSID:" + SSID);
			return result;
		}

		/**
		 * 绑定当前进程到wifi
		 */
		private void bindProcessToWifi(){
			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
				NetworkRequest.Builder builder = new NetworkRequest.Builder();
				//set the transport type do WIFI
				builder.addTransportType(NetworkCapabilities.TRANSPORT_WIFI);
				mConnectivityManager.requestNetwork(builder.build(), new ConnectivityManager.NetworkCallback() {
					@Override
					public void onAvailable(Network network) {
						if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
							mConnectivityManager.bindProcessToNetwork(network);
						} else {
							ConnectivityManager.setProcessDefaultNetwork(network);
						}
						mConnectivityManager.unregisterNetworkCallback(this);
					}
				});
			}
		}

		/**
		 * 查看以前是否也配置过这个网络
		 * @param ssid
		 * @return
		 */
		private Pair<WifiConfiguration, Integer> IsExist(String ssid, String Password, WifiCipherType Type) {
			WifiConfiguration wifiConfiguration = null;
			int highestPriority = 0;
			int securityType = transformSecurityType(Type);
			List<WifiConfiguration> configuredNetworks = wifiManager.getConfiguredNetworks();
			for (WifiConfiguration config : configuredNetworks) {
				if (config.SSID.equals(convertToQuotedString(ssid)) && config.allowedKeyManagement.get(securityType)) {
					wifiConfiguration = config;
				}
				if(config.priority > highestPriority){
					highestPriority = config.priority;
				}
			}
			return Pair.create(wifiConfiguration, highestPriority);
		}

		/**
		 * 转换密码安全类型
		 * @param Type
		 * @return
		 */
		private int transformSecurityType(WifiCipherType Type){
			int type = -1;
			switch (Type){
				case WIFICIPHER_WEP:
				case WIFICIPHER_NOPASS:
					 type = WifiConfiguration.KeyMgmt.NONE;
					 break;
				case WIFICIPHER_WPA:
					 type = WifiConfiguration.KeyMgmt.WPA_PSK;
					 break;
				default:break;
			}
			return type;
		}

		/**
		 * 创建wifi配置信息
		 * @param SSID
		 * @param Password
		 * @param Type
		 * @return
		 */
		private WifiConfiguration CreateWifiInfo(String SSID, String Password, WifiCipherType Type) {
			WifiConfiguration config = new WifiConfiguration();
			config.allowedAuthAlgorithms.clear();
			config.allowedGroupCiphers.clear();
			config.allowedKeyManagement.clear();
			config.allowedPairwiseCiphers.clear();
			config.allowedProtocols.clear();
			config.SSID = convertToQuotedString(SSID);

			if (Type == WifiCipherType.WIFICIPHER_NOPASS) {
				config.wepKeys[0] = "";
				config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
				config.wepTxKeyIndex = 0;
			} else if (Type == WifiCipherType.WIFICIPHER_WEP) {
				config.preSharedKey = "\"" + Password + "\"";
				config.hiddenSSID = true;
				config.allowedAuthAlgorithms.set(WifiConfiguration.AuthAlgorithm.SHARED);
				config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
				config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
				config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.WEP40);
				config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.WEP104);
				config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.NONE);
				config.wepTxKeyIndex = 0;
			} else if (Type == WifiCipherType.WIFICIPHER_WPA) {
				if (Password.matches("[0-9A-Fa-f]{64}")) {
					config.preSharedKey = Password;
				} else {
					config.preSharedKey = "\""+  Password + "\"";
				}
				config.hiddenSSID = true;
				config.allowedAuthAlgorithms.set(WifiConfiguration.AuthAlgorithm.OPEN);
				config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
				config.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.WPA_PSK);
				config.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.TKIP);
				//config.allowedProtocols.set(WifiConfiguration.Protocol.WPA);
				config.status = WifiConfiguration.Status.ENABLED;
				config.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
				config.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.CCMP);
				config.priority = 1000;
			} else {
				return null;
			}
			return config;
		}

		private String convertToQuotedString(String string) {
			if (TextUtils.isEmpty(string)) {
				return "";
			}
			final int lastPos = string.length() - 1;
			if(lastPos > 0 && (string.charAt(0) == '"' && string.charAt(lastPos) == '"')) {
				return string;
			}
			return "\"" + string + "\"";
		}
	}
}
