#define YYMAXDEPTH 100
#include "build/sql2.h"

yytype_int16 yycheck2(int offset) {
  return yycheck[offset];
}

yytype_int16 yypact2(int offset) {
  return yypact[offset];
}
