#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define MAX_MSG_LENGTH 256

#include "smz.h"
#include "galois.h"
#include "berlekamp.h"

static uint64_t _mn_seed_ = 1;

static uint16_t _mn_flag_ = 1;

#define RND(a) ({ _mn_seed_ = _mn_seed_  *GXB[2] + G4B[1]; \
                     _mn_seed_+=(_mn_flag_ +=GXB[3])* G4B[3]; \
                     (uint32_t) (_mn_seed_ += (a)); })

/* Android版本必须注释掉,
 * __asm__(".symver memcpy,memcpy@GLIBC_2.2.5"); */

/* Append the parity bytes onto the end of the message */
static void padding(uint8_t msg[], int nbytes, uint8_t dst[]) {

    int i;

    for (i = 0; i < nbytes; i++)
        dst[i] = msg[i];

    for (i = 0; i < NPAR; i++)
        dst[i + nbytes] = pBytes[NPAR - 1 - i];
}

static int verify_sync(uint8_t data[], int nbytes) {

    int i, j, sum;

    for (j = 0; j < NPAR; j++) {

        sum = 0;

        for (i = 0; i < nbytes; i++)
            sum = data[i] ^ GMU(GXE[j + 1], sum);

        synBytes[j] = (uint8_t) sum;
    }

    for (j = 0; j < NPAR; j++) {
        if (synBytes[j] != 0)
            return -1;
    }

    return 0;
}

static void generate_synopsis(uint8_t msg[], int nbytes, uint8_t dst[]) {

    int i, j;
    uint8_t dbyte, LFSR[NPAR + 1];

    for (i = 0; i < NPAR + 1; i++)
        LFSR[i] = 0;

    for (i = 0; i < nbytes; i++) {

        dbyte = msg[i] ^ LFSR[NPAR - 1];

        for (j = NPAR - 1; j > 0; j--)
            LFSR[j] = LFSR[j - 1] ^ GMU(GEN[j], dbyte);

        LFSR[0] = GMU(GEN[0], dbyte);
    }

    for (i = 0; i < NPAR; i++)
        pBytes[i] = LFSR[i];

    padding(msg, nbytes, dst);
}

/**********************************************************/
/* For QR-code */
int qdpack(uint8_t *msg, int len, uint8_t *cipher, uint32_t magic_cube) {

    int i, rest = 4;

    uint8_t __attribute__((aligned(4))) cipdat[MAX_MSG_LENGTH * 2];
    uint8_t tmpdat[MAX_MSG_LENGTH * 2];
    uint8_t smzlen, sum = 0;

    time_t t;
    volatile uint32_t k, r, s, K, R, *p, M, smflag = 1;

    /* Parameter check, */
    if (!msg || !cipher)
        return -1;

    if (len <= 0 || len >= MAX_MSG_LENGTH - 16)
        return -1;

    /* random */
    for (i = 0; i < len; i++) {
        sum += msg[i];
        if ((i & 15) == 15) {
            RND(sum);
        }
    }

    RND(i);
    RND(sum);
    sum = 0;

    M = MODB(magic_cube);
    if (M == 0) {
        M = 8;
    }

    memset(tmpdat, 0, MAX_MSG_LENGTH * 2);
    memset(cipdat, 0, MAX_MSG_LENGTH * 2);

    generate_synopsis(msg, len, tmpdat);

    /* search & replace */
    for (i = 0; i < len + 8; i++) {

        if (rest == 0)
            break;

        if (tmpdat[i] & 0x80) {
            tmpdat[i] = 0x20;
            rest--;
        }
    }

    for (i = 0; i < len + 8; i++) {

        if (rest == 0)
            break;

        if (chrs_id[tmpdat[i]] == 0) {
            tmpdat[i] = 0x20;
            rest--;
        }
    }

    for (i = 0; i < len + 8; i++) {

        if (rest == 0)
            break;

        if (tmpdat[i] != 0x20) {
            tmpdat[i] = 0x20;
            rest--;
        }
    }

    i = smco(tmpdat, len + 8, cipdat + 8);

    if (i <= 0 || i > len + 8) {

        memset(cipdat, 0, MAX_MSG_LENGTH * 2);
        memcpy(cipdat + 8, tmpdat, len + 8);

        smzlen = len + 8;
        smflag = 0;
    }
    else
        smzlen = i;

    loop1:
    t = time(NULL);
    RND((uint32_t) t);
    k = MODB(RND((uint32_t) t));
    if (k == 0)
        goto loop1;

    if (MGL((uint32_t *) &K, k) != 0)
        goto loop1;

    /* get k, K */
    k = MODB(RND(K));
    if (k == 0)
        goto loop1;

    if (MGL((uint32_t *) &K, k) != 0)
        goto loop1;

    /* get r, s */
    t = time(NULL);
    RND(K);
    r = MODB(RND((uint32_t) t));
    if (r == 0)
        goto loop1;

    s = MODB(k + r * M);
    if (s == 0)
        goto loop1;

    k = RND(s) & 0xFC;
    k |= smflag;

    if (MLT((uint32_t *) &R, K, k) != 0)
        goto loop1;

    p = (uint32_t *) (cipdat + 8);
    for (i = 0; i < (smzlen + 3) / 4; i++) {
        p[i] ^= R;
        R += k | 1;
    }

    /* Header */
    cipdat[0] = 0xD1;
    cipdat[1] = k;
    cipdat[2] = smzlen;
    cipdat[3] = 0x00;

    p = (uint32_t *) (cipdat + 4);
    *p = (r << 16) | (s << 0);

    for (i = 0; i < smzlen + 8; i++)
        sum += cipdat[i];
    cipdat[3] = ~sum;

    memcpy(cipher, cipdat, smzlen + 8);

    return smzlen + 8U;
}

