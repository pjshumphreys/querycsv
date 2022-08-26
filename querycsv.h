#ifndef QUERYCSV_H
#define QUERYCSV_H 1

#define _XOPEN_SOURCE_EXTENDED

/* standard lib headers */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#if !defined(FLEXINT_H) && (defined(__Z88DK) || __STDC_VERSION__ >= 199901L)
  #include <stdint.h>
#else
  /* Old versions of Norcroft appear to generate incorrect codes with short integers.
  Just use long ones for that compiler */
  #define QCSV_SHORT unsigned short
  #define QCSV_LONG long
#endif

#ifndef QCSV_SHORT
  #define QCSV_SHORT uint16_t
  #define QCSV_LONG int32_t
#endif

/* translatable strings */
#include "en_gb.h"

#define YY_EXTRA_TYPE struct qryData*
#define ECHO 1 /* disables flex from outputing unmatched input */
#define FALSE 0
#define TRUE  1
#define MYEOF -1
#define PATH_SEPARATOR '/'

/* sub expression types */
#define EXP_COLUMN 1
#define EXP_LITERAL 2
#define EXP_CALCULATED 3
#define EXP_PLUS 4
#define EXP_MINUS 5
#define EXP_MULTIPLY 6
#define EXP_DIVIDE 7
#define EXP_SLICE 8
#define EXP_LIMITS 9
#define EXP_CASE 10
#define EXP_CONCAT 11
#define EXP_UPLUS 12
#define EXP_UMINUS 13
#define EXP_AND 14
#define EXP_OR 15
#define EXP_NOT 16
#define EXP_EQ 17
#define EXP_NEQ 18
#define EXP_LT 19
#define EXP_GT 20
#define EXP_LTE 21
#define EXP_GTE 22
#define EXP_IN 23
#define EXP_NOTIN 24
#define EXP_GROUP 25
#define EXP_ISNULL 26
#define EXP_NOTNULL 27
#define EXP_ROWNUMBER 28

/* identifier reference types */
#define REF_COLUMN 1
#define REF_EXPRESSION 2

/* grouping types */
#define GRP_NONE 0
#define GRP_AVG 1
#define GRP_MIN 2
#define GRP_MAX 3
#define GRP_SUM 4
#define GRP_COUNT 5
#define GRP_CONCAT 6
#define GRP_ROWNUMBER 7
#define GRP_STAR 8
#define GRP_DIS_AVG 9   /* DIS = Distinct */
#define GRP_DIS_MIN 10
#define GRP_DIS_MAX 11
#define GRP_DIS_SUM 12
#define GRP_DIS_COUNT 13
#define GRP_DIS_CONCAT 14

/* output parameters. Now specified as part of the input grammar */
#define PRM_BOM 1    /* output a utf-8 byte order mark before the file contents. UTF-16 and 32 always get a BOM */
#define PRM_UNIX 2    /* put unix newlines in the output file */
#define PRM_MAC 4    /* put mac newlines in the output file */
#define PRM_SPACE 8   /* put a space before each column value that's not the first into the output file */
#define PRM_QUOTE 16  /* always double quote non null values on export */
#define PRM_TASWORD 32  /* pad newlines and EOF with spaces if outputting to tasword format. Uses extra space but makes editing easier */
#define PRM_INSERT 64  /* insert a new line after 64 non newline codepoints on the output file. Used to convert tasword 2 files to something more readable */
#define PRM_REMOVE 128  /* remove a new line codepoint sequence when it starts on the 65th codepoint on a line in the output file. */

/* input or input/output parameters */
#define PRM_HEADER 256  /* skip the first 128 bytes in a file on input. Used if a file has a plus3dos header */
#define PRM_TRIM 512    /* left trim and right trim whitespace from each column value on import */
#define PRM_BLANK 1024  /* import/export unquoted empty strings as NULL */
#define PRM_NULL 2048  /* import/export unquoted text NULL as NULL */
#define PRM_POSTGRES 4096  /* import/export using postgres text files */
#define PRM_EURO 8192  /* import/export with ";" as delimiter rather than "," */

