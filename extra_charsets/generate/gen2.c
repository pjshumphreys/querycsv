/* This program converts the stripped utf-8 mapping into a node js module
 * containing an array where each item in the array has the form:
 *
 *   unicode codepoint number,
 *   array of utf-8 byte numbers for that codepoint,
 *   shift-jis lead byte (0 for no lead byte)
 *   shift-jis trailiing byte
 */
/* This node js module is then used by gensjis3.js to produce a header
 * file that create.c can use. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define MYEOF -1

void reallocMsg(void **mem, size_t size) {
	void *temp;

	if(mem != NULL) {
		if(size) {
			temp = realloc(*mem, size);

			if(temp == NULL) {
				fputs("TDB_MALLOC_FAILED3", stderr);
				exit(EXIT_FAILURE);
			}

			*mem = temp;
		}
		else {
			free(*mem);
			*mem = NULL;
		}
	}
	else {
		fputs("TDB_INVALID_REALLOC", stderr);
		exit(EXIT_FAILURE);
	}
}

/*CP1252 mapping table*/
static const unsigned long cp1252[160] = {
	0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
	0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x008D, 0x017D, 0x008F,
	0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
	0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x009D, 0x017E, 0x0178,
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
	0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
	0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
	0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
	0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
	0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
	0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF,
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
	0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
	0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
	0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F
};

/* returns a number of codepoints, each as a long */
/* invalid bytes or bytes that form an overlong codepoint are treated as
a set of windows-1252 characters that are then each converted to their coresponding value in unicode */
/* the number of bytes read in the file is also returned, so you can fseek back to offset after the match later */
/* due to the nature of utf-8, commas, new lines and double quotes always appear at the end of the array */
void getCodepointsUTF8(
		FILE *stream,
		long *codepoints,
		int *arrLength,
		int *byteLength
) {
	int byteIndex = -1;
	int c;

	if(stream == NULL) {
		*arrLength = 0;
		*byteLength = 0;
		return;
	}
	c = fgetc(stream);

	if(c == EOF) {
		*arrLength = *byteLength = 1;
		codepoints[0] = MYEOF;
		return;
	}
	else {
		codepoints[++byteIndex] = (long)c;
	}

	do {  /* not a real loop. We only need it to be
		able to use the break statement */
		/* if the current byte offset is a valid utf-8 character that's not
		overlong or decomposable then return it */
		if(c < 0x81) {
			/* read 1 byte. no overlong checks needed as a 1 byte code can */
			/* never be overlong */
			*arrLength = *byteLength = 1;
			return;
		}
		/* ensure the current byte is the start of a valid utf-8 sequence */
		else if(c > 0xC1) {
			if(c < 0xE0) {
				/* read 2 bytes */
				if((c = fgetc(stream)) != EOF) {
					codepoints[++byteIndex] = c;
				}
				else {
					break;
				}

				if((c & 0xC0) == 0x80) {
					codepoints[0] = (codepoints[0] << 6) + codepoints[1] - 0x3080;

					*arrLength = 1;
					*byteLength = 2;

					return;
				}

				/* prevent the potential first byte of the next valid
					utf-8 sequence also being consumed */
				ungetc(c, stream);
				byteIndex--;
			}
			else if(c < 0xF0) {
				/* read 3 bytes */
				if((c = fgetc(stream)) != EOF) {
					codepoints[++byteIndex] = c;
				}
				else {
					break;
				}

				if(
						(c & 0xC0) == 0x80 &&
						(codepoints[0] != 0xE0 || c > 0x9F)
				) {
					if((c = fgetc(stream)) != EOF) {
						codepoints[++byteIndex] = c;
					}
					else {
						break;
					}

					if((c & 0xC0) == 0x80) {
						codepoints[0] = (codepoints[0] << 12) + (codepoints[1] << 6) + codepoints[2] - 0xE2080;

						*arrLength = 1;
						*byteLength = 3;
						return;
					}
				}

				/* prevent the potential first byte of the next valid
					utf-8 sequence also being consumed */
				ungetc(c, stream);
				byteIndex--;
			}
			else if(c < 0xF5) {
				/* read 4 bytes */
				if((c = fgetc(stream)) != EOF) {
					codepoints[++byteIndex] = c;
				}
				else {
					break;
				}

				if(
						(c & 0xC0) == 0x80 &&
						(codepoints[0] != 0xF0 || c > 0x8F) &&
						(codepoints[0] != 0xF4 || c < 0x90)
				) {
					if((c = fgetc(stream)) != EOF) {
						codepoints[++byteIndex] = c;
					}
					else {
						break;
					}

					if((c & 0xC0) == 0x80) {
						if((c = fgetc(stream)) != EOF) {
							codepoints[++byteIndex] = c;
						}
						else {
							break;
						}

						if((c & 0xC0) == 0x80) {
							codepoints[0] = (codepoints[0] << 18) + (codepoints[1] << 12) + (codepoints[2] << 6) + codepoints[3] - 0x3C82080;

							*arrLength = 1;
							*byteLength = 4;
							return;
						}
					}
				}

				/* prevent the potential first byte of the next valid
					utf-8 sequence also being consumed */
				ungetc(c, stream);
				byteIndex--;
			}
		}
	} while(0); /* don't really loop. */

	/* dump out the bytes matched, converting each to a separate codepoint */
	if(c == EOF) {
		codepoints[++byteIndex] = MYEOF;
	}

	*arrLength = *byteLength = byteIndex + 1;

	for( ; byteIndex > -1; byteIndex--) {
		/* use codepage 1252 conversions */
		codepoints[byteIndex] = (long)(cp1252[codepoints[byteIndex] - 0x80]);
	}
}