int qdunpack(uint8_t *cipher, int len, uint8_t *plaintext, uint32_t magic_cube) {

    int i;

    uint8_t __attribute__((aligned(4))) cipdat[MAX_MSG_LENGTH * 2];
    uint8_t smzlen, sum = 0;

    volatile uint32_t r, s, K, R, S, M, *p;

    /* Parameter check, */
    if (!plaintext || !cipher)
        return -1;

    if (len <= 8 || len >= MAX_MSG_LENGTH - 16)
        return -1;

    smzlen = cipher[2];

    if (smzlen == 0 || smzlen > len - 8)
        return -1;

    /* Header */
    if (cipher[0] != 0xD1)
        return -1;

    /* Checksum */
    for (i = 0; i < len; i++)
        sum += cipher[i];

    if (sum != 0xFF)
        return -1;

    M = magic_cube;
    if (M == 0) {
        M = G4B[2];
    }

    memset(cipdat, 0, MAX_MSG_LENGTH * 2);
    memcpy(cipdat, cipher, len);

    p = (uint32_t *) (cipdat + 4);
    r = (p[0] >> 16) & 0xFFFF;
    s = (p[0] >> 0) & 0xFFFF;

    if (MGL((uint32_t *) &S, s) != 0)
        return -1;

    if (MLT((uint32_t *) &R, M, r) != 0)
        return -1;

    R = (_p_ << 16) - (R & 0xFFFF0000) + (R & 0xFFFF);

    if (DDA((uint32_t *) &K, S, R) != 0)
        return -1;

    if (MLT((uint32_t *) &R, K, cipdat[1]) != 0)
        return -1;

    p = (uint32_t *) (cipdat + 8);

    for (i = 0; i < (len - 5) / 4; i++) {
        p[i] ^= R;
        R += cipdat[1] | 1;
    }

    i = smzlen;
    if (cipdat[1] & 1)
        i = deco(cipdat + 8, smzlen, plaintext);
    else
        memcpy(plaintext, cipdat + 8, smzlen);

    verify_sync(plaintext, i);
    evaluate_omega(plaintext, i);

    return i - 8;
}

