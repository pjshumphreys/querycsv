#include "querycsv.h"

void readCommandLine(char* string, int * params)
{
  if(string && params) {
    while(*string) {
      switch(*string) {
        case 'p':
        case 'P':
          *params |= PRM_TRIM;
        break;

        case 't':
        case 'T':
          *params |= PRM_SPACE;
        break;
        case 'i':
        case 'I':
          *params |= PRM_IMPORT;
        break;

        case 'e':
        case 'E':
          *params |= PRM_EXPORT;
        break;

        case 'b':
        case 'B':
          *params |= PRM_BOM;
        break;
      }
      string++;
    }
  }
}
