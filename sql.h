/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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
    OR = 258,
    AND = 259,
    NOT = 260,
    COMPARISON = 261,
    UMINUS = 262,
    ALL = 263,
    ANY = 264,
    AS = 265,
    ASC = 266,
    AUTHORIZATION = 267,
    BETWEEN = 268,
    BY = 269,
    CASE = 270,
    CHARACTER = 271,
    CHECK = 272,
    CLOSE = 273,
    COMMIT = 274,
    CONTINUE = 275,
    CONCAT = 276,
    CREATE = 277,
    CURRENT = 278,
    CURSOR = 279,
    DECIMAL = 280,
    DECLARE = 281,
    DEFAULT = 282,
    DELETE = 283,
    DESC = 284,
    DISTINCT = 285,
    DOUBLE = 286,
    ELSE = 287,
    END = 288,
    ERROR = 289,
    ESCAPE = 290,
    EXISTS = 291,
    FETCH = 292,
    FLOAT = 293,
    FOR = 294,
    FOREIGN = 295,
    FOUND = 296,
    FROM = 297,
    GOTO = 298,
    GRANT = 299,
    GROUP = 300,
    HAVING = 301,
    IN = 302,
    INDICATOR = 303,
    INSERT = 304,
    INTEGER = 305,
    INTO = 306,
    IS = 307,
    JOIN = 308,
    KEY = 309,
    LANGUAGE = 310,
    LEFT = 311,
    LIKE = 312,
    MODULE = 313,
    NULLX = 314,
    NUMERIC = 315,
    OF = 316,
    ON = 317,
    OPEN = 318,
    OPTION = 319,
    ORDER = 320,
    PARAMETER = 321,
    PRECISION = 322,
    PRIMARY = 323,
    PRIVILEGES = 324,
    PROCEDURE = 325,
    PUBLIC = 326,
    REAL = 327,
    REFERENCES = 328,
    ROLLBACK = 329,
    SCHEMA = 330,
    SELECT = 331,
    SET = 332,
    SMALLINT = 333,
    SOME = 334,
    SQLCODE = 335,
    SQLERROR = 336,
    TABLE = 337,
    THEN = 338,
    TO = 339,
    UNION = 340,
    UNIQUE = 341,
    UPDATE = 342,
    USER = 343,
    VALUES = 344,
    VIEW = 345,
    WHEN = 346,
    WHENEVER = 347,
    WHERE = 348,
    WITH = 349,
    WORK = 350,
    COBOL = 351,
    FORTRAN = 352,
    PASCAL = 353,
    PLI = 354,
    C = 355,
    ADA = 356,
    ALLOCFAIL = 357,
    AMMSC = 358,
    NAME = 359,
    STRING = 360,
    INTNUM = 361,
    APPROXNUM = 362
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 27 "sql.y" /* yacc.c:1909  */

	//double floatval;
	long intval;
	char * strval;
  struct columnReference * referencePtr;
  struct expression* expressionPtr;
  struct atomEntry* atomPtr;

#line 182 "sql.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (struct qryData* queryData, yyscan_t scanner);

#endif /* !YY_YY_SQL_H_INCLUDED  */
