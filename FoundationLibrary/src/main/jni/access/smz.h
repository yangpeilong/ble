#include "inter.h"
#include <stdint.h>
#include <stdlib.h>

static int merge_digital(uint8_t *in, uint8_t *ind, uint8_t *ou, int *len) {

    uint8_t buf[8];
    uint32_t halfval = 0;

    int index = 1;
    int hexflag = H4NUMBER;
    int halflag = 0, i;

    *len = 1;

    while (in < ind) {

        if (index > 5)
            break;

        if (in[0] >= '0' && in[0] <= '9') {

            if (halflag == 0) {
                halflag = 1;
                halfval = *in++ & 15;
            }

            else {

                halflag = 0;
                halfval <<= 4;
                halfval |= *in++ & 15;

                buf[index++] = (uint8_t) (halfval & 0xFF);
                (*len)++;
            }
        }

        else if (in[0] >= 'a' && in[0] <= 'f') {

            if (hexflag != H4LOWERCASE && hexflag != H4NUMBER)
                break;
            hexflag = H4LOWERCASE;

            if (halflag == 0) {
                halflag = 1;
                halfval = *in++ - 'a' + 10;
            }

            else {

                halflag = 0;
                halfval <<= 4;
                halfval |= *in++ - 'a' + 10;

                buf[index++] = (uint8_t) (halfval & 0xFF);
                (*len)++;
            }
        }

        else if (in[0] >= 'A' && in[0] <= 'F') {

            if (hexflag != H4UPPERCASE && hexflag != H4NUMBER)
                break;
            hexflag = H4UPPERCASE;

            if (halflag == 0) {
                halflag = 1;
                halfval = *in++ - 'A' + 10;
            }

            else {

                halflag = 0;
                halfval <<= 4;
                halfval |= *in++ - 'A' + 10;

                buf[index++] = (uint8_t) (halfval & 0xFF);
                (*len)++;
            }
        }

        else if (in[0] == ':') {

            if (hexflag != H4POINT && hexflag != H4NUMBER)
                break;
            hexflag = H4POINT;
            in++;

            if (halflag == 0) {
                halflag = 1;
                halfval = 10;
            }

            else {

                halflag = 0;
                halfval <<= 4;
                halfval |= 10;
                buf[index++] = (uint8_t) (halfval & 0xFF);
                (*len)++;
            }
        }

        else if (in[0] == '-') {

            if (hexflag != H4JOINTMARK && hexflag != H4NUMBER)
                break;
            hexflag = H4JOINTMARK;
            in++;

            if (halflag == 0) {
                halflag = 1;
                halfval = 10;
            }

            else {

                halflag = 0;
                halfval <<= 4;
                halfval |= 10;

                buf[index++] = (uint8_t) (halfval & 0xFF);
                (*len)++;
            }
        }

        else
            break;
    }

    if (index <= 2)
        return 0;

    buf[0] = (uint8_t) (H4BCD << 4);
    buf[0] |= (uint8_t) (((hexflag & 3) << 2) & 12);
    buf[0] |= (uint8_t) (((index - 3) << 0) & 3);

    for (i = 0; i < *len; i++)
        ou[i] = buf[i];

    return (index - 1) * 2;
}

