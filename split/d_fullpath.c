#if (defined(MICROSOFT) || defined(__unix__) || defined(__LINUX__)) || defined(CPM) || defined(MSX) && !(defined(EMSCRIPTEN) || defined(MPW_C))
int d_fullpath(char* input, char** output) {
  #if defined(CPM) || defined(MSX)
    char driveLetter;
    char* lastSeparator = NULL;
    size_t lastSeparatorLength = 0;

    lastSeparator = strrchr(input, '\\');

    if(lastSeparator != NULL) {
      *output = mystrdup(lastSeparator + 1);

      lastSeparatorLength = (int)(lastSeparator - input);

      origWd = malloc(lastSeparatorLength + 1);

      strncpy(origWd, input, lastSeparatorLength);
      origWd[lastSeparatorLength] = '\0';
    }
    else if(input[1] == ':') {
      driveLetter = input[0] & 0xdf;

      if(driveLetter > '@' && driveLetter < '[') {
        *output = mystrdup(input + 2);

        origWd = malloc(3);

        origWd[0] = driveLetter;
        origWd[1] = ':';
        origWd[2] = '\0';
      }
    }
    else {
      *output = mystrdup(input);
      origWd = NULL;
    }

    return TRUE;
  #else
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

        #ifdef MICROSOFT
          _chdrive(driveLetter - '@');
        #else
          bdos(CPM_LGIN, driveLetter - 'A');
        #endif
        currentLocation = &(input[2]);
        folderName = mystrdup("");
        fullPath = TRUE;
      }
      else
    #endif

    if(input[0] == pathSeparator) {
      #if defined(MICROSOFT) || defined(CPM) || defined(MSX)
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
  #endif
}
#endif
