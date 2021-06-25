package com.qdingnet.opendoor.utils;


import android.util.Log;

import java.io.UnsupportedEncodingException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.SecretKeySpec;

/**
 * AES加密工具类
 * http://my.oschina.net/tinglanrmb32/blog/351021
 * @author pipan (pipan@qding.me)
 * @date   19-3-25
 */
public class AESUtil {
    private static final String TAG = "AESUtil";

    /**
     * 加密
     * @param sSrc
     * @param sKey
     * @return
     * @throws Exception
     */
    public static byte[] encrypt(String sSrc, String sKey){
        if (sKey == null) {
            Log.d(TAG,"Key is Null");
            return null;
        }
        if (sKey.length() != 16) {
            Log.d(TAG,"Key's length is not equals to 16");
            return null;
        }
        try {
            //构造加密key
            byte[] raw = sKey.getBytes("UTF-8");
            SecretKeySpec sKeySpec = new SecretKeySpec(raw, "AES");
            //"算法/模式/补码方式"
            Cipher cipher = Cipher.getInstance("AES/ECB/PKCS5Padding");
            cipher.init(Cipher.ENCRYPT_MODE, sKeySpec);
            //加密
            return cipher.doFinal(sSrc.getBytes("UTF-8"));
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        } catch (NoSuchPaddingException e) {
            e.printStackTrace();
        } catch (InvalidKeyException e) {
            e.printStackTrace();
        } catch (IllegalBlockSizeException e) {
            e.printStackTrace();
        } catch (BadPaddingException e) {
            e.printStackTrace();
        }
        return null;
    }

    /**
     * 加密并用Base64转码
     * @param sSrc
     * @param sKey
     * @return
     */
    public static String encryptToBase64(String sSrc, String sKey){
        String result = "";
        try {
            byte[] encode = encrypt(sSrc, sKey);
            result = Base64.encodeToString(encode, Base64.URL_SAFE);
        }catch (Exception e){
            e.printStackTrace();
        }
        return result;
    }

    /**
     * 加密并用Hex转码
     * @param sSrc
     * @param sKey
     * @return
     */
    public static String encryptToHex(String sSrc, String sKey){
        String result = "";
        try {
            byte[] encode = encrypt(sSrc, sKey);
            result = new String(Hex.encodeHex(encode));
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }

    /**
     * 解密
     * @param encode
     * @param sKey
     * @return
     * @throws Exception
     */
    public static byte[] decrypt(byte[] encode, String sKey) {
        if (sKey == null) {
            Log.d(TAG,"Key is Null");
            return null;
        }
        if (sKey.length() != 16) {
            Log.d(TAG,"Key's length is not equals to 16");
            return null;
        }
        try {
            //构造加密key
            byte[] raw = sKey.getBytes("UTF-8");
            SecretKeySpec skeySpec = new SecretKeySpec(raw, "AES");
            //"算法/模式/补码方式"
            Cipher cipher = Cipher.getInstance("AES/ECB/PKCS5Padding");
            cipher.init(Cipher.DECRYPT_MODE, skeySpec);
            //解密
            return cipher.doFinal(encode);
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        } catch (NoSuchPaddingException e) {
            e.printStackTrace();
        } catch (InvalidKeyException e) {
            e.printStackTrace();
        } catch (IllegalBlockSizeException e) {
            e.printStackTrace();
        } catch (BadPaddingException e) {
            e.printStackTrace();
        }
        return null;
    }

    /**
     * Base64解码并解密
     * @param sSrc
     * @param sKey
     * @return
     * @throws Exception
     */
    public static String decryptFromBase64(String sSrc, String sKey) {
        String result = "";
        try {
            byte[] encode = Base64.decode(sSrc, Base64.URL_SAFE);
            byte[] decode = decrypt(encode, sKey);
            result = new String(decode, "UTF-8");
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }

    /**
     * Hex解码并解密
     * @param sSrc
     * @param sKey
     * @return
     * @throws Exception
     */
    public static String decryptFromHex(String sSrc, String sKey) {
        String result = "";
        try {
            byte[] encode = Hex.decodeHex(sSrc.toCharArray());
            byte[] decode = decrypt(encode, sKey);
            if(decode != null){
                result = new String(decode, "UTF-8");
            }
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }

    public static void main(String[] args) throws Exception {

    }
	
}
