/* variables that shouldn't be placed on the stack, so we make them global instead */

/* hash1 helper variables */
int i;
/*long hash1Codepoints[2];
struct hash1Entry lookupresult = {0, (long *)(&hash1Codepoints)};
*/

/* hash2 helper variables */
long codepoints[18];
struct hash2Entry entry = {0x0000, 1, (long *)(&codepoints)};
struct hash2Entry* retval;

/* hash4 helper varables */
char hash4letter[7];
struct hash4Entry hash4export = {(char *)(&hash4letter), 0, 0, 0};
char * devNull = NULL;

/*dedup string literals */
const char* TDB_COULDNT_OPEN_INPUT = TDB_COULDNT_OPEN_INPUT2;
const char* TDB_MALLOC_FAILED = TDB_MALLOC_FAILED2;