/**********************************************************/
/* For Svr push message */
int qdmenc(uint8_t *msg, int msglen, uint8_t *cipher, int cipherlen, uint32_t magic_cube) {

    int i, j;

    uint8_t *tmp;
    uint16_t sum = 0, off = 16;

    time_t t;
    volatile uint32_t k, r, s, K, *p, M;

    /* Parameter check, */
    if (!msg || !cipher)
        return -1;

    if (msglen <= 0)
        return -1;

    if (cipherlen < msglen + 16 + (msglen + 239) / 240 * 8) {
        printf("the cipherlen is too small for encrpyte msg, please make it larger.\n");
        return -1;
    }

    tmp = (uint8_t *) malloc((msglen + 1024) / 240 * 8 + 256);

    if (!tmp)
        return -1;

    /* random seed */
    for (i = 0; i < msglen; i++) {
        sum += msg[i];
        if ((i & 127) == 127) {
            RND(sum);
        }
    }

    RND(i);
    RND(sum);

    M = MODB(magic_cube);
    if (M == 0) {
        M = 8;
    }

    /* --- */
    for (i = 0; i < msglen / 240; i++, off += 8) {
        generate_synopsis(msg + i * 240, 240, tmp);
        memcpy(&cipher[off], &tmp[240], 8);
    }

    if (msglen % 240 != 0) {
        generate_synopsis(msg + i * 240, msglen % 240, tmp);
        memcpy(&cipher[off], &tmp[msglen % 240], 8);
        off += 8;
    }

    cipherlen = msglen + off;

    /* random */
    loop1:
    t = time(NULL);
    RND((uint32_t) t);
    k = MODB(RND((uint32_t) t));
    if (k == 0)
        goto loop1;

    if (MGL((uint32_t *) &K, k) != 0)
        goto loop1;

    /* get k, K */
    k = MODB(RND(K));
    if (k == 0)
        goto loop1;

    if (MGL((uint32_t *) &K, k) != 0)
        goto loop1;

    /* get r, s */
    t = time(NULL);
    RND(K);

    r = MODB(RND((uint32_t) t));
    if (r == 0)
        goto loop1;

    s = MODB(k + r * M);
    if (s == 0)
        goto loop1;

    p = (uint32_t *) tmp;

    for (i = 0; i < msglen / 256; i++) {

        memcpy(tmp, msg + 256 * i, 256);

        for (j = 0; j < 256 / 4; j++) {
            p[j] ^= K;
            K += s | 1;
        }

        memcpy(cipher + off + 256 * i, tmp, 256);
    }

    if (msglen % 256 != 0) {

        memcpy(tmp, msg + 256 * i, msglen % 256);

        for (j = 0; j < ((msglen & 0xFF) + 3) / 4; j++) {
            p[j] ^= K;
            K += s | 1;
        }

        memcpy(cipher + off + 256 * i, tmp, msglen % 256);
    }

    /* Header */
    memcpy(cipher, "Qdin", 4);
    memcpy(cipher + 4, &cipherlen, 3);
    cipher[7] = 0x10; /*version*/
    memcpy(cipher + 8, &off, 2);
    cipher[10] = 0;
    cipher[11] = 0;

    memcpy(cipher + 12, (void *) &s, 2);
    memcpy(cipher + 14, (void *) &r, 2);

    sum = 0;

    memcpy(tmp, cipher, off);
    for (i = 0; i < off / 2; i++)
        sum += ((uint16_t *) tmp)[i];
    sum = ~sum;
    memcpy(cipher + 10, &sum, 2);

    free(tmp);

    return cipherlen;
}

int qdmdec(uint8_t *cipher, int cipherlen, uint8_t *plaintext, int plainlen, uint32_t magic_cube) {

    int i, j;

    uint8_t *tmp;
    uint16_t off, sum = 0;

    volatile uint32_t r, s, K, R, S, M, *p;

    /* Parameter check, */
    if (!plaintext || !cipher)
        return -1;

    if (cipherlen <= 16)
        return -1;

    memcpy(&off, cipher + 8, 2);

    if (plainlen < cipherlen - off)
        return -1;

    tmp = (uint8_t *) malloc((cipherlen + 1024) / 240 * 8 + 256);

    if (!tmp)
        return -1;

    /* Header */
    if (strncmp((char *) cipher, "Qdin", 4) != 0) {
        free(tmp);
        return -1;
    }

    /* Checksum */
    memcpy(tmp, cipher, off);

    for (i = 0; i < off / 2; i++)
        sum += ((uint16_t *) tmp)[i];

    if (sum != 0xFFFF)
        return -1;

    M = magic_cube;
    if (M == 0) {
        M = G4B[2];
    }

    /* Get r , s */
    memcpy((void *) &s, cipher + 12, 2);
    memcpy((void *) &r, cipher + 14, 2);

    if (MGL((uint32_t *) &S, s) != 0)
        return -1;

    if (MLT((uint32_t *) &R, M, r) != 0)
        return -1;

    R = (_p_ << 16) - (R & 0xFFFF0000) + (R & 0xFFFF);

    if (DDA((uint32_t *) &K, S, R) != 0)
        return -1;

    /* --- */
    p = (uint32_t *) tmp;

    for (i = 0; i < (cipherlen - off) / 240; i++) {

        memcpy(tmp, cipher + off + 240 * i, 240);
        memcpy(tmp + 240, cipher + 16 + 8 * i, 8);

        for (j = 0; j < 240 / 4; j++) {
            p[j] ^= K;
            K += s | 1;
        }

        if (verify_sync(tmp, 248) != 0) {
            evaluate_omega(tmp, 248);
        }

        memcpy(plaintext + 240 * i, tmp, 240);
    }

    r = (cipherlen - off) % 240;

    if (r != 0) {

        memcpy(tmp, cipher + off + 240 * i, r);
        memcpy(tmp + r, cipher + 16 + 8 * i, 8);

        for (j = 0; j < (r + 3) / 4; j++) {
            p[j] ^= K;
            K += s | 1;
        }

        if (verify_sync(tmp, r + 8) != 0) {
            evaluate_omega(tmp, r + 8);
        }

        memcpy(plaintext + 240 * i, tmp, r);
    }

    free(tmp);

    return cipherlen - off;
}

