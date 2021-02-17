void updateRunningCounts(
    struct qryData *query,
    struct resultTree *item
) {
  struct resultTree *tempItem;
  struct resultColumnValue *match = item->columns;

  struct columnRefHashEntry *currentHashEntry;
  struct columnReference *currentReference;
  struct resultColumn *currentResultColumn;
  struct resultColumnValue *field;
  char *tempString = NULL;
  char *tempString2 = NULL;
  double tempFloat;

  int i, j = -1;

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

          /* if query->groupCount = 1 then we're starting a new group */
          if(query->groupCount == 1) {
            freeAndZero(currentResultColumn->groupText);
            currentResultColumn->groupNum = ctof(0);
            currentResultColumn->groupCount = 0;
          }

          field = &(match[currentResultColumn->resultColumnIndex]);

          if(field->isNull == FALSE) {
            stringGet((unsigned char **)(&tempString), field);

            /* distinct groupings. only add to the count if the column value hasn't already been seen */
            if(currentResultColumn->groupType > GRP_STAR) { /* distinct variants */
              if(query->groupCount > 1) {
                tempItem = item->link[0];

                for(j = 1; j < query->groupCount; j++) {
                  stringGet((unsigned char **)(&tempString2), &(tempItem->columns[currentResultColumn->resultColumnIndex]));

                  if(strCompare(
                    (unsigned char **)(&tempString),
                    (unsigned char **)(&tempString2),
                    2,
                    (void (*)(void))getUnicodeChar,
                    (void (*)(void))getUnicodeChar
                  ) == 0) {
                    freeAndZero(tempString2);
                    break;
                  }

                  freeAndZero(tempString2);
                  tempItem = tempItem->link[0];
                }
              }
              else {
                j = query->groupCount;
              }
            }

            switch(currentResultColumn->groupType) {
              case GRP_DIS_COUNT: {
                if(j == query->groupCount) {
                  currentResultColumn->groupCount++;
                }
              } break;

              case GRP_COUNT: {
                currentResultColumn->groupCount++;
              } break;

              case GRP_DIS_AVG:
              case GRP_DIS_SUM: {
                if(j == query->groupCount) {
                  currentResultColumn->groupCount++;
                  tempFloat = strctod(tempString, NULL);
                  currentResultColumn->groupNum = fadd(currentResultColumn->groupNum, tempFloat);
                }
              } break;

              case GRP_AVG:
              case GRP_SUM: {
                currentResultColumn->groupCount++;
                tempFloat = strctod(tempString, NULL);
                currentResultColumn->groupNum = fadd(currentResultColumn->groupNum, tempFloat);
              } break;

              case GRP_DIS_CONCAT: {
                if(j == query->groupCount) {
                  if(currentResultColumn->groupText != NULL) {
                    d_sprintf(
                      &(currentResultColumn->groupText),
                      S_STRING S_STRING S_STRING,
                      currentResultColumn->groupText,
                      currentResultColumn->groupSeparator,
                      tempString
                    );
                  }
                  else {
                    currentResultColumn->groupText = mystrdup(tempString != NULL ? tempString : "");
                  }
                }
              } break;

              case GRP_CONCAT: {
                if(currentResultColumn->groupText != NULL) {
                  d_sprintf(
                    &(currentResultColumn->groupText),
                    S_STRING S_STRING S_STRING,
                    currentResultColumn->groupText,
                    currentResultColumn->groupSeparator,
                    tempString
                  );
                }
                else {
                  currentResultColumn->groupText = mystrdup(tempString != NULL ? tempString : "");
                }
              } break;

              case GRP_MIN:
              case GRP_DIS_MIN: {
                if(currentResultColumn->groupText == NULL || strCompare(
                    (unsigned char **)(&tempString),
                    (unsigned char **)(&(currentResultColumn->groupText)),
                    2,    /* TRUE, */
                    (void (*)(void))getUnicodeChar,
                    (void (*)(void))getUnicodeChar
                  ) == -1) {

                  free(currentResultColumn->groupText);
                  currentResultColumn->groupText = tempString;
                  tempString = NULL;
                  currentReference = currentReference->nextReferenceWithName;
                  continue;
                }
              } break;

              case GRP_MAX:
              case GRP_DIS_MAX: {
                if(currentResultColumn->groupText == NULL || strCompare(
                    (unsigned char **)(&tempString),
                    (unsigned char **)(&(currentResultColumn->groupText)),
                    2,    /* TRUE, */
                    (void (*)(void))getUnicodeChar,
                    (void (*)(void))getUnicodeChar
                  ) == 1) {

                  free(currentResultColumn->groupText);
                  currentResultColumn->groupText = tempString;
                  tempString = NULL;
                  currentReference = currentReference->nextReferenceWithName;
                  continue;
                }
              } break;
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
