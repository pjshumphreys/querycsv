/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf hash4a.gperf  */
/* Computed positions: -k'1-3' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 6 "hash4a.gperf"
struct hash4Entry {
  const char *name;
  int script;
  int index;
  int isNotLower;
};
#include <string.h>

#define TOTAL_KEYWORDS 238
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 3
#define MIN_HASH_VALUE 1
#define MAX_HASH_VALUE 468
/* maximum key range = 468, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash_a (register const char *str, register size_t len)
{
  static unsigned short asso_values[] =
    {
      469, 469, 469, 469, 469, 469, 469, 469, 469, 469,
      469, 469, 469, 469, 469, 469, 469, 469, 469, 469,
      469, 469, 469, 469, 469, 469, 469, 469, 469, 469,
      469, 469, 469, 469, 469, 469, 469, 469, 469, 469,
      469, 469, 469, 469, 469, 469, 469, 469, 469, 469,
      469, 469, 469, 469, 469, 469, 469, 469, 469, 469,
      469, 469, 469, 469, 469, 204, 199, 189, 184, 179,
      174, 169, 164, 159, 154, 149, 139, 134, 124, 469,
      469, 469, 469, 469, 469, 469, 469, 469, 469, 469,
      469, 469, 469, 469, 469, 469, 469, 119, 114, 109,
      104,  99,  89,  84,  74,  49,  39,  29,  24,  19,
        0, 469, 469, 469, 469, 469, 469, 469, 469, 469,
      469, 469, 469, 469, 469, 469, 469, 469,   5, 250,
      185,  30,  80,  13,  55,   3, 244, 184, 188, 123,
       28,   0, 150, 104, 245, 129, 119,  99, 138,  84,
       69,  63,  53, 160, 234, 159,  90,  15,   0, 235,
      194,  79, 224,   4, 175, 225, 174, 185,  78,  43,
       13,  93,  50, 145, 254, 239, 140,  65, 220,  24,
       25,  75,   5, 230, 250,  50, 115, 105, 215, 125,
      224, 214, 469, 469, 469, 210,  40, 145,   5,  34,
      185,   0,  45, 469, 469, 469, 469, 469, 469, 469,
      469, 469, 469, 469, 469, 469, 469, 469, 469, 469,
      469, 469, 469, 469, 469,   5,  35, 469, 469, 469,
      469, 469, 469, 469,   0, 469, 469, 469, 469, 469,
      469, 469, 469, 469, 469, 469, 469, 469, 469, 469,
      469, 469, 469, 469, 469, 469
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

struct hash4Entry *
in_word_set_a (register const char *str, register size_t len)
{
  static struct hash4Entry wordlist[] =
    {
      {""},
#line 241 "hash4a.gperf"
      {"n",127,291,0},
#line 106 "hash4a.gperf"
      {"\311\236",127,123,0},
      {""}, {""},
#line 85 "hash4a.gperf"
      {"\311\207",127,91,0},
#line 137 "hash4a.gperf"
      {"\311\243",127,162,0},
#line 249 "hash4a.gperf"
      {"\306\236",127,300,0},
#line 227 "hash4a.gperf"
      {"\352\236\200",127,271,1},
      {""},
#line 54 "hash4a.gperf"
      {"\306\207",127,51,1},
#line 139 "hash4a.gperf"
      {"\306\243",127,164,0},
#line 33 "hash4a.gperf"
      {"\306\200",127,27,0},
      {""}, {""},
#line 163 "hash4a.gperf"
      {"\311\252",127,190,3},
#line 150 "hash4a.gperf"
      {"\352\236\252",127,176,1},
#line 105 "hash4a.gperf"
      {"\311\235",127,121,0},
#line 40 "hash4a.gperf"
      {"\341\266\200",127,36,0},
      {""},
#line 230 "hash4a.gperf"
      {"m",127,276,0},
#line 202 "hash4a.gperf"
      {"\352\235\207",127,237,0},
#line 248 "hash4a.gperf"
      {"\306\235",127,299,1},
#line 192 "hash4a.gperf"
      {"\352\235\200",127,227,1},
      {""},
#line 199 "hash4a.gperf"
      {"l",127,234,0},
#line 220 "hash4a.gperf"
      {"\341\266\205",127,259,0},
#line 243 "hash4a.gperf"
      {"\311\264",127,293,2},
      {""}, {""},
#line 183 "hash4a.gperf"
      {"k",127,217,0},
#line 195 "hash4a.gperf"
      {"\352\235\205",127,230,0},
#line 34 "hash4a.gperf"
      {"\311\203",127,28,1},
#line 232 "hash4a.gperf"
      {"\341\264\215",127,278,2},
      {""},
#line 75 "hash4a.gperf"
      {"\306\214",127,76,0},
#line 82 "hash4a.gperf"
      {"\341\264\207",127,86,2},
#line 43 "hash4a.gperf"
      {"\306\203",127,39,0},
#line 15 "hash4a.gperf"
      {"\341\264\200",127,3,2},
      {""},
#line 174 "hash4a.gperf"
      {"j",127,205,0},
#line 147 "hash4a.gperf"
      {"\307\266",127,172,1},
#line 240 "hash4a.gperf"
      {"\352\235\263",127,290,0},
#line 130 "hash4a.gperf"
      {"\341\266\203",127,154,0},
      {""},
#line 171 "hash4a.gperf"
      {"\311\251",127,201,0},
#line 62 "hash4a.gperf"
      {"\341\264\205",127,60,2},
#line 197 "hash4a.gperf"
      {"\312\236",127,232,0},
#line 193 "hash4a.gperf"
      {"\352\235\203",127,228,0},
      {""},
#line 161 "hash4a.gperf"
      {"i",127,188,1},
#line 93 "hash4a.gperf"
      {"\307\235",127,103,0},
#line 217 "hash4a.gperf"
      {"\311\254",127,254,0},
#line 126 "hash4a.gperf"
      {"\352\236\254",127,148,1},
      {""},
#line 99 "hash4a.gperf"
      {"\311\230",127,111,0},
#line 114 "hash4a.gperf"
      {"\352\236\230",127,134,1},
#line 86 "hash4a.gperf"
      {"\311\206",127,92,1},
#line 127 "hash4a.gperf"
      {"\352\254\266",127,150,0},
      {""},
#line 188 "hash4a.gperf"
      {"\306\230",127,223,1},
#line 206 "hash4a.gperf"
      {"\341\264\214",127,241,2},
#line 180 "hash4a.gperf"
      {"\312\235",127,212,0},
#line 35 "hash4a.gperf"
      {"\341\264\203",127,29,2},
      {""},
#line 69 "hash4a.gperf"
      {"\311\227",127,69,0},
#line 38 "hash4a.gperf"
      {"\352\236\227",127,33,0},
#line 235 "hash4a.gperf"
      {"\311\261",127,283,0},
#line 234 "hash4a.gperf"
      {"\341\266\206",127,282,0},
      {""},
#line 168 "hash4a.gperf"
      {"\306\227",127,197,1},
#line 67 "hash4a.gperf"
      {"\311\226",127,67,0},
#line 39 "hash4a.gperf"
      {"\352\236\226",127,34,1},
#line 203 "hash4a.gperf"
      {"\352\235\206",127,238,1},
      {""},
#line 141 "hash4a.gperf"
      {"h",127,166,0},
#line 172 "hash4a.gperf"
      {"\306\226",127,202,1},
#line 84 "hash4a.gperf"
      {"\352\254\263",127,90,0},
#line 88 "hash4a.gperf"
      {"\352\254\264",127,96,0},
      {""},
#line 167 "hash4a.gperf"
      {"\311\250",127,196,0},
#line 125 "hash4a.gperf"
      {"\311\241",127,147,0},
#line 170 "hash4a.gperf"
      {"\341\266\226",127,200,0},
#line 78 "hash4a.gperf"
      {"\352\235\261",127,81,0},
      {""},
#line 122 "hash4a.gperf"
      {"g",127,144,0},
#line 55 "hash4a.gperf"
      {"\311\225",127,53,0},
#line 148 "hash4a.gperf"
      {"\352\236\225",127,174,0},
#line 74 "hash4a.gperf"
      {"\341\264\206",127,75,2},
      {""},
#line 109 "hash4a.gperf"
      {"f",127,128,0},
#line 146 "hash4a.gperf"
      {"\306\225",127,171,0},
#line 101 "hash4a.gperf"
      {"\311\234",127,114,0},
#line 186 "hash4a.gperf"
      {"\341\266\204",127,221,0},
      {""},
#line 213 "hash4a.gperf"
      {"\311\253",127,248,0},
#line 102 "hash4a.gperf"
      {"\352\236\253",127,115,1},
#line 98 "hash4a.gperf"
      {"\341\266\225",127,109,0},
#line 196 "hash4a.gperf"
      {"\352\235\204",127,231,1},
      {""},
#line 80 "hash4a.gperf"
      {"e",127,84,0},
#line 41 "hash4a.gperf"
      {"\311\223",127,37,0},
#line 50 "hash4a.gperf"
      {"\352\236\223",127,46,0},
#line 216 "hash4a.gperf"
      {"\352\254\271",127,253,0},
      {""},
#line 60 "hash4a.gperf"
      {"d",127,58,0},
#line 132 "hash4a.gperf"
      {"\306\223",127,156,1},
#line 160 "hash4a.gperf"
      {"\304\261",127,187,0},
#line 153 "hash4a.gperf"
      {"\342\261\266",127,179,0},
      {""},
#line 45 "hash4a.gperf"
      {"c",127,41,0},
#line 97 "hash4a.gperf"
      {"\306\217",127,107,1},
#line 92 "hash4a.gperf"
      {"\341\266\223",127,102,0},
#line 47 "hash4a.gperf"
      {"\341\264\204",127,43,2},
      {""},
#line 30 "hash4a.gperf"
      {"b",127,24,0},
#line 189 "hash4a.gperf"
      {"\342\261\252",127,224,0},
#line 18 "hash4a.gperf"
      {"\341\266\217",127,7,0},
#line 20 "hash4a.gperf"
      {"\352\254\261",127,11,0},
      {""},
#line 13 "hash4a.gperf"
      {"a",127,1,0},
#line 27 "hash4a.gperf"
      {"\311\222",127,20,0},
#line 51 "hash4a.gperf"
      {"\352\236\222",127,47,1},
#line 57 "hash4a.gperf"
      {"\342\206\203",127,55,1},
      {""},
#line 242 "hash4a.gperf"
      {"N",127,292,1},
#line 117 "hash4a.gperf"
      {"\306\222",127,139,0},
#line 182 "hash4a.gperf"
      {"\312\204",127,216,0},
#line 112 "hash4a.gperf"
      {"\352\254\265",127,132,0},
      {""},
#line 76 "hash4a.gperf"
      {"\306\213",127,77,1},
#line 23 "hash4a.gperf"
      {"\311\221",127,14,0},
#line 87 "hash4a.gperf"
      {"\341\266\222",127,94,0},
#line 37 "hash4a.gperf"
      {"\341\265\254",127,32,0},
      {""},
#line 231 "hash4a.gperf"
      {"M",127,277,1},
#line 118 "hash4a.gperf"
      {"\306\221",127,140,1},
#line 143 "hash4a.gperf"
      {"\312\234",127,168,2},
#line 245 "hash4a.gperf"
      {"\341\264\273",127,295,1},
      {""},
#line 200 "hash4a.gperf"
      {"L",127,235,1},
#line 52 "hash4a.gperf"
      {"\352\236\224",127,49,0},
#line 71 "hash4a.gperf"
      {"\341\266\221",127,72,0},
#line 198 "hash4a.gperf"
      {"\352\236\260",127,233,1},
      {""},
#line 138 "hash4a.gperf"
      {"\306\224",127,163,1},
#line 190 "hash4a.gperf"
      {"\342\261\251",127,225,1},
#line 221 "hash4a.gperf"
      {"\311\255",127,261,0},
#line 218 "hash4a.gperf"
      {"\352\236\255",127,255,1},
      {""},
#line 184 "hash4a.gperf"
      {"K",127,218,1},
#line 103 "hash4a.gperf"
      {"\341\266\224",127,117,0},
#line 158 "hash4a.gperf"
      {"\312\273",127,185,0},
#line 222 "hash4a.gperf"
      {"\352\236\216",127,263,0},
      {""},
#line 176 "hash4a.gperf"
      {"J",127,207,2},
#line 185 "hash4a.gperf"
      {"\341\264\213",127,219,2},
#line 94 "hash4a.gperf"
      {"\306\216",127,104,1},
      {""}, {""},
#line 162 "hash4a.gperf"
      {"I",127,189,2},
#line 90 "hash4a.gperf"
      {"\311\233",127,99,0},
#line 96 "hash4a.gperf"
      {"\311\231",127,106,0},
#line 113 "hash4a.gperf"
      {"\352\236\231",127,133,0},
      {""},
#line 142 "hash4a.gperf"
      {"H",127,167,1},
#line 228 "hash4a.gperf"
      {"\306\233",127,273,0},
#line 187 "hash4a.gperf"
      {"\306\231",127,222,0},
#line 237 "hash4a.gperf"
      {"\352\254\272",127,286,0},
      {""},
#line 123 "hash4a.gperf"
      {"G",127,145,1},
#line 63 "hash4a.gperf"
      {"\304\221",127,61,0},
#line 159 "hash4a.gperf"
      {"\312\275",127,186,0},
#line 56 "hash4a.gperf"
      {"\342\206\204",127,54,0},
      {""},
#line 110 "hash4a.gperf"
      {"F",127,129,1},
#line 149 "hash4a.gperf"
      {"\311\246",127,175,0},
#line 108 "hash4a.gperf"
      {"\311\244",127,127,0},
#line 154 "hash4a.gperf"
      {"\342\261\265",127,180,1},
      {""},
#line 81 "hash4a.gperf"
      {"E",127,85,1},
#line 151 "hash4a.gperf"
      {"\342\261\250",127,177,0},
#line 211 "hash4a.gperf"
      {"\342\261\241",127,246,0},
#line 244 "hash4a.gperf"
      {"\341\264\216",127,294,0},
      {""},
#line 61 "hash4a.gperf"
      {"D",127,59,1},
#line 178 "hash4a.gperf"
      {"\311\211",127,209,0},
#line 157 "hash4a.gperf"
      {"\311\247",127,184,0},
#line 169 "hash4a.gperf"
      {"\341\265\273",127,198,2},
      {""},
#line 46 "hash4a.gperf"
      {"C",127,42,1},
#line 68 "hash4a.gperf"
      {"\306\211",127,68,1},
#line 44 "hash4a.gperf"
      {"\306\202",127,40,1},
#line 25 "hash4a.gperf"
      {"\352\254\260",127,17,0},
      {""},
#line 70 "hash4a.gperf"
      {"\306\212",127,70,1},
#line 131 "hash4a.gperf"
      {"\311\240",127,155,0},
#line 229 "hash4a.gperf"
      {"\312\216",127,275,0},
#line 116 "hash4a.gperf"
      {"\341\266\202",127,138,0},
      {""},
#line 31 "hash4a.gperf"
      {"B",127,25,1},
#line 119 "hash4a.gperf"
      {"\342\205\216",127,141,0},
#line 207 "hash4a.gperf"
      {"\352\235\211",127,242,0},
#line 194 "hash4a.gperf"
      {"\352\235\202",127,229,1},
      {""},
#line 14 "hash4a.gperf"
      {"A",127,2,1},
#line 133 "hash4a.gperf"
      {"\312\233",127,157,2},
#line 32 "hash4a.gperf"
      {"\312\231",127,26,2},
#line 95 "hash4a.gperf"
      {"\342\261\273",127,105,2},
      {""}, {""},
#line 129 "hash4a.gperf"
      {"\307\244",127,152,1},
#line 223 "hash4a.gperf"
      {"\310\264",127,265,0},
      {""}, {""}, {""},
#line 145 "hash4a.gperf"
      {"\304\246",127,170,1},
#line 166 "hash4a.gperf"
      {"\341\264\211",127,195,0},
#line 19 "hash4a.gperf"
      {"\341\264\202",127,9,0},
      {""}, {""},
#line 177 "hash4a.gperf"
      {"\341\264\212",127,208,3},
#line 247 "hash4a.gperf"
      {"\311\262",127,298,0},
#line 246 "hash4a.gperf"
      {"\341\265\260",127,297,0},
      {""}, {""},
#line 124 "hash4a.gperf"
      {"\311\242",127,146,2},
#line 144 "hash4a.gperf"
      {"\304\247",127,169,0},
#line 65 "hash4a.gperf"
      {"\341\265\255",127,64,0},
      {""}, {""},
#line 140 "hash4a.gperf"
      {"\306\242",127,165,1},
#line 135 "hash4a.gperf"
      {"\352\235\277",127,160,0},
#line 111 "hash4a.gperf"
      {"\352\234\260",127,130,2},
      {""}, {""},
#line 100 "hash4a.gperf"
      {"\311\232",127,113,0},
#line 181 "hash4a.gperf"
      {"\311\237",127,214,0},
#line 224 "hash4a.gperf"
      {"\352\235\262",127,267,0},
      {""}, {""},
#line 209 "hash4a.gperf"
      {"\306\232",127,244,0},
#line 136 "hash4a.gperf"
      {"\352\235\276",127,161,1},
#line 28 "hash4a.gperf"
      {"\342\261\260",127,21,1},
      {""}, {""},
#line 179 "hash4a.gperf"
      {"\311\210",127,210,1},
#line 21 "hash4a.gperf"
      {"\311\220",127,12,0},
#line 24 "hash4a.gperf"
      {"\342\261\255",127,15,1},
      {""}, {""},
#line 53 "hash4a.gperf"
      {"\306\210",127,50,0},
#line 91 "hash4a.gperf"
      {"\306\220",127,100,1},
#line 226 "hash4a.gperf"
      {"\352\236\201",127,270,0},
      {""}, {""},
#line 225 "hash4a.gperf"
      {"\311\256",127,269,0},
#line 42 "hash4a.gperf"
      {"\306\201",127,38,1},
#line 26 "hash4a.gperf"
      {"\341\266\220",127,19,0},
      {""}, {""},
#line 128 "hash4a.gperf"
      {"\307\245",127,151,0},
#line 208 "hash4a.gperf"
      {"\352\235\210",127,243,1},
#line 66 "hash4a.gperf"
      {"\341\266\201",127,66,0},
      {""}, {""},
#line 77 "hash4a.gperf"
      {"\310\241",127,79,0},
#line 156 "hash4a.gperf"
      {"\352\234\246",127,182,1},
#line 191 "hash4a.gperf"
      {"\352\235\201",127,226,0},
      {""}, {""}, {""},
#line 36 "hash4a.gperf"
      {"\341\264\257",127,30,0},
#line 83 "hash4a.gperf"
      {"\352\254\262",127,88,0},
      {""}, {""}, {""},
#line 104 "hash4a.gperf"
      {"\341\264\210",127,119,0},
#line 155 "hash4a.gperf"
      {"\352\234\247",127,181,0},
      {""}, {""},
#line 107 "hash4a.gperf"
      {"\312\232",127,125,0},
#line 201 "hash4a.gperf"
      {"\312\237",127,236,2},
#line 219 "hash4a.gperf"
      {"\352\254\267",127,257,0},
      {""}, {""}, {""},
#line 64 "hash4a.gperf"
      {"\304\220",127,62,1},
#line 152 "hash4a.gperf"
      {"\342\261\247",127,178,1},
      {""}, {""}, {""},
#line 49 "hash4a.gperf"
      {"\310\273",127,45,1},
      {""}, {""}, {""}, {""},
#line 212 "hash4a.gperf"
      {"\342\261\240",127,247,1},
#line 173 "hash4a.gperf"
      {"\341\265\274",127,204,0},
      {""}, {""}, {""},
#line 17 "hash4a.gperf"
      {"\310\272",127,5,1},
#line 215 "hash4a.gperf"
      {"\352\254\270",127,251,0},
      {""}, {""}, {""},
#line 58 "hash4a.gperf"
      {"\352\234\277",127,56,0},
      {""}, {""}, {""}, {""},
#line 210 "hash4a.gperf"
      {"\310\275",127,245,1},
#line 134 "hash4a.gperf"
      {"\341\265\267",127,159,0},
      {""}, {""}, {""},
#line 59 "hash4a.gperf"
      {"\352\234\276",127,57,1},
      {""}, {""}, {""}, {""},
#line 233 "hash4a.gperf"
      {"\341\265\257",127,280,0},
#line 29 "hash4a.gperf"
      {"\352\255\244",127,23,0},
      {""}, {""}, {""},
#line 214 "hash4a.gperf"
      {"\342\261\242",127,249,1},
#line 16 "hash4a.gperf"
      {"\342\261\245",127,4,0},
      {""}, {""}, {""},
#line 204 "hash4a.gperf"
      {"\305\202",127,239,0},
      {""}, {""}, {""}, {""},
#line 115 "hash4a.gperf"
      {"\341\265\256",127,136,0},
#line 120 "hash4a.gperf"
      {"\342\204\262",127,142,1},
      {""}, {""}, {""},
#line 22 "hash4a.gperf"
      {"\342\261\257",127,13,1},
#line 121 "hash4a.gperf"
      {"\352\237\273",127,143,0},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 72 "hash4a.gperf"
      {"\303\260",127,73,0},
#line 89 "hash4a.gperf"
      {"\342\261\270",127,98,0},
      {""}, {""}, {""},
#line 236 "hash4a.gperf"
      {"\342\261\256",127,284,1},
#line 79 "hash4a.gperf"
      {"\341\272\237",127,83,0},
      {""}, {""}, {""}, {""},
#line 238 "hash4a.gperf"
      {"\352\237\275",127,287,0},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 250 "hash4a.gperf"
      {"\310\240",127,301,1},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""},
#line 205 "hash4a.gperf"
      {"\305\201",127,240,1},
      {""}, {""}, {""}, {""},
#line 48 "hash4a.gperf"
      {"\310\274",127,44,0},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""},
#line 175 "hash4a.gperf"
      {"\310\267",127,206,1},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 239 "hash4a.gperf"
      {"\352\237\277",127,288,0},
      {""}, {""}, {""}, {""},
#line 73 "hash4a.gperf"
      {"\303\220",127,74,1},
      {""}, {""}, {""}, {""},
#line 164 "hash4a.gperf"
      {"\352\237\276",127,192,0},
      {""}, {""}, {""}, {""}, {""},
#line 165 "hash4a.gperf"
      {"\352\237\267",127,193,0}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register unsigned int key = hash_a (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
            return &wordlist[key];
        }
    }
  return 0;
}
