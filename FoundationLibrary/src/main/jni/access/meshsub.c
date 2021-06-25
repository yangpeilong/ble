/* crypto/mesh.c -*- mode:C; c-file-style: "eay" -*- */
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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>



static int32_t  MAXDOORS = 50;
static int32_t  SINCOUNT = 50;
static int32_t  MULCOUNT = 50;
static int32_t  ITBEGATE = 0 ;
static int32_t  MAXDAYS  = 2 ; /* */

/* 版本号*/
static int32_t  MESHVERSION = 0x4D534808 ;


static uint8_t  fbox[] =
   {
0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76,
0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0,
0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15,
0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75,
0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84,
0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF,
0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8,
0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2,
0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73,
0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB,
0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79,
0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08,
0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A,
0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E,
0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF,
0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16
   };

static uint8_t  rbox[] =
   {
0x52,0x09,0x6A,0xD5,0x30,0x36,0xA5,0x38,0xBF,0x40,0xA3,0x9E,0x81,0xF3,0xD7,0xFB,
0x7C,0xE3,0x39,0x82,0x9B,0x2F,0xFF,0x87,0x34,0x8E,0x43,0x44,0xC4,0xDE,0xE9,0xCB,
0x54,0x7B,0x94,0x32,0xA6,0xC2,0x23,0x3D,0xEE,0x4C,0x95,0x0B,0x42,0xFA,0xC3,0x4E,
0x08,0x2E,0xA1,0x66,0x28,0xD9,0x24,0xB2,0x76,0x5B,0xA2,0x49,0x6D,0x8B,0xD1,0x25,
0x72,0xF8,0xF6,0x64,0x86,0x68,0x98,0x16,0xD4,0xA4,0x5C,0xCC,0x5D,0x65,0xB6,0x92,
0x6C,0x70,0x48,0x50,0xFD,0xED,0xB9,0xDA,0x5E,0x15,0x46,0x57,0xA7,0x8D,0x9D,0x84,
0x90,0xD8,0xAB,0x00,0x8C,0xBC,0xD3,0x0A,0xF7,0xE4,0x58,0x05,0xB8,0xB3,0x45,0x06,
0xD0,0x2C,0x1E,0x8F,0xCA,0x3F,0x0F,0x02,0xC1,0xAF,0xBD,0x03,0x01,0x13,0x8A,0x6B,
0x3A,0x91,0x11,0x41,0x4F,0x67,0xDC,0xEA,0x97,0xF2,0xCF,0xCE,0xF0,0xB4,0xE6,0x73,
0x96,0xAC,0x74,0x22,0xE7,0xAD,0x35,0x85,0xE2,0xF9,0x37,0xE8,0x1C,0x75,0xDF,0x6E,
0x47,0xF1,0x1A,0x71,0x1D,0x29,0xC5,0x89,0x6F,0xB7,0x62,0x0E,0xAA,0x18,0xBE,0x1B,
0xFC,0x56,0x3E,0x4B,0xC6,0xD2,0x79,0x20,0x9A,0xDB,0xC0,0xFE,0x78,0xCD,0x5A,0xF4,
0x1F,0xDD,0xA8,0x33,0x88,0x07,0xC7,0x31,0xB1,0x12,0x10,0x59,0x27,0x80,0xEC,0x5F,
0x60,0x51,0x7F,0xA9,0x19,0xB5,0x4A,0x0D,0x2D,0xE5,0x7A,0x9F,0x93,0xC9,0x9C,0xEF,
0xA0,0xE0,0x3B,0x4D,0xAE,0x2A,0xF5,0xB0,0xC8,0xEB,0xBB,0x3C,0x83,0x53,0x99,0x61,
0x17,0x2B,0x04,0x7E,0xBA,0x77,0xD6,0x26,0xE1,0x69,0x14,0x63,0x55,0x21,0x0C,0x7D
   };


static uint8_t *ascii = "0123456789abcdef";

static void trans16 (uint32_t c , uint8_t *b)
   {

   b[0] = ascii [ (c >> 12) & 15 ];
   b[1] = ascii [ (c >> 8 ) & 15 ];
   b[2] = ascii [ (c >> 4 ) & 15 ];
   b[3] = ascii [ (c >> 0 ) & 15 ];

   }

static void trans32 (uint32_t c , uint8_t *b)
   {
   trans16 ((c>>16)&0xFFFF , b+0);
   trans16 ((c>>0 )&0xFFFF , b+4);
   }


