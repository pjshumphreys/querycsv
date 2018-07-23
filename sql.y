%{
#include "querycsv.h"
#include "sql.h"
#include "lexer.h"
%}

%code requires {

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

}

%output  "sql.c"
%defines "sql.h"

%define api.pure
%define parse.error verbose
%define parse.lac full
%lex-param { yyscan_t scanner  }
%parse-param { struct qryData* queryData }
%parse-param { yyscan_t scanner }

%union {
  /* double floatval; */
  long intval;
  char *strval;
  struct columnReference *referencePtr;
  struct expression *expressionPtr;
  struct atomEntry *atomPtr;
}

/* operators */

%left COLLATE
%left OR
%left AND
%left NOT
%left <intval> COMPARISON /* = <> < > <= >= */
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

/* literal keyword tokens */

%token ALL AS ASC BY CASE
%token COMMAND CONCAT
%token DESC DISTINCT ELSE ENCODING
%token END FROM
%token GROUP HAVING IN INTO
%token IS JOIN LEFT LIKE NULLX ON
%token ORDER OPTIONS
%token SELECT
%token THEN
%token WHEN WHERE
%token COLUMNS NEXT VALUE DATE NOW

%token <intval> AMMSC
%token <strval> NAME STRING
%token <strval> INTNUM
%token <intval> APPROXNUM /* floatval*/
%type <strval> optional_as_name literal command_types
%type <referencePtr> column_ref
%type <intval> opt_asc_desc optional_encoding
%type <expressionPtr> scalar_exp search_condition predicate function_ref
%type <expressionPtr> comparison_predicate in_predicate join_condition where_clause
%type <atomPtr> atom_commalist
%%

main_file:
    ENCODING STRING ';' {
      if(queryData->inputEncoding == ENC_UNKNOWN) {
        if((queryData->inputEncoding = parse_encoding(queryData, $2)) == ENC_UNSUPPORTED) {
          YYABORT;
        }

        YYACCEPT;
      }
    } opt_params command_or_select
  | opt_params command_or_select;

command_or_select:
    COMMAND command_types opt_into_clause {
      runCommand(queryData, $2);

      YYACCEPT;
    }
  |
    SELECT
    scalar_exp_commalist
    opt_from_clause
    opt_where_clause
    opt_group_by_clause
    opt_having_clause
    opt_order_by_clause
    opt_into_clause
  ;

command_types:
    COLUMNS '(' STRING ')' {
      queryData->commandMode = 1;

      queryData->CMD_ENCODING = ENC_DEFAULT;

      $$ = $3;
    }
  | COLUMNS '(' STRING ',' STRING ')' {
      queryData->commandMode = 1;

      if((queryData->CMD_ENCODING = parse_encoding(queryData, $5)) == ENC_UNSUPPORTED) {
        YYABORT;
      }

      $$ = $3;
    }

  | NEXT '(' STRING ',' INTNUM ')' {
      queryData->commandMode = 2;

      queryData->CMD_OFFSET = atol($5);
      free($5);

      queryData->CMD_ENCODING = ENC_DEFAULT;

      $$ = $3;
    }
  | NEXT '(' STRING ',' STRING ',' INTNUM ')' {
      queryData->commandMode = 2;

      queryData->CMD_OFFSET = atol($7);
      free($7);

      if((queryData->CMD_ENCODING = parse_encoding(queryData, $5)) == ENC_UNSUPPORTED) {
        YYABORT;
      }

      $$ = $3;
    }
  | VALUE '(' STRING ',' INTNUM ',' INTNUM ')' {
      queryData->commandMode = 3;

      queryData->CMD_COLINDEX = atol($7);
      free($7);

      queryData->CMD_OFFSET = atol($5);
      free($5);

      queryData->CMD_ENCODING = ENC_DEFAULT;

      $$ = $3;
    }
  | VALUE '(' STRING ',' STRING ',' INTNUM ',' INTNUM ')' {
      queryData->commandMode = 3;

      queryData->CMD_COLINDEX = atol($9);
      free($9);

      queryData->CMD_OFFSET = atol($7);
      free($7);

      if((queryData->CMD_ENCODING = parse_encoding(queryData, $5)) == ENC_UNSUPPORTED) {
        YYABORT;
      }

      $$ = $3;
    }
  | DATE {
      queryData->commandMode = 4;

      $$ = NULL;
    }
  ;

opt_params:
  | OPTIONS STRING ';' {
      readParams(queryData, $2);
    }
  ;

