#define YYMAXDEPTH 100
#define YY_LAC_ESTABLISH                                         \
do {                                                             \
  if (!yy_lac_established)                                       \
    {                                                            \
      YYDPRINTF ((stderr,                                        \
                  "LAC: initial context established for %s\n",   \
                  yytname[yytoken]));                            \
      yy_lac_established = 1;                                    \
      {                                                          \
        int yy_lac_status =                                      \
          yy_lac (yyesa, &yyes, &yyes_capacity, yyssp, yytoken); \
        switch(yy_lac_status) {                                  \
          case 1:                                                \
            goto yyerrlab;                                       \
          case 2:                                                \
            goto yyexhaustedlab;                                 \
        }                                                        \
      }                                                          \
    }                                                            \
} while (0)

#include "build/sql2.h"

yytype_int16 yycheck2(int offset) {
  return yycheck[offset];
}

yytype_int16 yypact2(int offset) {
  return yypact[offset];
}

yytype_int8 yyr1a(int offset) {
  return yyr1[offset];
}

yytype_int8 yydefact2(int offset) {
  return yydefact[offset];
}
