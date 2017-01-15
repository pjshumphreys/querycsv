/*CP437 mapping table*/
static const long
  hash1_0[] = {0x43, 0x327},  /* #LATIN CAPITAL LETTER C WITH CEDILLA */
  hash1_1[] = {0x75, 0x308},  /* #LATIN SMALL LETTER U WITH DIAERESIS */
  hash1_2[] = {0x65, 0x301},  /* #LATIN SMALL LETTER E WITH ACUTE */
  hash1_3[] = {0x61, 0x302},  /* #LATIN SMALL LETTER A WITH CIRCUMFLEX */
  hash1_4[] = {0x61, 0x308},  /* #LATIN SMALL LETTER A WITH DIAERESIS */
  hash1_5[] = {0x61, 0x300},  /* #LATIN SMALL LETTER A WITH GRAVE */
  hash1_6[] = {0x61, 0x30a},  /* #LATIN SMALL LETTER A WITH RING ABOVE */
  hash1_7[] = {0x63, 0x327},  /* #LATIN SMALL LETTER C WITH CEDILLA */
  hash1_8[] = {0x65, 0x302},  /* #LATIN SMALL LETTER E WITH CIRCUMFLEX */
  hash1_9[] = {0x65, 0x308},  /* #LATIN SMALL LETTER E WITH DIAERESIS */
  hash1_10[] = {0x65, 0x300},  /* #LATIN SMALL LETTER E WITH GRAVE */
  hash1_11[] = {0x69, 0x308},  /* #LATIN SMALL LETTER I WITH DIAERESIS */
  hash1_12[] = {0x69, 0x302},  /* #LATIN SMALL LETTER I WITH CIRCUMFLEX */
  hash1_13[] = {0x69, 0x300},  /* #LATIN SMALL LETTER I WITH GRAVE */
  hash1_14[] = {0x41, 0x308},  /* #LATIN CAPITAL LETTER A WITH DIAERESIS */
  hash1_15[] = {0x41, 0x30a},  /* #LATIN CAPITAL LETTER A WITH RING ABOVE */
  hash1_16[] = {0x45, 0x301},  /* #LATIN CAPITAL LETTER E WITH ACUTE */
  hash1_17[] = {0x61, 0x65},   /* #LATIN SMALL LIGATURE AE */
  hash1_18[] = {0x41, 0x45},   /* #LATIN CAPITAL LIGATURE AE */
  hash1_19[] = {0x6f, 0x302},  /* #LATIN SMALL LETTER O WITH CIRCUMFLEX */
  hash1_20[] = {0x6f, 0x308},  /* #LATIN SMALL LETTER O WITH DIAERESIS */
  hash1_21[] = {0x6f, 0x300},  /* #LATIN SMALL LETTER O WITH GRAVE */
  hash1_22[] = {0x75, 0x302},  /* #LATIN SMALL LETTER U WITH CIRCUMFLEX */
  hash1_23[] = {0x75, 0x300},  /* #LATIN SMALL LETTER U WITH GRAVE */
  hash1_24[] = {0x79, 0x308},  /* #LATIN SMALL LETTER Y WITH DIAERESIS */
  hash1_25[] = {0x4f, 0x308},  /* #LATIN CAPITAL LETTER O WITH DIAERESIS */
  hash1_26[] = {0x55, 0x308},  /* #LATIN CAPITAL LETTER U WITH DIAERESIS */
  hash1_27[] = {0xa2},         /* #CENT SIGN */
  hash1_28[] = {0xa3},         /* #POUND SIGN */
  hash1_29[] = {0xa5},         /* #YEN SIGN */
  hash1_30[] = {0x20a7},       /* #PESETA SIGN */
  hash1_31[] = {0x192},        /* #LATIN SMALL LETTER F WITH HOOK */
  hash1_32[] = {0x61, 0x301},  /* #LATIN SMALL LETTER A WITH ACUTE */
  hash1_33[] = {0x69, 0x301},  /* #LATIN SMALL LETTER I WITH ACUTE */
  hash1_34[] = {0x6f, 0x301},  /* #LATIN SMALL LETTER O WITH ACUTE */
  hash1_35[] = {0x75, 0x301},  /* #LATIN SMALL LETTER U WITH ACUTE */
  hash1_36[] = {0x6e, 0x303},  /* #LATIN SMALL LETTER N WITH TILDE */
  hash1_37[] = {0x4e, 0x303},  /* #LATIN CAPITAL LETTER N WITH TILDE */
  hash1_38[] = {0xaa},         /* #FEMININE ORDINAL INDICATOR */
  hash1_39[] = {0xba},         /* #MASCULINE ORDINAL INDICATOR */
  hash1_40[] = {0xbf},         /* #INVERTED QUESTION MARK */
  hash1_41[] = {0x2310},       /* #REVERSED NOT SIGN */
  hash1_42[] = {0xac},         /* #NOT SIGN */
  hash1_43[] = {0xbd},         /* #VULGAR FRACTION ONE HALF */
  hash1_44[] = {0xbc},         /* #VULGAR FRACTION ONE QUARTER */
  hash1_45[] = {0xa1},         /* #INVERTED EXCLAMATION MARK */
  hash1_46[] = {0xab},         /* #LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
  hash1_47[] = {0xbb},         /* #RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
  hash1_48[] = {0x2591},       /* #LIGHT SHADE */
  hash1_49[] = {0x2592},       /* #MEDIUM SHADE */
  hash1_50[] = {0x2593},       /* #DARK SHADE */
  hash1_51[] = {0x2502},       /* #BOX DRAWINGS LIGHT VERTICAL */
  hash1_52[] = {0x2524},       /* #BOX DRAWINGS LIGHT VERTICAL AND LEFT */
  hash1_53[] = {0x2561},       /* #BOX DRAWINGS VERTICAL SINGLE AND LEFT DOUBLE */
  hash1_54[] = {0x2562},       /* #BOX DRAWINGS VERTICAL DOUBLE AND LEFT SINGLE */
  hash1_55[] = {0x2556},       /* #BOX DRAWINGS DOWN DOUBLE AND LEFT SINGLE */
  hash1_56[] = {0x2555},       /* #BOX DRAWINGS DOWN SINGLE AND LEFT DOUBLE */
  hash1_57[] = {0x2563},       /* #BOX DRAWINGS DOUBLE VERTICAL AND LEFT */
  hash1_58[] = {0x2551},       /* #BOX DRAWINGS DOUBLE VERTICAL */
  hash1_59[] = {0x2557},       /* #BOX DRAWINGS DOUBLE DOWN AND LEFT */
  hash1_60[] = {0x255d},       /* #BOX DRAWINGS DOUBLE UP AND LEFT */
  hash1_61[] = {0x255c},       /* #BOX DRAWINGS UP DOUBLE AND LEFT SINGLE */
  hash1_62[] = {0x255b},       /* #BOX DRAWINGS UP SINGLE AND LEFT DOUBLE */
  hash1_63[] = {0x2510},       /* #BOX DRAWINGS LIGHT DOWN AND LEFT */
  hash1_64[] = {0x2514},       /* #BOX DRAWINGS LIGHT UP AND RIGHT */
  hash1_65[] = {0x2534},       /* #BOX DRAWINGS LIGHT UP AND HORIZONTAL */
  hash1_66[] = {0x252c},       /* #BOX DRAWINGS LIGHT DOWN AND HORIZONTAL */
  hash1_67[] = {0x251c},       /* #BOX DRAWINGS LIGHT VERTICAL AND RIGHT */
  hash1_68[] = {0x2500},       /* #BOX DRAWINGS LIGHT HORIZONTAL */
  hash1_69[] = {0x253c},       /* #BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL */
  hash1_70[] = {0x255e},       /* #BOX DRAWINGS VERTICAL SINGLE AND RIGHT DOUBLE */
  hash1_71[] = {0x255f},       /* #BOX DRAWINGS VERTICAL DOUBLE AND RIGHT SINGLE */
  hash1_72[] = {0x255a},       /* #BOX DRAWINGS DOUBLE UP AND RIGHT */
  hash1_73[] = {0x2554},       /* #BOX DRAWINGS DOUBLE DOWN AND RIGHT */
  hash1_74[] = {0x2569},       /* #BOX DRAWINGS DOUBLE UP AND HORIZONTAL */
  hash1_75[] = {0x2566},       /* #BOX DRAWINGS DOUBLE DOWN AND HORIZONTAL */
  hash1_76[] = {0x2560},       /* #BOX DRAWINGS DOUBLE VERTICAL AND RIGHT */
  hash1_77[] = {0x2550},       /* #BOX DRAWINGS DOUBLE HORIZONTAL */
  hash1_78[] = {0x256c},       /* #BOX DRAWINGS DOUBLE VERTICAL AND HORIZONTAL */
  hash1_79[] = {0x2567},       /* #BOX DRAWINGS UP SINGLE AND HORIZONTAL DOUBLE */
  hash1_80[] = {0x2568},       /* #BOX DRAWINGS UP DOUBLE AND HORIZONTAL SINGLE */
  hash1_81[] = {0x2564},       /* #BOX DRAWINGS DOWN SINGLE AND HORIZONTAL DOUBLE */
  hash1_82[] = {0x2565},       /* #BOX DRAWINGS DOWN DOUBLE AND HORIZONTAL SINGLE */
  hash1_83[] = {0x2559},       /* #BOX DRAWINGS UP DOUBLE AND RIGHT SINGLE */
  hash1_84[] = {0x2558},       /* #BOX DRAWINGS UP SINGLE AND RIGHT DOUBLE */
  hash1_85[] = {0x2552},       /* #BOX DRAWINGS DOWN SINGLE AND RIGHT DOUBLE */
  hash1_86[] = {0x2553},       /* #BOX DRAWINGS DOWN DOUBLE AND RIGHT SINGLE */
  hash1_87[] = {0x256b},       /* #BOX DRAWINGS VERTICAL DOUBLE AND HORIZONTAL SINGLE */
  hash1_88[] = {0x256a},       /* #BOX DRAWINGS VERTICAL SINGLE AND HORIZONTAL DOUBLE */
  hash1_89[] = {0x2518},       /* #BOX DRAWINGS LIGHT UP AND LEFT */
  hash1_90[] = {0x250c},       /* #BOX DRAWINGS LIGHT DOWN AND RIGHT */
  hash1_91[] = {0x2588},       /* #FULL BLOCK */
  hash1_92[] = {0x2584},       /* #LOWER HALF BLOCK */
  hash1_93[] = {0x258c},       /* #LEFT HALF BLOCK */
  hash1_94[] = {0x2590},       /* #RIGHT HALF BLOCK */
  hash1_95[] = {0x2580},       /* #UPPER HALF BLOCK */
  hash1_96[] = {0x3b1},        /* #GREEK SMALL LETTER ALPHA */
  hash1_97[] = {0x73, 0x7A},   /* #LATIN SMALL LETTER SHARP S */
  hash1_98[] = {0x393},        /* #GREEK CAPITAL LETTER GAMMA */
  hash1_99[] = {0x3c0},        /* #GREEK SMALL LETTER PI */
  hash1_100[] = {0x3a3},        /* #GREEK CAPITAL LETTER SIGMA */
  hash1_101[] = {0x3c3},        /* #GREEK SMALL LETTER SIGMA */
  hash1_102[] = {0xb5},         /* #MICRO SIGN */
  hash1_103[] = {0x3c4},        /* #GREEK SMALL LETTER TAU */
  hash1_104[] = {0x3a6},        /* #GREEK CAPITAL LETTER PHI */
  hash1_105[] = {0x398},        /* #GREEK CAPITAL LETTER THETA */
  hash1_106[] = {0x3a9},        /* #GREEK CAPITAL LETTER OMEGA */
  hash1_107[] = {0x3b4},        /* #GREEK SMALL LETTER DELTA */
  hash1_108[] = {0x221e},       /* #INFINITY */
  hash1_109[] = {0x3c6},        /* #GREEK SMALL LETTER PHI */
  hash1_110[] = {0x3b5},        /* #GREEK SMALL LETTER EPSILON */
  hash1_111[] = {0x2229},       /* #INTERSECTION */
  hash1_112[] = {0x2261},       /* #IDENTICAL TO */
  hash1_113[] = {0xb1},         /* #PLUS-MINUS SIGN */
  hash1_114[] = {0x2265},       /* #GREATER-THAN OR EQUAL TO */
  hash1_115[] = {0x2264},       /* #LESS-THAN OR EQUAL TO */
  hash1_116[] = {0x2320},       /* #TOP HALF INTEGRAL */
  hash1_117[] = {0x2321},       /* #BOTTOM HALF INTEGRAL */
  hash1_118[] = {0xf7},         /* #DIVISION SIGN */
  hash1_119[] = {0x2248},       /* #ALMOST EQUAL TO */
  hash1_120[] = {0xb0},         /* #DEGREE SIGN */
  hash1_121[] = {0x2219},       /* #BULLET OPERATOR */
  hash1_122[] = {0xb7},         /* #MIDDLE DOT */
  hash1_123[] = {0x221a},       /* #SQUARE ROOT */
  hash1_124[] = {0x207f},       /* #SUPERSCRIPT LATIN SMALL LETTER N */
  hash1_125[] = {0xb2},         /* #SUPERSCRIPT TWO */
  hash1_126[] = {0x25a0},       /* #BLACK SQUARE */
  hash1_127[] = {0xa0};          /* #NO-BREAK SPACE */
  
