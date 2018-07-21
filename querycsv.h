#ifndef QUERYCSV_H
#define QUERYCSV_H 1

/* standard lib headers */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

/* translatable strings */
#include "en_gb.h"

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
#define PRM_UNIX 2    /* output unix newlines in the output file */
#define PRM_MAC 4    /* output mac newlines in the output file */
#define PRM_SPACE 8   /* put a space before each column value that's not the first */
#define PRM_TRIM 16    /* left trim and right trim whitespace from each column value */
#define PRM_QUOTE 32  /* always double quote non null values */
#define PRM_BLANK 64  /* import unquoted empty strings as NULL */
#define PRM_NULL 128  /* import unquoted text NULL as NULL */
#define PRM_POSTGRES 256  /* import unquoted text \N as NULL */
#define PRM_ENULL 512  /* export NULL as unquoted text NULL */
#define PRM_EPOSTGRES 1024  /* export NULL as unquoted text \N */
#define PRM_DEFAULT PRM_BLANK  /* default parametters */

#define TRE_BLACK 1
#define TRE_RED 2
#define TRE_FREED 3

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
#define ENC_INPUT ENC_UTF8
#define ENC_OUTPUT ENC_UTF8
#define ENC_PRINT ENC_UTF8

/* macro used to help prevent double freeing of heap data */
#define freeAndZero(p) { free(p); p = 0; }

/* this macro was intended to facilitate multi tasking on classic mac os,
but hasn't actually been needed up to now. It's being kept just in case
it becomes needed */
#define MAC_YIELD

#if defined(__unix__) || defined(__LINUX__)
  #if defined(EMSCRIPTEN)
    #define main realmain
  #elif !defined(__WATCOMC__)
    int vsnprintf(char *s, size_t n, const char *format, va_list arg);
    #define HAS_VSNPRINTF   /* this function intentionally never works
    properly on watcom (for source compatability with the windows version) */
  #endif

  /* used as posix doesn't have stricmp */
  #include <strings.h>
  #define stricmp strcasecmp
#endif

#ifdef MICROSOFT
  #ifdef WINDOWS
    void setupWin32(int * argc, char *** argv);
    int fputs_w32(const char *str, FILE *stream);
    int fprintf_w32(FILE *stream, const char *format, ...);
    FILE *fopen_w32(const char *filename, const char *mode);

    #define fputs fputs_w32
    #define fopen fopen_w32
    #define fprintf fprintf_w32
    #define YYFPRINTF fprintf_w32   /* for the bison parser */
  #else
    #include <locale.h>  /*we need to call setlocale after setting the
    TZ environment variable for gmtime to work correctly on msdos watcom*/

    #undef ENC_INPUT
    #undef ENC_OUTPUT
    #undef ENC_PRINT
    #define ENC_INPUT ENC_CP437
    #define ENC_OUTPUT ENC_CP437
    #define ENC_PRINT ENC_CP437
  #endif
#endif

#ifdef MPW_C
  /* void macYield(void);
  #define MAC_YIELD macYield(); */
  #define YY_NO_UNISTD_H 1

  /* Mac style newlines by default */
  #undef PRM_DEFAULT
  #define PRM_DEFAULT PRM_MAC

  /* macs don't have stricmp, so we provide our own implementation */
  #ifdef __unix__
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
  #if TARGET_API_MAC_CARBON
    FILE *fopen_mac(const char *filename, const char *mode);
    void fsetfileinfo_absolute(
      const char *filename,
      unsigned long newcreator,
      unsigned long newtype
    );
    #define fopen fopen_mac
    #define ENC_INPUT ENC_MAC
    #define ENC_OUTPUT ENC_UTF16BE
  #else
    #define ENC_INPUT ENC_MAC
    #define ENC_OUTPUT ENC_MAC
    #define fsetfileinfo_absolute fsetfileinfo
  #endif
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

  /* manually define the errno values that lexer uses as errno.h doesn't
     exist in version 2 but lexer.c uses these defines */
  #define EINTR  4    /* Interrupted system call */
  #define ENOMEM 12   /* Cannot allocate memory */
  #define EINVAL 22   /* Invalid value */

  /* These aren't in norcroft version 2's stdlib.h */
  #define EXIT_FAILURE 1
  #define EXIT_SUCCESS 0

  #define SOFTFLOAT /* Use John Hauser's softfloat package rather
  than the compiler's built in floating point implementation */

  #if __LIB_VERSION < 300
    /* doesn't do well with 16 bit data types, so use the 32 bit ones all the time */
    #define YYTYPE_UINT16 unsigned int
    #define YYTYPE_INT16 int
    #undef QCSV_SHORT
    #define QCSV_SHORT long

    /* Mac style newlines by default */
    #undef PRM_DEFAULT
    #define PRM_DEFAULT PRM_MAC

    #undef ENC_INPUT
    #define ENC_INPUT ENC_BBC
    #undef ENC_OUTPUT
    #define ENC_OUTPUT ENC_BBC
    #undef ENC_PRINT
    #define ENC_PRINT ENC_BBC
  #else
    /* Unix style newlines by default */
    #undef PRM_DEFAULT
    #define PRM_DEFAULT PRM_UNIX

    #include <unixlib.h> /*for chdir? */

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
  #include <clib/utility_protos.h> /* for Stricmp */
  #define stricmp Stricmp

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

  #include <conio.h> /* for cgetc */
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
    #define fneg(_f) ((_f)*(-1))
    #define ftostr(_f,_a) (_a) != (_a) ? \
      d_sprintf((_f), "NaN") : \
      d_sprintf((_f), "%g", (_a)) /* d_sprintf knows how to
      convert doubles to strings */
  #endif

  #define in_word_set_a in_word_set_ai
  #define in_word_set_b in_word_set_bi
  #define in_word_set_c in_word_set_ci
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
  char *resultColumnName;
  struct resultColumn *nextColumnInstance;
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
  long startOffset;
  int isQuoted;
  int isNormalized;
  int isNull;
  size_t length;
  char *value;  /* pre normalised value */
};

/* results are sorted in a binary tree for quick in order retrieval */
/* TODO: make the tree_insert function use a red black tree algorithm */
struct resultTree {
  struct resultTree *left;
  struct resultTree *right;
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
  int inputEncoding;
  void (*getCodepoints)(FILE *, long *, int *, int *);
  int outputEncoding;
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
  int islower;
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
    codepoints[0] = (long)c; \
    return; \
  } \
  codepoints[0] = (long)(map[c-0x80])

#include "gen.h"
#endif
