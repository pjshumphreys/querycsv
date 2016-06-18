void setupWin32(int * argc, char *** argv);
int fputs_w32(const char *str, FILE *stream);
int fprintf_w32(FILE *stream, const char *format, ...);
FILE *fopen_w32(const char *filename, const char *mode);
char* getcwd_w32(char* buf, size_t size);

#define fputs fputs_w32
#define fopen fopen_w32
#define fprintf fprintf_w32
#define getcwd getcwd_w32
#define YYFPRINTF fprintf_w32   //for the bison parser
