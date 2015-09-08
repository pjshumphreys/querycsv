//#include "querycsv.h"
//#if WINDOWS
//  #inlcude "win32.h"
//#endif
#include <stdio.h>
#include <ctype.h>
 
int stricmp(const char *p1, const char *p2) {
  register unsigned char *s1 = (unsigned char *) p1;
  register unsigned char *s2 = (unsigned char *) p2;
  unsigned char c1, c2;
 
  do {
      c1 = (unsigned char) toupper((int)*s1++);
      c2 = (unsigned char) toupper((int)*s2++);
      if (c1 == '\0') {
            return c1 - c2;
      }
  } while (c1 == c2);
 
  return c1 - c2;
}

void parse_table_factor(struct qryData* queryData, int isLeftJoin, char* fileName, char* tableName) {
  FILE* csvFile;
  struct inputTable* newTable = NULL;
  struct columnReference* currentReference = NULL;
  struct columnReference* newReference;
  struct inputColumn* newColumn;
  char* columnText = NULL;
  char* columnText2 = NULL;
  size_t columnLength = 0;
  int recordContinues = TRUE;

  //ensure we are trying to open all the files we need
  if(queryData->parseMode != 0) {
    free(fileName);
    free(tableName);
    return;
  }

  //try to prevent heap fragmentation
  csvFile = (FILE *)malloc(sizeof(FILE));
  columnText = strdup(tableName);
  columnText2 = strdup(fileName);

  if(csvFile == NULL || columnText == NULL || columnText2 == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    exit(EXIT_FAILURE);
  }

  free(fileName);
  free(tableName);
  free(csvFile);
  
  //try opening the file specified in the query
  csvFile = fopen_skipBom(columnText2);
  tableName = strdup(columnText);
  free(columnText);
  free(columnText2);  //free the filename string data as we don't need it any more

  columnText = NULL;

  if(csvFile == NULL || tableName == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    exit(EXIT_FAILURE);
  }

  //allocate the storage space for the new table's data
  reallocMsg(TDB_MALLOC_FAILED, (void**)(&newTable), sizeof(struct inputTable));

  //start populating our newly created table record
  newTable->queryTableName = tableName;
  newTable->fileStream = csvFile;
  newTable->firstInputColumn = NULL;  //the table initially has no columns
  newTable->isLeftJoined = FALSE;
  newTable->noLeftRecord = TRUE;   //set just for initialsation purposes

  //add the new table to the linked list in the query data
  if(queryData->firstInputTable == NULL) {
    newTable->fileIndex = 1;
    newTable->nextInputTable = newTable;
    queryData->firstInputTable = newTable;
  }
  else {
    newTable->fileIndex = 0; // we have to fill the file indexes in afterwards because of left join reordering (queryData->firstInputTable->fileIndex)+1;

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
      newTable->nextInputTable = queryData->firstInputTable->nextInputTable;  //this is a circularly linked list until we've finished adding records
      queryData->firstInputTable->nextInputTable = newTable;
      queryData->firstInputTable = newTable;
    }
  }

  //read csv columns until end of line occurs
  do {
    newReference = NULL;
    newColumn = NULL;

    recordContinues = getCsvColumn(&(newTable->fileStream), &columnText, &columnLength, NULL, NULL, (queryData->params & PRM_TRIM) == 0);

    snprintf_d(&columnText, "_%s", columnText);

    //test whether a column with this name already exists in the hashtable
    currentReference = lookup_string(queryData->columnReferenceHashTable, columnText);

    //if a column with this name is already in the hash table
    if(currentReference == NULL) {
      //create the new column record
      reallocMsg(TDB_MALLOC_FAILED, (void**)(&newReference), sizeof(struct columnReference));

      //store the column name
      newReference->referenceName = columnText;

      //no other columns with the same name yet
      newReference->nextReferenceWithName = NULL;
     
      //insert the column into the hashtable
      add_string(queryData->columnReferenceHashTable, columnText, newReference);  
    }
    //otherwise, add it to the linked list
    else {
      //we don't need another copy of this text
      free(columnText);

      //create the new column record
      reallocMsg(TDB_MALLOC_FAILED, (void**)(&newReference), sizeof(struct columnReference));

      //store the column name
      newReference->referenceName = currentReference->referenceName;
      
      //if there's only been 1 column with this name up until now
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

    reallocMsg(TDB_MALLOC_FAILED, (void**)(&newColumn), sizeof(struct inputColumn));

    newReference->reference.columnPtr = newColumn;

    newColumn->fileColumnName = newReference->referenceName;

    //store the column's reference back to it's parent table
    newColumn->inputTablePtr = (void*)newTable;

    //we don't yet know whether this column will actually be used in the output query, so indicate this for stage 2's use
    newColumn->firstResultColumn = NULL;

    //populate the columnIndex and nextColumnInTable fields
    if(newTable->firstInputColumn == NULL) {
      newColumn->columnIndex = 1;
      newColumn->nextColumnInTable = newColumn;
      newTable->firstInputColumn = newColumn;
    }
    else {
      newColumn->columnIndex = (newTable->firstInputColumn->columnIndex)+1;
      newColumn->nextColumnInTable = newTable->firstInputColumn->nextColumnInTable;  //this is a circularly linked list until we've finished adding records
      newTable->firstInputColumn->nextColumnInTable = newColumn;
      newTable->firstInputColumn = newColumn;
    }

    //clean up for obtaining the next column
    columnText = NULL;
    columnLength = 0;
  } while(recordContinues);

  //keep the current offset of the csv file as we'll need it when we're searching for matching results 
  newTable->firstRecordOffset = ftell(newTable->fileStream);

  //keep an easy to retrieve count of the number of columns in the csv file
  newTable->columnCount = newTable->firstInputColumn->columnIndex;

  //cut the circularly linked list of columns in this table
  newColumn = newTable->firstInputColumn->nextColumnInTable;
  newTable->firstInputColumn->nextColumnInTable = NULL;
  newTable->firstInputColumn = newColumn;
}

