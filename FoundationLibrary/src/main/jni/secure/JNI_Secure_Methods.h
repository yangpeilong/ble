/*
 * NativeMethods.h
 *
 *  Created on: Apr 1, 2014
 *      Author: china
 */

#ifndef JNI_SECURE_METHODS_H_
#define JNI_SECURE_METHODS_H_
/*注意不要进行代码格式化，会搞乱下面的方法格式*/
#include <jni.h>
#include "config.h"

jint JNI_register(JNIEnv * env, jobject obj, jobject context);
jint JNI_register_with_auth_message(JNIEnv * env, jobject obj, jobject context, jbyteArray auth_message);
jbyteArray JNI_encrypt(JNIEnv * env, jobject obj, jbyteArray packet);
jbyteArray JNI_decrypt(JNIEnv * env, jobject obj, jbyteArray packet);

static JNINativeMethod secure_methods[] =
		{
		  { "register", "(Ljava/lang/Object;)I" , (void*)JNI_register},
		  { "register", "(Ljava/lang/Object;[B)I" , (void*)JNI_register_with_auth_message},
		  { "encrypt", "([B)[B" , (void*)JNI_encrypt},
		  { "decrypt", "([B)[B" , (void*)JNI_decrypt},
		};
#endif /* JNI_METHODS_H_ */
