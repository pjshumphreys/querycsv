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

%token ALL ANY AS ASC AUTHORIZATION BETWEEN BY CASE
%token CHARACTER CHECK CLOSE COMMIT COMMAND CONTINUE CONCAT CREATE CURRENT
%token CURSOR DESIMAL DECLARE DEFAULT DELEET DESC DISTINCT DUBBLE ELSE END
%token ERRER ESCAPE EXISTS FETCH FLOWT FOR FOREIGN FOUND FROM GOTO
%token GRANT GROUP HAVING INN INDICATOR INSERT INTEGER INTO
%token IS JOIN KEY LANGUAGE LEFT LIKE MODULE NULLX NUMERIC OF ON
%token OPEN OPTION ORDER PARAMS PARAMETER PRECISION PRIMARY PRIVILEGES PROCEDURE
%token PUBLIK REAL REFERENCES ROLLBACK SCHEMA SELECT SET
%token SMALLINT SOME SQLCODE SQLERROR TABLE THEN TO UNION
%token UNIQUE UPDATE USER VALUES VIEW WHEN WHENEVER WHERE WITH WORK
%token COBOL FORTRAN PASKAL PLI C ADA ALLOCFAIL

%token <intval> AMMSC
%token <strval> NAME STRING
%token <intval> INTNUM
%token <intval> APPROXNUM /* floatval*/
%type <strval> optional_as_name literal
%type <referencePtr> column_ref
%type <intval> opt_asc_desc
%type <expressionPtr> scalar_exp search_condition predicate function_ref
%type <expressionPtr> comparison_predicate in_predicate join_condition where_clause
%type <atomPtr> atom_commalist
%%

command_or_select:
    COMMAND STRING ';' {
    if(queryData->parseMode != 1) {
      runCommand($2);
    }
  }
  | opt_params
    SELECT
    scalar_exp_commalist
    FROM table_references
    opt_where_clause
    opt_group_by_clause
    opt_having_clause
    opt_order_by_clause
    opt_into_clause
  ;

opt_params:
  | PARAMS STRING ';' {
    if(queryData->parseMode != 1) {
      readParams($2, &(queryData->params));
    }
  }

scalar_exp_commalist:
		scalar_exp optional_as_name { parse_expCommaList(queryData, $1, $2, GRP_NONE); }
	|	scalar_exp_commalist ',' scalar_exp optional_as_name {parse_expCommaList(queryData, $3, $4, GRP_NONE); }
	;

optional_as_name: { $$ = NULL; }
  | AS NAME { if(queryData->parseMode != 1) { free($2); $$ = NULL; } else {$$ = $2;} }
  ;

scalar_exp:
    scalar_exp COLLATE NAME {
      if($1 != NULL) {
        $1->caseSensitive = (stricmp($3,"_sensitive") == 0);
      }
      
      free($3);
  }
  | scalar_exp '+' scalar_exp { $$ = parse_scalarExp(queryData, $1, EXP_PLUS, $3); }
	|	scalar_exp '-' scalar_exp { $$ = parse_scalarExp(queryData, $1, EXP_MINUS, $3); }
	|	scalar_exp '*' scalar_exp { $$ = parse_scalarExp(queryData, $1, EXP_MULTIPLY, $3); }
	|	scalar_exp '/' scalar_exp { $$ = parse_scalarExp(queryData, $1, EXP_DIVIDE, $3); }
  | CONCAT '(' scalar_exp ',' scalar_exp ')' { $$ = parse_scalarExp(queryData, $3, EXP_CONCAT, $5); }
	|	'+' scalar_exp %prec UMINUS { $$ = parse_scalarExp(queryData, $2, EXP_UPLUS, NULL); }
	|	'-' scalar_exp %prec UMINUS { $$ = parse_scalarExp(queryData, $2, EXP_UMINUS, NULL); }
	|	literal { $$ = parse_scalarExpLiteral(queryData, $1); free($1); }
	|	column_ref { $$ = parse_scalarExpColumnRef(queryData, $1); }
	|	function_ref { $$ = $1; }
	|	'(' scalar_exp ')' { $$ = $2; }
	;

literal:
		STRING { $$ = $1; }
	|	INTNUM { $$ = NULL; }
	|	APPROXNUM { $$ = NULL; }
	;

