/*as we will be using ascii/utf-8 internally for text and don't want to recompile the standard library
on the c64 build, the format specifiers need to be in uppercase */
#ifdef __CC65__
  #define S_STRING "%S"
  #define LD_STRING "%LD"
  #define D_STRING "%D"
  #define FOPEN_READ "RB";
  #define FOPEN_WRITE "WB";
  #define TDB_MALLOC_FAILED2 "cOULD NOT ALLOCATE NEEDED MEMORY\n" /*avoid doing another malloc to print the out of memory message */
#else
  #define S_STRING "%s"
  #define LD_STRING "%ld"
  #define D_STRING "%d"
  #define FOPEN_READ "rb";
  #define FOPEN_WRITE "wb";
  #define TDB_MALLOC_FAILED2 "Could not allocate needed memory\n"
#endif

/* some translatable text strings */
#define TDB_DEFAULT_TZ "TZ=GMT0BST,M3.5.0/1,M10.5.0/2"
#define TDB_LOCALE "en_GB.UTF8"
#define TDB_COULDNT_OPEN_INPUT2 "Couldn't open input file\n"
#define TDB_COULDNT_OPEN_INPUT inputOpenFail
#define TDB_TZTIMED_FAILED "tztime_d failed\n"
#define TDB_SPRINTFD_FAILED "sprintf_d failed\n"
#define TDB_STRFTIMED_FAILED "strftime_d failed\n"
#define TDB_COULDNT_SEEK "Couldn't seek to specified file offset\n"
#define TDB_PARSER_SYNTAX "A syntax error occurred when parsing the input file\n"
#define TDB_PARSER_USED_ALL_RAM "Parsing the input file used up all memory storage space\n"
#define TDB_PARSER_UNKNOWN "An unknown error occurred when parsing the input file\n"
#define TDB_LEX_UNTERMINATED "Unterminated string on line " LD_STRING " (" S_STRING ")\n", lineno, text
#define TDB_MALLOC_FAILED mallocFail
#define TDB_MALLOC_FAILED3 TDB_MALLOC_FAILED, 1, 33
#define TDB_NFD_BROKEN "The unicode NFD conversion code is broken\n"
#define TDB_UNTITLED_COLUMN "_Column " D_STRING
#define TDB_INVALID_COMMAND_LINE "No file name specifed\nUsage: querycsv [filename]\n"
#define TDB_INVALID_REALLOC "ReallocMsg called with NULL\n"
#define TDB_INVALID_UTF8 "Invalid utf-8 bytes stored in memory\n"
#define TDB_INVALID_COUNT_STAR "only count(*) is valid\n"
#define TDB_OUTPUT_FAILED "opening output file failed\n"
#define TDB_AGGREG_AGGREG "can't aggregate an aggregate\n"
#define TDB_INVALID_ENCODING "unsupported encoding " S_STRING "\n"
#define TDB_PRESS_A_KEY "\nPress any key to continue\n"
#define TDB_NAME "unknown or ambiguous column name (" S_STRING ")\n"
#define TDB_NAME_NAME "unknown or ambiguous column name (" S_STRING "." S_STRING ")\n"
#define TDB_FILENAME "filename:"
#define TDB_NOT_FOUND "not found"
#define TDB_ENCODING "encoding:"
