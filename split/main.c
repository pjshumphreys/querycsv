int main(int argc, char **argv) {
  int argc2;
  char ** argv2;

  #ifdef __CC65__
    char * temp;

    temp = petsciiToUtf8(argv[1]);
    strncpy(argv[1], temp, 199);

    /* ensure null termination of the string */
    argv[1][199] = '\0';

    free(temp);
  #endif

  #ifdef __Z88DK
    int start;

    /* initialise variables needed by z88dk's libc */
    myhand_status = 3;

    /* initialise the heap so malloc and free will work */
    mallinit();
    sbrk(main_origins[libCPage], main_sizes[libCPage]); /* lib c variant specific free ram. All variants permit at least some */

    if(argv[1] != NULL) {
      start = (int)(argv[1]) + strlen(argv[1]) + 10;
      sbrk(start, 44032 /* 0xc000 - 5kb */ - start); /* free ram from the end of the a$ variable up to the paging code minus about 2 kb for stack space */
    }
  #endif

  argc2 = argc;
  argv2 = argv;

  yydebug = 1;

  #ifdef MICROSOFT
    devNull = "NUL";  /* null filename on DOS/Windows */

    #ifdef WINDOWS
      setupWin32(&argc2, &argv2);
    #elif defined(__WATCOMC__)
      /* Watcom StdClib on MSDOS needs the TZ environment variable set
      then setlocale to be called to properly calculate gmtime */

      /* supply some default timezone data if none is present */
      if(getenv("TZ") == NULL) {
        putenv(TDB_DEFAULT_TZ);
      }

      /* set the locale (among other things, this applies the */
      /* timezone data to the date functions) */
      setlocale(LC_ALL, TDB_LOCALE);

      /* store the original working directory so we can load qrycsv00.ovl properly */
      origWd = getcwd(NULL, PATH_MAX+1);

      #ifdef DOS_DAT
        /* open the hash2 data file on startup */
        openDat();
      #endif

      /* set the working directory back to its original value at exit */
      atexit(atexit_dos);
    #endif
  #endif

  #if defined(__unix__) || defined(__LINUX__)
    #ifdef __WATCOMC__
      /* On the linux version of watcom vsnprintf still
      works as it does on WIN32/MSDOS (i.e. it's broken). fall back to the
      fprintf approach */
      devNull = "/dev/null";
    #endif

    /* for unix, setlocale makes sure ncursesw is used */
    setlocale(LC_ALL, TDB_LOCALE);
  #endif

  #ifdef __CC_NORCROFT
    devNull = "null:";

    #if __LIB_VERSION >= 300
      setupRiscOS(&argc2, &argv2);
    #endif
  #endif

  /* identify whether to run a script or display the usage message */
  if(argc2 == 2) {
    return runQuery(argv2[1], TRUE);
  }
  else if (argc2 == 3 && strcmp("-c", argv2[1]) == 0) {
    return runQuery(argv2[2], FALSE);
  }

  /* something else. print an error message and quit */
  fputs(TDB_INVALID_COMMAND_LINE, stderr);

  return EXIT_FAILURE;
}
