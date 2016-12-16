/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_SQL_H_INCLUDED
# define YY_YY_SQL_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 7 "sql.y" /* yacc.c:1909  */

 
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
 

#line 53 "sql.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    COLLATE = 258,
    OR = 259,
    AND = 260,
    NOT = 261,
    COMPARISON = 262,
    UMINUS = 263,
    ALL = 264,
    ANY = 265,
    AS = 266,
    ASC = 267,
    AUTHORIZATION = 268,
    BETWEEN = 269,
    BY = 270,
    CASE = 271,
    CHARACTER = 272,
    CHECK = 273,
    CLOSE = 274,
    COMMIT = 275,
    COMMAND = 276,
    CONTINUE = 277,
    CONCAT = 278,
    CREATE = 279,
    CURRENT = 280,
    CURSOR = 281,
    DESIMAL = 282,
    DECLARE = 283,
    DEFAULT = 284,
    DELEET = 285,
    DESC = 286,
    DISTINCT = 287,
    DUBBLE = 288,
    ELSE = 289,
    END = 290,
    ERRER = 291,
    ESCAPE = 292,
    EXISTS = 293,
    FETCH = 294,
    FLOWT = 295,
    FOR = 296,
    FOREIGN = 297,
    FOUND = 298,
    FROM = 299,
    GOTO = 300,
    GRANT = 301,
    GROUP = 302,
    HAVING = 303,
    INN = 304,
    INDICATOR = 305,
    INSERT = 306,
    INTEGER = 307,
    INTO = 308,
    IS = 309,
    JOIN = 310,
    KEY = 311,
    LANGUAGE = 312,
    LEFT = 313,
    LIKE = 314,
    MODULE = 315,
    NULLX = 316,
    NUMERIC = 317,
    OF = 318,
    ON = 319,
    OPEN = 320,
    OPTION = 321,
    ORDER = 322,
    PARAMS = 323,
    PARAMETER = 324,
    PRECISION = 325,
    PRIMARY = 326,
    PRIVILEGES = 327,
    PROCEDURE = 328,
    PUBLIK = 329,
    REAL = 330,
    REFERENCES = 331,
    ROLLBACK = 332,
    SCHEMA = 333,
    SELECT = 334,
    SET = 335,
    SMALLINT = 336,
    SOME = 337,
    SQLCODE = 338,
    SQLERROR = 339,
    TABLE = 340,
    THEN = 341,
    TO = 342,
    UNION = 343,
    UNIQUE = 344,
    UPDATE = 345,
    USER = 346,
    VALUES = 347,
    VIEW = 348,
    WHEN = 349,
    WHENEVER = 350,
    WHERE = 351,
    WITH = 352,
    WORK = 353,
    COBOL = 354,
    FORTRAN = 355,
    PASKAL = 356,
    PLI = 357,
    C = 358,
    ADA = 359,
    ALLOCFAIL = 360,
    AMMSC = 361,
    NAME = 362,
    STRING = 363,
    INTNUM = 364,
    APPROXNUM = 365
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 26 "sql.y" /* yacc.c:1909  */

	/* double floatval; */
	long intval;
	char *strval;
  struct columnReference *referencePtr;
  struct expression *expressionPtr;
  struct atomEntry *atomPtr;

#line 185 "sql.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (struct qryData* queryData, yyscan_t scanner);

#endif /* !YY_YY_SQL_H_INCLUDED  */
