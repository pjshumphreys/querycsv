int sortCodepoints(const void* a, const void* b) __z88dk_params_offset(-4) {
  QCSV_LONG c = (*(*(QCSV_LONG **)a));
  QCSV_LONG d = (*(*(QCSV_LONG **)b));

  if(c < d) {
    return -1;
  }

  return (c > d);
}

int sortBytes(const void* a, const void* b) __z88dk_params_offset(-4) {
  int i = 0;
  unsigned char * c = &((*(struct lookup**)a)->bytes[0]);
  unsigned char * d = &((*(struct lookup**)b)->bytes[0]);

  for(; i < mbcs_trailing; i++) {
    if(*c != *d) {
      return *c < *d ? -1 : 1;
    }

    c++;
    d++;
  }

  return 0;
}

void parse_mbcs(char * name) {
  FILE * input;
  int found;
  int c;

  unsigned int temp;
  int state = -2;
  int currentResult = 0;

  unsigned char * current;

  /*
    Only one mbcs can be loaded at any one time
    (they're a pain to process beyond conversion to and from unicode).
    Cleanup any data that may have been used before
  */
  freeAndZero(mbcs_data);
  freeAndZero(c2b);
  freeAndZero(b2c);
  freeAndZero(mbcs_temp);
  mbcs_length = 0;
  mbcs_trailing = 0;
  mbcs_size = 1;

  /* open the file */
  input = fopen(name, fopen_read);

  if(input == NULL) {
    fprintf(stderr, TDB_COULDNT_OPEN_MBCS, name);
    exit(EXIT_FAILURE);
  }

  do {
    /* Get a hex number from the file */
    temp = 0;
    found = 0;

    do {
      c = fgetc(input);

      if(c <= '9' && c >= '0') {
        temp = (temp << 4) + (c - '0');
        found = 1;
      }
      else if(c <= 'F' && c >= 'A') {
        temp = (temp << 4) + (c - 'A') + 10;
        found = 1;
      }
      else if(c <= 'f' && c >= 'a') {
        temp = (temp << 4) + (c - 'a') + 10;
        found = 1;
      }
      else if(c == EOF || found == 1) {
        break;
      }
    } while(1);

    switch(state) {
      case -2: {
        /* read file length */
        mbcs_length = temp;
        state = -1;
      } continue;

      case -1: {
        /* read how many trailing bytes there are */
        mbcs_trailing = temp;

        mbcs_size = (((sizeof(QCSV_LONG) + mbcs_trailing) / sizeof(QCSV_LONG)) + 1) * sizeof(QCSV_LONG);

        reallocMsg((void **)(&mbcs_data), mbcs_length * mbcs_size);
        reallocMsg((void **)(&c2b), mbcs_length * sizeof(struct lookup *));
        reallocMsg((void **)(&b2c), mbcs_length * sizeof(struct lookup *));
        reallocMsg((void **)(&mbcs_temp), mbcs_size);

        current = mbcs_data;
        state = 0;
      } continue;

      case 0: {
        ((struct lookup **)b2c)[currentResult] = (struct lookup *)current;
        ((struct lookup **)c2b)[currentResult] = (struct lookup *)current;

        /* store read number as the unicode codepoint */
        *((unsigned int *)current) = temp;
        current += sizeof(QCSV_LONG);
      } break;

      default: {
        /* store the number as the n-1th trailing byte */
        *((unsigned char*)current) = temp;
        current++;
      } break;
    }

    if(state == mbcs_trailing) {
      current = (unsigned char *)(((struct lookup **)b2c)[currentResult]) + mbcs_size;
      currentResult++;

      if(currentResult == mbcs_length) {
        break;
      }

      state = 0;
    }
    else {
      state++;
    }
  } while(1);

  fclose(input);

  qsort(
    c2b,
    mbcs_length,
    sizeof(struct lookup *),
    sortCodepoints
  );

  qsort(
    b2c,
    mbcs_length,
    sizeof(struct lookup *),
    sortBytes
  );
}
