/* libc like functions that are integrated into the main body of the program */

#define QCSV_NOZ80MALLOC
#include "querycsv.h"

/* variables needed by libc */
int myhand_status;
int newline;
int currentWaitCursor;
int cursorOutput;
int startOfLine;

/* variables needed by strtod */
const double fltMinusOne = -1.0;
const double fltOne = 1.0;
const double fltTen = 10.0;
void initMapper(void);

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

double strtod_z80(const char *str, char **end) {
  double d = ctof(0);
  int isNegative = 0, sign = 1;
  int n = 0;
  const char *p, *a;
  double f;
  double base;

  a = p = str;

  /*if(fltInited == 0) {
    fltSmall = pow10a(-309);
    fltInited = 1;
  }*/

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

char* dtoa_z80(char *s, double n) {
  char *p;
  ftoa(n, 32, s);

  /* rtrim the trailing zeros and decimal point */
  p = s + strlen(s) - 1;

  while(*p == '0') {
    *p = '\0';
    p--;
  }

  if(*p == '.') {
    *p = '\0';
  }

  return s;
}

void mallinit_z80(void) {
  myHeap.nextFree = myHeap.first = NULL;
}

void sbrk_z80(void *addr, unsigned int size) {
  if(addr == NULL || size < sizeof(struct heapItem)) {
    return;
  }

  next = (struct heapItem *)addr;
  next->next = NULL;
  next->size = size - sizeof(struct heapItem);
  next->type = HEAP_FREE;

  if(myHeap.first == NULL) {
    myHeap.nextFree = myHeap.first = next;
    return;
  }

  current = myHeap.first;

  /* Tack the new block onto the end of the linked list */
  while(current->next != NULL) {
    current = current->next;
  }

  current->next = next;
}

/* unused (for now) */
/*
void *calloc_z80(unsigned int num, unsigned int size) {
  unsigned int tot = num * size;
  void *temp;

  temp = malloc_z80(tot);

  if(temp) {
    memset(temp, 0, tot);
  }

  return temp;
}
*/
extern char **argv_z80;

void atexit_z80(void);

void setupZ80(int * argc, char *** argv) {
  static char length;

  /* initialise variables needed by z88dk's libc */
  myhand_status = 3;

  __asm
    	; Setup std* streams
      ld      hl,__sgoioblk+2
      ld      (hl),19 ;stdin
      ld      hl,__sgoioblk+12
      ld      (hl),21 ;stdout
      ld      hl,__sgoioblk+22
      ld      (hl),21 ;stderr
  __endasm;

  newline = FALSE;
  currentWaitCursor = 0;
  cursorOutput = FALSE;
  startOfLine = TRUE;


  unsigned char * test = ((unsigned char *)(0x0007));
  if(*test < 0x89) {
    fputs_z80("Not enough memory", stderr);
    exit(-1);
  }

  /* initialise the heap so malloc and free will work */
  mallinit_z80();
  sbrk_z80(0x8000, -0x8001 + (((*test)-2) << 8)); /* use 2 kb for the stack , the rest of memory above 0x8000 for the heap */

  /* reset the command line args and process them ourselves */
  int sizeNeeded = (*((char*)(0x0080)))+1;
  test = ((unsigned char *)(0x0081));

  int notInQuotes = TRUE;
  int maybeNewField = TRUE;
  int argc_z80 = 0;

  char i, j;

  /* cut up the string. Behaviour should be as described on "the old new thing" */
  if(sizeNeeded) {
    for(i = 0, j = 0; i < sizeNeeded; ) {
      switch(test[i]) {
        case '\\': {
          if(maybeNewField) {
            argc_z80++;
            maybeNewField = FALSE;
          }

          if(i+2 < sizeNeeded && test[i+1] == '\\' && test[i+2] == '"') {
            test[j] = '\\';
            i+=2;
            j++;
            }
          else if(i+1 < sizeNeeded && test[i+1] == '"') {
            test[j] = '"';
            i+=2;
            j++;
          }
          else {
            test[j] = '\\';
            i++;
            j++;
          }
        } break;

        case '\0':
        case '\x0d':
        case '\x0a':
        case ' ': {
          if(notInQuotes) {
             test[j] = '\0';
             maybeNewField = TRUE;
          }
          else {
            if(maybeNewField) {
              argc_z80++;
              maybeNewField = FALSE;
            }
            test[j] = ' ';
          }
          i++;
          j++;
        } break;

        case '"': {
          notInQuotes = !notInQuotes;
          test[i] = '\0';
          i++;
        } break;

        default: {
          if(maybeNewField) {
            argc_z80++;
            maybeNewField = FALSE;
          }
          test[j] = test[i];
          i++;
          j++;
        } break;
      }
    }
  }

  argc_z80++;

  if((argv_z80 = (char**)malloc_z80(sizeof(char*)*(argc_z80+1))) == NULL) {
    fputs_z80("Couldn't get command line", stderr);
    exit(EXIT_FAILURE);
  }

  initMapper();

  atexit(atexit_z80);

  /* don't bother cleaning up the heap as it isn't valid after the program exits anyway */
  argv_z80[0] = "querycsv";
  maybeNewField = TRUE;

  if(sizeNeeded) {
    for(i = 0, j = 1; i < sizeNeeded; i++) {
      if(test[i] == '\0') {
        maybeNewField = TRUE;
      }
      else if(maybeNewField) {
        argv_z80[j] = &(test[i]);
        maybeNewField = FALSE;
        j++;
      }
    }
  }

  argv_z80[argc_z80] = NULL;

  *argc = argc_z80;
  *argv = argv_z80;
}