static int unpack_digital(uint8_t *in, uint8_t *ind, uint8_t *ou, int *len) {

    int i;

    uint8_t type, option, count;

    uint8_t buf[16], *pbuf = buf;
    uint8_t halfval;

    type = (in[0] >> 4) & 15;
    option = (in[0] >> 2) & 3;
    count = (in[0] >> 0) & 3;

    if (type != H4BCD)
        return 0;

    count = count + 3;

    if (count > (int) (ind - in))
        return 0;

    if (option == H4JOINTMARK) {

        for (i = 1; i < count; i++) {

            halfval = (in[i] >> 4) & 15;

            if (halfval >= 0 && halfval <= 9)
                *pbuf++ = 0x30 | halfval;
            else if (halfval == 10)
                *pbuf++ = '-';
            else
                return 0;

            halfval = (in[i] >> 0) & 15;

            if (halfval >= 0 && halfval <= 9)
                *pbuf++ = 0x30 | halfval;
            else if (halfval == 10)
                *pbuf++ = '-';
            else
                return 0;
        }
    }
    else if (option == H4UPPERCASE) {

        for (i = 1; i < count; i++) {

            halfval = (in[i] >> 4) & 15;

            if (halfval >= 0 && halfval <= 9)
                *pbuf++ = 0x30 | halfval;
            else
                *pbuf++ = 'A' + halfval - 10;

            halfval = (in[i] >> 0) & 15;

            if (halfval >= 0 && halfval <= 9)
                *pbuf++ = 0x30 | halfval;
            else
                *pbuf++ = 'A' + halfval - 10;

        }
    }
    else if (option == H4LOWERCASE) {

        for (i = 1; i < count; i++) {

            halfval = (in[i] >> 4) & 15;

            if (halfval >= 0 && halfval <= 9)
                *pbuf++ = 0x30 | halfval;
            else
                *pbuf++ = 'a' + halfval - 10;

            halfval = (in[i] >> 0) & 15;

            if (halfval >= 0 && halfval <= 9)
                *pbuf++ = 0x30 | halfval;
            else
                *pbuf++ = 'a' + halfval - 10;

        }
    }
    else {

        for (i = 1; i < count; i++) {

            halfval = (in[i] >> 4) & 15;

            if (halfval >= 0 && halfval <= 9)
                *pbuf++ = 0x30 | halfval;
            else if (halfval == 10)
                *pbuf++ = ':';
            else
                return 0;

            halfval = (in[i] >> 0) & 15;

            if (halfval >= 0 && halfval <= 9)
                *pbuf++ = 0x30 | halfval;
            else if (halfval == 10)
                *pbuf++ = ':';
            else
                return 0;

        }
    }

    *len = (count - 1) * 2;

    for (i = 0; i < *len; i++)
        ou[i] = buf[i];

    return count;
}

static int merge_letter(uint8_t *in, uint8_t *ind, uint8_t *ou, int *len) {

    int n_consecutive, i;

    uint8_t indices[MAX_SUCCESSOR_N + 1];

    for (i = 0; i <= MAX_SUCCESSOR_N; i++)
        indices[i] = 0xFF;

    for (i = 0; i < 32; i++)
        if (chrs_by_chr_id[i] == in[0])
            break;

    if (i == 32)
        return 0;
    indices[0] = i;

    for (n_consecutive = 1; n_consecutive <= MAX_SUCCESSOR_N; n_consecutive++) {

        if (n_consecutive >= ind - in)
            break;

        for (i = 0; i < 16; i++)
            if (chrs_by_chr_and_successor_id[in[n_consecutive - 1]][i] == in[n_consecutive])
                break;

        if (i == 16)
            break;

        indices[n_consecutive] = i;

    }

    if (n_consecutive < 2)
        return 0;

    Retry:
    switch (n_consecutive) {

        case 2:

            if (indices[0] < 4 && indices[1] < 4) {
                ou[0] = (uint8_t) (B1C22 << 4);
                ou[0] |= (indices[0] << 2) & 15;
                ou[0] |= (indices[1] << 0) & 3;

                *len = 1;
                return n_consecutive;
            }
            break;

        case 3:

            if (indices[0] < 16 && indices[1] < 16 && indices[2] < 16) {
                ou[0] = (uint8_t) (B2C444 << 4);
                ou[0] |= (indices[0] << 0);
                ou[1] = (indices[1] << 4);
                ou[1] |= (indices[2] << 0);

                *len = 2;
                return n_consecutive;
            }
            else if (indices[0] < 32 && indices[1] < 8 && indices[2] < 16) {
                ou[0] = (uint8_t) (B2C534 << 4);
                ou[0] |= (indices[0] >> 1);
                ou[1] = (indices[0] << 7);
                ou[1] |= (indices[1] << 4);
                ou[1] |= (indices[2] << 0);

                *len = 2;
                return n_consecutive;
            }

            --n_consecutive;
            goto Retry;
            break;

        case 4:

            if (indices[0] < 4 && indices[1] < 8 && indices[2] < 8 && indices[3] < 16) {
                ou[0] = (uint8_t) (B2C2334 << 4);
                ou[0] |= (indices[0] << 2) & 15;
                ou[0] |= (indices[1] >> 1) & 3;
                ou[1] = (indices[1] << 7);
                ou[1] |= (indices[2] << 4);
                ou[1] |= (indices[3] << 0);

                *len = 2;
                return n_consecutive;
            }
            else if (indices[0] < 8 && indices[1] < 8 && indices[2] < 8 && indices[3] < 8) {
                ou[0] = (uint8_t) (B2C3333 << 4);
                ou[0] |= (indices[0] << 1) & 15;
                ou[0] |= (indices[1] >> 2) & 1;
                ou[1] = (indices[1] << 6);
                ou[1] |= (indices[2] << 3);
                ou[1] |= (indices[3] << 0);

                *len = 2;
                return n_consecutive;
            }
            else if (indices[0] < 16 && indices[1] < 4 && indices[2] < 8 && indices[3] < 8) {
                ou[0] = (uint8_t) (B2C4233 << 4);
                ou[0] |= (indices[0] << 0) & 15;
                ou[1] = (indices[1] << 6);
                ou[1] |= (indices[2] << 3);
                ou[1] |= (indices[3] << 0);

                *len = 2;
                return n_consecutive;
            }

            --n_consecutive;
            goto Retry;
            break;

        default:
            break;
    }

    return 0;
}

