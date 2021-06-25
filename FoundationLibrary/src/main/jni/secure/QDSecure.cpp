//
// Created by pipan on 19-3-25.
//

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "JNI_Secure_Methods.h"
#include "../access/debugLog.h"
#include "md5.h"
#include "config.h"

//授权标志
static int authorized = NO_AUTHORIZE;
//授权名单
static const authorize_info* authorized_info = NULL;
//判断是否已授权
bool isAuthorized = ENABLE_AUTHORIZE == 0;
//可开门设备识别码
const char* AUTH_CODE = NULL;

/**
 * java端字符串转c字符串
 * @param env
 * @param jstr
 * @return
 */
static char *jstring_to_string(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}

/**
 * 验证应用包名
 * @param env
 * @param context
 * @return
 */
authorize_info* checkPackageName(JNIEnv * env, jobject context){
    jclass context_class = env->GetObjectClass(context);
    //得到getPackageInfo方法的ID
    jmethodID mId = env->GetMethodID(context_class, "getPackageName", "()Ljava/lang/String;");
    jstring packageName = static_cast<jstring>(env->CallObjectMethod(context, mId));
    char* c_packageName = jstring_to_string(env, packageName);
    int index = -1;
    for(int i=0, size= sizeof(authorizeTables) / sizeof(authorizeTables[0]); i<size; i++)
    {
        if(strcmp(authorizeTables[i].package_name, c_packageName) == 0)
        {
            index = i;
            break;
        }
    }
    LOGD("%s....package:%s, index:%d", __FUNCTION__, c_packageName, index);
    //删除本地资源及引用
    env->DeleteLocalRef(context_class);
    env->DeleteLocalRef(packageName);
    if(c_packageName)
    {
        free(c_packageName);
    }
    return index != -1 ? &authorizeTables[index] : (authorize_info*)NULL;
}

/**
 * 验证应用签名
 * @param env
 * @param context
 * @return
 */