static uint32_t chans32 (uint8_t *b)
   {


   uint32_t  dat = 0 ;
   int  i , j ;


   for (i = 0 ; i < 8 ; ++ i)
      {

      dat <<= 4 ;

      for (j = 0 ; j < 16 ; ++ j)
         {
         if (b[i] == ascii[j])
            break ;
         }
      dat |= j & 15 ;
      }

   return  dat ;

   }


#include "galois.h"


struct ctx
   {
   uint32_t state [5];
   uint32_t count [2];
   uint8_t  buffer[64];
   };


#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))
#define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
    |(rol(block->l[i],8)&0x00FF00FF))
#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))


#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30)
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30)
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30)
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30)
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30)


#define  MDIM                          (128*256*4*4*2*4)


//static struct  ctx  preCTX [4*4096] ;
static uint32_t   preNUM [MDIM];

#define  MESHLOWER(a,b,c,d)     ({ (a) *= (b) ; \
      (a) += (c) ; ((a) << (d)) | ((a) >> (64 - (d))) ; })


static uint64_t meshblock  (uint64_t hsh , uint8_t *msg)
   {return  MESHLOWER(hsh , msg[0] , msg[1] , hsh & 15) ;}



/*************************************************************
 *
 * previousDays,
 *
 * Parameter:
 * Return:
 *
 *************************************************************
 * */

static int daybeforemon [13] = 
{30 , 31 , 28 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30, 31};
static int daybeforeleap[13] = 
{30 , 31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30, 31};


static void previousDays (uint8_t date[4][9])
   {


   int  i , day , mon , year ;


   for (i = 0 ; i < 3 ; ++ i)
      {

      year = (date[i][2] - '0') * 10 ;
      year+= (date[i][3] - '0') ;
      mon  = (date[i][4] - '0') * 10 ;
      mon += (date[i][5] - '0') ;
      day  = (date[i][6] - '0') * 10 ;
      day += (date[i][7] - '0') ;



      if (day == 1)
         {memcpy(date[i+1] , date[i] , 4 );

      /* */
          if (mon != 1)
             {mon -- ;}
          else
             {mon  = 12 ;

              year -- ;
              date[i+1][2] = year/10 + '0';
              date[i+1][3] = year%10 + '0';
             }

          date[i+1][4] = mon/10 + '0';
          date[i+1][5] = mon%10 + '0';


          if ((year&3) == 0)
             {day = daybeforeleap[mon] ;}
          else
             {day = daybeforemon [mon] ;}

          date[i+1][6] = day/10 + '0';
          date[i+1][7] = day%10 + '0';

         }
      else
         {memcpy(date[i+1] , date[i] , 6 );

          day -- ;
          date[i+1][6] = day/10 + '0';
          date[i+1][7] = day%10 + '0';
         }

      }
   }


static void secondDays (uint8_t date[4][9])
   {


   int  i , day , maxday , mon , year ;


   for (i = 0 ; i < 3 ; ++ i)
      {

      year = (date[i][2] - '0') * 10 ;
      year+= (date[i][3] - '0') ;
      mon  = (date[i][4] - '0') * 10 ;
      mon += (date[i][5] - '0') ;
      day  = (date[i][6] - '0') * 10 ;
      day += (date[i][7] - '0') ;


      if ((year&3) == 0)
         {maxday = daybeforeleap[mon] ;}
      else
         {maxday = daybeforemon [mon] ;}


      if (day == maxday)
         {memcpy(date[i+1] , date[i] , 4 );

      /* */
          if (mon != 12)
             {mon ++ ; }
          else
             {mon  = 1 ;

              year ++  ;
              date[i+1][2] = year/10 + '0';
              date[i+1][3] = year%10 + '0';
             }

          date[i+1][4] = mon/10 + '0';
          date[i+1][5] = mon%10 + '0';
          date[i+1][6] = '0';
          date[i+1][7] = '1';

         }
      else
         {memcpy(date[i+1] , date[i] , 6 );

          day  ++  ;
          date[i+1][6] = day/10 + '0' ;
          date[i+1][7] = day%10 + '0' ;
         }

      }
   }


/*************************************************************
 *
 * meshupdate,
 *
 * Parameter:
 * Return:
 *
 *************************************************************
 * */

