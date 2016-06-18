#include "querycsv.h"

void exp_uminus(char** value, double leftVal)
{
  if(leftVal == 0.0) {
    *value = strdup("0");
  }
  else {
    d_sprintf(value, "%g", leftVal*-1);
  }
}
