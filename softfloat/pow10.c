#include <stdlib.h>
#include "milieu.h"
#include "macros.h"
#include "softfloat.h"
double fltMinusOne;
double fltZero;
double fltOne;
double fltHalf;
double fltFour;
double fltTen;
double fltSmall;
int fltNotInited = 1;

double
two54,
ivln10hi,
ivln10lo,
log10_2hi,
log10_2lo,
Lg1,
Lg2,
Lg3,
Lg4,
Lg5,
Lg6,
Lg7;

double pow10a(int exp) {
  int sign = 0;
  double result = fltTen;

  if(exp == 0) {
    return fltOne;
  }

  if(exp < 0) {
    sign = 1;
    exp = abs(exp);
  }

  while(--exp) {
    result = fmul(fltTen, result);
  }

  if(sign) {
    result = fdiv(fltOne,result);
  }

  return result;
}

void initfp(void) {
  fltMinusOne = ctof(-1);
  fltZero = ctof(0);
  fltOne = ctof(1);
  fltHalf = fdiv(fltOne, ctof(2));
  fltTen = ctof(10);
  fltSmall = pow10a(-309);

  two54.high = 0x43500000, two54.low = 0x00000000,
  ivln10hi.high = 0x3fdbcb7b, ivln10hi.low = 0x15200000,
  ivln10lo.high = 0x3dbb9438, ivln10lo.low = 0xca9aadd5,
  log10_2hi.high = 0x3FD34413, log10_2hi.low = 0x509F6000,
  log10_2lo.high = 0x3D59FEF3, log10_2lo.low = 0x11F12B36,
  Lg1.high = 0x3FE55555, Lg1.low = 0x55555593,
  Lg2.high = 0x3FD99999, Lg2.low = 0x9997FA04,
  Lg3.high = 0x3FD24924, Lg3.low = 0x94229359,
  Lg4.high = 0x3FCC71C5, Lg4.low = 0x1D8E78AF,
  Lg5.high = 0x3FC74664, Lg5.low = 0x96CB03DE,
  Lg6.high = 0x3FC39A09, Lg6.low = 0xD078C69F,
  Lg7.high = 0x3FC2F112, Lg7.low = 0xDF3E5244;

  fltNotInited = 0;
}