struct resultColumn* parse_new_output_column(
    struct qryData * queryData,
    int isHidden2,
    int isCalculated2,
    char * resultColumnName2,
    int aggregationType
) {
  struct resultColumn* newResultColumn = NULL;

  reallocMsg(TDB_MALLOC_FAILED, (void**)(&newResultColumn), sizeof(struct resultColumn));

  //insert this new output column into the list in the query data
  if(queryData->firstResultColumn == NULL) {
    newResultColumn->resultColumnIndex = 0;
    queryData->firstResultColumn = newResultColumn;
    newResultColumn->nextColumnInResults = newResultColumn;
  }
  else {
    newResultColumn->resultColumnIndex = queryData->firstResultColumn->resultColumnIndex+1;
    newResultColumn->nextColumnInResults = queryData->firstResultColumn->nextColumnInResults;   //maintain circularly linked list for now
    queryData->firstResultColumn->nextColumnInResults = newResultColumn;
    queryData->firstResultColumn = newResultColumn;
  }
  
  //fill out the rest of the necessary fields
  newResultColumn->isHidden = isHidden2;
  newResultColumn->isCalculated = isCalculated2;
  newResultColumn->resultColumnName = resultColumnName2;
  newResultColumn->nextColumnInstance = NULL;   //TODO: this field needs to be filled out properly

  newResultColumn->groupType = aggregationType;
  newResultColumn->groupText = NULL;
  newResultColumn->groupNum = 0;
  newResultColumn->groupCount = 0;
  newResultColumn->groupingDone = FALSE;

  return newResultColumn;
}

