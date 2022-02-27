/* Just a simple program to test what the dos in use thinks is in a file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  char * temp;
  FILE * out;
  int c;

  temp = malloc(256);

  printf("Please enter a filename:\n");
  memset(temp, 0, 256);
  scanf("%255s", temp);

  if((out = fopen(temp, "rb")) == NULL) {
    printf("Counldn't open file\n");
  }

  while((c = fgetc(out)) != EOF) {
    printf("%d ", c);
  }

  printf("EOF\n");

  fclose(out);
  free(temp);
  return 0;
}