#define PRM_DEFAULT PRM_BLANK  /* default parametters */

#define TRE_BLACK 1
#define TRE_RED 2
#define TRE_CONVERTED 3
#define TRE_SKIP 4
#define isRed(x) (x != NULL && x->type == TRE_RED)

/* short codes for the character encodings we want to support */
#define ENC_UNKNOWN 0
#define ENC_CP437 1
#define ENC_CP850 2
#define ENC_CP1252 3
#define ENC_MAC 4
#define ENC_DEFAULT 5
#define ENC_UNSUPPORTED 6
#define ENC_ASCII 7
#define ENC_UTF8 8
#define ENC_UTF16LE 9
#define ENC_UTF16BE 10
#define ENC_UTF32LE 11
#define ENC_UTF32BE 12
#define ENC_MBCS 13
#define ENC_CP1047 14
#define ENC_ATARIST 15
#define ENC_PETSCII 16  /* Files in the petscii encoding are treated as always having a two byte header/bom, which is set to 0x01, 0x08 for written files */
#define ENC_BBC 17
#define ENC_ZX 18
/* Tasword 2 file format. Same as the ZX (spectrum) character
 * set but with hard-coded line lengths of 64 characters.
 * Newlines and EOF are stored as graphics characters and space
 * padded as necessary. Files also have a maximum size constraint
 * which will cause the program to abort if exceeded */
#define ENC_TSW 19

#define ENC_INPUT ENC_UTF8  /* used when a file is read */
#define ENC_OUTPUT ENC_UTF8 /* used when a file is written */
#define ENC_PRINT ENC_UTF8  /* used when outputting to the screen */

/* macro used to help prevent double freeing of heap data */
#define freeAndZero(p) { free(p); p = 0; }

#define myltoa(x, y) sprintf((char *)(x), LD_STRING, (y))

/* this macro was intended to facilitate multi tasking on classic mac os,
but hasn't actually been needed up to now. It's being kept just in case
it becomes needed and because it's useful for debugging */
#define MAC_YIELD

#if defined(__unix__) || defined(__LINUX__)
  #ifdef EMSCRIPTEN
    #include <setjmp.h>     /* jmp_buf, setjmp, longjmp */
    #include <errno.h>
    #include <sys/types.h>
    #include <sys/stat.h>

    #include <dirent.h>
    #include <fcntl.h>
    #include <ftw.h>
    #include <emscripten.h>

    void exit_emcc(int status);

    #define main realmain
    #define exit exit_emcc
  #endif

  #ifndef __WATCOMC__
    int vsnprintf(char *s, size_t n, const char *format, va_list arg);
    #define HAS_VSNPRINTF   /* this function intentionally doesn't work
    properly on watcom (to have source compatability with the windows version) */
  #endif

  #include <unistd.h> /* for chdir and getcwd */
#endif