static void transform(uint32_t state[5] , uint8_t buffer[64])
   {

   uint32_t  a , b , c , d , e ;

   typedef union {
      uint8_t  c[64];
      uint32_t l[16];
   } CHAR64LONG16;

   CHAR64LONG16* block = (CHAR64LONG16*)buffer;

/* Copy context->state[] to working vars */
   a = state[0];
   b = state[1];
   c = state[2];
   d = state[3];
   e = state[4];

/* 4 rounds of 20 operations each. Loop unrolled. */
   R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
   R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
   R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
   R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
   R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
   R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
   R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
   R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
   R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
   R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
   R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
   R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
   R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
   R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
   R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
   R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
   R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
   R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
   R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
   R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);

/* Add the working vars back into context.state[] */
   state[0] += a;
   state[1] += b;
   state[2] += c;
   state[3] += d;
   state[4] += e;

   a = b = c = d = e = 0;

   }


static void meshinit   (struct ctx *p)
   {

   memset(p , 0 , sizeof(*p));

   p->state[0] = 0x67452301;
   p->state[1] = 0xEFCDAB89;
   p->state[2] = 0x98BADCFE;
   p->state[3] = 0x10325476;
   p->state[4] = 0xC3D2E1F0;

   }

static void mesh2init  (struct ctx *p)
   {

   memset(p ,0 , sizeof(*p));

   p->state[0] = 0x7380166F ;
   p->state[1] = 0x4914B2B9 ;
   p->state[2] = 0x172442D7 ;
   p->state[3] = 0xDA8A0600 ;
   p->state[4] = 0xA96F30BC ;
   }

static void meshupdate (struct ctx *p , uint8_t *msg , uint32_t len)
   {

 
   uint32_t  i , j ;


   j = p->count[0];
   if((p->count[0] += len << 3) < j)
      {p->count[1] ++ ;}

   p->count[1] += (len>>29);

   j = (j >> 3) & 63;
   if ((j + len)> 63)
      {
      memcpy(&p->buffer[j] , msg , (i = 64-j));
      transform(p->state, p->buffer);
      for ( ; i + 63 < len; i += 64)
         {memcpy(p->buffer , &msg[i] , 64);
          transform(p->state, p->buffer);
         }
      j  = 0;
      }
   else
      {i = 0;}

   memcpy(&p->buffer[j] , &msg[i] , len - i);
   }


static void meshfinal  (struct ctx *p)
   {

   uint32_t  i ;
   uint8_t   c ;
   uint8_t  finalcount[8];

   for (i = 0 ; i < 8 ; ++ i)
      {
      finalcount[i] = (uint8_t)((p->count[(i >= 4 ? 0 : 1)]
               >> ((3-(i & 3)) * 8) ) & 255);
      }

   c = 0200;
   meshupdate(p , &c , 1);

   c = 0;
   while ((p->count[0] & 504) != 448)
      {meshupdate(p , &c , 1);}

   meshupdate(p , finalcount , 8);

   }



/*************************************************************
 *
 * meshenc, dec,
 *
 * Parameter:
 * Return:
 *
 *************************************************************
 * */

static uint32_t meshenc (uint32_t hh , uint32_t key)
   {


   uint32_t  cc ;


   cc = (uint32_t)( hh & 0xFFFF) ;
   hh&= 0x70000 ;

   cc = (uint32_t) fbox [(cc >> 8) & 255] << 8 |
        (uint32_t) fbox [(cc >> 0) & 255] << 0 ;


   return  ((hh | cc)^ key) & 0x7FFFF ;

   }


static uint32_t meshdec (uint32_t hh , uint32_t key)
   {


   uint32_t  cc ;


   hh = (hh ^ key) & 0x7FFFF ;

   cc = (uint32_t)( hh & 0xFFFF) ;
   hh&= 0x70000 ;

   cc = (uint32_t) rbox [(cc >> 8) & 255] << 8 |
        (uint32_t) rbox [(cc >> 0) & 255] << 0 ;


   return  hh | cc ;

   }



/*************************************************************
 *
 * mesh_version,
 *
 * Parameter:
 * Return:
 *
 *************************************************************
 * */

int32_t mesh_version (void)
   {return  MESHVERSION ;}


/*************************************************************
 *
 * mesh_preset,
 *
 * Parameter:
 * Return:
 *
 *************************************************************
 * */