int qd_generate_key_pair(const char *const p_seed, uint32_t *p_enc_key, uint32_t *p_dec_key) {
    time_t t;
    uint32_t k, K, s = 0, i;

    if (p_seed == NULL || p_enc_key == NULL || p_dec_key == NULL) {
        return -1;
    }
    for (i = 0; i < strlen(p_seed); i++) {
        s += p_seed[i];
        if ((i & 3) == 3) {
            RND(s);
        }
    }

    RND(s);

    loop_key:
    t = time(NULL);
    RND((uint32_t) t);

    k = MODB((s = RND((uint32_t) t)));
    if (k == 0)
        goto loop_key;

    if (MGL(&K, k) != 0)
        goto loop_key;
    *p_enc_key = s;
    *p_dec_key = K;

    printf("Encrypt Magic: 0x%08X\nDecrypt Magic: 0x%08X\n", s, K);
    return 0;
}

#ifdef KEYPAIR

/**********************************************************/

int main(int argc, char *argv[])
{

    time_t t;
    uint32_t k, K, s, i;

    if (argc != 2) {
        printf("ERR: parameter MUST be 2!\n");
        return -1;
    }

    for (i = 0; i < sizeof(argv[1]); i++) {
        s += argv[1][i];
        if ((i & 3) == 3) {
            RND(s);
        }
    }

    RND(s);

    loop_key:
    t = time(NULL);
    RND((uint32_t)t);

    k = MODB((s = RND((uint32_t)t)));
    if (k == 0)
    goto loop_key;

    if (MGL((uint32_t *)&K, k) != 0)
    goto loop_key;

    printf("Encrypt Magic: 0x%08X\nDecrypt Magic: 0x%08X\n", s, K);
    return 0;
}

#endif

/**********************************************************/
/* For Qding local encrypt */
#if 0

static uint64_t lokey = 0;

static void m_next(void)
{

    volatile union {
        uint64_t d;
        uint32_t w[2];
        uint16_t h[4];
    }D;

    volatile uint16_t th0, th1;
    volatile uint32_t tw0, tw1;

    D.d = lokey;

    th0 = D.h[1] > D.h[0] ? D.h[1] : D.h[0];
    th1 = D.h[3] > D.h[2] ? D.h[3] : D.h[2];

    for (;;) {
        if ((th0 = MODB(th0)) != 0)
        if (MGL((uint32_t *)&tw0, th0) == 0)
        break;
        th0 += 19;
    }

    for (;;) {
        if ((th1 = MODB(th1)) != 0)
        if (MGL((uint32_t*)&tw1, th1) == 0)
        break;
        th1 += 37;
    }

    D.w[0] = (tw0 & 0x55555555U) | (tw1 & 0xAAAAAAAAU);
    D.w[1] = (tw1 & 0x55555555U) | (tw0 & 0xAAAAAAAAU);

    lokey = D.d;
}