#ifdef MICROSOFT
  #define YY_NO_UNISTD_H 1

  #ifdef WINDOWS
    void setupWin32(int * argc, char *** argv);
    size_t fwrite_w32(const void * ptr, size_t size, size_t count, FILE * stream);
    int fputs_w32(const char *str, FILE *stream);
    int fprintf_w32(FILE *stream, const char *format, ...);
    int chdir_w32(const char *dirname);
    FILE *fopen_w32(const char *filename, const char *mode);

    #define chdir chdir_w32
    #define fopen fopen_w32
    #define fwrite fwrite_w32
    #define fputs fputs_w32
    #define fprintf fprintf_w32
    #define YYFPRINTF fprintf_w32   /* for the bison parser */
  #else
    #ifdef __TURBOC__
      #define PATH_MAX 256
      #include <dir.h>   /* for chdir and getcwd */
      typedef long int32_t;
      /* these two functions have a different name in turbo c */
      #define _getdrive getdisk
      #define _chdrive setdisk
    #else
      #include <direct.h>   /* for chdir and getcwd */
    #endif

    extern int consoleEncoding;
    #undef ENC_PRINT
    #define ENC_PRINT consoleEncoding

    void setupDos(void);
    void atexit_dos(void);
    size_t fwrite_dos(const void * ptr, size_t size, size_t count, FILE * stream);
    int fputs_dos(const char *str, FILE *stream);
    int fprintf_dos(FILE *stream, const char *format, ...);

    #ifdef DOS_DAT
      void openDat(void);
    #endif

    #define fputs fputs_dos
    #define fwrite fwrite_dos
    #define fprintf fprintf_dos
    #define YYFPRINTF fprintf_dos   /* for the bison parser */

    #undef ENC_INPUT
    #undef ENC_OUTPUT
    #define ENC_INPUT ENC_CP437
    #define ENC_OUTPUT ENC_CP437
    /* ENC_PRINT is kept as utf-8 as we do the charset conversion in the fputs/fprintf wrapper functions now */
  #endif
#endif

#ifdef MPW_C
  #define YY_NO_UNISTD_H 1

  #ifdef RETRO68
    #define HAS_VSNPRINTF
    void fsetfileinfo_absolute(
      const char *filename,
      unsigned long newcreator,
      unsigned long newtype
    );
  #else
    #define fsetfileinfo_absolute fsetfileinfo
  #endif

  /* Mac style newlines by default */
  #undef PRM_DEFAULT
  #define PRM_DEFAULT PRM_BLANK | PRM_MAC

  size_t fwrite_mac(const void * str, size_t size, size_t count, FILE * stream);
  int fputs_mac(const char *str, FILE *stream);
  int fprintf_mac(FILE *stream, const char *format, ...);
  #define fwrite fwrite_mac
  #define fputs fputs_mac
  #define fprintf fprintf_mac
  #define YYFPRINTF fprintf_mac   /* for the bison parser */

  #define main realmain /*macs need to do pre and post handling, but
  SIO (which I previously used) doesn't seem to work with Carbon */

  #undef ENC_INPUT
  #undef ENC_OUTPUT

  void macYield(void);
  #undef MAC_YIELD
  #define MAC_YIELD macYield();

  #if TARGET_API_MAC_CARBON
    #define ENC_INPUT ENC_MAC
    #define ENC_OUTPUT ENC_UTF16BE
  #else
    #define ENC_INPUT ENC_MAC
    #define ENC_OUTPUT ENC_MAC
  #endif
  /* ENC_PRINT is kept as utf-8 as we do the charset conversion in the fputs/fprintf wrapper functions now */

  void exit_mac(int dummy);
  #define exit exit_mac
#endif

#ifdef __CC_NORCROFT
  #undef PATH_SEPARATOR
  #define PATH_SEPARATOR '.'

  #if __LIB_VERSION < 300
    /* doesn't do well with 16 bit data types, so use the 32 bit ones all the time */
    #undef QCSV_SHORT
    #define QCSV_SHORT long
    #define YYTYPE_UINT16 unsigned int
    #define YYTYPE_INT16 int

    /* These aren't in norcroft version 2's stdlib.h */
    #define EXIT_FAILURE 1
    #define EXIT_SUCCESS 0

    /* manually define the errno values that lexer uses as errno.h doesn't
       exist in version 2 but lexer.c uses these defines */
    #define EINTR  4    /* Interrupted system call */
    #define ENOMEM 12   /* Cannot allocate memory */
    #define EINVAL 22   /* Invalid value */

    /* Mac style newlines by default */
    #undef PRM_DEFAULT
    #define PRM_DEFAULT PRM_BLANK | PRM_MAC

    #undef ENC_INPUT
    #define ENC_INPUT ENC_BBC
    #undef ENC_OUTPUT
    #define ENC_OUTPUT ENC_BBC
    #undef ENC_PRINT
    #define ENC_PRINT ENC_BBC
  #else
    /* Unix style newlines by default */
    #undef PRM_DEFAULT
    #define PRM_DEFAULT PRM_BLANK | PRM_UNIX

    #include <unixlib.h> /* for chdir */

    void setupRiscOS(int *argc, char ***argv);  /* additional stuff needed at start up */

    #undef ENC_OUTPUT
    #define ENC_OUTPUT ENC_CP1252
    #undef ENC_PRINT
    #define ENC_PRINT ENC_CP1252
  #endif

  #define YY_NO_UNISTD_H 1
  #define HAS_KERNEL_SWI   /* Later versions of Norcroft have vsnprintf,
  but the early ones don't. Therefore we use the fprintf approach. */
  #include <kernel.h> /* for _kernel_osbyte function (used by d_sprintf) */

  #define SOFTFLOAT /* Use John Hauser's softfloat package rather
  than the compiler's built in floating point implementation */