void mesh_preset (int32_t maxdoors , int32_t max_single_count ,
      int32_t max_multi_count , int32_t Isgate , int32_t maxdays)
   {

   MAXDOORS = maxdoors ;
   SINCOUNT = max_single_count ;
   MULCOUNT = max_multi_count ;

   MAXDAYS  = maxdays > 3 ? 3 : maxdays;
   ITBEGATE =  Isgate  ;

   }


uint32_t mesh_generate (uint8_t *locality , uint8_t *door ,
      uint8_t *date  , uint32_t eastimate , uint32_t time ,
      uint32_t count , uint32_t key);

uint32_t mesh_generate2(uint8_t *info , uint32_t infolen ,
      uint32_t time , uint32_t count , uint32_t key);


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

void mesh_precomp (uint8_t *locality , uint8_t *doors[] ,
      uint8_t *date , uint32_t key)
   { /*由主daemon固定时刻调用,预计算*/


   uint32_t  da , i , count , *pn = preNUM ;


   for (da = 0 ; da < MAXDAYS ; ++ da)

      {

/* 多次有效 */
      for ( i = 0 ; doors[i][0] != 0 && i < MAXDOORS ; ++ i )
         for (count = 0 ; count < MULCOUNT ; ++ count)

            {

            *pn ++ = mesh_generate (locality , doors[i],
                  date , da , 0 /*tim*/ , count , key) ;
            }

/* 单次有效 */
      for ( i = 0 ; doors[i][0] != 0 && i < MAXDOORS ; ++ i )
         for (count = 0 ; count < SINCOUNT ; ++ count)

            {

            *pn ++ = mesh_generate (locality , doors[i],
                  date , da , 1 /*tim*/ , count , key) ;
            }

      }
   }

/* 采用统一的Info,代替原Locality + day + door */

void mesh_precomp2 (uint8_t *infos[] , uint32_t infolens[] ,
      uint32_t key)
   { /*由主daemon固定时刻调用,预计算*/


   uint32_t  i , count , *pn = preNUM ;


/* 多次有效 */
   for ( i = 0 ; infos[i][0] != 0 && i < MAXDOORS ; ++ i )
      for (count = 0 ; count < MULCOUNT ; ++ count)

         {

         *pn ++ = mesh_generate2 (infos[i] , infolens[i],
               0 /*tim*/ , count , key) ;
         }

/* 单次有效 */
   for ( i = 0 ; infos[i][0] != 0 && i < MAXDOORS ; ++ i )
      for (count = 0 ; count < SINCOUNT ; ++ count)

         {

         *pn ++ = mesh_generate2 (infos[i] , infolens[i],
               1 /*tim*/ , count , key) ;
         }

   }


/* 现在count数也各单元不同了,也分单次和多次count,这下离散程度是
 * 最高的 */

void mesh_precomp3 (uint8_t *infos[] , uint32_t infolens[] ,
      uint32_t mulcnt[] , uint32_t sincnt[] , uint32_t key)
   { /*由主daemon固定时刻调用,预计算*/


   uint32_t  i , count , *pn = preNUM ;


/* 多次有效 */
   for ( i = 0 ; infos[i][0] != 0 ; ++ i )
      for (count = 0 ; count < mulcnt[i] ; ++ count)

         {

         *pn ++ = mesh_generate2 (infos[i] , infolens[i],
               0 /*tim*/ , count , key) ;
         }

/* 单次有效 */
   for ( i = 0 ; infos[i][0] != 0 ; ++ i )
      for (count = 0 ; count < sincnt[i] ; ++ count)

         {

         *pn ++ = mesh_generate2 (infos[i] , infolens[i],
               1 /*tim*/ , count , key) ;
         }

   }


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
uint32_t mesh_generate (uint8_t *locality , uint8_t *door ,
      uint8_t *date  , uint32_t eastimate , uint32_t time ,
      uint32_t count , uint32_t key)
   {


   struct  ctx  cx ;


   uint32_t  h  ;
   uint8_t  ch  [5], tn [3];
   uint8_t  days[4][9];


   memset (days    , 0  ,sizeof(days));
   memcpy (days[0] ,date,     8      );

   secondDays (days);


   if (count > SINCOUNT - 1 )
      {count = MULCOUNT - 1 ; time = 0 ;}

   if (eastimate > MAXDAYS - 1 )
      {eastimate = MAXDAYS - 1;}


   tn[0] = time == 1 ? 15 : 14 ;
   tn[1] =   0  ;
   ch[4] =   0  ;


   trans16 (count , ch);


   meshinit   (&cx);
   meshupdate (&cx , locality , strlen(locality));
   meshupdate (&cx , days[eastimate] , sizeof(days[eastimate]));
   meshupdate (&cx , door , strlen(door));
   meshupdate (&cx ,  tn , 1);
   meshupdate (&cx ,  ch , 4);
   meshfinal  (&cx);


   h = cx.state[0] & 0xFFFFF ;

   if (h > 0xF423F )
      {h-= 0xF423F;}

   return h ;

   }


