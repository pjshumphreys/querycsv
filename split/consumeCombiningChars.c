#include "querycsv.h"

int consumeCombiningChars(unsigned char **str1, unsigned char **str2, unsigned char **offset1, unsigned char **offset2, void (*get1)(), void (*get2)(), int * bytesMatched1, int * bytesMatched2, int * accentcheck)
{
  int combiner1, combiner2, skip1 = FALSE, skip2 = FALSE;

  (*offset1)+=(*bytesMatched1);
  (*offset2)+=(*bytesMatched2);

  do {
    if(skip1 == FALSE) {
     combiner1 = isCombiningChar(
          (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get1))
          (offset1, str1, 0, bytesMatched1, get1)
      );
    }
    
    if(skip2 == FALSE) {
      combiner2 = isCombiningChar(
          (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))
          (offset2, str2, 0, bytesMatched2, get2)
      );
    }

    if(skip1 == FALSE && skip2 == FALSE) {
      if(combiner1 != combiner2) {
        //if either of them is not a combining character or not equal then accentcheck is set to 1
        //if accentcheck is already 1 then return the one thats greater
        if(*accentcheck == 2) {
          return (combiner1 > combiner2) ? 1 : -1;
        }
        else {
          *accentcheck = 1;
        }
      }
    }

    if(combiner1 == 0) {
      skip1 = TRUE; 
    }
    else {
      (*offset1)+=(*bytesMatched1);
    }

    if(combiner2 == 0) {
      skip2 = TRUE; 
    }
    else {
      (*offset2)+=(*bytesMatched2);
    }
  } while (skip1 == FALSE || skip2 == FALSE);

  return 0;
}

