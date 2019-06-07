int parse_collate(char* currentLocation) {
  int retval = 0;
  char * result = NULL;

  for(;;) {
    d_strtok(&result, " ,", &currentLocation);

    if(result) {
      if(stricmp(result, "sensitive") == 0) {
        retval |= 1;
      }
      else if(stricmp(result, "digits") == 0) {
        retval |= 4;
      }

      continue;
    }

    return retval;
  }
}