/* 采用统一的Info,代替原Locality + day + door */

uint32_t mesh_generate2 (uint8_t *info , uint32_t infolen ,
      uint32_t time , uint32_t count , uint32_t key)
   {


   struct  ctx  cx ;


   uint32_t  h  ;
   uint8_t  ch  [5], tn [3];


   tn[0] = time == 1 ? 15 : 14 ;
   tn[1] =   0  ;
   ch[4] =   0  ;


   trans16 (count , ch);


   meshinit   (&cx);
   meshupdate (&cx , info,infolen);
   meshupdate (&cx ,  tn , 1);
   meshupdate (&cx ,  ch , 4);
   meshfinal  (&cx);


   h = cx.state[0] & 0xFFFFF ;

   if (h > 0xF423F )
      {h-= 0xF423F;}

   return h ;

   }


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

int32_t mesh_verify (uint8_t *locality , uint8_t *doors[] ,
      uint8_t *date , uint32_t num , uint32_t key)
   {


   int32_t   rtn  = -1 ;
   uint32_t  da , i , count , *pn = preNUM ;


   for (da = 0 ; rtn == -1 && da < MAXDAYS ; ++ da)

      {

/* 多次有效 */
      for ( i = 0 ; doors[i][0] != 0 && rtn == -1 && i < MAXDOORS ; ++ i )
         for (count = 0 ; rtn == -1 && count < MULCOUNT ; ++ count)

            {

            if (*pn ++ == num)
               {rtn = i << 8 | 0 << 7 | da << 4 ; break ;}
            }

/* 单次有效 */
      for ( i = 0 ; doors[i][0] != 0 && rtn == -1 && i < MAXDOORS ; ++ i )
         for (count = 0 ; rtn == -1 && count < SINCOUNT ; ++ count)

            {

            if (*pn ++ == num)
               {rtn = i << 8 | 1 << 7 | da << 4 ; break ;}
            }

      }


   return  rtn ;

   }

/* 采用统一的Info,代替原Locality + day + door */

int32_t mesh_verify2 (uint8_t *infos[] , uint32_t num , uint32_t key)
   {


   int32_t   rtn  = -1 ;
   uint32_t   i , count , *pn = preNUM ;


/* 多次有效 */
   for ( i = 0 ; infos[i][0] != 0 && rtn == -1 && i < MAXDOORS ; ++ i )
      for (count = 0 ; rtn == -1 && count < MULCOUNT ; ++ count)

         {

         if (*pn ++ == num)
            {rtn = i << 8 | 0 << 7 ; break ;}
         }

/* 单次有效 */
   for ( i = 0 ; infos[i][0] != 0 && rtn == -1 && i < MAXDOORS ; ++ i )
      for (count = 0 ; rtn == -1 && count < SINCOUNT ; ++ count)

         {

         if (*pn ++ == num)
            {rtn = i << 8 | 1 << 7 ; break ;}
         }


   return  rtn ;

   }


/* 现在count数也各单元不同了,也分单次和多次count,这下离散程度是
 * 最高的 */

int32_t mesh_verify3 (uint8_t *infos[] , uint32_t num ,
      uint32_t mulcnt[] , uint32_t sincnt[] , uint32_t key)
   {


   int32_t   rtn  = -1 ;
   uint32_t   i , count , *pn = preNUM ;


/* 多次有效 */
   for ( i = 0 ; infos[i][0] != 0 && rtn == -1 ; ++ i )
      for (count = 0 ; rtn == -1 && count < mulcnt[i] ; ++ count)

         {

         if (*pn ++ == num)
            {rtn = i << 8 | 0 << 7 ; break ;}
         }

/* 单次有效 */
   for ( i = 0 ; infos[i][0] != 0 && rtn == -1 ; ++ i )
      for (count = 0 ; rtn == -1 && count < sincnt[i] ; ++ count)

         {

         if (*pn ++ == num)
            {rtn = i << 8 | 1 << 7 ; break ;}
         }


   return  rtn ;

   }