scalar_exp_commalist:
    scalar_exp optional_as_name {
      parse_expCommaList(queryData, $1, $2, GRP_NONE);
    }
  | scalar_exp_commalist ',' scalar_exp optional_as_name {
      parse_expCommaList(queryData, $3, $4, GRP_NONE);
    }
  ;

optional_as_name:
    /* empty */ {
      $$ = NULL;
    }
  | AS NAME {
      if(queryData->parseMode != 1) {
        free($2);
        $$ = NULL;
      }
      else {
        $$ = $2;
      }
    }
  ;

scalar_exp:
    scalar_exp COLLATE NAME {
      if($1 != NULL) {
        /* TODO: Confirm this works correctly in all cases */
        $1->caseSensitive = (stricmp($3,"_sensitive") == 0);
      }

      free($3);
    }
  | scalar_exp '+' scalar_exp {
      $$ = parse_scalarExp(queryData, $1, EXP_PLUS, $3);
    }
  | scalar_exp '-' scalar_exp {
      $$ = parse_scalarExp(queryData, $1, EXP_MINUS, $3);
    }
  | scalar_exp '*' scalar_exp {
      $$ = parse_scalarExp(queryData, $1, EXP_MULTIPLY, $3);
    }
  | scalar_exp '/' scalar_exp {
      $$ = parse_scalarExp(queryData, $1, EXP_DIVIDE, $3);
    }
  | CONCAT '(' scalar_exp ',' scalar_exp ')' {
      $$ = parse_scalarExp(queryData, $3, EXP_CONCAT, $5);
    }
  | '+' scalar_exp %prec UMINUS {
      $$ = parse_scalarExp(queryData, $2, EXP_UPLUS, NULL);
    }
  | '-' scalar_exp %prec UMINUS {
      $$ = parse_scalarExp(queryData, $2, EXP_UMINUS, NULL);
    }
  | literal {
      $$ = parse_scalarExpLiteral(queryData, $1);
      free($1);
    }
  | NOW opt_now_brackets {
      if(queryData->dateString == NULL) {
        getCurrentDate(&(queryData->dateString));
      }

      $$ = parse_scalarExpLiteral(queryData, queryData->dateString);
    }
  | column_ref {
      $$ = parse_scalarExpColumnRef(queryData, $1);
    }
  | function_ref {
      $$ = $1;
    }
  | case_exp {
      $$ = parse_scalarExpLiteral(queryData, "");
    }
  | '(' scalar_exp ')' {
      $$ = $2;
    }
  ;

case_exp:
    simple_case
  | searched_case
  ;

simple_case:
    CASE scalar_exp
    simple_when_clause
    opt_case_else END
  ;

simple_when_clause:
    WHEN scalar_exp THEN result
  | simple_when_clause WHEN scalar_exp THEN result
  ;

opt_case_else:
    /* empty */
  | ELSE result
  ;

searched_case:
    CASE
    searched_when_clause
    opt_case_else END
  ;

searched_when_clause:
    WHEN search_condition THEN result
  | searched_when_clause WHEN search_condition THEN result
  ;

result:
    scalar_exp
  | NULLX
  ;

literal:
    STRING {
      $$ = $1;
    }
  | INTNUM {
      $$ = $1;
    }
  | APPROXNUM {
      $$ = NULL;
    }
  ;

column_ref:
    NAME {
      struct columnReference *retval = NULL;

      if(parse_columnRefUnsuccessful(queryData, &retval, NULL, $1)) {
        fprintf(stderr, "unknown or ambiguous column name (%s)\n", $1+1);
        free($1);
        YYERROR;
      }
      else {
        $$ = retval;
      }
    }
  | NAME '.' NAME {
      struct columnReference *retval = NULL;

      if(parse_columnRefUnsuccessful(queryData, &retval, $1, $3)) {
        fprintf(stderr, "unknown or ambiguous column name (%s.%s)\n", $1+1, $3+1);
        free($1);
        free($3);
        YYERROR;
      }
      else {
        $$ = retval;
      }
    }
  ;

function_ref:
    AMMSC '(' '*' ')' {
      $$ = parse_functionRefStar(queryData, $1);
    }
  | AMMSC '(' DISTINCT scalar_exp ')' {
      $$ = parse_functionRef(queryData, $1, $4, TRUE);
    }
  | AMMSC '(' ALL scalar_exp ')' {
      $$ = parse_functionRef(queryData, $1, $4, FALSE);
    }
  | AMMSC '(' scalar_exp ')' {
      $$ = parse_functionRef(queryData, $1, $3, FALSE);
    }
  ;

