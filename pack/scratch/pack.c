#include <limits.h>
#include <lua.h>
#include <stdint.h>
#include <stdio.h>

/* number of bits in a character */
#define NB      CHAR_BIT

/* mask for one character (NB 1's)
 * Left shift 1 by CHAR_BIT amount of bits */

#define MC      ((1 << NB) - 1)

// BE
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

// BE
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
