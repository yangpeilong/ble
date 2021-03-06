
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>



#define  Rw                               (PR.w)
#define  Rx                               (PR.h[0])
#define  Ry                               (PR.h[1])
#define  Aw                               (PA.w)
#define  Ax                               (PA.h[0])
#define  Ay                               (PA.h[1])
#define  Bw                               (PB.w)
#define  Bx                               (PB.h[0])
#define  By                               (PB.h[1])

#define  NPAR                             8
#define  MAXDEG                           16



#define  MOD(a)      ({ (a) >= 0xFDF3 ? (a)-0xFDF3 : (a) ; })
#define  ORD(a)      ({ (a) >= 0xFCB7 ? (a)-0xFCB7 : (a) ; })

#define  MOD8(a)     ({ \
      union { uint32_t w; uint8_t h[4]; } PR ;   \
      Rw = (a) ; \
      if ( Ry != 0 ) Rw = (uint32_t) Ry + Rx ;   \
      if ( Ry != 0 ) Rw = (uint32_t) Ry + Rx ;   \
      Rx == 255 ? 0 : (uint32_t) Rx ; })



/* Galois Algebra Number Filed */

static uint32_t GXE [] =

   {

0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1D, 0x3A,
0x74, 0xE8, 0xCD, 0x87, 0x13, 0x26, 0x4C, 0x98, 0x2D, 0x5A,
0xB4, 0x75, 0xEA, 0xC9, 0x8F, 0x03, 0x06, 0x0C, 0x18, 0x30,
0x60, 0xC0, 0x9D, 0x27, 0x4E, 0x9C, 0x25, 0x4A, 0x94, 0x35,
0x6A, 0xD4, 0xB5, 0x77, 0xEE, 0xC1, 0x9F, 0x23, 0x46, 0x8C,
0x05, 0x0A, 0x14, 0x28, 0x50, 0xA0, 0x5D, 0xBA, 0x69, 0xD2,
0xB9, 0x6F, 0xDE, 0xA1, 0x5F, 0xBE, 0x61, 0xC2, 0x99, 0x2F,
0x5E, 0xBC, 0x65, 0xCA, 0x89, 0x0F, 0x1E, 0x3C, 0x78, 0xF0,
0xFD, 0xE7, 0xD3, 0xBB, 0x6B, 0xD6, 0xB1, 0x7F, 0xFE, 0xE1,
0xDF, 0xA3, 0x5B, 0xB6, 0x71, 0xE2, 0xD9, 0xAF, 0x43, 0x86,
0x11, 0x22, 0x44, 0x88, 0x0D, 0x1A, 0x34, 0x68, 0xD0, 0xBD,
0x67, 0xCE, 0x81, 0x1F, 0x3E, 0x7C, 0xF8, 0xED, 0xC7, 0x93,
0x3B, 0x76, 0xEC, 0xC5, 0x97, 0x33, 0x66, 0xCC, 0x85, 0x17,
0x2E, 0x5C, 0xB8, 0x6D, 0xDA, 0xA9, 0x4F, 0x9E, 0x21, 0x42,
0x84, 0x15, 0x2A, 0x54, 0xA8, 0x4D, 0x9A, 0x29, 0x52, 0xA4,
0x55, 0xAA, 0x49, 0x92, 0x39, 0x72, 0xE4, 0xD5, 0xB7, 0x73,
0xE6, 0xD1, 0xBF, 0x63, 0xC6, 0x91, 0x3F, 0x7E, 0xFC, 0xE5,
0xD7, 0xB3, 0x7B, 0xF6, 0xF1, 0xFF, 0xE3, 0xDB, 0xAB, 0x4B,
0x96, 0x31, 0x62, 0xC4, 0x95, 0x37, 0x6E, 0xDC, 0xA5, 0x57,
0xAE, 0x41, 0x82, 0x19, 0x32, 0x64, 0xC8, 0x8D, 0x07, 0x0E,
0x1C, 0x38, 0x70, 0xE0, 0xDD, 0xA7, 0x53, 0xA6, 0x51, 0xA2,
0x59, 0xB2, 0x79, 0xF2, 0xF9, 0xEF, 0xC3, 0x9B, 0x2B, 0x56,
0xAC, 0x45, 0x8A, 0x09, 0x12, 0x24, 0x48, 0x90, 0x3D, 0x7A,
0xF4, 0xF5, 0xF7, 0xF3, 0xFB, 0xEB, 0xCB, 0x8B, 0x0B, 0x16,
0x2C, 0x58, 0xB0, 0x7D, 0xFA, 0xE9, 0xCF, 0x83, 0x1B, 0x36,
0x6C, 0xD8, 0xAD, 0x47, 0x8E, 0x01, 0x02, 0x04, 0x08, 0x10,
0x20, 0x40, 0x80, 0x1D, 0x3A, 0x74, 0xE8, 0xCD, 0x87, 0x13,
0x26, 0x4C, 0x98, 0x2D, 0x5A, 0xB4, 0x75, 0xEA, 0xC9, 0x8F,
0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x9D, 0x27, 0x4E,
0x9C, 0x25, 0x4A, 0x94, 0x35, 0x6A, 0xD4, 0xB5, 0x77, 0xEE,
0xC1, 0x9F, 0x23, 0x46, 0x8C, 0x05, 0x0A, 0x14, 0x28, 0x50,
0xA0, 0x5D, 0xBA, 0x69, 0xD2, 0xB9, 0x6F, 0xDE, 0xA1, 0x5F,
0xBE, 0x61, 0xC2, 0x99, 0x2F, 0x5E, 0xBC, 0x65, 0xCA, 0x89,
0x0F, 0x1E, 0x3C, 0x78, 0xF0, 0xFD, 0xE7, 0xD3, 0xBB, 0x6B,
0xD6, 0xB1, 0x7F, 0xFE, 0xE1, 0xDF, 0xA3, 0x5B, 0xB6, 0x71,
0xE2, 0xD9, 0xAF, 0x43, 0x86, 0x11, 0x22, 0x44, 0x88, 0x0D,
0x1A, 0x34, 0x68, 0xD0, 0xBD, 0x67, 0xCE, 0x81, 0x1F, 0x3E,
0x7C, 0xF8, 0xED, 0xC7, 0x93, 0x3B, 0x76, 0xEC, 0xC5, 0x97,
0x33, 0x66, 0xCC, 0x85, 0x17, 0x2E, 0x5C, 0xB8, 0x6D, 0xDA,
0xA9, 0x4F, 0x9E, 0x21, 0x42, 0x84, 0x15, 0x2A, 0x54, 0xA8,
0x4D, 0x9A, 0x29, 0x52, 0xA4, 0x55, 0xAA, 0x49, 0x92, 0x39,
0x72, 0xE4, 0xD5, 0xB7, 0x73, 0xE6, 0xD1, 0xBF, 0x63, 0xC6,
0x91, 0x3F, 0x7E, 0xFC, 0xE5, 0xD7, 0xB3, 0x7B, 0xF6, 0xF1,
0xFF, 0xE3, 0xDB, 0xAB, 0x4B, 0x96, 0x31, 0x62, 0xC4, 0x95,
0x37, 0x6E, 0xDC, 0xA5, 0x57, 0xAE, 0x41, 0x82, 0x19, 0x32,
0x64, 0xC8, 0x8D, 0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0, 0xDD,
0xA7, 0x53, 0xA6, 0x51, 0xA2, 0x59, 0xB2, 0x79, 0xF2, 0xF9,
0xEF, 0xC3, 0x9B, 0x2B, 0x56, 0xAC, 0x45, 0x8A, 0x09, 0x12,
0x24, 0x48, 0x90, 0x3D, 0x7A, 0xF4, 0xF5, 0xF7, 0xF3, 0xFB,
0xEB, 0xCB, 0x8B, 0x0B, 0x16, 0x2C, 0x58, 0xB0, 0x7D, 0xFA,
0xE9, 0xCF, 0x83, 0x1B, 0x36, 0x6C, 0xD8, 0xAD, 0x47, 0x8E,
0x01, 0x00

   };