#endif

#ifdef __VBCC__
  #define HAS_VSNPRINTF
#endif

#ifdef AMIGA
  /* Unix style newlines by default */
  #undef PRM_DEFAULT
  #define PRM_DEFAULT PRM_BLANK & PRM_UNIX

  #define YY_NO_UNISTD_H 1

  #define main realmain   /* We need to define our own main function as
  VBCC seems to be doing something automagical with the main function
  specifically in regard to WBStartup */

  #undef ENC_OUTPUT
  #define ENC_OUTPUT ENC_CP1252
  #undef ENC_PRINT
  #define ENC_PRINT ENC_CP1252
#endif

#ifdef ATARI
  #define YY_NO_UNISTD_H 1

  #define main realmain   /* as the atari st by default has no command line
  user interface and can only perform co-operative multitasking via desk
  accesories, we need to run some code before the "main" function */

  #undef ENC_OUTPUT
  #define ENC_OUTPUT ENC_ATARIST
  #undef ENC_PRINT
  #define ENC_PRINT ENC_ATARIST
#endif

/* This block controls both the zx spectrum and cp/m build definitions on the Z88DK compiler toolchain */
#ifdef __Z88DK
  /* z88dk's default implementations of classic malloc & free waste quite a
    lot of memory due to heap fragmentation, which limits the size of scripts
    that can be run. Replace them with our own implementations */
  /* Some examples of the previous naive behaviour :

    * realloc always allocs a new block of the requested size, memcpys the
      data across then frees the original block.

    * If a block is malloced (so will be at the top of the heap) and then freed
      with no other calls to realloc or malloc in between then no free block
      coalition is performed.

    * The heap grows downward in memory, which forces a poorer implementation of
      realloc. growing upward would instead permit the same pointer value to be
      returned from realloc with a new size if the item being realloced were at
      the top of the heap.

    * Requests for large blocks aren't fit into an existing free block of the
      correct size, but fresh memory from the end of the heap is used instead.
  */
  void mallinit_z80(void);
  void sbrk_z80(void *addr, unsigned int size);
  void *malloc_z80(unsigned int size);
  void free_z80(void *addr);
  void *realloc_z80(void *p, unsigned int size);
  void *calloc_z80(unsigned int num, unsigned int size);

  void reallocMsg(void **mem, size_t size);

  #define HEAP_FREE 0
  #define HEAP_ALLOCED 1

  /* malloc/free related structures */
  struct heapItem {
    struct heapItem * next; /* where the next block is, 0 for no next block */
    unsigned int size; /* how many bytes are contained in this block, not including these 5 header bytes */
    unsigned char type; /* 0 = free, 1 = allocated */
  };

  struct heapInternal {
    struct heapItem * nextFree;
    struct heapItem * first;
  };

  double strtod_z80(const char* str, char** endptr);  /* z88dk doesn't
  have strtod, but does have floating point support. We supply our own implementation */
  char* dtoa_z80(char *s, double n); /* wrapper around ftoa that rtrims trailing zeroes
  and the decimal point if necessary */

  FILE * fopen_z80(const char * filename, const char * mode);
  int fputs_z80(const char * str, FILE * stream);
  int fprintf_z80(char * dummy, ...) __smallc;
  size_t fwrite_z80(const void * ptr, size_t size, size_t count, FILE * stream);

  #undef ENC_INPUT
  #undef ENC_OUTPUT
  #undef ENC_PRINT

  #ifdef __SPECTRUM
    /* esxdos has its disk buffer at 0x2000 - 0x3fff, but we also want to use
    that space for our heap. Use another buffer above 0xbfff to marshal the
    data between the two */
    size_t fread_zx(void * ptr, size_t size, size_t count, FILE * stream);

    /* make EXIT_FAILURE report a "STOP statement" error instead of "NEXT without FOR" */
    #undef EXIT_FAILURE
    #define EXIT_FAILURE 9

    #define ENC_INPUT ENC_TSW
    #define ENC_OUTPUT ENC_TSW
    #define ENC_PRINT ENC_TSW
  #else
    #include <cpm.h>  /* bdos function */
    #include <fcntl.h>    /* chdir and getwd functions */
    #include <arch/z80.h> /* needed for AsmCall function and Z80_registers definition */

    #define ENC_INPUT ENC_ASCII
    #define ENC_OUTPUT ENC_ASCII
    #define ENC_PRINT ENC_ASCII

    void logNum(int num) __z88dk_fastcall;
    void macYield(void);
    #undef MAC_YIELD
    #define MAC_YIELD macYield();
  #endif

  #ifndef QCSV_NOZ80MALLOC
    #undef realloc
    #undef calloc
    #define realloc realloc_z80
    #define calloc calloc_z80
    #define malloc malloc_z80
    #define free free_z80
    #define strtod strtod_z80

    #define fopen fopen_z80
    #define fprintf(stream, format, ...) loc_type = 1; \
    loc_output = (void*)(stream); \
    loc_format = format; \
    fprintf_z80(NULL, __VA_ARGS__)
    #define YYFPRINTF(stream, format, ...) loc_type = 1; \
    loc_output = (void *)(stream); \
    loc_format = format; \
    fprintf_z80(NULL, __VA_ARGS__)   /* for the bison parser */
    #undef fputs
    #define fputs fputs_z80
    #define fwrite fwrite_z80

    #ifdef __SPECTRUM
      #define fread fread_zx
    #endif
  #endif

  #define YY_NO_UNISTD_H 1
  #define HAS_VSNPRINTF

  /* classic clib z88dk doesn't have ferror. stub it out */
  #define ferror(...) 0

  /* in z88dk fflush only actually does anything on tcp connections,
  so we can eliminate the function call */
  #define fflush(...) 0
  #define mystrnicmp strnicmp

  /* z88dk doesn't have bsearch, but it does have l_bsearch.
    employ some macro magic to smooth things over. */
  #define bsearch(a, b, c, d, e) l_bsearch(a, b, c, e)

  /*
    qsort is in z88dk's stdlib, but the function pointers would
    need to be different to those used with l_bsearch. So redefine
    qsort to point to l_qsort instead
  */
  #ifdef qsort
    #undef qsort
  #endif
  #define qsort(a, b, c, d) l_qsort(a, b, d)


  void logNum(int num) __z88dk_fastcall;
  void toggleSpinner(int num) __z88dk_fastcall;
  void setupZ80(int * argc, char *** argv);

  /* on z88dk, the __stdc calling convention modifier is necessary to make
  functions that use varargs work correctly */
  /* we redirect d_sprintf to a macro so it can reside in the libc page and share code with fprintf */
  #define d_sprintf(output, format, ...) loc_type = 0; \
    loc_output = (void*)(output); \
    loc_format = format; \
    fprintf_z80(NULL, __VA_ARGS__)

  /* z88dk's varargs never works properly for longs in user written functions,
    but it does have a non standardized ltoa function which works. Use a macro
    to sprintf elsewhere to do the same thing */
  #undef myltoa
  #define myltoa(x, y) ltoa((y), (x), 10)