int strAppendUTF8(long codepoint, unsigned char **nfdString, int *retval) {
	int nfdLength = 0;

	if(retval) {
		nfdLength = *retval;
	}

	if(codepoint < 0x80) {
		reallocMsg((void**)nfdString, nfdLength+1);

		(*nfdString)[nfdLength++] = codepoint;
	}
	else if(codepoint < 0x800) {
		reallocMsg((void**)nfdString, nfdLength+2);

		(*nfdString)[nfdLength++] = (codepoint >> 6) + 0xC0;
		(*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
	}
	else if(codepoint < 0x10000) {
		reallocMsg((void**)nfdString, nfdLength+3);

		(*nfdString)[nfdLength++] = (codepoint >> 12) + 0xE0;
		(*nfdString)[nfdLength++] = ((codepoint >> 6) & 0x3F) + 0x80;
		(*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
	}
	else {
		reallocMsg((void**)nfdString, nfdLength+4);

		(*nfdString)[nfdLength++] = (codepoint >> 18) + 0xF0;
		(*nfdString)[nfdLength++] = ((codepoint >> 12) & 0x3F) + 0x80;
		(*nfdString)[nfdLength++] = ((codepoint >> 6) & 0x3F) + 0x80;
		(*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
	}

	if(retval) {
		*retval = nfdLength;
	}

	return TRUE;
}

int d_fgets(char** ws, FILE* stream) {
  char buf[80];
  char* newWs = NULL;
  char* potentialNewWs = NULL;
  size_t totalLength = (size_t)0;
  size_t potentialTotalLength = (size_t)0;
  size_t bufferLength;

  /* check sanity of inputs */
  if(ws == NULL) {
    return FALSE;
  }

  /* try reading some text from the file into the buffer */
  while(fgets((char *)&(buf[0]), 80, stream) != NULL) {

    /* get the length of the string in the buffer */
    bufferLength = strlen((char *)&(buf[0]));

    /*
      add it to the potential new length.
      this might not become the actual new length if the realloc fails
    */
    potentialTotalLength+=bufferLength;

    /*
      try reallocating the output string to be a bit longer
      if it fails then set any existing buffer to the return value and return true
    */
    if((potentialNewWs = (char*)realloc(newWs, ((potentialTotalLength+1)*sizeof(char)))) == NULL) {

      /* if we've already retrieved some text */
      if(newWs != NULL) {

        /* ensure null termination of the string */
        newWs[totalLength] = '\0';

        /* set the output string pointer and return true */
        if(*ws) {
          free(*ws);
        }
        *ws = newWs;

        return TRUE;
      }

      /*
        otherwise no successful allocation was made.
        return false without modifying the output string location
      */
      return FALSE;
    }

    /* copy the buffer data into potentialNewWs */
    memcpy(potentialNewWs+totalLength, &buf, bufferLength*sizeof(char));

    /* the potential new string becomes the actual one */
    totalLength = potentialTotalLength;
    newWs = potentialNewWs;

    /* if the last character is '\n' (ie we've reached the end of a line) then return the result */
    if(newWs[totalLength-1] == '\n') {

      /* ensure null termination of the string */
      newWs[totalLength-1] = '\0';

      /* set the output string pointer and return true */
      if(*ws) {
        free(*ws);
      }

      *ws = newWs;

      return TRUE;
    }
  }

  /* if we've already retrieved some text */
  if(newWs != NULL) {
    /* ensure null termination of the string */
    newWs[totalLength] = '\0';

    /* set the output string point and return true */
    if(*ws) {
      free(*ws);
    }
    *ws = newWs;

    return TRUE;
  }

  /*
    otherwise no successful allocation was made.
    return false without modifying the output string location
  */
  return FALSE;
}

int compareStrings(void*a, void*b) {
	return strcmp(*(char**)a, *(char**)b);
}

int main(int argc, char ** argv) {
	FILE * input;
	FILE * intermediate;
	FILE * output;

	long codepoints[4];
	int arrLength = 0;
	int byteLength = 0;
	int state = 1;
	int i;
	int codepoint;
	int lineCount = 1;
	int trailingBytes = 1;
	int doTrailing = 1;

	input = stdin;
	intermediate = tmpfile();

	//read a double quote
	getCodepointsUTF8(input, &codepoints, &arrLength, &byteLength);

	while(codepoints[0] != MYEOF) {
		if(arrLength != 1) {
			printf("abort!\n");
			exit(1);
		}

		switch(state) {
			case 1: {
				if(codepoints[0] == 0x22) {
					state = 2;
				}
			} break;

			case 2: {
				codepoint = codepoints[0];
				state = 3;
			} break;

			case 3: {
				if(codepoints[0] == 0x22) {
					fprintf(intermediate, "%06x,", codepoint);

					getCodepointsUTF8(input, &codepoints, &arrLength, &byteLength);

					if(codepoints[0] == 0x22) {
						getCodepointsUTF8(input, &codepoints, &arrLength, &byteLength);
					}

					state = 4;
				}
				else {
					state = 5;
				}
			} break;

			case 4: {
				if(codepoints[0] == 0x0a) {
					lineCount++;
					doTrailing = 0;
          fputs("\n", intermediate);
					state = 1;
				}
				else {
					fputc(codepoints[0], intermediate);

					if(doTrailing && codepoints[0] == 0x2c) {
						trailingBytes++;
					}
				}
			} break;

			case 5: {
				if(codepoints[0] == 0x0a) {
					state = 1;
				}
			} break;
		}

		getCodepointsUTF8(input, &codepoints, &arrLength, &byteLength);
	}

	//fclose(input);

	rewind(intermediate);	

	char** array = malloc(sizeof(char*) * lineCount);
	
	if(array == NULL) {
		printf("abort!\n");
		exit(1);
	}
	
	for(i = 0; i < lineCount; i++) {
		array[i] = NULL;
		d_fgets(&(array[i]), intermediate);
		if(array[i] == NULL) {
		  lineCount = i;
		  break;
		}
	}
	
	fclose(intermediate);

	output = fopen(argv[1], "w");
	
	qsort(array, lineCount, sizeof(char*), compareStrings);
	
	fprintf(output,"%x,%x\n", lineCount, trailingBytes);
	
	for(i = 0; i < lineCount; i++) {	
		char* temp = strrchr(array[i], ',') + 1;
		char* temp2 = NULL;

		int num = strtol(temp, &temp2, 16);
		
		if(num < 0x20) {
			fprintf(output, "%x", num);
			temp = temp2;
			
			temp = array[i];
			temp2 = NULL;
			strtol(temp, &temp2, 16);
			temp = temp2;
		}
		else {
			temp = array[i];
			temp2 = NULL;
		}

		do {
			if(temp[0] == 0) {
				break;
			}
			
			if(temp[0] == ',') {
				fprintf(output, ",");
				temp++;
				continue;
			}
		
			fprintf(output, "%x", strtol(temp, &temp2, 16));
			temp = temp2;
		} while(1);

		fprintf(output, "\n");
	}

	fclose(output);

	return 0;
}