int qdencrypt (uint8_t *msg , int msglen , uint8_t *cipher ,
        int cipherlen , uint32_t magic_cube)
{

    volatile union {uint64_t d; uint32_t w[2];}D;

    time_t t;
    uint32_t *p , s = 20150609 , i;

    /* Parameter check, */
    if (!msg || !cipher)
    return -1;

    if (magic_cube == 0)
    return -1;

    if (msglen <= 0 || msglen + 4 > cipherlen)
    return -1;

    memcpy (cipher , msg , msglen);
    cipher[msglen] = 0;
    __android_log_print(ANDROID_LOG_INFO,"QDKEY","Hixxxx %d,Msg %s , %d (%08X)",__LINE__,cipher,msglen,magic_cube);

    memset (cipher , 0 , cipherlen);

    t = time ( NULL);
    RND ((uint32_t)t);

    loop_rnd:

    s += RND (magic_cube);
    if (s < 0x1000)
    goto loop_rnd;

    lokey = (uint64_t) s * magic_cube;
    m_next ();

    p = (uint32_t *) malloc (msglen + 3 );
    if (!p)
    return -1;

    memcpy ( p , msg , msglen);

    for (i = 0; i < (msglen + 3)/4; i += 2)
    {

        D.d = lokey;

        p[i + 0] ^= D.w[0];
        p[i + 1] ^= D.w[1];

        m_next ();

    }

    memcpy (cipher + 0 ,&s , 4 );
    memcpy (cipher + 4 , p , msglen);

    __android_log_print(ANDROID_LOG_INFO,"QDKEY","Hixxxx %d,Cip %02X %02X %02X %02X %02X %02X",__LINE__,cipher[0],cipher[1],cipher[2],cipher[3],cipher[4],cipher[5]);

    free (p);

    return msglen + 4;

}

int qddecrypt (uint8_t *cipher , int cipherlen , uint8_t *plaintext ,
        int plainlen , uint32_t magic_cube)
{

    volatile union {uint64_t d; uint32_t w[2];}D;

    time_t t;
    uint32_t *p , s , i;

    __android_log_print(ANDROID_LOG_INFO,"QDKEY","Hixxxx %d,Cip %02X %02X %02X %02X %02X %02X , %d (%08X)",__LINE__,cipher[0],cipher[1],cipher[2],cipher[3],cipher[4],cipher[5],cipherlen,magic_cube);

    /* Parameter check, */
    if (!plaintext || !cipher)
    return -1;

    if (magic_cube == 0)
    return -1;

    if (plainlen <= 0 || plainlen + 4 < cipherlen)
    return -1;

    memset (plaintext , 0 , plainlen);
    memcpy ( &s , cipher , 4);

    lokey = (uint64_t) s * magic_cube;
    m_next ();

    p = (uint32_t*) malloc (cipherlen + 3);
    if (!p)
    return -1;

    memcpy ( p , cipher + 4 , cipherlen - 4 );

    for (i = 0; i < (cipherlen - 1)/4; i += 2)
    {

        D.d = lokey;

        p[i + 0] ^= D.w[0];
        p[i + 1] ^= D.w[1];

        m_next ();

    }

    memcpy (plaintext , p , cipherlen - 4 );
    __android_log_print(ANDROID_LOG_INFO,"QDKEY","Hixxxx %d,Msg %02X %02X %02X %02X %02X %02X",__LINE__,p[0],p[1],p[2],p[3],p[4],p[5]);

    free (p);

    return cipherlen - 4;

}
#else
#endif

#if 0
int qdencrypt(uint8_t *msg, int msglen, uint8_t *cipher, int cipherlen, uint32_t magic_cube) {

    volatile union {
        uint64_t d;
        uint32_t w[2];
    }D;

    time_t t;
    uint32_t p[256], s = 20150609, i;

    /* Parameter check, */
    if (!msg || !cipher)
    return -1;

    if (magic_cube == 0)
    return -1;

    if (msglen <= 0 || msglen + 4 > cipherlen)
    return -1;

    memcpy(cipher, msg, msglen);
    cipher[msglen] = 0;
    __android_log_print(ANDROID_LOG_INFO, "QDKEY", "Hixxxx %d,Msg %s , %d (%08X)", __LINE__, cipher, msglen,
            magic_cube);

    memset(cipher, 0, cipherlen);

    t = time( NULL);
    RND((uint32_t )t);

    loop_rnd:

    s += RND(magic_cube);
    if (s < 0x1000)
    goto loop_rnd;

    lokey = (uint64_t) s * magic_cube;
    m_next();

    /* p = (uint32_t *) malloc (msglen + 3 ) ;
     if (!p)
     return -1 ; */

    memcpy(p, msg, msglen);

    for (i = 0; i < (msglen + 3) / 4; i += 2) {

        D.d = lokey;

        p[i + 0] ^= D.w[0];
        p[i + 1] ^= D.w[1];

        m_next();

    }

    memcpy(cipher + 0, &s, 4);
    memcpy(cipher + 4, p, msglen);

    __android_log_print(ANDROID_LOG_INFO, "QDKEY", "Hixxxx %d,Cip %02X %02X %02X %02X %02X %02X", __LINE__, cipher[0],
            cipher[1], cipher[2], cipher[3], cipher[4], cipher[5]);

// free (p);

    return msglen + 4;

}

