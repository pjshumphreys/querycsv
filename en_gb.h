/* some translatable text strings */
#define TDB_DEFAULT_TZ "TZ=GMT0BST,M3.5.0/1,M10.5.0/2"
#define TDB_LOCALE "en_GB.UTF8"
#define TDB_COULDNT_OPEN_INPUT2 "Couldn't open input file\n"
#define TDB_COULDNT_OPEN_INPUT inputOpenFail
#define TDB_NOT_YET_IMPLEMENTED "Not yet implemented\n"
#define TDB_TZTIMED_FAILED "tztime_d failed\n"
#define TDB_SPRINTFD_FAILED "sprintf_d failed\n"
#define TDB_STRFTIMED_FAILED "strftime_d failed\n"
#define TDB_COULDNT_SEEK "Couldn't seek to specified file offset\n"
#define TDB_PARSER_SYNTAX "A syntax error occurred when parsing the input file\n"
#define TDB_PARSER_USED_ALL_RAM "Parsing the input file used up all memory storage space\n"
#define TDB_PARSER_UNKNOWN "An unknown error occurred when parsing the input file\n"
#define TDB_LEX_UNTERMINATED "Unterminated string on line %ld (%s)\n", lineno, text
#define TDB_MALLOC_FAILED2 "Could not allocate needed memory\n"
#define TDB_MALLOC_FAILED mallocFail
#define TDB_NFD_BROKEN "The unicode NFD conversion code is broken\n"
#define TDB_UNTITLED_COLUMN "_Column %d", queryData->columnCount
#define TDB_INVALID_COMMAND_LINE "No file name specifed\nUsage: querycsv [filename]"
#define TDB_INVALID_REALLOC "ReallocMsg called with NULL\n"
#define TDB_INVALID_UTF8 "Invalid utf-8 bytes stored in memory\n"
#define TDB_INVALID_COUNT_STAR "only count(*) is valid\n"
#define TDB_OUTPUT_FAILED "opening output file failed\n"
#define TDB_AGGREG_AGGREG "can't aggregate an aggregate\n"
#define TDB_INVALID_ENCODING "unsupported encoding %s\n", encoding
