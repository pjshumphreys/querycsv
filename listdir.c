#include <stdio.h>      /* printf, fopen */
#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h> 

#define TRUE 1
#define FALSE 0
#define DEVNULL "/dev/null"

//free memory and set the pointer to NULL. this function should hopefully get inlined
void strFree(char ** str) {
  free(*str);
  *str = NULL;
}

//*
#include <unistd.h>     // getcwd
#include <dirent.h>     // opendir, readdir, closedir

int starts_with(const char *string, const char *prefix) {
  while(*prefix) {
      if(*prefix++ != *string++) {
        return FALSE;
      }
  }

  return TRUE;
}

char* readlink_d(char *path) {
  ssize_t result = 0;
  ssize_t old_result =-1;
  ssize_t len = 16;
  char * str = NULL;
  char * str2 = NULL;

  while (result == -1 || result != old_result) {
    if((str2 = (char*)realloc(str, len)) == NULL) {
      free(str);
      return NULL;
    }
    str = str2;
    
    old_result = result;
    result = readlink(path, str, len);
    len *= 2;
  }

  //realloc 1 more byte for null terminator
  if((str2 = (char*)realloc(str, result+1)) == NULL) {
    free(str);
    return NULL;
  }
  str = str2;

  str[result] = '\0';

  //shrink the allocation
  if((str2 = (char*)realloc(str, strlen(str)+1)) == NULL) {
    free(str);
    return NULL;
  }

  return str2;
} 

int statdir(char * dir, struct dirent * direp, char** fullPath, struct stat *buf) {
  char* temp;
  char* orig_path;
  char* orig_path2;
  char* link_path;
  int retval;

  temp = (char*)malloc(strlen(dir)+strlen(direp->d_name)+2);
  sprintf(temp, "%s/%s", dir, direp->d_name);
  *fullPath = temp;

  retval = lstat(temp, buf);

  if(retval != 0) {
    strFree(fullPath);
    return retval;
  }

  if(S_ISLNK(buf->st_mode)) {
    retval = stat(temp, buf);

    if(retval != 0) {
      retval = lstat(temp, buf);
    }
    else if(S_ISDIR(buf->st_mode)) {
      orig_path = NULL;
      orig_path2 = NULL;
      link_path = NULL;

      link_path = realpath(temp, NULL);

      getcwd_d(&orig_path);

      orig_path2 = realpath(orig_path, NULL); 
           
      if(starts_with(orig_path2, link_path) ||
      starts_with(link_path, orig_path2)){
        retval = lstat(temp, buf);
      }

      free(link_path);
      free(orig_path);
      free(orig_path2);
    }
  }
  
  return retval;
}


//*/

/*
#include "win32.h"

//*/

int needsEscaping(char* str, int params) {
  if(
    str == NULL ||
    //strcmp(str, (((params & PRM_EXPORT) == 0)?"\\N":"\\N")) == 0 ||
    *str == ' ' ||
    *str == '\t') {
    return TRUE;
  }
     
  while(*str) {
    if(*str == '"' || *str == '\n' || *str == ',') {
    return TRUE;
    }
    str++;
  }

  str--;
     
  if(*str == ' ' || *str == '\t') {
    return TRUE;
  }
  
  return FALSE;
}

char *strReplace(char *search, char *replace, char *subject) {
  char *replaced = (char*)calloc(1, 1), *temp = NULL;
  char *p = subject, *p3 = subject, *p2;
  int  found = 0;

  if(
      search == NULL ||
      replace == NULL ||
      subject == NULL ||
      strlen(search) == 0 ||
      strlen(replace) == 0 ||
      strlen(subject) == 0
    ) {
    return NULL;
  }  
  
  while((p = strstr(p, search)) != NULL) {
    found = 1;
    temp = realloc(replaced, strlen(replaced) + (p - p3) + strlen(replace));

    if(temp == NULL) {
      free(replaced);
      return NULL;
    }

    replaced = temp;
    strncat(replaced, p - (p - p3), p - p3);
    strcat(replaced, replace);
    p3 = p + strlen(search);
    p += strlen(search);
    p2 = p;
  }
  
  if (found == 1) {
    if (strlen(p2) > 0) {
      temp = realloc(replaced, strlen(replaced) + strlen(p2) + 1);

      if (temp == NULL) {
        free(replaced);
        return NULL;
      }

      replaced = temp;
      strcat(replaced, p2);
    }
  }
  else {
    temp = realloc(replaced, strlen(subject) + 1);

    if (temp != NULL) {
      replaced = temp;
      strcpy(replaced, subject);
    }
  }

  return replaced;
}

