#include "querycsv.h"

/* getValue can be passed a scalar expression's abstract syntax */
/* tree and it will evaluate it and turn it into a literal string of text characters. */
/* the evaluated value must be freed later though. */
void getValue(
    struct expression *expressionPtr,
    struct resultColumnParam *match
  ) {

  struct expression *calculatedField;
  struct resultColumn *column;
  struct resultColumnValue *field;
  double temp1;
  double temp2;

  MAC_YIELD

  expressionPtr->leftNull = FALSE;

  switch(expressionPtr->type) {
    case EXP_COLUMN: {
      /* get the value of the first instance in the result set of */
      /* this input column (it should have just been filled out with a */
      /* value for the current record) */

      field = &(match->ptr[
          ((struct inputColumn*)(expressionPtr->unionPtrs.voidPtr))->
          firstResultColumn->resultColumnIndex
        ]);

      if(field->leftNull) {
        expressionPtr->leftNull = TRUE;
        expressionPtr->value = strdup("");
      }
      else {
        stringGet((unsigned char **)(&(expressionPtr->value)), field, match->params);
      }
    } break;

    case EXP_LITERAL: {
      expressionPtr->value = strdup((char *)expressionPtr->unionPtrs.voidPtr);
    } break;

    case EXP_CALCULATED: {
      calculatedField = ((struct expression*)(expressionPtr->unionPtrs.voidPtr));

      getValue(calculatedField, match);

      expressionPtr->leftNull = calculatedField->leftNull;
      expressionPtr->value = strdup(calculatedField->value);

      freeAndZero(calculatedField->value);
    } break;

    case EXP_GROUP: {
      column = (struct resultColumn *)(expressionPtr->unionPtrs.voidPtr);
      if(column->groupingDone) {
        field = &(match->ptr[column->resultColumnIndex]);

        if(field->leftNull == FALSE) {
          stringGet((unsigned char **)(&(expressionPtr->value)), field, match->params);
          break;
        }
      }
      else if(column->groupText != NULL) {
        expressionPtr->value = strdup(column->groupText);
        break;
      }

      expressionPtr->leftNull = TRUE;
      expressionPtr->value = strdup("");
    } break;

    case EXP_UPLUS: {
      getValue(
          expressionPtr->unionPtrs.leaves.leftPtr,
          match
        );

      if(expressionPtr->unionPtrs.leaves.leftPtr->leftNull) {
        expressionPtr->leftNull = TRUE;
        expressionPtr->value = strdup("");
      }
      else {
        temp1 = strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL);
        ftostr(&(expressionPtr->value), temp1);
      }

      freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
    } break;

    case EXP_UMINUS: {
      getValue(
          expressionPtr->unionPtrs.leaves.leftPtr,
          match
        );

      if(expressionPtr->unionPtrs.leaves.leftPtr->leftNull) {
        expressionPtr->leftNull = TRUE;
        expressionPtr->value = strdup("");
      }
      else {
        exp_uminus(
            &(expressionPtr->value),
            strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL)
          );
      }

      freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
    } break;

    default: {
      if(expressionPtr->type > EXP_CONCAT) {
        expressionPtr->value = strdup("");
        break;
      }

      getValue(
          expressionPtr->unionPtrs.leaves.leftPtr,
          match
        );

      getValue(
          expressionPtr->unionPtrs.leaves.rightPtr,
          match
        );

      if(
          expressionPtr->unionPtrs.leaves.leftPtr->leftNull ||
          expressionPtr->unionPtrs.leaves.rightPtr->leftNull
        ) {
        expressionPtr->leftNull = TRUE;
        expressionPtr->value = strdup("");
      }
      else {
        switch(expressionPtr->type){
          case EXP_PLUS:
            temp1 = strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL);
            temp2 = fadd(temp1, strtod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL));
            ftostr(&(expressionPtr->value), temp2);
          break;

          case EXP_MINUS:
            temp1 = strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL);
            temp2 = fsub(temp1, strtod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL));
            ftostr(&(expressionPtr->value), temp2);
          break;

          case EXP_MULTIPLY:
            temp1 = strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL);
            temp2 = fmul(temp1, strtod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL));
            ftostr(&(expressionPtr->value), temp2);
          break;

          case EXP_DIVIDE:
            exp_divide(
                &(expressionPtr->value),
                strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL),
                strtod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL)
              );
          break;

          case EXP_CONCAT:
            d_sprintf(
                &(expressionPtr->value),
                "%s%s",
                expressionPtr->unionPtrs.leaves.leftPtr->value,
                expressionPtr->unionPtrs.leaves.rightPtr->value
              );
          break;
        }
      }

      freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
      freeAndZero(expressionPtr->unionPtrs.leaves.rightPtr->value);
    } break;
  }
}
#include "querycsv.h"

void parse_orderingSpec(
    struct qryData *queryData,
    struct expression *expressionPtr,
    int isDescending
  ) {

  struct sortingList *sortingListPtr = NULL;

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return;
  }
  
  reallocMsg(
      (void**)&(sortingListPtr),
      sizeof(struct sortingList)
    );

  sortingListPtr->expressionPtr = expressionPtr;
  sortingListPtr->isDescending = isDescending;

  if(queryData->orderByClause == NULL) {
    sortingListPtr->nextInList = sortingListPtr;
    queryData->orderByClause = sortingListPtr;
  }
  else {
    sortingListPtr->nextInList = queryData->orderByClause->nextInList;
    queryData->orderByClause->nextInList = sortingListPtr;
    queryData->orderByClause = sortingListPtr;
  }
}


#include "querycsv.h"

/*produces a different node type depending on whether a csv column is being referered to*/
struct expression *parse_scalarExpColumnRef(
    struct qryData *queryData,
    struct columnReference *referencePtr
  ) {

  struct expression *expressionPtr = NULL, *expressionColumnPtr;
  struct inputColumn *columnPtr = NULL;

  MAC_YIELD
  
  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg((void**)(&expressionPtr), sizeof(struct expression));

  expressionPtr->value = NULL;

  if(referencePtr->referenceType == REF_COLUMN) {
    expressionPtr->type = EXP_COLUMN;

    /* unbox csv column reference */
    columnPtr = referencePtr->reference.columnPtr;
    
    expressionPtr->unionPtrs.voidPtr = (void *)(columnPtr);
    expressionPtr->minColumn = columnPtr->columnIndex;
    expressionPtr->minTable = ((struct inputTable*)columnPtr->inputTablePtr)->fileIndex;

    /* this line might need to be changed */
    expressionPtr->containsAggregates = FALSE;
  }
  else {
    expressionPtr->type = EXP_CALCULATED;

    /* unbox calculated expression column */
    expressionColumnPtr = referencePtr->reference.calculatedPtr.expressionPtr;

    /* TODO: fill out these 3 fields with the correct value */
    expressionPtr->unionPtrs.voidPtr = (void *)(expressionColumnPtr);
    expressionPtr->minColumn = expressionColumnPtr->minColumn;
    expressionPtr->minTable = expressionColumnPtr->minTable;

    /* this line might need to be changed */
    expressionPtr->containsAggregates = expressionColumnPtr->containsAggregates;
  }
  
  return expressionPtr;
}
#include "querycsv.h"

/* returns true if the column could not be found or is ambiguious */
int parse_columnRefUnsuccessful(
    struct qryData *queryData,
    struct columnReference **result,
    char *tableName,
    char *columnName
  ) {

  struct columnReference *currentReference = NULL;

  MAC_YIELD

  /* ensure we have built up the hash table with all data from the source csv files first */
  if(queryData->parseMode != 1) {
    free(tableName);
    free(columnName);
    
    *result = NULL;
    return FALSE; /* successful, but only because we don't really care about getting column references when in parse mode 0 */
  }

  /* look up the specified column name in the hash table */
  currentReference = hash_lookupString(queryData->columnReferenceHashTable, columnName);
  
  /* if a table name was specified, try to find the first column reference */
  /* in the list thats of type 1 with a matching table name and return that */
  if(tableName != NULL) {
    while (currentReference != NULL) {
      if(
          currentReference->referenceType == 1 &&
          strcmp(tableName, ((struct inputTable*)(currentReference->reference.columnPtr->inputTablePtr))->queryTableName) == 0
        ) {
        break;
      }

      /* the table name didn't match on this loop iteration. maybe the next one? */
      currentReference = currentReference->nextReferenceWithName;
    }
  }
  
  /* does the match contain a subsequent match of the same type? */
  /* if the match contains a subsequent match of the same type, then the name is ambiguous so trigger an error */
  else if(
      currentReference != NULL &&
      currentReference->nextReferenceWithName != NULL &&
      currentReference->nextReferenceWithName->referenceType == currentReference->referenceType
    ) {
      
    /* the column name was ambiguous */
    currentReference = NULL;
  }

  free(tableName);
  free(columnName);

  /* if the name refers to a column which hasn't yet been put */
  /* into the output result set then stick it in there now */
  if(
      currentReference != NULL &&
      currentReference->referenceType == REF_COLUMN &&
      currentReference->reference.columnPtr->firstResultColumn == NULL
    ) {
    /* attach this output column reference to the input table column, so we'll */
    /* know where to output it when running the query */
    currentReference->reference.columnPtr->firstResultColumn = parse_newOutputColumn(
        queryData,
        /*isHidden = */TRUE,
        /*isCalculated = */FALSE,
        /*resultColumnName = */NULL,
        /*aggregationType = */GRP_NONE
      );
  }
  
  *result = currentReference;
  return currentReference == NULL;
}
#include "querycsv.h"

struct columnReferenceHash *hash_createTable(int size) {
  struct columnReferenceHash *new_table;

  int i;

  MAC_YIELD

  if (size < 1) {
    return NULL; /* invalid size for table */
  }

  /* Attempt to allocate memory for the table structure */
  if ((new_table = malloc(sizeof(struct columnReferenceHash))) == NULL) {
      return NULL;
  }
    
  /* Attempt to allocate memory for the table itself */
  if ((new_table->table = malloc(sizeof(struct columnReference *) * size)) == NULL) {
      return NULL;
  }

  /* Initialize the elements of the table */
  for(i=0; i<size; i++) {
    new_table->table[i] = NULL;
  }

  /* Set the table's size */
  new_table->size = size;

  return new_table;
}
#include "querycsv.h"

void parse_tableFactor(
    struct qryData *queryData,
    int isLeftJoin,
    char *fileName,
    char *tableName
  ) {

  FILE *csvFile;
  struct inputTable *newTable = NULL;
  struct columnReference *currentReference = NULL;
  struct columnReference *newReference;
  struct inputColumn *newColumn;
  char *columnText = NULL;
  char *columnText2 = NULL;
  size_t columnLength = 0;
  int recordContinues = TRUE;
  long headerByteLength = 0;

  MAC_YIELD

  /* ensure we are trying to open all the files we need */
  if(queryData->parseMode != 0) {
    free(fileName);
    free(tableName);
    return;
  }

  /* try to prevent heap fragmentation by shuffing the */
  csvFile = fopen(fileName, "rb");
  columnText = strdup(tableName);
  columnText2 = strdup(fileName);

  if(csvFile == NULL || columnText == NULL || columnText2 == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    exit(EXIT_FAILURE);
  }

  free(fileName);
  free(tableName);
  fclose(csvFile);
  
  /* try opening the file specified in the query */
  csvFile = skipBom(columnText2, &headerByteLength);
  tableName = strdup(columnText);
  fileName = strdup(columnText2);
  free(columnText);
  free(columnText2);  /* free the filename string data as we don't need it any more */

  columnText = NULL;

  if(csvFile == NULL || tableName == NULL || fileName == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    exit(EXIT_FAILURE);
  }

  /* allocate the storage space for the new table's data */
  reallocMsg((void**)(&newTable), sizeof(struct inputTable));

  /* start populating our newly created table record */
  newTable->queryTableName = tableName;
  newTable->fileStream = csvFile;
  newTable->firstInputColumn = NULL;  /* the table initially has no columns */
  newTable->isLeftJoined = FALSE;
  newTable->noLeftRecord = TRUE;   /* set just for initialsation purposes */

  /* add the new table to the linked list in the query data */
  if(queryData->firstInputTable == NULL) {
    newTable->fileIndex = 1;
    newTable->nextInputTable = newTable;
    queryData->firstInputTable = newTable;
  }
  else {
    newTable->fileIndex = 0; /*  we have to fill the file indexes in afterwards because of left join reordering (queryData->firstInputTable->fileIndex)+1; */

    if(isLeftJoin) {
      newTable->isLeftJoined = TRUE;

      if(queryData->secondaryInputTable == NULL) {
        queryData->secondaryInputTable = newTable;
        newTable->nextInputTable = queryData->firstInputTable->nextInputTable;
        queryData->firstInputTable->nextInputTable = newTable;
      }
      else {
        newTable->nextInputTable = queryData->secondaryInputTable->nextInputTable;
        queryData->secondaryInputTable->nextInputTable = newTable;
        queryData->secondaryInputTable = newTable;
      }
    }
    else {
      newTable->nextInputTable = queryData->firstInputTable->nextInputTable;  /* this is a circularly linked list until we've finished adding records */
      queryData->firstInputTable->nextInputTable = newTable;
      queryData->firstInputTable = newTable;
    }
  }

  /* read csv columns until end of line occurs */
  do {
    newReference = NULL;
    newColumn = NULL;

    recordContinues = getCsvColumn(
        &(newTable->fileStream),
        &columnText,
        &columnLength,
        NULL,
        &headerByteLength,
        (queryData->params & PRM_TRIM
      ) == 0);

    d_sprintf(&columnText, "_%s", columnText);

    /* test whether a column with this name already exists in the hashtable */
    currentReference = hash_lookupString(queryData->columnReferenceHashTable, columnText);

    /* if a column with this name is already in the hash table */
    if(currentReference == NULL) {
      /* create the new column record */
      reallocMsg((void**)(&newReference), sizeof(struct columnReference));

      /* store the column name */
      newReference->referenceName = columnText;

      /* no other columns with the same name yet */
      newReference->nextReferenceWithName = NULL;
     
      /* insert the column into the hashtable */
      hash_addString(queryData->columnReferenceHashTable, columnText, newReference);  
    }
    /* otherwise, add it to the linked list */
    else {
      /* we don't need another copy of this text */
      free(columnText);

      /* create the new column record */
      reallocMsg((void**)(&newReference), sizeof(struct columnReference));

      /* store the column name */
      newReference->referenceName = currentReference->referenceName;
      
      /* if there's only been 1 column with this name up until now */
      if(currentReference->nextReferenceWithName == NULL) {
        newReference->nextReferenceWithName = newReference;
        currentReference->nextReferenceWithName = newReference;
      }
      else {
        newReference->nextReferenceWithName = currentReference->nextReferenceWithName->nextReferenceWithName;
        currentReference->nextReferenceWithName->nextReferenceWithName = newReference;
        currentReference->nextReferenceWithName = newReference;
      }
    }

    newReference->referenceType = REF_COLUMN;

    reallocMsg((void**)(&newColumn), sizeof(struct inputColumn));

    newReference->reference.columnPtr = newColumn;

    newColumn->fileColumnName = newReference->referenceName;

    /* store the column's reference back to it's parent table */
    newColumn->inputTablePtr = (void*)newTable;

    /* we don't yet know whether this column will actually be used in the output query, so indicate this for stage 2's use */
    newColumn->firstResultColumn = NULL;

    /* populate the columnIndex and nextColumnInTable fields */
    if(newTable->firstInputColumn == NULL) {
      newColumn->columnIndex = 1;
      newColumn->nextColumnInTable = newColumn;
      newTable->firstInputColumn = newColumn;
    }
    else {
      newColumn->columnIndex = (newTable->firstInputColumn->columnIndex)+1;
      newColumn->nextColumnInTable = newTable->firstInputColumn->nextColumnInTable;  /* this is a circularly linked list until we've finished adding records */
      newTable->firstInputColumn->nextColumnInTable = newColumn;
      newTable->firstInputColumn = newColumn;
    }

    /* clean up for obtaining the next column */
    columnText = NULL;
    columnLength = 0;
  } while(recordContinues);

  /* keep the current offset of the csv file as we'll need it when we're searching for matching results. avoid using ftell as it doesn't work in cc65 */
  newTable->firstRecordOffset = headerByteLength;

  /* keep an easy to retrieve count of the number of columns in the csv file */
  newTable->columnCount = newTable->firstInputColumn->columnIndex;

  /* cut the circularly linked list of columns in this table */
  newColumn = newTable->firstInputColumn->nextColumnInTable;
  newTable->firstInputColumn->nextColumnInTable = NULL;
  newTable->firstInputColumn = newColumn;
}
#include "querycsv.h"

