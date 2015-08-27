/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "sql.y" /* yacc.c:339  */

#include "querycsv.h"
#include "sql.h"
#include "lexer.h"
#include "parser.c"

#line 73 "sql.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "sql.h".  */
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
#line 8 "sql.y" /* yacc.c:355  */

 
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
 

#line 112 "sql.c" /* yacc.c:355  */

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
#line 27 "sql.y" /* yacc.c:355  */

	//double floatval;
	long intval;
	char * strval;
  struct columnReference * referencePtr;
  struct expression* expressionPtr;
  struct atomEntry* atomPtr;

#line 241 "sql.c" /* yacc.c:355  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (struct qryData* queryData, yyscan_t scanner);

#endif /* !YY_YY_SQL_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 255 "sql.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
# define YYCOPY_NEEDED 1
#endif


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  17
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   181

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  116
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  28
/* YYNRULES -- Number of rules.  */
#define YYNRULES  65
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  132

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   362

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     113,   114,     9,     7,   112,     8,   115,    10,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    73,    73,    84,    85,    88,    89,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   107,   108,
     109,   113,   119,   128,   129,   130,   131,   135,   136,   140,
     140,   141,   141,   144,   146,   150,   153,   155,   159,   163,
     164,   165,   166,   167,   171,   172,   176,   180,   181,   185,
     186,   189,   191,   195,   196,   199,   201,   204,   206,   210,
     211,   215,   216,   217,   220,   222
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OR", "AND", "NOT", "COMPARISON", "'+'",
  "'-'", "'*'", "'/'", "UMINUS", "ALL", "ANY", "AS", "ASC",
  "AUTHORIZATION", "BETWEEN", "BY", "CASE", "CHARACTER", "CHECK", "CLOSE",
  "COMMIT", "CONTINUE", "CONCAT", "CREATE", "CURRENT", "CURSOR", "DECIMAL",
  "DECLARE", "DEFAULT", "DELETE", "DESC", "DISTINCT", "DOUBLE", "ELSE",
  "END", "ERROR", "ESCAPE", "EXISTS", "FETCH", "FLOAT", "FOR", "FOREIGN",
  "FOUND", "FROM", "GOTO", "GRANT", "GROUP", "HAVING", "IN", "INDICATOR",
  "INSERT", "INTEGER", "INTO", "IS", "JOIN", "KEY", "LANGUAGE", "LEFT",
  "LIKE", "MODULE", "NULLX", "NUMERIC", "OF", "ON", "OPEN", "OPTION",
  "ORDER", "PARAMETER", "PRECISION", "PRIMARY", "PRIVILEGES", "PROCEDURE",
  "PUBLIC", "REAL", "REFERENCES", "ROLLBACK", "SCHEMA", "SELECT", "SET",
  "SMALLINT", "SOME", "SQLCODE", "SQLERROR", "TABLE", "THEN", "TO",
  "UNION", "UNIQUE", "UPDATE", "USER", "VALUES", "VIEW", "WHEN",
  "WHENEVER", "WHERE", "WITH", "WORK", "COBOL", "FORTRAN", "PASCAL", "PLI",
  "C", "ADA", "ALLOCFAIL", "AMMSC", "NAME", "STRING", "INTNUM",
  "APPROXNUM", "','", "'('", "')'", "'.'", "$accept", "select_stmt",
  "scalar_exp_commalist", "optional_as_name", "scalar_exp", "literal",
  "column_ref", "function_ref", "table_references", "join_table", "$@1",
  "$@2", "optional_join_condition", "join_condition", "opt_where_clause",
  "where_clause", "search_condition", "predicate", "comparison_predicate",
  "in_predicate", "atom_commalist", "opt_group_by_clause",
  "column_ref_commalist", "opt_having_clause", "opt_order_by_clause",
  "ordering_spec", "opt_asc_desc", "opt_into_clause", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,    43,    45,    42,
      47,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   332,   333,   334,   335,   336,   337,   338,   339,   340,
     341,   342,   343,   344,   345,   346,   347,   348,   349,   350,
     351,   352,   353,   354,   355,   356,   357,   358,   359,   360,
     361,   362,    44,    40,    41,    46
};
# endif

