//
//  QDUtils.h
//  QC201_2.0_study
//
//  Created by 胡涛 on 1/29/16.
//  Copyright © 2016 胡涛. All rights reserved.
//

#ifndef QDUtils_h
#define QDUtils_h

#include <stdio.h>
#include <stdint.h>

/*
 ** Make sure we can call this stuff from C++.
 */
#ifdef __cplusplus
extern "C" {
#endif

uint32_t PJWHash(const char * p_data, int data_len);
uint16_t Do_Calc_CheckSum(uint8_t *data, uint32_t len);

char * base64_encode_ht( const unsigned char * bindata, char * base64, int binlength );

int base64_decode_ht( const char * base64, unsigned char * bindata );

extern const char * base64char;


#ifdef __cplusplus
} /* end of the 'extern "C"' block */
#endif

#endif /* QDUtils_h */
