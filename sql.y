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
  int intval;
  char *strval;
  struct columnReference *referencePtr;
  struct expression *expressionPtr;
  struct atomEntry *atomPtr;
  struct caseEntry *casePtr;
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
%token CONCAT SLICE ROWNUMBER
%token DESC DISTINCT ELSE ENCODING
%token END FROM
%token GROUP HAVING IN INTO
%token IS JOIN LEFT LIKE NULLX ON
%token ORDER PARTITION OVER OPTIONS
%token SELECT SORT
%token THEN
%token WHEN WHERE
%token COLUMNS NEXT VALUE DATE OUTPUT NOW

%token <intval> AGGFN
%token <strval> NAME STRING
%token <strval> INTNUM
%token <intval> APPROXNUM /* floatval*/
%type <strval> optional_as_name literal optional_separator
%type <referencePtr> column_ref
%type <intval> opt_asc_desc optional_encoding from_options
%type <expressionPtr> case_exp simple_case searched_case opt_over column_ref_commalist2
%type <expressionPtr> scalar_exp optional_exp search_condition predicate function_ref
%type <expressionPtr> comparison_predicate in_predicate join_condition where_clause
%type <atomPtr> atom_commalist
%type <casePtr> when_clause when_clause2
%%

main_file:
    ENCODING STRING ';' {
      if(queryData->inputEncoding == ENC_UNKNOWN) {
        if((queryData->inputEncoding = parse_encoding($2)) == ENC_UNSUPPORTED) {
          YYABORT;
        }

        YYACCEPT;
      }
    } opt_params command_or_select
  | opt_params command_or_select;

command_or_select:
    SELECT
    scalar_exp_commalist
    opt_from_clause
    opt_where_clause
    opt_order_by_clause
    opt_group_by_clause
    opt_having_clause
    opt_into_clause
  | COLUMNS STRING optional_encoding opt_into_clause {
      queryData->commandMode = 1;
      queryData->inputFileName = $2;
      queryData->CMD_ENCODING = $3;

      YYACCEPT;
    }
  | OUTPUT STRING optional_encoding opt_into_clause {
      queryData->commandMode = 5;
      queryData->inputFileName = $2;
      queryData->CMD_ENCODING = $3;

      YYACCEPT;
    }
  | NEXT STRING optional_encoding INTNUM opt_into_clause {
      queryData->commandMode = 2;
      queryData->inputFileName = $2;
      queryData->CMD_ENCODING = $3;

      queryData->CMD_OFFSET = atol($4);
      free($4);

      YYACCEPT;
    }
  | VALUE STRING optional_encoding INTNUM INTNUM opt_into_clause {
      queryData->commandMode = 3;
      queryData->inputFileName = $2;
      queryData->CMD_ENCODING = $3;

      queryData->CMD_OFFSET = atol($4);
      free($4);

      queryData->CMD_COLINDEX = atol($5);
      free($5);

      YYACCEPT;
    }
  | DATE opt_into_clause {
      queryData->commandMode = 4;

      YYACCEPT;
    }
  ;

opt_params:
  | OPTIONS STRING ';' {
      queryData->params = parse_params(queryData, $2, queryData->params);
    }
  ;