#define YYPACT_NINF -97

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-97)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -74,    67,     9,    67,    67,   -96,   -65,   -45,   -97,   -97,
     -97,    67,   -39,    79,   -97,   -97,   -97,   -97,   -97,   -97,
      67,    60,   -37,    28,   -19,    67,    67,    67,    67,    67,
     -17,   -97,    69,   -13,    67,    67,    32,   -97,   -97,    91,
       5,   -97,    79,    54,    54,   -97,   -97,   -97,    67,   -97,
      36,    43,   -97,    -2,     0,    57,    24,    59,   -97,   -97,
      51,   -97,   -97,   -97,   102,     8,    24,    24,    90,    80,
     -97,   -97,   -97,   101,    73,   -97,    17,   112,   -97,     6,
       1,    76,    67,    16,    24,    24,    22,    24,    70,   -97,
      30,   -97,    27,   144,   -55,   139,   -97,   -97,    33,    80,
     126,    92,    83,   -97,   -55,   -97,   -91,    22,    67,    46,
     -97,    24,   -97,   -97,    83,   -32,   -55,   -97,   -97,   103,
      44,   -97,    80,   -97,   -97,   -97,   -97,   -97,   -97,    67,
     103,   -97
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,    21,    18,    19,
      20,     0,     0,     5,    14,    15,    16,     1,    12,    13,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     3,     0,     0,     0,     0,     0,    22,    17,     0,
      36,    28,     5,     7,     8,     9,    10,     6,     0,    23,
       0,     0,    26,     0,     0,     0,     0,    51,    37,     4,
       0,    25,    24,    27,     0,     0,     0,     0,     0,    38,
      43,    44,    45,     0,    55,    11,     0,     0,    41,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    57,    29,
       0,    42,     0,    46,     0,    40,    39,    53,    52,    56,
       0,    64,    33,    31,     0,    49,     0,     0,     0,     0,
       2,     0,    30,    34,     0,     0,     0,    47,    54,    61,
      58,    65,    35,    32,    48,    50,    62,    63,    59,     0,
      61,    60
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -97,   -97,   -97,   116,    -1,   -86,   -85,   -97,   -97,   -97,
     -97,   -97,   -97,    45,   -97,   -97,    37,   -97,   -97,   -97,
      56,   -97,   -97,   -97,   -97,   -97,    31,   -97
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,    12,    31,    68,    14,    15,    16,    40,    41,
     102,   114,   112,   113,    57,    58,    69,    70,    71,    72,
     106,    74,    98,    88,   101,   120,   128,   110
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      13,    97,    18,    19,    84,    85,     1,    24,   105,    17,
      23,    81,    82,    26,    27,    28,    29,    20,   105,    32,
      36,   116,   118,   117,    42,    43,    44,    45,    46,    66,
     125,     3,     4,    50,    51,    26,    27,    28,    29,    26,
      27,    28,    29,    26,    27,    28,    29,    60,    21,     5,
      26,    27,    28,    29,     8,     9,    10,    83,    26,    27,
      28,    29,    54,    28,    29,    55,    79,     3,     4,    33,
      22,    37,    34,    25,     3,     4,    26,    27,    28,    29,
     116,    93,   124,    84,    85,     5,    26,    27,    28,    29,
      39,    47,     5,    30,    35,    81,    82,    26,    27,    28,
      29,    49,    56,    78,    80,    53,    63,   119,    73,    64,
      26,    27,    28,    29,    65,    91,    76,    77,   126,    86,
      38,    95,    96,    87,    99,    89,    90,    92,   130,    94,
       7,     6,     7,     8,     9,    10,   127,    67,   103,   100,
     104,    83,    38,    85,   108,   107,    52,   109,   122,   111,
      61,    26,    27,    28,    29,   121,   129,    62,    59,   123,
     115,   131,     0,     0,     0,    75,     0,     6,     7,     8,
       9,    10,     0,    11,     6,     7,     8,     9,    10,     0,
      11,    48
};

