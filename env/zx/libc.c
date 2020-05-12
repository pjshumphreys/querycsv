/* fake program to get the necessary libc functions into 1 memory page */
#include "querycsv.h"

const double fltMinusOne = -1.0;
const double fltOne = 1.0;
const double fltTen = 10.0;
extern double fltSmall;
extern int fltInited;
extern long heap;
extern char * origWd; /* The dummy functions used to pull the clib functions
  into the binary need to refer to a char *, but we don't want to declare one
  that'll never be used. Reuse one that already exists */

double pow10a(int exp) {
  int sign = 0;
  double result = fltTen;

  if(exp == 0) {
    return fltOne;
  }

  if(exp < 0) {
    sign = 1;
    exp = abs(exp);
  }

  while(--exp) {
    result = fmul(fltTen, result);
  }

  if(sign) {
    result = fdiv(fltOne, result);
  }

  return result;
}

/*
 * strtod implementation.
 * original author: Yasuhiro Matsumoto
 * tidied up and modified to use soft float macros: Paul Humphreys
 * license: public domain
 */

double strtod(const char *str, char **end) {
  double d = ctof(0);
  int isNegative = 0, sign = 1;
  int n = 0;
  const char *p, *a;
  double f;
  double base;

  a = p = str;

  if(fltInited == 0) {
    fltSmall = pow10a(-309);
    fltInited = 1;
  }

  /*skip white space */
  while(isspace(*p)) {
    ++p;
  }

  for(;;) {
    /* decimal part */
    if(*p == '-') {
      isNegative = !isNegative;
      ++p;
      continue;
    }
    else if(*p == '+') {
      ++p;
      continue;
    }

    if(isdigit(*p)) {
      d = ctof(*p - '0');
      ++p;

      while(*p && isdigit(*p)) {
        d = fadd(fmul(d, fltTen), ctof(*p - '0'));
        ++p;
        ++n;
      }

      a = p;
    }
    else if(*p != '.') {
      break;
    }

    if(isNegative) {
      d = fmul(d, fltMinusOne);
    }

    /* fraction part */
    if(*p == '.') {
      f = ctof(0);
      base = fdiv(fltOne, fltTen);

      ++p;

      if(isdigit(*p)) {
        while(*p && isdigit(*p)) {
          f = fadd(f, fmul(base, ctof(*p - '0')));
          base = fdiv(base, fltTen);
          ++p;
          ++n;
        }
      }

      d = (isNegative ? fsub(d, f) : fadd(d, f));
      a = p;
    }

    /* exponential part */
    if((*p == 'E') || (*p == 'e')) {
      int e = 0;
      ++p;

      if(*p == '-') {
        sign = -1;
        ++p;
      }
      else if(*p == '+') {
        ++p;
      }

      if(isdigit(*p)) {
        while(*p == '0') {
          ++p;
        }

        e = (int)(*p++ - '0');

        while(*p && isdigit(*p)) {
          e = (e * 10) + (int)(*p - '0');
          ++p;
        }

        e = e * sign;
      }
      else if(!isdigit(*(a-1))) {
        a = str;
        break;
      }
      else if(*p == 0) {
        break;
      }

      /*
      if(e <= -308) {
        if(e == -308 && feq(d, 2.2250738585072011)) {
          d = ctof(0);
          a = p;
          errno = ERANGE;
          break;
        }
        else if(feq(d, 2.2250738585072012)) {
          d = fmul(d, fltSmall);
          a = p;
          break;
        }
      }
      */

      d = fmul(d, pow10a(e));
      a = p;
    }
    else if(p > str && !isdigit(*(p-1))) {
      a = str;
      break;
    }

    break;
  }

  if(end) {
    *end = (char*)a;
  }

  return d;
}
/*
void test(char* str) {
  double d1, d2;
  char *e1, *e2;
  int x1, x2;

  printf("CASE: %s\n", str);

  errno = 0;
  e1 = NULL;
  d1 = mystrtod(str, &e1);
  x1 = errno;

  errno = 0;
  e2 = NULL;
  d2 = mystrtod(str, &e2);
  x2 = errno;

  if(!feq(d1, d2) || e1 != e2 || x1 != x2) {
    printf("  ERR: %s, %s\n", str, strerror(errno));
    printf("    E1 %f, %g, %s, %d\n", d1, d1, e1 ? e1 : "", x1);
    printf("    E2 %f, %g, %s, %d\n", d2, d2, e2 ? e2 : "", x2);

    if(!feq(d1, d2)) {
      puts("different value");
    }

    if(e1 != e2) {
      puts("different end position");
    }

    if (x1 != x2) {
      puts("different errno");
    }
  }
  else {
    printf("  SUCCESS [%f][%s]: %s\n", d1, e1 ? e1 : "");
  }

  printf("\n");
}

int main(int argc, char* argv[]) {
  test(".1");
  test("  .");
  test("  1.2e3");
  test(" +1.2e3");
  test("1.2e3");
  test("+1.2e3");
  test("+1.e3");
  test("-1.2e3");
  test("-1.2e3.5");
  test("-1.2e");
  test("--1.2e3.5");
  test("--1-.2e3.5");
  test("-a");
  test("a");
  test(".1e");
  test(".1e3");
  test(".1e-3");
  test(".1e-");
  test(" .e-");
  test(" .e");
  test(" e");
  test(" e0");
  test(" ee");
  test(" -e");
  test(" .9");
  test(" ..9");
  test("009");
  test("0.09e02");
  / * http://thread.gmane.org/gmane.editors.vim.devel/19268/ * /
  test("0.9999999999999999999999999999999999");
  test("2.2250738585072010e-308"); / * BUG * /
  / * PHP (slashdot.jp): http://opensource.slashdot.jp/story/11/01/08/0527259/PHP%E3%81%AE%E6%B5%AE%E5%8B%95%E5%B0%8F%E6%95%B0%E7%82%B9%E5%87%A6%E7%90%86%E3%81%AB%E7%84%A1%E9%99%90%E3%83%AB%E3%83%BC%E3%83%97%E3%81%AE%E3%83%90%E3%82%B0 * /
  test("2.2250738585072011e-308");
  / * Gauche: http://blog.practical-scheme.net/gauche/20110203-bitten-by-floating-point-numbers-again * /
  test("2.2250738585072012e-308");
  test("2.2250738585072013e-308"); / * Hmm. * /
  test("2.2250738585072014e-308"); / * Hmm. * /
}
*/

