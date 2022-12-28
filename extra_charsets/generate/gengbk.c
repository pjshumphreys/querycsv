/* This program outputs a html file that contains every gbk byte sequence. */
/* The resultant file can then be converted to a utf-8 file using iconv to get the gbk mapping. */
/* Some of the byte sequences won't have a corresponding unicode character. Those that don't
 * will end up with a U+FFFD codepoint on the line. We can then strip these lines from the output file
 * using "perl -pe 's/[^�\n]*�[^\n]*\n//sg' < gbk.txt | tee gbk2.txt >/dev/null" to get the browsers gbk to unicode mapping */
/* I figure if anyone knows the true de-facto gbk mapping, its iconv. */
/* You may want to edit mappings manually after this */
#include <stdio.h>

int main(int argc, char ** argv) {
  FILE * output;
  int i, j, k, l;

  output = fopen("gbk.txt","w");

  if(output == NULL) {
    return -1;
  }

  //fputs("<!DOCTYPE html>\n<html><head><meta charset=\"GB18030\" /><body><pre>", output);

   for(i = 1; i < 256; i++) {
    if((unsigned char)i > 0x80 && (unsigned char)i < 0xff) {
      for(j = 0x30; j < 0xfe; j++) {
        if((unsigned char)j == 0x7f) {
          //not valid
        }
        else if((unsigned char)j > 0x39) {
          //16 bit characters
          fprintf(output, "\"%c%c\",00,00,%02x,%02x\n", (unsigned char)i, (unsigned char)j, i, j);
        }
        else for(k = 0x81; k < 0xff; k++) {
          for(l = 0x30; l < 0x40; l++) {
            //32 bit characters
            fprintf(output, "\"%c%c%c%c\",%02x,%02x,%02x,%02x\n", (unsigned char)i, (unsigned char)j, (unsigned char)k, (unsigned char)l, i, j, k, l);
          }
        }
      }
    }
    else if (i > 31) {
      //8 bit characters
      fprintf(output, "\"%c\",00,00,00,%02x\n", (unsigned char)i, i);
    }
    else {
      //8 bit characters
      fprintf(output, "\" \",00,00,00,%02x\n", (unsigned char)i, i);
    }
  }

  i = 0x8f;
  for(j = 0x80; j < 0xff; j++) {
    for(k = 0x80; k < 0xff; k++) {
      //24 bit characters
      fprintf(output, "\"%c%c%c\",%02x,%02x,%02x\n", (unsigned char)i, (unsigned char)j, (unsigned char)k, i, j, k);
    }
  }

  //fputs("</pre></body></html>\n", output);

  fclose(output);

  return 0;
}
