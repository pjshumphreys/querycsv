#ifndef QUERYCSV_H
#define QUERYCSV_H 1

/* standard lib headers */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#if !defined(FLEXINT_H) && (defined(__Z88DK) || __STDC_VERSION__ >= 199901L)
  #include <stdint.h>
#endif

#ifdef __Z88DK
  #define YY_NO_UNISTD_H 1
  #define HAS_VSNPRINTF

  /* classic clib z88dk doesn't have ferror. stub it out */
  #define ferror(...) 0
  #define mystrnicmp strnicmp

  double strtod(const char* str, char** endptr);  /* z88dk doesn't
  have strtod, but does have floating point support. We supply our own implementation */

  /*
  z88dk doesn't have bsearch, but it does have l_bsearch.
  employ some macro magic to smooth things over.
  */
  #define bsearch(a, b, c, d, e) l_bsearch(a, b, c, e)

  #define __Z88DK_R2L_CALLING_CONVENTION /* Makes varargs kinda work on Z88DK
as long as the function using them uses the __stdc calling convention */

  void logNum(int num) __z88dk_fastcall;
  void setupZX(char * filename) __z88dk_fastcall;
#endif

#include <stdarg.h>

#define YY_EXTRA_TYPE struct qryData*
#define ECHO 1 /* disables flex from outputing unmatched input */
#define FALSE 0
#define TRUE  1
#define MYEOF -1

/* sub expression types */
#define EXP_COLUMN 1
#define EXP_LITERAL 2
#define EXP_CALCULATED 3
#define EXP_PLUS 4
#define EXP_MINUS 5
#define EXP_MULTIPLY 6
#define EXP_DIVIDE 7
#define EXP_CONCAT 8
#define EXP_UPLUS 9
#define EXP_UMINUS 10
#define EXP_AND 11
#define EXP_OR 12
#define EXP_NOT 13
#define EXP_EQ 14
#define EXP_NEQ 15
#define EXP_LT 16
#define EXP_GT 17
#define EXP_LTE 18
#define EXP_GTE 19
#define EXP_IN 20
#define EXP_NOTIN 21
#define EXP_GROUP 22
#define EXP_ISNULL 23
#define EXP_NOTNULL 24
#define EXP_CASE 25

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
#define GRP_STAR 7
#define GRP_DIS_AVG 8   /* DIS = Distinct */
#define GRP_DIS_MIN 9
#define GRP_DIS_MAX 10
#define GRP_DIS_SUM 11
#define GRP_DIS_COUNT 12
#define GRP_DIS_CONCAT 13

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
#define ENC_UTF8 7
#define ENC_UTF16LE 8
#define ENC_UTF16BE 9
#define ENC_UTF32LE 10
#define ENC_UTF32BE 11
#define ENC_CP1047 12
#define ENC_PETSCII 13
#define ENC_ATARIST 14
#define ENC_BBC 15
#define ENC_ZX 16
#define ENC_ASCII 17
/* Tasword 2 file format. Same as the ZX (spectrum) character
 * set but with hard-coded line lengths of 64 characters.
 * Newlines and EOF are stored as graphics characters and space
 * padded as necessary. Files also have a maximum size constraint
 * which will cause the program to abort if exceeded */
#define ENC_TSW 18

#define ENC_INPUT ENC_UTF8
#define ENC_OUTPUT ENC_UTF8
#define ENC_PRINT ENC_UTF8

/* macro used to help prevent double freeing of heap data */
#define freeAndZero(p) { free(p); p = 0; }

/* this macro was intended to facilitate multi tasking on classic mac os,
but hasn't actually been needed up to now. It's being kept just in case
it becomes needed and because it's useful for debugging */
#define MAC_YIELD

#if defined(__unix__) || defined(__LINUX__)
  #ifdef EMSCRIPTEN
    #define main realmain
  #endif

  #ifndef __WATCOMC__
    int vsnprintf(char *s, size_t n, const char *format, va_list arg);
    #define HAS_VSNPRINTF   /* this function intentionally doesn't work
    properly on watcom (to have source compatability with the windows version) */
  #endif

  /* used as posix doesn't have stricmp */
  #include <unistd.h> /* for _chdir */
  #include <strings.h>
  #include <locale.h>
  #define stricmp strcasecmp
#endif