/* EXP Index array */

static uint32_t GOL [] =

   {

0x00, 0x00, 0x01, 0x19, 0x02, 0x32, 0x1A, 0xC6, 0x03, 0xDF,
0x33, 0xEE, 0x1B, 0x68, 0xC7, 0x4B, 0x04, 0x64, 0xE0, 0x0E,
0x34, 0x8D, 0xEF, 0x81, 0x1C, 0xC1, 0x69, 0xF8, 0xC8, 0x08,
0x4C, 0x71, 0x05, 0x8A, 0x65, 0x2F, 0xE1, 0x24, 0x0F, 0x21,
0x35, 0x93, 0x8E, 0xDA, 0xF0, 0x12, 0x82, 0x45, 0x1D, 0xB5,
0xC2, 0x7D, 0x6A, 0x27, 0xF9, 0xB9, 0xC9, 0x9A, 0x09, 0x78,
0x4D, 0xE4, 0x72, 0xA6, 0x06, 0xBF, 0x8B, 0x62, 0x66, 0xDD,
0x30, 0xFD, 0xE2, 0x98, 0x25, 0xB3, 0x10, 0x91, 0x22, 0x88, 
0x36, 0xD0, 0x94, 0xCE, 0x8F, 0x96, 0xDB, 0xBD, 0xF1, 0xD2,
0x13, 0x5C, 0x83, 0x38, 0x46, 0x40, 0x1E, 0x42, 0xB6, 0xA3,
0xC3, 0x48, 0x7E, 0x6E, 0x6B, 0x3A, 0x28, 0x54, 0xFA, 0x85,
0xBA, 0x3D, 0xCA, 0x5E, 0x9B, 0x9F, 0x0A, 0x15, 0x79, 0x2B,
0x4E, 0xD4, 0xE5, 0xAC, 0x73, 0xF3, 0xA7, 0x57, 0x07, 0x70,
0xC0, 0xF7, 0x8C, 0x80, 0x63, 0x0D, 0x67, 0x4A, 0xDE, 0xED,
0x31, 0xC5, 0xFE, 0x18, 0xE3, 0xA5, 0x99, 0x77, 0x26, 0xB8,
0xB4, 0x7C, 0x11, 0x44, 0x92, 0xD9, 0x23, 0x20, 0x89, 0x2E,
0x37, 0x3F, 0xD1, 0x5B, 0x95, 0xBC, 0xCF, 0xCD, 0x90, 0x87,
0x97, 0xB2, 0xDC, 0xFC, 0xBE, 0x61, 0xF2, 0x56, 0xD3, 0xAB,
0x14, 0x2A, 0x5D, 0x9E, 0x84, 0x3C, 0x39, 0x53, 0x47, 0x6D,
0x41, 0xA2, 0x1F, 0x2D, 0x43, 0xD8, 0xB7, 0x7B, 0xA4, 0x76,
0xC4, 0x17, 0x49, 0xEC, 0x7F, 0x0C, 0x6F, 0xF6, 0x6C, 0xA1,
0x3B, 0x52, 0x29, 0x9D, 0x55, 0xAA, 0xFB, 0x60, 0x86, 0xB1,
0xBB, 0xCC, 0x3E, 0x5A, 0xCB, 0x59, 0x5F, 0xB0, 0x9C, 0xA9,
0xA0, 0x51, 0x0B, 0xF5, 0x16, 0xEB, 0x7A, 0x75, 0x2C, 0xD7,
0x4F, 0xAE, 0xD5, 0xE9, 0xE6, 0xE7, 0xAD, 0xE8, 0x74, 0xD6,
0xF4, 0xEA, 0xA8, 0x50, 0x58, 0xAF

   };


