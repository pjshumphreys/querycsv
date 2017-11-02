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
        if((resultColumnName = mystrdup(((struct inputColumn*)(expressionPtr->unionPtrs.voidPtr))->fileColumnName)) == NULL) {
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
      while(newResultColumn != NULL) {

        /* if the expression is just the most recently referenced column, */
        /* then associate the reference with it also  */
        if(queryData->firstResultColumn == newResultColumn) {
          queryData->firstResultColumn->isHidden = FALSE;
          queryData->firstResultColumn->resultColumnName = mystrdup(newReference->referenceName);

          return queryData->firstResultColumn;
        }

        /* if the next column instance is null then this input column is
        not yet in the result set or we may need to output the same input
        column multiple times, so create a new output column copy then
        break the loop */
        if(newResultColumn->nextColumnInstance == NULL) {
          newResultColumn->nextColumnInstance = parse_newResultColumn(
              queryData,
              /*isHidden = */FALSE,
              /*isCalculated = */FALSE,
              /*resultColumnName = */mystrdup(newReference->referenceName),
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
      newReference->reference.calculatedPtr.firstResultColumn = parse_newResultColumn(
          queryData,
          /*isHidden = */aggregationType != GRP_NONE,
          /*isCalculated = */TRUE,
          /*resultColumnName = */mystrdup(newReference->referenceName),
          /*aggregationType = */aggregationType
        );

      return newReference->reference.calculatedPtr.firstResultColumn;
    }
  }
}

