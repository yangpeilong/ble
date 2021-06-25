#ifndef _MD5_H_
#define _MD5_H_

#define MD5_HASHBYTES 16

typedef unsigned int	u_int32_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MD5Context {
	u_int32_t buf[4];
	u_int32_t bits[2];
	unsigned char in[64];
} MD5_CTX;

void MD5Init(MD5_CTX *ctx);
void MD5Update(MD5_CTX *ctx, unsigned char const *buf, unsigned len);
void MD5Final(unsigned char digest[MD5_HASHBYTES], MD5_CTX *ctx);

/**
* @param filename - filename.
* @param digest - a reference, hexadecimal notation.
*/
void MD5File(const char *filename, unsigned char *digest);

/**
* @param data - message.
* @param len - message length
* @param digest - a reference, hexadecimal notation.
*/
void MD5Data(const unsigned char *data, unsigned int len, unsigned char *digest);

#ifdef __cplusplus
}
#endif

#endif /* !_MD5_H_ */