static const yytype_int16 yycheck[] =
{
       1,    86,     3,     4,     3,     4,    80,    46,    94,     0,
      11,     5,     6,     7,     8,     9,    10,   113,   104,    20,
      21,   112,   107,   114,    25,    26,    27,    28,    29,     5,
     116,     7,     8,    34,    35,     7,     8,     9,    10,     7,
       8,     9,    10,     7,     8,     9,    10,    48,   113,    25,
       7,     8,     9,    10,   109,   110,   111,    51,     7,     8,
       9,    10,    57,     9,    10,    60,    67,     7,     8,     9,
     115,   108,    12,   112,     7,     8,     7,     8,     9,    10,
     112,    82,   114,     3,     4,    25,     7,     8,     9,    10,
     109,   108,    25,    14,    34,     5,     6,     7,     8,     9,
      10,   114,    97,    66,    67,    14,   108,   108,    49,   109,
       7,     8,     9,    10,    57,   114,    14,   109,    15,    18,
     114,    84,    85,    50,    87,   108,    14,    51,   129,   113,
     108,   107,   108,   109,   110,   111,    33,   113,   108,    69,
     113,    51,   114,     4,    18,   112,   114,    55,   111,    66,
     114,     7,     8,     9,    10,   109,   112,   114,    42,   114,
     104,   130,    -1,    -1,    -1,   114,    -1,   107,   108,   109,
     110,   111,    -1,   113,   107,   108,   109,   110,   111,    -1,
     113,   112
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    80,   117,     7,     8,    25,   107,   108,   109,   110,
     111,   113,   118,   120,   121,   122,   123,     0,   120,   120,
     113,   113,   115,   120,    46,   112,     7,     8,     9,    10,
      14,   119,   120,     9,    12,    34,   120,   108,   114,   109,
     124,   125,   120,   120,   120,   120,   120,   108,   112,   114,
     120,   120,   114,    14,    57,    60,    97,   130,   131,   119,
     120,   114,   114,   108,   109,    57,     5,   113,   120,   132,
     133,   134,   135,    49,   137,   114,    14,   109,   132,   120,
     132,     5,     6,    51,     3,     4,    18,    50,   139,   108,
      14,   114,    51,   120,   113,   132,   132,   122,   138,   132,
      69,   140,   126,   108,   113,   121,   136,   112,    18,    55,
     143,    66,   128,   129,   127,   136,   112,   114,   122,   120,
     141,   109,   132,   129,   114,   121,    15,    33,   142,   112,
     120,   142
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   116,   117,   118,   118,   119,   119,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   121,   121,
     121,   122,   122,   123,   123,   123,   123,   124,   124,   126,
     125,   127,   125,   128,   128,   129,   130,   130,   131,   132,
     132,   132,   132,   132,   133,   133,   134,   135,   135,   136,
     136,   137,   137,   138,   138,   139,   139,   140,   140,   141,
     141,   142,   142,   142,   143,   143
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     9,     2,     4,     0,     2,     3,     3,     3,
       3,     6,     2,     2,     1,     1,     1,     3,     1,     1,
       1,     1,     3,     4,     5,     5,     4,     3,     1,     0,
       7,     0,     8,     0,     1,     2,     0,     1,     2,     3,
       3,     2,     3,     1,     1,     1,     3,     5,     6,     1,
       3,     0,     3,     1,     3,     0,     2,     0,     3,     2,
       4,     0,     1,     1,     0,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      YY_LAC_DISCARD ("YYBACKUP");                              \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (queryData, scanner, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, queryData, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, struct qryData* queryData, yyscan_t scanner)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (queryData);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, struct qryData* queryData, yyscan_t scanner)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, queryData, scanner);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, struct qryData* queryData, yyscan_t scanner)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , queryData, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, queryData, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

/* Given a state stack such that *YYBOTTOM is its bottom, such that
   *YYTOP is either its top or is YYTOP_EMPTY to indicate an empty
   stack, and such that *YYCAPACITY is the maximum number of elements it
   can hold without a reallocation, make sure there is enough room to
   store YYADD more elements.  If not, allocate a new stack using
   YYSTACK_ALLOC, copy the existing elements, and adjust *YYBOTTOM,
   *YYTOP, and *YYCAPACITY to reflect the new capacity and memory
   location.  If *YYBOTTOM != YYBOTTOM_NO_FREE, then free the old stack
   using YYSTACK_FREE.  Return 0 if successful or if no reallocation is
   required.  Return 1 if memory is exhausted.  */
