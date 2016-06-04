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
#line 8 "sql.y" /* yacc.c:1909  */

 
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
    CONTINUE = 276,
    CONCAT = 277,
    CREATE = 278,
    CURRENT = 279,
    CURSOR = 280,
    DESIMAL = 281,
    DECLARE = 282,
    DEFAULT = 283,
    DELEET = 284,
    DESC = 285,
    DISTINCT = 286,
    DUBBLE = 287,
    ELSE = 288,
    END = 289,
    ERRER = 290,
    ESCAPE = 291,
    EXISTS = 292,
    FETCH = 293,
    FLOWT = 294,
    FOR = 295,
    FOREIGN = 296,
    FOUND = 297,
    FROM = 298,
    GOTO = 299,
    GRANT = 300,
    GROUP = 301,
    HAVING = 302,
    INN = 303,
    INDICATOR = 304,
    INSERT = 305,
    INTEGER = 306,
    INTO = 307,
    IS = 308,
    JOIN = 309,
    KEY = 310,
    LANGUAGE = 311,
    LEFT = 312,
    LIKE = 313,
    MODULE = 314,
    NULLX = 315,
    NUMERIC = 316,
    OF = 317,
    ON = 318,
    OPEN = 319,
    OPTION = 320,
    ORDER = 321,
    PARAMETER = 322,
    PRECISION = 323,
    PRIMARY = 324,
    PRIVILEGES = 325,
    PROCEDURE = 326,
    PUBLIC = 327,
    REAL = 328,
    REFERENCES = 329,
    ROLLBACK = 330,
    SCHEMA = 331,
    SELECT = 332,
    SET = 333,
    SMALLINT = 334,
    SOME = 335,
    SQLCODE = 336,
    SQLERROR = 337,
    TABLE = 338,
    THEN = 339,
    TO = 340,
    UNION = 341,
    UNIQUE = 342,
    UPDATE = 343,
    USER = 344,
    VALUES = 345,
    VIEW = 346,
    WHEN = 347,
    WHENEVER = 348,
    WHERE = 349,
    WITH = 350,
    WORK = 351,
    COBOL = 352,
    FORTRAN = 353,
    PASKAL = 354,
    PLI = 355,
    C = 356,
    ADA = 357,
    ALLOCFAIL = 358,
    AMMSC = 359,
    NAME = 360,
    STRING = 361,
    INTNUM = 362,
    APPROXNUM = 363
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 27 "sql.y" /* yacc.c:1909  */

	//double floatval;
	long intval;
	char * strval;
  struct columnReference * referencePtr;
  struct expression* expressionPtr;
  struct atomEntry* atomPtr;

#line 183 "sql.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (struct qryData* queryData, yyscan_t scanner);

#endif /* !YY_YY_SQL_H_INCLUDED  */
