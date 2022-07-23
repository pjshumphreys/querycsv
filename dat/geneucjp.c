/* This program outputs a html file that contains every euc-jp byte sequence. */
/* The resultant file can then be loaded into chrome or firefox and the text copied
 * to the os clipboard then pasted into a utf-8 file to get that browsers euc-jp mapping. */
/* Some of the byte sequences won't have a corresponding unicode character. Those that don't
 * will end up with a U+FFFD codepoint on the line. We can then strip these lines from the output file
 * using "perl -pe 's/[^�\n]*�[^\n]*\n//sg' < sjis.txt | tee stripped.txt >/dev/null" to get the browsers euc-jp to unicode mapping */
/* I figure if anyone knows the true de-facto euc-jp mapping, its browser vendors. */
/* You may want to edit mappings manually after this */
#include <stdio.h>

int main(int argc, char ** argv) {
  FILE * output;
  int i, j, k, l;

  output = fopen("output.html","wb");

  if(output == NULL) {
    return -1;
  }

  fputs("<!DOCTYPE html>\n<html><head><meta charset=\"EUC-JP\" /><body><pre>", output);

   for(i = 32; i < 256; i++) {
    if((unsigned char)i > 0x7f) {
      for(j = 0x80; j < 0xff; j++) {
        //16 bit characters
        fprintf(output, "\"%c%c\",0x0,0x0,0x%02x,0x%02x\n", (unsigned char)i, (unsigned char)j, i, j);
      }
    }
    else {
      //8 bit characters
      fprintf(output, "\"%c\",0x0,0x0,0x0,0x%02x\n", (unsigned char)i, i);
    }
  }

  i = 0x8f;
  for(j = 0x80; j < 0xff; j++) {
    for(k = 0x80; k < 0xff; k++) {
      //24 bit characters
      fprintf(output, "\"%c%c%c\",0x0,0x%02x,0x%02x,0x%02x\n", (unsigned char)i, (unsigned char)j, (unsigned char)k, i, j, k);
    }
  }

  fputs("</pre></body></html>\n", output);

  fclose(output);

  return 0;
}