//get localtime, gmtime and utc offset string from a time_t. allocate/free memory as needed
//any of the last three parameters can be skipped by passing null
int tztime_d(time_t *now, struct tm *local, struct tm *utc, char **output) {
  struct tm * lcl;
  struct tm * gm;

  int hourlcl;
  int hourutc;
  int difference;
  int hour_difference;
  int minute_difference;

  char* output2;
  char* format = "+%02i%02i";

  if(now == NULL) {
    return FALSE;
  }

  if(local != NULL) {
    lcl = local;

    memcpy((void*)lcl, (void*)localtime(now), sizeof(struct tm));
  }
  else if (output != NULL) {
    lcl = (struct tm *)malloc(sizeof(struct tm));

    if(lcl == NULL) {
      return FALSE;
    }

    memcpy((void*)lcl, (void*)localtime(now), sizeof(struct tm));
  } 

  if(utc != NULL) {
    gm = utc;

    memcpy((void*)gm, (void*)gmtime(now), sizeof(struct tm));
  }
  else if(output != NULL) {
    gm = (struct tm *)malloc(sizeof(struct tm));

    if(gm == NULL) {
      if(local == NULL) {
        free((void*)lcl);
      }

      return FALSE;
    }

    memcpy((void*)gm, (void*)gmtime(now), sizeof(struct tm));
  }

  if(output != NULL) {
    hourlcl = lcl->tm_hour;
    hourutc = gm->tm_hour;
    
    output2 = (char*)realloc((void*)*output, 6*sizeof(char));
    
    if(output2 == NULL) {
      if(local == NULL) {
        free((void*)lcl);
      }

      if(utc == NULL) {
        free((void*)gm);
      }

      return FALSE;
    }
    *output = output2;

    if(lcl->tm_year > gm->tm_year) {
      hourlcl+=24;
    }
    else if (gm->tm_year > lcl->tm_year) {
      hourutc+=24;
    }
    else if(lcl->tm_yday > gm->tm_yday) {
      hourlcl+=24;
    }
    else if (gm->tm_yday > lcl->tm_yday) {
      hourutc+=24;
    }

    difference = ((hourlcl - hourutc)*60)+lcl->tm_min - gm->tm_min;
    minute_difference = difference % 60;
    hour_difference = (difference - minute_difference)/60;

    if(hour_difference < 0) {
      format = "%03i%02i";
    }

    sprintf(*output, format, hour_difference, minute_difference);
  }
  
  if(local == NULL) {
    free((void*)lcl);
  }

  if(utc == NULL) {
    free((void*)gm);
  }

  return TRUE;
}

//format a date into a string. allocate/free memory as needed
int strftime_d(char** ptr, char* format, struct tm* timeptr) {
  size_t length = 32; //starting value
  size_t length2 = 0;
  char *output = NULL;
  char *output2 = NULL;

  if(ptr == NULL || format == NULL) {
    return FALSE;
  }

  while (length2 == 0) {
    output2 = realloc((void*)output, length*sizeof(char));

    if(output2 == NULL) {
      strFree(&output);

      return FALSE;
    }
    output = output2;

    //calling strftime using the buffer we created
    length2 = strftime(output, length, format, timeptr);

    //double the allocation length to use if we need to try again
    length *= 2;  
  }

  //shrink the allocated memory to fit the returned length
  output2 = realloc((void*)output, (length2+1)*sizeof(char));

  //quit if the shrinking didn't work successfully
  if(output2 == NULL) {
    strFree(&output);

    return FALSE;
  }

  //free the contents of ptr then update it to point to the string we've built up
  strFree(ptr);
  *ptr = output2;

  //everything completed successfully
  return TRUE;
}

//write a formatted string into a string buffer. allocate/free memory as needed
int sprintf_d(char** str, char* format, ...) {
  FILE* pFile;
  size_t newSize;
  char* newStr = NULL;
  va_list args;
  
  //Check sanity of inputs and open /dev/null so that we can
  //get the space needed for the new string. There's unfortunately no
  //easier way to do this that uses only ISO functions. Filenames can only
  //be portably specified in terms of the user's codepage as well :(
  if(str == NULL || format == NULL || (pFile = fopen(DEVNULL, "wb")) == NULL) {
    return FALSE;
  }

  //get the space needed for the new string
  va_start(args, format);
  newSize = (size_t)(vfprintf(pFile, format, args)+1); //plus L'\0'
  va_end(args);

  //Create a new block of memory with the correct size rather than using realloc
  //as any old values could overlap with the format string. quit on failure
  if((newStr = (char*)malloc(newSize*sizeof(char))) == NULL) {
    return FALSE;
  }

  fclose(pFile);

  //do the string formatting for real. vsnprintf doesn't seem to be available on Lattice C
  va_start(args, format);
  vsprintf(newStr, format, args);
  va_end(args);

  //ensure null termination of the string
  newStr[newSize] = '\0';

  //free the old contents of the output if present
  free(*str);

  //set the output pointer to the new pointer location
  *str = newStr;
  
  //everything occurred successfully
  return newSize;
}

