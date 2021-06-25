//
//  QDUtils.c
//  QC201_2.0_study
//
//  Created by 胡涛 on 1/29/16.
//  Copyright © 2016 胡涛. All rights reserved.
//

#include "qd_common_utils.h"
#include <stdlib.h>
#include <string.h>

const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char * base64_encode_ht(const unsigned char * bindata, char * base64, int binlength) {
	int i, j;
	unsigned char current;
	printf("len = %d\n", binlength);

	for (i = 0, j = 0; i < binlength; i += 3) {
		current = (bindata[i] >> 2);
		current &= (unsigned char) 0x3F;
		base64[j++] = base64char[(int) current];

		current = ((unsigned char) (bindata[i] << 4)) & ((unsigned char) 0x30);
		if (i + 1 >= binlength) {
			base64[j++] = base64char[(int) current];
			base64[j++] = '=';
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char) (bindata[i + 1] >> 4)) & ((unsigned char) 0x0F);
		base64[j++] = base64char[(int) current];

		current = ((unsigned char) (bindata[i + 1] << 2)) & ((unsigned char) 0x3C);
		if (i + 2 >= binlength) {
			base64[j++] = base64char[(int) current];
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char) (bindata[i + 2] >> 6)) & ((unsigned char) 0x03);
		base64[j++] = base64char[(int) current];

		current = ((unsigned char) bindata[i + 2]) & ((unsigned char) 0x3F);
		base64[j++] = base64char[(int) current];
	}
	base64[j] = '\0';
	return base64;
}

int base64_decode_ht(const char * base64, unsigned char * bindata) {
	int i, j;
	unsigned char k;
	unsigned char temp[4];
	for (i = 0, j = 0; base64[i] != '\0'; i += 4) {
		memset(temp, 0xFF, sizeof(temp));
		for (k = 0; k < 64; k++) {
			if (base64char[k] == base64[i])
				temp[0] = k;
		}
		for (k = 0; k < 64; k++) {
			if (base64char[k] == base64[i + 1])
				temp[1] = k;
		}
		for (k = 0; k < 64; k++) {
			if (base64char[k] == base64[i + 2])
				temp[2] = k;
		}
		for (k = 0; k < 64; k++) {
			if (base64char[k] == base64[i + 3])
				temp[3] = k;
		}

		bindata[j++] = ((unsigned char) (((unsigned char) (temp[0] << 2)) & 0xFC))
				| ((unsigned char) ((unsigned char) (temp[1] >> 4) & 0x03));
		if (base64[i + 2] == '=')
			break;

		bindata[j++] = ((unsigned char) (((unsigned char) (temp[1] << 4)) & 0xF0))
				| ((unsigned char) ((unsigned char) (temp[2] >> 2) & 0x0F));
		if (base64[i + 3] == '=')
			break;

		bindata[j++] = ((unsigned char) (((unsigned char) (temp[2] << 6)) & 0xF0)) | ((unsigned char) (temp[3] & 0x3F));
	}
	return j;
}

//该散列算法是基于贝尔实验室的彼得J温伯格的的研究。在Compilers一书中（原则，技术和工具），建议采用这个算法的散列函数的哈希方法。
uint32_t PJWHash(const char * p_data, int data_len) {
	uint32_t BitsInUnsignedInt = (uint32_t) (4 * 8);
	uint32_t ThreeQuarters = (uint32_t) ((BitsInUnsignedInt * 3) / 4);
	uint32_t OneEighth = (uint32_t) (BitsInUnsignedInt / 8);
	uint32_t HighBits = (uint32_t) (0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
	uint32_t hash = 0;
	uint32_t test = 0;
	for (int i = 0; i < data_len; i++) {
		hash = (hash << OneEighth) + p_data[i];
		if ((test = hash & HighBits) != 0) {
			hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
		}
	}
	return hash;
}

uint16_t Do_Calc_CheckSum(uint8_t *data, uint32_t len)
{
    uint16_t sum = 0;
    uint16_t *pdata = (uint16_t *)data;
    int i = 0;

    for(i = 0; i < len>>1; i++)
    {
        sum ^= pdata[i];
    }

    if (len & 0x01 == 0x01)
    {
        sum ^= (uint16_t)data[len - 1];
    }
    return sum;
}

