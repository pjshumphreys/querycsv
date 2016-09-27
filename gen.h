void cleanup_atomList(struct atomEntry *currentAtom);
void cleanup_columnReferences(struct columnReferenceHash *table);
void cleanup_expression(struct expression *currentExpression);
void cleanup_inputColumns(struct inputColumn *currentInputColumn);
void cleanup_inputTables(struct inputTable *currentInputTable);
void cleanup_matchValues(struct qryData *query,struct resultColumnValue **match);
void cleanup_orderByClause(struct sortingList *currentSortingList);
void cleanup_query(struct qryData *query);
void cleanup_resultColumns(struct resultColumn *currentResultColumn);
int combiningCharCompare(const void *a,const void *b);
int consumeCombiningChars(unsigned char **str1,unsigned char **str2,unsigned char **offset1,unsigned char **offset2,void(*get1)(),void(*get2)(),int *bytesMatched1,int *bytesMatched2,int *accentcheck);
int d_sprintf(char **str,char *format,...);
int d_strftime(char **ptr,char *format,struct tm *timeptr);
int d_tztime(time_t *now,struct tm *local,struct tm *utc,char **output);
int endOfFile(FILE *stream);
void exp_divide(char **value,double leftVal,int rightVal);
void exp_uminus(char **value,double leftVal);
void getCalculatedColumns(struct qryData *query,struct resultColumnValue *match,int runAggregates);
int getColumnCount(char *inputFileName);
int getColumnValue(char *inputFileName,long offset,int columnIndex);
int getCsvColumn(FILE **inputFile,char **value,size_t *strSize,int *quotedValue,long *startPosition,int doTrim);
int getCurrentDate();
struct resultColumnValue *getFirstRecord(struct resultTree *root,struct qryData *query);
void getGroupedColumns(struct qryData *query);
struct hash4Entry *getLookupTableEntry(unsigned char **offset,unsigned char **str,int *lastMatchedBytes,void(*get)(),int firstChar);
int getMatchingRecord(struct qryData *query,struct resultColumnValue *match);
int getNextRecordOffset(char *inputFileName,long offset);
long getUnicodeChar(unsigned char **offset,unsigned char **str,int plusBytes,int *bytesMatched,void(*get)());
long getUnicodeCharFast(unsigned char **offset,unsigned char **str,int plusBytes,int *bytesMatched,void(*get)());
void getValue(struct expression *expressionPtr,struct resultColumnParam *match);
void groupResults(struct qryData *query);
void groupResultsInner(struct qryData *query,struct resultColumnValue *columns,int i);
int hash_addString(struct columnReferenceHash *hashtable,char *str,struct columnReference *new_list);
unsigned int hash_compare(struct columnReferenceHash *hashtable,char *str);
struct columnReferenceHash *hash_createTable(int size);
void hash_freeTable(struct columnReferenceHash *hashtable);
struct columnReference *hash_lookupString(struct columnReferenceHash *hashtable,char *str);
int isNumberWithGetByteLength(unsigned char *offset,int *lastMatchedBytes,int firstChar);
int main(int argc,char *argv[]);
int needsEscaping(char *str,int params);
void outputHeader(struct qryData *query);
void outputResult(struct qryData *query,struct resultColumnValue *columns,int currentIndex);
struct atomEntry *parse_atomCommaList(struct qryData *queryData,struct atomEntry *lastEntryPtr,char *newEntry);
int parse_columnRefUnsuccessful(struct qryData *queryData,struct columnReference **result,char *tableName,char *columnName);
struct resultColumn *parse_expCommaList(struct qryData *queryData,struct expression *expressionPtr,char *resultColumnName,int aggregationType);
struct expression *parse_functionRef(struct qryData *queryData,long aggregationType,struct expression *expressionPtr,int isDistinct);
struct expression *parse_functionRefStar(struct qryData *queryData,long aggregationType);
void parse_groupingSpec(struct qryData *queryData,struct expression *expressionPtr);
struct expression *parse_inPredicate(struct qryData *queryData,struct expression *leftPtr,int isNotIn,struct atomEntry *lastEntryPtr);
struct resultColumn *parse_newOutputColumn(struct qryData *queryData,int isHidden2,int isCalculated2,char *resultColumnName2,int aggregationType);
void parse_orderingSpec(struct qryData *queryData,struct expression *expressionPtr,int isDescending);
struct expression *parse_scalarExp(struct qryData *queryData,struct expression *leftPtr,int operator,struct expression *rightPtr);
struct expression *parse_scalarExpColumnRef(struct qryData *queryData,struct columnReference *referencePtr);
struct expression *parse_scalarExpLiteral(struct qryData *queryData,char *literal);
void parse_tableFactor(struct qryData *queryData,int isLeftJoin,char *fileName,char *tableName);
void parse_whereClause(struct qryData *queryData,struct expression *expressionPtr);
void readParams(char *string,int *params);
void readQuery(char *queryFileName,struct qryData *query);
void reallocMsg(char *failureMessage,void **mem,size_t size);
int recordCompare(const void *a,const void *b,void *c);
void runCommand(char *string);
int runQuery(char *queryFileName);
FILE *skipBom(const char *filename);
int strAppend(char c,char **value,size_t *strSize);
int strAppendUTF8(long codepoint,unsigned char **nfdString,int nfdLength);
int strCompare(unsigned char **str1,unsigned char **str2,int caseSensitive,void(*get1)(),void(*get2)());
void strFree(char **str);
void stringGet(unsigned char **str,struct resultColumnValue *field,int params);
int strNumberCompare(char *input1,char *input2);
char *strReplace(char *search,char *replace,char *subject);
int strRTrim(char **value,size_t *strSize,char *minSize);
int tree_insert(struct qryData *query,struct resultColumnValue *columns,struct resultTree **root);
void tree_walkAndCleanup(struct qryData *query,struct resultTree **root,void(*callback)(struct qryData *,struct resultColumnValue *,int));
void updateRunningCounts(struct qryData *query,struct resultColumnValue *match);
int walkRejectRecord(int currentTable,struct expression *expressionPtr,struct resultColumnParam *match);
void yyerror2(long lineno,char *text);
int yyerror(struct qryData *queryData,void *scanner,const char *msg);
#define INTERFACE 0
#define EXPORT_INTERFACE 0
#define LOCAL_INTERFACE 0
#define EXPORT
#define LOCAL static
#define PUBLIC
#define PRIVATE
#define PROTECTED
