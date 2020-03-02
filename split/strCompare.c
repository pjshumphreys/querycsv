int clause5(int script1, int script2) {
  if(script1 == 12448) {
    script1 = 12353;
  }

  if(script2 == 12448) {
    script2 = 12353;
  }

  return script1 == script2;
}

int strCompare(
    unsigned char **str1,
    unsigned char **str2,
    int caseSensitive,
    void (*get1)(void),
    void (*get2)(void)
) {
  /*
    caseSensitive is a bitfield that has the following meaning
    16 - if true then interleave hiragana and katakana
    8 - if true then non lower case letters come first
    4 - compare numbers as individual characters
    2 - if true then accented letters come after the base letter. implies bit 1 automatically. otherwise treated the same.
    1 - if true then lower case letters come first. otherwise treated the same
  */

  unsigned char *offset1 = *str1, *offset2 = *str2;
  long char1 = 0, char2 = 0;
  struct hash4Entry *entry1 = NULL, *entry2 = NULL;
  int firstChar = TRUE;
  int compareNumbers;
  int combinerResult;
  int comparison;
  int char1found;
  int bytesMatched1;
  int bytesMatched2;
  int accentcheck;
  int upperCaseFirst;
  int mergeKana;
  int kanacheck;

  comparison = char1found = bytesMatched1 = bytesMatched2 =
  accentcheck = kanacheck = 0;

  MAC_YIELD

  /* we specified we want to not treat numbers specially but as individual characters instead */
  if(caseSensitive & 4) {
    compareNumbers = FALSE;
    caseSensitive &= ~(4);
  }
  else {
    compareNumbers = TRUE;
  }

  /* we specified uppercase should come first */
  if(caseSensitive & 8) {
    upperCaseFirst = TRUE;
    caseSensitive &= ~(8);

    if(caseSensitive == 0) {
      caseSensitive = 1;
    }
  }
  else {
    upperCaseFirst = FALSE;
  }

  /* we specified uppercase should come first */
  if(caseSensitive & 16) {
    mergeKana = TRUE;
    caseSensitive &= ~(16);
  }
  else {
    mergeKana = FALSE;
  }

  do {  /* we'll quit from this function via other means */
    /* check if we've reached the end of string 2 */
    if(*offset2 == 0) {
      /* if string2 is on the slower version of getUnicodeChar, */
      /* there were no weird characters, so we can switch to always using the faster version  */

      /* if they both are null then the strings are equal. otherwise string 2 is lesser */
      if(*offset1 == 0) {
        if(accentcheck) {
          /* a difference just on the accents on a letter were found. re-compare with accent checking enabled. */
          accentcheck = 2;
          if(caseSensitive == 2) {
            caseSensitive = 1;
          }
        }

        if(kanacheck) {
          kanacheck = 2;
        }

        if(accentcheck || kanacheck) {
          offset1 = *str1;
          offset2 = *str2;
          continue;
        }

        return 0;
      }

      return 1;
    }

    /* check if we've reached the end of string 1 */
    else if(*offset1 == 0) {
      /* if they both are null then the strings are equal. otherwise string 1 is lesser */
      return -1;
    }

    /* character 1 has not yet been found */
    else if(char1found == 0) {
      /* read a character from string 1 */
      char1 = (*((long (*)(unsigned char **, unsigned char **, int,  int *, void (*)(void)))get1))(&offset1, str1, 0, &bytesMatched1, get1);

      if(char1 != 0x34F) {
        /* read a character from string 2 */
        char2 = (*((long (*)(unsigned char **, unsigned char **, int,  int *, void (*)(void)))get2))(&offset2, str2, 0, &bytesMatched2, get2);

        if((entry1 = getLookupTableEntry(&entry1Internal, &offset1, str1, &bytesMatched1, get1, firstChar, compareNumbers))) {
          /* the first character is in the lookup table */

          if(char2 != 0x34F) {
            if((entry2 = getLookupTableEntry(&entry2Internal, &offset2, str2, &bytesMatched2, get2, firstChar, compareNumbers))) {
              /* compare the lookup table entries */

              if(entry1->script == entry2->script) {
                if(entry1->index == 0 && entry2->index == 0) {
                  /* both entries are numbers, so compare them */
                  comparison = strNumberCompare((char *)offset1, (char *)offset2);
                }
                else if(caseSensitive == 1) {
                  if(upperCaseFirst && entry1->script == 127) {
                    comparison = (entry1->index - (entry1->isNotLower) + (entry1->isNotLower == 0 ? 1 : entry1->index == 188 ? 2 : 0)) -
                      (entry2->index - (entry2->isNotLower) + (entry2->isNotLower == 0 ? 1 : entry2->index == 188 ? 2 : 0));
                  }
                  else {
                    comparison = entry1->index - entry2->index;
                  }
                }
                else {
                  comparison = (entry1->index - (entry1->isNotLower)) - (entry2->index - (entry2->isNotLower));

                  if((entry1->index - entry2->index) != 0 && caseSensitive == 2) {
                    accentcheck = 1;
                  }
                }

                if(comparison != 0) {
                  return (comparison > 0) ? 1 : -1;
                }
              }
              else if (mergeKana && clause5(entry1->script, entry2->script)) {
                if(kanacheck == 2) {
                  comparison = entry1->index - entry2->index;
                }
                else {
                  comparison = (entry1->index - (entry1->isNotLower)) - (entry2->index - (entry2->isNotLower));

                  if(comparison == 0 && (entry1->index - entry2->index)) {
                    kanacheck = 1;
                  }
                }

                if(comparison != 0) {
                  return (comparison > 0) ? 1 : -1;
                }
              }
              else {
                /* scripts are ordered */
                return (entry1->script > entry2->script) ? 1 : -1;
              }
            }
            /* compare codepoints */
            else if(entry1->script != char2) {
              return (entry1->script > char2) ? 1 : -1;
            }

            if((combinerResult = consumeCombiningChars(
                str1, str2,
                &offset1, &offset2,
                get1, get2,
                &bytesMatched1, &bytesMatched2,
                &accentcheck)) != 0) {
              return combinerResult;
            }

            if(firstChar) {
              firstChar = FALSE;
            }
          }
          else {
            /* we've found the first character, but not yet the second one. */
            /* we can skip some assignments and checks on the next loop iteration */
            char1found = 1;   /* in lookup */
            offset2 += bytesMatched2;
          }
        }
        else if(char2 != 0x34F) {
          if((entry2 = getLookupTableEntry(&entry2Internal, &offset2, str2, &bytesMatched2, get2, firstChar, compareNumbers))){
            if(char1 != entry2->script) {
              return (char1 > entry2->script) ? 1: -1;
            }
          }
          /* compare codepoints */
          else if(char1 != char2) {
            return (char1 > char2) ? 1: -1;
          }

          if((combinerResult = consumeCombiningChars(
              str1, str2,
              &offset1, &offset2,
              get1, get2,
              &bytesMatched1, &bytesMatched2,
              &accentcheck)) != 0) {
            return combinerResult;
          }

          if(firstChar) {
            firstChar = FALSE;
          }
        }
        else {
          /* we've found the first character, but not yet the second one. */
          /* we can skip some assignments and checks on the next loop iteration */
          char1found = 2;   /* in lookup */
          offset2 += bytesMatched2;
        }
      }
      else {
        /* we've not yet found the first codepoint to compare. move to the next one */
        offset1 += bytesMatched1;
      }
    }

    /* character 1 has been found and is in the lookup table */
    else if(char1found == 1) {
      /* read a character from string 2 */
      char2 = (*((long (*)(unsigned char **, unsigned char **, int,  int *, void (*)(void)))get2))(&offset1, str1, 0, &bytesMatched2, get2);

      if(char2 != 0x34F) {
        if((entry2 = getLookupTableEntry(&entry2Internal, &offset2, str2, &bytesMatched2, get2, firstChar, compareNumbers))) {
          /* both characters have been found and are in the lookup table. compare the lookup table entries           */

          if(entry1->script == entry2->script) {
            if(entry1->index == 0 && entry2->index == 0) {
              /* both entries are numbers, so compare them */
              comparison = strNumberCompare((char *)offset1, (char *)offset2);
            }
            else if(caseSensitive == 1) {
              if(upperCaseFirst && entry1->script == 127) {
                comparison = (entry1->index - (entry1->isNotLower) + (entry1->isNotLower == 0 ? 1 : entry1->index == 188 ? 2 : 0)) -
                  (entry2->index - (entry2->isNotLower) + (entry2->isNotLower == 0 ? 1 : entry2->index == 188 ? 2 : 0));
              }
              else {
                comparison = entry1->index - entry2->index;
              }
            }
            else {
              comparison = (entry1->index - (entry1->isNotLower)) - (entry2->index - (entry2->isNotLower));

              if((entry1->index - entry2->index) != 0 && caseSensitive == 2) {
                accentcheck = 1;
              }
            }

            if(comparison != 0) {
              return comparison > 0 ? 1 : -1;
            }
          }
          else if (mergeKana && clause5(entry1->script, entry2->script)) {
            if(kanacheck == 2) {
              comparison = entry1->index - entry2->index;
            }
            else {
              comparison = (entry1->index - (entry1->isNotLower)) - (entry2->index - (entry2->isNotLower));

              if(comparison == 0 && (entry1->index - entry2->index)) {
                kanacheck = 1;
              }
            }

            if(comparison != 0) {
              return (comparison > 0) ? 1 : -1;
            }
          }
          else {
            /* scripts are ordered */
            return entry1->script > entry2->script ? 1 : -1;
          }
        }
        /* compare the codepoints */
        else if(entry1->script != char2) {
          return (entry1->script > char2) ? 1: -1;
        }

        if((combinerResult = consumeCombiningChars(
            str1, str2,
            &offset1, &offset2,
            get1, get2,
            &bytesMatched1, &bytesMatched2,
            &accentcheck)) != 0) {
          return combinerResult;
        }

        if(firstChar) {
          firstChar = FALSE;
        }

        char1found = 0;
      }
      else {
        offset2 += bytesMatched2;
      }
    }

    /* character 1 has been found but was not in the lookup table */
    else {
      char2 = (*((long (*)(unsigned char **, unsigned char **, int,  int *, void (*)(void)))get2))(&offset2, str2, 0, &bytesMatched2, get2);

      if(char2 != 0x34F) {
        /* the first or both characters were not in the lookup table. */
        /* compare the code point then successive combining characters */
        if((entry2 = getLookupTableEntry(&entry2Internal, &offset2, str2, &bytesMatched2, get2, firstChar, compareNumbers)) && char1 != entry2->script) {
          return (char1 > entry2->script) ? 1: -1;
        }
        /* compare codepoints */
        else if(char1 != char2) {
          return (char1 > char2) ? 1: -1;
        }

        if((combinerResult = consumeCombiningChars(
            str1, str2,
            &offset1, &offset2,
            get1, get2,
            &bytesMatched1, &bytesMatched2,
            &accentcheck)) != 0) {
          return combinerResult;
        }

        char1found = 0;

        if(firstChar) {
          firstChar = FALSE;
        }
      }
      else {
        offset2 += bytesMatched2;
      }
    }
  } while(1);
}