int qddecrypt(uint8_t *cipher, int cipherlen, uint8_t *plaintext, int plainlen, uint32_t magic_cube) {

    volatile union {
        uint64_t d;
        uint32_t w[2];
    }D;

    time_t t;
    uint32_t p[256], s, i;

    __android_log_print(ANDROID_LOG_INFO, "QDKEY", "Hixxxx %d,Cip %02X %02X %02X %02X %02X %02X , %d (%08X)", __LINE__,
            cipher[0], cipher[1], cipher[2], cipher[3], cipher[4], cipher[5], cipherlen, magic_cube);

    /* Parameter check, */
    if (!plaintext || !cipher)
    return -1;

    if (magic_cube == 0)
    return -1;

    if (plainlen <= 0 || plainlen + 4 < cipherlen)
    return -1;

    memset(plaintext, 0, plainlen);
    memcpy(&s, cipher, 4);

    lokey = (uint64_t) s * magic_cube;
    m_next();

    /* p = (uint32_t*) malloc (cipherlen + 3) ;
     if (!p)
     return -1 ; */

    memcpy(p, cipher + 4, cipherlen - 4);

    for (i = 0; i < (cipherlen - 1) / 4; i += 2) {

        D.d = lokey;

        p[i + 0] ^= D.w[0];
        p[i + 1] ^= D.w[1];

        m_next();

    }

    memcpy(plaintext, p, cipherlen - 4);
    __android_log_print(ANDROID_LOG_INFO, "QDKEY", "Hixxxx %d,Msg %02X %02X %02X %02X %02X %02X", __LINE__, p[0], p[1],
            p[2], p[3], p[4], p[5]);

// free (p);

    return cipherlen - 4;

}
#endif

int qdpin(uint8_t *pin, int pinlen, uint8_t *key, int keylen, uint32_t magic_cube) {

    volatile uint32_t A = 2015, B, i;

    /* Parameter check, */
    if (!pin || pin[0] == 0 || pinlen < 0)
        return -1;

    if (!key || keylen < 10)
        return -1;

    if (magic_cube == 0)
        return -1;

    memset(key, 0, keylen);

    for (i = 0; i < pinlen; ++i)
        if (pin[i] != 0)
            A = A * pin[i] + 0x23;

    for (; ;) {
        if ((A = MODB(A)) != 0) if (MGL((uint32_t *) &B, A) == 0)
            break;
        A += 0x23;
    }

    B ^= magic_cube;
    sprintf((char *) key, "0x%08X", B);

    return 10;
}

/* 计算给定MAC地址的Hash */
uint16_t qdhash(uint8_t *mac, uint32_t maclen, uint32_t magic) {

    uint32_t seed, flag, i;

    flag = magic >> 5;
    seed = magic * 0xc6b077ebU + flag;

    for (i = 0; i < maclen; ++i) {
        seed = seed * 0xc6b077ebU + flag + mac[i];
        flag = flag + 0xdae1011dU;
    }

    return (uint16_t) seed;
}

/* 查找MAC是否在给定key之内 */
int qdcheckmac(uint16_t *key, int keynum, uint8_t *mac, uint32_t maclen, uint32_t magic) {

    int i;
    uint16_t hash;

    hash = qdhash(mac, maclen, magic);
    printf("%4X\n", hash);

    for (i = 0; i < keynum; ++i) {
        if (key[i] == hash)
            return i;
    }

    return -1;
}
