#include <limits.h>
#include <lua.h>
#include <stdint.h>
#include <stdio.h>

/*
 * Bit shifting operator
 * Any blank spaces generated while shifting are filled up by zeroes
 *
 * How to pack a 32 bit unsigned integer into a binary format (a sequence 
 * of bytes)?
 *
 * Algorithm:
 *      1. Divide a 32 bit unsigned integer into 32 / X bytes where X is number 
 *      of bits in a byte.
 *      2. Pack the first X bits into the first byte.
 *      3. Pack the second X bits into the second byte.
 *      4. ... Repeat the same / similar procedure in Step 2 and Step 3 until 
 *      all bits in that unsigned integer are packed into individual bytes / 
 *      a portion of sequence of bits.
 * 
 * Remark
 *      1. Byte Order (BE and LE) (0x11223344)
 *          - LE means the least significant bit will the first byte
 *              - 0x44332211
 *          - BE means the most significant bit will the first byte
 *              - 0x11223344
 *      2. The number of bits in a byte can changed (impossible).
 *      3. By remark 2, be really careful when packing different data types 
 *      into binary form since different machine / architecture define 
 *      the number of bits required to store them differently.
 *          - int in machine A might take 32 bits
 *          - int in machine B might take less than 32 bits.
 *      4. Overflowing (Underflow as well?)
 *
 */

/* number of bits in a character */
#define NB      CHAR_BIT

/* mask for one character (NB 1's)
 * Left shift 1 by CHAR_BIT amount of bits */

#define MC      ((1 << NB) - 1)

/*
 * BE order
 *
 * Example 0x11223344
 *
 * 1. Right shift 24 bits (3 bytes) => first  byte => assign to the first  byte
 *  0x00000011 (Discard 000000, keep 11)
 * 2. Right shift 16 bits (2 bytes) => second byte => assign to the second byte
 *  0x00001122 (Discard 000011, keep 22)
 * 3. Right shift 8  bits (1 byte ) => third  byte => assign to the third  byte
 *  0x00112233 (Discard 001122, keep 33)
 * 4. Assign n directly in to the fourth byte
 *  0x11223344 (Discard 112233, keep 44)
 *
 * Why step 2 ~ 4 works? Since a byte (X bit although it always will be 8 bits) 
 * cannot fit all 32 bits, it will keep the last X bits in that 32 bits (left 
 * to right). Then, discard the rest of 32 - X bits.
 */
void pack_uint32_1 (unsigned long n) {
   unsigned char buff[4] = { 0 };

   buff[0] = n >> 24; 
   buff[1] = n >> 16; 
   buff[2] = n >>  8; 
   buff[3] = n; 

   for (size_t i = 0; i < 4; i++) {
       printf("Byte %zd: %x\n", i, buff[i]);
   }
}

/*
 * is_little = 1
 * 
 * n = 0x11223344
 * 
 * b = [0x00, 0x00, 0x00, 0x00]
 * 
 * b[0] = (char) (0x11223344 & ( (1 << 8) - 1 ) )
 *      = (char) (0x11223344 & ( 0b1000 0000 - 1 )
 *      = (char) (0x11223344 & ( 256 - 1 )
 *      = (char) (0x11223344 & ( 255 )
 *      = (char) (0x11223344 & 0x000000ff)
 *      = (char) (0x00000044) = 0x44
 * 
 * n >>= 8 / 0x00112233 /
 * 
 * b[1] = (char) (0x00112233 & ( (1 << 8) - 1 ) )
 *      = (char) (0x00112233 & 0x000000ff
 *      = (char) (0x00000033)
 *      = 0x33
 * 
 * n >>= 8 / 0x00001122 /
 * 
 * b[2] = (char) 0x00001122 & 0x000000ff = 0x22
 * 
 * n >>= 8 / 0x00000011 /
 * 
 * b[3] = (char) 0x00000011 & 0x000000ff = 0x11
 * 
 * b = { 0x44, 0x33, 0x22, 0x11 }
*/
void pack_uint32_2 (uint32_t n) {
    uint8_t buff[4] = { 0 };

    buff[0] = n >> 24;
    buff[1] = n >> 16;
    buff[2] = n >>  8;
    buff[3] = n;

    for (size_t i = 0; i < 4; i++) {
        printf("Byte %zd: %x\n", i, buff[i]);
    }
}

// Runtime
void pack_uint32_3 (lua_Unsigned n, int islittle) {
    char buff[4] = { 0 };

    buff[islittle ? 0 : 4 - 1] = (char) (n & MC);
    for (int i = 1; i < 4; i++) {
        n >>= NB;
        buff[islittle ? i : 4 - 1 - i] = (char) (n & MC);
    }

    for (size_t i = 0; i < 4; i++) {
        printf("Byte %zd: %x\n", i, buff[i]);
    }
}

int main() {
    uint32_t n = 0x11223344;

    printf("Packing method 1\n");
    pack_uint32_1(n);

    printf("Packing method 2\n");
    pack_uint32_2(n);

    printf("Packing method 3\n");
    pack_uint32_3(n, 1);
}