const int main_origins[6] = {
  0,
  16384,  /* plus3dos */
  0,      /* residos48 */
  0,      /* residos128 */
  8192,   /* esxdos48 */
  8192    /* esxdos128 */
};

const int main_sizes[6] = {
  0,
  6911, /* page 5 isn't used as we'll be switched to the second screen during runtime */
  16384,
  23295,  /* 16384 + 6911 */
  8192,
  15103 /* 8192 + 6911 */
};

void setupZX(char * filename) __z88dk_fastcall {
  int start;

  /* initialise variables needed by z88dk's libc */
  myhand_status = 3;

  /* initialise the heap so malloc and free will work */
  mallinit();
  memset(main_origins[libCPage], 0, main_sizes[libCPage]); /* lib c variant specific free ram. All variants permit at least some */
  sbrk(main_origins[libCPage], main_sizes[libCPage]); /* lib c variant specific free ram. All variants permit at least some */

  if(filename != NULL) {
    start = (int)(filename) + strlen(filename) + 40;
    memset(start, 0, 44032 /* 0xc000 - 5kb */ - start); /* free ram from the end of the a$ variable up to the paging code minus about 2 kb for stack space */
    sbrk(start, 44032 /* 0xc000 - 5kb */ - start); /* free ram from the end of the a$ variable up to the paging code minus about 2 kb for stack space */
  }
}

void b(char * string, unsigned char * format, ...) {
  va_list args;
  va_list args2;

  FILE* test;
  long num;

  num = atol(string);

  string = malloc(1);
  fgets(string, 1, stdin);
  free(string);
  string = calloc(1, 3);
  string = realloc(string, 5);
  strcpy(string, origWd);
  strncpy(string, origWd, 3);
  fgets(string, 1, stdin);
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

  fprintf(test, origWd, 1);
  fputs(origWd, test);

  test = fopen(origWd, "rb");
  fseek(test, 9, SEEK_SET);
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
  sprintf(string, origWd, num);

  va_start(args, format);
  vsprintf(string, format, args);
  va_end(args);

  va_start(args2, format);
  vsnprintf(string, 2, format, args2);
  va_end(args2);

  free(string);
}

int main(int argc, char * argv[]) {
  mallinit();
  sbrk(24000, 4000);

  origWd = "%d";
  b(origWd, (unsigned char *)origWd);

  return 0;
}
