int parse_collate(char* currentLocation) {
  int retval = 0;
  char * result = NULL;

  do {
    d_strtok(&result, " ,", &currentLocation);

    if(result == NULL) {
      return retval;
    }

    if(stricmp(result, "lower") == 0) {
      if(!(retval & 2)) {
        retval |= 1;
      }
    }
    else if(stricmp(result, "accents") == 0) {
      retval |= 2;
      retval &= ~(1);
    }
    else if(stricmp(result, "digits") == 0) {
      retval |= 4;
    }
    else if(stricmp(result, "upper") == 0) {
      retval |= 8;
    }

  } while(1);
}