/* General Galois Point */

static uint32_t GXB [] =
   {0xB217EC9D , 0x00030001 , 0xC6B077EB , 0xDAE1011E};

static uint32_t G4B [] =
   {0xF5E90DB5 , 0xF9321980 , 0x94626842 , 0x4F270A88};

static uint32_t GEN [] =
   {

0x25, 0xE0, 0x08, 0xAC, 0x47, 0xB2, 0x2C, 0xE3, 0x01, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00

   };



static uint8_t *ascii = "abcdefghijklmnopqrst";

static void trans16 (uint32_t c , uint8_t *b)
   {

   b[0] = ascii [ (c >> 12) & 15 ];
   b[1] = ascii [ (c >> 8 ) & 15 ];
   b[2] = ascii [ (c >> 4 ) & 15 ];
   b[3] = ascii [ (c >> 0 ) & 15 ];

   }


/* Modulo Operation to Prime */
static uint16_t  MOD3 ( uint32_t a )
   {

   uint32_t    R = a ;
   uint16_t    x , y ;

Loop:

   x = (R >> 0 ) & 0xFFFF ;
   y = (R >> 16) & 0xFFFF ;

   x = MOD (x) ;

   if ( y ==  0 )
      return  x ;

   R = (( uint32_t) y << 9) + (uint32_t) y * 13 + x ;
   goto   Loop ;


   return  x  ;

   }


