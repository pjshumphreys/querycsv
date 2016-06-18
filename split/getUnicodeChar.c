#include "querycsv.h"

long getUnicodeChar(unsigned char **offset, unsigned char **str, int plusBytes, int *bytesMatched, void (*get)())
{
  struct hash2Entry* entry = NULL;
  int bytesread = 0;
  long codepoint;
  unsigned char *temp = (unsigned char *)((*offset) + plusBytes);

  //if the current byte offset is a valid utf-8 character that's not overlong or decomposable then return it
  if(*temp < 0x80) {
    //read 1 byte. no overlong checks needed as a 1 byte code can
    //never be overlong, and is never a combining character
    *bytesMatched = 1;
    
    return (long)(*temp);
  }
  //ensure the current byte is the start of a valid utf-8 sequence
  else if(*temp > 0xC1) {
    if (*temp < 0xE0) { 
      //read 2 bytes
      if(
          (*(temp+1) & 0xC0) == 0x80
      ) {
        codepoint = ((long)(*(temp)) << 6) + *(temp+1) - 0x3080;
        
        //the codepoint is valid. but is it decomposable?
        if((entry = isInHash2(codepoint))) {
          bytesread = 2;
        }
        else {
          //otherwise return it
          *bytesMatched = 2;
          
          return codepoint;
        }
      }
    }
    else if (*temp < 0xF0) {
      //read 3 bytes
      if(
          (*(temp+1) & 0xC0) == 0x80 &&
          (*(temp) != 0xE0 || *(temp+1) > 0x9F) &&
          (*(temp+2) & 0xC0) == 0x80
      ) {
        codepoint = ((long)(*(temp)) << 12) + ((long)(*(temp+1)) << 6) + (*(temp+2)) - 0xE2080;

        //the codepoint is valid. but is it decomposable?
        if((entry = isInHash2(codepoint))) {
          bytesread = 3;
        }
        else {
          //otherwise return it
          *bytesMatched = 3;
          
          return codepoint;
        }
      }
    }
    else if (*temp < 0xF5) {
      //read 4 bytes
      if(
          (*(temp+1) & 0xC0) == 0x80 &&
          (*temp != 0xF0 || *(temp+1) > 0x8F) &&
          (*temp != 0xF4 || *(temp+1) < 0x90) &&
          (*(temp+2) & 0xC0) == 0x80 &&
          (*(temp+3) & 0xC0) == 0x80
      ) {
        codepoint = (((long)(*temp)) << 18) + ((long)(*(temp+1)) << 12) + ((long)(*(temp+2)) << 6) + (*(temp+3)) - 0x3C82080;

        //the codepoint is valid. but is it decomposable?
        if((entry = isInHash2(codepoint))) {
          bytesread = 4;
        }
        else {
          //otherwise return it
          *bytesMatched = 4;
          
          return codepoint;
        }
      }
    }
  }

  //switch to the fast getUnicodeChar
  get = (void (*)())&getUnicodeCharFast;

  //invalid bytes or overlong and decomposable codepoints mean the string needs to be NFD normalized.
  return normaliseAndGet(offset, str, plusBytes, bytesMatched, bytesread, entry);
}

