int parse_collate(char* currentLocation) {
  int retval = 0;
  char * result = NULL;

  do {
    d_strtok(&result, " ,", &currentLocation);

    if(result == NULL) {
      return retval;
    }

    if(stricmp(result, "sensitive") == 0) {
      retval |= 1;
    }
    else if(stricmp(result, "digits") == 0) {
      retval |= 4;
    }

  } while(1);
}