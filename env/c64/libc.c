/* fake program to get the necessary libc functions into 1 memory page */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#pragma rodata-name ("FAKERODATA")
#pragma data-name ("FAKEDATA")
#pragma bss-name ("FAKEDATA")
#pragma code-name ("FOO")
  char* c = "%d";

void foobar(char *string, char* format, ...) {
  va_list args;
  va_list args2;

  FILE* test;
  int num;

  string = malloc(1);
  free(string);
  string = realloc(string, 5);
  strcpy(string, c);
  num = strcmp(c, string);
  num = strncmp(c, string, 3);
  num = strlen(string);
  string = strstr(string, c);
  strchr(string, ',');

  memset(string, 0, 4);
  strcat(string, c);
  strncat(string, c, 3);
  memcpy(string+1, string, 2);
  memmove(string+1, string, 2);

  chdir(string);
  isdigit('8');
  test = fopen(c, string);
  num = ferror(test);
  clearerr(test);
  num = feof(test);
  num = fclose(test);
  fflush(stdout);
  fread(string, 2, 2, stdin);
  num = fgetc(stdin);
  ungetc(num, stdin);
  fwrite(string, 1, 1, stdout);
  sprintf(string, c, num);

  va_start(args, format);
  vsprintf(string, format, args);
  va_end(args);

  va_start(args2, format);
  vsnprintf(string, 2, format, args2);
  va_end(args2);

  free(string);
}

int main(void) {
  foobar(NULL, c);
  return 0;
}