void readParams(char *string, int *params) {

  MAC_YIELD

  if(string && params) {
    while(*string) {
      switch(*string) {
        case 'p':
        case 'P':
          *params |= PRM_TRIM;
        break;

        case 't':
        case 'T':
          *params |= PRM_SPACE;
        break;
        case 'i':
        case 'I':
          *params |= PRM_IMPORT;
        break;

        case 'e':
        case 'E':
          *params |= PRM_EXPORT;
        break;

        case 'b':
        case 'B':
          *params |= PRM_BOM;
        break;
      }
      string++;
    }
  }
}
#include "querycsv.h"

/* checks whether the end of file is the next character in the stream. */
/* Used by getMatchingRecord */
int endOfFile(FILE* stream) {
  int c;

  MAC_YIELD

  c = fgetc(stream);
  ungetc(c, stream);

  return c != EOF;
}
#include "querycsv.h"

int strNumberCompare(char *input1, char *input2) {
  char* string1 = input1;
  char* string2 = input2;
  int decimalNotFound1 = TRUE;
  int decimalNotFound2 = TRUE;
  int compare = 0;
  int negate = 1;
  int do1 = TRUE;
  int do2 = TRUE;

  MAC_YIELD
  
  /* if the first character of only one of the numbers is negative, */
  /* then the positive one is greater. We still need to skip the digits though */
  if(*string1 == '-') {
    string1++;

    if(*string2 != '-') {
      if(*string2 == '+') {
        string2++;
      }
      
      compare = -1;
    }
    else {
      /* if both are negative, then the subsequent results need to be reversed */
      negate = -1;
      string2++;
    }
  }
  else {
    if(*string1 == '+') {
      string1++;
    }

    if(*string2 == '-') {
      string2++;
      compare = 1;
    }
    else if(*string2 == '+') {
      string2++;
    }
  }

  if(compare == 0) {
    /* skip the leading zeros of both numbers */
    while(*string1 == '0') {
      string1++;
    }

    while(*string2 == '0') {
      string2++;
    }

    input1 = string1;
    input2 = string2;

    /* the number whose digits finish first is the smaller. */
    /* We still need to skip the remaining digits though */
    while(compare == 0) {
      if(*string1 > '9' || *string1 < '0') {
        if(*string2 > '9' || *string2 < '0') {
          /* if both numbers finish at the same time then */
          /* rewind and look at the individual digits */
          /* we don't need to skip the remaining digits */
          string1 = input1;
          string2 = input2;

          for ( ; ; ) {
            if(do1) {
              if(decimalNotFound1 && (*string1 == '.' || *string1 == ',')) {
                decimalNotFound1 = FALSE;
              }
              else if(*string1 > '9' || *string1 < '0') {
                do1 = FALSE;
              }
              else if (do2 == FALSE && *string1 != '0') {
                compare = -1;
                break;
              }
            }

            if(do2) {
              if(decimalNotFound2 && (*string2 == '.' || *string2 == ',')) {
                decimalNotFound2 = FALSE;
              }
              else if(*string2 > '9' || *string2 < '0') {
                do2 = FALSE;
              }
              else if (do1 == FALSE && *string2 != '0') {
                compare = 1;
                break;
              }
            }

            if(do1) {
              if(do2) {
                /* neither number has finished yet. */
                if(*string1 >= '0' &&
                    *string1 <= '9' &&
                    *string2 >= '0' &&
                    *string2 <= '9' &&
                    *string1 != *string2) {
                  compare = *string1 < *string2?-1:1;
                  break;
                }

                string2++;
              }

              string1++;
            }
            else if(do2) {
              string2++;
            }
            else {
              /* both numbers have finished */
              /* the numbers are entirely equal */

              return 0;
            }
          }
        }
        else {
          compare = -1;
        }
      }
      else if(*string2 > '9' || *string2 < '0') {
        compare = 1;
      }
      else {
        string1++;
        string2++;
      }
    }
  }

  return compare * negate;
}
#include "querycsv.h"

long getUnicodeChar(
    unsigned char **offset,
    unsigned char **str,
    int plusBytes,
    int *bytesMatched,
    void (*get)()
  ) {

  struct hash2Entry* entry = NULL;
  int bytesread = 0;
  long codepoint;
  unsigned char *temp = (unsigned char *)((*offset) + plusBytes);

  MAC_YIELD

  /* if the current byte offset is a valid utf-8 character that's not overlong or decomposable then return it */
  if(*temp < 0x80) {
    /* read 1 byte. no overlong checks needed as a 1 byte code can */
    /* never be overlong, and is never a combining character */
    *bytesMatched = 1;
    
    return (long)(*temp);
  }
  /* ensure the current byte is the start of a valid utf-8 sequence */
  else if(*temp > 0xC1) {
    if (*temp < 0xE0) { 
      /* read 2 bytes */
      if(
          (*(temp+1) & 0xC0) == 0x80
      ) {
        codepoint = ((long)(*(temp)) << 6) + *(temp+1) - 0x3080;
        
        /* the codepoint is valid. but is it decomposable? */
        if((entry = isInHash2(codepoint))) {
          bytesread = 2;
        }
        else {
          /* otherwise return it */
          *bytesMatched = 2;
          
          return codepoint;
        }
      }
    }
    else if (*temp < 0xF0) {
      /* read 3 bytes */
      if(
          (*(temp+1) & 0xC0) == 0x80 &&
          (*(temp) != 0xE0 || *(temp+1) > 0x9F) &&
          (*(temp+2) & 0xC0) == 0x80
      ) {
        codepoint = ((long)(*(temp)) << 12) + ((long)(*(temp+1)) << 6) + (*(temp+2)) - 0xE2080;

        /* the codepoint is valid. but is it decomposable? */
        if((entry = isInHash2(codepoint))) {
          bytesread = 3;
        }
        else {
          /* otherwise return it */
          *bytesMatched = 3;
          
          return codepoint;
        }
      }
    }
    else if (*temp < 0xF5) {
      /* read 4 bytes */
      if(
          (*(temp+1) & 0xC0) == 0x80 &&
          (*temp != 0xF0 || *(temp+1) > 0x8F) &&
          (*temp != 0xF4 || *(temp+1) < 0x90) &&
          (*(temp+2) & 0xC0) == 0x80 &&
          (*(temp+3) & 0xC0) == 0x80
      ) {
        codepoint = (((long)(*temp)) << 18) + ((long)(*(temp+1)) << 12) + ((long)(*(temp+2)) << 6) + (*(temp+3)) - 0x3C82080;

        /* the codepoint is valid. but is it decomposable? */
        if((entry = isInHash2(codepoint))) {
          bytesread = 4;
        }
        else {
          /* otherwise return it */
          *bytesMatched = 4;
          
          return codepoint;
        }
      }
    }
  }

  /* switch to the fast getUnicodeChar */
  get = (void (*)())&getUnicodeCharFast;

  /* invalid bytes or overlong and decomposable codepoints mean the string needs to be NFD normalized. */
  return normaliseAndGet(offset, str, plusBytes, bytesMatched, bytesread, entry);
}


#include "querycsv.h"

void outputHeader(
    struct qryData *query
  ) {
  int firstColumn = TRUE;
  struct resultColumn *currentResultColumn;
  FILE *outputFile;
  char *separator = (((query->params) & PRM_SPACE) != 0)?",":", ";

  MAC_YIELD

  /* set up the output context */
  if(query->intoFileName) {
    query->newLine = "\r\n";
    query->outputFile = fopen(query->intoFileName, "wb");

    if (query->outputFile == NULL) {
      fputs("opening output file failed", stderr);
      return;
    }
  }
  else {
    query->outputFile = stdout;
    query->newLine = "\n";
  }

  outputFile = query->outputFile;

  /* write the byte order mark if it was requested */
  if(((query->params) & PRM_BOM) != 0) {
    fputs("\xEF\xBB\xBF", outputFile);
  }

  /* write column headers to the output file */
  for(
      currentResultColumn = query->firstResultColumn;
      currentResultColumn != NULL;
      currentResultColumn = currentResultColumn->nextColumnInResults
    ) {

    if(currentResultColumn->isHidden == FALSE) {
      if (!firstColumn) {
        fputs(separator, outputFile);
      }
      else {
        firstColumn = FALSE;
      }

      /* strip over the leading underscore */
      fputs((currentResultColumn->resultColumnName)+1, outputFile);
    }
  }

  fputs(query->newLine, outputFile);
}
#include "querycsv.h"

int combiningCharCompare(const void *a, const void *b) {
  MAC_YIELD
  
  return (*((long*)a) < *((long*)b)) ? -1 : (*((long*)a) != *((long*)b) ? 1 : 0);
}
#include "querycsv.h"

void getCalculatedColumns(
    struct qryData *query,
    struct resultColumnValue *match,
    int runAggregates
  ) {

  int i, j;
  struct columnReference *currentReference;
  struct resultColumn *currentResultColumn;
  struct columnRefHashEntry *currentHashEntry;
  struct resultColumnParam matchParams;

  MAC_YIELD

  matchParams.ptr = match;
  matchParams.params = query->params;

  for(i = 0; i < query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {

      currentReference = currentHashEntry->content;

      while(currentReference != NULL) {
        if(
            currentReference->referenceType == REF_EXPRESSION &&
            currentReference->reference.calculatedPtr.expressionPtr->containsAggregates == runAggregates &&

            /* get the current result column */
            (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL
          ) {

          /* start setting column value fields */
          j = currentResultColumn->resultColumnIndex;
          match[j].isQuoted = FALSE;
          match[j].isNormalized = FALSE;
          /* match[j].source = &(query->scratchpad); */

          /* seek to the end of the scratchpad file and update the start position */
          /* fseek(query->scratchpad, 0, SEEK_END); */
          /* fflush(query->scratchpad); */
          match[j].startOffset = 0;   /* ftell(query->scratchpad); */

          /* get expression value for this match */
          getValue(currentReference->reference.calculatedPtr.expressionPtr, &matchParams);

          /* store the value's length */
          if(currentReference->reference.calculatedPtr.expressionPtr->leftNull) {
            match[j].length = 0;
            match[j].leftNull = TRUE;
            match[j].value = strdup("");
          }
          else {
            match[j].leftNull = FALSE;
            match[j].length = strlen(currentReference->reference.calculatedPtr.expressionPtr->value);
            match[j].value = currentReference->reference.calculatedPtr.expressionPtr->value;
            
            /* write the value to the scratchpad file */
            /* fputs(currentReference->reference.calculatedPtr.expressionPtr->value, query->scratchpad); */
          }

          /* free the expression value for this match */
          /* strFree(&(currentReference->reference.calculatedPtr.expressionPtr->value)); */
          currentReference->reference.calculatedPtr.expressionPtr->value = NULL;
        }

        currentReference = currentReference->nextReferenceWithName;
      }

      /* go to the next reference in the hash table */
      currentHashEntry = currentHashEntry->nextReferenceInHash;
    }
  }
}
#include "querycsv.h"

int main(int argc, char *argv[]) {
  int argc2;
  char ** argv2;

  argc2 = argc;
  argv2 = argv;

  #ifdef WINDOWS
    setupWin32(&argc2, &argv2);
  #endif

  #ifdef __CC_NORCROFT
    setupRiscOS(&argc2, &argv2);
  #endif

  /* supply a default temporary folder if none is present */
  if(getenv(TEMP_VAR) == NULL) {
    putenv(DEFAULT_TEMP);
  }

  /* supply some default timezone data if none is present */
  if(getenv("TZ") == NULL) {
    putenv(TDB_DEFAULT_TZ);
  }

  /* set the locale (among other things, this applies the */
  /* timezone data to the date functions) */
  setlocale(LC_ALL, TDB_LOCALE);

  /* identify whether to run a script or display the usage message */
  if(argc2 == 2) {
    return runQuery(argv2[1]);
  }

  /* something else. print an error message and quit */
  fputs(TDB_INVALID_COMMAND_LINE, stderr);

  return -1;
}
#include "querycsv.h"

void groupResults(struct qryData *query) {
  struct resultTree *resultsOrig;
  struct resultColumn *currentResultColumn;

  MAC_YIELD

  /* backup the original result set */
  resultsOrig = query->resultSet;
  query->resultSet = NULL;

  /* keep a reference to the current record */
  query->match = getFirstRecord(
    resultsOrig,
    query
  );

  /* store a copy of the first record and initialise the running totals */
  updateRunningCounts(query, query->match);

  tree_walkAndCleanup(
    query,
    &(resultsOrig),
    &groupResultsInner
  );

  /* mark each column as having had grouping performed */
  currentResultColumn = query->firstResultColumn;
  while(currentResultColumn != NULL) {
    currentResultColumn->groupingDone = TRUE;
    currentResultColumn = currentResultColumn->nextColumnInResults;
  }
}
#include "querycsv.h"

int getCsvColumn(
    FILE **inputFile,
    char **value,
    size_t *strSize,
    int *quotedValue,
    long *startPosition,
    int doTrim
  ) {

  int c;
  char *tempString = NULL;
  int canEnd = TRUE;
  int quotePossible = TRUE;
  int exitCode = 0;
  char *minSize = NULL;
  long offset = 0;

  MAC_YIELD

  if(quotedValue != NULL) {
    *quotedValue = FALSE;
  }

  if(strSize != NULL) {
    *strSize = 0;
  }

  if(value == NULL) {
    value = &tempString;
  }

  if(startPosition != NULL) {
    offset = *startPosition;
  }

  if(feof(*inputFile) != 0) {
    return FALSE;
  }

  /* read a character */
  do {
    offset++;
    c = fgetc(*inputFile);

    switch(c) {
      case ' ':
        if(!canEnd) {
          minSize = &((*value)[*strSize]);
        }
        strAppend(' ', value, strSize);
      break;

      case '\r':
        offset++;
        if((c = fgetc(*inputFile)) != '\n' && c != EOF) {
          offset--;
          ungetc(c, *inputFile);
        }
        else if (c == EOF) {
          exitCode = 2;
          break;
        }

      case '\n':
        if(canEnd) {
          exitCode = 2;
          break;
        }
        else {
          if (quotedValue != NULL) {
            *quotedValue = TRUE;
          }
          strAppend('\r', value, strSize);
          strAppend('\n', value, strSize);
        }
      break;

      case '\0':
        if (quotedValue != NULL) {
          *quotedValue = TRUE;
        }
      break;

      case EOF:
        exitCode = 2;
        break;
      break;

      case '"':
        canEnd = FALSE;

        if (quotedValue != NULL) {
          *quotedValue = TRUE;
        }

        if(quotePossible) {
          if(strSize != NULL) {
            *strSize = 0;
          }

          quotePossible = FALSE;
        }
        else {
          offset++;
          c = fgetc(*inputFile);

          switch(c) {
            case ' ':
            case '\r':
            case '\n':
            case EOF:
            case ',':
              canEnd = TRUE;
              offset--;
              ungetc(c, *inputFile);
            break;

            case '"':
              strAppend('"', value, strSize);
            break;

            default:
              strAppend('"', value, strSize);
              offset--;
              ungetc(c, *inputFile);
            break;
          }
        }
      break;

      case ',':
        if(canEnd) {
          exitCode = 1;
          break;
        }

      default:
        if(doTrim && quotePossible) {
          if(strSize != NULL) {
            *strSize = 0;
          }
        }

        quotePossible = FALSE;
        strAppend(c, value, strSize);
      break;
    }
  } while (exitCode == 0);

  if(doTrim) {
    strRTrim(value, strSize, minSize);
  }

  strAppend('\0', value, strSize);

  if(strSize != NULL) {
    (*strSize)--;
  }

  if(startPosition != NULL) {
    *startPosition = offset;
  }

  free(tempString);
  return exitCode == 1;
}
#include "querycsv.h"

void parse_whereClause(
    struct qryData *queryData,
    struct expression *expressionPtr
) {
  struct expression *currentClauses;
  struct expression *newClause;

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return;
  }

  if(queryData->joinsAndWhereClause == NULL) {
    /* first join or where clause. just use it dirrectly */
    queryData->joinsAndWhereClause = expressionPtr;
  }
  else {
    /* get current clauses */
    currentClauses = queryData->joinsAndWhereClause;

    newClause = NULL;

    reallocMsg(
        (void**)&(newClause),
        sizeof(struct expression)
      );

    newClause->type = EXP_AND;
    newClause->value = NULL;

    if(
        expressionPtr->minTable < currentClauses->minTable ||
        (expressionPtr->minTable == currentClauses->minTable &&
        expressionPtr->minColumn < currentClauses->minColumn)
      ) {
      newClause->unionPtrs.leaves.leftPtr = expressionPtr;
      newClause->unionPtrs.leaves.rightPtr = currentClauses;

      /* The minimum table and column needed to evaluate this */
      /* sub-expression is the greater of the two operands */
      newClause->minColumn = currentClauses->minColumn;
      newClause->minTable = currentClauses->minTable;
    }
    else {
      newClause->unionPtrs.leaves.leftPtr = currentClauses;
      newClause->unionPtrs.leaves.rightPtr = expressionPtr;

      /* The minimum table and column needed to evaluate this */
      /* sub-expression is the greater of the two operands */
      newClause->minColumn = expressionPtr->minColumn;
      newClause->minTable = expressionPtr->minTable;
    }

    queryData->joinsAndWhereClause = newClause;
  }
}
#include "querycsv.h"

