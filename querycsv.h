#define __STDC_WANT_LIB_EXT1__ 1  //for enabling qsort_s
#define FALSE 0
#define TRUE  1

#if MICROSOFT
  #define DEVNULL "NUL"   //null filename on DOS/Windows
  #define TEMP_VAR "TEMP"
  #define DEFAULT_TEMP "TEMP=."
  #include <io.h>   //for unlink
#else
  #define DEVNULL "/dev/null"   //null filename on linux/OS X
  #define TEMP_VAR "TMPDIR"
  #define DEFAULT_TEMP "TMPDIR=/tmp"
  #include <unistd.h>   //for unlink
  //reentrant qsort
  #include "sort_r.h"
  #define qsort_s sort_r
#endif

//standard lib headers
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <locale.h>
#include <time.h>

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


//structures
struct resultColumn {
  int resultColumnIndex;
  int isHidden;
  int isCalculated;
  int groupType;
  char * groupText;
  double groupNum;
  char * resultColumnName;
  struct resultColumn* nextColumnInstance;
  struct resultColumn* nextColumnInResults;
};

struct inputColumn {
  int columnIndex;  //position of this column within a csv record
  char* fileColumnName; //name according to csv header record
  void* inputTablePtr;  //reference back to the parent table
  struct resultColumn* firstResultColumn;    //links to where this column is output into the result set
  struct inputColumn* nextColumnInTable;
};

struct inputTable {
  int fileIndex;
  int columnCount;
  int isLeftJoined;
  int noLeftRecord;
  long firstRecordOffset;  //where in the file the beginning of the first record is located 
  char* queryTableName;  //according to the query
  FILE* fileStream;
  struct inputTable* nextInputTable;
  struct inputColumn* firstInputColumn;   
};

struct atomEntry {
  int index;
  char * content;
  struct atomEntry * nextInList;
};

struct expression {
  int type;
  int minTable;
  int minColumn;
  int containsAggregates;
  union {
    struct {
      struct expression * leftPtr;
      struct expression * rightPtr;
    } leaves;
    void * voidPtr;
    struct {
      struct expression * leftPtr;
      struct atomEntry * lastEntryPtr;
    } inLeaves;
  } unionPtrs;
  int caseSensitive;
  char* value;
};

struct columnReference {
  char* referenceName; //if an input table column then according to csv header record, if expression then according to name specifed in the query using 'as'
  int referenceType;  //1 - actual input column, 2 - expression
  union {
    struct inputColumn* columnPtr;
    struct {
      struct expression* expressionPtr;
      struct resultColumn* firstResultColumn;    //links to where this column is output into the result set
    } calculatedPtr;
  } reference; //a pointer to the information we want
  struct columnReference* nextReferenceWithName; //if this is non null and the query didn't specify which table to use then we should error
};

struct columnRefHashEntry {
  char* referenceName; //if an input table column then according to csv header record, if expression then according to name specifed in the query using 'as'
  struct columnReference* content;
  struct columnRefHashEntry* nextReferenceInHash;
};

struct columnReferenceHash {
  int size;       /* the size of the table */
  struct columnRefHashEntry **table; /* the table elements */
};

struct sortingList {
  struct expression * expressionPtr;
  int isDescending;       
  struct sortingList *nextInList;
};

struct qryData {
  int parseMode;  //0 - open files and get their layouts cached, 1 - use the cache data to populate the rest of this data structure
  int hasGrouping;
  int columnCount;
  int hiddenColumnCount;
  int groupCount;
  char* intoFileName;
  struct columnReferenceHash* columnReferenceHashTable; //used to get a reference to an input column given a column name
  struct inputTable* firstInputTable;
  struct inputTable* secondaryInputTable;   //initially the last left joined input table. Then the current input table when getting matching records
  struct resultColumn * firstResultColumn;
  struct expression* joinsAndWhereClause;
  struct sortingList* orderByClause;
  struct sortingList* groupByClause;
  int currentInputTable;
  int currentInputColumn;
  FILE* scratchpad;
  char* scratchpadName;
};

struct resultColumnValue { //this information should be stored in files
  long startOffsetOrig;
  long startOffset;
  int isQuoted;
  int isNormalized;
  size_t lengthOrig;
  size_t length;
  FILE ** source;
};

struct resultSet {
  size_t recordCount;
  //FILE* recordCache;  //collection instances of resultColumn
  struct resultColumnValue* records;
};

//function prototypes

int snprintf_d(char** str, char* format, ...);
void reallocMsg(char *failureMessage, void** mem, size_t size);
int strCompare(unsigned char **str1, unsigned char **str2, int caseSensitive, void (*get1)(), void (*get2)());
long getUnicodeChar(unsigned char **offset, unsigned char **str, int plusBytes, int *bytesMatched, void (*get)());
long getUnicodeCharFast(unsigned char **offset, unsigned char **str, int plusBytes, int* bytesMatched, void (*get)());

int getCsvColumn(FILE** inputFile, char** value, size_t* strSize, int* quotedValue, long* startPosition);

void parse_table_factor(
    struct qryData* queryData,
    int isLeftJoin,
    char* fileName,
    char* tableName
  );

int parse_column_ref_unsuccessful(
    struct qryData* queryData,
    struct columnReference** result,
    char* tableName,
    char* columnName
  );

struct resultColumn* parse_exp_commalist(
    struct qryData* queryData,
    struct expression* expressionPtr,
    char* outputColumnName,
    int makeHidden
  );

struct expression* parse_scalar_exp_literal(
    struct qryData* queryData,
    char* literal
  );

struct expression * parse_scalar_exp(
    struct qryData* queryData,
    struct expression* leftPtr,
    int operator,
    struct expression* rightPtr
  );

struct expression* parse_scalar_exp_column_ref(
    struct qryData* queryData,
    struct columnReference* referencePtr
  );

void parse_where_clause(
    struct qryData* queryData,
    struct expression* expressionPtr
  );

struct expression* parse_in_predicate(
    struct qryData* queryData,
    struct expression* leftPtr,
    int isNotIn,
    struct atomEntry* lastEntryPtr
  );

struct atomEntry* parse_atom_commalist(
    struct qryData* queryData,
    struct atomEntry* lastEntryPtr,
    char* newEntry
  );

void parse_ordering_spec(
    struct qryData* queryData,
    struct expression *expressionPtr,
    int isDescending
  );

void parse_grouping_spec(
    struct qryData* queryData,
    struct expression *expressionPtr
  );

struct expression* parse_function_ref(
    struct qryData* queryData,
    long aggregationType,
    struct expression *expressionPtr
  );

struct columnReferenceHash* create_hash_table(int size);
unsigned int hashfunc(struct columnReferenceHash *hashtable, char *str);
struct columnReference *lookup_string(struct columnReferenceHash *hashtable, char *str);

int add_string(
    struct columnReferenceHash *hashtable,
    char *str,
    struct columnReference *new_list
  );

void free_table(struct columnReferenceHash *hashtable);

char *strReplace(char *search, char *replace, char *subject);
void yyerror2(long lineno, char *text);