bool checkSignature(JNIEnv * env, jobject context, authorize_info* authorize_info){
    //获得应用包的信息
    jclass context_class = env->GetObjectClass(context);
    jmethodID pm_id = env->GetMethodID(context_class, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    jobject pm_obj = env->CallObjectMethod(context, pm_id);
    jclass pm_clazz = env->GetObjectClass(pm_obj);
    //获得 PackageInfo 类
    jmethodID package_info_id = env->GetMethodID(pm_clazz, "getPackageInfo","(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    jstring package_name = env->NewStringUTF(authorize_info->package_name);
    jobject package_info_obj = env->CallObjectMethod(pm_obj, package_info_id, package_name, 64);
    jclass package_info_clazz = env->GetObjectClass(package_info_obj);
    //获得签名数组属性的 ID
    jfieldID signatures_fieldId = env->GetFieldID(package_info_clazz, "signatures", "[Landroid/content/pm/Signature;");
    jobject signatures_obj = env->GetObjectField(package_info_obj, signatures_fieldId);
    jobjectArray signaturesArray = (jobjectArray)signatures_obj;
    jsize size = env->GetArrayLength(signaturesArray);
    //生成签名md5值
    unsigned char digest[MD5_HASHBYTES];
    MD5_CTX ctx;
    MD5Init(&ctx);
    for(int i=0; i<size; i++)
    {
        jobject signature_obj = env->GetObjectArrayElement(signaturesArray, i);
        jclass signature_clazz = env->GetObjectClass(signature_obj);
        jmethodID signature_method = env->GetMethodID(signature_clazz, "toByteArray", "()[B");
        jbyteArray bytes = (jbyteArray)env->CallObjectMethod(signature_obj, signature_method);
        jbyte* signature_bytes = env->GetByteArrayElements(bytes, JNI_FALSE);
        MD5Update(&ctx, (const unsigned char*)signature_bytes, env->GetArrayLength(bytes));
        //释放本地引用
        env->DeleteLocalRef(signature_obj);
        env->DeleteLocalRef(signature_clazz);
        env->ReleaseByteArrayElements(bytes, signature_bytes, JNI_ABORT);
    }
    MD5Final(digest, &ctx);
    //生成hex格式
    char hex_digest[MD5_HASHBYTES * 2 + 1] = {0};
    for (int i = 0; i < MD5_HASHBYTES; i++) {
        sprintf(hex_digest + (i * 2), "%02X", digest[i]);
    }
    //释放本地引用
    env->DeleteLocalRef(context_class);
    env->DeleteLocalRef(pm_obj);
    env->DeleteLocalRef(pm_clazz);
    env->DeleteLocalRef(package_name);
    env->DeleteLocalRef(package_info_obj);
    env->DeleteLocalRef(package_info_clazz);
    env->DeleteLocalRef(signatures_obj);
    //比较签名
    uint32_t count = sizeof(authorize_info->signature_md5) / sizeof(authorize_info->signature_md5[0]);
    int signatureBlankCount = 0;
    for(int i = 0; i < count; i++){
        if(authorize_info->signature_md5[i] == NULL)
        {
            signatureBlankCount++;
        }
        else if(strcmp(hex_digest, authorize_info->signature_md5[i]) == 0)
        {
            return true;
        }
    }
    return signatureBlankCount == count;
}

/**
 * 安全控件注册
 * @param env
 * @param obj
 * @param context
 */
jint JNI_register(JNIEnv * env, jobject obj, jobject context)
{
    //执行授权验证
    if(ENABLE_AUTHORIZE)
    {
        //包名验证
        authorize_info* authorize_info = checkPackageName(env, context);
        //包名验证失败
        if(authorize_info == NULL)
        {
            authorized = AUTHORIZE_FAILURE;
        }
        else
        {
            //签名验证
            if(checkSignature(env, context, authorize_info))
            {
                authorized = AUTHORIZE_OK;
                authorized_info = authorize_info;
                AUTH_CODE = authorize_info->auth_code;
            }
            else
            {
                authorized = AUTHORIZE_FAILURE;
            }
        }
        //设置授权标志
        isAuthorized = authorized == AUTHORIZE_OK;
    }
    return authorized;
}

/**
 * 安全控件注册(带验证信息)
 * @param env
 * @param obj
 * @param context
 * @param auth_message
 * @return
 */
jint JNI_register_with_auth_message(JNIEnv * env, jobject obj, jobject context, jbyteArray auth_message){
    //TODO
    return authorized;
}

/**
 * 获取加密key
 * @param env
 * @return
 */
static jbyteArray get_AES128_Key(JNIEnv * env){
    const char* key;
    if(ENABLE_AUTHORIZE)
    {
        if(authorized == NO_AUTHORIZE)
        {
            LOGE("Please Call ProtocolHelper.register() method");
            key = "";
        } else if(authorized == AUTHORIZE_OK)
        {
            key = authorized_info->key;
        } else
        {
            key = "";
        }
    }
    else
    {
        key = AES128_KEY;
    }
    size_t len = strlen(key);
    jbyteArray keys = env->NewByteArray(len);
    if(len > 0)
    {
        env->SetByteArrayRegion(keys, 0, len, (jbyte*)key);
    }
    return keys;
}

/**
 * 数据加解密
 * @param env
 * @param obj
 * @param packet
 * @param mode
 * @return
 */
static jbyteArray cipher(JNIEnv* env, jbyteArray packet, jint mode){
    jbyteArray result;
    jbyteArray keys = get_AES128_Key(env);
    jsize length = env->GetArrayLength(keys);
    if(length == AES_KEY_LENGTH)
    {
        //构造javax.crypto.spec.SecretKeySpec类
        jclass SecretKeySpecClass = env->FindClass("javax/crypto/spec/SecretKeySpec");
        jmethodID SecretKeySpecConstructor = env->GetMethodID(SecretKeySpecClass, "<init>", "([BLjava/lang/String;)V");
        jstring aes = env->NewStringUTF(JAVA_ALGORITHM);
        jobject SecretKeySpec = env->NewObject(SecretKeySpecClass, SecretKeySpecConstructor, keys, aes);
        //构造javax.crypto.Cipher类
        jclass CipherClass = env->FindClass("javax/crypto/Cipher");
        jmethodID GetInstanceMethod = env->GetStaticMethodID(CipherClass, "getInstance", "(Ljava/lang/String;)Ljavax/crypto/Cipher;");
        jstring transformation = env->NewStringUTF(AES_JAVA_MODE);
        jobject Cipher = env->CallStaticObjectMethod(CipherClass, GetInstanceMethod, transformation);
        //调用cipher.init()
        jmethodID InitMethod = env->GetMethodID(CipherClass, "init", "(ILjava/security/Key;)V");
        env->CallVoidMethod(Cipher, InitMethod, mode, SecretKeySpec);
        //调用cipher.doFinal()
        jmethodID DoFinalMethod = env->GetMethodID(CipherClass, "doFinal", "([B)[B");
        result = (jbyteArray)env->CallObjectMethod(Cipher, DoFinalMethod, packet);
        //释放本地引用
        env->DeleteLocalRef(SecretKeySpecClass);
        env->DeleteLocalRef(aes);
        env->DeleteLocalRef(SecretKeySpec);
        env->DeleteLocalRef(CipherClass);
        env->DeleteLocalRef(transformation);
        env->DeleteLocalRef(Cipher);
    }
    else
    {
        result = env->NewByteArray(0);
    }
    //释放本地引用
    env->DeleteLocalRef(keys);
    return result;
}

/**
 * 加密
 * @param env
 * @param obj
 * @param packet
 * @return
 */
jbyteArray JNI_encrypt(JNIEnv* env, jobject obj, jbyteArray packet){
    //Cipher.ENCRYPT_MODE = 1
    return cipher(env, packet, 1);
}
/**
 * 解密
 * @param env
 * @param obj
 * @param packet
 * @return
 */
jbyteArray JNI_decrypt(JNIEnv * env, jobject obj, jbyteArray packet){
    //Cipher.DECRYPT_MODE = 2
    return cipher(env, packet, 2);
}