table_references:
    STRING AS NAME optional_encoding {
      parse_tableFactor(queryData, FALSE, $1, $3, $4);
    }
  | join_table
  ;

join_table:
    table_references JOIN STRING AS NAME optional_encoding {
      parse_tableFactor(queryData, FALSE, $3, $5, $6);
    } optional_join_condition
  | table_references LEFT JOIN STRING AS NAME optional_encoding {
      parse_tableFactor(queryData, TRUE, $4, $6, $7);
    } join_condition
  ;

optional_encoding:
    /* empty */ {
      $$ = ENC_DEFAULT;
    }
  | ENCODING STRING {
      if(($$ = parse_encoding(queryData, $2)) == ENC_UNSUPPORTED) {
        YYABORT;
      }
    }
  ;

optional_join_condition:
    /* empty */
  | join_condition
  ;

join_condition:
    ON search_condition {
      parse_whereClause(queryData, $2);
    }
  ;

opt_from_clause:
    /* empty */
  | FROM table_references
  ;

opt_where_clause:
    /* empty */
  | where_clause
  ;

where_clause:
    WHERE search_condition {
      parse_whereClause(queryData, $2);
    }
  ;

search_condition:
    search_condition AND search_condition {
      $$ = parse_scalarExp(queryData, $1, EXP_AND, $3);
    }
  | search_condition OR search_condition {
      $$ = parse_scalarExp(queryData, $1, EXP_OR, $3);
    }
  | NOT search_condition {
      $$ = parse_scalarExp(queryData, $2, EXP_NOT, NULL);
    }
  | '(' search_condition ')' {
      $$ = $2;
    }
  | predicate {
      $$ = $1;
    }
  ;

predicate:
    comparison_predicate {
      $$ = $1;
    }
  | in_predicate {
      $$ = $1;
    }
  | scalar_exp IS NOT NULLX {
      $$ = parse_scalarExp(queryData, $1, EXP_NOTNULL, NULL);
    }
  | scalar_exp IS NULLX {
      $$ = parse_scalarExp(queryData, $1, EXP_ISNULL, NULL);
    }
  ;

comparison_predicate:
    scalar_exp COMPARISON scalar_exp {
      $$ = parse_scalarExp(queryData, $1, $2+EXP_EQ, $3);
    }
  ;

in_predicate:
    scalar_exp IN '(' atom_commalist ')' {
      $$ = parse_inPredicate(queryData, $1, FALSE, $4);
    }
  | scalar_exp NOT IN '(' atom_commalist ')' {
      $$ = parse_inPredicate(queryData, $1, TRUE, $5);
    }
  ;

atom_commalist:
    literal {
      $$ = parse_atomCommaList(queryData, NULL, $1);
    }
  | atom_commalist ',' literal {
      $$ = parse_atomCommaList(queryData, $1, $3);
    }
  ;

opt_group_by_clause:
    /* empty */
  | GROUP BY column_ref_commalist
  ;

column_ref_commalist:
    column_ref {
      parse_groupingSpec(queryData, parse_scalarExpColumnRef(queryData, $1));
    }
  | column_ref_commalist ',' column_ref {
      parse_groupingSpec(queryData, parse_scalarExpColumnRef(queryData, $3));
    }
  ;

opt_now_brackets:
    /* empty */
  | '(' ')'
  ;

opt_having_clause:
    /* empty */
  | HAVING search_condition
  ;

opt_order_by_clause:
    /* empty */
  | ORDER BY ordering_spec
  ;

ordering_spec:
    scalar_exp opt_asc_desc {
      parse_orderingSpec(queryData, $1, $2);
    }
  | ordering_spec ',' scalar_exp opt_asc_desc {
      parse_orderingSpec(queryData, $3, $4);
    }
  ;

opt_asc_desc:
    /* empty */ {
      $$ = 0;
    }
  | ASC {
      $$ = 0;
    }
  | DESC {
      $$ = 1;
    }
  ;

opt_into_clause:
    /* empty */
  | INTO STRING optional_encoding {
      if(queryData->outputFileName == NULL) {
        queryData->outputFileName = $2;

        if($3 != ENC_DEFAULT) {
          queryData->outputEncoding = $3;
        }
      }
      else {
        free($2);
      }
    }
  ;
%%
