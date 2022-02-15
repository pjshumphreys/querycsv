int parse_collate(char* currentLocation) {
  int retval = 0;
  char * result = NULL;

  do {
    d_strtok(&result, " ,", &currentLocation);

    if(result == NULL) {
      return retval;
    }

    if(mystrnicmp(result, "sensitive", 10) == 0) {
      retval |= 2;
      retval &= ~(1);
    }
    else if(mystrnicmp(result, "digits", 7) == 0) {
      retval |= 4;
    }
    else if(mystrnicmp(result, "upper", 6) == 0) {
      retval |= 8;
    }
    else if(mystrnicmp(result, "kana", 5) == 0) {
      retval |= 16;
    }

  } while(1);
}
