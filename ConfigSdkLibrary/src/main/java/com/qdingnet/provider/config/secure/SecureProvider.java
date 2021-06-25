package com.qdingnet.provider.config.secure;

import android.content.Context;

import com.qdingnet.opendoor.utils.Base64;
import com.qdingnet.qdaccess.ProtocolHelper;

/**
 * 安全提供者
 * @author pipan (pipan@qding.me)
 * @date   2019/5/7
 */
public class SecureProvider {

    /**
     * 注册安全控件
     * @param context
     * @return
     */
    public static final int register(Context context){
        return ProtocolHelper.register(context);
    }

    /**
     * 原文字符串加密，输出Base64编码的字符串
     * @param src
     * @return
     */
    public static final String encrypt(String src){
        String result = "";
        if(!isEmpty(src)){
            try {
                byte[] encode = ProtocolHelper.encrypt(src.getBytes("UTF-8"));
                result = Base64.encodeToString(encode, Base64.URL_SAFE);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return result;
    }

    /**
     * 原文字符串先进行Base64解码，然后进行解码，输出字符串
     * @param src
     * @return
     */
    public static final String decrypt(String src){
        String result = "";
        if(!isEmpty(src)){
            try {
                byte[] encode = Base64.decode(src, Base64.URL_SAFE);
                result = new String(ProtocolHelper.decrypt(encode));
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return result;
    }

    /**
     * 判断字符串是否为空
     * @param src
     * @return
     */
    static final boolean isEmpty(CharSequence src){
        return src == null || src.length() == 0;
    }
}