void parse_groupingSpec(
    struct qryData *queryData,
    struct expression *expressionPtr
  ) {
      
  struct sortingList *sortingListPtr = NULL;

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return;
  }
  
  reallocMsg(
      (void**)&(sortingListPtr),
      sizeof(struct sortingList)
    );

  sortingListPtr->expressionPtr = expressionPtr;
  sortingListPtr->isDescending = FALSE;

  if(queryData->groupByClause == NULL) {
    sortingListPtr->nextInList = sortingListPtr;
    queryData->groupByClause = sortingListPtr;
  }
  else {
    sortingListPtr->nextInList = queryData->groupByClause->nextInList;
    queryData->groupByClause->nextInList = sortingListPtr;
    queryData->groupByClause = sortingListPtr;
  }
}


#include "querycsv.h"

void cleanup_inputColumns(struct inputColumn *currentInputColumn) {
  struct inputColumn *next;

  MAC_YIELD

  while(currentInputColumn != NULL) {
    next = currentInputColumn->nextColumnInTable;

    /* free(currentInputColumn->fileColumnName); will be done by the column references instead */
    free(currentInputColumn);

    currentInputColumn = next;
  }
}
#include "querycsv.h"

void cleanup_atomList(struct atomEntry *currentAtom) {
  struct atomEntry* next;

  MAC_YIELD

  while(currentAtom != NULL) {
    next = currentAtom->nextInList;

    free(currentAtom->content);
    free(currentAtom);

    currentAtom = next;
  }
}
#include "querycsv.h"

void exp_uminus(char **value, double leftVal) {
  double temp;

  MAC_YIELD

  temp = ctof(0);

  if(fcmp(leftVal, temp)) {
    temp = fneg(leftVal);

    ftostr(value, temp);
  }
  else {
    *value = strdup("0");
  }
}
#include "querycsv.h"

void hash_freeTable(struct columnReferenceHash *hashtable) {
  int i;
  struct columnRefHashEntry *list, *temp;

  MAC_YIELD

  if (hashtable==NULL) {
    return;
  }

  /* Free the memory for every item in the table, including the 
   * strings themselves.
   */
  for(i=0; i<hashtable->size; i++) {
      list = hashtable->table[i];
      while(list!=NULL) {
          temp = list;
          list = list->nextReferenceInHash;
          free(temp->referenceName);
          free(temp);
      }
  }

  /* Free the table itself */
  free(hashtable->table);
  free(hashtable);
}
#include "querycsv.h"

struct expression *parse_scalarExp(
    struct qryData *queryData,
    struct expression *leftPtr,
    int operator,
    struct expression *rightPtr
  ) {

  struct expression *expressionPtr = NULL;

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg((void**)(&expressionPtr), sizeof(struct expression));

  expressionPtr->type = operator;
  expressionPtr->value = NULL;

  if(rightPtr == NULL) {
    expressionPtr->unionPtrs.leaves.leftPtr = leftPtr;
    expressionPtr->unionPtrs.leaves.rightPtr = NULL;

    expressionPtr->minColumn = leftPtr->minColumn;
    expressionPtr->minTable = leftPtr->minTable;

    expressionPtr->containsAggregates = leftPtr->containsAggregates;
  }
  
  else if(rightPtr->minTable < leftPtr->minTable ||
    (rightPtr->minTable == leftPtr->minTable &&
    rightPtr->minColumn < leftPtr->minColumn)
  ) {
      
    /* if the operator is commutable, then re-order so that the minimum table reference is on the left */
    if(
        operator == EXP_PLUS ||
        operator == EXP_MULTIPLY ||
        operator == EXP_AND ||
        operator == EXP_OR
      ) {
      expressionPtr->unionPtrs.leaves.leftPtr = rightPtr;
      expressionPtr->unionPtrs.leaves.rightPtr = leftPtr;
    }
    else {
      expressionPtr->unionPtrs.leaves.leftPtr = leftPtr;
      expressionPtr->unionPtrs.leaves.rightPtr = rightPtr;
    }

    /* The minimum table and column needed to evaluate this */
    /* sub-expression is the greater of the two operands */
    expressionPtr->minColumn = leftPtr->minColumn;
    expressionPtr->minTable = leftPtr->minTable;

    expressionPtr->containsAggregates = (leftPtr->containsAggregates) || (rightPtr->containsAggregates);
  }
  else {
    expressionPtr->unionPtrs.leaves.leftPtr = leftPtr;
    expressionPtr->unionPtrs.leaves.rightPtr = rightPtr;
    expressionPtr->minColumn = rightPtr->minColumn;
    expressionPtr->minTable = rightPtr->minTable;

    expressionPtr->containsAggregates = (leftPtr->containsAggregates) || (rightPtr->containsAggregates);
  }

  return expressionPtr;
}

#include "querycsv.h"

void cleanup_columnReferences(struct columnReferenceHash *table) {
  int i;
  struct columnRefHashEntry *currentHashEntry, *nextHashEntry;
  struct columnReference *currentReference, *nextReference;

  MAC_YIELD

  for(i=0; i<table->size; i++) {
    currentHashEntry = table->table[i];

    while(currentHashEntry != NULL) {
      nextHashEntry = currentHashEntry->nextReferenceInHash;

      free(currentHashEntry->referenceName);
      currentReference = currentHashEntry->content;
      free(currentHashEntry);

      while(currentReference != NULL) {
        nextReference = currentReference->nextReferenceWithName;

        if(currentReference->referenceType == REF_COLUMN) {
          free(currentReference->reference.columnPtr);
        }

        if(currentReference->referenceType == REF_EXPRESSION) {
          cleanup_expression(currentReference->reference.calculatedPtr.expressionPtr);
        }

        free(currentReference);

        currentReference = nextReference;
      }

      currentHashEntry = nextHashEntry;
    }
  }

  free(table->table);
  free(table);
}
#include "querycsv.h"

struct expression *parse_inPredicate(
    struct qryData *queryData,
    struct expression *leftPtr,
    int isNotIn,
    struct atomEntry *lastEntryPtr
  ) {

  struct expression *expressionPtr = NULL;

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg((void**)(&expressionPtr), sizeof(struct expression));

  expressionPtr->type = isNotIn==TRUE?EXP_NOTIN:EXP_IN;
  expressionPtr->value = NULL;
  expressionPtr->unionPtrs.inLeaves.leftPtr = leftPtr;
  expressionPtr->unionPtrs.inLeaves.lastEntryPtr = lastEntryPtr;
  expressionPtr->minColumn = leftPtr->minColumn;
  expressionPtr->minTable = leftPtr->minTable;

  expressionPtr->containsAggregates = leftPtr->containsAggregates;

  return expressionPtr;
}

#include "querycsv.h"

void exp_divide(char **value, double leftVal, double rightVal) {
  double temp;
  
  MAC_YIELD

  temp = ctof(0);
  
  if(fcmp(rightVal, temp)) {
    temp = fdiv(leftVal, rightVal);

    ftostr(value, temp);
  }
  else {
    *value = strdup("Infinity");
  }
}
#include "querycsv.h"

int strCompare(
    unsigned char **str1,
    unsigned char **str2,
    int caseSensitive,
    void (*get1)(),
    void (*get2)()
  ) {

  unsigned char *offset1 = *str1, *offset2 = *str2;
  long char1 = 0, char2 = 0;
  struct hash4Entry *entry1, *entry2;
  int firstChar = TRUE, comparison = 0, char1found = FALSE;
  int bytesMatched1 = 0, bytesMatched2 = 0;
  int accentcheck = 0, combinerResult;

  MAC_YIELD

  for( ; ; ) {  /* we'll quit from this function via other means */
    /* check if we've reached the end of string 2 */
    if (*offset2 == 0) {
      /* if string2 is on the slower version of getUnicodeChar, */
      /* there were no weird characters, so we can switch to always using the faster version  */

      /* if they both are null then the strings are equal. otherwise string 2 is lesser */
      if(*offset1 == 0) {
        if(accentcheck == 0) {
          return 0;
        }
        else {
          /* a difference just on the accents on a letter were found. re-compare with accent checking enabled. */
          accentcheck = 2;
          if(caseSensitive == 2) {
            caseSensitive = 1;
          }
          offset1 = *str1;
          offset2 = *str2;
          continue;
        }
      }
      
      return 1;
    }
    
    /* check if we've reached the end of string 1 */
    else if(*offset1 == 0) {
      /* if they both are null then the strings are equal. otherwise string 1 is lesser */
      return -1;
    }

    /* character 1 has not yet been found */
    else if (char1found == 0) {
      /* read a character from string 1 */
      char1 = (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get1))(&offset1, str1, 0, &bytesMatched1, get1);

      if (char1 != 0x34F) {
        /* read a character from string 2 */
        char2 = (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))(&offset2, str2, 0, &bytesMatched2, get2);

        if((entry1 = getLookupTableEntry(&offset1, str1, &bytesMatched1, get1, firstChar))) {
          /* the first character is in the lookup table */

          if(char2 != 0x34F) {
            if((entry2 = getLookupTableEntry(&offset2, str2, &bytesMatched2, get2, firstChar))) {
              /* compare the lookup table entries */
          
              if(entry1->script == entry2->script) {
                if(entry1->index == 0 && entry2->index == 0) {
                  /* both entries are numbers, so compare them */
                  comparison = strNumberCompare((char *)offset1, (char *)offset2);
                }
                else if(caseSensitive == 1) {
                  comparison = entry1->index - entry2->index;
                }
                else {
                  comparison = (entry1->index - (entry1->islower)) - (entry2->index - (entry2->islower));

                  if((entry1->index - entry2->index) != 0 && caseSensitive == 2) {
                    accentcheck = 1;
                  }
                }
              
                if(comparison != 0) {
                  return comparison > 0 ? 1 : -1;
                }
              }
              else {
                /* scripts are ordered */
                return entry1->script > entry2->script ? 1 : -1;
              }
            }
            /* compare codepoints */
            else if(entry1->script != char2) {
              return (entry1->script > char2) ? 1 : -1;
            }

            if((combinerResult = consumeCombiningChars(
                str1, str2,
                &offset1, &offset2,
                get1, get2,
                &bytesMatched1, &bytesMatched2,
                &accentcheck)) != 0) {
              return combinerResult;
            }
        
            if(firstChar) {
              firstChar = FALSE;
            }
          }
          else {
            /* we've found the first character, but not yet the second one. */
            /* we can skip some assignments and checks on the next loop iteration */
            char1found = 1;   /* in lookup */
            offset2 += bytesMatched2;
          }
        }
        else if(char2 != 0x34F) {
          if((entry2 = getLookupTableEntry(&offset2, str2, &bytesMatched2, get2, firstChar))){
            if(char1 != entry2->script) {
              return (char1 > entry2->script) ? 1: -1;
            }
          }
          /* compare codepoints */
          else if(char1 != char2) {
            return (char1 > char2) ? 1: -1; 
          }

          if((combinerResult = consumeCombiningChars(
              str1, str2,
              &offset1, &offset2,
              get1, get2,
              &bytesMatched1, &bytesMatched2,
              &accentcheck)) != 0) {
            return combinerResult;
          }
      
          if(firstChar) {
            firstChar = FALSE;
          }
        }
        else {
          /* we've found the first character, but not yet the second one. */
          /* we can skip some assignments and checks on the next loop iteration */
          char1found = 2;   /* in lookup */
          offset2 += bytesMatched2;
        }
      }  
      else {
        /* we've not yet found the first codepoint to compare. move to the next one */
        offset1 += bytesMatched1;
      }
    }

    /* character 1 has been found and is in the lookup table */
    else if (char1found == 1) {
      /* read a character from string 2 */
      char2 = (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))(&offset1, str1, 0, &bytesMatched2, get2);

      if(char2 != 0x34F) {
        if((entry2 = getLookupTableEntry(&offset2, str2, &bytesMatched2, get2, firstChar))) {
          /* both characters have been found and are in the lookup table. compare the lookup table entries           */
      
          if(entry1->script == entry2->script) {
            if(entry1->index == 0 && entry2->index == 0) {
              /* both entries are numbers, so compare them */
              comparison = strNumberCompare((char *)offset1, (char *)offset2);
            }
            else if(caseSensitive == 1) {
              comparison = entry1->index - entry2->index;
            }
            else {
              comparison = (entry1->index - (entry1->islower)) - (entry2->index - (entry2->islower));

              if((entry1->index - entry2->index) != 0 && caseSensitive == 2) {
                accentcheck = 1;
              }
            }
          
            if(comparison != 0) {
              return comparison > 0 ? 1 : -1;
            }
          }
          else {
            /* scripts are ordered */
            return entry1->script > entry2->script ? 1 : -1;
          }
        }
        /* compare the codepoints */
        else if(entry1->script != char2) {
          return (entry1->script > char2) ? 1: -1; 
        }

        if((combinerResult = consumeCombiningChars(
            str1, str2,
            &offset1, &offset2,
            get1, get2,
            &bytesMatched1, &bytesMatched2,
            &accentcheck)) != 0) {
          return combinerResult;
        }

        if(firstChar) {
          firstChar = FALSE;
        }

        char1found = 0;
      }
      else {
        offset2 += bytesMatched2;
      }
    }
    
    /* character 1 has been found but was not in the lookup table */
    else {  
      char2 = (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))(&offset2, str2, 0, &bytesMatched2, get2);

      if(char2 != 0x34F) {
        /* the first or both characters were not in the lookup table. */
        /* compare the code point then successive combining characters */
        if((entry2 = getLookupTableEntry(&offset2, str2, &bytesMatched2, get2, firstChar)) && char1 != entry2->script) {
          return (char1 > entry2->script) ? 1: -1; 
        }
        /* compare codepoints */
        else if(char1 != char2) {
          return (char1 > char2) ? 1: -1; 
        }

        if((combinerResult = consumeCombiningChars(
            str1, str2,
            &offset1, &offset2,
            get1, get2,
            &bytesMatched1, &bytesMatched2,
            &accentcheck)) != 0) {
          return combinerResult;
        }

        char1found = 0;
    
        if(firstChar) {
          firstChar = FALSE;
        } 
      }
      else {
        offset2 += bytesMatched2;
      }
    }
  } 
}
#include "querycsv.h"

