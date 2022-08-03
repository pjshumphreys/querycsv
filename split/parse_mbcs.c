#ifdef __Z88DK
  int sortCodepoints(const void* a, const void* b) {
    if(((*(unsigned int *)a)) < ((*(unsigned int *)b))) {
      return -1;
    }

    return ((*(unsigned int *)a)) > ((*(unsigned int *)b));
  }

  int sortBytes(const void* a, const void* b) {
    int i = 0;
    unsigned char * c = &(((struct lookup*)a)->bytes[0]);
    unsigned char * d = &(((struct lookup*)b)->bytes[0]);

    for(; i < mbcs_trailing; i++) {
      if(*c != *d) {
        return *c < *d ? -1 : 1;
      }

      c++;
      d++;
    }

    return 0;
  }
#else
  int sortCodepoints(const void* a, const void* b) {
    if((*(*(unsigned int **)a)) < (*(*(unsigned int **)b))) {
      return -1;
    }

    return (*(*(unsigned int **)a)) > (*(*(unsigned int **)b));
  }

  int sortBytes(const void* a, const void* b) {
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
#endif

void parse_mbcs(char * name) {
  FILE * input;
  int found;
  int c;

  int temp;
  int state = 0;
  int currentResult = 0;

  unsigned char * current;

  /* open the file */
  input = fopen(name, "r");

  if(input == NULL) {
    freeAndZero(mbcs_data);
    freeAndZero(c2b);
    freeAndZero(b2c);
    freeAndZero(mbcs_temp);
    mbcs_length = 0;
    mbcs_trailing = 0;
    mbcs_size = 1;
    return;
  }

  /* read file length */
  getHexNumber(input, mbcs_length, found, c);

  /* read how many trailing bytes there are */
  getHexNumber(input, mbcs_trailing, found, c);

  mbcs_size = (((sizeof(unsigned int) + mbcs_trailing) / sizeof(unsigned int)) + 1) * sizeof(unsigned int);

  reallocMsg((void **)(&mbcs_data), mbcs_length * mbcs_size);
  reallocMsg((void **)(&c2b), mbcs_length * sizeof(struct lookup *));
  reallocMsg((void **)(&b2c), mbcs_length * sizeof(struct lookup *));
  reallocMsg((void **)(&mbcs_temp), mbcs_size);

  current = mbcs_data;

  do {
    getHexNumber(input, temp, found, c);

    if(state == 0) {
      ((struct lookup **)b2c)[currentResult] = (struct lookup *)current;
      ((struct lookup **)c2b)[currentResult] = (struct lookup *)current;

      /* store read number as the unicode codepoint */
      *((unsigned int *)current) = temp;
      current += sizeof(unsigned int);
    }
    else {
      /* store the number as the n-1th trailing byte */
      *((unsigned char*)current) = temp;
      current++;
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

  fclose(input);
}