static int
yy_lac_stack_realloc (YYSIZE_T *yycapacity, YYSIZE_T yyadd,
#if YYDEBUG
                      char const *yydebug_prefix,
                      char const *yydebug_suffix,
#endif
                      yytype_int16 **yybottom,
                      yytype_int16 *yybottom_no_free,
                      yytype_int16 **yytop, yytype_int16 *yytop_empty)
{
  YYSIZE_T yysize_old =
    *yytop == yytop_empty ? 0 : *yytop - *yybottom + 1;
  YYSIZE_T yysize_new = yysize_old + yyadd;
  if (*yycapacity < yysize_new)
    {
      YYSIZE_T yyalloc = 2 * yysize_new;
      yytype_int16 *yybottom_new;
      /* Use YYMAXDEPTH for maximum stack size given that the stack
         should never need to grow larger than the main state stack
         needs to grow without LAC.  */
      if (YYMAXDEPTH < yysize_new)
        {
          YYDPRINTF ((stderr, "%smax size exceeded%s", yydebug_prefix,
                      yydebug_suffix));
          return 1;
        }
      if (YYMAXDEPTH < yyalloc)
        yyalloc = YYMAXDEPTH;
      yybottom_new =
        (yytype_int16*) YYSTACK_ALLOC (yyalloc * sizeof *yybottom_new);
      if (!yybottom_new)
        {
          YYDPRINTF ((stderr, "%srealloc failed%s", yydebug_prefix,
                      yydebug_suffix));
          return 1;
        }
      if (*yytop != yytop_empty)
        {
          YYCOPY (yybottom_new, *yybottom, yysize_old);
          *yytop = yybottom_new + (yysize_old - 1);
        }
      if (*yybottom != yybottom_no_free)
        YYSTACK_FREE (*yybottom);
      *yybottom = yybottom_new;
      *yycapacity = yyalloc;
    }
  return 0;
}

/* Establish the initial context for the current lookahead if no initial
   context is currently established.

   We define a context as a snapshot of the parser stacks.  We define
   the initial context for a lookahead as the context in which the
   parser initially examines that lookahead in order to select a
   syntactic action.  Thus, if the lookahead eventually proves
   syntactically unacceptable (possibly in a later context reached via a
   series of reductions), the initial context can be used to determine
   the exact set of tokens that would be syntactically acceptable in the
   lookahead's place.  Moreover, it is the context after which any
   further semantic actions would be erroneous because they would be
   determined by a syntactically unacceptable token.

   YY_LAC_ESTABLISH should be invoked when a reduction is about to be
   performed in an inconsistent state (which, for the purposes of LAC,
   includes consistent states that don't know they're consistent because
   their default reductions have been disabled).  Iff there is a
   lookahead token, it should also be invoked before reporting a syntax
   error.  This latter case is for the sake of the debugging output.

   For parse.lac=full, the implementation of YY_LAC_ESTABLISH is as
   follows.  If no initial context is currently established for the
   current lookahead, then check if that lookahead can eventually be
   shifted if syntactic actions continue from the current context.
   Report a syntax error if it cannot.  */
#define YY_LAC_ESTABLISH                                         \
do {                                                             \
  if (!yy_lac_established)                                       \
    {                                                            \
      YYDPRINTF ((stderr,                                        \
                  "LAC: initial context established for %s\n",   \
                  yytname[yytoken]));                            \
      yy_lac_established = 1;                                    \
      {                                                          \
        int yy_lac_status =                                      \
          yy_lac (yyesa, &yyes, &yyes_capacity, yyssp, yytoken); \
        if (yy_lac_status == 2)                                  \
          goto yyexhaustedlab;                                   \
        if (yy_lac_status == 1)                                  \
          goto yyerrlab;                                         \
      }                                                          \
    }                                                            \
} while (0)

/* Discard any previous initial lookahead context because of Event,
   which may be a lookahead change or an invalidation of the currently
   established initial context for the current lookahead.

   The most common example of a lookahead change is a shift.  An example
   of both cases is syntax error recovery.  That is, a syntax error
   occurs when the lookahead is syntactically erroneous for the
   currently established initial context, so error recovery manipulates
   the parser stacks to try to find a new initial context in which the
   current lookahead is syntactically acceptable.  If it fails to find
   such a context, it discards the lookahead.  */
#if YYDEBUG
# define YY_LAC_DISCARD(Event)                                           \
do {                                                                     \
  if (yy_lac_established)                                                \
    {                                                                    \
      if (yydebug)                                                       \
        YYFPRINTF (stderr, "LAC: initial context discarded due to "      \
                   Event "\n");                                          \
      yy_lac_established = 0;                                            \
    }                                                                    \
} while (0)
#else
# define YY_LAC_DISCARD(Event) yy_lac_established = 0
#endif