/* Modulo Operation to Galois */
static uint16_t  MODB ( uint32_t a )
   {

   uint32_t    R = a ;
   uint16_t    x , y ;

Loop:

   x = (R >> 0 ) & 0xFFFF ;
   y = (R >> 16) & 0xFFFF ;

   x = ORD (x) ;

   if ( y ==  0 )
      return  x ;

   R = (uint32_t) y * 841 + x ;
   goto   Loop ;


   return  x  ;

   }



static uint32_t  GIN ( int elt )
   {
   return  GXE [ 255 - GOL[elt] ] ;
   }


/* Algebra Operation to Galois Field */
static int  INM ( uint16_t a , uint16_t *r)
   {

   int  sign =  -1 ;
   uint16_t  A , B , X , Y , M , D ;


   A = 0xFDF3 ; B = a ; X =  1 ; Y = 0 ;


_in_loop:


   D = A / B ; M = A - B * D ;

   if ( M == 0 )
      {

      *r = sign < 0 ? X : 0xFDF3-X ;

      if ( MOD3 (( uint32_t) (*r) * a ) == 1 )
         {return 0 ;}
      return  -1 ;
      }


   A = B ; B = M ;
   D = Y + X*D ; Y = X ; X = D ;

   sign = -sign;

   goto  _in_loop ;

   return  -1 ;

   }


/* Algebra Operation to Galois Field */
static int  DUB ( uint32_t *R , uint32_t A )
   {


   uint16_t  x , y ;
   uint16_t  a , b , r , s ;


   x = (A >> 0 ) & 0xFFFF ;
   y = (A >> 16) & 0xFFFF ;


   if ( y == 0 )
      {return -1 ;}


   a  = MOD3 ((uint32_t) y << 1);
   if ( INM (a,&r) != 0 )
      {return -1 ;}

   a  =  r ;

   b  = MOD3 ( (uint32_t) x * x  );
   b  = MOD3 (((uint32_t) b  << 1) + b + 4);
   b  = MOD3 ( (uint32_t) b  * a );
   a  = MOD3 ( (uint32_t) x << 1 );
   s  = MOD3 ( (uint32_t) b  * b );

   s  =  s < a ? 0xFDF3 - a + s : s - a ;
   r  =  x < s ? 0xFDF3 - s + x : x - s ;
   r  = MOD3 ((uint32_t) r * b) ;

   r  =  r < y ? 0xFDF3 - y + r : r - y ;
   *R = (uint32_t) r << 16 | s ;

   return 0;

   }



/* Algebra Operation to Galois Field */
static int  DDA ( uint32_t *R , uint32_t A , uint32_t B )
   {

   uint16_t  x , y ;
   uint16_t  a , b , r , s ;


   x = (A >> 0 ) & 0xFFFF ;
   r = (B >> 0 ) & 0xFFFF ;
   y = (A >> 16) & 0xFFFF ;
   s = (B >> 16) & 0xFFFF ;


   if ( x == r )
      {

      if ( y == s )
         {return DUB(R,A);}
      else
         {return  -1 ;}
      }


   a  = r < x ? 0xFDF3 - x + r : r - x;
   b  = s < y ? 0xFDF3 - y + s : s - y;

   if ( INM(a,&s) != 0 )
      {return  -1 ;}
   a  =  s ;
   b  = MOD3 ((uint32_t) b * a );


   a  = MOD3 ((uint32_t) x + r );
   s  = MOD3 ((uint32_t) b * b );

   s  = s < a ? 0xFDF3 - a + s : s - a;
   a  = x < s ? 0xFDF3 - s + x : x - s;

   r  = MOD3 ((uint32_t) b * a );
   r  = r < y ? 0xFDF3 - y + r : r - y ;

   *R = (uint32_t) r << 16 | s ;

   return 0;

   }



