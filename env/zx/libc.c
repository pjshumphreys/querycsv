/* fake program to get the necessary libc functions into 1 memory page */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

const char pageBuf[256];  /* not really a constant but a buffer that exists in high memory that's only used internally to the libc functions */

__asm
  EXTERN mypager
  EXTERN defaultBank
__endasm;

/* don't bother to copy the mode parameter as for our purposes it will always be a static string */
FILE * zx_fopen(const char * filename, const char * mode) {
  int len;

  len = strlen(filename);

  if(len > 255) {
    return NULL;  /* very long filenames are not supported. 256 bytes strings should be big enough though */
  }

  memcpy(&pageBuf, filename, len + 1);
  return fopen(&pageBuf, mode);
}

size_t zx_fread(void * ptr, size_t size, size_t count, FILE * stream) {
  int tot2;
  union {
    int tot;
    struct {
      unsigned char remainder;  /* l */
      unsigned char loop;  /* h */
    } bytes;
  } temp;

  if(stream == stdin) {
    return fread(ptr, size, count, stream);
  }

  temp.tot = size * count;
  tot2 = 0;

  while(temp.bytes.loop) {
    tot2 += fread(&pageBuf, 1, 256, stream);

    /* page out esxdos */
    __asm
      push af
      ld a, (defaultBank)
      call mypager
      pop af
    __endasm;

    memcpy(ptr, &pageBuf, 256);

    ptr += 256;
    --(temp.bytes.loop);
  }

  tot2 += fread(&pageBuf, 1, (int)(temp.bytes.remainder), stream);

  /* page out esxdos */
  __asm
    push af
    ld a, (defaultBank)
    call mypager
    pop af
  __endasm;

  memcpy(ptr, &pageBuf, (int)(temp.bytes.remainder));

  return tot2;
}

size_t zx_fwrite(const void * ptr, size_t size, size_t count, FILE * stream) {
  union {
    int tot;
    struct {
      unsigned char remainder;
      unsigned char loop;
    } bytes;
  } temp;
  int tot2;
  temp.tot = size * count;

  /* if stream is stdout or stderr then just call fputc_cons ourselves.
  There are probably reasons why this wouldn't be correct in all general
  cases but it should work well enough for the purposes of this program */
  if(stream == stdout || stream == stderr) {
    tot2 = temp.tot;

    while(temp.tot--) {
      fputc_cons(*ptr++);
    }

    return tot2;
  }

  tot2 = 0;

  while(temp.bytes.loop) {
    memcpy(&pageBuf, ptr, 256);
    tot2 += fwrite(&pageBuf, 1, 256, stream);

    /* page out esxdos */
    __asm
      push af
      ld a, (defaultBank)
      call mypager
      pop af
    __endasm;

    ptr += 256;
    --(temp.bytes.loop);
  }

  memcpy(&pageBuf, ptr, (int)(temp.bytes.remainder));
  tot2 += fwrite(&pageBuf, 1, (int)(temp.bytes.remainder), stream);

  /* page out esxdos */
  __asm
    push af
    ld a, (defaultBank)
    call mypager
    pop af
  __endasm;

  return tot2;
}


void b(char * string, unsigned char * format, ...) {
  va_list args;
  va_list args2;
  double d;

  FILE* test;
  int num;

  num = atol(string);

  d = log10(d);
  d = floor(d);

  num = ((int)(d));
  /* string = malloc(1); */
  /* free(string); */
  /* string = calloc(1, 3); */
  /* string = realloc(string, 5); */
  abs(num);
  strcpy(string, origWd);
  strncpy(string, origWd, 3);
  num = strcmp(origWd, string);
  num = stricmp(origWd, string);
  num = strncmp(origWd, string, 3);
  num = strnicmp(origWd, string, 3);
  num = strlen(string);
  string = strstr(string, origWd);

  memset(string, 0, 4);
  strcat(string, origWd);
  strncat(string, origWd, 3);
  memcpy(string+1, string, 2);
  memmove(string+1, string, 2);

  /* fprintf(test, origWd, 1); */
  /* fputs(origWd, test); */

  /* test = fopen(origWd, "rb"); */
  fseek(test, 9, SEEK_SET);
  clearerr(test);
  num = fclose(test);
  /* fread(string, 2, 2, stdin); */
  num = fgetc(stdin);
  ungetc(num, stdin);
  num = feof(stdin);
  /* fwrite(string, 1, 1, stdout); */
  fputc(num, stderr);
  /* fflush(stdout); */
  sprintf(string, origWd, num);
  isspace(num);
  isdigit(num);

  va_start(args, format);
  vfprintf(string, format, args);
  va_end(args);

  va_start(args2, format);
  vsnprintf(string, 2, format, args2);
  va_end(args2);

  /* free(string); */
}

int main(int argc, char * argv[]) {
  /*
    mallinit();
    sbrk(24000, 4000);
  */

  origWd = "%d";
  b(origWd, (unsigned char *)origWd);

  return 0;
}
