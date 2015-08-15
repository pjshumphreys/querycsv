#include "strcomp.h"

int hashCompare(const void* a, const void* b) {
  return (*((long*)a) < *((long*)b)) ? -1 :
		(*((long*)a) != *((long*)b) ? 1 : 0);
}

int isCombining(long codepoint) {
  struct hash3Entry * result;

  if(
      codepoint < 0x300 ||
      (
        codepoint > 0x309A && (
          codepoint < 0xA66F || (
            codepoint < 0xFB1E &&
            codepoint > 0xABED
          )
        )
      )
  ) {
    return 0;
  }

  result = (struct hash3Entry*)bsearch(
      (const void*)&codepoint,
      (const void*)&hash3EntryMap,
      745,
      sizeof(struct hash3Entry),
      &hashCompare
    );

  return (result == NULL ? 0 : result->order);
}

struct hash2Entry* isinHash2(long codepoint) {

  #include "hash2.h"

  if(
      codepoint < 0xA0 ||
      (
        codepoint > 0x33FF && (
          codepoint < 0xA69C || (
            codepoint < 0xF900 &&
            codepoint > 0xAB5F
          )
        )
      )
  ) {
    return NULL;
  }

  return (struct hash2Entry*)bsearch(
      (const void*)&codepoint,
      (const void*)&hash2,
      5730,
      sizeof(struct hash2Entry),
      &hashCompare
    );
}

