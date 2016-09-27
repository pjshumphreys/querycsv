#include "querycsv.h"

void exp_divide(char **value, double leftVal, int rightVal) {
  MAC_YIELD
  
  if(rightVal == 0.0) {
    *value = strdup("Infinity");
  }
  else {
    d_sprintf(value, "%g", leftVal/rightVal);
  }
}
