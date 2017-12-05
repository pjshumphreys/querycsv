#include "querycsv.h"

void readParams(struct qryData *queryData, char *string) {
  int params;
  char* i;

  MAC_YIELD

  if(queryData->parseMode == 1 || string == NULL) {
    return;
  }

  params = 0;
  i = string;

  while(*i) {
    switch(*i) {
      case 'p':
      case 'P':
        params |= PRM_TRIM;
      break;

      case 't':
      case 'T':
        params |= PRM_SPACE;
      break;
      case 'i':
      case 'I':
        params |= PRM_IMPORT;
      break;

      case 'e':
      case 'E':
        params |= PRM_EXPORT;
      break;

      case 'b':
      case 'B':
        params |= PRM_BOM;
      break;
    }

    i++;
  }

  queryData->params = params;
}
