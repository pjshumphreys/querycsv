/* checks whether the end of file is the next character in the stream. */
/* Used by getMatchingRecord */
int endOfFile(FILE* stream) {
  int c;

  MAC_YIELD

  c = fgetc(stream);
  ungetc(c, stream);

  return c != EOF;
}
