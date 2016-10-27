#include "querycsv.h"

void stringGet(
    unsigned char **str,
    struct resultColumnValue *field,
    int params
  ) {

  MAC_YIELD

  if(str != NULL) {
    *str = (unsigned char *)(strdup((char const *)(field->value)));
  }

  /*
  long offset = ftell(*(field->source));

  fseek(*(field->source), field->startOffset, SEEK_SET);
  fflush(*(field->source));

  if (field->isQuoted) {
    //can't use a shortcut to get the string value, so get it the same way we did the last time
    getCsvColumn(field->source,str,&(field->length),NULL,NULL, (params & PRM_TRIM) == 0);
  }
  else {
    //can use a shortcut to get the string value
    reallocMsg("alloc failed", (void**)str, field->length+1);

    if(fread(*str, 1, field->length, *(field->source)) != field->length) {
      fputs("didn't read string properly\n", stderr);
      exit(EXIT_FAILURE);
    }

    (*str)[field->length] = '\0';
  }

  //reset the file offset as we don't know what else the file is being used for
  fseek(*(field->source), offset, SEEK_SET);
  fflush(*(field->source));
  */
}
