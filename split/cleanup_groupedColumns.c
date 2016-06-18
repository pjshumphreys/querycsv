#include "querycsv.h"

void cleanup_groupedColumns(struct qryData * query, struct resultColumnValue * match)
{
  struct columnRefHashEntry* currentHashEntry;
  struct columnReference* currentReference;
  struct resultColumn* currentResultColumn;

  int i,j;

  query->groupCount = 0;

  //for each column in the output result set ...
  for(i = 0; i < query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {

      currentReference = currentHashEntry->content;

      while(currentReference != NULL) {
        //... check if the current column in the result set is a grouped one
        if(
            currentReference->referenceType == REF_EXPRESSION &&
            (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL &&
            currentResultColumn->groupType != GRP_NONE
          ) {

          //write groupText value into the scratchpad,
          //so we can order by aggregate columns later
          ////////////////////////////////////////////

          //start setting column value fields
          j = currentResultColumn->resultColumnIndex;
          match[j].isQuoted = FALSE;
          match[j].isNormalized = FALSE;
          match[j].source = &(query->scratchpad);

          //seek to the end of the scratchpad file and update the start position
          fseek(query->scratchpad, 0, SEEK_END);
          fflush(query->scratchpad);
          match[j].startOffset = ftell(query->scratchpad);

          //store the value's length
          if(currentResultColumn->groupText == NULL) {
            match[j].length = 0;
            match[j].leftNull = TRUE;
          }
          else {
            match[j].leftNull = FALSE;
            match[j].length = strlen(currentResultColumn->groupText);

            //write the value to the scratchpad file
            fputs(currentResultColumn->groupText, query->scratchpad);
          }

          strFree(&(currentResultColumn->groupText));
          currentResultColumn->groupNum = 0;
          currentResultColumn->groupCount = 0;
        }

        currentReference = currentReference->nextReferenceWithName;
      }

      //go to the next reference in the hash table
      currentHashEntry = currentHashEntry->nextReferenceInHash;
    }
  }
}
