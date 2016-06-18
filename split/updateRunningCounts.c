#include "querycsv.h"

void updateRunningCounts(struct qryData * query, struct resultColumnValue * match) {
  struct columnRefHashEntry* currentHashEntry;
  struct columnReference* currentReference;
  struct resultColumn* currentResultColumn;
  struct resultColumnValue* field;
  char* tempString = NULL;
  char* tempString2 = NULL;

  int i, j;

  query->groupCount++;

  //for each column in the output result set ...
  for(i = 0; i < query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {

      currentReference = currentHashEntry->content;

      while(currentReference != NULL) {
        //... check if the current column in the result set is a grouped one, and increment/set the group variables in the appropriate way
        if(
            currentReference->referenceType == REF_EXPRESSION &&
            (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL &&
            currentResultColumn->groupType != GRP_NONE
          ) {
          field = &(match[currentResultColumn->resultColumnIndex]);

          if(field->leftNull == FALSE) {
            stringGet(&tempString, field, query->params);

            if(currentResultColumn->groupType > GRP_STAR) {
              if(query->groupCount > 1) {
                for(j = 1; j < query->groupCount; j++) {
                  stringGet(&tempString2, &(match[(currentResultColumn->resultColumnIndex) - (query->columnCount)]), query->params);

                  if(strCompare(
                    &tempString,
                    &tempString2,
                    TRUE,
                    (void (*)())getUnicodeChar,
                    (void (*)())getUnicodeChar
                  ) == 0) {
                    strFree(&tempString2);
                    break;
                  }

                  strFree(&tempString2);
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
                  currentResultColumn->groupNum += strtod(tempString, NULL);
                }
              break;

              case GRP_AVG:
              case GRP_SUM:
                currentResultColumn->groupCount++;
                currentResultColumn->groupNum += strtod(tempString, NULL);
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
                    &tempString,
                    &(currentResultColumn->groupText),
                    2,//TRUE,
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
                    &tempString,
                    &(currentResultColumn->groupText),
                    2,//TRUE,
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

            strFree(&tempString);
          }
        }

        currentReference = currentReference->nextReferenceWithName;
      }

      //go to the next reference in the hash table
      currentHashEntry = currentHashEntry->nextReferenceInHash;
    }
  }
}
