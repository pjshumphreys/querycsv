#include "querycsv.h"

void parse_tableFactor(
    struct qryData *queryData,
    int isLeftJoin,
    char *fileName,
    char *tableName,
    int fileEncoding
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
  int encodingFromBom = ENC_UNKNOWN;

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
  csvFile = skipBom(columnText2, &headerByteLength, &encodingFromBom);
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

  if(encodingFromBom == ENC_UNKNOWN || encodingFromBom == ENC_CP1047) {
    if(fileEncoding == ENC_DEFAULT) {
      newTable->fileEncoding = ENC_INPUT;
    }
    else {
      newTable->fileEncoding = fileEncoding;
    }
  }
  else {
    newTable->fileEncoding = encodingFromBom;
  }

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
        newTable->fileEncoding,
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