static const struct hash1Entry hash1[128] = {
  {2, hash1_0},  /* #LATIN CAPITAL LETTER C WITH CEDILLA */
  {2, hash1_1},  /* #LATIN SMALL LETTER U WITH DIAERESIS */
  {2, hash1_2},  /* #LATIN SMALL LETTER E WITH ACUTE */
  {2, hash1_3},  /* #LATIN SMALL LETTER A WITH CIRCUMFLEX */
  {2, hash1_4},  /* #LATIN SMALL LETTER A WITH DIAERESIS */
  {2, hash1_5},  /* #LATIN SMALL LETTER A WITH GRAVE */
  {2, hash1_6},  /* #LATIN SMALL LETTER A WITH RING ABOVE */
  {2, hash1_7},  /* #LATIN SMALL LETTER C WITH CEDILLA */
  {2, hash1_8},  /* #LATIN SMALL LETTER E WITH CIRCUMFLEX */
  {2, hash1_9},  /* #LATIN SMALL LETTER E WITH DIAERESIS */
  {2, hash1_10},  /* #LATIN SMALL LETTER E WITH GRAVE */
  {2, hash1_11},  /* #LATIN SMALL LETTER I WITH DIAERESIS */
  {2, hash1_12},  /* #LATIN SMALL LETTER I WITH CIRCUMFLEX */
  {2, hash1_13},  /* #LATIN SMALL LETTER I WITH GRAVE */
  {2, hash1_14},  /* #LATIN CAPITAL LETTER A WITH DIAERESIS */
  {2, hash1_15},  /* #LATIN CAPITAL LETTER A WITH RING ABOVE */
  {2, hash1_16},  /* #LATIN CAPITAL LETTER E WITH ACUTE */
  {2, hash1_17},         /* #LATIN SMALL LIGATURE AE */
  {2, hash1_18},         /* #LATIN CAPITAL LIGATURE AE */
  {2, hash1_19},  /* #LATIN SMALL LETTER O WITH CIRCUMFLEX */
  {2, hash1_20},  /* #LATIN SMALL LETTER O WITH DIAERESIS */
  {2, hash1_21},  /* #LATIN SMALL LETTER O WITH GRAVE */
  {2, hash1_22},  /* #LATIN SMALL LETTER U WITH CIRCUMFLEX */
  {2, hash1_23},  /* #LATIN SMALL LETTER U WITH GRAVE */
  {2, hash1_24},  /* #LATIN SMALL LETTER Y WITH DIAERESIS */
  {2, hash1_25},  /* #LATIN CAPITAL LETTER O WITH DIAERESIS */
  {2, hash1_26},  /* #LATIN CAPITAL LETTER U WITH DIAERESIS */
  {1, hash1_27},         /* #CENT SIGN */
  {1, hash1_28},         /* #POUND SIGN */
  {1, hash1_29},         /* #YEN SIGN */
  {1, hash1_30},       /* #PESETA SIGN */
  {1, hash1_31},        /* #LATIN SMALL LETTER F WITH HOOK */
  {2, hash1_32},  /* #LATIN SMALL LETTER A WITH ACUTE */
  {2, hash1_33},  /* #LATIN SMALL LETTER I WITH ACUTE */
  {2, hash1_34},  /* #LATIN SMALL LETTER O WITH ACUTE */
  {2, hash1_35},  /* #LATIN SMALL LETTER U WITH ACUTE */
  {2, hash1_36},  /* #LATIN SMALL LETTER N WITH TILDE */
  {2, hash1_37},  /* #LATIN CAPITAL LETTER N WITH TILDE */
  {1, hash1_38},         /* #FEMININE ORDINAL INDICATOR */
  {1, hash1_39},         /* #MASCULINE ORDINAL INDICATOR */
  {1, hash1_40},         /* #INVERTED QUESTION MARK */
  {1, hash1_41},       /* #REVERSED NOT SIGN */
  {1, hash1_42},         /* #NOT SIGN */
  {1, hash1_43},         /* #VULGAR FRACTION ONE HALF */
  {1, hash1_44},         /* #VULGAR FRACTION ONE QUARTER */
  {1, hash1_45},         /* #INVERTED EXCLAMATION MARK */
  {1, hash1_46},         /* #LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
  {1, hash1_47},         /* #RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
  {1, hash1_48},       /* #LIGHT SHADE */
  {1, hash1_49},       /* #MEDIUM SHADE */
  {1, hash1_50},       /* #DARK SHADE */
  {1, hash1_51},       /* #BOX DRAWINGS LIGHT VERTICAL */
  {1, hash1_52},       /* #BOX DRAWINGS LIGHT VERTICAL AND LEFT */
  {1, hash1_53},       /* #BOX DRAWINGS VERTICAL SINGLE AND LEFT DOUBLE */
  {1, hash1_54},       /* #BOX DRAWINGS VERTICAL DOUBLE AND LEFT SINGLE */
  {1, hash1_55},       /* #BOX DRAWINGS DOWN DOUBLE AND LEFT SINGLE */
  {1, hash1_56},       /* #BOX DRAWINGS DOWN SINGLE AND LEFT DOUBLE */
  {1, hash1_57},       /* #BOX DRAWINGS DOUBLE VERTICAL AND LEFT */
  {1, hash1_58},       /* #BOX DRAWINGS DOUBLE VERTICAL */
  {1, hash1_59},       /* #BOX DRAWINGS DOUBLE DOWN AND LEFT */
  {1, hash1_60},       /* #BOX DRAWINGS DOUBLE UP AND LEFT */
  {1, hash1_61},       /* #BOX DRAWINGS UP DOUBLE AND LEFT SINGLE */
  {1, hash1_62},       /* #BOX DRAWINGS UP SINGLE AND LEFT DOUBLE */
  {1, hash1_63},       /* #BOX DRAWINGS LIGHT DOWN AND LEFT */
  {1, hash1_64},       /* #BOX DRAWINGS LIGHT UP AND RIGHT */
  {1, hash1_65},       /* #BOX DRAWINGS LIGHT UP AND HORIZONTAL */
  {1, hash1_66},       /* #BOX DRAWINGS LIGHT DOWN AND HORIZONTAL */
  {1, hash1_67},       /* #BOX DRAWINGS LIGHT VERTICAL AND RIGHT */
  {1, hash1_68},       /* #BOX DRAWINGS LIGHT HORIZONTAL */
  {1, hash1_69},       /* #BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL */
  {1, hash1_70},       /* #BOX DRAWINGS VERTICAL SINGLE AND RIGHT DOUBLE */
  {1, hash1_71},       /* #BOX DRAWINGS VERTICAL DOUBLE AND RIGHT SINGLE */
  {1, hash1_72},       /* #BOX DRAWINGS DOUBLE UP AND RIGHT */
  {1, hash1_73},       /* #BOX DRAWINGS DOUBLE DOWN AND RIGHT */
  {1, hash1_74},       /* #BOX DRAWINGS DOUBLE UP AND HORIZONTAL */
  {1, hash1_75},       /* #BOX DRAWINGS DOUBLE DOWN AND HORIZONTAL */
  {1, hash1_76},       /* #BOX DRAWINGS DOUBLE VERTICAL AND RIGHT */
  {1, hash1_77},       /* #BOX DRAWINGS DOUBLE HORIZONTAL */
  {1, hash1_78},       /* #BOX DRAWINGS DOUBLE VERTICAL AND HORIZONTAL */
  {1, hash1_79},       /* #BOX DRAWINGS UP SINGLE AND HORIZONTAL DOUBLE */
  {1, hash1_80},       /* #BOX DRAWINGS UP DOUBLE AND HORIZONTAL SINGLE */
  {1, hash1_81},       /* #BOX DRAWINGS DOWN SINGLE AND HORIZONTAL DOUBLE */
  {1, hash1_82},       /* #BOX DRAWINGS DOWN DOUBLE AND HORIZONTAL SINGLE */
  {1, hash1_83},       /* #BOX DRAWINGS UP DOUBLE AND RIGHT SINGLE */
  {1, hash1_84},       /* #BOX DRAWINGS UP SINGLE AND RIGHT DOUBLE */
  {1, hash1_85},       /* #BOX DRAWINGS DOWN SINGLE AND RIGHT DOUBLE */
  {1, hash1_86},       /* #BOX DRAWINGS DOWN DOUBLE AND RIGHT SINGLE */
  {1, hash1_87},       /* #BOX DRAWINGS VERTICAL DOUBLE AND HORIZONTAL SINGLE */
  {1, hash1_88},       /* #BOX DRAWINGS VERTICAL SINGLE AND HORIZONTAL DOUBLE */
  {1, hash1_89},       /* #BOX DRAWINGS LIGHT UP AND LEFT */
  {1, hash1_90},       /* #BOX DRAWINGS LIGHT DOWN AND RIGHT */
  {1, hash1_91},       /* #FULL BLOCK */
  {1, hash1_92},       /* #LOWER HALF BLOCK */
  {1, hash1_93},       /* #LEFT HALF BLOCK */
  {1, hash1_94},       /* #RIGHT HALF BLOCK */
  {1, hash1_95},       /* #UPPER HALF BLOCK */
  {1, hash1_96},        /* #GREEK SMALL LETTER ALPHA */
  {2, hash1_97},         /* #LATIN SMALL LETTER SHARP S */
  {1, hash1_98},        /* #GREEK CAPITAL LETTER GAMMA */
  {1, hash1_99},        /* #GREEK SMALL LETTER PI */
  {1, hash1_100},        /* #GREEK CAPITAL LETTER SIGMA */
  {1, hash1_101},        /* #GREEK SMALL LETTER SIGMA */
  {1, hash1_102},         /* #MICRO SIGN */
  {1, hash1_103},        /* #GREEK SMALL LETTER TAU */
  {1, hash1_104},        /* #GREEK CAPITAL LETTER PHI */
  {1, hash1_105},        /* #GREEK CAPITAL LETTER THETA */
  {1, hash1_106},        /* #GREEK CAPITAL LETTER OMEGA */
  {1, hash1_107},        /* #GREEK SMALL LETTER DELTA */
  {1, hash1_108},       /* #INFINITY */
  {1, hash1_109},        /* #GREEK SMALL LETTER PHI */
  {1, hash1_110},        /* #GREEK SMALL LETTER EPSILON */
  {1, hash1_111},       /* #INTERSECTION */
  {1, hash1_112},       /* #IDENTICAL TO */
  {1, hash1_113},         /* #PLUS-MINUS SIGN */
  {1, hash1_114},       /* #GREATER-THAN OR EQUAL TO */
  {1, hash1_115},       /* #LESS-THAN OR EQUAL TO */
  {1, hash1_116},       /* #TOP HALF INTEGRAL */
  {1, hash1_117},       /* #BOTTOM HALF INTEGRAL */
  {1, hash1_118},         /* #DIVISION SIGN */
  {1, hash1_119},       /* #ALMOST EQUAL TO */
  {1, hash1_120},         /* #DEGREE SIGN */
  {1, hash1_121},       /* #BULLET OPERATOR */
  {1, hash1_122},         /* #MIDDLE DOT */
  {1, hash1_123},       /* #SQUARE ROOT */
  {1, hash1_124},       /* #SUPERSCRIPT LATIN SMALL LETTER N */
  {1, hash1_125},         /* #SUPERSCRIPT TWO */
  {1, hash1_126},       /* #BLACK SQUARE */
  {1, hash1_127}          /* #NO-BREAK SPACE */
};