%{
#include "querycsv.h"
#include "sql.h"
#include "yyinput.h"
	/* macro to save the text and return a token */
#define TOK(name) { return name; }
#define LEX_WORKAROUND strReplace("]]", "]", yytext)
%}
%option outfile="lexer.c" header-file="lexer.h"

%option warn nodefault

%option reentrant noyywrap never-interactive nounistd yylineno
%option bison-bridge

%s commands

%%

	/* literal keyword tokens */

(?i:ALL)		TOK(ALL)
(?i:AND)		TOK(AND)
(?i:AVG)		{ yylval->intval = GRP_AVG;return AMMSC; }
(?i:MIN)		{ yylval->intval = GRP_MIN;return AMMSC; }
(?i:MAX)		{ yylval->intval = GRP_MAX;return AMMSC; }
(?i:SUM)		{ yylval->intval = GRP_SUM;return AMMSC; }
(?i:COUNT)		{ yylval->intval = GRP_COUNT;return AMMSC; }
(?i:GROUP_CONCAT)		{ yylval->intval = GRP_CONCAT;return AMMSC; }
(?i:ANY)		TOK(ANY)
(?i:AS)			TOK(AS)
(?i:ASC)		TOK(ASC)
(?i:AUTHORIZATION)	TOK(AUTHORIZATION)
(?i:BETWEEN)		TOK(BETWEEN)
(?i:BY)			TOK(BY)
(?i:ENCODING)		TOK(ENCODING)
(?i:CHECK)		TOK(CHECK)
(?i:CLOSE)		TOK(CLOSE)
(?i:COLLATE)		TOK(COLLATE)
(?i:COMMAND)		{ BEGIN(commands); return COMMAND; }
<defname>(?i:COLUMNS)		{ BEGIN(INITIAL); return COLUMNS; }
<defname>(?i:NEXT)		{ BEGIN(INITIAL); return NEXT; }
<defname>(?i:VALUE)		{ BEGIN(INITIAL); return VALUE; }
<defname>(?i:DATE)		{ BEGIN(INITIAL); return DATE; }
<defname>[^ \t\r\n]		{ yyless(0); BEGIN(INITIAL); }

(?i:COMMIT)		TOK(COMMIT)
(?i:CONTINUE)		TOK(CONTINUE)
(?i:CONCAT)		TOK(CONCAT)
(?i:CREATE)		TOK(CREATE)
(?i:CURRENT)		TOK(CURRENT)
(?i:CURSOR)		TOK(CURSOR)
(?i:DECIMAL)		TOK(DESIMAL)
(?i:DECLARE)		TOK(DECLARE)
(?i:DEFAULT)		TOK(DEFAULT)
(?i:DELETE)		TOK(DELEET)
(?i:DESC)		TOK(DESC)
(?i:DISTINCT)		TOK(DISTINCT)
(?i:DOUBLE)		TOK(DUBBLE)
(?i:ESCAPE)		TOK(ESCAPE)
(?i:EXISTS)		TOK(EXISTS)
(?i:FETCH)		TOK(FETCH)
(?i:FLOAT)		TOK(FLOWT)
(?i:FOR)		TOK(FOR)
(?i:FOREIGN)		TOK(FOREIGN)
(?i:FOUND)		TOK(FOUND)
(?i:FROM)		TOK(FROM)
(?i:GO[ \t]*TO)		TOK(GOTO)
(?i:GRANT)		TOK(GRANT)
(?i:GROUP)		TOK(GROUP)
(?i:HAVING)		TOK(HAVING)
(?i:IN)			TOK(INN)
(?i:INDICATOR)		TOK(INDICATOR)
(?i:INSERT)		TOK(INSERT)
(?i:INT(EGER)?)		TOK(INTEGER)
(?i:INTO)		TOK(INTO)
(?i:IS)			TOK(IS)
(?i:JOIN)		TOK(JOIN)
(?i:KEY)		TOK(KEY)
(?i:LANGUAGE)		TOK(LANGUAGE)
(?i:LEFT)		TOK(LEFT)
(?i:LIKE)		TOK(LIKE)
(?i:NOT)		TOK(NOT)
(?i:NULL)		TOK(NULLX)
(?i:NUMERIC)		TOK(NUMERIC)
(?i:OF)			TOK(OF)
(?i:ON)			TOK(ON)
(?i:OPEN)		TOK(OPEN)
(?i:OPTION)		TOK(OPTION)
(?i:OR)			TOK(OR)
(?i:ORDER)		TOK(ORDER)
(?i:PARAMS)		TOK(PARAMS)
(?i:PRECISION)		TOK(PRECISION)
(?i:PRIMARY)		TOK(PRIMARY)
(?i:PRIVILEGES)		TOK(PRIVILEGES)
(?i:PROCEDURE)		TOK(PROCEDURE)
(?i:PUBLIC)		TOK(PUBLIK)
(?i:REAL)		TOK(REAL)
(?i:REFERENCES)		TOK(REFERENCES)
(?i:ROLLBACK)		TOK(ROLLBACK)
(?i:SCHEMA)		TOK(SCHEMA)
(?i:SELECT)		TOK(SELECT)
(?i:SMALLINT)		TOK(SMALLINT)
(?i:SOME)		TOK(SOME)
(?i:SQLCODE)		TOK(SQLCODE)
(?i:TABLE)		TOK(TABLE)
(?i:TO)			TOK(TO)
(?i:UNION)		TOK(UNION)
(?i:UNIQUE)		TOK(UNIQUE)
(?i:UPDATE)		TOK(UPDATE)
(?i:USER)		TOK(USER)
(?i:VALUES)		TOK(VALUES)
(?i:VIEW)		TOK(VIEW)
(?i:WHENEVER)		TOK(WHENEVER)
(?i:WHERE)		TOK(WHERE)
(?i:WITH)		TOK(WITH)
(?i:WORK)		TOK(WORK)

	/* punctuation */

