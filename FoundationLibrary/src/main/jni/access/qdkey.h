#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

/*
 ** Make sure we can call this stuff from C++.
 */
#ifdef __cplusplus
extern "C" {
#endif

int qd_generate_key_pair(const char *const p_seed, uint32_t *p_enc_key, uint32_t *p_dec_key);

int qdpack(uint8_t *msg, int len, uint8_t *cipher, uint32_t magic_cube);

int qdunpack(uint8_t *cipher, int len, uint8_t *plaintext, uint32_t magic_cube);

int qdmenc(uint8_t *msg, int msglen, uint8_t *cipher, int cipherlen, uint32_t magic_cube);

int qdmdec(uint8_t *cipher, int cipherlen, uint8_t *plaintext, int plainlen, uint32_t magic_cube);

int qdcheckmac(uint16_t *key, int keynum, uint8_t *mac, uint32_t maclen, uint32_t magic);

uint16_t qdhash(uint8_t *mac, uint32_t maclen, uint32_t magic);

#ifdef __cplusplus
} /* end of the 'extern "C"' block */
#endif