static int unpack_letter(uint8_t *in, uint8_t *ind, uint8_t *ou, int *len) {

    uint8_t type, idx[4];

    type = (in[0] >> 4) & 15;

    if ((type & 8) == 0 || type == H4BCD || type == HALUTF)
        return 0;

    switch (type) {

        case B1C22:
            idx[0] = (in[0] >> 2) & 3;
            idx[1] = (in[0] >> 0) & 3;
            ou[0] = chrs_by_chr_id[idx[0]];
            ou[1] = chrs_by_chr_and_successor_id[ou[0]][idx[1]];
            *len = 2;
            return 1;

        case B2C2334:
            idx[0] = (in[0] >> 2) & 3;
            idx[1] = (in[0] >> 0) & 3;
            idx[1] <<= 1;
            idx[1] |= (in[1] >> 7) & 1;
            idx[2] = (in[1] >> 4) & 7;
            idx[3] = (in[1] >> 0) & 15;

            ou[0] = chrs_by_chr_id[idx[0]];
            ou[1] = chrs_by_chr_and_successor_id[ou[0]][idx[1]];
            ou[2] = chrs_by_chr_and_successor_id[ou[1]][idx[2]];
            ou[3] = chrs_by_chr_and_successor_id[ou[2]][idx[3]];
            *len = 4;
            return 2;

        case B2C3333:
            idx[0] = (in[0] >> 1) & 7;
            idx[1] = (in[0] >> 0) & 1;
            idx[1] <<= 2;
            idx[1] |= (in[1] >> 6) & 3;
            idx[2] = (in[1] >> 3) & 7;
            idx[3] = (in[1] >> 0) & 7;

            ou[0] = chrs_by_chr_id[idx[0]];
            ou[1] = chrs_by_chr_and_successor_id[ou[0]][idx[1]];
            ou[2] = chrs_by_chr_and_successor_id[ou[1]][idx[2]];
            ou[3] = chrs_by_chr_and_successor_id[ou[2]][idx[3]];
            *len = 4;
            return 2;

        case B2C4233:
            idx[0] = (in[0] >> 0) & 15;
            idx[1] = (in[1] >> 6) & 3;
            idx[2] = (in[1] >> 3) & 7;
            idx[3] = (in[1] >> 0) & 7;

            ou[0] = chrs_by_chr_id[idx[0]];
            ou[1] = chrs_by_chr_and_successor_id[ou[0]][idx[1]];
            ou[2] = chrs_by_chr_and_successor_id[ou[1]][idx[2]];
            ou[3] = chrs_by_chr_and_successor_id[ou[2]][idx[3]];
            *len = 4;
            return 2;

        case B2C444:
            idx[0] = (in[0] >> 0) & 15;
            idx[1] = (in[1] >> 4) & 15;
            idx[2] = (in[1] >> 0) & 15;

            ou[0] = chrs_by_chr_id[idx[0]];
            ou[1] = chrs_by_chr_and_successor_id[ou[0]][idx[1]];
            ou[2] = chrs_by_chr_and_successor_id[ou[1]][idx[2]];
            *len = 3;
            return 2;

        case B2C534:
            idx[0] = (in[0] >> 0) & 15;
            idx[0] <<= 1;
            idx[0] |= (in[1] >> 7) & 1;
            idx[1] = (in[1] >> 4) & 7;
            idx[2] = (in[1] >> 0) & 15;

            ou[0] = chrs_by_chr_id[idx[0]];
            ou[1] = chrs_by_chr_and_successor_id[ou[0]][idx[1]];
            ou[2] = chrs_by_chr_and_successor_id[ou[1]][idx[2]];
            *len = 3;
            return 2;

        default:
            break;
    }

    return 0;

}

