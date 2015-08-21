//some translatable text strings
#define TDB_DEFAULT_TZ "TZ=GMT0BST,M3.5.0/1,M10.5.0/2"
#define TDB_LOCALE "en_GB.UTF8"
#define TDB_COULDNT_OPEN_INPUT "Couldn't open input file\n"
#define TDB_NOT_YET_IMPLEMENTED "Not yet implemented\n"
#define TDB_TZTIMED_FAILED "tztime_d failed\n"
#define TDB_SNPRINTFD_FAILED "snprintf_d failed\n"
#define TDB_STRFTIMED_FAILED "strftime_d failed\n"
#define TDB_COULDNT_SEEK "Couldn't seek to specified file offset\n"
#define TDB_PARSER_SYNTAX "A syntax error occurred when parsing the input file\n"
#define TDB_PARSER_USED_ALL_RAM "Parsing the input file used up all memory storage space\n"
#define TDB_PARSER_UNKNOWN "An unknown error occurred when parsing the input file\n"
#define TDB_LEX_UNTERMINATED "Unterminated string on line %d (%s)\n", lineno, text
#define TDB_MALLOC_FAILED "Could not allocate needed memory\n"
#define TDB_NFD_BROKEN "The unicode NFD conversion code is broken\n"
#define TDB_UNTITLED_COLUMN "_Column %d", queryData->columnCount

#define TDB_INVALID_COMMAND_LINE "Invalid command line options\nUsage: querycsv option\nOptions:\n--run queryfile                       Run the select statement in the file\n                                      named queryfile\n--columns csvfile                     Outputs the number of columns the the\n                                      csv file named csvfile\n--next csvfile previousoffset         Outputs the file offset of the start of\n                                      the next record in the csv file csvfile\n                                      after the previous offset. outputs the\n                                      first record offset if previousoffset is\n                                      0\n--value csvfile offset column         Outputs the text of field column in the\n                                      record starting at offset in the csv\n                                      file csvfile.\n--date                                Outputs the current date in ISO-8601\n                                      format, including the UTC offset. The\n                                      UTC offset can be changed by setting\n                                      the TZ environment variable"

