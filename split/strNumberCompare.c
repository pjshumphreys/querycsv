#include "querycsv.h"

int strNumberCompare(char *input1, char *input2) {
  char* string1 = input1;
  char* string2 = input2;
  int decimalNotFound1 = TRUE;
  int decimalNotFound2 = TRUE;
  int compare = 0;
  int negate = 1;
  int do1 = TRUE;
  int do2 = TRUE;

  MAC_YIELD

  /* if the first character of only one of the numbers is negative, */
  /* then the positive one is greater. We still need to skip the digits though */
  if(*string1 == '-') {
    string1++;

    if(*string2 != '-') {
      if(*string2 == '+') {
        string2++;
      }

      compare = -1;
    }
    else {
      /* if both are negative, then the subsequent results need to be reversed */
      negate = -1;
      string2++;
    }
  }
  else {
    if(*string1 == '+') {
      string1++;
    }

    if(*string2 == '-') {
      string2++;
      compare = 1;
    }
    else if(*string2 == '+') {
      string2++;
    }
  }

  if(compare == 0) {
    /* skip the leading zeros of both numbers */
    while(*string1 == '0') {
      string1++;
    }

    while(*string2 == '0') {
      string2++;
    }

    input1 = string1;
    input2 = string2;

    /* the number whose digits finish first is the smaller. */
    /* We still need to skip the remaining digits though */
    while(compare == 0) {
      if(*string1 > '9' || *string1 < '0') {
        if(*string2 > '9' || *string2 < '0') {
          /* if both numbers finish at the same time then */
          /* rewind and look at the individual digits */
          /* we don't need to skip the remaining digits */
          string1 = input1;
          string2 = input2;

          for ( ; ; ) {
            if(do1) {
              if(decimalNotFound1 && (*string1 == '.' || *string1 == ',')) {
                decimalNotFound1 = FALSE;
              }
              else if(*string1 > '9' || *string1 < '0') {
                do1 = FALSE;
              }
              else if (do2 == FALSE && *string1 != '0') {
                compare = -1;
                break;
              }
            }

            if(do2) {
              if(decimalNotFound2 && (*string2 == '.' || *string2 == ',')) {
                decimalNotFound2 = FALSE;
              }
              else if(*string2 > '9' || *string2 < '0') {
                do2 = FALSE;
              }
              else if (do1 == FALSE && *string2 != '0') {
                compare = 1;
                break;
              }
            }

            if(do1) {
              if(do2) {
                /* neither number has finished yet. */
                if(*string1 >= '0' &&
                    *string1 <= '9' &&
                    *string2 >= '0' &&
                    *string2 <= '9' &&
                    *string1 != *string2) {
                  compare = *string1 < *string2?-1:1;
                  break;
                }

                string2++;
              }

              string1++;
            }
            else if(do2) {
              string2++;
            }
            else {
              /* both numbers have finished */
              /* the numbers are entirely equal */

              return 0;
            }
          }
        }
        else {
          compare = -1;
        }
      }
      else if(*string2 > '9' || *string2 < '0') {
        compare = 1;
      }
      else {
        string1++;
        string2++;
      }
    }
  }

  return compare * negate;
}
