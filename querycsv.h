#ifndef QUERYCSV_H
#define QUERYCSV_H 1

/* standard lib headers */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <locale.h>
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
#define PRM_TRIM 1    /* left trim and right trim whitespace from each column value */
#define PRM_SPACE 2   /* put a space before each column value tat's not the first */
#define PRM_IMPORT 4
#define PRM_EXPORT 8
#define PRM_BOM 16    /* output a utf-8 byte order mark before the file contents */

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
#define ENC_INPUT ENC_UTF8
#define ENC_OUTPUT ENC_UTF8

/* macro used to help prevent double freeing of heap data */
#define freeAndZero(p) { free(p); p = 0; }

/* ugly hacks to raise the game of cc65 */
#ifndef __CC65__
  #define __fastcall__ /* do nothing */

  /* duplicates of the macros in cc65-floatlib that just use the native floating point support */
  #define fadd(_f,_a) ((_f)+(_a))
  #define fsub(_f,_a) ((_f)-(_a))
  #define fmul(_f,_a) ((_f)*(_a))
  #define fdiv(_f,_a) ((_f)/(_a))
  #define fcmp(_d,_s) ((_d)!=(_s))
  #define ctof(_s) ((double)(_s))
  #define ftostr(_f,_a) d_sprintf((_f), "%g", (_a)) /* d_sprintf knows how to convert doubles to strings */
  #define fneg(_f) ((_f)*(-1))

  #define in_word_set_a in_word_set_ai
  #define in_word_set_b in_word_set_bi
  #define in_word_set_c in_word_set_ci
#else
  #define MAC_YIELD
  #define HAS_VSNPRINTF
  #include "cc65iso.h"  /* Changes the character set cc65 uses from petscii to ascii. We'll convert our output strings ourselves */
  #include "floatlib/float.h" /* fudges kinda support for floating point into cc65 by utilising functionality in the c64 basic rom */
  #define ftostr(_d,_a) {reallocMsg((void**)_d, 33);_ftostr(*(_d),_a);reallocMsg((void**)_d, strlen(*(_d))+1);} /* the _ftostr function in cc65-floatlib seems to output at most 32 characters */
  #define fneg(_d) _fneg(_d) 
  double strtod(const char* str, char** endptr);  /* cc65 doesn't have strtod (as it doesn't have built in floating point number support). We supply our own implementation that provides the same semantics but uses cc65-floatlib */

  #undef ENC_INPUT
  #undef ENC_OUTPUT
  #define ENC_INPUT ENC_PETSCII
  #define ENC_OUTPUT ENC_PETSCII
#endif

#ifdef __unix__
  #ifdef EMSCRIPTEN
    #define main realmain
  #else
    int vsnprintf(char *s, size_t n, const char *format, va_list arg);
  #endif

  #define MAC_YIELD
  #define HAS_VSNPRINTF   /* this function is available on windows but doesn't work properly there */

  /* used as posix doesn't have stricmp */
  #include <strings.h>
  #define stricmp strcasecmp
#endif

#ifdef MICROSOFT
  #define MAC_YIELD
  #define DEVNULL "NUL"   /* null filename on DOS/Windows */

  #ifdef WINDOWS
    #include "win32.h"
  #else
    #undef ENC_INPUT
    #undef ENC_OUTPUT
    #define ENC_INPUT ENC_CP437
    #define ENC_OUTPUT ENC_CP437
  #endif
#endif

#ifdef MPW_C
  void macYield();
  #define MAC_YIELD
  /* macYield(); */
  #define DEVNULL "Dev:Null"   /* null filename on MacOS Classic (i.e. pre OS X) */
  #define DEVNULL2 "/dev/null"  /* needed as the carbon build can run on OS X */
  #define YY_NO_UNISTD_H 1
  /* macs don't have stricmp, so we provide our own implementation */
  #ifdef __unix__
    #undef stricmp   /* this function is available on windows but doesn't work properly there */
    #undef HAS_VSNPRINTF   /* this function is available on windows but doesn't work properly there */
  #endif
  #define ENC_INPUT ENC_MAC
  #define ENC_OUTPUT ENC_MAC
#endif

#ifdef __CC_NORCROFT
  #define MAC_YIELD
  #define YY_NO_UNISTD_H 1
  #define HAS_VSNPRINTF       /* Norcroft is not a brain dead compiler */
  #include <errno.h>      /* <errno.h> only has definitions for a small number of error types */
  #include <unixlib.h>        /* for strcasecmp */
  #define stricmp strcasecmp  /* strcasecmp is defined in unixlib.h */
  void setupRiscOS(int *argc, char ***argv);  /* additional stuff needed at start up */
  FILE *fopen_ros(const char *filename, const char *mode);
  #define fopen fopen_ros

  #undef ENC_OUTPUT
  #define ENC_OUTPUT ENC_CP1252
#endif

#ifdef __VBCC__
  #define MAC_YIELD
  #define YY_NO_UNISTD_H 1
  #define HAS_VSNPRINTF
  void setupAmiga(int* argc, char*** argv);
  #include <clib/utility_protos.h>
  #define stricmp Stricmp
  #define main realmain   /* We need to define our own main function as VBCC seems to be doing something automagical with the main function specifically in regard to WBStartup */

  #undef ENC_OUTPUT
  #define ENC_OUTPUT ENC_CP1252
#endif

/* structures */
struct resultColumn {
  int resultColumnIndex;
  int isHidden;
  int isCalculated;
  int groupType;
  char *groupText;
  double groupNum;
  int groupCount;
  int groupingDone;
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
  FILE *fileStream;
  struct inputTable *nextInputTable;
  struct inputColumn *firstInputColumn;
  int fileEncoding;
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
  int leftNull;
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
  int leftNull;
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
};

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
  unsigned short codepoint;
  char byte;
};
struct codepointToBytes {
  unsigned short codepoint;
  char cp437;
  char cp850;
  char cp1047;
  char mac;
};

/* function prototypes */
struct hash4Entry *in_word_set_a(register const char *str,register unsigned int len);
struct hash4Entry *in_word_set_b(register const char *str,register unsigned int len);
struct hash4Entry *in_word_set_c(register const char *str,register unsigned int len);

/* void getHash1(char byte); */

int isCombiningChar(long codepoint);

struct hash2Entry* isInHash2(long codepoint);
#include "gen.h"
#endif
