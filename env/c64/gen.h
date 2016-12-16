void getValue(struct expression *expressionPtr,struct resultColumnParam *match);
void parse_orderingSpec(struct qryData *queryData,struct expression *expressionPtr,int isDescending);
struct expression *parse_scalarExpColumnRef(struct qryData *queryData,struct columnReference *referencePtr);
int parse_columnRefUnsuccessful(struct qryData *queryData,struct columnReference **result,char *tableName,char *columnName);
struct columnReferenceHash *hash_createTable(int size);
void parse_tableFactor(struct qryData *queryData,int isLeftJoin,char *fileName,char *tableName);
void readParams(char *string,int *params);
int endOfFile(FILE *stream);
int strNumberCompare(char *input1,char *input2);
long getUnicodeChar(unsigned char **offset,unsigned char **str,int plusBytes,int *bytesMatched,void(*get)());
void outputHeader(struct qryData *query);
int combiningCharCompare(const void *a,const void *b);
void getCalculatedColumns(struct qryData *query,struct resultColumnValue *match,int runAggregates);
int main(int argc,char *argv[]);
void groupResults(struct qryData *query);
int getCsvColumn(FILE **inputFile,char **value,size_t *strSize,int *quotedValue,long *startPosition,int doTrim);
void parse_whereClause(struct qryData *queryData,struct expression *expressionPtr);
void parse_groupingSpec(struct qryData *queryData,struct expression *expressionPtr);
void cleanup_inputColumns(struct inputColumn *currentInputColumn);
void cleanup_atomList(struct atomEntry *currentAtom);
void exp_uminus(char **value,double leftVal);
void hash_freeTable(struct columnReferenceHash *hashtable);
struct expression *parse_scalarExp(struct qryData *queryData,struct expression *leftPtr,int operator,struct expression *rightPtr);
void cleanup_columnReferences(struct columnReferenceHash *table);
struct expression *parse_inPredicate(struct qryData *queryData,struct expression *leftPtr,int isNotIn,struct atomEntry *lastEntryPtr);
void exp_divide(char **value,double leftVal,double rightVal);
int strCompare(unsigned char **str1,unsigned char **str2,int caseSensitive,void(*get1)(),void(*get2)());
char *strReplace(char *search,char *replace,char *subject);
struct expression *parse_functionRef(struct qryData *queryData,long aggregationType,struct expression *expressionPtr,int isDistinct);
struct resultColumn *parse_newOutputColumn(struct qryData *queryData,int isHidden2,int isCalculated2,char *resultColumnName2,int aggregationType);
int d_strftime(char **ptr,char *format,struct tm *timeptr);
int getColumnValue(char *inputFileName,long offset,int columnIndex);
void cleanup_expression(struct expression *currentExpression);
struct atomEntry *parse_atomCommaList(struct qryData *queryData,struct atomEntry *lastEntryPtr,char *newEntry);
void cleanup_query(struct qryData *query);
int strRTrim(char **value,size_t *strSize,char *minSize);
int walkRejectRecord(int currentTable,struct expression *expressionPtr,struct resultColumnParam *match);
int getColumnCount(char *inputFileName);
struct resultColumn *parse_expCommaList(struct qryData *queryData,struct expression *expressionPtr,char *resultColumnName,int aggregationType);
struct expression *parse_functionRefStar(struct qryData *queryData,long aggregationType);
int myfseek(FILE *stream,long offset,int origin);
void tree_walkAndCleanup(struct qryData *query,struct resultTree **root,void(*callback)(struct qryData *,struct resultColumnValue *,int));
int runQuery(char *queryFileName);
unsigned int hash_compare(struct columnReferenceHash *hashtable,char *str);
struct columnReference *hash_lookupString(struct columnReferenceHash *hashtable,char *str);
void reallocMsg(void **mem,size_t size);
int consumeCombiningChars(unsigned char **str1,unsigned char **str2,unsigned char **offset1,unsigned char **offset2,void(*get1)(),void(*get2)(),int *bytesMatched1,int *bytesMatched2,int *accentcheck);
int getNextRecordOffset(char *inputFileName,long offset);
int getMatchingRecord(struct qryData *query,struct resultColumnValue *match);
void stringGet(unsigned char **str,struct resultColumnValue *field,int params);
void cleanup_inputTables(struct inputTable *currentInputTable);
void outputResult(struct qryData *query,struct resultColumnValue *columns,int currentIndex);
int recordCompare(const void *a,const void *b,void *c);
struct expression *parse_scalarExpLiteral(struct qryData *queryData,char *literal);
int strAppendUTF8(long codepoint,unsigned char **nfdString,int nfdLength);
int getCurrentDate(void);
int needsEscaping(char *str,int params);
void updateRunningCounts(struct qryData *query,struct resultColumnValue *match);
int yyerror(struct qryData *queryData,void *scanner,const char *msg);
void readQuery(char *queryFileName,struct qryData *query);
int tree_insert(struct qryData *query,struct resultColumnValue *columns,struct resultTree **root);
int d_tztime(time_t *now,struct tm *local,struct tm *utc,char **output);
struct resultColumnValue *getFirstRecord(struct resultTree *root,struct qryData *query);
void groupResultsInner(struct qryData *query,struct resultColumnValue *columns,int i);
FILE *skipBom(const char *filename,long *offset);
long getUnicodeCharFast(unsigned char **offset,unsigned char **str,int plusBytes,int *bytesMatched,void(*get)());
void runCommand(char *string);
struct hash4Entry *getLookupTableEntry(unsigned char **offset,unsigned char **str,int *lastMatchedBytes,void(*get)(),int firstChar);
int isNumberWithGetByteLength(unsigned char *offset,int *lastMatchedBytes,int firstChar);
#if !defined(HAS_STRDUP)
char *__fastcall__ strdup(const char *s);
#endif
void cleanup_resultColumns(struct resultColumn *currentResultColumn);
int strAppend(char c,char **value,size_t *strSize);
int d_sprintf(char **str,char *format,...);
int hash_addString(struct columnReferenceHash *hashtable,char *str,struct columnReference *new_list);
void strFree(char **str);
void getGroupedColumns(struct qryData *query);
void yyerror2(long lineno,char *text);
void cleanup_orderByClause(struct sortingList *currentSortingList);
void cleanup_matchValues(struct qryData *query,struct resultColumnValue **match);
#define INTERFACE 0
#define EXPORT_INTERFACE 0
#define LOCAL_INTERFACE 0
#define EXPORT
#define LOCAL static
#define PUBLIC
#define PRIVATE
#define PROTECTED