"="     { yylval->intval = 0;return COMPARISON; }
"<>"    { yylval->intval = 1;return COMPARISON; }
"!="    { yylval->intval = 1;return COMPARISON; }
"<"     { yylval->intval = 2;return COMPARISON; }
">"     { yylval->intval = 3;return COMPARISON; }
"<="    { yylval->intval = 4;return COMPARISON; }
">="    { yylval->intval = 5;return COMPARISON; }

[-+*/(),.;]	TOK(yytext[0])

	/* names */

[A-Za-z][A-Za-z0-9_]*	{
    yylval->strval = NULL;
    d_sprintf(&(yylval->strval), "_%s", yytext);
    return NAME;
}

\"([^\"]|\"\")+\" {
    yytext[strlen(yytext)-1] = '\0';
    yylval->strval = strReplace("\"\"", "\"", yytext);
    return STRING;
}

\"([^\"]|\"\")*$	{	yyerror2(yylineno, yytext); }

\[([^\]]|\]\])+\] {
    yytext[0] = '_';
    yytext[strlen(yytext)-1] = '\0';
    yylval->strval = LEX_WORKAROUND;
    return NAME;
}

\[([^\]]|\]\])*$	{	yyerror2(yylineno, yytext); }


	/* numbers */

[0-9]+ {
    /*
    yylval->intval = atol(yytext);
    return INTNUM;
    */
    yylval->strval = strdup(yytext);
    return STRING;
}

[0-9]+"."[0-9]* |
"."[0-9]*	|
[0-9]+[eE][+-]?[0-9]+	|
[0-9]+"."[0-9]*[eE][+-]?[0-9]+ |
"."[0-9]*[eE][+-]?[0-9]+	{
    /*
    yylval->floatval = strtod(yytext, NULL);
    return APPROXNUM;
    */
    yylval->strval = strdup(yytext);
    return STRING;
}

	/* strings */

'([^']|'')*' {
    yytext[strlen(yytext)-1] = '\0';
    yylval->strval = strReplace("''", "'", yytext+1);
    return STRING;
}

'([^']|'')*$	{	yyerror2(yylineno, yytext); }

[ \t\r\n]+	/* white space */

"--"[^\n]*  /* comment */


.		/* random non-SQL text. trigger a syntax error */
%%