char *strReplace(char *search, char *replace, char *subject) {
  char *replaced = (char*)calloc(1, 1), *temp = NULL;
  char *p = subject, *p3 = subject, *p2;
  int found = 0;

  MAC_YIELD

  if(
      search == NULL ||
      replace == NULL ||
      subject == NULL ||
      strlen(search) == 0 ||
      strlen(replace) == 0 ||
      strlen(subject) == 0
    ) {
    return NULL;
  }

  while((p = strstr(p, search)) != NULL) {
    found = 1;
    temp = realloc(replaced, strlen(replaced) + (p - p3) + strlen(replace));

    if(temp == NULL) {
      free(replaced);
      return NULL;
    }

    replaced = temp;
    strncat(replaced, p - (p - p3), p - p3);
    strcat(replaced, replace);
    p3 = p + strlen(search);
    p += strlen(search);
    p2 = p;
  }

  if (found == 1) {
    if (strlen(p2) > 0) {
      temp = realloc(replaced, strlen(replaced) + strlen(p2) + 1);

      if (temp == NULL) {
        free(replaced);
        return NULL;
      }

      replaced = temp;
      strcat(replaced, p2);
    }
  }
  else {
    temp = realloc(replaced, strlen(subject) + 1);

    if (temp != NULL) {
      replaced = temp;
      strcpy(replaced, subject);
    }
  }

  return replaced;
}
#include "querycsv.h"

struct expression *parse_functionRef(
    struct qryData *queryData,
    long aggregationType,
    struct expression *expressionPtr,
    int isDistinct
  ) {

  struct expression *expressionPtr2 = NULL;
  struct resultColumn *columnPtr = NULL;

  MAC_YIELD
  
  if(queryData->parseMode != 1) {
    return NULL;
  }

  if(isDistinct) {
    aggregationType+=GRP_STAR;
  }

  if(expressionPtr->containsAggregates) {
    /* I don't think in sql you can aggregate an aggregate. */
    /* therefore we should error out if we get to this point */
    fputs("can't aggregate an aggregate", stderr);
    exit(EXIT_FAILURE);
  }

  queryData->hasGrouping = TRUE;  /* at least one, perhaps more */
  
  /* parse_expCommaList is used to put an expression into a */
  /* new, hidden, calculated column in the output result set */
  columnPtr = parse_expCommaList(queryData, expressionPtr, NULL, aggregationType);   /* parse_exp_commalist returns an output column pointer */

  /* we then want to create a new expression node that references this new column */
  /* create an expression node that references the new hidden column */
  reallocMsg((void**)(&expressionPtr2), sizeof(struct expression));

  expressionPtr2->type = EXP_GROUP;
  expressionPtr2->value = NULL;

  expressionPtr2->unionPtrs.voidPtr = (void *)(columnPtr);  /* the expression nodes reference points directly to the hidden column */
  expressionPtr2->minColumn = expressionPtr->minColumn;
  expressionPtr2->minTable = expressionPtr->minTable;

  /* change the ref type back up the expression tree to be calculated later */
  expressionPtr2->containsAggregates = TRUE;

  return expressionPtr2;
}

#include "querycsv.h"

struct resultColumn *parse_newOutputColumn(
    struct qryData *queryData,
    int isHidden2,
    int isCalculated2,
    char *resultColumnName2,
    int aggregationType
  ) {

  struct resultColumn *newResultColumn = NULL;

  MAC_YIELD

  reallocMsg((void**)(&newResultColumn), sizeof(struct resultColumn));

  /* insert this new output column into the list in the query data */
  if(queryData->firstResultColumn == NULL) {
    newResultColumn->resultColumnIndex = 0;
    queryData->firstResultColumn = newResultColumn;
    newResultColumn->nextColumnInResults = newResultColumn;
  }
  else {
    newResultColumn->resultColumnIndex = queryData->firstResultColumn->resultColumnIndex+1;
    newResultColumn->nextColumnInResults = queryData->firstResultColumn->nextColumnInResults;   /* maintain circularly linked list for now */
    queryData->firstResultColumn->nextColumnInResults = newResultColumn;
    queryData->firstResultColumn = newResultColumn;
  }
  
  /* fill out the rest of the necessary fields */
  newResultColumn->isHidden = isHidden2;
  newResultColumn->isCalculated = isCalculated2;
  newResultColumn->resultColumnName = resultColumnName2;
  newResultColumn->nextColumnInstance = NULL;   /* TODO: this field needs to be filled out properly */

  newResultColumn->groupType = aggregationType;
  newResultColumn->groupText = NULL;
  newResultColumn->groupNum = 0.0;
  newResultColumn->groupCount = 0;
  newResultColumn->groupingDone = FALSE;

  return newResultColumn;
}
#include "querycsv.h"

/* format a date into a string. allocate/free memory as needed */
int d_strftime(char **ptr, char *format, struct tm *timeptr) {
  size_t length = 32; /* starting value */
  size_t length2 = 0;
  char *output = NULL;
  char *output2 = NULL;

  MAC_YIELD

  if(ptr == NULL || format == NULL) {
    return FALSE;
  }

  while (length2 == 0) {
    output2 = realloc((void*)output, length*sizeof(char));

    if(output2 == NULL) {
      freeAndZero(output);

      return FALSE;
    }
    output = output2;

    /* calling strftime using the buffer we created */
    length2 = strftime(output, length, format, timeptr);

    /* double the allocation length to use if we need to try again */
    length *= 2;
  }

  /* shrink the allocated memory to fit the returned length */
  output2 = realloc((void*)output, (length2+1)*sizeof(char));

  /* quit if the shrinking didn't work successfully */
  if(output2 == NULL) {
    freeAndZero(output);

    return FALSE;
  }

  /* free the contents of ptr then update it to point to the string we've built up */
  freeAndZero(*ptr);
  *ptr = output2;

  /* everything completed successfully */
  return TRUE;
}
#include "querycsv.h"

int getColumnValue(
    char *inputFileName,
    long offset,
    int columnIndex
  ) {

  FILE *inputFile = NULL;
  char *output = (char*)malloc(1);
  size_t strSize = 0;
  int currentColumn = 0;

  MAC_YIELD

  /* attempt to open the input file */
  inputFile = skipBom(inputFileName, NULL);
  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    freeAndZero(output);
    return -1;
  }

  /* seek to offset */
  /* TODO. replace the fseek reperence with our own implementation as fseek doesn't work in cc65 */
  if(myfseek(inputFile, offset, SEEK_SET) != 0) {
    fputs(TDB_COULDNT_SEEK, stderr);
    freeAndZero(output);
    return -1;
  }

  /* get the text of the specified csv column (if available). */
  /* if it's not available we'll return an empty string */
  while(
        ++currentColumn != columnIndex ?
        getCsvColumn(&inputFile, NULL, NULL, NULL, NULL, TRUE):
        (getCsvColumn(&inputFile, &output, &strSize, NULL, NULL, TRUE) && FALSE)
      ) {
    /* get next column */
  }

  /* output the value */
  fputs(output, stdout);

  /* free the string memory */
  freeAndZero(output);

  /* close the input file and return */
  fclose(inputFile);
  return 0;
}
#include "querycsv.h"

void cleanup_expression(struct expression *currentExpression) {

  MAC_YIELD

  if(currentExpression != NULL) {
    switch(currentExpression->type) {
      case EXP_IN:
      case EXP_NOTIN:
        cleanup_expression(currentExpression->unionPtrs.inLeaves.leftPtr);
        cleanup_atomList(currentExpression->unionPtrs.inLeaves.lastEntryPtr);
      break;

      case EXP_COLUMN:
        /* the memory used by the input column will be freed elsewhere */
      break;

      case EXP_GROUP:
        /* the memory used here is cleaned up elsewhere */
      break;

      case EXP_LITERAL:
        free(currentExpression->unionPtrs.voidPtr);
      break;

      case EXP_CALCULATED:
        /* TODO: confirm that the expression will be freed elsewhere */
      break;

      case EXP_UMINUS:
      case EXP_UPLUS:
        cleanup_expression(currentExpression->unionPtrs.leaves.leftPtr);
      break;

      default:
        cleanup_expression(currentExpression->unionPtrs.leaves.leftPtr);
        cleanup_expression(currentExpression->unionPtrs.leaves.rightPtr);
      break;
    }

    free(currentExpression);
  }
}
#include "querycsv.h"

struct atomEntry *parse_atomCommaList(
    struct qryData *queryData,
    struct atomEntry *lastEntryPtr,
    char *newEntry
  ) {

  struct atomEntry *newEntryPtr;

  MAC_YIELD
  
  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg((void**)(&newEntryPtr), sizeof(struct atomEntry));

  if(lastEntryPtr == NULL) {
    newEntryPtr->index = 1;
    newEntryPtr->nextInList = newEntryPtr;
  }
  else {
    newEntryPtr->index = lastEntryPtr->index+1;
    newEntryPtr->nextInList = lastEntryPtr->nextInList;
    lastEntryPtr->nextInList = newEntryPtr;
  }

  newEntryPtr->content = newEntry;

  return newEntryPtr;
}


#include "querycsv.h"

void cleanup_query(struct qryData *query) {
  MAC_YIELD
  
  cleanup_columnReferences(query->columnReferenceHashTable);
  cleanup_resultColumns(query->firstResultColumn);
  cleanup_orderByClause(query->groupByClause);
  cleanup_orderByClause(query->orderByClause);
  cleanup_expression(query->joinsAndWhereClause);
  cleanup_inputTables(query->firstInputTable);
  free(query->intoFileName);
}
#include "querycsv.h"

/* decrement size of a string to represent right trimming whitespace  */
int strRTrim(char **value, size_t *strSize, char *minSize) {
  char *end;
  char *str;
  int size;

  MAC_YIELD

  if(value == NULL || strSize == NULL || *value == NULL) {
    return FALSE;
  }

  str = *value;
  size = *strSize;

  end = str + size - 1;
  while(end > str && end != minSize && (*end == ' ' || *end == '\t')) {
    end--;
    size--;
  }

  *strSize = size;

  return TRUE;
}
#include "querycsv.h"

/* TRUE means the record was rejected.  FALSE means the record should not yet be rejected */
int walkRejectRecord(
    int currentTable,
    struct expression *expressionPtr,
    struct resultColumnParam *match
  ) {

  int retval, i;
  struct atomEntry *currentAtom;

  MAC_YIELD

  if(expressionPtr == NULL || currentTable < expressionPtr->minTable) {
    return FALSE;
  }

  if(expressionPtr->type >= EXP_EQ && expressionPtr->type <= EXP_GTE){
    getValue(expressionPtr->unionPtrs.leaves.leftPtr, match);
    getValue(expressionPtr->unionPtrs.leaves.rightPtr, match);

    if(
        expressionPtr->unionPtrs.leaves.leftPtr->leftNull ||
        expressionPtr->unionPtrs.leaves.rightPtr->leftNull
      ) {

      freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
      freeAndZero(expressionPtr->unionPtrs.leaves.rightPtr->value);

      return FALSE;
    }

    retval = strCompare(
        (unsigned char **)(&(expressionPtr->unionPtrs.leaves.leftPtr->value)),
        (unsigned char **)(&(expressionPtr->unionPtrs.leaves.rightPtr->value)),
        expressionPtr->unionPtrs.leaves.leftPtr->caseSensitive,
        (void (*)())&getUnicodeChar,
        (void (*)())&getUnicodeChar
      );

    freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
    freeAndZero(expressionPtr->unionPtrs.leaves.rightPtr->value);

    switch(expressionPtr->type) {
      case EXP_EQ:
        return retval != 0;
      case EXP_NEQ:
        return retval == 0;
      case EXP_LT:
        return retval != -1;
      case EXP_LTE:
        return retval == 1;
      case EXP_GT:
        return retval != 1;
      case EXP_GTE:
        return retval == -1;
    }
  }
  else if(expressionPtr->type == EXP_AND) {
    return
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) ||
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.rightPtr, match);
  }
  else if(expressionPtr->type == EXP_OR) {
    return
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) &&
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.rightPtr, match);
  }
  else if(expressionPtr->type <= EXP_NOT) {
    return walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) == 0;
  }
  else if(expressionPtr->type == EXP_IN || expressionPtr->type == EXP_NOTIN) {
    getValue(expressionPtr->unionPtrs.leaves.leftPtr, match);

    if(expressionPtr->unionPtrs.leaves.leftPtr->leftNull) {
      freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
      return FALSE;
    }

    for(
        i = expressionPtr->unionPtrs.inLeaves.lastEntryPtr->index,
          currentAtom = expressionPtr->unionPtrs.inLeaves.lastEntryPtr->nextInList;
        i != currentAtom->index;
        currentAtom = currentAtom->nextInList
      ) {
      if(strCompare(
        (unsigned char **)(&(expressionPtr->unionPtrs.leaves.leftPtr->value)),
        (unsigned char **)(&(currentAtom->content)),
        expressionPtr->unionPtrs.leaves.leftPtr->caseSensitive,
        (void (*)())&getUnicodeChar,
        (void (*)())&getUnicodeChar
      ) == 0) {
        freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
        return expressionPtr->type == EXP_NOTIN;  /* FALSE */
      }
    }

    freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
    return expressionPtr->type != EXP_NOTIN;  /* TRUE */
  }

  return FALSE;
}
#include "querycsv.h"