#ifdef MICROSOFT
  #define YY_NO_UNISTD_H 1

  #ifdef WINDOWS
    void setupWin32(int * argc, char *** argv);
    int fputs_w32(const char *str, FILE *stream);
    int fprintf_w32(FILE *stream, const char *format, ...);
    int chdir_w32(const char *dirname);
    FILE *fopen_w32(const char *filename, const char *mode);

    #define chdir chdir_w32
    #define fputs fputs_w32
    #define fopen fopen_w32
    #define fprintf fprintf_w32
    #define YYFPRINTF fprintf_w32   /* for the bison parser */
  #else
    #include <direct.h>   /* for getcwd */
    void atexit_dos(void);
    int fputs_dos(const char *str, FILE *stream);
    int fprintf_dos(FILE *stream, const char *format, ...);

    #ifdef DOS_DAT
      void openDat(void);
    #endif

    #define chdir _chdir
    #define fputs fputs_dos
    #define fprintf fprintf_dos
    #define YYFPRINTF fprintf_dos   /* for the bison parser */

    #ifdef __WATCOMC__
      #include <locale.h>  /* we need to call setlocale after setting the
      TZ environment variable for gmtime to work correctly on msdos watcom */
    #endif

    #undef ENC_INPUT
    #undef ENC_OUTPUT
    #define ENC_INPUT ENC_CP437
    #define ENC_OUTPUT ENC_CP437
    /* ENC_PRINT is now utf-8 as we'll do the charset conversion in fputs/fprintf wrapper functions now */
  #endif
#endif

#ifdef MPW_C
  #define YY_NO_UNISTD_H 1

  #ifdef RETRO68
    #define HAS_VSNPRINTF
  #endif

  /* Mac style newlines by default */
  #undef PRM_DEFAULT
  #define PRM_DEFAULT PRM_BLANK | PRM_MAC

  /* macs don't have stricmp, so we provide our own implementation */
  #if defined(__unix__) || defined(RETRO68)
    #undef stricmp
    int stricmp(const char *str1, const char *str2);
  #endif

  int fputs_mac(const char *str, FILE *stream);
  int fprintf_mac(FILE *stream, const char *format, ...);
  #define fputs fputs_mac
  #define fprintf fprintf_mac
  #define YYFPRINTF fprintf_mac   /* for the bison parser */

  #define main realmain /*macs need to do pre and post handling, but
  SIO (which I previously used) doesn't seem to work with Carbon */

  #undef ENC_INPUT
  #undef ENC_OUTPUT

  #if defined(RETRO68)
    void fsetfileinfo_absolute(
      const char *filename,
      unsigned long newcreator,
      unsigned long newtype
    );
  #else
    #define fsetfileinfo_absolute fsetfileinfo
  #endif

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

  void exit_mac(int dummy);
  #define exit exit_mac
#endif

/* Old versions of Norcroft appear to generate incorrect codes with short integers.
Just use long ones for that compiler */
#define QCSV_SHORT short

#ifdef __CC_NORCROFT
  #define YY_NO_UNISTD_H 1
  #define HAS_KERNEL_SWI   /* Later versions of Norcroft have vsnprintf,
  but the early ones don't. Therefore we use the fprintf approach. */
  #include <kernel.h> /* for _kernel_osbyte function (used by d_sprintf) */

  int stricmp(const char *str1, const char *str2);

  #define SOFTFLOAT /* Use John Hauser's softfloat package rather
  than the compiler's built in floating point implementation */

  #if __LIB_VERSION < 300
    /* These aren't in norcroft version 2's stdlib.h */
    #define EXIT_FAILURE 1
    #define EXIT_SUCCESS 0

    /* manually define the errno values that lexer uses as errno.h doesn't
       exist in version 2 but lexer.c uses these defines */
    #define EINTR  4    /* Interrupted system call */
    #define ENOMEM 12   /* Cannot allocate memory */
    #define EINVAL 22   /* Invalid value */

    /* doesn't do well with 16 bit data types, so use the 32 bit ones all the time */
    #define YYTYPE_UINT16 unsigned int
    #define YYTYPE_INT16 int
    #undef QCSV_SHORT
    #define QCSV_SHORT long

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
#endif

#ifdef __VBCC__
  #define HAS_VSNPRINTF
#endif

#ifdef AMIGA
  /* Unix style newlines by default */
  #undef PRM_DEFAULT
  #define PRM_DEFAULT PRM_BLANK & PRM_UNIX

  #define YY_NO_UNISTD_H 1
  int stricmp(const char *str1, const char *str2); /* use our own stricmp as the
  amiga's built in one is a hassle to use with vbcc */

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
  int stricmp(const char *str1, const char *str2); /* atari st computers
  don't have stricmp, so we provide our own implementation */

  #define main realmain   /* as the atari st by default has no command line
  user interface and can only perform co-operative multitasking via desk
  accesories, we need to run some code before the "main" function */

  #undef ENC_OUTPUT
  #define ENC_OUTPUT ENC_ATARIST
  #undef ENC_PRINT
  #define ENC_PRINT ENC_ATARIST