column_ref:
		NAME {
      if(parse_columnRefUnsuccessful(queryData, &($$), NULL, $1)) {
        fputs("unknown or ambiguous column name\n", stderr);
        YYERROR;
      }
    }
	|	NAME '.' NAME {
      if(parse_columnRefUnsuccessful(queryData, &($$), $1, $3)) {
        fputs("unknown or ambiguous column name\n", stderr);
        YYERROR;
      }
    }
	;

function_ref:
		AMMSC '(' '*' ')' { $$ = parse_functionRefStar(queryData, $1); }
	|	AMMSC '(' DISTINCT scalar_exp ')' { $$ = parse_functionRef(queryData, $1, $4, TRUE); }
	|	AMMSC '(' ALL scalar_exp ')' { $$ = parse_functionRef(queryData, $1, $4, FALSE); }
	|	AMMSC '(' scalar_exp ')' { $$ = parse_functionRef(queryData, $1, $3, FALSE); }
	;

table_references:
    STRING AS NAME { parse_tableFactor(queryData, FALSE, $1, $3); }
  | join_table
  ;
  
join_table:
    table_references JOIN STRING AS NAME { parse_tableFactor(queryData, FALSE, $3, $5); } optional_join_condition
  | table_references LEFT JOIN STRING AS NAME { parse_tableFactor(queryData, TRUE, $4, $6); } join_condition
  ;
  
optional_join_condition:
		/* empty */
  | join_condition
  ;
  
join_condition:
   ON search_condition { parse_whereClause(queryData, $2); }
  ;

opt_where_clause:
		/* empty */
	|	where_clause
	;
  
where_clause:
		WHERE search_condition { parse_whereClause(queryData, $2); }
	;

search_condition:
		search_condition AND search_condition { $$ = parse_scalarExp(queryData, $1, EXP_AND, $3); }
	|	search_condition OR search_condition { $$ = parse_scalarExp(queryData, $1, EXP_OR, $3); }
	|	NOT search_condition { $$ = parse_scalarExp(queryData, $2, EXP_NOT, NULL); }
	|	'(' search_condition ')' { $$ = $2; }
	|	predicate { $$ = $1; }
	;

predicate:
		comparison_predicate { $$ = $1; }
	|	in_predicate { $$ = $1; }
	;

comparison_predicate:
		scalar_exp COMPARISON scalar_exp { $$ = parse_scalarExp(queryData, $1, $2+EXP_EQ, $3); }
	;

in_predicate:
		scalar_exp INN '(' atom_commalist ')' { $$ = parse_inPredicate(queryData, $1, FALSE, $4); }
	|	scalar_exp NOT INN '(' atom_commalist ')' { $$ = parse_inPredicate(queryData, $1, TRUE, $5); }
	;

atom_commalist:
		literal { $$ = parse_atomCommaList(queryData, NULL, $1); }
	|	atom_commalist ',' literal { $$ = parse_atomCommaList(queryData, $1, $3); }
	;

opt_group_by_clause:
		/* empty */
	|	GROUP BY column_ref_commalist { queryData->hasGrouping = TRUE; }
	;

column_ref_commalist:
		column_ref { parse_groupingSpec(queryData, parse_scalarExpColumnRef(queryData, $1)); }
	|	column_ref_commalist ',' column_ref { parse_groupingSpec(queryData, parse_scalarExpColumnRef(queryData, $3)); }
	;

opt_having_clause:
		/* empty */
	|	HAVING search_condition
	;

opt_order_by_clause:
		/* empty */
	|	ORDER BY ordering_spec
	;

ordering_spec:
		scalar_exp opt_asc_desc { parse_orderingSpec(queryData, $1, $2); }
	|	ordering_spec ',' scalar_exp opt_asc_desc { parse_orderingSpec(queryData, $3, $4); }
	;

opt_asc_desc:
		/* empty */ {$$ = 0;}
	|	ASC {$$ = 0;}
	|	DESC {$$ = 1;}
	;

opt_into_clause:
    /* empty */
  | INTO STRING { if(queryData->parseMode != 1) {free($2);} else {queryData->intoFileName = $2;} }
  ;
%%