#else
  /* the functions used with l_sort and l_bsearch shouldn't have offsets. Filter out __z88dk_params_offset for other compilers  */
  #define __z88dk_params_offset(VV)
#endif

/* ugly hacks to raise the game of cc65 */
#ifdef __CC65__
  #define ELIM 1
  #define YY_NO_UNISTD_H 1
  #define HAS_VSNPRINTF /* although cc65 doesn't have floating point,
  at least it has vsnprintf*/

  /* Mac style newlines by default */
  #undef PRM_DEFAULT
  #define PRM_DEFAULT PRM_BLANK

  #include <unistd.h>   /* for chdir */
  #include "cc65iso.h" /* switch back from petscii to ascii */
  #include "floatlib/float.h" /* fudges kinda support for floating point
  into cc65 by utilising functionality in the c64 basic rom */
  #define ftostr(_d,_a) { \
    reallocMsg((void**)_d, 33); \
    _ftostr(*(_d), (_a)); \
    reallocMsg((void**)_d, strlen(*(_d)) + 1); \
    } /* the _ftostr function in cc65-floatlib seems to output at
    most 32 characters */
  #define fneg(_d) _fneg(_d)
  double strtod(const char* str, char** endptr);  /* cc65 doesn't
  have strtod (as it doesn't have built in floating point number support).
  We supply our own implementation that provides the same semantics but
  uses cc65-floatlib */

  char * petsciiToUtf8(char *input);
  size_t fwrite_c64(const void * ptr, size_t size, size_t count, FILE * stream);
  int fputs_c64(const char *str, FILE *stream);
  int fprintf_c64(FILE *stream, const char *format, ...);
  FILE *fopen_c64(const char *filename, const char *mode);

  #define fwrite fwrite_c64
  #define fputs fputs_c64
  #define fopen fopen_c64
  #define fprintf fprintf_c64
  #define YYFPRINTF fprintf_c64   /* for the bison parser */

  #undef ENC_INPUT
  #undef ENC_OUTPUT
  #undef ENC_PRINT
  #define ENC_INPUT ENC_PETSCII
  #define ENC_OUTPUT ENC_PETSCII
  #define ENC_PRINT ENC_PETSCII