int getColumnCount(char *inputFileName) {
  FILE *inputFile = NULL;
  int columnCount = 1;

  MAC_YIELD

  /* attempt to open the input file */
  inputFile = skipBom(inputFileName, NULL);
  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return -1;
  }

  /* read csv columns until end of line occurs */
  while(getCsvColumn(&inputFile, NULL, NULL, NULL, NULL, TRUE)) {
    columnCount++;
  }

  /* output the number of columns we counted */
  fprintf(stdout, "%d", columnCount);

  /* close the input file and return */
  fclose(inputFile);
  return 0;
}
#include "querycsv.h"

struct resultColumn *parse_expCommaList(
    struct qryData *queryData,
    struct expression *expressionPtr,
    char *resultColumnName,
    int aggregationType
  ) {

  struct columnReference *currentReference = NULL;
  struct columnReference *newReference = NULL;
  struct columnReference *newReference2 = NULL;
  struct resultColumn *newResultColumn = NULL;

  MAC_YIELD

  /* ensure we have finished opening all the files we need */
  if(queryData->parseMode != 1) {
    return NULL;
  }

  /* increment the column count for display purposes */
  if(aggregationType == GRP_NONE) {
    queryData->columnCount++;
  
    /* if the expression wasn't given a name then provide it with a default one */
    if(resultColumnName == NULL) {
      if(expressionPtr->type == EXP_COLUMN) {
        if((resultColumnName = strdup(((struct inputColumn*)(expressionPtr->unionPtrs.voidPtr))->fileColumnName)) == NULL) {
          fputs(TDB_MALLOC_FAILED, stderr);
          exit(EXIT_FAILURE);
        }
      }
      else if(d_sprintf(&resultColumnName, TDB_UNTITLED_COLUMN) == FALSE) {
        fputs(TDB_MALLOC_FAILED, stderr);
        exit(EXIT_FAILURE);
      }
    }
  }
  else {
    free(resultColumnName);

    queryData->hiddenColumnCount++;

    if(d_sprintf(&resultColumnName, "%d", queryData->hiddenColumnCount) == FALSE) {
      fputs(TDB_MALLOC_FAILED, stderr);
      exit(EXIT_FAILURE);
    }
  }

  /*  stick this new reference into the lookup table for identifiers */
  /* /////////////////////////////////////////////////////////////// */
  {
    /* test if the key already exists */
    currentReference = hash_lookupString(queryData->columnReferenceHashTable, resultColumnName);

    /* no column with this name has been defined at all. just create the new column reference and insert it into the hash table */
    if(currentReference == NULL) {
      reallocMsg((void**)(&newReference), sizeof(struct columnReference));

      newReference->referenceName = resultColumnName;
      newReference->nextReferenceWithName = NULL;

      hash_addString(queryData->columnReferenceHashTable, resultColumnName, newReference);
    }

    /* otherwise the reference has already been used */
    else {
      /* the text already exists, so we don't need the copy in resultColumnName any longer */
      free(resultColumnName);

      reallocMsg((void**)(&newReference), sizeof(struct columnReference));

      /* if the reference returned is of type REF_COLUMN (i.e. a column in a csv file), make a copy of it, overwrite the original with the new one then insert the copy afterwards */
      if(currentReference->referenceType == REF_COLUMN) {
        reallocMsg((void**)(&newReference2), sizeof(struct columnReference));

        /* switch the current and new references */
        memcpy(newReference, currentReference, sizeof(struct columnReference));
        memcpy(currentReference, newReference2, sizeof(struct columnReference));
        free(newReference2);
        currentReference->nextReferenceWithName = newReference;
        currentReference->referenceName = newReference->referenceName;
        newReference = currentReference;
      }

      /* otherwise insert before the first reference of type REF_COLUMN */
      else {
        while(
            currentReference->nextReferenceWithName != NULL &&
            currentReference->nextReferenceWithName->referenceType != REF_COLUMN
          ) {

          currentReference = currentReference->nextReferenceWithName;
        }

        newReference->nextReferenceWithName = currentReference->nextReferenceWithName;
        currentReference->nextReferenceWithName = newReference;
        newReference->referenceName = currentReference->referenceName;
      }
    }

    newReference->referenceType = REF_EXPRESSION;
    newReference->reference.calculatedPtr.expressionPtr = expressionPtr;
  }

  /* put the new reference into the output columns */
  /* ///////////////////////////////////////////// */
  {
    /* if the expression is just a singular reference of a column in one of the csv files, */
    /* then use it as a non calculated column */
    if(expressionPtr->type == EXP_COLUMN && aggregationType == GRP_NONE) {
      /* get the csv file column from the expression leaf */
      newResultColumn = ((struct inputColumn*)(expressionPtr->unionPtrs.voidPtr))->firstResultColumn;

      /* don't store the result column against the reference as it will be stored against the column instead */
      newReference->reference.calculatedPtr.firstResultColumn = NULL;

      /* loop over each copy of the csv column in the result set, */
      /* testing if it's the most recently defined result column */
      while (newResultColumn != NULL) {

        /* if the expression is just the most recently referenced column, */
        /* then associate the reference with it also  */
        if(queryData->firstResultColumn == newResultColumn) {
          queryData->firstResultColumn->isHidden = FALSE;
          queryData->firstResultColumn->resultColumnName = strdup(newReference->referenceName);

          return queryData->firstResultColumn;
        }

        /* if the next column instance is null then this input column is not yet in the result set */
        /* or we may need to output the same input column multiple times, */
        /* so create a new output column copy then break the loop */
        if(newResultColumn->nextColumnInstance == NULL) {
          newResultColumn->nextColumnInstance = parse_newOutputColumn(
              queryData,
              /*isHidden = */FALSE,
              /*isCalculated = */FALSE,
              /*resultColumnName = */strdup(newReference->referenceName),
              /*aggregationType = */GRP_NONE
            );

          return newResultColumn->nextColumnInstance;
        }
        
        /* otherwise get the next instance then continue looping */
        else {
          newResultColumn = newResultColumn->nextColumnInstance;
        }
      }

      return NULL;
    }

    /* the most recently defined expression is not a direct column reference or the most recent reference is not this one. */
    /* add another column to the result set, marking it as being calculated if it refers to an expression */
    /* make it hidden if makeHidden is true (e.g. when the expression will be used in an aggregation) */
    else {
      newReference->reference.calculatedPtr.firstResultColumn = parse_newOutputColumn(
          queryData,
          /*isHidden = */aggregationType != GRP_NONE,
          /*isCalculated = */TRUE,
          /*resultColumnName = */strdup(newReference->referenceName),
          /*aggregationType = */aggregationType
        );

      return newReference->reference.calculatedPtr.firstResultColumn;
    }
  }
}

#include "querycsv.h"

struct expression *parse_functionRefStar(
    struct qryData *queryData,
    long aggregationType
  ) {

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return NULL;
  }

  if(aggregationType != GRP_COUNT) {
    /* I don't think in sql you can aggregate an aggregate. */
    /* therefore we should error out if we get to this point */
    fputs("only count(*) is valid",stderr);
    exit(EXIT_FAILURE);
  }
  
  return parse_functionRef(
    queryData,
    GRP_STAR,
    parse_scalarExpLiteral(queryData, ""),
    FALSE
  );
}
#include "querycsv.h"

/* fake only the skipping of bytes and presume the file is already rewound */
int myfseek(FILE *stream, long offset, int origin) {
  long current;
  int c;  /* discarded */

  for(current = 0; current<offset; current++) {
    c = fgetc(stream);
  }

  return 0;
}
#include "querycsv.h"

/* Calls a callback function once for every item in the tree, and de-allocates memory used by the tree in the process */

/* This function only de-allocates/frees memory for the tree, ***but not the data items the tree contains*** */
void tree_walkAndCleanup(
    struct qryData *query,
    struct resultTree **root,
    void (*callback)(struct qryData *, struct resultColumnValue *, int)
  ) {
  struct resultTree *currentResult;
  struct resultTree *parentResult;
  int i = 0;

  MAC_YIELD
  
  if(root == NULL || *root == NULL) {
    return;
  }

  currentResult = *root;

  for( ; ; ) {
    if(currentResult->left) {
      currentResult = currentResult->left;
      continue;
    }

    if((currentResult->type) != TRE_FREED) {
      callback(
        query,
        currentResult->columns,
        i
      );

      currentResult->type = TRE_FREED;
      i+=1;
    }

    if(currentResult->right) {
      currentResult = currentResult->right;
      continue;
    }

    parentResult = currentResult->parent;
    
    if(parentResult != NULL) {
      if(parentResult->left == currentResult) {
        currentResult = parentResult;
        free(currentResult->left);
        currentResult->left = NULL;
      }
      else {
        currentResult = parentResult;
        free(currentResult->right);
        currentResult->right = NULL;
      }
    }
    else {
      free(currentResult);
      *root = NULL;
      return;
    }
  }
}
#include "querycsv.h"

int runQuery(char *queryFileName) {
  struct qryData query;
  struct resultColumnValue* match = NULL;

  MAC_YIELD

  readQuery(queryFileName, &query);

  /* allocates space for the next record in the record set */
  reallocMsg(
    (void**)&match,
    (query.columnCount)*sizeof(struct resultColumnValue)
  );

  /* if there is no sorting of results required and the user didn't */
  /* specify an output file then output the results to the screen as soon as they become available */
  if(
      query.orderByClause == NULL &&
      query.intoFileName == NULL &&
      query.groupByClause == NULL
    ) {

    /* output the header */
    outputHeader(&query); 

    while(getMatchingRecord(&query, match)) {
      /* print record to stdout */
      outputResult(&query, match, 0);
      match = NULL;

      reallocMsg(
        (void**)&match,
        (query.columnCount)*sizeof(struct resultColumnValue)
      );
    }

    /* the last record wasn't used */
    free(match);
    match = NULL;
  }
  else {
    if(query.groupByClause != NULL) {
      query.useGroupBy = TRUE;
    }

    while(getMatchingRecord(&query, match)) {
      /* add another record to the result set. */
      /* The match variable's allocated memory is the responsibility of the tree now */
      tree_insert(&query, match, &(query.resultSet));
      match = NULL;

      reallocMsg(
        (void**)&match,
        (query.columnCount)*sizeof(struct resultColumnValue)
      );
    }

    /* the last record wasn't used */
    free(match);
    match = NULL;

    /* perform group by operations if it was specified in the query */
    if(query.groupByClause != NULL) {
      groupResults(&query);
      query.useGroupBy = FALSE;
    }

    /* output the results to the specified file */
    outputHeader(&query);

    /* output each record */
    tree_walkAndCleanup(
      &query,
      &(query.resultSet),
      &outputResult
    );

    /* close the output file */
    if(query.intoFileName) {
      fclose(query.outputFile);

      /* output the number of records returned iff there was an into clause specified */
      fprintf(stdout, "%d", query.recordCount);
      fflush(stdout);
    }
  }

  /* free the query data structures */
  cleanup_query(&query);

  return 0;
}
#include "querycsv.h"

unsigned int hash_compare(
    struct columnReferenceHash *hashtable,
    char *str
) {
  unsigned int hashval;

  MAC_YIELD
  
  /* we start our hash out at 0 */
  hashval = 0;

  /* for each character, we multiply the old hash by 31 and add the current
   * character.  Remember that shifting a number left is equivalent to 
   * multiplying it by 2 raised to the number of places shifted.  So we 
   * are in effect multiplying hashval by 32 and then subtracting hashval.  
   * Why do we do this?  Because shifting and subtraction are much more 
   * efficient operations than multiplication.
   */

  for(; *str != '\0'; str++) {
    hashval = *str + (hashval << 5) - hashval;
  }

  /* we then return the hash value mod the hashtable size so that it will
   * fit into the necessary range
   */

  return hashval % hashtable->size;
}
#include "querycsv.h"

struct columnReference *hash_lookupString(
    struct columnReferenceHash *hashtable,
    char *str
) {
  struct columnRefHashEntry *list;
  unsigned int hashval = hash_compare(hashtable, str);

  MAC_YIELD

  /* Go to the correct list based on the hash value and see if str is
   * in the list.  If it is, return return a pointer to the list element.
   * If it isn't, the item isn't in the table, so return NULL.
   */
  for(list = hashtable->table[hashval]; list != NULL; list = list->nextReferenceInHash) {
    if (strcmp(str, list->referenceName) == 0) {
      return list->content;
    }
  }

  return NULL;
}
#include "querycsv.h"

void reallocMsg(void **mem, size_t size) {
  void *temp = NULL;

  MAC_YIELD
  
  if((temp = realloc(*mem, size)) == NULL) {
    fputs(TDB_MALLOC_FAILED, stderr);
    exit(EXIT_FAILURE);
  }

  *mem = temp;
}

/* static void* binaryTree;  // if malloc, calloc or realloc fail then try cleaning up some memory by flushing our binary tree to disk */
#include "querycsv.h"