/* Given the stack whose top is *YYSSP, return 0 iff YYTOKEN can
   eventually (after perhaps some reductions) be shifted, return 1 if
   not, or return 2 if memory is exhausted.  As preconditions and
   postconditions: *YYES_CAPACITY is the allocated size of the array to
   which *YYES points, and either *YYES = YYESA or *YYES points to an
   array allocated with YYSTACK_ALLOC.  yy_lac may overwrite the
   contents of either array, alter *YYES and *YYES_CAPACITY, and free
   any old *YYES other than YYESA.  */
static int
yy_lac (yytype_int16 *yyesa, yytype_int16 **yyes,
        YYSIZE_T *yyes_capacity, yytype_int16 *yyssp, int yytoken)
{
  yytype_int16 *yyes_prev = yyssp;
  yytype_int16 *yyesp = yyes_prev;
  YYDPRINTF ((stderr, "LAC: checking lookahead %s:", yytname[yytoken]));
  if (yytoken == YYUNDEFTOK)
    {
      YYDPRINTF ((stderr, " Always Err\n"));
      return 1;
    }
  while (1)
    {
      int yyrule = yypact[*yyesp];
      if (yypact_value_is_default (yyrule)
          || (yyrule += yytoken) < 0 || YYLAST < yyrule
          || yycheck[yyrule] != yytoken)
        {
          yyrule = yydefact[*yyesp];
          if (yyrule == 0)
            {
              YYDPRINTF ((stderr, " Err\n"));
              return 1;
            }
        }
      else
        {
          yyrule = yytable[yyrule];
          if (yytable_value_is_error (yyrule))
            {
              YYDPRINTF ((stderr, " Err\n"));
              return 1;
            }
          if (0 < yyrule)
            {
              YYDPRINTF ((stderr, " S%d\n", yyrule));
              return 0;
            }
          yyrule = -yyrule;
        }
      {
        YYSIZE_T yylen = yyr2[yyrule];
        YYDPRINTF ((stderr, " R%d", yyrule - 1));
        if (yyesp != yyes_prev)
          {
            YYSIZE_T yysize = yyesp - *yyes + 1;
            if (yylen < yysize)
              {
                yyesp -= yylen;
                yylen = 0;
              }
            else
              {
                yylen -= yysize;
                yyesp = yyes_prev;
              }
          }
        if (yylen)
          yyesp = yyes_prev -= yylen;
      }
      {
        int yystate;
        {
          int yylhs = yyr1[yyrule] - YYNTOKENS;
          yystate = yypgoto[yylhs] + *yyesp;
          if (yystate < 0 || YYLAST < yystate
              || yycheck[yystate] != *yyesp)
            yystate = yydefgoto[yylhs];
          else
            yystate = yytable[yystate];
        }
        if (yyesp == yyes_prev)
          {
            yyesp = *yyes;
            *yyesp = yystate;
          }
        else
          {
            if (yy_lac_stack_realloc (yyes_capacity, 1,
#if YYDEBUG
                                      " (", ")",
#endif
                                      yyes, yyesa, &yyesp, yyes_prev))
              {
                YYDPRINTF ((stderr, "\n"));
                return 2;
              }
            *++yyesp = yystate;
          }
        YYDPRINTF ((stderr, " G%d", yystate));
      }
    }
}


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.  In order to see if a particular token T is a
   valid looakhead, invoke yy_lac (YYESA, YYES, YYES_CAPACITY, YYSSP, T).

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store or if
   yy_lac returned 2.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyesa, yytype_int16 **yyes,
                YYSIZE_T *yyes_capacity, yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
       In the first two cases, it might appear that the current syntax
       error should have been detected in the previous state when yy_lac
       was invoked.  However, at that time, there might have been a
       different syntax error that discarded a different initial context
       during error recovery, leaving behind the current lookahead.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      YYDPRINTF ((stderr, "Constructing syntax error message\n"));
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          int yyx;

          for (yyx = 0; yyx < YYNTOKENS; ++yyx)
            if (yyx != YYTERROR && yyx != YYUNDEFTOK)
              {
                {
                  int yy_lac_status = yy_lac (yyesa, yyes, yyes_capacity,
                                              yyssp, yyx);
                  if (yy_lac_status == 2)
                    return 2;
                  if (yy_lac_status == 1)
                    continue;
                }
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
# if YYDEBUG
      else if (yydebug)
        YYFPRINTF (stderr, "No expected tokens.\n");
# endif
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, struct qryData* queryData, yyscan_t scanner)
{
  YYUSE (yyvaluep);
  YYUSE (queryData);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (struct qryData* queryData, yyscan_t scanner)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

    yytype_int16 yyesa[20];
    yytype_int16 *yyes;
    YYSIZE_T yyes_capacity;

  int yy_lac_established = 0;
  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  yyes = yyesa;
  yyes_capacity = sizeof yyesa / sizeof *yyes;
  if (YYMAXDEPTH < yyes_capacity)
    yyes_capacity = YYMAXDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    {
      YY_LAC_ESTABLISH;
      goto yydefault;
    }
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      YY_LAC_ESTABLISH;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  YY_LAC_DISCARD ("shift");

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  {
    int yychar_backup = yychar;
    switch (yyn)
      {
          case 3:
#line 84 "sql.y" /* yacc.c:1646  */
    {parse_exp_commalist(queryData, (yyvsp[-1].expressionPtr), (yyvsp[0].strval), GRP_NONE); }
#line 1701 "sql.c" /* yacc.c:1646  */
    break;

  case 4:
#line 85 "sql.y" /* yacc.c:1646  */
    {parse_exp_commalist(queryData, (yyvsp[-1].expressionPtr), (yyvsp[0].strval), GRP_NONE); }
#line 1707 "sql.c" /* yacc.c:1646  */
    break;

  case 5:
#line 88 "sql.y" /* yacc.c:1646  */
    { (yyval.strval) = NULL; }
#line 1713 "sql.c" /* yacc.c:1646  */
    break;

  case 6:
#line 89 "sql.y" /* yacc.c:1646  */
    { if(queryData->parseMode != 1) { free((yyvsp[0].strval)); (yyval.strval) = NULL; } else {(yyval.strval) = (yyvsp[0].strval);} }
#line 1719 "sql.c" /* yacc.c:1646  */
    break;

  case 7:
#line 93 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_scalar_exp(queryData, (yyvsp[-2].expressionPtr), EXP_PLUS, (yyvsp[0].expressionPtr)); }
#line 1725 "sql.c" /* yacc.c:1646  */
    break;

  case 8:
#line 94 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_scalar_exp(queryData, (yyvsp[-2].expressionPtr), EXP_MINUS, (yyvsp[0].expressionPtr)); }
#line 1731 "sql.c" /* yacc.c:1646  */
    break;

  case 9:
#line 95 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_scalar_exp(queryData, (yyvsp[-2].expressionPtr), EXP_MULTIPLY, (yyvsp[0].expressionPtr)); }
#line 1737 "sql.c" /* yacc.c:1646  */
    break;

  case 10:
#line 96 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_scalar_exp(queryData, (yyvsp[-2].expressionPtr), EXP_DIVIDE, (yyvsp[0].expressionPtr)); }
#line 1743 "sql.c" /* yacc.c:1646  */
    break;

  case 11:
#line 97 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_scalar_exp(queryData, (yyvsp[-3].expressionPtr), EXP_CONCAT, (yyvsp[-1].expressionPtr)); }
#line 1749 "sql.c" /* yacc.c:1646  */
    break;

  case 12:
#line 98 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_scalar_exp(queryData, (yyvsp[0].expressionPtr), EXP_UPLUS, NULL); }
#line 1755 "sql.c" /* yacc.c:1646  */
    break;

  case 13:
#line 99 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_scalar_exp(queryData, (yyvsp[0].expressionPtr), EXP_UMINUS, NULL); }
#line 1761 "sql.c" /* yacc.c:1646  */
    break;

  case 14:
#line 100 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_scalar_exp_literal(queryData, (yyvsp[0].strval)); free((yyvsp[0].strval)); }
#line 1767 "sql.c" /* yacc.c:1646  */
    break;

  case 15:
#line 101 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_scalar_exp_column_ref(queryData, (yyvsp[0].referencePtr)); }
#line 1773 "sql.c" /* yacc.c:1646  */
    break;

  case 16:
#line 102 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = (yyvsp[0].expressionPtr); }
#line 1779 "sql.c" /* yacc.c:1646  */
    break;

  case 17:
#line 103 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = (yyvsp[-1].expressionPtr); }
#line 1785 "sql.c" /* yacc.c:1646  */
    break;

  case 18:
#line 107 "sql.y" /* yacc.c:1646  */
    { (yyval.strval) = (yyvsp[0].strval); }
#line 1791 "sql.c" /* yacc.c:1646  */
    break;

  case 19:
#line 108 "sql.y" /* yacc.c:1646  */
    { (yyval.strval) = NULL; }
#line 1797 "sql.c" /* yacc.c:1646  */
    break;

  case 20:
#line 109 "sql.y" /* yacc.c:1646  */
    { (yyval.strval) = NULL; }
#line 1803 "sql.c" /* yacc.c:1646  */
    break;

  case 21:
#line 113 "sql.y" /* yacc.c:1646  */
    {
      if(parse_column_ref_unsuccessful(queryData, &((yyval.referencePtr)), NULL, (yyvsp[0].strval))) {
        fprintf(stderr, "unknown or ambiguous column name\n");
        YYERROR;
      }
    }
#line 1814 "sql.c" /* yacc.c:1646  */
    break;

  case 22:
#line 119 "sql.y" /* yacc.c:1646  */
    {
      if(parse_column_ref_unsuccessful(queryData, &((yyval.referencePtr)), (yyvsp[-2].strval), (yyvsp[0].strval))) {
        fprintf(stderr, "unknown or ambiguous column name\n");
        YYERROR;
      }
    }
#line 1825 "sql.c" /* yacc.c:1646  */
    break;

  case 23:
#line 128 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = NULL; }
#line 1831 "sql.c" /* yacc.c:1646  */
    break;

  case 24:
#line 129 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = NULL; }
#line 1837 "sql.c" /* yacc.c:1646  */
    break;

  case 25:
#line 130 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_function_ref(queryData, (yyvsp[-4].intval), (yyvsp[-1].expressionPtr)); }
#line 1843 "sql.c" /* yacc.c:1646  */
    break;

  case 26:
#line 131 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_function_ref(queryData, (yyvsp[-3].intval), (yyvsp[-1].expressionPtr)); }
#line 1849 "sql.c" /* yacc.c:1646  */
    break;

  case 27:
#line 135 "sql.y" /* yacc.c:1646  */
    { parse_table_factor(queryData, FALSE, (yyvsp[-2].strval), (yyvsp[0].strval)); }
#line 1855 "sql.c" /* yacc.c:1646  */
    break;

  case 29:
#line 140 "sql.y" /* yacc.c:1646  */
    { parse_table_factor(queryData, FALSE, (yyvsp[-2].strval), (yyvsp[0].strval)); }
#line 1861 "sql.c" /* yacc.c:1646  */
    break;

  case 31:
#line 141 "sql.y" /* yacc.c:1646  */
    { parse_table_factor(queryData, TRUE, (yyvsp[-2].strval), (yyvsp[0].strval)); }
#line 1867 "sql.c" /* yacc.c:1646  */
    break;

  case 35:
#line 150 "sql.y" /* yacc.c:1646  */
    { parse_where_clause(queryData, (yyvsp[0].expressionPtr)); }
#line 1873 "sql.c" /* yacc.c:1646  */
    break;

  case 38:
#line 159 "sql.y" /* yacc.c:1646  */
    { parse_where_clause(queryData, (yyvsp[0].expressionPtr)); }
#line 1879 "sql.c" /* yacc.c:1646  */
    break;

  case 39:
#line 163 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_scalar_exp(queryData, (yyvsp[-2].expressionPtr), EXP_AND, (yyvsp[0].expressionPtr)); }
#line 1885 "sql.c" /* yacc.c:1646  */
    break;

  case 40:
#line 164 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_scalar_exp(queryData, (yyvsp[-2].expressionPtr), EXP_OR, (yyvsp[0].expressionPtr)); }
#line 1891 "sql.c" /* yacc.c:1646  */
    break;

  case 41:
#line 165 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_scalar_exp(queryData, (yyvsp[0].expressionPtr), EXP_NOT, NULL); }
#line 1897 "sql.c" /* yacc.c:1646  */
    break;

  case 42:
#line 166 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = (yyvsp[-1].expressionPtr); }
#line 1903 "sql.c" /* yacc.c:1646  */
    break;

  case 43:
#line 167 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = (yyvsp[0].expressionPtr); }
#line 1909 "sql.c" /* yacc.c:1646  */
    break;

  case 44:
#line 171 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = (yyvsp[0].expressionPtr); }
#line 1915 "sql.c" /* yacc.c:1646  */
    break;

  case 45:
#line 172 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = (yyvsp[0].expressionPtr); }
#line 1921 "sql.c" /* yacc.c:1646  */
    break;

  case 46:
#line 176 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_scalar_exp(queryData, (yyvsp[-2].expressionPtr), (yyvsp[-1].intval)+EXP_EQ, (yyvsp[0].expressionPtr)); }
#line 1927 "sql.c" /* yacc.c:1646  */
    break;

  case 47:
#line 180 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_in_predicate(queryData, (yyvsp[-4].expressionPtr), FALSE, (yyvsp[-1].atomPtr)); }
#line 1933 "sql.c" /* yacc.c:1646  */
    break;

  case 48:
#line 181 "sql.y" /* yacc.c:1646  */
    { (yyval.expressionPtr) = parse_in_predicate(queryData, (yyvsp[-5].expressionPtr), TRUE, (yyvsp[-1].atomPtr)); }
#line 1939 "sql.c" /* yacc.c:1646  */
    break;

  case 49:
#line 185 "sql.y" /* yacc.c:1646  */
    { (yyval.atomPtr) = parse_atom_commalist(queryData, NULL, (yyvsp[0].strval)); }
#line 1945 "sql.c" /* yacc.c:1646  */
    break;

  case 50:
#line 186 "sql.y" /* yacc.c:1646  */
    { (yyval.atomPtr) = parse_atom_commalist(queryData, (yyvsp[-2].atomPtr), (yyvsp[0].strval)); }
#line 1951 "sql.c" /* yacc.c:1646  */
    break;

  case 53:
#line 195 "sql.y" /* yacc.c:1646  */
    { parse_grouping_spec(queryData, parse_scalar_exp_column_ref(queryData, (yyvsp[0].referencePtr))); }
#line 1957 "sql.c" /* yacc.c:1646  */
    break;

  case 54:
#line 196 "sql.y" /* yacc.c:1646  */
    { parse_grouping_spec(queryData, parse_scalar_exp_column_ref(queryData, (yyvsp[0].referencePtr))); }
#line 1963 "sql.c" /* yacc.c:1646  */
    break;

  case 59:
#line 210 "sql.y" /* yacc.c:1646  */
    { parse_ordering_spec(queryData, (yyvsp[-1].expressionPtr), (yyvsp[0].intval)); }
#line 1969 "sql.c" /* yacc.c:1646  */
    break;

  case 60:
#line 211 "sql.y" /* yacc.c:1646  */
    { parse_ordering_spec(queryData, (yyvsp[-1].expressionPtr), (yyvsp[0].intval)); }
#line 1975 "sql.c" /* yacc.c:1646  */
    break;

  case 61:
#line 215 "sql.y" /* yacc.c:1646  */
    {(yyval.intval) = 0;}
#line 1981 "sql.c" /* yacc.c:1646  */
    break;

  case 62:
#line 216 "sql.y" /* yacc.c:1646  */
    {(yyval.intval) = 0;}
#line 1987 "sql.c" /* yacc.c:1646  */
    break;

  case 63:
#line 217 "sql.y" /* yacc.c:1646  */
    {(yyval.intval) = 1;}
#line 1993 "sql.c" /* yacc.c:1646  */
    break;

  case 65:
#line 222 "sql.y" /* yacc.c:1646  */
    { if(queryData->parseMode != 1) {free((yyvsp[0].strval));} else {queryData->intoFileName = (yyvsp[0].strval);} }
#line 1999 "sql.c" /* yacc.c:1646  */
    break;


#line 2003 "sql.c" /* yacc.c:1646  */
        default: break;
      }
    if (yychar_backup != yychar)
      YY_LAC_DISCARD ("yychar change");
  }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (queryData, scanner, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyesa, &yyes, &yyes_capacity, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        if (yychar != YYEMPTY)
          YY_LAC_ESTABLISH;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (queryData, scanner, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, queryData, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, queryData, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  /* If the stack popping above didn't lose the initial context for the
     current lookahead token, the shift below will for sure.  */
  YY_LAC_DISCARD ("error recovery");

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if 1
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (queryData, scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, queryData, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, queryData, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yyes != yyesa)
    YYSTACK_FREE (yyes);
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 224 "sql.y" /* yacc.c:1906  */