#else
  #define fopen_read "rb"
  #define fopen_write "wb"

  #define __fastcall__ /* do nothing */

  #ifdef SOFTFLOAT
    #include "milieu.h"
    #include "macros.h"
    #include "softfloat.h"

    char* mydtoa(char *s, double n);
    double mystrtod(const char *str, char **end);
    extern double fltMinusOne;

    #define strtod mystrtod /* swap strtod for our own implementation */
  #else
    /* duplicates of the macros in cc65-floatlib that just use the
    native floating point support */
    #define fadd(_f,_a) ((_f)+(_a))
    #define fsub(_f,_a) ((_f)-(_a))
    #define fmul(_f,_a) ((_f)*(_a))
    #define fdiv(_f,_a) ((_f)/(_a))
    #define fcmp(_d,_s) ((_d)!=(_s))
    #define ctof(_s) ((double)(_s))
    #define fneg(_f) (-(_f))
    #ifdef __Z88DK
      #include <math.h>   /* for ftoa, used in dtoa_z80 */

      #define ftostr(_d,_a) { \
        reallocMsg((void**)_d, 33); \
        dtoa_z80(*(_d), (_a)); \
        reallocMsg((void**)_d, strlen(*(_d)) + 1); \
        } /* dtoa_z80 function should output at
        most 32 characters */
    #else
      #define ftostr(_f,_a) (_a) != (_a) ? \
        d_sprintf((_f), "NaN") : \
        d_sprintf((_f), "%g", (_a)) /* z88dk doesn't have the g
        format specifier, but it also doesn't distingish between floats
        and doubles so the f specifier will work correctly */
    #endif
  #endif
