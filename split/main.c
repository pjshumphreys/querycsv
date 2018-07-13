int main(int argc, char *argv[]) {
  int argc2;
  char ** argv2;

  argc2 = argc;
  argv2 = argv;

  #ifdef MICROSOFT
    devNull = "NUL";  /* null filename on DOS/Windows */

    #ifdef WINDOWS
      setupWin32(&argc2, &argv2);
    #else
      /* Watcom StdClib on MSDOS needs the TZ environment variable set
      then setlocale to be called to properly calculate gmtime */

      /* supply some default timezone data if none is present */
      if(getenv("TZ") == NULL) {
        putenv(TDB_DEFAULT_TZ);
      }

      /* set the locale (among other things, this applies the */
      /* timezone data to the date functions) */
      setlocale(LC_ALL, TDB_LOCALE);
    #endif
  #else
    #ifdef __WATCOMC__
      /* On the linux version of watcom vsnprintf still
      works as it does on WIN32/MSDOS (i.e. it's broken). fall back to the
      fprintf approach */
      devNull = "/dev/null";
    #endif

    #ifdef __CC_NORCROFT
      devNull = "null:";

      #if __LIB_VERSION >= 300
        setupRiscOS(&argc2, &argv2);
      #endif
    #endif
  #endif

  /* identify whether to run a script or display the usage message */
  if(argc2 == 2) {
    return runQuery(argv2[1]);
  }

  /* something else. print an error message and quit */
  fputs(TDB_INVALID_COMMAND_LINE, stderr);

  return EXIT_FAILURE;
}
