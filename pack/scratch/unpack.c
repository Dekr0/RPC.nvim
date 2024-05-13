#include <limits.h>
#include <lua.h>
#include <stdint.h>
#include <stdio.h>

/* number of bits in a 32 bits unsigned integer */
#define UINT32_NB    32

/* number of bytes in a 32 bits unsigned integer */
#define UINT32_NBY   UINT32_NB / CHAR_BIT

/* mask for one character (NB 1's) */
#define MC           ((1 << CHAR_BIT) - 1)

/*
 * How to unpack a sequence of bytes represented a 32 bit unsigned integer?
 * 
 * Procedure
 *  Assume there are X bits in a byte.
 *  1. You receive a sequence of bytes represented a 32 bits unsigned integer.
 *  Each byte (each group of X bits) represents a portion of a 32 bits unsigned 
 *  integer.
 *  2. Assign first byte (group of X bits) of the received byte into the first 
 *  portion (X bits) of a 32 bits unsigned integer (left to right / right to left)
 *  based on the byte order.
 *  3. Assign second byte (group of X bits) of the received byte into the second 
 *  portion (X bits) of a 32 bits unsigned integer (left to right / right to left)
 *  based on the byte order.
 *  ...
 *  4. Repeat the same / similar steps in step 2 and step 3 until until all 32 
 *  bits are assigned by the received bytes
 *
 * Remark
 *  1. The amount of bits in a sequence of bytes you received can be larger / 
 *  smaller than the amount of bits for the data type you wan to recover. 
 *  Specifically, the amount of bits for a data type depends on architecture, 
 *  compiler, OS, etc. 
 *  2. Overflow and underflow
 *  3. Signed and unsigned
 *  4. Byte order.
 *  5. Be really careful on data type casting.
 */

void pack_uint32 (uint32_t n, int is_little, unsigned char *b) {
    b[is_little ? 0 : UINT32_NBY - 1] = (char) (n & MC); /* first byte */
    for (size_t i = 1; i < UINT32_NBY; i++) {
        n >>= CHAR_BIT;
        b[is_little ? i : UINT32_NBY - 1 - i] = (char) (n & MC);
    }
}


/* Example BE (left to right) { 0x11, 0x22, 0x33, 0x44 }
 * 1. Left shift 24 bits (3 bytes) of the first  received byte => the first  8 bits (byte) go into the most significant byte
 *  0x00000011 << 24 = 0x11000000 
 * 2. Left shift 16 bits (2 bytes) of the second received byte => the second 8 bits (byte) go into the second most significant byte
 *  0x00000022 << 16 = 0x00220000
 * 3. Left shift  8 bits (1 bytes) of the third  received byte => the third  8 bits (byte) go into the third  most significant byte
 *  0x00000033 << 8 = 0x00003300
 * 4. Assign the last received byte go into the least significant byte
 *  0x00000044
 *
 *  0x11000000 | 0x00220000 | 0x00003300 | 0x00000044
 *  0x11220000 | 0x00003300 | 0x00000044
 *  0x11223300 | 0x00000044
 *  0x11223344
 *
 * Why the | (bit OR) operator? 
 * What | operator can do is to copy a sequence of bits to a blank field (zero bits).
 * You need to use it along with bit shifting so that it copy into the right place.
 * Otherwise, some zero bits that are not blank bits will get overwritten
 * Example
 * 1010 0000 1100
 * 1111 0101 0000
 * copy the second byte.
 * left shift 1 byte => 0101 0000 0000
 * right shift 1 byte => 0000 0101 0000
 * 1010 0000 1100 | 0000 0101 0000
 */
uint32_t unpack_uint32_1 (unsigned char *b, int is_little) {
    return is_little 
        ? ((uint32_t) b[3] << 24) |
          ((uint32_t) b[2] << 16) |
          ((uint32_t) b[1] << 8 ) |
          b[0] 
        : ((uint32_t) b[0] << 24) |
          ((uint32_t) b[1] << 16) |
          ((uint32_t) b[2] << 8 ) |
          b[3];
}

/* Example 
 * BE { 0x11, 0x22, 0x33, 0x44 }
 * 0x0   << 8 => 0x000
 * 0x000   | 0x011   => 0x011
 * 0x011 << 8 => 0x01100
 * 0x01100 | 0x00022 => 0x01122
 */
uint32_t unpack_uint32 (unsigned char *b, int is_little) {
    unsigned long long n = 0;
    size_t i;
    for (i = UINT32_NBY - 1; i >= 0; i--) {
        n <<= CHAR_BIT;
        n |= (unsigned long long) (unsigned char) b[is_little ? i : UINT32_NBY - 1 - i];
    }
    return (uint32_t) n;
}

int main(void) {
    uint32_t n = 0x11223344;

    unsigned char b[UINT32_NBY] = { 0 };

    pack_uint32(n, 1, b);

    for (size_t i = 0; i < 4; i++) {
        printf("Byte %zd: %x\n\n", i, b[i]);
    }

    unsigned long long rn = unpack_uint32_1(b, 1);

    printf("Recovered: %llx", rn);
}