into_options:
  | OPTIONS STRING {
      queryData->params = parse_params(queryData, $2, queryData->params);
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

optional_exp:
    /* empty */ {
      $$ = parse_scalarExpLiteral(queryData, NULL);
    }
  | ',' scalar_exp {
      $$ = $2;
    }
  ;

scalar_exp:
    scalar_exp COLLATE STRING {
      if($1 != NULL) {
        $1->caseSensitive = parse_collate($3);
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
  | ROWNUMBER opt_now_brackets opt_over {
    $$ = parse_functionRef(queryData, GRP_ROWNUMBER, $3, FALSE, NULL);
  }
  | SLICE '(' scalar_exp ',' scalar_exp optional_exp ')' {
      $$ = parse_scalarExp(queryData, $3, EXP_SLICE, parse_scalarExp(queryData, $5, EXP_LIMITS, $6));
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
  | NULLX {
      $$ = parse_scalarExpLiteral(queryData, NULL);
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
      $$ = $1;
    }
  | '(' scalar_exp ')' {
      $$ = $2;
    }
  ;

opt_over:
    /* empty */ {
      $$ = parse_scalarExp(queryData, NULL, EXP_ROWNUMBER, NULL);
    }
  | OVER '(' PARTITION BY column_ref_commalist2 ')' {
      $$ = parse_columnRefList(queryData, $5, NULL);
    }
  ;

case_exp:
    simple_case { $$ = $1; }
  | searched_case { $$ = $1; }
  ;

simple_case:
    CASE scalar_exp
    when_clause
    END { $$ = parse_case(queryData, $2, $3, NULL); }
  | CASE scalar_exp
    when_clause
    ELSE scalar_exp END { $$ = parse_case(queryData, $2, $3, $5); }
  ;

searched_case:
    CASE
    when_clause2
    END { $$ = parse_case(queryData, NULL, $2, NULL); }
  | CASE
    when_clause2
    ELSE scalar_exp END { $$ = parse_case(queryData, NULL, $2, $4); }
  ;

when_clause:
    WHEN scalar_exp THEN scalar_exp { $$ = parse_when(queryData, NULL, $2, $4); }
  | when_clause WHEN scalar_exp THEN scalar_exp { $$ = parse_when(queryData, $1, $3, $5); }
  ;

when_clause2:
    WHEN search_condition THEN scalar_exp { $$ = parse_when(queryData, NULL, $2, $4); }
  | when_clause WHEN search_condition THEN scalar_exp { $$ = parse_when(queryData, $1, $3, $5); }
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
        fprintf(stderr, TDB_NAME, $1+1);
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
        fprintf(stderr, TDB_NAME_NAME, $1+1, $3+1);
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
    AGGFN '(' '*' ')' {
      $$ = parse_functionRefStar(queryData, $1);
    }
  | AGGFN '(' DISTINCT scalar_exp optional_separator ')' {
     $$ = parse_functionRef(queryData, $1, $4, TRUE, $5);
    }
  | AGGFN '(' ALL scalar_exp optional_separator ')' {
      $$ = parse_functionRef(queryData, $1, $4, FALSE, $5);
    }
  | AGGFN '(' scalar_exp optional_separator ')' {
      $$ = parse_functionRef(queryData, $1, $3, FALSE, $4);
    }
  ;

optional_separator:
    {
    $$ = NULL;
  }
  | ',' STRING {
      $$ = queryData->parseMode != 1 ? NULL : mystrdup($2);
    }
  ;

table_references:
    STRING AS NAME from_options optional_encoding {
      parse_tableFactor(queryData, FALSE, $1, $3, $4, $5);
    }
  | STRING from_options optional_encoding {
      parse_tableFactor(queryData, FALSE, $1, mystrdup("_table"), $2, $3);
    }
  | join_table
  ;

join_table:
    table_references JOIN STRING AS NAME from_options optional_encoding {
      parse_tableFactor(queryData, FALSE, $3, $5, $6, $7);
    } optional_join_condition
  | table_references LEFT JOIN STRING AS NAME from_options optional_encoding {
      parse_tableFactor(queryData, TRUE, $4, $6, $7, $8);
    } join_condition
  ;

from_options:
    /* empty */ { $$ = PRM_DEFAULT; }
  | OPTIONS STRING {
      $$ = parse_params(queryData, $2, PRM_DEFAULT);
    }
  ;

optional_encoding:
    /* empty */ {
      $$ = ENC_DEFAULT;
    }
  | ENCODING STRING {
      if(($$ = parse_encoding($2)) == ENC_UNSUPPORTED) {
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
  | GROUP BY column_ref_commalist opt_order_by_clause
  ;

column_ref_commalist:
    column_ref {
      parse_groupingSpec(queryData, parse_scalarExpColumnRef(queryData, $1));
    }
  | column_ref_commalist ',' column_ref {
      parse_groupingSpec(queryData, parse_scalarExpColumnRef(queryData, $3));
    }
  ;

column_ref_commalist2:
    column_ref {
      $$ = parse_scalarExpColumnRef(queryData, $1);
    }
  | column_ref_commalist2 ',' column_ref {
      $$ = parse_columnRefList(queryData, $1, $3);
    }
  ;

opt_now_brackets:
    /* empty */
  | '(' ')'
  ;

opt_having_clause:
    /* empty */
  | HAVING search_condition {
    parse_whereClause(queryData, $2);
  } opt_order_by_clause
  ;

opt_order_by_clause:
    /* empty */
  | ORDER BY ordering_spec
  | SORT BY ordering_spec
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
  | INTO STRING into_options optional_encoding {
      if(queryData->outputFileName == NULL) {
        queryData->outputFileName = $2;

        if($4 != ENC_DEFAULT) {
          queryData->outputEncoding = $4;
        }
      }
      else {
        free($2);
      }
    }
  ;
%%
