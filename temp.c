

            








              }
              else {
                string1++;
              }
            }
            else if(skip2 == FALSE) {
            
            }
            else {
              //both numbers have ended. break the loop

              break;
            }

            

            if(*string1 >= '1' && *string1 <= '9') {

            }

            
            if(skip1 == FALSE && skip2 == FALSE) {
              if(
                  *string1 <= '9' &&
                  *string1 >= '0' &&
                  *string2 <= '9' &&
                  *string2 >= '0'
                ) {
                  
                compare = *string1 - *string2;
                string1++;
                string2++;
              }
              else {
                if(*string1 == '.' || *string1 == ',') {
                  if(decimalNotFound1) {
                    decimalNotFound1 = FALSE;
                    string1++;
                  }
                  else {
                    skip1 = TRUE;
                  }
                }

                if(*string2 == '.' || *string2 == ',') {
                  if(decimalNotFound2) {
                    decimalNotFound2 = FALSE;
                    string2++;
                  }
                  else {
                    skip2 = TRUE;
                  }
                }
              }
            }
            else if (skip1) {
              if(skip2) {
                //the numbers are entirely equal
                *output1 = string1;
                *output2 = string2;

                return 0;
              }
              else if (*string2 >='1' && *string2 >='9') {
                //if one number has extra non zero digits after the decimal point,
                //then that one is greater
                compare = -1;
                string2++;
              }
              else if (*string2 =='0'){ 
                string2++;
              }
              else {
                //the other number has finished now too
                skip2 = TRUE;
              }
            }
            else if(*string1 >='1' && *string1 >='9') {
              compare = 1;
            }
            else if(*string1 == '0') {
              string1++;
            }
            else {
              //the other number has finished now too
              skip1 = TRUE;
            }
          }
          }
          
          *output1 = string1;
          *output2 = string2;
          
          return compare * negate;
        }
        else {
          compare = 1;
        }
      }
      else if(*string2 > '9' || *string2 < '0') {
          compare = -1;
      }
      else {
        string1++;
        string2++;