#endif

/* structures */
struct resultColumn {
  int resultColumnIndex;
  int isHidden;
  int isCalculated;
  int groupType;
  int groupCount;
  int groupingDone;
  double groupNum;
  char *groupText;
  char * groupSeparator;
  char *resultColumnName;
  struct resultColumn *nextColumnInstance;  /* if the same expression is used in multiple places */
  struct resultColumn *nextColumnInResults;
};

struct inputColumn {
  int columnIndex;  /* position of this column within a csv record */
  char *fileColumnName; /* name according to csv header record */
  void *inputTablePtr;  /* reference back to the parent table */
  struct resultColumn *firstResultColumn;    /* links to where this column is output into the result set */
  struct inputColumn *nextColumnInTable;
};

struct inputTable {
  char *fileName;
  FILE *fileStream;
  int fileEncoding;
  QCSV_LONG codepoints[4];
  int cpIndex;
  int cpByteLength;
  int arrLength;
  int byteLength;

  int fileIndex;
  int columnCount;
  int isLeftJoined;
  int noLeftRecord;
  long firstRecordOffset;  /* where in the file the beginning of the first record is located */
  char *queryTableName;  /* according to the query */
  int options;
  struct inputTable *nextInputTable;
  struct inputColumn *firstInputColumn;
};

struct atomEntry {
  int index;
  char *content;
  struct atomEntry *nextInList;
};

struct expression {
  int type;
  int minTable;
  int minColumn;
  int containsAggregates;
  int isNull;
  int caseSensitive;
  char *value;
  union {
    struct {
      struct expression *leftPtr;
      struct expression *rightPtr;
    } leaves;
    void *voidPtr;
    struct {
      struct expression *leftPtr;
      struct atomEntry *lastEntryPtr;
    } inLeaves;
  } unionPtrs;
};

struct expressionEntry {
  struct expression* value;
  struct expressionEntry *nextInList;
};

struct caseEntry {
  struct expression* value;
  struct caseEntry *nextInList;
  struct expression* test;
};

struct columnReference {
  char *referenceName; /* if an input table column then according to csv header record, if expression then according to name specifed in the query using 'as' */
  int referenceType;  /* 1 - actual input column, 2 - expression */
  union {
    struct inputColumn *columnPtr;
    struct {
      struct expression *expressionPtr;
      struct resultColumn *firstResultColumn;    /* links to where this column is output into the result set */
    } calculatedPtr;
  } reference; /* a pointer to the information we want */
  struct columnReference *nextReferenceWithName; /* if this is non null and the query didn't specify which table to use then we should error */
};

struct columnEntry {
  struct columnReference* reference;
  struct columnEntry *nextInList;
};

struct columnRefHashEntry {
  char *referenceName; /* if an input table column then according to csv header record, if expression then according to name specifed in the query using 'as' */
  struct columnReference *content;
  struct columnRefHashEntry *nextReferenceInHash;
};

struct columnReferenceHash {
  int size;       /* the size of the table */
  struct columnRefHashEntry **table; /* the table elements */
};

struct sortingList {
  struct expression *expressionPtr;
  int isDescending;
  struct sortingList *nextInList;
};

struct resultColumnValue { /* this information should be stored in files */
  char *value;  /* pre normalised value */
  size_t length;
  char isNull;
};

/* results are sorted in a binary tree for quick in order retrieval */
struct resultTree {
  struct resultTree *link[2];
  struct resultTree *parent;
  struct resultColumnValue *columns;
  int type; /* red, black or freed */
};

struct qryData {
  char *inputFileName;
  FILE *inputFileStream;
  int inputEncoding;
  QCSV_LONG codepoints[4];
  int cpIndex;
  int cpByteLength;
  int arrLength;
  int byteLength;

