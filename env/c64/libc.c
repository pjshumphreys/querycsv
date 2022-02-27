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
  fgets(string, 1, stdin);
  free(string);
  string = realloc(string,5);
  strcpy(string, c);
  fgets(string,1,stdin);
  num = strcmp(c, string);
  num = strncmp(c, string, 3);
  num = strlen(string);
  string = strstr(string, c);

  memset(string, 0, 4);
  strcat(string, c);
  strncat(string, c, 3);
  memcpy(string+1, string, 2);
  memmove(string+1, string, 2);

  chdir(string);
  strchr(string, ',');
  test = fopen(c,"rb");
  isdigit('8');
  num = ferror(test);
  clearerr(test);
  num = fclose(test);
  fread(string, 2, 2, stdin);
  num = fgetc(stdin);
  ungetc(num, stdin);
  num = feof(stdin);
  fwrite(string, 1, 1, stdout);
  num = fgetc(stdin);
  fflush(stdout);
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
