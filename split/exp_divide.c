#include "querycsv.h"

void exp_divide(char **value, double leftVal, double rightVal) {
  double temp;

  MAC_YIELD

  temp = ctof(0);

  if(fcmp(rightVal, temp)) {
    temp = fdiv(leftVal, rightVal);

    ftostr(value, temp);
  }
  else {
    *value = mystrdup("Infinity");
  }
}
