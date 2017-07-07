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
    while(currentReference != NULL) {
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
