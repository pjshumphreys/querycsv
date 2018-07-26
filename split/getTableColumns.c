void getTableColumns(struct qryData *query, struct inputTable *currentInputTable) {
  int recordContinues = TRUE;
  long headerByteLength = currentInputTable->firstRecordOffset;

  struct columnReference *currentReference;
  struct columnReference *newReference;
  struct inputColumn *newColumn;
  char *columnText;
  size_t columnLength;

  MAC_YIELD

  /* read csv columns until end of line occurs */
  do {
    columnLength = 0;
    columnText = NULL;
    newReference = NULL;
    newColumn = NULL;

    recordContinues = getCsvColumn(
        currentInputTable,
        &columnText,
        &columnLength,
        NULL,
        &headerByteLength,
        !(query->params & PRM_TRIM),
        query->newLine
      );

    d_sprintf(&columnText, "_%s", columnText);

    /* test whether a column with this name already exists in the hashtable */
    currentReference = hash_lookupString(query->columnReferenceHashTable, columnText);

    /* if a column with this name is not already in the hash table */
    if(currentReference == NULL) {
      /* create the new column record */
      reallocMsg((void**)(&newReference), sizeof(struct columnReference));

      /* store the column name */
      newReference->referenceName = columnText;

      /* no other columns with the same name yet */
      newReference->nextReferenceWithName = NULL;

      /* insert the column into the hashtable */
      hash_addString(query->columnReferenceHashTable, columnText, newReference);
    }
    /* otherwise, add it to the linked list */
    else {
      /* we don't need another copy of this text */
      free(columnText);

      /* create the new column record */
      reallocMsg((void**)(&newReference), sizeof(struct columnReference));

      /* store the column name */
      newReference->referenceName = currentReference->referenceName;

      newReference->nextReferenceWithName = NULL;
      /* if there's only been 1 column with this name up until now */
      while(currentReference->nextReferenceWithName != NULL) {
        currentReference = currentReference->nextReferenceWithName;
      }

      currentReference->nextReferenceWithName = newReference;
    }

    newReference->referenceType = REF_COLUMN;

    reallocMsg((void**)(&newColumn), sizeof(struct inputColumn));

    newReference->reference.columnPtr = newColumn;

    newColumn->fileColumnName = newReference->referenceName;

    /* store the column's reference back to it's parent table */
    newColumn->inputTablePtr = (void*)currentInputTable;

    /* we don't yet know whether this column will actually be used in the output query, so indicate this for stage 2's use */
    newColumn->firstResultColumn = NULL;

    /* populate the columnIndex and nextColumnInTable fields */
    if(currentInputTable->firstInputColumn == NULL) {
      newColumn->columnIndex = 1;
      newColumn->nextColumnInTable = newColumn;
      currentInputTable->firstInputColumn = newColumn;
    }
    else {
      newColumn->columnIndex = (currentInputTable->firstInputColumn->columnIndex)+1;
      newColumn->nextColumnInTable = currentInputTable->firstInputColumn->nextColumnInTable;  /* this
      is a circularly linked list until we've finished adding records */
      currentInputTable->firstInputColumn->nextColumnInTable = newColumn;
      currentInputTable->firstInputColumn = newColumn;
    }
  } while(recordContinues);

  /* keep the current offset of the csv file as we'll need it when we're searching
  for matching results. avoid using ftell as it doesn't work in cc65 */
  currentInputTable->firstRecordOffset = headerByteLength;

  /* keep an easy to retrieve count of the number of columns in the csv file */
  currentInputTable->columnCount = currentInputTable->firstInputColumn->columnIndex;

  /* cut the circularly linked list of columns in this table */
  newColumn = currentInputTable->firstInputColumn->nextColumnInTable;
  currentInputTable->firstInputColumn->nextColumnInTable = NULL;
  currentInputTable->firstInputColumn = newColumn;
}
