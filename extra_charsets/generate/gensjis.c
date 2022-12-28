/* This program outputs a html file that contains every shift-jis byte sequence. */
/* The resultant file can then be loaded into chrome or firefox and the text copied
 * to the os clipboard then pasted into a utf-8 file to get that browsers shift-jis mapping. */
/* Some of the byte sequences won't have a corresponding unicode character. Those that don't
 * will end up with a U+FFFD codepoint on the line. We can then strip these lines from the output file
 * using "perl -pe 's/[^�\n]*�[^\n]*\n//sg' < sjis.txt | tee sjis2.txt >/dev/null" to get the browsers shift-jis to unicode mapping */
/* I figure if anyone knows the true de-facto shift-jis mapping, its browser vendors. */
/* You may want to edit mappings manually after this */
#include <stdio.h>

int main(int argc, char ** argv) {
  FILE * output;
  int i, j;
  
  output = fopen("sjis.html","wb");
  
  if(output == NULL) {
    return -1;
  }
  
  fputs("<!DOCTYPE html>\n<html><head><meta charset=\"Shift-JIS\" /><body><pre>", output);

   for(i = 1; i < 256; i++) {
    if((i > 0x80 && i < 0xa1) || i > 0xdf) {
      for(j = 0x20; j < 0xff; j++) {
        //16 bit characters
        fprintf(output, "\"%c%c\",%02x,%02x\n", (unsigned char)i, (unsigned char)j, i, j);
      }
    }
    else if (i> 31) {
      //8 bit characters
      fprintf(output, "\"%c\",00,%02x\n", (unsigned char)i, i);    
    }
    else {
      //8 bit characters
      fprintf(output, "\" \",00,%02x\n", (unsigned char)i, i);    
    }
  }

  fputs("</pre></body></html>\n", output);

  fclose(output);
  
  return 0;
}
