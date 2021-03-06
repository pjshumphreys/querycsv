#ifndef __Z88DK
/* write a formatted string into a string buffer. allocate/free memory as needed */
/* the zx build has this function in its libc page to minimise page flipping */
int d_sprintf(char **str, char *format, ...) {
  size_t newSize;
  char *newStr = NULL;
  va_list args;

  #ifndef HAS_VSNPRINTF
    FILE *pFile;

    #ifdef HAS_KERNEL_SWI
      int origOutput;
    #endif
  #endif

  MAC_YIELD

  /* Check sanity of inputs */
  if(str == NULL || format == NULL) {
    return FALSE;
  }

  #ifdef HAS_VSNPRINTF
    /* get the space needed for the new string */
    va_start(args, format);
    newSize = (size_t)(vsnprintf(NULL, 0, format, args)); /* plus '\0' */
    va_end(args);
  #else
    /* open /dev/null so that we can get the space needed for the new string. */
    if((pFile = fopen(devNull, fopen_write)) == NULL) {
      #ifdef HAS_KERNEL_SWI
        /*
          disable output on stdout to simulate printing to
          /dev/null so that we can get the space needed for
          the new string without outputting text. */
        origOutput = _kernel_osbyte(236, 0, 0xFF);
        _kernel_osbyte(3, 22, 0);

        /* get the space needed for the new string */
        va_start(args, format);
        newSize = (size_t)(vfprintf(stdout, format, args)); /* plus '\0' */
        va_end(args);

        /* turn on stdout printing again */
        _kernel_osbyte(3, origOutput, 0);
      #else
        return FALSE;
      #endif
    }
    else {
      /* get the space needed for the new string */
      va_start(args, format);
      newSize = (size_t)(vfprintf(pFile, format, args)); /* plus '\0' */
      va_end(args);

      /* close the file. We don't need to look at the return code as we were writing to /dev/null */
      fclose(pFile);
    }
  #endif

  /* Create a new block of memory with the correct size rather than using realloc */
  /* as any old values could overlap with the format string. quit on failure */
  reallocMsg((void**)&newStr, newSize + 1);

  /* do the string formatting for real. */
  va_start(args, format);

  /* use vsprintf if we must as vsnprintf doesn't seem to be available on Lattice C */
  vsprintf(newStr, format, args);

  va_end(args);

  /* ensure null termination of the string */
  newStr[newSize] = '\0';

  /* free the old contents of the output if present */
  free(*str);

  /* set the output pointer to the new pointer location */
  *str = newStr;

  /* everything occurred successfully */
  return newSize;
}
#endif
