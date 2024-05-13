#include <stdio.h>

int main(void) {
  /* the interpretation of char, unsigned char, signed char depends on usage 
   * context (e.g. String format specifier) */
  char a = -1;
  unsigned char b = 129;
  signed char c = -1;
  printf("a = %hhd\n", a);
  printf("b = %hhu\n", b);
  printf("c = %hhd\n", c);
  printf("b = %hhd\n", b);
  printf("c = %hhu\n", c);
}
