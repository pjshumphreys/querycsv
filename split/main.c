#ifndef __Z88DK
#ifndef ELIM
void logNum(int num) {
  FILE * pFile = fopen("log.txt", "a");

  if(pFile != NULL) {
    fprintf(pFile, "%05d\n", num);
    fclose (pFile);
  }
}
#endif
#endif

int main(int argc, char **argv) {
  int argc2;
  char ** argv2;
  char * temp;
  unsigned int temp3;

  #ifdef __CC65__
    char temp2 = 0;

    temp = petsciiToUtf8(argv[1]);
    strncpy(argv[1], temp, 20);

    /* ensure null termination of the string */
    argv[1][19] = '\0';

    free(temp);

    /* if a comma then a valid drive number appears after the file name then change to that drive */
    if((temp = strchr(argv[1], ',')) != NULL) {
      *temp = '\0';
      temp++;

      if(isdigit(temp[0])) {
        temp2 = temp[0] - '0';

        if(temp[1]) {
          if(isdigit(temp[1]) && !temp[2]) {
            temp2 = (((temp2 << 2) + temp2) << 1) + (temp[1] - '0');
          }
          else {
            temp2 = 0;
          }
        }
      }
    }

    /* use drive 8 by default */
    if(temp2 < 9 || temp2 > 30) {
      temp = "8";
    }

    chdir(temp);

    spinnerEnabled = TRUE;
    cursorOutput = FALSE;
    currentWaitCursor = 0;
  #endif

  argv2 = argv;
  argc2 = argc;

  #ifdef __Z88DK
    setupZ80(&argc2, &argv2);
  #endif

  #ifdef MICROSOFT
    devNull = "NUL";  /* null filename on DOS/Windows */

    #ifdef WINDOWS
      setupWin32(&argc2, &argv2);
    #else
      setupDos();
    #endif
  #else
    #if defined(__unix__) || defined(__LINUX__)
      #ifdef __WATCOMC__
        /* On the linux version of watcom vsnprintf still works as it does
        on WIN32/MSDOS (i.e. it's broken). fall back to the fprintf approach */
        devNull = "/dev/null";
      #endif
    #endif
  #endif

  #ifdef __CC_NORCROFT
    devNull = "null:";

    #if __LIB_VERSION >= 300
      setupRiscOS(&argc2, &argv2);
    #endif
  #endif

  mbcs_data = NULL;
  mbcs_temp = NULL;
  c2b = NULL;
  b2c = NULL;

  /* Identify whether to run a script or display the usage message */
  /*
    The Z88DK compiler (depending on the target runtime) might not call
    atexit registered functions if the main function just returns a value,
    so we have to explicitly call exit instead (*sigh*)
  */
  if(argc2 == 2) {
    exit(runQuery(argv2[1], TRUE));
  }
  else if (argc2 == 3 && mystrnicmp("-c", argv2[1], 3) == 0) {
    exit(runQuery(argv2[2], FALSE));
  }
  else {
    /* something else. print an error message and quit */

    /* get just the file name of the program */
    temp3 = strlen(argv2[0]);
    temp = argv2[0]+temp3-1;

    do {
      switch(*temp) {
        case ':':
        case '\\':
        case PATH_SEPARATOR:
          temp++;
        break;

        default: {
          if(!(--temp3)) {
            break;
          }

          temp--;
        } continue;
      }

      break;
    } while(1);

    /* print the usage message */
    fprintf(stderr, TDB_INVALID_COMMAND_LINE, temp);
    exit(EXIT_FAILURE);
  }

  return EXIT_FAILURE;  /* satisfy pedantic compilers */
}