int getcwd_d(char** buf) {
  char* temp;
  char* temp2;
  size_t length;

  if(buf == NULL) {
    return EFAULT;
  }

  //try calling getcwd with a NULL parameter. some clibs
  //will perform a malloc and return a string of the right size
  errno = 0;
  temp = getcwd(NULL, 0);
  
  if(errno == 0 && temp != NULL) {
    *buf = temp;
    return 0;
  }

  length = 32;
  temp = *buf;
  
  for( ; ; ) {
    if((temp2 = (char *)realloc(temp, length)) == NULL) {
      free(temp);
      *buf = NULL;
      return errno;
    }
    
    temp = temp2;
    
    if(getcwd(temp, length) == NULL) {
      if(errno != ERANGE) {
        free(temp);
        *buf = NULL;
        return errno;
      }
      else {
        length *= 2;
      }
    }
    else {
      if((temp2 = (char *)realloc(temp, strlen(temp)+1)) == NULL) {
        free(temp);
        *buf = NULL;
        return errno;
      }
      
      *buf = temp2;
      return 0;
    }
  }
}

char* getUtcTime(time_t * theTime) {
  char * output = NULL;
  struct tm local;


  //get localtime tm object and utc offset string
  //(we don't want to keep gmtime though as the users clock
  // will probably be set relative to localtime)
  if(tztime_d(theTime, &local, NULL, &output) == FALSE) {
    strFree(&output);

    return NULL;
  };

  //place the utc offset in the output string.
  //%z unfortunately can't be used as it doesn't work properly
  //in some c library implementations (Watcom and MSVC)
  if(sprintf_d(&output, "%%Y-%%m-%%dT%%H:%%M:%%S%s", output) == FALSE) {
    
    strFree(&output);

    return NULL;
  };
  
  //place the rest of the time data in the output string
  if(strftime_d(&output, output, &local) == FALSE) {
    strFree(&output);

    return NULL;
  }

  return output;
} 

struct folder {
  char* path;
  time_t mtime;
  struct folder* next;
};

int main(int argc, char *argv[]) {
  //int argc2;
  //char ** argv2;
  DIR *dirp;
  struct dirent *direntp;
  struct stat buf;
  //char* cwd = NULL;
  char* str = NULL;
  char* fullPath = NULL;
  char* escapedPath = NULL;
  struct folder * first = NULL;
  struct folder * last = NULL;
  struct folder * temp = NULL;

  //argc2 = argc;
  //argv2 = argv;

  //setupWin32(&argc2, &argv2);

  //getcwd_d(&cwd);

  fputs("mtime,size,path,name\n", stdout);

  first = last = (struct folder *)malloc(sizeof(struct folder));
  first->next = NULL;
  first->path = strdup(".");
  
  dirp = opendir(first->path);

  if(dirp != NULL) {
    for( ; ; ) {
      direntp = readdir(dirp);
      
      if(direntp == NULL) {
        closedir(dirp);

        do {
          if(first->next != NULL) {
            temp = first->next;
            free(first->path);
            free(first);
            first = temp;

            strFree(&escapedPath);
            
            if(needsEscaping(first->path, 0)) {
              //this line has do be done first and separately to prevent heap fragmentation
              escapedPath = strReplace("\"","\"\"", first->path);
            
              fprintf(
                stdout,
                "%s,,\"%s\",\"\"\n",
                (str = getUtcTime(&(first->mtime))),
                escapedPath
              );
            }
            else {
              fprintf(
                stdout,
                "%s,,%s,\"\"\n",
                (str = getUtcTime(&(first->mtime))),
                first->path
              );
            }

            strFree(&str);
            
            dirp = opendir(first->path);
          }
          else {
            dirp = NULL;
            break;
          }
        } while(dirp == NULL);

        if(dirp) {
          continue;
        }
        else {
          free(first->path);
          free(first);
          break;
        }
      }

      if(strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0) {
        continue;
      }

      if(statdir(first->path, direntp, &fullPath, &buf) == 0) {
        if(S_ISDIR(buf.st_mode)) {
          temp = (struct folder *)malloc(sizeof(struct folder));
          temp->path = fullPath;
          temp->mtime = buf.st_mtime;
          temp->next = NULL;
          last->next = temp;
          last = temp;
        }
        else {
          strFree(&fullPath);
          str = getUtcTime(&(buf.st_mtime));

          if(escapedPath) {
            fprintf(
              stdout,
              "%s,%d,\"%s\",",
              str,
              buf.st_size,
              escapedPath
            );
          }
          else {
            fprintf(
              stdout,
              "%s,%d,%s,",
              str,
              buf.st_size,
              first->path
            );
          }
            
          strFree(&str);
                        
          if(needsEscaping(direntp->d_name, 0)) {
            fprintf(
                stdout,
                "\"%s\"\n",
                (str = strReplace("\"","\"\"", direntp->d_name))
              );
              
            strFree(&str);
          }
          else {		
            fprintf(stdout, "%s\n", direntp->d_name);
          }
        }
      }
    }
  }

  //free(cwd);

  return EXIT_SUCCESS;
}
