#if (defined(MICROSOFT) || defined(__unix__) || defined(__LINUX__)) && !(defined(EMSCRIPTEN) || defined(MPW_C))
int d_fullpath(char* input, char** output) {
  #ifdef MICROSOFT
    char pathSeparator = '\\';
    #define separatorString "\\"
    char driveLetter;
  #else
    char pathSeparator = '/';
    #define separatorString "/"
  #endif
  char * currentLocation = input;
  char * lastSeparator = NULL;

  char * folderName = NULL;
  char * previous = NULL;
  char * result = NULL;
  int fullPath = FALSE;

  /* if input is null then just quit */
  if(input == NULL || input[0] == '\0' || output == NULL) {
    return FALSE;
  }

  /*
    if the path starts with a drive letter and we're on
    windows then just call chdrive and chdir appropriately
  */
  #ifdef MICROSOFT
  if(input[1] == ':') {
    driveLetter = input[0] & 0xdf;

    if(driveLetter < 'A' || driveLetter > 'Z') {
      return FALSE;
    }

    _chdrive(driveLetter - '@');
    currentLocation = &(input[2]);
    folderName = mystrdup("");
    fullPath = TRUE;
  }
  else
  #endif
  if(input[0] == pathSeparator) {
    #ifdef WINDOWS
    if(input[1] == pathSeparator) {
      currentLocation = &(input[2]);
      folderName = mystrdup(separatorString);
    } else
    #endif
    {
      folderName = mystrdup("");
    }

    fullPath = TRUE;
  }

  /* navigate through each path part. return empty file name if any doesn't succeed */
  do {
    d_strtok(&result, separatorString, &currentLocation);

    if(result == NULL) {
      freeAndZero(*output);

      if(fullPath) {
        lastSeparator = strrchr(folderName, pathSeparator);

        if(lastSeparator != NULL) {
          *lastSeparator = '\0';
        }

        if(chdir((folderName[0] == '\0' ? separatorString : folderName))) {
          free(folderName);
          return FALSE;
        }

        if(lastSeparator != NULL) {
          *output = mystrdup(++lastSeparator);
        }
        else {
          *output = mystrdup(folderName);
        }

        free(folderName);
      }
      else {
        *output = previous;
      }

      return TRUE;
    }

    if(strcmp(result, ".") == 0) {
      freeAndZero(previous);
      continue;
    }

    if(
      strcmp(result, "..") == 0
    ) {
      if(fullPath) {
        if(
          strcmp(folderName, separatorString) != 0
          #ifdef WINDOWS
            && strcmp(folderName, "//") != 0
          #endif
        ) {
          lastSeparator = strrchr(folderName, pathSeparator);

          if(lastSeparator != NULL) {
            *lastSeparator = '\0';
          }
        }
      }
      else {
        if(previous) {
          if(chdir(previous)) {
            free(previous);
            free(result);

            return FALSE;
          }
          free(previous);
        }

        previous = result;
        result = NULL;
      }

      continue;
    }

    if(fullPath) {
      d_sprintf(&folderName, "%s" separatorString "%s", folderName, result);
    }
    else {
      if(previous) {
        if(chdir(previous)) {
          free(previous);
          free(result);

          return FALSE;
        }
        free(previous);
      }

      previous = result;
      result = NULL;
    }
  } while(1);

  return FALSE;
}
#endif
