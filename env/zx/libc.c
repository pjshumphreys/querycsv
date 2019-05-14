/* fake program to get the necessary libc functions into 1 memory page */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

char * a = "%d";

void b(char * string, unsigned char * format, ...) {
  va_list args;
  va_list args2;

  FILE* test;
  int num;

  mallinit();
  sbrk(24000, 4000);

  string = malloc(1);
  fgets(string, 1, stdin);
  free(string);
  string = calloc(1, 3);
  string = realloc(string, 5);
  strcpy(string, a);
  fgets(string, 1, stdin);
  num = strcmp(a, string);
  num = strncmp(a, string, 3);
  num = strlen(string);
  string = strstr(string, a);

  memset(string, 0, 4);
  strcat(string, a);
  strncat(string, a, 3);
  memcpy(string+1, string, 2);
  memmove(string+1, string, 2);

  fprintf(test, a, 1);
  fputs(a, test);

  test = fopen(a, "rb");
  clearerr(test);
  num = fclose(test);
  fread(string, 2, 2, stdin);
  num = fgetc(stdin);
  ungetc(num, stdin);
  num = feof(stdin);
  fwrite(string, 1, 1, stdout);
  num = fgetc(stdin);
  fputc(num, stderr);
  fflush(stdout);
  sprintf(string, a, num);

  va_start(args, format);
  vsprintf(string, format, args);
  va_end(args);

  va_start(args2, format);
  vsnprintf(string, 2, format, args2);
  va_end(args2);

  free(string);
}

int main(void) {
  b(a, (unsigned char *)a);
  return 0;
}
