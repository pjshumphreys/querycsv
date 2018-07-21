/* strctod strtod, but replace comma with dot first so as to allow european
format numbers to work without changing the locale */
double strctod(const char* str, char** endptr) {
  char * temp;
  double retval;
  
  if(str) {
    temp = strReplace(",", ".", str);
    retval = strtod(temp, endptr);
    free(temp);

    return retval;
  }

  return ctof(0);
}