/* Algebra Operation to Galois Field */
static int  TIA ( uint32_t *R , uint32_t A )
   {


   uint16_t  x , y , z ;
   uint16_t  a , b , c , d , r , s ;


   x = (A >> 0 ) & 0xFFFF ;
   y = (A >> 16) & 0xFFFF ;


   if ( y == 0 )
      {*R = A ; return  0 ;}


   a  = MOD3 ( (uint32_t) y <<  1);
   b  = MOD3 ( (uint32_t) a  *  a);
   c  = MOD3 (((uint32_t) x <<  1) + x);
   d  = MOD3 ( (uint32_t) x  *  c);
   d  = MOD3 ( (uint32_t) d  +  4);


   r  = MOD3 ( (uint32_t) b  *  c);
   s  = MOD3 ( (uint32_t) d  *  d);
   r  = r < s ? 0xFDF3 - s + r : r - s ;


   if ( r == 0 )
      {return  -1 ;}


   s = MOD3 ((uint32_t)r * a);
   if ( INM (s,&z) != 0 )
      {return  -1 ;}
   s = z ;


   a  = MOD3 ((uint32_t)r  *  s);
   a  = MOD3 ((uint32_t)a  *  d);

   b  = MOD3 ((uint32_t)b  *  b);
   b  = MOD3 ((uint32_t)s  *  b);
   b  = b < a ? 0xFDF3 - a + b : b - a ;


   c  = b < a ? 0xFDF3 - a + b : b - a ;
   d  = MOD3 ((uint32_t)a  + b);
   r  = MOD3 ((uint32_t)c  * d);
   r  = MOD3 ((uint32_t)r  + x);

   s  = x < r ? 0xFDF3 - r + x : x - r ;
   s  = MOD3 ((uint32_t)s  * b);

   s  = s < y ? 0xFDF3 - y + s : s - y ;


   *R = (uint32_t) r << 16 | s ;
   return 0;

   }


/* Algebra Operation to Galois Field */
static uint32_t GMU ( uint32_t a , uint32_t b )
   {

   uint32_t  i , j ;

   if ( a == 0 || b == 0 )
      return 0 ;

   i = GOL [a];
   j = GOL [b];
   return  GXE [i+j];
   }


/* Algebra Operation to Galois Field */
static int MGL ( uint32_t *R , uint32_t k )
   {

   uint32_t  a , i , r = 0 ;


   for ( i = 0 ; i < 16 ; i += 2 )
      {

      a = ( k >> ( 14 - i ) ) & 3;

      if (a)
         {

         if ( i == 14 )
            { *R = GXB[a] ; return  0 ; }

         else
            {

            r = G4B[a];
            a = ( k >> ( 12 - i ) ) & 3;

            if ( a != 0 )
               {
               if ( DDA (&r , r , GXB[a]) != 0 )
                  {return  -1;}
               }

            i = i + 4 ;
            break ;
            }
         }
      }


   for ( ; i < 16 ; i += 2 )
      {

      if ( DUB (&r , r) != 0 )
         {return  -1;}
      if ( DUB (&r , r) != 0 )
         {return  -1;}

      a = ( k >> ( 14 - i ) ) & 3;

      if ( a != 0 )
         {
         if ( DDA (&r , r , GXB[a]) != 0 )
            {return  -1;}
         }
      }


   *R = r ;
   return 0;

   }


/* Algebra Operation to Galois Field */
static int MLT ( uint32_t *R , uint32_t X , uint32_t k )
   {


   uint32_t  a , i , r ;
   uint32_t xX [4] = {0} ;


   xX[1] = X;

   if (DUB (&xX[2] , X) != 0 )
      {return  -1;}
   if (TIA (&xX[3] , X) != 0 )
      {return  -1;}


   for ( i = 0; i < 16 ; i += 2 )
      {

      a = ( k >> ( 14 - i ) ) & 3;

      if (a)
         {r = xX[a] ; i += 2 ; break ;}
      }


   for ( ; i < 16 ; i += 2 )
      {

      if ( DUB (&r , r) != 0 )
         {return  -1;}
      if ( DUB (&r , r) != 0 )
         {return  -1;}

      a = ( k >> ( 14 - i ) ) & 3;

      if (a)
         {
         if ( DDA (&r , r , xX[a]) != 0 )
            {return  -1;}
         }
      }


   *R = r ;
   return 0;

   }