int consumeCombiningChars(
    unsigned char **str1,
    unsigned char **str2,
    unsigned char **offset1,
    unsigned char **offset2,
    void (*get1)(),
    void (*get2)(),
    int *bytesMatched1,
    int *bytesMatched2,
    int *accentcheck
) {
  int combiner1, combiner2, skip1 = FALSE, skip2 = FALSE;

  MAC_YIELD

  (*offset1)+=(*bytesMatched1);
  (*offset2)+=(*bytesMatched2);

  do {
    if(skip1 == FALSE) {
     combiner1 = isCombiningChar(
          (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get1))
          (offset1, str1, 0, bytesMatched1, get1)
      );
    }
    
    if(skip2 == FALSE) {
      combiner2 = isCombiningChar(
          (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))
          (offset2, str2, 0, bytesMatched2, get2)
      );
    }

    if(skip1 == FALSE && skip2 == FALSE) {
      if(combiner1 != combiner2) {
        /* if either of them is not a combining character or not equal then accentcheck is set to 1 */
        /* if accentcheck is already 1 then return the one thats greater */
        if(*accentcheck == 2) {
          return (combiner1 > combiner2) ? 1 : -1;
        }
        else {
          *accentcheck = 1;
        }
      }
    }

    if(combiner1 == 0) {
      skip1 = TRUE; 
    }
    else {
      (*offset1)+=(*bytesMatched1);
    }

    if(combiner2 == 0) {
      skip2 = TRUE; 
    }
    else {
      (*offset2)+=(*bytesMatched2);
    }
  } while (skip1 == FALSE || skip2 == FALSE);

  return 0;
}

#include "querycsv.h"

int getNextRecordOffset(char *inputFileName, long offset) {
  FILE *inputFile = NULL;

  MAC_YIELD

  /* attempt to open the input file */
  inputFile = skipBom(inputFileName, NULL);
  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return -1;
  }

  /* seek to offset */
  /* TODO: replace the fseek reference with our own implementation as fseek doesn't work in cc65 */
  if(myfseek(inputFile, offset, SEEK_SET) != 0) {
    fputs(TDB_COULDNT_SEEK, stderr);
    return -1;
  }

  /* read csv columns until end of line occurs */
  while(getCsvColumn(&inputFile, NULL, NULL, NULL, &offset, TRUE)) {
    /* do nothing */
  }

  /* get current file position */
  fprintf(stdout, "%ld", offset);

  /* close the input file and return */
  fclose(inputFile);
  return 0;
}
#include "querycsv.h"

int getMatchingRecord(struct qryData *query, struct resultColumnValue *match)
{
  struct inputTable *currentInputTable;
  struct inputColumn *currentInputColumn;
  struct resultColumn *currentResultColumn;
  struct resultColumnValue columnOffsetData;
  struct resultColumnParam matchParams;
  int recordHasColumn;
  int doLeftRecord = FALSE;
  long templong = 0;

  MAC_YIELD

  matchParams.ptr = match;
  matchParams.params = query->params;

  /* if secondaryInputTable is NULL then */
  /* the query hasn't returned any results yet. */
  /* needed as this function should continue where it left off next time */
  if(query->secondaryInputTable == NULL) {
    query->secondaryInputTable = query->firstInputTable;
  }

  currentInputTable = query->secondaryInputTable;

  /* get the offsets of each column in turn and confirm whether the criteria for a match is met */
  /* ****************************************************************************************** */

  do {  /* tables */
    while(
        endOfFile(currentInputTable->fileStream) ||
        (
          currentInputTable->isLeftJoined &&
          currentInputTable->noLeftRecord &&
          (doLeftRecord = TRUE)
        )
      ) {   /* records */

      /* reset the flag that says the column values ran out */
      recordHasColumn = TRUE;

      for (
          currentInputColumn = currentInputTable->firstInputColumn;
          currentInputColumn != NULL;
          currentInputColumn = currentInputColumn->nextColumnInTable
        ) {  /* columns */

        /* if we haven't yet reached the end of a record, get the next column value. */
        /* if it returns false we use an empty string for the value instead */

        if(recordHasColumn == TRUE && !doLeftRecord) {
          columnOffsetData.value = NULL;
          templong = columnOffsetData.startOffset;
          recordHasColumn = getCsvColumn(
              &(currentInputTable->fileStream),
              &(columnOffsetData.value),
              &(columnOffsetData.length),
              &(columnOffsetData.isQuoted),
              &(templong),
              (query->params & PRM_TRIM) == 0
            );

          /* these values should actually be set depending on whether the value was quoted or not */
          /* if the value is quoted we should probably also NFD normalise it before writing to the scratchpad */
          columnOffsetData.isNormalized = FALSE;
          columnOffsetData.leftNull = FALSE;
        }

        /* construct an empty column reference. */
        else {
          columnOffsetData.leftNull = doLeftRecord;
          columnOffsetData.startOffset = 0;
          columnOffsetData.length = 0;
          columnOffsetData.isQuoted = FALSE;
          columnOffsetData.isNormalized = TRUE; /* an empty string needs no unicode normalization */
          columnOffsetData.value = strdup("");
        }

        /* put the values retrieved into each of the columns in the output match */
        for(
            currentResultColumn = currentInputColumn->firstResultColumn;
            currentResultColumn != NULL;
            currentResultColumn = currentResultColumn->nextColumnInstance
          ) {

          memcpy(
              &(match[currentResultColumn->resultColumnIndex]),
              &columnOffsetData,
              sizeof(struct resultColumnValue)
            );
        }
      }
      /* end of columns */

      /* consume any remaining column data that may exist in this record */
      if(recordHasColumn == TRUE && !doLeftRecord) {
        while(getCsvColumn(
            &(currentInputTable->fileStream),
            NULL,
            NULL,
            NULL,
            NULL,
            (query->params & PRM_TRIM) == 0
          )) {
          /* do nothing */
        }
      }

      /* TRUE means the record was rejected. FALSE means the record hasn't yet been rejected */
      if(walkRejectRecord(
          currentInputTable->fileIndex,
          query->joinsAndWhereClause,
          &matchParams
        )) {
        if(doLeftRecord) {
          doLeftRecord = FALSE;
          break;
        }
        else {
          /* go to next record */
          continue;
        }
      }
      else if(currentInputTable->nextInputTable == NULL) {
        /* there was a record match for this table */
        currentInputTable->noLeftRecord = FALSE;

        /* mark every record in every table as having a match, */
        /* even if using a special left join one */
        currentInputTable = query->firstInputTable;

        while((currentInputTable) != (query->secondaryInputTable)) {
          currentInputTable->noLeftRecord = FALSE;
          currentInputTable = currentInputTable->nextInputTable;
        }

        currentInputTable = query->secondaryInputTable;

        /* do calculated columns */
        getCalculatedColumns(query, match, FALSE);

        if(!walkRejectRecord(
          currentInputTable->fileIndex+1, /* +1 means all tables and *CALCULATED* columns */
          query->joinsAndWhereClause,
          &matchParams
        )) {
          return TRUE;
        }
      }
      else {
        /* there are more columns still to check in the next file */
        currentInputTable = query->secondaryInputTable = currentInputTable->nextInputTable;
      }

      doLeftRecord = FALSE;
    }
    /* end of records */

    /* rewind the file, but skip the column headers line */
    /* TODO: replace the fseek reference with our own code as fseek doesn't work in cc65*/
    myfseek(currentInputTable->fileStream, currentInputTable->firstRecordOffset, SEEK_SET);
    currentInputTable->noLeftRecord = TRUE;

    /* go back up the list of tables. */
    if(currentInputTable->fileIndex == 1) {
      currentInputTable = NULL;
    }
    else {
      currentInputTable = query->firstInputTable;

      while(currentInputTable->nextInputTable != query->secondaryInputTable) {
        currentInputTable = currentInputTable->nextInputTable;
      }

      query->secondaryInputTable = currentInputTable;
    }

  } while (currentInputTable != NULL);
  /* end of tables */

  /* all data scanned. no more matches */
  return FALSE;
}
#include "querycsv.h"

void stringGet(
    unsigned char **str,
    struct resultColumnValue *field,
    int params
  ) {

  MAC_YIELD

  if(str != NULL) {
    *str = (unsigned char *)(strdup((char const *)(field->value)));
  }

  /*
  long offset = ftell(*(field->source));

  fseek(*(field->source), field->startOffset, SEEK_SET);
  fflush(*(field->source));

  if (field->isQuoted) {
    //can't use a shortcut to get the string value, so get it the same way we did the last time
    getCsvColumn(field->source,str,&(field->length),NULL,NULL, (params & PRM_TRIM) == 0);
  }
  else {
    //can use a shortcut to get the string value
    reallocMsg((void**)str, field->length+1);

    if(fread(*str, 1, field->length, *(field->source)) != field->length) {
      fputs("didn't read string properly\n", stderr);
      exit(EXIT_FAILURE);
    }

    (*str)[field->length] = '\0';
  }

  //reset the file offset as we don't know what else the file is being used for
  fseek(*(field->source), offset, SEEK_SET);
  fflush(*(field->source));
  */
}
#include "querycsv.h"

void cleanup_inputTables(struct inputTable *currentInputTable) {
  struct inputTable *next;

  MAC_YIELD

  while(currentInputTable != NULL) {
    next = currentInputTable->nextInputTable;

    free(currentInputTable->queryTableName);
    fclose(currentInputTable->fileStream);
    free(currentInputTable);

    currentInputTable = next;
  }
}
#include "querycsv.h"

/* output a record to stdio */
void outputResult(
    struct qryData *query,
    struct resultColumnValue *columns,
    int currentIndex
  ) {
  struct resultColumn *currentResultColumn;
  struct resultColumnValue *field;
  int firstColumn = TRUE, j;
  FILE *outputFile = query->outputFile;
  char *separator = (((query->params) & PRM_SPACE) != 0)?",":", ";

  char *string = NULL;
  char *string2 = NULL;

  MAC_YIELD

  /* for output columns */
  j=0;
  for(
      currentResultColumn = query->firstResultColumn;
      currentResultColumn != NULL;
      currentResultColumn = currentResultColumn->nextColumnInResults
    ) {

    if(currentResultColumn->isHidden == FALSE) {
      if (!firstColumn) {
        fputs(separator, outputFile);
      }
      else {
        firstColumn = FALSE;
      }

      field = &(columns[j]);

      switch(field->leftNull) {
        case TRUE: {
          if(((query->params) & PRM_EXPORT) != 0) {
            fputs("\\N", outputFile);
          }
        } break;

        default: {
          stringGet((unsigned char **)(&string), field, query->params);

          /* need to properly re-escape fields that need it */
          if(*string == '\0') {
            fputs("\"\"", outputFile);  /* empty string always needs escaping */
          }
          else if(needsEscaping(string, query->params)) {
            string2 = strReplace("\"","\"\"", string);
            fputs("\"", outputFile);
            fputs(string2, outputFile);
            fputs("\"", outputFile);
            freeAndZero(string2);
          }
          else {
            fputs(string, outputFile);
          }
        } break;
      }
    }

    j++;
  }

  freeAndZero(string);
  cleanup_matchValues(query, &columns);

  fputs(query->newLine, outputFile);
}
#include "querycsv.h"

/* compares two whole records to one another. multiple columns can be involved in this comparison. */
int recordCompare(
    const void *a,
    const void *b,
    void *c
  ) {

  struct sortingList *orderByClause;
  char *string1, *string2, *output1, *output2;
  int compare;

  struct resultColumnParam matchParams;

  MAC_YIELD

  matchParams.params = ((struct qryData*)c)->params;

  for(
      orderByClause = ((struct qryData*)c)->useGroupBy?
          ((struct qryData*)c)->groupByClause:
          ((struct qryData*)c)->orderByClause;
      orderByClause != NULL;
      orderByClause = orderByClause->nextInList
    ) {

    /* get the value of the expression using the values in record a */
    matchParams.ptr = (struct resultColumnValue*)a;
    getValue(
        orderByClause->expressionPtr,
        &matchParams
      );
    string1 = output1 = orderByClause->expressionPtr->value;
    orderByClause->expressionPtr->value = NULL;

    /* get the value of the expression using the values in record b */
    matchParams.ptr = (struct resultColumnValue*)b;
    getValue(
        orderByClause->expressionPtr,
        &matchParams
      );
    string2 = output2 = orderByClause->expressionPtr->value;
    orderByClause->expressionPtr->value = NULL;

    /* do the comparison of the two current expression values */
    compare = strCompare(
        (unsigned char **)(&output1),
        (unsigned char **)(&output2),
        2,    /* orderByClause->expressionPtr->caseSensitive, */
        (void (*)())getUnicodeChar,
        (void (*)())getUnicodeChar
      );

    /* clean up used memory. The string1 & string2 pointers might be made */
    /* stale (and freed automatically) by unicode NFD normalisation in */
    /* strCompare function */
    freeAndZero(output1);
    freeAndZero(output2);

    /*  if the fields didn't compare as being the same, then return which was greater */
    if(compare != 0) {
      return orderByClause->isDescending?compare:-compare;
    }
  }

  /* all fields to compare compared equally */
  return 0;
}
#include "querycsv.h"

struct expression *parse_scalarExpLiteral(
    struct qryData *queryData,
    char *literal
  ) {

  struct expression *expressionPtr = NULL;

  MAC_YIELD
  
  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg((void**)(&expressionPtr), sizeof(struct expression));

  expressionPtr->type = EXP_LITERAL;
  expressionPtr->value = NULL;
  expressionPtr->unionPtrs.voidPtr = strdup(literal);
  expressionPtr->minColumn = 0;
  expressionPtr->minTable = 0;

  expressionPtr->containsAggregates = FALSE;

  return expressionPtr;
}
#include "querycsv.h"

