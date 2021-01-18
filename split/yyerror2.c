void yyerror2(long lineno, char *text) {
  char lineNoBuf[12];
  MAC_YIELD

  myltoa(&lineNoBuf, lineno);

  fprintf(stderr, TDB_LEX_UNTERMINATED, (char *)(&lineNoBuf), text);
}