int strAppendUTF8(long codepoint, unsigned char ** nfdString, int nfdLength) {
  if (codepoint < 0x80) {
    reallocMsg("realloc failure\n", (void**)nfdString, nfdLength+1);

    (*nfdString)[nfdLength++] = codepoint;
  }
  else if (codepoint < 0x800) {
    reallocMsg("realloc failure\n", (void**)nfdString, nfdLength+2);

    (*nfdString)[nfdLength++] = (codepoint >> 6) + 0xC0;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }
  else if (codepoint < 0x10000) {
    reallocMsg("realloc failure\n", (void**)nfdString, nfdLength+3);

    (*nfdString)[nfdLength++] = (codepoint >> 12) + 0xE0;
    (*nfdString)[nfdLength++] = ((codepoint >> 6) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }
  else {
    reallocMsg("realloc failure\n", (void**)nfdString, nfdLength+4);

    (*nfdString)[nfdLength++] = (codepoint >> 18) + 0xF0;
    (*nfdString)[nfdLength++] = ((codepoint >> 12) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = ((codepoint >> 6) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }

  return nfdLength;
}

//getUnicodeCharFast does not check for invalid or overlong bytes.
//it also presumes the the entire string is already in nfd form
long getUnicodeCharFast(
	unsigned char **offset,
	unsigned char **str,
    int plusBytes,
    int* bytesMatched,
    void (*get)()
  ) {

  unsigned char *temp = (unsigned char *)(*(offset+plusBytes));
  
  if(*temp < 0x80) {
    //return the information we obtained
    *bytesMatched = 1;
    
    return (long)(*temp);
  }
  else if(*temp < 0xE0) {
    //read 2 bytes
    *bytesMatched = 2;
    
    return (long)((*(temp) << 6) + *(temp+1)) - 0x3080;
  }
  else if (*temp < 0xF0) {
    //read 3 bytes
    *bytesMatched = 3;
    
    return ((long)(*temp) << 12) + ((long)(*(temp+1)) << 6) + (long)(*(temp+2)) - 0xE2080;
  }
    
  //read 4 bytes
  *bytesMatched = 4;
  
  return ((long)(*temp) << 18) + ((long)(*(temp+1)) << 12) + ((long)(*(temp+2)) << 6) + (*(temp+3)) - 0x3C82080;
}

int normaliseAndGet(
    unsigned char **offset,
    unsigned char **str,
    int plusBytes,
    int *bytesMatched,
    int bytesRead,
    struct hash2Entry* entry
  ) {
  int offsetInt = *offset - *str;
  unsigned char * nfdString = NULL;
  int nfdLength = offsetInt, i = 0, j; 

  long * codepointBuffer = NULL;
  int bufferLength;

  long codepoint;
  struct hash1Entry const * lookupresult;

  //if the allocation failed, print an error messge and exit
  reallocMsg(TDB_MALLOC_FAILED, (void**)&nfdString, strlen(*str)+1);

  //copy the string matched up to now directly into the output string
  memcpy(nfdString, (void*)(*str), offsetInt);

  //if we've done the work of finding an entry then we should make use of it to populate the codepoint array initially
  if(plusBytes != 0 || entry == NULL) {
    bufferLength = 0;
  }
  else {
    bufferLength = entry->length;
    reallocMsg("realloc failure\n", (void**)&codepointBuffer, bufferLength*sizeof(long));
    memcpy(codepointBuffer, (void*)(entry->codepoints), bufferLength*sizeof(long));
    entry = NULL;
    *offset += bytesRead;
  }

  for( ; ; ) {  //the get a codepoint code will break out of this loop
    //while there are still unexamined codepoints in the buffer
    while(i != bufferLength) {
      if(isCombining(codepointBuffer[i]) == 0) {
        //if i=0 then there were no combining characters at the head of the buffer
        //if i=1 then there is only 1 combining character at the head of the
        //buffer. therefore it doesn't need sorting
        if(i > 1) { 
          //TODO: qsort the combining characters that preceed this codepoint
        }

        //output all the codepoints up to and including this one as utf-8 sequences
        for(j = 0; j <= i; j++) {
          nfdLength = strAppendUTF8(codepointBuffer[j], &nfdString, nfdLength);
        }
        
        bufferLength-=++i;
        memmove((void*)codepointBuffer, (void*)&codepointBuffer[i], bufferLength*sizeof(long)); //downward in memory i number of codepoints
        i = 0;
      }
      else {
        i++;
      }
    }

    //if bytesread is 0 then we are expecting trailing bytes
    //from an invalid or overlong codepoint, but ensure the
    //next byte is still a trailing byte first
    if(bytesRead == 0 && ((**offset) & 0xC0) == 0x80 && plusBytes == 0) {
      //do another lookup using hash1
      lookupresult = &hash1[(*((*offset)++))-128];

      reallocMsg("realloc failure\n", (void**)&codepointBuffer, (bufferLength+(lookupresult->length))*sizeof(long));
      memcpy(codepointBuffer+(bufferLength*sizeof(long)), (void*)(lookupresult->codepoints), (lookupresult->length)*sizeof(long));
      bufferLength += lookupresult->length;
    }
    //otherwise we are expecting to be able to read a codepoint 
    else {
      //try getting a unicode code point
      //if it's invalid or overlong, turn on byte reading mode. and read the first byte using hash 1
      //otherwise check if it's decomposable.
      //if it's decomposable, put the entire sequence into the buffer then continue
      //otherwise just put the codepoint into the buffer then continue

      plusBytes = 0;
      bytesRead = 0;

      if(**offset < 0x80) {
        //if offset is 0 then we've reached the end of the string. quit the do loop
        if(**offset == 0) {
          break;
        }
        
        bytesRead = 1;
                
        //read 1 byte. no overlong checks needed as a 1 byte code can
        //never be overlong, and is never a combining character
        reallocMsg("realloc failure\n", (void**)&codepointBuffer, (1+bufferLength)*sizeof(long));
        codepointBuffer[bufferLength++] = (long)(*((*offset)++));

        continue;
      }
      
      //ensure the current byte is the start of a valid utf-8 sequence
      if(**offset > 0xC1) {
        if (**offset < 0xE0) { 
          //read 2 bytes
          if(
              (*((*offset)+1) & 0xC0) == 0x80
          ) {
            bytesRead = 2;
            codepoint = ((long)(*((*offset)++)) << 6) + (*((*offset)++)) - 0x3080;            
          }
        }
        else if (**offset < 0xF0) {
          //read 3 bytes
          if(
              (*((*offset)+1) & 0xC0) == 0x80 &&
              (*(*offset) != 0xE0 || *((*offset)+1) > 0x9F) &&
              (*((*offset)+2) & 0xC0) == 0x80
          ) {
            bytesRead = 3;
            codepoint = ((long)(*((*offset)++)) << 12) + ((long)(*((*offset)++)) << 6) + (*((*offset)++)) - 0xE2080;
          }
        }
        else if (**offset < 0xF5) {
          //read 4 bytes
          if(
              (*((*offset)+1) & 0xC0) == 0x80 &&
              (*(*offset) != 0xF0 || *((*offset)+1) > 0x8F) &&
              (*(*offset) != 0xF4 || *((*offset)+1) < 0x90) &&
              (*((*offset)+2) & 0xC0) == 0x80 &&
              (*((*offset)+3) & 0xC0) == 0x80
          ) {
            bytesRead = 4;
            codepoint = ((long)(*((*offset)++)) << 18) + ((long)(*((*offset)++)) << 12) + ((long)(*((*offset)++)) << 6) + (*((*offset)++)) - 0x3C82080;
          }
        }
        else {
          //use hash 1 to convert it
          lookupresult = &hash1[(*((*offset)++))-128];

          //put the whole byte sequence into the buffer
          reallocMsg("realloc failure\n", (void**)&codepointBuffer, (bufferLength+(lookupresult->length))*sizeof(long));
          memcpy(codepointBuffer+(bufferLength*sizeof(long)), (void*)(lookupresult->codepoints), (lookupresult->length)*sizeof(long));
          bufferLength += lookupresult->length;

          continue;
        }
      }
      else {
        //use hash 1 to convert it
        lookupresult = &hash1[(*((*offset)++))-128];

        //put the whole byte sequence into the buffer
        reallocMsg("realloc failure\n", (void**)&codepointBuffer, (bufferLength+(lookupresult->length))*sizeof(long));
        memcpy(&(codepointBuffer[bufferLength]), (void*)(lookupresult->codepoints), (lookupresult->length)*sizeof(long));
        bufferLength += lookupresult->length;

        continue;
      }

      if(bytesRead != 0) {
        if((entry = isinHash2(codepoint)) == NULL) {
          //the codepoint we found was not decomposable. just put it in the buffer
          reallocMsg("realloc failure\n", (void**)&codepointBuffer, (1+bufferLength)*sizeof(long));
          codepointBuffer[bufferLength] = codepoint;
          bufferLength += 1;
        }
        else {
          //a decomposable codepoint was found in hash 2.

          //put the whole byte sequence into the buffer
          reallocMsg("realloc failure\n", (void**)&codepointBuffer, (bufferLength+(entry->length))*sizeof(long));
          memcpy(&(codepointBuffer[bufferLength]), (void*)(entry->codepoints), (entry->length)*sizeof(long));
          bufferLength += entry->length;
          entry = NULL;
        }
      }
    }
  }

  if(i > 1) { 
    //TODO: qsort the combining characters that preceed this codepoint
  }

  //output the rest of the codepoints (which will all be combining characters)
  for(j = 0; j < i; j++) {
    nfdLength = strAppendUTF8(codepointBuffer[j], &nfdString, nfdLength);
  }

  //append null to the string
  strAppendUTF8(0, &nfdString, nfdLength);

  //free the codepoint buffer
  free(codepointBuffer);
  
  //swap out the string that will be searched from now on. free the old version
  free(*str);
  *str = nfdString;
  (*offset) = (unsigned char *)((*str) + offsetInt);

  //the whole string has been normalized to nfd form.
  //now use the fast version to get the next codepoint
  return getUnicodeCharFast(offset, str, plusBytes, bytesMatched, (void (*)())getUnicodeCharFast);
}

long getUnicodeChar(unsigned char **offset, unsigned char **str, int plusBytes, int *bytesMatched, void (*get)()) {
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
        if((entry = isinHash2(codepoint))) {
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
        if((entry = isinHash2(codepoint))) {
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
        if((entry = isinHash2(codepoint))) {
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

struct hash4Entry * getLookupTableEntry(
    unsigned char **offset,
    unsigned char **str,
    int *lastMatchedBytes,
    void (*get)()
  ) {
  struct hash4Entry * temp = NULL, *temp2 = NULL;
  int totalBytes = 0;
  
  while((temp = in_word_set(*offset, totalBytes+(*lastMatchedBytes)))) {
    //the match is so far holding up. 

    //keep this match for later as it may be the last one we find
    temp2 = temp;

    //add the byte length to the total
    totalBytes += *lastMatchedBytes;
    
    //get a code point
    (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get))
    (offset, str, totalBytes, lastMatchedBytes, get);
  } 

  //don't update the value passed to us if we didn't find any match at all
  if(temp2 != NULL) {
    //copy the match data into the output
    *lastMatchedBytes = totalBytes;
  }
  
  return temp2;
}

int strCompare(unsigned char **str1, unsigned char **str2, int caseSensitive, void (*get1)(), void (*get2)()) {
  unsigned char *offset1 = *str1, *offset2 = *str2;
  long char1 = 0, char2 = 0;
  double dbl1 = 0, dbl2 = 0;
  struct hash4Entry *entry1, *entry2;
  int firstChar = TRUE, comparison = 0, char1found = FALSE;
  int bytesMatched1 = 0, bytesMatched2 = 0;
  int accentcheck = 0, combiner1, combiner2, skip1, skip2;

  for( ; ; ) {  //we'll quit from this function via other means
    //check if we've reached the end of string 2
    if (*offset2 == 0) {
      //if string2 is on the slower version of getUnicodeChar,
      //there were no weird characters, so we can switch to always using the faster version 

      //if they both are null then the strings are equal. otherwise string 2 is lesser
      if(*offset1 == 0) {
        if(accentcheck == 0) {
          return 0;
        }
        else {
          //a difference just on the accents on a letter were found. re-compare with accent checking enabled.
          accentcheck = 2;
          offset1 = *str1;
          offset2 = *str2;
          continue;
        }
      }
      
      return 1;
    }
    
    //check if we've reached the end of string 1
    else if(*offset1 == 0) {
      //if they both are null then the strings are equal. otherwise string 1 is lesser
      return -1;
    }
    
    //check if the current characters of each string are ascii
    else if(*offset2 < 128 && *offset1 < 128) {
      //compare letters
      if(
          *offset1 >= 'A' && *offset1 <= 'z' &&
          (*offset1 >= 'a' || *offset1 <= 'Z')
      ) {
        if(
            *offset2 >= 'A' && *offset2 <= 'z' &&
            (*offset2 >= 'a' || *offset2 <= 'Z')
        ) {
          //both characters are alphabetical, however we still need to honour the list in hash4.
          //therefore we need to do unicode checking, but we do know that nether of the characters to
          //compare is 0x34F. In addition, we may already have data associated with the
          //first string which we can re-use

          //read a character from string 2
          char2 = (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))(&offset2, str2, 0, &bytesMatched2, get2);
         
          if(char1found == 0) {
            char1 = (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get1))(&offset1, str1, 0, &bytesMatched1, get1);
          }

          if(entry1 = getLookupTableEntry(&offset1, str1, &bytesMatched1, get1)) {
            if (entry2 = getLookupTableEntry(&offset2, str2, &bytesMatched2, get2)) {

              //compare the lookup table entries
              if(entry1->script == entry2->script) {
                if(caseSensitive) {
                  comparison = entry1->index - entry2->index;
                }
                else {
                  comparison = (entry1->index - (entry1->islower)) - (entry2->index - (entry2->islower));
                }
              
                if(comparison != 0) {
                  return comparison > 0 ? 1 : -1;
                }
              }
              else {
                //scripts are ordered
                return entry1->script > entry2->script ? 1 : -1;
              }
            }
            else if(entry1->script != char2) {
              //compare codepoints
              return (entry1->script > char2) ? 1 : -1;
            }
          }
          else if ((entry2 = getLookupTableEntry(&offset2, str2, &bytesMatched2, get2)) && char1 != entry2->script) {
            //compare codepoints
            return (char1 > entry2->script) ? 1 : -1;
          }
          else if(char1 != char2) {
            //compare codepoints
            return (char1 > char2) ? 1 : -1;
          }
          
          offset1+=bytesMatched1;
          offset2+=bytesMatched2;

          //consume and compare all the remaining combining characters
                  
          skip1 = FALSE;
          skip2 = FALSE;

          do {
            if(skip1 == FALSE) {
             combiner1 = isCombining(
                  (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get1))
                  (&offset1, str1, 0, &bytesMatched1, get1)
              );
            }
            
            if(skip2 == FALSE) {
              combiner2 = isCombining(
                  (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))
                  (&offset2, str2, 0, &bytesMatched2, get2)
              );
            }
            
            if(skip1 == FALSE && skip2 == FALSE) {
              if(combiner1 != combiner2) {
                //if either of them is not a combining character or not equal then accentcheck is set to 1
                //if accentcheck is already 1 then return the one thats greater
                if(accentcheck == 2) {
                  return (combiner1 > combiner2) ? 1 : -1;
                }
                else {
                  accentcheck = 1;
                }
              }
            }
            
            if(combiner1 == 0) {
              skip1 = TRUE; 
            }
            else {
              offset1+=bytesMatched1;
            }

            if(combiner2 == 0) {
              skip2 = TRUE; 
            }
            else {
              offset2+=bytesMatched2;
            }
          } while (skip1 == FALSE || skip2 == FALSE);
        }
        else {
          //ascii non letters sort first
          return 1;
        }
      }
      else if(
          *offset2 >= 'A' && *offset2 <= 'z' &&
          (*offset2 >= 'a' || *offset2 <= 'Z')
      ) {
        //ascii non letters sort first
        return -1;
      }

      //parse numbers
      else if(
          (*offset1 >= '0' && *offset1 <= '9') ||
          (firstChar && (
          (*offset1 == '.' && (*(offset1+1) >= '0' && *(offset1+1) <= '9')) ||
          (*offset1 == '-' && ((*(offset1+1) >= '0' && *(offset1+1) <= '9') || (*(offset1+1) == '.' && (*(offset1+2) >= '0' && *(offset1+2) <= '9')))) ||
          (*offset1 == '+' && ((*(offset1+1) >= '0' && *(offset1+1) <= '9') || (*(offset1+1) == '.' && (*(offset1+2) >= '0' && *(offset1+2) <= '9'))))
          ))
      ) {
        if (
          (*offset2 >= '0' && *offset2 <= '9') ||
          (firstChar && (
          (*offset2 == '.' && (*(offset2+1) >= '0' && *(offset2+1) <= '9')) ||
          (*offset2 == '-' && ((*(offset2+1) >= '0' && *(offset2+1) <= '9') || (*(offset2+1) == '.' && (*(offset2+2) >= '0' && *(offset2+2) <= '9')))) ||
          (*offset2 == '+' && ((*(offset2+1) >= '0' && *(offset2+1) <= '9') || (*(offset2+1) == '.' && (*(offset2+2) >= '0' && *(offset2+2) <= '9'))))
          ))
        ) {
          dbl1 = strtod((char *)offset1, (char **)&offset1);
          dbl2 = strtod((char *)offset2, (char **)&offset2);

          //if the numbers parsed are not identical then we can return which string is lesser
          if(dbl1 != dbl2) {
            return (dbl1 < dbl2) ? -1 : 1;
          }
        }
        else {
          return 1;
        }
      }
      else if (
          (*offset2 >= '0' && *offset2 <= '9') ||
          (firstChar && (
          (*offset2 == '.' && (*(offset2+1) >= '0' && *(offset2+1) <= '9')) ||
          (*offset2 == '-' && ((*(offset2+1) >= '0' && *(offset2+1) <= '9') || (*(offset2+1) == '.' && (*(offset2+2) >= '0' && *(offset2+2) <= '9')))) ||
          (*offset2 == '+' && ((*(offset2+1) >= '0' && *(offset2+1) <= '9') || (*(offset2+1) == '.' && (*(offset2+2) >= '0' && *(offset2+2) <= '9'))))
          ))
        ) {

        //non numbers sort first
        return -1;
      }
      else {
        //binary comparison on everything else
        if(*offset1 != *offset2) {
          return (*offset1 > *offset2) ? 1 : -1;
        }

        //update the offsets
        offset1++;
        offset2++;
      }

      //a previous iteration of the loop may have done a unicode check.
      //clear the flag as we've just done a comparison
      char1found = 0;
      
      if(firstChar) {
        firstChar = FALSE;
      }
    }

    //in depth unicode checks for this iteration

    //character 1 has not yet been found
    else if (char1found == 0) {
      //read a character from string 1
      char1 = (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get1))(&offset1, str1, 0, &bytesMatched1, get1);

      if (char1 != 0x34F) {
        //read a character from string 2
        char2 = (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))(&offset2, str2, 0, &bytesMatched2, get2);

        if((entry1 = getLookupTableEntry(&offset1, str1, &bytesMatched1, get1))) {
          //the first character is in the lookup table

          if(char2 != 0x34F) {
            if((entry2 = getLookupTableEntry(&offset2, str2, &bytesMatched2, get2))) {
              //compare the lookup table entries
          
              if(entry1->script == entry2->script) {
                if(caseSensitive) {
                  comparison = entry1->index - entry2->index;
                }
                else {
                  comparison = (entry1->index - (entry1->islower)) - (entry2->index - (entry2->islower));
                }
              
                if(comparison != 0) {
                  return comparison > 0 ? 1 : -1;
                }
              }
              else {
                //scripts are ordered
                return entry1->script > entry2->script ? 1 : -1;
              }
            }
            //compare codepoints
            else if(entry1->script != char2) {
              return (entry1->script > char2) ? 1 : -1;
            }

            offset1 += bytesMatched1;
            offset2 += bytesMatched2;

            skip1 = FALSE;
            skip2 = FALSE;

            do {
              if(skip1 == FALSE) {
               combiner1 = isCombining(
                    (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get1))
                    (&offset1, str1, 0, &bytesMatched1, get1)
                );
              }
              
              if(skip2 == FALSE) {
                combiner2 = isCombining(
                    (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))
                    (&offset2, str2, 0, &bytesMatched2, get2)
                );
              }
              
              if(skip1 == FALSE && skip2 == FALSE) {
                if(combiner1 != combiner2) {
                  //if either of them is not a combining character or not equal then accentcheck is set to 1
                  //if accentcheck is already 1 then return the one thats greater
                  if(accentcheck == 2) {
                    return (combiner1 > combiner2) ? 1 : -1;
                  }
                  else {
                    accentcheck = 1;
                  }
                }
              }
              
              if(combiner1 == 0) {
                skip1 = TRUE; 
              }
              else {
                offset1+=bytesMatched1;
              }

              if(combiner2 == 0) {
                skip2 = TRUE; 
              }
              else {
                offset2+=bytesMatched2;
              }
            } while (skip1 == FALSE || skip2 == FALSE);
        
            if(firstChar) {
              firstChar = FALSE;
            }
          }
          else {
            //we've found the first character, but not yet the second one.
            //we can skip some assignments and checks on the next loop iteration
            char1found = 1;   //in lookup
            offset2 += bytesMatched2;
          }
        }
        else if(char2 != 0x34F) {
          if((entry2 = getLookupTableEntry(&offset2, str2, &bytesMatched2, get2))){
            if(char1 != entry2->script) {
              return (char1 > entry2->script) ? 1: -1;
            }
          }
          //compare codepoints
          else if(char1 != char2) {
            return (char1 > char2) ? 1: -1; 
          }

          offset1 += bytesMatched1;
          offset2 += bytesMatched2;

          skip1 = FALSE;
          skip2 = FALSE;

          do {
            if(skip1 == FALSE) {
             combiner1 = isCombining(
                  (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get1))
                  (&offset1, str1, 0, &bytesMatched1, get1)
              );
            }
            
            if(skip2 == FALSE) {
              combiner2 = isCombining(
                  (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))
                  (&offset2, str2, 0, &bytesMatched2, get2)
              );
            }
            
            if(skip1 == FALSE && skip2 == FALSE) {
              if(combiner1 != combiner2) {
                //if either of them is not a combining character or not equal then accentcheck is set to 1
                //if accentcheck is already 1 then return the one thats greater
                if(accentcheck == 2) {
                  return (combiner1 > combiner2) ? 1 : -1;
                }
                else {
                  accentcheck = 1;
                }
              }
            }
            
            if(combiner1 == 0) {
              skip1 = TRUE; 
            }
            else {
              offset1+=bytesMatched1;
            }

            if(combiner2 == 0) {
              skip2 = TRUE; 
            }
            else {
              offset2+=bytesMatched2;
            }
          } while (skip1 == FALSE || skip2 == FALSE);
      
          if(firstChar) {
            firstChar = FALSE;
          }
        }
        else {
          //we've found the first character, but not yet the second one.
          //we can skip some assignments and checks on the next loop iteration
          char1found = 2;   //in lookup
          offset2 += bytesMatched2;
        }
      }  
      else {
        //we've not yet found the first codepoint to compare. move to the next one
        offset1 += bytesMatched1;
      }
    }

    //character 1 has been found and is in the lookup table
    else if (char1found == 1) {
      //read a character from string 2
      char2 = (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))(&offset1, str1, 0, &bytesMatched2, get2);

      if(char2 != 0x34F) {
        if((entry2 = getLookupTableEntry(&offset2, str2, &bytesMatched2, get2))) {
          //both characters have been found and are in the lookup table. compare the lookup table entries          
      
          if(entry1->script == entry2->script) {
            if(caseSensitive) {
              comparison = entry1->index - entry2->index;
            }
            else {
              comparison = (entry1->index - (entry1->islower)) - (entry2->index - (entry2->islower));
            }
          
            if(comparison != 0) {
              return comparison > 0 ? 1 : -1;
            }
          }
          else {
            //scripts are ordered
            return entry1->script > entry2->script ? 1 : -1;
          }
        }
        //compare the codepoints
        else if(entry1->script != char2) {
          return (entry1->script > char2) ? 1: -1; 
        }

        offset1 += bytesMatched1;
        offset2 += bytesMatched2;

        skip1 = FALSE;
        skip2 = FALSE;

        do {
          if(skip1 == FALSE) {
           combiner1 = isCombining(
                (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get1))
                (&offset1, str1, 0, &bytesMatched1, get1)
            );
          }
          
          if(skip2 == FALSE) {
            combiner2 = isCombining(
                (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))
                (&offset2, str2, 0, &bytesMatched2, get2)
            );
          }
          
          if(skip1 == FALSE && skip2 == FALSE) {
            if(combiner1 != combiner2) {
              //if either of them is not a combining character or not equal then accentcheck is set to 1
              //if accentcheck is already 1 then return the one thats greater
              if(accentcheck == 2) {
                return (combiner1 > combiner2) ? 1 : -1;
              }
              else {
                accentcheck = 1;
              }
            }
          }
          
          if(combiner1 == 0) {
            skip1 = TRUE; 
          }
          else {
            offset1+=bytesMatched1;
          }

          if(combiner2 == 0) {
            skip2 = TRUE; 
          }
          else {
            offset2+=bytesMatched2;
          }
        } while (skip1 == FALSE || skip2 == FALSE);

        char1found = 0;
    
        if(firstChar) {
          firstChar = FALSE;
        }
      }
      else {
        offset2 += bytesMatched2;
      }
    }
    
    //character 1 has been found but was not in the lookup table
    else {  
      char2 = (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))(&offset2, str2, 0, &bytesMatched2, get2);

      if(char2 != 0x34F) {
        //the first or both characters were not in the lookup table.
        //compare the code point then successive combining characters
        if((entry2 = getLookupTableEntry(&offset2, str2, &bytesMatched2, get2)) && char1 != entry2->script) {
          return (char1 > entry2->script) ? 1: -1; 
        }
        //compare codepoints
        else if(char1 != char2) {
          return (char1 > char2) ? 1: -1; 
        }

        offset1 += bytesMatched1;
        offset2 += bytesMatched2;

        skip1 = FALSE;
        skip2 = FALSE;

        do {
          if(skip1 == FALSE) {
           combiner1 = isCombining(
                (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get1))
                (&offset1, str1, 0, &bytesMatched1, get1)
            );
          }
          
          if(skip2 == FALSE) {
            combiner2 = isCombining(
                (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))
                (&offset2, str2, 0, &bytesMatched2, get2)
            );
          }
          
          if(skip1 == FALSE && skip2 == FALSE) {
            if(combiner1 != combiner2) {
              //if either of them is not a combining character or not equal then accentcheck is set to 1
              //if accentcheck is already 1 then return the one thats greater
              if(accentcheck == 2) {
                return (combiner1 > combiner2) ? 1 : -1;
              }
              else {
                accentcheck = 1;
              }
            }
          }
          
          if(combiner1 == 0) {
            skip1 = TRUE; 
          }
          else {
            offset1+=bytesMatched1;
          }

          if(combiner2 == 0) {
            skip2 = TRUE; 
          }
          else {
            offset2+=bytesMatched2;
          }
        } while (skip1 == FALSE || skip2 == FALSE);

        char1found = 0;
    
        if(firstChar) {
          firstChar = FALSE;
        } 
      }
      else {
        offset2 += bytesMatched2;
      }
    }
  } 
}

/*
int main(int argc, unsigned char** argv) {
  unsigned char *a, *b;
  if (argc == 3) {
    a = strdup(argv[1]);
    b = strdup(argv[2]);

    printf("%d\n", strCompare(&a, &b, 0, (void (*)())getUnicodeChar, (void (*)())&getUnicodeChar));

    free(a);
    free(b);
  }
  
  return 0;
}
//*/