#endif

/* ugly hacks to raise the game of cc65 */
#ifdef __CC65__
  #define YY_NO_UNISTD_H 1
  #define HAS_VSNPRINTF /* although cc65 doesn't have floating point,
  at least it has vsnprintf*/

  /* Mac style newlines by default */
  #undef PRM_DEFAULT
  #define PRM_DEFAULT PRM_MAC

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
  int fputs_c64(const char *str, FILE *stream);
  int fprintf_c64(FILE *stream, const char *format, ...);
  FILE *fopen_c64(const char *filename, const char *mode);
  int mystricmp(const char *str1, const char *str2);

  #define fputs fputs_c64
  #define fopen fopen_c64
  #define fprintf fprintf_c64
  #define YYFPRINTF fprintf_c64   /* for the bison parser */
  #define stricmp mystricmp

  #undef ENC_INPUT
  #undef ENC_OUTPUT
  #undef ENC_PRINT
  #define ENC_INPUT ENC_PETSCII
  #define ENC_OUTPUT ENC_PETSCII
  #define ENC_PRINT ENC_PETSCII
#else
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
    #define ftostr(_f,_a) (_a) != (_a) ? \
      d_sprintf((_f), "NaN") : \
      d_sprintf((_f), "%g", (_a)) /* d_sprintf knows how to
      convert doubles to strings */
  #endif
#endif

/* translatable strings */
#include "en_gb.h"

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
  int fileIndex;
  int columnCount;
  int isLeftJoined;
  int noLeftRecord;
  long firstRecordOffset;  /* where in the file the beginning of the first record is located */
  char *queryTableName;  /* according to the query */
  char *fileName;
  FILE *fileStream;
  int options;
  int fileEncoding;
  long codepoints[4];
  int cpIndex;
  int cpByteLength;
  int arrLength;
  int byteLength;
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

struct caseEntry {
  struct expression* test;
  struct expression* value;
  struct caseEntry *nextInList;
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
/* TODO: make the tree_insert function use a red black tree algorithm */
struct resultTree {
  struct resultTree *link[2];
  struct resultTree *parent;
  struct resultColumnValue *columns;
  int type; /* red, black or freed */
};

struct qryData {
  int parseMode;  /* 0 - open files and get their layouts cached, 1 - use the cache data to populate the rest of this data structure, */
  int hasGrouping;
  int commandMode;
  int columnCount;
  int hiddenColumnCount;
  int recordCount;
  int groupCount;
  int useGroupBy;
  int params;
  char *inputFileName;
  int inputEncoding;
  void (*getCodepoints)(FILE *, long *, int *, int *);
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

struct resultColumnParam {
  int params;
  struct resultColumnValue *ptr;
};

struct hash1Entry {
  int length;
  const long *codepoints;
};

struct hash2Entry {
  long codepoint;
  int length;
  const long *codepoints;
};

struct hash3Entry {
  long codepoint;
  /* int priority; */
  int order;
};

struct hash4Entry {
  const char *name;
  int script;
  int index;
  int isNotLower;
};

struct codepointToByte {
  unsigned QCSV_SHORT codepoint;
  char byte;
};

struct codepointToBytes {
  unsigned QCSV_SHORT codepoint;
  char cp437;
  char cp850;
  char cp1047;
  char mac;
};

/* function prototypes */
struct hash4Entry *in_word_set_a(register const char *str, register unsigned int len);
struct hash4Entry *in_word_set_b(register const char *str, register unsigned int len);
struct hash4Entry *in_word_set_c(register const char *str, register unsigned int len);

int isCombiningChar(long codepoint);

struct hash2Entry* isInHash2(long codepoint);

/* macro for the benefit of the c64 build */
#define getCodepoints8Bit(map) int c; \
  if(stream == NULL) { \
    *arrLength = *byteLength = 0; \
    return; \
  } \
  *arrLength = *byteLength = 1; \
  if((c = fgetc(stream)) == EOF) { \
    codepoints[0] = MYEOF; \
    return; \
  } \
  if(c < 0x80) { \
    if(c < 0x1f) { \
      codepoints[0] = (long)(map[c+0x80]); \
      return; \
    } \
    codepoints[0] = (long)c; \
    return; \
  } \
  codepoints[0] = (long)(map[c-0x80])

#include "gen.h"
#endif
