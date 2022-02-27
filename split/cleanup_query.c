void cleanup_query(struct qryData *query) {
  int i;
  unsigned char c;

  MAC_YIELD

  /* cleanup_columnReferences */
  {
    struct columnReferenceHash *table = query->columnReferenceHashTable;
    struct columnRefHashEntry *currentHashEntry, *nextHashEntry;
    struct columnReference *currentReference, *nextReference;

    for(i = 0; i < table->size; i++) {
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

  /* cleanup_resultColumns */
  {
    struct resultColumn *currentResultColumn = query->firstResultColumn;
    struct resultColumn *next;

    while(currentResultColumn != NULL) {
      next = currentResultColumn->nextColumnInResults;

      free(currentResultColumn->resultColumnName);
      free(currentResultColumn->groupText);
      free(currentResultColumn->groupSeparator);
      free(currentResultColumn);

      currentResultColumn = next;
    }
  }

  /* cleanup_groupByClause */
  {
    struct sortingList *currentSortingList = query->groupByClause;
    struct sortingList *next;

    while(currentSortingList != NULL) {
      next = currentSortingList->nextInList;

      cleanup_expression(currentSortingList->expressionPtr);

      free(currentSortingList);

      currentSortingList = next;
    }
  }

  /* cleanup_orderByClause */
  {
    struct sortingList *currentSortingList = query->orderByClause;
    struct sortingList *next;

    MAC_YIELD

    while(currentSortingList != NULL) {
      next = currentSortingList->nextInList;

      cleanup_expression(currentSortingList->expressionPtr);

      free(currentSortingList);

      currentSortingList = next;
    }
  }

  cleanup_expression(query->joinsAndWhereClause);

  /* cleanup_inputTables */
  {
    struct inputTable *currentInputTable = query->firstInputTable;
    struct inputTable *next;

    while(currentInputTable != NULL) {
      next = currentInputTable->nextInputTable;

      free(currentInputTable->queryTableName);
      free(currentInputTable->fileName);
      fclose(currentInputTable->fileStream);
      free(currentInputTable);

      currentInputTable = next;
    }
  }

  free(query->dateString);

  if(query->outputFileName) {
    if(query->outputEncoding == ENC_TSW) {
      if(query->outputOffset % 64 != 0) {
        /* add EOF byte and padding with spaces */
        i = 64 - (query->outputOffset % 64);

        c = query->params & PRM_REMOVE ? 32 : 143;
        fwrite((void*)&c, 1, 1, query->outputFile);

        i--;

        c = 32;

        while(i--) {
          fwrite((void*)&c, 1, 1, query->outputFile);
        }
      }
    }

    #ifdef __Z88DK
      else {
        /* output a soft-EOF byte to end the file on cp/m or zx spectrum builds */
        c = 26;
        fwrite((void*)&c, 1, 1, query->outputFile);
      }
    #endif

    fclose(query->outputFile);

    #ifdef MPW_C
      /* Set the fourcc type and creator codes for macos.
         the function "fsetfileinfo" can be used to change
         the creator and type code for a file.
      */
      switch(query->outputEncoding) {
        case ENC_UTF16LE:
        case ENC_UTF16BE:
        case ENC_UTF32LE:
        case ENC_UTF32BE: {
          /*
          * SUE (the carbon version) is the only editor I know
          * that can read unicode text on macos 8.6 in practice.
          */
          fsetfileinfo_absolute(query->outputFileName, 'SUE ', 'utxt');
        } break;

        default: {
          fsetfileinfo_absolute(query->outputFileName, 'SUE ', 'TEXT');
        }
      }
    #endif

    free(query->outputFileName);
  }
}