int strAppendUTF8(long codepoint, unsigned char **nfdString, int nfdLength) {

  MAC_YIELD

  if (codepoint < 0x80) {
    reallocMsg((void**)nfdString, nfdLength+1);

    (*nfdString)[nfdLength++] = codepoint;
  }
  else if (codepoint < 0x800) {
    reallocMsg((void**)nfdString, nfdLength+2);

    (*nfdString)[nfdLength++] = (codepoint >> 6) + 0xC0;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }
  else if (codepoint < 0x10000) {
    reallocMsg((void**)nfdString, nfdLength+3);

    (*nfdString)[nfdLength++] = (codepoint >> 12) + 0xE0;
    (*nfdString)[nfdLength++] = ((codepoint >> 6) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }
  else {
    reallocMsg((void**)nfdString, nfdLength+4);

    (*nfdString)[nfdLength++] = (codepoint >> 18) + 0xF0;
    (*nfdString)[nfdLength++] = ((codepoint >> 12) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = ((codepoint >> 6) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }

  return nfdLength;
}
#include "querycsv.h"

int getCurrentDate(void) {
  time_t now;
  struct tm local;

  char *output = NULL;

  MAC_YIELD

  /* get unix epoch seconds */
  time(&now);

  /* get localtime tm object and utc offset string */
  /* (we don't want to keep gmtime though as the users clock */
  /*  will probably be set relative to localtime) */
  if(d_tztime(&now, &local, NULL, &output) == FALSE) {
    fputs(TDB_TZTIMED_FAILED, stderr);

    freeAndZero(output);

    return -1;
  };

  /* place the utc offset in the output string. */
  /* %z unfortunately can't be used as it doesn't work properly */
  /* in some c library implementations (Watcom and MSVC) */
  if(d_sprintf(&output, "%%Y-%%m-%%dT%%H:%%M:%%S%s", output) == FALSE) {
    fputs(TDB_SPRINTFD_FAILED, stderr);

    freeAndZero(output);

    return -1;
  };

  /* place the rest of the time data in the output string */
  if(d_strftime(&output, output, &local) == FALSE) {
    fputs(TDB_STRFTIMED_FAILED, stderr);

    freeAndZero(output);

    return -1;
  }

  /* print the timestamp */
  fputs(output, stdout);

  /* free the string data */
  freeAndZero(output);

  /* quit */
  return 0;
}
#include "querycsv.h"

int needsEscaping(char *str, int params) {
  MAC_YIELD

  if(
    str == NULL ||
    strcmp(str, (((params & PRM_EXPORT) == 0)?"\\N":"\\N")) == 0 ||
    *str == ' ' ||
    *str == '\t') {
    return TRUE;
  }

  while(*str) {
    if(*str == '"' || *str == '\n' || *str == ',') {
    return TRUE;
    }
    str++;
  }

  str--;

  if(*str == ' ' || *str == '\t') {
    return TRUE;
  }

  return FALSE;
}
#include "querycsv.h"

void updateRunningCounts(struct qryData *query, struct resultColumnValue *match) {
  struct columnRefHashEntry *currentHashEntry;
  struct columnReference *currentReference;
  struct resultColumn *currentResultColumn;
  struct resultColumnValue *field;
  char *tempString = NULL;
  char *tempString2 = NULL;
  double tempFloat;

  int i, j;

  MAC_YIELD

  query->groupCount++;

  /* for each column in the output result set ... */
  for(i = 0; i < query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {

      currentReference = currentHashEntry->content;

      while(currentReference != NULL) {
        /* ... check if the current column in the result set is a grouped one, and increment/set the group variables in the appropriate way */
        if(
            currentReference->referenceType == REF_EXPRESSION &&
            (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL &&
            currentResultColumn->groupType != GRP_NONE
          ) {
          field = &(match[currentResultColumn->resultColumnIndex]);

          if(field->leftNull == FALSE) {
            stringGet((unsigned char **)(&tempString), field, query->params);

            if(currentResultColumn->groupType > GRP_STAR) {
              if(query->groupCount > 1) {
                for(j = 1; j < query->groupCount; j++) {
                  stringGet((unsigned char **)(&tempString2), &(match[(currentResultColumn->resultColumnIndex) - (query->columnCount)]), query->params);

                  if(strCompare(
                    (unsigned char **)(&tempString),
                    (unsigned char **)(&tempString2),
                    TRUE,
                    (void (*)())getUnicodeChar,
                    (void (*)())getUnicodeChar
                  ) == 0) {
                    freeAndZero(tempString2);
                    break;
                  }

                  freeAndZero(tempString2);
                }
              }
              else {
                j = query->groupCount;
              }
            }

            switch(currentResultColumn->groupType) {
              case GRP_DIS_COUNT:
                if(j == query->groupCount) {
                  currentResultColumn->groupCount++;
                }
              break;

              case GRP_COUNT:
                currentResultColumn->groupCount++;
              break;

              case GRP_DIS_AVG:
              case GRP_DIS_SUM:
                if(j == query->groupCount) {
                  currentResultColumn->groupCount++;
                  tempFloat = strtod(tempString, NULL);
                  currentResultColumn->groupNum = fadd(currentResultColumn->groupNum, tempFloat);
                }
              break;

              case GRP_AVG:
              case GRP_SUM:
                currentResultColumn->groupCount++;
                tempFloat = strtod(tempString, NULL);
                currentResultColumn->groupNum = fadd(currentResultColumn->groupNum, tempFloat);
              break;

              case GRP_DIS_CONCAT:
                if(j == query->groupCount) {
                  d_sprintf(
                      &(currentResultColumn->groupText),
                      "%s%s",
                      currentResultColumn->groupText,
                      tempString
                    );
                }
              break;

              case GRP_CONCAT:
                d_sprintf(
                    &(currentResultColumn->groupText),
                    "%s%s",
                    currentResultColumn->groupText,
                    tempString
                  );
              break;

              case GRP_MIN:
              case GRP_DIS_MIN:
                if(currentResultColumn->groupText == NULL || strCompare(
                    (unsigned char **)(&tempString),
                    (unsigned char **)(&(currentResultColumn->groupText)),
                    2,    /* TRUE, */
                    (void (*)())getUnicodeChar,
                    (void (*)())getUnicodeChar
                  ) == -1) {

                  free(currentResultColumn->groupText);
                  currentResultColumn->groupText = tempString;
                  tempString = NULL;
                  currentReference = currentReference->nextReferenceWithName;
                  continue;
                }
              break;

              case GRP_MAX:
              case GRP_DIS_MAX:
                if(currentResultColumn->groupText == NULL || strCompare(
                    (unsigned char **)(&tempString),
                    (unsigned char **)(&(currentResultColumn->groupText)),
                    2,    /* TRUE, */
                    (void (*)())getUnicodeChar,
                    (void (*)())getUnicodeChar
                  ) == 1) {

                  free(currentResultColumn->groupText);
                  currentResultColumn->groupText = tempString;
                  tempString = NULL;
                  currentReference = currentReference->nextReferenceWithName;
                  continue;
                }
              break;
            }

            freeAndZero(tempString);
          }
        }

        currentReference = currentReference->nextReferenceWithName;
      }

      /* go to the next reference in the hash table */
      currentHashEntry = currentHashEntry->nextReferenceInHash;
    }
  }
}
#include "querycsv.h"

int yyerror(struct qryData *queryData, void *scanner, const char *msg) {
  MAC_YIELD

  fputs(msg, stderr);

  return 1;
}
#include "querycsv.h"
#include "sql.h"
#include "lexer.h"

void readQuery(
    char *queryFileName,
    struct qryData *query
  ) {
  FILE *queryFile = NULL;
  void *scanner;
  struct inputTable *currentInputTable;
  struct columnReference *currentReferenceWithName;
  struct resultColumn *currentResultColumn;
  struct sortingList *currentSortingList;
  struct columnRefHashEntry *currentHashEntry;

  int inputTableIndex = 2, i;

  MAC_YIELD

  /* read the query file and create the data structures we'll need */
  /* ///////////////////////////////////////////////////////////// */

  /* attempt to open the input file */
  queryFile = fopen(queryFileName, "r");
  if(queryFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    exit(EXIT_FAILURE);
  }

  /* setup the initial values in the queryData structure */

  query->parseMode = 0;   /* specify we want to just read the file data for now */
  query->hasGrouping = FALSE;
  query->useGroupBy = FALSE;
  query->columnCount = 0;
  query->hiddenColumnCount = 0;
  query->recordCount = 0;
  query->groupCount = 0;
  query->params = 0;
  query->intoFileName = NULL;
  query->columnReferenceHashTable = hash_createTable(32);
  query->firstInputTable = NULL;
  query->secondaryInputTable = NULL;
  query->firstResultColumn = NULL;
  query->joinsAndWhereClause = NULL;
  query->orderByClause = NULL;
  query->groupByClause = NULL;
  query->resultSet = NULL;

  /* setup reentrant flex scanner data structure */
  yylex_init(&scanner);

  /* feed our script file into the scanner structure */
  yyset_in(queryFile, scanner);

  /* parse the script file into the query data structure. */
  /* the parser will set up the contents of qryData as necessary */
  /* check that the parsing completed sucessfully, otherwise show a message and quit */
  switch(yyparse(query, scanner)) {
    case 0:
      /* parsing finished sucessfully. continue processing */
    break;

    case 1:
      /* the input script contained a syntax error. show message and exit */
      fputs(TDB_PARSER_SYNTAX, stderr);
      exit(EXIT_FAILURE);
    break;

    case 2:
      /* the input script parsing exhausted memory storage space. show message and exit */
      fputs(TDB_PARSER_USED_ALL_RAM, stderr);
      exit(EXIT_FAILURE);
    break;

    default:
      /* an unknown error occured when parsing the input script. show message and exit */
      /* (this shouldn't ever happen but you never know) */
      fputs(TDB_PARSER_UNKNOWN, stderr);
      exit(EXIT_FAILURE);
    break;
  }

  query->newLine = query->intoFileName?"\r\n":"\n";

  /* set query->firstInputTable to actually be the first input table. */
  query->firstInputTable = currentInputTable =
  (query->secondaryInputTable != NULL ?
  query->secondaryInputTable :
  query->firstInputTable)->nextInputTable;

  /* set the index columns for every table other than the first */
  while (currentInputTable->nextInputTable != query->firstInputTable) {
    currentInputTable = currentInputTable->nextInputTable;
    currentInputTable->fileIndex = inputTableIndex++;
  }

  /* cut the circularly linked list */
  currentInputTable->nextInputTable = NULL;

  /* set the secondary input table to NULL as we'll be re-using it */
  /* to store the current input table when getting matching records */
  query->secondaryInputTable = NULL;

  /* for every column reference in the hash table, fix up the nextReferenceWithName fields */
  for(i=0; i<query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {
      if(currentHashEntry->content->nextReferenceWithName) {
        currentReferenceWithName = currentHashEntry->content->nextReferenceWithName->nextReferenceWithName;
        currentHashEntry->content->nextReferenceWithName->nextReferenceWithName = NULL;
        currentHashEntry->content->nextReferenceWithName = currentReferenceWithName;
      }

      currentHashEntry = currentHashEntry->nextReferenceInHash;
    }
  }

  /* clean up the re-entrant flex scanner */
  yylex_destroy(scanner);

  /* TODO: use a hash table of the tables names to ensure they are all unique. */
  /* cause an error and exit the program if any table names are non-unique. */

  /* get set up for the second stage (populating the rest of the qryData structure using the cached data from stage 1) */
  query->parseMode = 1;

  /* rewind the file. Can't use fseek though as it doesn't work on CC65 */
  fclose(queryFile);
  queryFile = fopen(queryFileName, "r");
  if(queryFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    exit(EXIT_FAILURE);
  }
  
  /* setup reentrant flex scanner data structure */
  yylex_init(&scanner);

  /* feed our script file into the scanner structure */
  yyset_in(queryFile, scanner);

  /* parse the script file into the query data structure. */
  /* the parser will set up the contents of qryData as necessary */
  /* check that the parsing completed sucessfully, otherwise show a message and quit */
  switch(yyparse(query, scanner)) {
    case 0:
      /* parsing finished sucessfully. continue processing */
    break;

    case 1:
      /* the input script contained a syntax error. show message and exit */
      fputs(TDB_PARSER_SYNTAX, stderr);
      exit(EXIT_FAILURE);
    break;

    case 2:
      /* the input script parsing exhausted memory storage space. show message and exit */
      fputs(TDB_PARSER_USED_ALL_RAM, stderr);
      exit(EXIT_FAILURE);
    break;

    default:
      /* an unknown error occured when parsing the input script. show message and exit */
      /* (this shouldn't ever happen but you never know) */
      fputs(TDB_PARSER_UNKNOWN, stderr);
      exit(EXIT_FAILURE);
    break;
  }

  currentResultColumn = query->firstResultColumn;

  /* fix the column count value just in case it's wrong (e.g. if there were hidden columns) */
  query->columnCount = currentResultColumn->resultColumnIndex+1;

  /*  cut the circularly linked list of result columns */
  query->firstResultColumn = query->firstResultColumn->nextColumnInResults;
  currentResultColumn->nextColumnInResults = NULL;

  /* cut the circularly linked list of sort by expressions */
  if(query->orderByClause != NULL) {
    currentSortingList = query->orderByClause;
    query->orderByClause = query->orderByClause->nextInList;
    currentSortingList->nextInList = NULL;
  }

  /* cut the circularly linked list of group by expressions */
  if(query->groupByClause != NULL) {
    currentSortingList = query->groupByClause;
    query->groupByClause = query->groupByClause->nextInList;
    currentSortingList->nextInList = NULL;
  }

  /* clean up the re-entrant flex scanner */
  yylex_destroy(scanner);

  /* close the query file */
  fclose(queryFile);
}
#include "querycsv.h"

int tree_insert(
    struct qryData *query,
    struct resultColumnValue *columns,
    struct resultTree **root
  ) {

  struct resultTree *currentResult;
  struct resultTree *newResult;
  int comparison;

  MAC_YIELD

  if(root == NULL) {
    /* Huh? no memory location was specified to hold the tree? */
    /* Just exit and let the author of the calling function figure out their mistake */
    return FALSE;
  }

  if((newResult = malloc(sizeof(struct resultTree))) == NULL) {
    return FALSE;
  }

  newResult->parent = NULL;
  newResult->left = NULL;
  newResult->right = NULL;
  newResult->columns = columns;
  newResult->type = TRE_BLACK;

  if(*root == NULL) {
    /* No entries have been inserted at all. */
    /* Just insert the data into a new node */
    *root = newResult;
    return TRUE;
  }
  else {
    /* navigate down the tree, and insert the new data into the correct place within it */

    /* start at the top */
    currentResult = *root;

    for( ; ; ) {
      comparison = recordCompare(currentResult->columns, columns, query);

      if(comparison < 1) {
        if(currentResult->left == NULL) {
          newResult->parent = currentResult;
          currentResult->left = newResult;
          return TRUE;
        }
        else {
          currentResult = currentResult->left;
        }
      }
      else {
        if(currentResult->right == NULL) {
          newResult->parent = currentResult;
          currentResult->right = newResult;
          return TRUE;
        }
        else {
          currentResult = currentResult->right;
        }
      }
    }
  }
}
#include "querycsv.h"

/* get localtime, gmtime and utc offset string from a time_t. allocate/free memory as needed */
/* any of the last three parameters can be skipped by passing null */
int d_tztime(
    time_t *now,
    struct tm *local,
    struct tm *utc,
    char **output
) {
  struct tm *lcl;
  struct tm *gm;

  int hourlcl;
  int hourutc;
  int difference;
  int hour_difference;
  int minute_difference;

  char *output2;
  char *format = "+%02i%02i";

  MAC_YIELD

  if(now == NULL) {
    return FALSE;
  }

  if(local != NULL) {
    lcl = local;

    memcpy((void*)lcl, (void*)localtime(now), sizeof(struct tm));
  }
  else if (output != NULL) {
    lcl = (struct tm *)malloc(sizeof(struct tm));

    if(lcl == NULL) {
      return FALSE;
    }

    memcpy((void*)lcl, (void*)localtime(now), sizeof(struct tm));
  }

  if(utc != NULL) {
    gm = utc;

    memcpy((void*)gm, (void*)gmtime(now), sizeof(struct tm));
  }
  else if(output != NULL) {
    gm = (struct tm *)malloc(sizeof(struct tm));

    if(gm == NULL) {
      if(local == NULL) {
        free((void*)lcl);
      }

      return FALSE;
    }

    memcpy((void*)gm, (void*)gmtime(now), sizeof(struct tm));
  }

  if(output != NULL) {
    hourlcl = lcl->tm_hour;
    hourutc = gm->tm_hour;

    output2 = (char*)realloc((void*)*output, 6);

    if(output2 == NULL) {
      if(local == NULL) {
        free((void*)lcl);
      }

      if(utc == NULL) {
        free((void*)gm);
      }

      return FALSE;
    }
    *output = output2;

    if(lcl->tm_year > gm->tm_year) {
      hourlcl+=24;
    }
    else if (gm->tm_year > lcl->tm_year) {
      hourutc+=24;
    }
    else if(lcl->tm_yday > gm->tm_yday) {
      hourlcl+=24;
    }
    else if (gm->tm_yday > lcl->tm_yday) {
      hourutc+=24;
    }

    difference = ((hourlcl - hourutc)*60)+lcl->tm_min - gm->tm_min;
    minute_difference = difference % 60;
    hour_difference = (difference - minute_difference)/60;

    if(hour_difference < 0) {
      format = "%03i%02i";
    }

    sprintf(*output, format, hour_difference, minute_difference);
  }

  if(local == NULL) {
    free((void*)lcl);
  }

  if(utc == NULL) {
    free((void*)gm);
  }

  return TRUE;
}
#include "querycsv.h"

struct resultColumnValue *getFirstRecord(
    struct resultTree *root,
    struct qryData *query
  ) {
  struct resultTree *currentResult;

  MAC_YIELD

  if(root == NULL) {
    return NULL;
  }

  currentResult = root;

  while(currentResult->left) {
    currentResult = currentResult->left;
  }

  return currentResult->columns;
}
#include "querycsv.h"

/* loop over each record in the result set, other than the first one */
void groupResultsInner(
    struct qryData *query,
    struct resultColumnValue *columns,
    int i
  ) {

  struct resultColumnValue *previousMatch;

  MAC_YIELD

  /* loop over each record in the result set, other than the first one */
  if(i) {
    previousMatch = query->match;
    query->match = columns;

    /* if the current record to look at is identical to the previous one */
    if(
        (query->groupByClause != NULL &&   /* if no group by clause then every record is part of one group */
        recordCompare(
          (void *)previousMatch,
          (void *)query->match,
          (void *)query
        ) != 0) ||
        i == query->recordCount
      ) {
      /* fix up the calculated columns that need it */
      getGroupedColumns(query);

      /* calculate remaining columns that make use of aggregation */
      getCalculatedColumns(query, previousMatch, TRUE);

      /* free the group text strings (to prevent heap fragmentation) */
      /* cleanup_groupedColumns(query, previousMatch); */

      query->useGroupBy = FALSE;

      /* append the record to the new result set */
      tree_insert(query, previousMatch, &(query->resultSet));

      query->useGroupBy = TRUE;
    }
    else {
      cleanup_matchValues(query, &previousMatch);
      free(previousMatch);
    }

    if(i < query->recordCount) {
      updateRunningCounts(query, query->match);
    }
  }
}
#include "querycsv.h"

/*skips the BOM if present from a file */
FILE *skipBom(const char *filename, long* offset) {
  FILE *file;

  MAC_YIELD

  file = fopen(filename, "rb");

  if (file != NULL) {
    /* skip over the bom if present */
    if(fgetc(file) == 239 && fgetc(file) == 187 && fgetc(file) == 191) {
      if(offset) {
        *offset = 3;
      }

      return file;
    }

    /* the byte order mark was not found, and calling ungetc multiple times is not */
    /* portable (doesn't work on cc65). Therefore we just close and reopen the file */
    fclose(file);

    file = fopen(filename, "rb");
  }

  if(offset) {
    *offset = 0;
  }

  return file;
}
#include "querycsv.h"

/* getUnicodeCharFast does not check for invalid or overlong bytes. */
/* it also presumes the the entire string is already in nfd form */
long getUnicodeCharFast(
    unsigned char **offset,
    unsigned char **str,
    int plusBytes,
    int *bytesMatched,
    void (*get)()
) {
  unsigned char *temp = (unsigned char *)(*(offset+plusBytes));

  MAC_YIELD
  
  if(*temp < 0x80) {
    /* return the information we obtained */
    *bytesMatched = 1;
    
    return (long)(*temp);
  }
  else if(*temp < 0xE0) {
    /* read 2 bytes */
    *bytesMatched = 2;
    
    return (long)((*(temp) << 6) + *(temp+1)) - 0x3080;
  }
  else if (*temp < 0xF0) {
    /* read 3 bytes */
    *bytesMatched = 3;
    
    return ((long)(*temp) << 12) + ((long)(*(temp+1)) << 6) + (long)(*(temp+2)) - 0xE2080;
  }
    
  /* read 4 bytes */
  *bytesMatched = 4;
  
  return ((long)(*temp) << 18) + ((long)(*(temp+1)) << 12) + ((long)(*(temp+2)) << 6) + (*(temp+3)) - 0x3C82080;
}
#include "querycsv.h"

void runCommand(char *string) {
  MAC_YIELD
/*
    //get the number of columns in a file
    if (strcmp(argv2[1], "--columns") == 0 && argc2 == 3) {
      return getColumnCount(argv2[2]);
    }

    //get the file offset of the start of the next record in a file
    else if (strcmp(argv2[1], "--next") == 0 && argc2 == 4) {
      return getNextRecordOffset(argv2[2], atol(argv2[3]));
    }

    //get the unescaped value of column X of the record starting at the file offset
    else if (strcmp(argv2[1], "--value") == 0 && argc2 == 5) {
      return getColumnValue(argv2[2], atol(argv2[3]), atoi(argv2[4]));
    }

    //get the unescaped trimmed value of column X of the record starting at the file offset
    else if (strcmp(argv2[1], "--trimval") == 0 && argc2 == 5) {
      return getColumnValue(argv2[2], atol(argv2[3]), atoi(argv2[4]));
    }

    //get the current date in ISO8601 format (local time with UTC offset)
    else if (strcmp(argv2[1], "--date") == 0 && argc2 == 3) {
      if(strcmp(argv2[2], "now") == 0) {
        return getCurrentDate();
      }
      else {
        fputs("not supported", stderr);

        return -1;
      }
    }

    //run a query
    else if(argc2 == 3) {
      return runQuery(argv2[2], argv2[1]);
    }
*/
}
#include "querycsv.h"

static struct hash4Entry numberEntry = { NULL, 127, 0, 0 };

struct hash4Entry *getLookupTableEntry(
    unsigned char **offset,
    unsigned char **str,
    int *lastMatchedBytes,
    void (*get)(),
    int firstChar
  ) {
  struct hash4Entry *temp = NULL, *temp2 = NULL;
  int totalBytes = 0;

  MAC_YIELD

  if(isNumberWithGetByteLength(*offset, lastMatchedBytes, firstChar)) {
    return &numberEntry;
  }
  
  while((temp = in_word_set((char const *)(*offset), totalBytes+(*lastMatchedBytes)))) {
    /* the match is so far holding up.  */

    /* keep this match for later as it may be the last one we find */
    temp2 = temp;

    /* add the byte length to the total */
    totalBytes += *lastMatchedBytes;
    
    /* get a code point */
    (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get))
    (offset, str, totalBytes, lastMatchedBytes, get);
  } 

  /* don't update the value passed to us if we didn't find any match at all */
  if(temp2 != NULL) {
    /* copy the match data into the output */
    *lastMatchedBytes = totalBytes;
  }
  
  return temp2;
}
#include "querycsv.h"

int isNumberWithGetByteLength(
    unsigned char *offset,
    int *lastMatchedBytes,
    int firstChar
) {
  int decimalNotFound = TRUE;
  unsigned char *string = offset;

  MAC_YIELD

  if(
      (*offset >= '0' && *offset <= '9') ||
      (firstChar && (
      (*offset == '.' && (*(offset+1) >= '0' && *(offset+1) <= '9')) ||
      (*offset == '-' && ((*(offset+1) >= '0' && *(offset+1) <= '9') || (*(offset+1) == '.' && (*(offset+2) >= '0' && *(offset+2) <= '9')))) ||
      (*offset == '+' && ((*(offset+1) >= '0' && *(offset+1) <= '9') || (*(offset+1) == '.' && (*(offset+2) >= '0' && *(offset+2) <= '9'))))
      ))) {

    if(*string == '-' || *string == '+') {
      string++;
    }
    
    while(
      (*string >= '0' && *string <= '9') ||
      (decimalNotFound && (*string == '.' || *string == ',') &&
      !(decimalNotFound = FALSE))
    ) {
      string++;
    }

    *(lastMatchedBytes)+=(string-offset)-1;

    return TRUE;
  }
  else {
    return FALSE;
  }
}
#include "querycsv.h"

#ifndef HAS_STRDUP

char* __fastcall__ strdup(const char* s) {
  char *res = NULL;

  reallocMsg((void**)&res, strlen(s) + 1);

  strcpy(res, s);
  return res;
}
#endif
#include "querycsv.h"

void cleanup_resultColumns(struct resultColumn *currentResultColumn) {
  struct resultColumn *next;

  MAC_YIELD
  
  while(currentResultColumn != NULL) {
    next = currentResultColumn->nextColumnInResults;

    free(currentResultColumn->resultColumnName);
    free(currentResultColumn);

    currentResultColumn = next;
  }
}
#include "querycsv.h"

/* append a character into a string with a given length, using realloc */
int strAppend(char c, char **value, size_t *strSize) {
  char *temp;

  MAC_YIELD

  /* validate inputs */
  /* increase value length by 1 character */

  /* update the string pointer */
  /* increment strSize */
  if(strSize != NULL) {
    if(value != NULL) {
      if((temp = realloc(*value, (*strSize)+1)) != NULL) {
      *value = temp;

      /* store the additional character */
      (*value)[*strSize] = c;
      }
      else {
      return FALSE;
      }
    }

    (*strSize)++;
  }

  return TRUE;
}
#include "querycsv.h"

/* write a formatted string into a string buffer. allocate/free memory as needed */
int d_sprintf(char **str, char *format, ...) {
  size_t newSize;
  char *newStr = NULL;
  va_list args;
  #ifndef HAS_VSNPRINTF
  FILE *pFile;
  #endif

  MAC_YIELD

  /* Check sanity of inputs */
  if(str == NULL || format == NULL) {
    return FALSE;
  }

  #ifdef HAS_VSNPRINTF
    /* get the space needed for the new string */
    va_start(args, format);
    newSize = (size_t)(vsnprintf(NULL, 0, format, args)+1); /* plus '\0' */
    va_end(args);
  #else
    /* open /dev/null so that we can get the space needed for the new string. */
    if ((pFile = fopen(DEVNULL, "wb")) == NULL) {
      return FALSE;
    }

    /* get the space needed for the new string */
    va_start(args, format);
    newSize = (size_t)(vfprintf(pFile, format, args)+1); /* plus '\0' */
    va_end(args);

    /* close the file. We don't need to look at the return code as we were writing to /dev/null */
    fclose(pFile);
  #endif

  /* Create a new block of memory with the correct size rather than using realloc */
  /* as any old values could overlap with the format string. quit on failure */
  if((newStr = (char*)malloc(newSize)) == NULL) {
    return FALSE;
  }

  /* do the string formatting for real. vsnprintf doesn't seem to be available on Lattice C */
  va_start(args, format);
  vsprintf(newStr, format, args);
  va_end(args);

  /* ensure null termination of the string */
  newStr[newSize] = '\0';

  /* free the old contents of the output if present */
  free(*str);

  /* set the output pointer to the new pointer location */
  *str = newStr;

  /* everything occurred successfully */
  return newSize;
}
#include "querycsv.h"

int hash_addString(
    struct columnReferenceHash *hashtable,
    char *str,
    struct columnReference *new_list
) {
    struct columnReference *current_list;
    struct columnRefHashEntry *new_list2;
    unsigned int hashval = hash_compare(hashtable, str);

    MAC_YIELD

    /* Does item already exist? */
    current_list = hash_lookupString(hashtable, str);

    /* item already exists, don't insert it again. */
    if (current_list != NULL) {
      return 2;
    }

    if ((new_list2 = malloc(sizeof(struct columnRefHashEntry))) == NULL) {
      return 1;
    }
    
    /* Insert into list */
    new_list2->referenceName = str;
    new_list2->content = new_list;
    new_list2->nextReferenceInHash = hashtable->table[hashval];

    
    hashtable->table[hashval] = new_list2;

    return 0;
}
#include "querycsv.h"

/* free memory and set the pointer to NULL. this function should hopefully get inlined */
void strFree(char **str) {
  MAC_YIELD
  
  free(*str);
  *str = NULL;
}
#include "querycsv.h"

void getGroupedColumns(struct qryData *query) {
  struct columnRefHashEntry *currentHashEntry;
  struct columnReference *currentReference;
  struct resultColumn *currentResultColumn;

  int i;

  MAC_YIELD

  /* for each column in the output result set ... */
  for(i = 0; i < query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {
      currentReference = currentHashEntry->content;

      while(currentReference != NULL) {
        /* ... check if the current column in the result set is a grouped one, and increment/set the group variables in the appropriate way */
        if(
            currentReference->referenceType == REF_EXPRESSION &&
            (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL &&
            currentResultColumn->groupType != GRP_NONE
          ) {

          /* convert the aggregation types that need it back into a string */
          switch(currentResultColumn->groupType) {
            case GRP_AVG:
            case GRP_DIS_AVG:
              currentResultColumn->groupNum = fdiv(currentResultColumn->groupNum,
                currentResultColumn->groupCount);
            case GRP_SUM:
            case GRP_DIS_SUM:
              ftostr(&(currentResultColumn->groupText),currentResultColumn->groupNum);
            break;
            case GRP_COUNT:
            case GRP_DIS_COUNT:
              d_sprintf(&(currentResultColumn->groupText), "%d", currentResultColumn->groupCount);
            break;
            case GRP_STAR:
              d_sprintf(&(currentResultColumn->groupText), "%d", query->groupCount);
            break;
          }
        }

        currentReference = currentReference->nextReferenceWithName;
      }

      /* go to the next reference in the hash table */
      currentHashEntry = currentHashEntry->nextReferenceInHash;
    }
  }
}
#include "querycsv.h"

void yyerror2(long lineno, char *text) {
  MAC_YIELD
  
  fprintf(stderr, TDB_LEX_UNTERMINATED);
}
#include "querycsv.h"

void cleanup_orderByClause(struct sortingList *currentSortingList) {
  struct sortingList *next;

  MAC_YIELD

  while(currentSortingList != NULL) {
    next = currentSortingList->nextInList;

    cleanup_expression(currentSortingList->expressionPtr);

    free(currentSortingList);

    currentSortingList = next;
  }
}
#include "querycsv.h"

void cleanup_matchValues(
    struct qryData *query,
    struct resultColumnValue **match
  ) {

  int i, len;

  MAC_YIELD

  /* for each column in the output result set ... */
  for(i = 0, len = query->columnCount; i < len; i++) {
    free((*match)[i].value);
  }
  
  free(*match);
  *match = NULL;
}
