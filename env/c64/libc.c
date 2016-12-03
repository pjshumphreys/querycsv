/* fake program to get the necessary libc functions into 1 memory page */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <locale.h>


#pragma rodataseg ("FAKERODATA")
#pragma dataseg ("FAKEDATA")
#pragma bssseg ("FAKEDATA")
  char* c = "C";
  char* pe = "C=C";
  char* fmt = "%d";

#pragma codeseg ("FOO")
int compar(const void * a, const void * b) {
  return 0;
}

int foobar(char *string, char* format, ...) {
  va_list args;
  va_list args2;

  FILE* test;
  int num;
  double fraction;
  time_t timer;
  struct tm * datetime;

  setlocale(1, c);
  putenv(pe);
  string = getenv(c);

  string = malloc(1);
  free(string);
  string = calloc(1,2);
  string = realloc(string,5);
  strcpy(string, c);
  num = strcmp(c, string);
  num = strncmp(c, string, 3);
  num = stricmp(c, string);
  num = strlen(string);
  string = strstr(string, c);

  /* fraction = strtod("C", &string); */
  memset(string, 0, 4);
  strcat(string, c);
  strncat(string, c, 2);
  memcpy(string+1, string, 2);
  memmove(string+1, string, 2);
  /*bsearch (string, string, 2, 2, compar);*/

  test = fopen(c,"rb");
  num = ferror(test);
  clearerr(test);
  num = fclose(test);
  fread(string, 2, 2, stdin);
  num = fgetc(stdin);
  ungetc(num, stdin);
  num = feof(stdin);
  fwrite(string, 1, 1, stdout);
  fputc(num, stderr);
  fputs(string, stdout);
  fprintf(stdout,fmt, num);
  fflush(stdout);
  sprintf(string,fmt, num);

  va_start(args, format);
  vsprintf(string, format, args);
  va_end(args);
  
  va_start(args2, format);
  vsnprintf(string, 2, format, args2);
  va_end(args2);
  
  /*
  time(&timer);
  datetime = gmtime(&timer);
  datetime = localtime(&timer);
  strftime(string, 2, "%d", datetime);
  */
  
  free(string);
  exit(0);  
}

int main(int argc, char**argv) {
  char *string;
  foobar(string, fmt);
  return 0;
}

/*strdup*/
