%{
#include "querycsv.h"
#include "sql.h"

#define YY_INPUT(buf,result,max_size) { \
  myyyinput(yyin, yyextra, buf, &(result), max_size); \
}

/* macro to save the text and return a token */
#define TOK(name) { return name; }
#define LEX_WORKAROUND strReplace("]]", "]", yytext)
%}
%option outfile="lexer.c" header-file="lexer.h"

%option warn nodefault

%option reentrant noyywrap never-interactive nounistd yylineno
%option bison-bridge

%s commands

U       [\x80-\xbf]
U2      [\xc2-\xdf]
U3      [\xe0-\xef]
U4      [\xf0-\xf4]

UANY    {U2}{U}|{U3}{U}{U}|{U4}{U}{U}{U}

%%

  /* literal keyword tokens */

(?i:ALL)    TOK(ALL)
(?i:AND)    TOK(AND)
(?i:AVG)    { yylval->intval = GRP_AVG;return AGGFN; }
(?i:MIN)    { yylval->intval = GRP_MIN;return AGGFN; }
(?i:MAX)    { yylval->intval = GRP_MAX;return AGGFN; }
(?i:SUM)    { yylval->intval = GRP_SUM;return AGGFN; }
(?i:CURRENT_TIMESTAMP)    { return NOW; }
(?i:COUNT)    { yylval->intval = GRP_COUNT;return AGGFN; }
(?i:GROUP_CONCAT)   { yylval->intval = GRP_CONCAT;return AGGFN; }
(?i:AS)     TOK(AS)
(?i:ASC)    TOK(ASC)
(?i:BY)     TOK(BY)
(?i:ENCODING)   TOK(ENCODING)
(?i:CASE)   TOK(CASE)
(?i:COLLATE)    TOK(COLLATE)
(?i:OUTPUT)    { TOK(OUTPUT) }
(?i:COLUMNS)    { TOK(COLUMNS) }
(?i:NEXT)   { TOK(NEXT) }
(?i:VALUE)    { TOK(VALUE) }
(?i:DATE)   { TOK(DATE) }
(?i:NOW)   { TOK(NOW) }

(?i:CONCAT)   TOK(CONCAT)
(?i:DESC)   TOK(DESC)
(?i:DISTINCT)   TOK(DISTINCT)
(?i:ELSE)   TOK(ELSE)
(?i:END)    TOK(END)
(?i:FROM)   TOK(FROM)
(?i:GROUP)    TOK(GROUP)
(?i:HAVING)   TOK(HAVING)
(?i:IN)     TOK(IN)
(?i:INTO)   TOK(INTO)
(?i:IS)     TOK(IS)
(?i:JOIN)   TOK(JOIN)
(?i:LEFT)   TOK(LEFT)
(?i:LIKE)   TOK(LIKE)
(?i:NOT)    TOK(NOT)
(?i:NULL)   TOK(NULLX)
(?i:ON)     TOK(ON)
(?i:OR)     TOK(OR)
(?i:ORDER)    TOK(ORDER)
(?i:SORT)   TOK(SORT)
(?i:OPTIONS)    TOK(OPTIONS)
(?i:SELECT)   TOK(SELECT)
(?i:THEN)   TOK(THEN)
(?i:WHEN)   TOK(WHEN)
(?i:WHERE)    TOK(WHERE)

  /* punctuation */

"="     { yylval->intval = 0;return COMPARISON; }
"<>"    { yylval->intval = 1;return COMPARISON; }
"!="    { yylval->intval = 1;return COMPARISON; }
"<"     { yylval->intval = 2;return COMPARISON; }
">"     { yylval->intval = 3;return COMPARISON; }
"<="    { yylval->intval = 4;return COMPARISON; }
">="    { yylval->intval = 5;return COMPARISON; }

[-+*/(),.;] TOK(yytext[0])

  /* names */

([A-Za-z]|{UANY})([A-Za-z0-9_]|{UANY})* {
    yylval->strval = NULL;
    d_sprintf(&(yylval->strval), "_" S_STRING, yytext);
    return NAME;
}

\[([^\]]|\]\])+\] {
    yytext[0] = '_';
    yytext[strlen(yytext)-1] = '\0';
    yylval->strval = LEX_WORKAROUND;
    return NAME;
}

\[([^\]]|\]\])*$  { yyerror2(yylineno, yytext); }


  /* numbers */

[0-9]+ {
    yylval->strval = mystrdup(yytext);
    return INTNUM;
}

[0-9]+"."[0-9]* |
"."[0-9]* |
[0-9]+[eE][+-]?[0-9]+ |
[0-9]+"."[0-9]*[eE][+-]?[0-9]+ |
"."[0-9]*[eE][+-]?[0-9]+  {
    yylval->strval = mystrdup(yytext);
    return STRING;
}

  /* strings */

'([^']|'')*' {
    yytext[strlen(yytext)-1] = '\0';
    yylval->strval = strReplace("''", "'", yytext+1);
    return STRING;
}

'([^']|'')*$  { yyerror2(yylineno, yytext); }

\"([^\"]|\"\")+\" {
    yytext[strlen(yytext)-1] = '\0';
    yylval->strval = strReplace("\"\"", "\"", yytext+1);
    return STRING;
}

\"([^\"]|\"\")*$  { yyerror2(yylineno, yytext); }


[ \t\r\n\205]+  /* white space */

"--"[^\n]*  /* comment */


.   /* random non-SQL text. trigger a syntax error */
%%
