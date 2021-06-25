/* crypto/meshsub.h -*- mode:C; c-file-style: "eay" -*- */
/*
 * Written by Monsoon Song for the Temporalgo project.
 */
/* ====================================================================
 * Copyright (c) 2015-2019 Qding Technologies Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by Qding Technologies Inc.
 *    for use in the Qding Development Kit. "
 *
 * 4. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Qding Technologies Inc.
 *    for use in the Qding Development Kit. "
 *
 * THIS SOFTWARE IS PROVIDED BY QDIN TECHNOLOGIES INC. ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ALGORITHM SOFTWARE OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 *
 * @author Monsoon Song <songzhengbin@qding.me>
 *
 */


#ifndef  __mesh_H__
#define  __mesh_H__


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif


/*************************************************************
 *
 * mesh_version,
 *
 * Parameter:
 * Return:
 *
 *************************************************************
 * */

int32_t mesh_version(void);

/*************************************************************
 *
 * mesh_preset,
 *
 * Parameter:
 * Return:
 *
 *************************************************************
 * */

void mesh_preset(int32_t maxdoors, int32_t max_single_count,
                 int32_t max_multi_count, int32_t Isgate, int32_t maxdays);

/*************************************************************
 *
 * mesh_precomp,
 *
 * Parameter:
 *    locality, 省市..小区 , 小区地图位置
 *    doors , 所有有效的单元门 , 以空串结束
 *    date , 日期 , 格式如,"20150101"
 * Return:
 *
 *************************************************************
 * */

void mesh_precomp(uint8_t *locality, uint8_t *doors[],
                  uint8_t *date, uint32_t key);

/* 采用统一的Info,代替原Locality + day + door */

void mesh_precomp2(uint8_t *infos[], uint32_t infolens[],
                   uint32_t key);

/* 现在count数也各单元不同了,也分单次和多次count,这下离散程度是
 * 最高的 */

void mesh_precomp3(uint8_t *infos[], uint32_t infolens[],
                   uint32_t mulcnt[], uint32_t sincnt[], uint32_t key);

/*************************************************************
 *
 * mesh_generate,
 *
 * Parameter:
 *    locality, 省市..小区 , 小区地图位置
 *    door , 单元门
 *    date , 日期 , 格式如,"20150101"
 *    eastimate , 预约天数
 *    time , 次数 , 0,无限次,1,1次,
 *    count, 用于区分同一单元内同一日期的不同访客
 *    key  , 社区密钥
 * Return:
 *
 *************************************************************
 * */

uint32_t mesh_generate(uint8_t *locality, uint8_t *door,
                       uint8_t *date, uint32_t eastimate, uint32_t time,
                       uint32_t count, uint32_t key);

/* 采用统一的Info,代替原Locality + day + door */

uint32_t mesh_generate2(uint8_t *info, uint32_t infolen,
                        uint32_t time, uint32_t count, uint32_t key);

/*************************************************************
 *
 * mesh_verify,
 *
 * Parameter:
 *    locality, 省市..小区 , 小区地图位置
 *    doors, 所有有效的单元门 , 以空串结束
 *    date , 日期 , 格式如,"20150101"
 *    num  , 输入的数字
 *    key  , 社区密钥
 * Return: -1,Failed
 *    bit15-8,Doors Index,
 *    bit7,有效次数,1,1次,0,无限次
 *    bit5-4,截止天数
 *
 *************************************************************
 * */

int32_t mesh_verify(uint8_t *locality, uint8_t *doors[],
                    uint8_t *date, uint32_t num, uint32_t key);

int32_t mesh_verify2(uint8_t *infos[], uint32_t num,
                     uint32_t key);

int32_t mesh_verify3(uint8_t *infos[], uint32_t num,
                     uint32_t mulcnt[], uint32_t sincnt[], uint32_t key);


/* For IC card 发卡加解密运算 */

/*************************************************************
 *
 * iccard_encrypt , iccard_decrypt
 *       For IC card 发卡加解密运算
 *
 * Parameter:
 *    roinfo,rolen 只读扇区,比如序列号,长度
 *    wbinfo,wblen 加密扇区,长度
 *    enc  , 校验值
 * Return: 0,Error
 *
 *************************************************************
 * */
uint16_t iccard_encrypt(uint8_t *roinfo, uint16_t rolen,
                        uint8_t *wbinfo, uint16_t wblen);

uint16_t iccard_decrypt(uint8_t *roinfo, uint16_t rolen,
                        uint8_t *wbinfo, uint16_t wblen, uint16_t enc);

/* For BLE广播数据加解密运算 */

uint16_t ble_encrypt(uint8_t *roinfo, uint16_t rolen,
                     uint8_t *wbinfo, uint16_t wblen);

uint16_t ble_decrypt(uint8_t *roinfo, uint16_t rolen,
                     uint8_t *wbinfo, uint16_t wblen, uint16_t enc);

/* For QD hash 运算 */

uint32_t qdin_hash(uint8_t *msg1, uint16_t len1,
                   uint8_t *msg2, uint16_t len2);


/* 用于后台的任意比特输出的Hash */

/*************************************************************
 *
 *  qall_hash,
 *       比特级别的Hash运算
 *       (注意,不检查参数,需要调用者保证)
 *
 * Parameter:
 *    msg,mbytelen 输入消息,字节长度
 *    out,obitlen  输出buf,bit长度,
 *    count  , 第几次运算(从0开始计数,0,表示第一次)
 * Return: 0,Ok,-1,Error
 *
 *************************************************************
 * */
int32_t qall_hash(uint8_t *msg, uint16_t mbytelen,
                  uint8_t *out, uint16_t obitlen, uint16_t count);


#ifdef __cplusplus
} /* end of the 'extern "C"' block */
#endif

#endif