static int smco(void *original, int strlen, void *out) {

    uint8_t *ou = (uint8_t *) out;
    uint8_t *in = (uint8_t *) original;
    uint8_t *ind = (uint8_t *) original + strlen;

    uint8_t buf[8] = {0};
    uint32_t halfval = 0;

    int i, halflag = 0;
    int count, buflen;

    if (in == NULL || ou == NULL)
        return -1;

    if (strlen <= 0)
        return -1;

    while (in < ind) {

        count = merge_digital(in, ind, buf, &buflen);

        if (count > 0 && buflen > 0 && buflen <= 6) {

            for (i = 0; i < buflen; i++) {

                if (halflag == 0) {
                    ou[i] = buf[i];
                }

                else {
                    halfval = (halfval << 8) | buf[i];
                    ou[i] = (halfval >> 4) & 0xFF;
                    halfval &= 15;
                }
            }

            in += count;
            ou += buflen;
            continue;

        }

        count = merge_letter(in, ind, buf, &buflen);

        if (count > 0 && buflen > 0 && buflen <= 2) {

            for (i = 0; i < buflen; i++) {

                if (halflag == 0) {
                    ou[i] = buf[i];
                }

                else {
                    halfval = (halfval << 8) | buf[i];
                    ou[i] = (halfval >> 4) & 0xFF;
                    halfval &= 15;
                }
            }

            in += count;
            ou += buflen;
            continue;

        }

        if (in[0] & 0x80) {

            if (halflag == 0) {

                halflag = 1;
                halfval = ((uint32_t) HALUTF << 8) | *in++;
                *ou++ = (halfval >> 4) & 0xFF;
                halfval &= 15;

            }
            else {

                halflag = 0;
                *ou++ = ((halfval << 4) | HALUTF) & 0xFF;
                *ou++ = *in++;
                halfval = 0;

            }
        }

        else {

            if (halflag == 0) {
                *ou++ = *in++;
            }
            else {
                halfval = (halfval << 8) | *in++;
                *ou++ = (halfval >> 4) & 0xFF;
                halfval &= 15;
            }
        }

    }

    if (halflag != 0)
        *ou++ = (halfval << 4) & 0xFF;

    return (int) ((void *) ou - out);

}

static int deco(void *original, int complen, void *out) {

    uint8_t *ou = (uint8_t *) out;
    uint8_t *in = (uint8_t *) original;
    uint8_t *ind = (uint8_t *) original + complen;

    uint8_t buf[16];
    uint32_t halfval = 0;

    int i, halflag = 0;
    int count, olen;

    if (in == NULL || ou == NULL)
        return -1;

    if (complen <= 0)
        return -1;

    while (in < ind) {

        for (i = 0; i < 12; i++) {
            if (in + i >= ind)
                break;

            if (halflag == 0)
                buf[i] = in[i];
            else {
                halfval = (halfval << 8) | in[i];
                buf[i] = (halfval >> 4) & 0xFF;
                halfval &= 15;
            }
        }

        count = unpack_digital(buf, &buf[i], ou, &olen);

        if (count > 0 && olen > 0 && olen <= 10) {

            if (halflag != 0)
                halfval = in[count - 1] & 15;
            in += count;
            ou += olen;
            continue;

        }

        count = unpack_letter(buf, &buf[i], ou, &olen);

        if (count > 0 && olen > 1 && olen <= 4) {

            if (halflag != 0)
                halfval = in[count - 1] & 15;
            in += count;
            ou += olen;
            continue;
        }

        if (buf[0] & 0x80) {

            if ((buf[0] & 0xF0) != HALUTF << 4)
                return -1;

            *ou++ = (buf[0] << 4 | buf[1] >> 4) & 0xFF;

            if (halflag == 0) {
                halflag = 1;
                in += 2;
                halfval = buf[1] & 15;
            }
            else {
                halflag = 0;
                in += 1;
                halfval = 0;
            }
        }

        else {

            in++;
            *ou++ = buf[0];
            if (halflag != 0)
                halfval = (buf[1] >> 4) & 15;

        }
    }

    return (int) ((void *) ou - out);

}

