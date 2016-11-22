#include "querycsv.h"

void exp_uminus(char **value, double leftVal) {
  double temp;

  MAC_YIELD

  temp = ctof(0);

  if(fcmp(leftVal, temp)) {
    temp = fneg(leftVal);

    ftostr(value, temp);
  }
  else {
    *value = strdup("0");
  }
}