  int parseMode;  /* 0 - open files and get their layouts cached, 1 - use the cache data to populate the rest of this data structure, */
  int hasGrouping;
  int hasRowCount;
  int commandMode;
  int columnCount;
  int hiddenColumnCount;
  int recordCount;
  int groupCount;
  int useGroupBy;
  int params;
  int outputEncoding;
  int outputOffset;
  int codepointsInLine;
  char *outputFileName;
  char *newLine;
  FILE *outputFile;
  struct columnReferenceHash *columnReferenceHashTable; /* used to get a reference to an input column given a column name */
  struct inputTable *firstInputTable;
  struct inputTable *secondaryInputTable;   /* initially the last left joined input table. Then the current input table when getting matching records */
  struct resultColumn *firstResultColumn;
  struct expression *joinsAndWhereClause;
  struct sortingList *orderByClause;
  struct sortingList *groupByClause;
  struct resultTree *resultSet; /* stored as a binary tree now */
  struct resultColumnValue *match;
  char* dateString;
};

/*
  some alternative names for the fields in this structure,
  so it can be repurposed to run commands */
#define CMD_RETVAL columnCount
#define CMD_OFFSET hiddenColumnCount
#define CMD_COLINDEX recordCount
#define CMD_ENCODING groupCount
#define CMD_PARAMS useGroupBy

struct resultColumnParam {
  int params;
  struct resultColumnValue *ptr;
};

/* hash1 was a mapping of unicode codepoints to windows 1252 bytes. Not used any more */

/* hash2 is a sparse mapping of codepoints to NFD form */
struct hash2Entry {
  QCSV_LONG codepoint;
  int length;
  const QCSV_LONG *codepoints;
};

/*hash3 is the priorities for unicode combining codepoints */
struct hash3Entry {
  QCSV_LONG codepoint;
  /* int priority; */
  int order;
};

/* hash4 maps utf-8 strings to collation sequence numbers */
struct hash4Entry {
  const char *name;
  int script;
  int index;
  int isNotLower;
};

/* codepointToByte maps from unicode codepoints to legacy charsets */
struct codepointToByte {
  QCSV_SHORT codepoint;
  char byte;
};

/* codepointToBytes is a special case of codepointToByte for saving some memory */
struct codepointToBytes {
  QCSV_SHORT codepoint;
  char cp437;
  char cp850;
  char mac;
};

/* lookup is a special case of codepointToByte for the mbcs lookups loaded from a file */
struct lookup {
  QCSV_LONG codepoint;
  unsigned char bytes[1]; /* flexible array member */
};

#ifndef NOHASH4
struct hash4aEntry {struct hash4Entry a; };
struct hash4bEntry {struct hash4Entry b; };
struct hash4cEntry {struct hash4Entry c; };

/* function prototypes */
struct hash4aEntry *in_word_set_a(register const char *str, register unsigned int len);
struct hash4bEntry *in_word_set_b(register const char *str, register unsigned int len);
struct hash4cEntry *in_word_set_c(register const char *str, register unsigned int len);

#endif

int isCombiningChar(QCSV_LONG codepoint);

struct hash2Entry* isInHash2(QCSV_LONG codepoint);

/* macro for the benefit of the c64 build */
#define getCodepoints8Bit(map) int c; \
  if(stream == NULL) { \
    *arrLength = *byteLength = 0; \
    return; \
  } \
  *arrLength = *byteLength = 1; \
  if((c = fgetc(stream)) == EOF) { \
    *byteLength = 0; \
    codepoints[0] = MYEOF; \
    return; \
  } \
  if(c < 0x80) { \
    if(c < 0x1f) { \
      codepoints[0] = (QCSV_LONG)(map[c+0x80]); \
      return; \
    } \
    codepoints[0] = (QCSV_LONG)c; \
    return; \
  } \
  codepoints[0] = (QCSV_LONG)(map[c-0x80])

#include "gen.h"
#endif
