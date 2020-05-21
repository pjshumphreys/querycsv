/* fake program to get the necessary libc functions into 1 memory page */
#include <math.h>

#define QCSV_NOZXMALLOC
#include "querycsv.h"

struct heapItem {
  struct heapItem * next; /* where the next block is, 0 for no next block */
  unsigned int size; /* how many bytes are contained in this block, not including these 5 header bytes */
  unsigned char type; /* 0 = free, 1 = allocated */
};

struct heapInternal {
  struct heapItem * nextFree;
  struct heapItem * first;
};

/* variables needed by libc */
int myhand_status;

/* variables needed by strtod */
const double fltMinusOne = -1.0;
const double fltZero = 0.0;
const double fltOne = 1.0;
const double fltTen = 10.0;
/* double fltSmall;
int fltInited;*/

struct heapInternal myHeap;
struct heapItem * current;
struct heapItem * next;

#define HEAP_FREE 0
#define HEAP_ALLOCED 1

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

const char digitLookup[11] = "0123456789";

extern int stkend;

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

double zx_strtod(const char *str, char **end) {
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

char* zx_dtoa(char *s, double n) {
  char *c;
  char *c2;
  int sign, m, m1, carry = 0;
  int useExp, output = 0, decimalPoint = 0;
  double weight, digit;
  int i, j;

  /*if(fltInited == 0) {
    fltSmall = pow10a(-309);
    fltInited = 1;
  }*/

  /* handle special cases */
  if(isnan(n)) {
    strcpy(s, "NaN");
    return s;
  }

  if(isinf(n)) {
    strcpy(s, "Infinity");
    return s;
  }

  if(feq(n, fltZero)) {
    strcpy(s, "0");
    return s;
  }

  c = s;
  sign = 0;

  if(!fgt(n, fltZero)) {
    n = fmul(n, fltMinusOne);
    sign = 1;
    *(c++) = '-';
    s++;
  }

  /* calculate magnitude */
  m = ftoc(floor(log10(n)));
  m1 = abs(m);
  useExp = m1 >= 9 && (sign || m1 >= 14);

  /* set up for scientific notation */
  if(useExp) {
    n = fdiv(n, pow10a(m));
    m1 = m;
    m = 0;
  }

  if(m < 0) {
    m = 0;
  }

  /* convert the number */
  while(fgt(n, fltZero) || m >= 0) {
    weight = pow10a(m);

    if(fgt(weight, fltZero)) {
      digit = floor(fdiv(n, weight));
      if(output > 13 && m < 0) {
        *c = '\0';
        carry = ftoc(digit) > 4;
        break;
      }

      n = fsub(n, fmul(digit, weight));
      if(output || fgt(digit, fltZero) || m == 0) {
        *(c++) = digitLookup[ftoc(digit)];
        output++;
      }
    }

    if(m == 0) {
      *(c++) = '.';
      decimalPoint = 1;
    }

    m--;
  }

  /* do any rounding needed on the characters directly */
  if(carry) {
    c2 = c;
    c--;

    while(carry && c != s) {
      if(*c != '.') {
        if(*c == '9') {
          *c = '0';
        }
        else {
          *c = digitLookup[strchr(digitLookup, *c)-digitLookup+1];
          carry = 0;
        }
      }

      c--;
    }

    if(carry) {
      if(*c == '9') {
        *c = '0';
        memmove(c+1, c, strlen(c)+1);
        *c = '1';
      }
      else {
        *c = digitLookup[strchr(digitLookup, *c)-digitLookup+1];
      }
    }

    c = c2;
  }

  /* remove trailing zeros, with the decimal point as well if need be */
  if(decimalPoint) {
    c--;
    for(;;) {
      if(*c == '0') {
        *(c--) = 0;
        continue;
      }

      if(*c == '.') {
        *(c--) = 0;
      }

      break;
    }
    c++;
  }

  if(useExp) {
    /* convert the exponent */
    *(c++) = 'e';

    if(m1 >= 0) {
      *(c++) = '+';
    }
    else {
      *(c++) = '-';
      m1 = -m1;
    }

    m = 0;

    while(m1 > 0) {
      *(c++) = digitLookup[m1 % 10];
      m1 /= 10;
      m++;
    }

    c -= m;

    for(i = 0, j = m-1; i<j; i++, j--) {
      /* swap without temporary */
      c[i] ^= c[j];
      c[j] ^= c[i];
      c[i] ^= c[j];
    }

    c += m;
  }

  *c = '\0';

  if(sign) {
    s--;
  }

  return s;
}

void zx_mallinit(void) {
  myHeap.nextFree = myHeap.first = NULL;
}

void zx_sbrk(void *addr, unsigned int size) {
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

void *zx_malloc(unsigned int size) {
  unsigned int cleanedUp;
  unsigned int temp;

  cleanedUp = FALSE;

  /* for larger allocations, try to overlay an exact match starting from the beginning of the heap */
  if(size > 255) {
    current = myHeap.first;

    do {
      if(current == NULL) {
        break;
      }

      if(current == myHeap.nextFree) {
        break;
      }

      if(current->type == HEAP_FREE && current->size == size) {
        current->type = HEAP_ALLOCED;

        return (void *)current + sizeof(struct heapItem);
      }

      current = current->next;
    } while(1);
  }

  temp = size + sizeof(struct heapItem);

  do {
    /* no free memory available. just quit */
    if(myHeap.nextFree == NULL) {
      return NULL;
    }

    /* find a suitable location the put the new data, starting at myHeap.nextFree */
    current = myHeap.nextFree;

    do {
      if(current->type == HEAP_FREE && current->size >= temp) {
        /* suitable location found, set up the headers then return the pointer */
        next = (struct heapItem *)((void*)current + temp);
        next->next = current->next;
        next->size = current->size - temp;
        next->type = HEAP_FREE;

        current->size = size;
        current->type = HEAP_ALLOCED;
        current->next = next;

        myHeap.nextFree = next;

        return (void *)current + sizeof(struct heapItem);
      }

      current = current->next;
    } while(current);

    /* if no suitable free position was found and the heap has already been cleaned up then fail */
    if(cleanedUp) {
      return NULL;
    }

    myHeap.nextFree = NULL;
    current = myHeap.first;

    /* Attempt to coalesce the free blocks together then try again, but only once */
    while(current != NULL) {
      next = current->next;

      if(current->type == HEAP_FREE) {
        if(myHeap.nextFree == NULL) {
          myHeap.nextFree = current;
        }

        if(
          next && next->type == HEAP_FREE &&
          (struct heapItem *)(((void *)current) + sizeof(struct heapItem) + current->size) == next
        ) {
          current->next = next->next;
          current->size += next->size + sizeof(struct heapItem);
          continue;
        }
      }

      current = next;
    }

    cleanedUp = TRUE;
  } while (1);
}

void zx_free(void *addr) {
  if(addr == NULL) {
    return;
  }

  current = ((struct heapItem *)(addr - sizeof(struct heapItem)));
  current->type = HEAP_FREE;

  /* try to keep the next available free block as unfragmented as possible */
  if(current->next != myHeap.nextFree) {
    return;
  }

  if(NULL == myHeap.nextFree) {
    return;
  }

  next = (struct heapItem *)(addr + current->size);

  if(next != myHeap.nextFree) {
    return;
  }

  current->next = next->next;
  current->size += next->size + sizeof(struct heapItem);
  myHeap.nextFree = current;
}

void *zx_realloc(void *p, unsigned int size) {
  void * newOne;
  unsigned int tempSize;
  unsigned int updateNextFree;

  /* if realloc'ing a null pointer then just do a malloc */
  if(p == NULL) {
    return zx_malloc(size);
  }

  current = (struct heapItem *)(p - sizeof(struct heapItem));

  next = current->next;

  /* Is the existing block adjacent to a free one with enough
    total space? if so then just resize it and return the existing block */
  if(
    next != NULL &&
    next->type == HEAP_FREE &&
    (struct heapItem *)(((void *)current) + sizeof(struct heapItem) + current->size) == next
  ) {
     /* get the total amount of memory available in this interval */
    tempSize = current->size + next->size;

    if(tempSize >= size) {
      tempSize -= size;

      /* if the nextFree block is the same one as the free one we're updating, update the pointer as well */
      updateNextFree = (next == myHeap.nextFree);

      /* remove the old free block from the linked list as we'll be making a new one */
      current->next = next->next;

      /* update the current block's size to its new value */
      current->size = size;

      next = (struct heapItem *)(((void *)current) + sizeof(struct heapItem) + size);
      next->next = current->next;
      next->size = tempSize;
      next->type = HEAP_FREE;

      current->next = next;

      if(updateNextFree) {
        myHeap.nextFree = next;
      }

      return p;
    }
  }

  /* attempt to allocate a new block of the necessary size, memcpy the data into it then free the old one */
  newOne = zx_malloc(size);

  /* if the malloc failed, just fail here as well */
  if(!newOne) {
    return NULL;
  }

  /* memcpy the data if necessary */
  tempSize = size;

  if(tempSize > current->size) {
    tempSize = current->size;
  }

  if(tempSize) {
    memcpy(newOne, p, tempSize);
  }

  /* free the old data */
  current->type = HEAP_FREE;

  /* return a pointer to the new data */
  return newOne;
}

void *zx_calloc(unsigned int num, unsigned int size) {
  size_t tot;
  void *temp;

  tot = fmul(num, size);
  temp = zx_malloc(tot);

  if(temp) {
    memset(temp, 0, tot);
  }

  return temp;
}

int zx_fprintf(FILE *stream, char *format, ...) __stdc {
  size_t newSize;
  char *newStr;
  va_list args;

  /* Check sanity of inputs */
  if(format == NULL) {
    return FALSE;
  }

  /* if the stream is stdout or stderr just do a normal printf as the esxdos
    paging won't come into play therefore we don't need to do the formatting twice */
  if(stream == stdout || stream == stderr) {
    va_start(args, format);
    newSize = (size_t)(vfprintf(stream, format, args));
    va_end(args);

    return newSize;
  }

  newStr = NULL;

  /* get the space needed for the new string */
  va_start(args, format);
  newSize = (size_t)(vsnprintf(NULL, 0, format, args)); /* plus '\0' */
  va_end(args);

  /* Create a new block of memory with the correct size rather than using realloc */
  /* as any old values could overlap with the format string. quit on failure */
  if((newStr = (char*)zx_malloc(newSize + 1)) == NULL) {
    return FALSE;
  }

  /* do the string formatting for real. */
  va_start(args, format);
  vsnprintf(newStr, newSize + 1, format, args);
  va_end(args);

  zx_fwrite(newStr, 1, newSize, stream);

  zx_free(newStr);

  return newSize;
}

int zx_fputs(const char * str, FILE * stream) {
  return zx_fwrite(str, 1, strlen(str), stream);
}

void setupZX(char * filename) __z88dk_fastcall {
  int start;

  /* initialise variables needed by z88dk's libc */
  myhand_status = 3;

  /* initialise the heap so malloc and free will work */
  zx_mallinit();
  memset(main_origins[libCPage], 0, main_sizes[libCPage]);
  zx_sbrk(main_origins[libCPage], main_sizes[libCPage]); /* lib c variant specific free ram. All variants permit at least some */

  if(filename != NULL) {
    start = stkend + 1;
    memset(start, 0, 44032 /* 0xc000 - 5kb */ - start);
    zx_sbrk(start, 44032 /* 0xc000 - 5kb */ - start); /* free ram from the end of the a$ variable up to the paging code minus about 2 kb for stack space */
  }
}

/*
void b(char * string, unsigned char * format, ...) {
  va_list args;
  va_list args2;

  FILE* test;
  int num;

  num = atol(string);

  /* string = malloc(1); * /
  /* free(string); * /
  /* string = calloc(1, 3); * /
  /* string = realloc(string, 5); * /
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

  /* free(string); * /
}

int main(int argc, char * argv[]) {
  /*
    mallinit();
    sbrk(24000, 4000);
  * /

  origWd = "%d";
  b(origWd, (unsigned char *)origWd);

  return 0;
}
*/
