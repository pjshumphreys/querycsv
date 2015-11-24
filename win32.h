#include <windows.h>
/*
#define MAX_UTF8_PATH 780 // (_MAX_PATH)*3

struct dirent {
  unsigned  d_type;
  time_t    d_ctime; //-1 for FAT file systems
  time_t    d_atime; //-1 for FAT file systems
  time_t    d_mtime;
  int64_t   d_size; //64-bit size info
  char      d_name[MAX_UTF8_PATH]; 
  char      d_first; //flag for 1st time
  long      d_handle; //handle to pass to FindNext
};

typedef struct dirent DIR;

DIR *opendir(const char *name);
struct dirent *readdir(DIR *inval);
int closedir(DIR * inval);
*/

void setupWin32(int * argc, char *** argv);
void strAppendChars_w32(FILE* stream, char** value, size_t* strSize);
int fputs_w32(const char *str, FILE *stream);
int fprintf_w32(FILE *stream, const char *format, ...);
FILE *fopen_w32(const char *filename, const char *mode);
char* getcwd_w32(char* buf, size_t size);

#define strAppendChars strAppendChars_w32
#define fputs fputs_w32
#define fopen fopen_w32
#define fprintf fprintf_w32
#define getcwd getcwd_w32
#define YYFPRINTF fprintf_w32   //for the bison parser
