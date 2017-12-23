void yyerror2(long lineno, char *text) {
  MAC_YIELD

  fprintf(stderr, TDB_LEX_UNTERMINATED);
}
