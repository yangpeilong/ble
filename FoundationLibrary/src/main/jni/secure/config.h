//
// Created by pipan on 19-3-26.
//
#ifndef CONFIG_H
#define CONFIG_H

//加解密算法
#define JAVA_ALGORITHM "AES"
//AES模式配置信息
#define AES_KEY_LENGTH 16
#define AES_JAVA_MODE "AES/ECB/PKCS5Padding"
//AES加解密key
#define AES128_KEY "e6gjISYIExhP7Wg1"
//使能授权标志
#define ENABLE_AUTHORIZE   0
//授权状态
#define NO_AUTHORIZE       0
#define AUTHORIZE_OK       1
#define AUTHORIZE_FAILURE -1
//单个app可授权签名最大数量
#define MAX_SIGNATURE_MD5_NUM 2
//授权白名单结构体(包名:md5(签名):加解密秘钥(AES))
struct authorize_info{
    const char* package_name;
    const char* signature_md5[MAX_SIGNATURE_MD5_NUM];
    const char* key;
    const char* auth_code;
};
//授权白名单
static authorize_info authorizeTables[] = {
        //千丁物联app
        {"com.qding.qdingsmart",                       {"3CBD7B46D9DB27DC40F15BBBF5B3BFCB"},                                     "e6gjISYIExhP7Wg1"},
        //千丁工程管理工具
        {"com.qdingnet.provider.project.manager.app",  {"D735645B6BE5F5C55186E7AEBCAEB172"},                                     "e6gjISYIExhP7Wg1"},
        //千丁门禁二代配置工具
        {"com.qdingnet.qdoorconfig",                   {"5BD32589831E932D8D8089C2D5B4295A"},                                     "e6gjISYIExhP7Wg1"},
        //千丁门禁二代第三方配置工具
        {"com.qding.opendoor.config",                  {"5BD32589831E932D8D8089C2D5B4295A"},                                     "e6gjISYIExhP7Wg1"},
        //千丁门禁二代产测工具
        {"com.qdingnet.qdoorfactroytest",              {"82BE61977005B6D958CEC1A537B08C82"},                                     "e6gjISYIExhP7Wg1"},
        //千丁门禁四代产测工具
        {"com.qdingnet.qdoorfactroytest.v4",           {"82BE61977005B6D958CEC1A537B08C82"},                                     "e6gjISYIExhP7Wg1"},
        //千丁门禁五代产测工具
        {"com.qdingnet.factory.v5",                    {"82BE61977005B6D958CEC1A537B08C82"},                                     "e6gjISYIExhP7Wg1"},
        //千丁sdk sample
        {"com.qdingnet.access.library",                {"72C654ACA8E442FA6DD24A174E7BCE2D"},                                     "e6gjISYIExhP7Wg1"},
        //第三方sdk sample
        {"com.qdingnet.provider.sample",               {"72C654ACA8E442FA6DD24A174E7BCE2D"},                                     "e6gjISYIExhP7Wg1"},
        //门禁配置sdk sample
        {"com.qdingnet.config.sample",                 {"72C654ACA8E442FA6DD24A174E7BCE2D"},                                     "e6gjISYIExhP7Wg1"},
        //门禁厂测sdk sample
        {"com.qdingnet.factory.sample",                {"72C654ACA8E442FA6DD24A174E7BCE2D"},                                     "e6gjISYIExhP7Wg1"},
        //门丁app
        {"com.qding.community",                        {"3CBD7B46D9DB27DC40F15BBBF5B3BFCB", "72C654ACA8E442FA6DD24A174E7BCE2D"}, "e6gjISYIExhP7Wg1"},
        //小丁管家app/丁管家app
        {"com.qding.guanjia",                          {"5C5D55AB37B2B2D5C8056F3BA1CB5388", "72C654ACA8E442FA6DD24A174E7BCE2D"}, "e6gjISYIExhP7Wg1"},
        //云对讲sdk demo
        {"com.xqx.app.third.demo",                     {"72C654ACA8E442FA6DD24A174E7BCE2D"},                                     "e6gjISYIExhP7Wg1"},
        //门禁设备端sdk sample
        {"com.qdingnet.provider.device.sdk.demo",      {"72C654ACA8E442FA6DD24A174E7BCE2D"},                                     "e6gjISYIExhP7Wg1"},
        //物联网2.0配置工具
        {"com.qding.project.config.tool",              {"5CD58DCAA8C525EF8529C2C8A5DEA45A"},                                     "e6gjISYIExhP7Wg1"},

        //上海闵行寰俊物业
        {"com.zyxing.www",                             {"9D52B18DA65186C5530588F5970F28A9"},                                     "e6gjISYIExhP7Wg1"},
        //北京天岳恒物业
        {"com.silinkeji.dongya",                       {"7DBE9FFFF451ADB687AFC86A00D255D0"},                                     "e6gjISYIExhP7Wg1"},
        //合肥祥源汇物业
        {"com.uama.smartcommunityforxiangyuan",        {"282D03FAC7F6045011C262E48D71A8CC"},                                     "e6gjISYIExhP7Wg1"},
        //祥源工程管理工具
        {"com.xiangyuan.provider.project.manager.app", {"2C6BA358860F13D67E92398E807A55A7"},                                     "e6gjISYIExhP7Wg1"},
};
//是否已授权
extern bool isAuthorized;
//可开门设备识别码
extern const char* AUTH_CODE;
#endif
