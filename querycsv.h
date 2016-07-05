#ifndef QUERYCSV_H
#define QUERYCSV_H 1

#define ECHO 1 //disables flex from outputing unmatched input
#define FALSE 0
#define TRUE  1

//standard lib headers
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <locale.h>
#include <time.h>

#if MICROSOFT
  #define DEVNULL "NUL"   //null filename on DOS/Windows
  #define TEMP_VAR "TEMP"
  #define DEFAULT_TEMP "TEMP=."
  #define MAX_UTF8_PATH 780 // (_MAX_PATH)*3

  struct dirent {
    unsigned  d_type;
    time_t    d_ctime; //-1 for FAT file systems
    time_t    d_atime; //-1 for FAT file systems
    time_t    d_mtime;
    long      d_size; //64-bit size info
    char      d_name[MAX_UTF8_PATH]; 
    char      d_first; //flag for 1st time
    long      d_handle; //handle to pass to FindNext
  };

  #define DIR struct dirent

  DIR *opendir(const char *name);
  struct dirent *readdir(DIR *inval);
  int closedir(DIR * inval);

  #if WINDOWS
    #include "win32.h"
  #endif
#else
  #define HAS_VSNPRINTF 1   //this function is available on windows but doesn't work properly there
  #define TEMP_VAR "TMPDIR"
  #define DEFAULT_TEMP "TMPDIR=/tmp"
  #include <dirent.h>   //for opendir, readdir & closedir

  //used if the compiler doesn't have stricmp
  #ifdef NO_STRICMP
    #include <strings.h>
    #define stricmp strcasecmp
  #endif
#endif

//translatable strings
#include "en_gb.h"

//sub expression types
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

//identifier reference types
#define REF_COLUMN 1
#define REF_EXPRESSION 2

//grouping types
#define GRP_NONE 0
#define GRP_AVG 1
#define GRP_MIN 2
#define GRP_MAX 3
#define GRP_SUM 4
#define GRP_COUNT 5
#define GRP_CONCAT 6
#define GRP_STAR 7
#define GRP_DIS_AVG 8   //DIS = Distinct
#define GRP_DIS_MIN 9
#define GRP_DIS_MAX 10
#define GRP_DIS_SUM 11
#define GRP_DIS_COUNT 12
#define GRP_DIS_CONCAT 13

//output parameters. Now specified as part of the input grammar 
#define PRM_TRIM 1    //left trim and right trim whitespace from each column value 
#define PRM_SPACE 2   //put a space before each column value tat's not the first 
#define PRM_IMPORT 4
#define PRM_EXPORT 8
#define PRM_BOM 16    //output a utf-8 byte order mark before the file contents

#define TRE_BLACK 1
#define TRE_RED 2
#define TRE_FREED 3

//structures
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
  int columnIndex;  //position of this column within a csv record
  char *fileColumnName; //name according to csv header record
  void *inputTablePtr;  //reference back to the parent table
  struct resultColumn *firstResultColumn;    //links to where this column is output into the result set
  struct inputColumn *nextColumnInTable;
};

struct inputTable {
  int fileIndex;
  int columnCount;
  int isLeftJoined;
  int noLeftRecord;
  long firstRecordOffset;  //where in the file the beginning of the first record is located 
  char *queryTableName;  //according to the query
  FILE *fileStream;
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
  char *referenceName; //if an input table column then according to csv header record, if expression then according to name specifed in the query using 'as'
  int referenceType;  //1 - actual input column, 2 - expression
  union {
    struct inputColumn *columnPtr;
    struct {
      struct expression *expressionPtr;
      struct resultColumn *firstResultColumn;    //links to where this column is output into the result set
    } calculatedPtr;
  } reference; //a pointer to the information we want
  struct columnReference *nextReferenceWithName; //if this is non null and the query didn't specify which table to use then we should error
};

struct columnRefHashEntry {
  char *referenceName; //if an input table column then according to csv header record, if expression then according to name specifed in the query using 'as'
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

struct resultColumnValue { //this information should be stored in files
  long startOffset;
  int isQuoted;
  int isNormalized;
  int leftNull;
  size_t length;
  char *value;  //pre normalised value
};

//results are sorted in a binary tree for quick in order retrieval
struct resultTree {
  struct resultTree *left;
  struct resultTree *right;
  struct resultTree *parent;
  struct resultColumnValue *columns;
  int type;
};

struct qryData {
  int parseMode;  //0 - open files and get their layouts cached, 1 - use the cache data to populate the rest of this data structure,
  int hasGrouping;
  int columnCount;
  int hiddenColumnCount;
  int recordCount;
  int groupCount;
  int useGroupBy;
  int params;
  char *intoFileName;
  char *newLine;
  FILE *outputFile;
  struct columnReferenceHash *columnReferenceHashTable; //used to get a reference to an input column given a column name
  struct inputTable *firstInputTable;
  struct inputTable *secondaryInputTable;   //initially the last left joined input table. Then the current input table when getting matching records
  struct resultColumn *firstResultColumn;
  struct expression *joinsAndWhereClause;
  struct sortingList *orderByClause;
  struct sortingList *groupByClause;
  struct resultTree *resultSet; //stored as a binary tree now
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
  //int priority;
  int order;
};

struct hash4Entry {
  const char *name;
  int script;
  int index;
  int islower;
};

//function prototypes
struct hash4Entry *in_word_set(register const char *str,register unsigned int len);

int normaliseAndGet(
    unsigned char **offset,
    unsigned char **str,
    int plusBytes,
    int *bytesMatched,
    int bytesRead,
    struct hash2Entry* entry
  );

int isCombiningChar(long codepoint);

struct hash2Entry* isInHash2(long codepoint);
#include "gen.h"
#endif

