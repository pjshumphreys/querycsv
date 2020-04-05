/* variables that shouldn't be placed on the stack, so we make them global instead */

#ifdef __Z88DK
  long heap;  /* used by z88dk's malloc and free functions */
  int myerrno;  /* errno will be overlaid on top of this variable by the build script, so errno2 itself will not be used directly */

  /* variables needed by stdtod */
  double fltSmall;
  int fltInited;

  /* hash2 helper variables */
  long codepoints[18];

  struct hash2Entry entry = {0x0000, 1, &codepoints};
#else
  /* hash2 helper variables */
  long codepoints[18];

  struct hash2Entry entry = {0x0000, 1, (long *)(&codepoints)};
#endif
struct hash2Entry * retval;

/* hash4 helper varables */
struct hash4Entry hash4export = { NULL, 0, 0, 0 };
struct hash4Entry entry1Internal = { NULL, 0, 0, 0 };
struct hash4Entry entry2Internal = { NULL, 0, 0, 0 };

char * devNull = NULL;
char * origWd = NULL;

/* hash1 helper variables */
int i;

/*the return value from the getBytes functions. bodged to make the c64 build work */
char returnByte;

/*dedup string literals */
const char * TDB_COULDNT_OPEN_INPUT = TDB_COULDNT_OPEN_INPUT2;
const char * TDB_MALLOC_FAILED = TDB_MALLOC_FAILED2;

#ifdef __CC65__
char * fopen_read = "RB";
char * fopen_write = "WB";
#else
#define fopen_read "rb"
#define fopen_write "wb"
#endif