/* For IC card 发卡加解密运算 */
/* 手机上下行数据的加解密运算 */

uint16_t iccard_encrypt (uint8_t *roinfo , uint16_t rolen ,
      uint8_t *wbinfo , uint16_t wblen)
   { /* Return , 0, means err */


   struct  ctx  cx ;
 
   uint32_t  i,r , R , t [16];
   uint16_t aclen= wblen ;
   uint8_t  *pi  = wbinfo;

 
   mesh2init  (&cx);
   meshupdate (&cx , roinfo , rolen);
   meshupdate (&cx , wbinfo , aclen);
   meshfinal  (&cx);


   r = cx.state[0] - 0x115 ;


_loop_icenc:

   r = MODB (r + 0x115) ;
   if (r == 0)
      {goto _loop_icenc ;}

   if (MLT (&R , 2489477186 , r) != 0 )
      {goto _loop_icenc ;}




   while (wblen >= 64)
      {
      
      memcpy ((void*)t , pi , 64 );

      for ( i = 0 ; i < 16 ; ++ i)
         {t[i] ^= (R += r) ;}

      memcpy (pi , (void*)t , 64 );

      pi   += 64 ;
      wblen-= 64 ;
      }


   if (wblen > 0 )
      {
      
      memcpy ((void*)t , pi , wblen);

      for ( i = 0 ; i < 16 ; ++ i)
         {t[i] ^= (R += r) ;}

      memcpy (pi , (void*)t , wblen);

      }


   return  (uint16_t) r ;

   }


uint16_t iccard_decrypt (uint8_t *roinfo , uint16_t rolen ,
      uint8_t *wbinfo , uint16_t wblen, uint16_t enc)
   { /* Return , 0, means err , 1,OK */


   struct  ctx  cx ;
 
   uint32_t  i,r , R , t [16];
   uint16_t aclen= wblen ;
   uint8_t  *pi  = wbinfo;


   if ( enc == 0 )
      {return  0;}

   MLT (&R , 2489477186 , enc);


   while (aclen >= 64)
      {
      
      memcpy ((void*)t , pi , 64 );

      for ( i = 0 ; i < 16 ; ++ i)
         {t[i] ^= (R += enc) ;}

      memcpy (pi , (void*)t , 64 );

      pi   += 64 ;
      aclen-= 64 ;
      }


   if (aclen > 0 )
      {
      
      memcpy ((void*)t , pi , aclen);

      for ( i = 0 ; i < 16 ; ++ i)
         {t[i] ^= (R += enc) ;}

      memcpy (pi , (void*)t , aclen);
      }
 

   mesh2init  (&cx);
   meshupdate (&cx , roinfo , rolen);
   meshupdate (&cx , wbinfo , wblen);
   meshfinal  (&cx);


   r = cx.state[0] - 0x115 ;

_loop_redec:

   r = MODB (r + 0x115) ;
   if (r == 0)
      {goto _loop_redec ;}

   if (MLT (&R , 2489477186 , r) != 0 )
      {goto _loop_redec ;}


   return  r == enc ? 1 : 0 ;

   }


/* For BLE广播数据加解密运算 */

uint16_t ble_encrypt (uint8_t *roinfo , uint16_t rolen ,
      uint8_t *wbinfo , uint16_t wblen)
   { /* Return , 0, means err */

      return iccard_encrypt (roinfo , rolen , wbinfo , wblen);
   
   }

uint16_t ble_decrypt (uint8_t *roinfo , uint16_t rolen ,
      uint8_t *wbinfo , uint16_t wblen, uint16_t enc)
   { /* Return , 0, means err , 1,OK */

      return iccard_decrypt (roinfo , rolen , wbinfo , wblen , enc);
   
   }


/* For QD hash 运算 */

