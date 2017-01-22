#include "querycsv.h"
#include <stdio.h>

long codepoints[18];
struct hash2Entry entry = {0x0000, 1, (long *)&codepoints};
struct hash2Entry* retval;
int i;

int combiningCharCompare(const void *a, const void *b) {
  MAC_YIELD
  
  return (*((long*)a) < *((long*)b)) ? -1 : (*((long*)a) != *((long*)b) ? 1 : 0);
}


int main(int argc, char *argv[]) {
  printf("hello");
  return 0;
}