//returns true if the column could not be found or is ambiguious
int parse_column_ref_unsuccessful(
    struct qryData* queryData,
    struct columnReference** result,
    char * tableName,
    char* columnName
  ) {

  struct columnReference* currentReference = NULL;

  //ensure we have built up the hash table with all data from the source csv files first
  if(queryData->parseMode != 1) {
    free(tableName);
    free(columnName);
    
    *result = NULL;
    return FALSE; //successful, but only because we don't really care about getting column references when in parse mode 0
  }

  //look up the specified column name in the hash table
  currentReference = lookup_string(queryData->columnReferenceHashTable, columnName);
  
  //if a table name was specified, try to find the first column reference
  //in the list thats of type 1 with a matching table name and return that
  if(tableName != NULL) {
    while (currentReference != NULL) {
      if(
          currentReference->referenceType == 1 &&
          strcmp(tableName, ((struct inputTable*)(currentReference->reference.columnPtr->inputTablePtr))->queryTableName) == 0
        ) {
        break;
      }

      //the table name didn't match on this loop iteration. maybe the next one?
      currentReference = currentReference->nextReferenceWithName;
    }
  }
  
  //does the match contain a subsequent match of the same type?
  //if the match contains a subsequent match of the same type, then the name is ambiguous so trigger an error
  else if(
      currentReference != NULL &&
      currentReference->nextReferenceWithName != NULL &&
      currentReference->nextReferenceWithName->referenceType == currentReference->referenceType
    ) {
      
    //the column name was ambiguous
    currentReference = NULL;
  }

  free(tableName);
  free(columnName);

  //if the name refers to a column which hasn't yet been put
  //into the output result set then stick it in there now
  if(
      currentReference != NULL &&
      currentReference->referenceType == REF_COLUMN &&
      currentReference->reference.columnPtr->firstResultColumn == NULL
    ) {
    //attach this output column reference to the input table column, so we'll
    //know where to output it when running the query
    currentReference->reference.columnPtr->firstResultColumn = parse_new_output_column(
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

struct resultColumn* parse_exp_commalist(
    struct qryData* queryData,
    struct expression* expressionPtr,
    char* resultColumnName,
    int aggregationType
  ) {

  struct columnReference* currentReference = NULL;
  struct columnReference* newReference = NULL;
  struct columnReference* newReference2 = NULL;
  struct resultColumn* newResultColumn = NULL;

  //ensure we have finished opening all the files we need
  if(queryData->parseMode != 1) {
    return NULL;
  }

  //increment the column count for display purposes
  if(aggregationType == GRP_NONE) {
    queryData->columnCount++;
  
    //if the expression wasn't given a name then provide it with a default one
    if(resultColumnName == NULL) {
      if(expressionPtr->type == EXP_COLUMN) {
        if((resultColumnName = strdup(((struct inputColumn*)(expressionPtr->unionPtrs.voidPtr))->fileColumnName)) == NULL) {
          fputs(TDB_MALLOC_FAILED, stderr);
          exit(EXIT_FAILURE);
        }
      }
      else if(snprintf_d(&resultColumnName, TDB_UNTITLED_COLUMN) == FALSE) {
        fputs(TDB_MALLOC_FAILED, stderr);
        exit(EXIT_FAILURE);
      }
    }
  }
  else {
    free(resultColumnName);

    queryData->hiddenColumnCount++;

    if(snprintf_d(&resultColumnName, "%d", queryData->hiddenColumnCount) == FALSE) {
      fputs(TDB_MALLOC_FAILED, stderr);
      exit(EXIT_FAILURE);
    }
  }

  // stick this new reference into the lookup table for identifiers
  /////////////////////////////////////////////////////////////////
  {
    //test if the key already exists
    currentReference = lookup_string(queryData->columnReferenceHashTable, resultColumnName);

    //no column with this name has been defined at all. just create the new column reference and insert it into the hash table
    if(currentReference == NULL) {
      reallocMsg(TDB_MALLOC_FAILED, (void**)(&newReference), sizeof(struct columnReference));

      newReference->referenceName = resultColumnName;
      newReference->nextReferenceWithName = NULL;

      add_string(queryData->columnReferenceHashTable, resultColumnName, newReference);
    }

    //otherwise the reference has already been used
    else {
      //the text already exists, so we don't need the copy in resultColumnName any longer
      free(resultColumnName);

      reallocMsg(TDB_MALLOC_FAILED, (void**)(&newReference), sizeof(struct columnReference));

      //if the reference returned is of type REF_COLUMN (i.e. a column in a csv file), make a copy of it, overwrite the original with the new one then insert the copy afterwards
      if(currentReference->referenceType == REF_COLUMN) {
        reallocMsg(TDB_MALLOC_FAILED, (void**)(&newReference2), sizeof(struct columnReference));

        //switch the current and new references
        memcpy(newReference, currentReference, sizeof(struct columnReference));
        memcpy(currentReference, newReference2, sizeof(struct columnReference));
        free(newReference2);
        currentReference->nextReferenceWithName = newReference;
        currentReference->referenceName = newReference->referenceName;
        newReference = currentReference;
      }

      //otherwise insert before the first reference of type REF_COLUMN
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

  //put the new reference into the output columns
  ///////////////////////////////////////////////
  {
    //if the expression is just a singular reference of a column in one of the csv files,
    //then use it as a non calculated column
    if(expressionPtr->type == EXP_COLUMN && aggregationType == GRP_NONE) {
      //get the csv file column from the expression leaf
      newResultColumn = ((struct inputColumn*)(expressionPtr->unionPtrs.voidPtr))->firstResultColumn;

      //don't store the result column against the reference as it will be stored against the column instead
      newReference->reference.calculatedPtr.firstResultColumn = NULL;

      //loop over each copy of the csv column in the result set,
      //testing if it's the most recently defined result column
      while (newResultColumn != NULL) {

        //if the expression is just the most recently referenced column,
        //then associate the reference with it also 
        if(queryData->firstResultColumn == newResultColumn) {
          queryData->firstResultColumn->isHidden = FALSE;
          queryData->firstResultColumn->resultColumnName = strdup(newReference->referenceName);

          return queryData->firstResultColumn;
        }

        //if the next column instance is null then this input column is not yet in the result set
        //or we may need to output the same input column multiple times,
        //so create a new output column copy then break the loop
        if(newResultColumn->nextColumnInstance == NULL) {
          newResultColumn->nextColumnInstance = parse_new_output_column(
              queryData,
              /*isHidden = */FALSE,
              /*isCalculated = */FALSE,
              /*resultColumnName = */strdup(newReference->referenceName),
              /*aggregationType = */GRP_NONE
            );

          return newResultColumn->nextColumnInstance;
        }
        
        //otherwise get the next instance then continue looping
        else {
          newResultColumn = newResultColumn->nextColumnInstance;
        }
      }

      return NULL;
    }

    //the most recently defined expression is not a direct column reference or the most recent reference is not this one.
    //add another column to the result set, marking it as being calculated if it refers to an expression
    //make it hidden if makeHidden is true (e.g. when the expression will be used in an aggregation)
    else {
      newReference->reference.calculatedPtr.firstResultColumn = parse_new_output_column(
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

struct expression * parse_scalar_exp(
    struct qryData* queryData,
    struct expression* leftPtr,
    int operator,
    struct expression* rightPtr
  ) {
  struct expression* expressionPtr = NULL;

  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg(TDB_MALLOC_FAILED, (void**)(&expressionPtr), sizeof(struct expression));

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
      
    //if the operator is commutable, then re-order so that the minimum table reference is on the left
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

    //The minimum table and column needed to evaluate this
    //sub-expression is the greater of the two operands
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

struct expression* parse_scalar_exp_literal(struct qryData* queryData, char* literal) {
  struct expression* expressionPtr = NULL;
  
  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg(TDB_MALLOC_FAILED, (void**)(&expressionPtr), sizeof(struct expression));

  expressionPtr->type = EXP_LITERAL;
  expressionPtr->value = NULL;
  expressionPtr->unionPtrs.voidPtr = strdup(literal);
  expressionPtr->minColumn = 0;
  expressionPtr->minTable = 0;

  expressionPtr->containsAggregates = FALSE;

  return expressionPtr;
}

/*produces a different node type depending on whether a csv column is being referered to*/
struct expression* parse_scalar_exp_column_ref(
    struct qryData* queryData,
    struct columnReference* referencePtr
  ) {
  struct expression *expressionPtr = NULL, *expressionColumnPtr;
  struct inputColumn* columnPtr = NULL;
  
  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg(TDB_MALLOC_FAILED, (void**)(&expressionPtr), sizeof(struct expression));

  expressionPtr->value = NULL;

  if(referencePtr->referenceType == REF_COLUMN) {
    expressionPtr->type = EXP_COLUMN;

    //unbox csv column reference
    columnPtr = referencePtr->reference.columnPtr;
    
    expressionPtr->unionPtrs.voidPtr = (void *)(columnPtr);
    expressionPtr->minColumn = columnPtr->columnIndex;
    expressionPtr->minTable = ((struct inputTable*)columnPtr->inputTablePtr)->fileIndex;

    //this line might need to be changed
    expressionPtr->containsAggregates = FALSE;
  }
  else {
    expressionPtr->type = EXP_CALCULATED;

    //unbox calculated expression column
    expressionColumnPtr = referencePtr->reference.calculatedPtr.expressionPtr;

    //TODO: fill out these 3 fields with the correct value
    expressionPtr->unionPtrs.voidPtr = (void *)(expressionColumnPtr);
    expressionPtr->minColumn = expressionColumnPtr->minColumn;
    expressionPtr->minTable = expressionColumnPtr->minTable;

    //this line might need to be changed
    expressionPtr->containsAggregates = expressionColumnPtr->containsAggregates;
  }
  
  return expressionPtr;
}

void parse_where_clause(
    struct qryData* queryData,
    struct expression* expressionPtr
  ) {

  struct expression* currentClauses;
  struct expression* newClause;

  if(queryData->parseMode != 1) {
    return;
  }

  if(queryData->joinsAndWhereClause == NULL) {
    //first join or where clause. just use it dirrectly
    queryData->joinsAndWhereClause = expressionPtr;
  }
  else {
    //get current clauses
    currentClauses = queryData->joinsAndWhereClause;

    newClause = NULL;

    reallocMsg(
        "couldn't allocate clause memory",
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

      //The minimum table and column needed to evaluate this
      //sub-expression is the greater of the two operands
      newClause->minColumn = currentClauses->minColumn;
      newClause->minTable = currentClauses->minTable;
    }
    else {
      newClause->unionPtrs.leaves.leftPtr = currentClauses;
      newClause->unionPtrs.leaves.rightPtr = expressionPtr;

      //The minimum table and column needed to evaluate this
      //sub-expression is the greater of the two operands
      newClause->minColumn = expressionPtr->minColumn;
      newClause->minTable = expressionPtr->minTable;
    }

    queryData->joinsAndWhereClause = newClause;
  }
}

struct expression* parse_in_predicate(
    struct qryData* queryData,
    struct expression* leftPtr,
    int isNotIn,
    struct atomEntry* lastEntryPtr
  ) {
  struct expression* expressionPtr = NULL;

  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg(TDB_MALLOC_FAILED, (void**)(&expressionPtr), sizeof(struct expression));

  expressionPtr->type = isNotIn==TRUE?EXP_NOTIN:EXP_IN;
  expressionPtr->value = NULL;
  expressionPtr->unionPtrs.inLeaves.leftPtr = leftPtr;
  expressionPtr->unionPtrs.inLeaves.lastEntryPtr = lastEntryPtr;
  expressionPtr->minColumn = leftPtr->minColumn;
  expressionPtr->minTable = leftPtr->minTable;

  expressionPtr->containsAggregates = leftPtr->containsAggregates;

  return expressionPtr;
}

struct atomEntry* parse_atom_commalist(
    struct qryData* queryData,
    struct atomEntry* lastEntryPtr,
    char* newEntry
  ) {

  struct atomEntry* newEntryPtr;
  
  if(queryData->parseMode != 1) {
    return NULL;
  }

  reallocMsg(TDB_MALLOC_FAILED, (void**)(&newEntryPtr), sizeof(struct atomEntry));

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

void parse_ordering_spec(
    struct qryData* queryData,
    struct expression *expressionPtr,
    int isDescending
  ) {
    
  struct sortingList* sortingListPtr = NULL;

  if(queryData->parseMode != 1) {
    return;
  }
  
  reallocMsg(
      "couldn't allocate sortingList entry",
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

void parse_grouping_spec(
    struct qryData* queryData,
    struct expression *expressionPtr
  ) {
    
  struct sortingList* sortingListPtr = NULL;

  if(queryData->parseMode != 1) {
    return;
  }
  
  reallocMsg(
      "couldn't allocate sortingList entry",
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

struct expression* parse_function_ref(
    struct qryData* queryData,
    long aggregationType,
    struct expression *expressionPtr,
    int isDistinct
  ) {

  struct expression* expressionPtr2 = NULL;
  struct resultColumn* columnPtr = NULL;
  
  if(queryData->parseMode != 1) {
    return NULL;
  }

  if(isDistinct) {
    aggregationType+=GRP_STAR;
  }

  if(expressionPtr->containsAggregates) {
    //I don't think in sql you can aggregate an aggregate.
    //therefore we should error out if we get to this point
    fprintf(stderr,"can't aggregate an aggregate");
    exit(EXIT_FAILURE);
  }

  queryData->hasGrouping = TRUE;  //at least one, perhaps more
  
  //parse_exp_commalist is used to put an expression into a
  //new, hidden, calculated column in the output result set
  columnPtr = parse_exp_commalist(queryData, expressionPtr, NULL, aggregationType);   //parse_exp_commalist returns an output column pointer

  //we then want to create a new expression node that references this new column
  //create an expression node that references the new hidden column
  reallocMsg(TDB_MALLOC_FAILED, (void**)(&expressionPtr2), sizeof(struct expression));

  expressionPtr2->type = EXP_GROUP;
  expressionPtr2->value = NULL;

  expressionPtr2->unionPtrs.voidPtr = (void *)(columnPtr);  //the expression nodes reference points directly to the hidden column
  expressionPtr2->minColumn = expressionPtr->minColumn;
  expressionPtr2->minTable = expressionPtr->minTable;

  //change the ref type back up the expression tree to be calculated later
  expressionPtr2->containsAggregates = TRUE;

  return expressionPtr2;
} 

struct expression* parse_function_ref_star(
    struct qryData* queryData,
    long aggregationType
  ) {

  if(queryData->parseMode != 1) {
    return NULL;
  }

  if(aggregationType != GRP_COUNT) {
    //I don't think in sql you can aggregate an aggregate.
    //therefore we should error out if we get to this point
    fprintf(stderr,"only count(*) is valid");
    exit(EXIT_FAILURE);
  }
  
  return parse_function_ref(
    queryData,
    GRP_STAR,
    parse_scalar_exp_literal(queryData, ""),
    FALSE
  );
}