uint32_t qdin_hash (uint8_t *msg1 , uint16_t len1 ,
      uint8_t *msg2 , uint16_t len2)
   {


   struct  ctx  cx ;
 

   mesh2init  (&cx);
   meshupdate (&cx , msg1 , len1);
   meshupdate (&cx , msg2 , len2);
   meshfinal  (&cx);

   return  cx.state[0] ;
   
   }


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
int32_t qall_hash (uint8_t *msg , uint16_t mbytelen ,
      uint8_t *out , uint16_t obitlen , uint16_t count)
   {


   struct  ctx  cx ;
 
   uint8_t   msk = 0xFF ;
   uint16_t  bytnum , bitnum , i = 0 ;


   bytnum = (obitlen + 7) >> 3 ;
   bitnum = (obitlen & 7) ;


   if (bitnum != 0 )
      {msk <<= 8 - bitnum ; msk >>= 8 - bitnum ;}


   mesh2init  (&cx);


   for ( ; ; )
      {
      
      for ( ; i < count ; ++ i)
         {meshupdate (&cx , msg , mbytelen);}

      meshupdate (&cx , msg , mbytelen);
      meshfinal  (&cx);

      if (bytnum > 20 )
         {bytnum-= 20 ;
          memcpy (out , cx.state ,  20 );
          out += 20 ;
         }
      else
         {memcpy (out , cx.state , bytnum);
          break ;
         }
      }


   out [bytnum-1] &= msk ;

   return  0 ;
   
   }


/* 用于生成QR-Code二维码的加密数据 */

/*************************************************************
 *
 *  qdin_qrcode_encrypt,
 *       (注意,不检查参数,需要调用者保证)
 *
 * Parameter:
 *    msg,msglen 输入消息,字节长度,必须是小写16进制数据
 *    out,输出buf,由调用者保证其足够,(msglen+4)
 *
 *************************************************************
 * */
void qdin_qrcode_encrypt (uint8_t *key , uint16_t keylen ,
      uint8_t *msg , uint16_t msglen , uint8_t *out)
   {


   struct  ctx  cx ;
 
   uint32_t  i = 0 , j , r , R ;
   uint8_t  cb [8] ;

 
   mesh2init  (&cx);
   meshupdate (&cx , key , keylen);
   meshupdate (&cx , msg , msglen);
   meshfinal  (&cx);


   r = cx.state[0] - 0x115 ;


_loop_qrenc:

   r = MODB (r + 0x115) ;
   if (r == 0)
      {goto _loop_qrenc ;}

   if (MLT (&R , 2489477186 , r) != 0 )
      {goto _loop_qrenc ;}


   trans16( r , cb);
   memcpy (out, cb , 4 ); out += 4 ;


   while (msglen - i >= 8)
      {

      memcpy (cb , &msg[i] , 8 );
      i += 8 ;
      j  = chans32(cb) ;
      j ^= (R += r) ;
      trans32( j , cb);
      memcpy (out, cb, 8 ); out += 8 ;
      }

   if (msglen - i)
      {

      memset (cb , '0' , 8 );
      memcpy (cb , &msg[i] , msglen-i);
      j  = chans32(cb) ;
      j ^= (R += r) ;
      trans32( j , cb);
      memcpy (out, cb, msglen-i);
      }

   }


uint32_t qdin_qrcode_decrypt (uint8_t *key , uint16_t keylen ,
      uint8_t *cry , uint16_t crylen , uint8_t *out)
   { /* Return , 0, means err , 1,OK */


   struct  ctx  cx ;
 
   uint32_t  i = 4 , j , r , R ;
   uint8_t  cb [8] ;
   uint8_t  *po= out ;


   if ( crylen <= 4 )
      {return  0;}


   memset (cb+0, '0' , 8 );
   memcpy (cb+4, cry , 4 );

   r = chans32 (cb);
   if ( r == 0 )
      {return  0;}

   MLT (&R , 2489477186 , r );


   while (crylen - i >= 8)
      {

      memcpy (cb , &cry[i] , 8 );
      i += 8 ;
      j  = chans32(cb) ;
      j ^= (R += r) ;
      trans32( j , cb);
      memcpy (out, cb, 8 ); out += 8 ;
      }

   if (crylen - i)
      {

      memset (cb , '0' , 8 );
      memcpy (cb , &cry[i] , crylen-i);
      j  = chans32(cb) ;
      j ^= (R += r) ;
      trans32( j , cb);
      memcpy (out, cb, crylen-i);
      }


   mesh2init  (&cx);
   meshupdate (&cx , key , keylen);
   meshupdate (&cx , po  , crylen-4);
   meshfinal  (&cx);


   j = cx.state[0] - 0x115 ;

_loop_qrdec:

   j = MODB (j + 0x115) ;
   if (j == 0)
      {goto _loop_qrdec ;}

   if (MLT (&R , 2489477186 , j) != 0 )
      {goto _loop_qrdec ;}


   return  r == j ? 1 : 0 ;

   }


#undef   MESHVERSION
#undef   MESHLOWER


