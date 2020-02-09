/* This program outputs a html file that outputs every shift-jis byte sequence. */
/* The resultant file can then be loaded into chrome or firefox and the text copied
 * to the os clipboard then pasted into a utf-8 file to get that browsers shift-jis mapping. */
/* Some of the byte sequences won't have a corresponding unicode character. Those that don't
 * will end up with a U+FFFD codepoint on the line. We can the strip them from the output file
 * using "grep -v [U+FFFD] utf8.txt > stripped.txt" to get the browsers shift-jis to unicode mapping */
/* I figure if anyone knows the true de-facto shift-jis mapping, its browser vendors. */
/* You may want to edit mappings manually after this */
#include <stdio.h>

int main(int argc, char ** argv) {
  FILE * output;
  int i, j;
  
  output = fopen("output.html","wb");
  
  if(output == NULL) {
    return -1;
  }
  
  fputs("<!DOCTYPE html>\n<html><head><meta charset=\"Shift-JIS\" /><body><pre>", output);

   for(i = 32; i < 256; i++) {
    if((i > 0x80 && i < 0xa0) || (i > 0xdf && i < 0xfd)) {
      for(j = 0x40; j < 0xfd; j++) {
        if(j != 0x7f) {
          //16 bit characters
          fprintf(output, "\"%c%c\",0x%02x,0x%02x\n", (unsigned char)i, (unsigned char)j, i, j);
        }
      }
    }
    else if (i == 0x2c) {
    }
    else {
      //8 bit characters
      fprintf(output, "\"%c\",0x0,0x%02x\n", (unsigned char)i, i);    
    }
  }

  fputs("</pre></body></html>\n", output);

  fclose(output);
  
  return 0;
}
