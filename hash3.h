/* Unicode combining diacritic characters and their priorities */
struct hash3Entry {
  long codepoint;
  int priority;
  int order;
};

const struct hash3Entry hash3EntryMap[745] = {
  {0x300,230,336},
  {0x301,230,334},
  {0x302,230,338},
  {0x303,230,339},
  {0x304,230,333},
  {0x305,230,340},
  {0x306,230,337},
  {0x307,230,341},
  {0x308,230,342},
  {0x309,230,343},
  {0x30A,230,344},
  {0x30B,230,345},
  {0x30C,230,335},
  {0x30D,230,346},
  {0x30E,230,347},
  {0x30F,230,348},
  {0x310,230,349},
  {0x311,230,350},
  {0x312,230,351},
  {0x313,230,352},
  {0x314,230,353},
  {0x315,232,732},
  {0x316,220,171},
  {0x317,220,172},
  {0x318,220,173},
  {0x319,220,174},
  {0x31A,232,733},
  {0x31B,216,161},
  {0x31C,220,175},
  {0x31D,220,176},
  {0x31E,220,177},
  {0x31F,220,178},
  {0x320,220,179},
  {0x321,202,155},
  {0x322,202,156},
  {0x323,220,180},
  {0x324,220,181},
  {0x325,220,182},
  {0x326,220,183},
  {0x327,202,157},
  {0x328,202,158},
  {0x329,220,184},
  {0x32A,220,185},
  {0x32B,220,186},
  {0x32C,220,187},
  {0x32D,220,188},
  {0x32E,220,189},
  {0x32F,220,190},
  {0x330,220,191},
  {0x331,220,192},
  {0x332,220,193},
  {0x333,220,194},
  {0x334,1,1},
  {0x335,1,2},
  {0x336,1,3},
  {0x337,1,4},
  {0x338,1,5},
  {0x339,220,195},
  {0x33A,220,196},
  {0x33B,220,197},
  {0x33C,220,198},
  {0x33D,230,354},
  {0x33E,230,355},
  {0x33F,230,356},
  {0x340,230,357},
  {0x341,230,358},
  {0x342,230,359},
  {0x343,230,360},
  {0x344,230,361},
  {0x345,240,745},
  {0x346,230,362},
  {0x347,220,199},
  {0x348,220,200},
  {0x349,220,201},
  {0x34A,230,363},
  {0x34B,230,364},
  {0x34C,230,365},
  {0x34D,220,202},
  {0x34E,220,203},
  {0x350,230,366},
  {0x351,230,367},
  {0x352,230,368},
  {0x353,220,204},
  {0x354,220,205},
  {0x355,220,206},
  {0x356,220,207},
  {0x357,230,369},
  {0x358,232,734},
  {0x359,220,208},
  {0x35A,220,209},
  {0x35B,230,370},
  {0x35C,233,736},
  {0x35D,234,740},
  {0x35E,234,741},
  {0x35F,233,737},
  {0x360,234,742},
  {0x361,234,743},
  {0x362,233,738},
  {0x363,230,371},
  {0x364,230,372},
  {0x365,230,373},
  {0x366,230,374},
  {0x367,230,375},
  {0x368,230,376},
  {0x369,230,377},
  {0x36A,230,378},
  {0x36B,230,379},
  {0x36C,230,380},
  {0x36D,230,381},
  {0x36E,230,382},
  {0x36F,230,383},
  {0x483,230,384},
  {0x484,230,385},
  {0x485,230,386},
  {0x486,230,387},
  {0x487,230,388},
  {0x591,220,210},
  {0x592,230,389},
  {0x593,230,390},
  {0x594,230,391},
  {0x595,230,392},
  {0x596,220,211},
  {0x597,230,393},
  {0x598,230,394},
  {0x599,230,395},
  {0x59A,222,323},
  {0x59B,220,212},
  {0x59C,230,396},
  {0x59D,230,397},
  {0x59E,230,398},
  {0x59F,230,399},
  {0x5A0,230,400},
  {0x5A1,230,401},
  {0x5A2,220,213},
  {0x5A3,220,214},
  {0x5A4,220,215},
  {0x5A5,220,216},
  {0x5A6,220,217},
  {0x5A7,220,218},
  {0x5A8,230,402},
  {0x5A9,230,403},
  {0x5AA,220,219},
  {0x5AB,230,404},
  {0x5AC,230,405},
  {0x5AD,222,324},
  {0x5AE,228,330},
  {0x5AF,230,406},
  {0x5B0,10,98},
  {0x5B1,11,99},
  {0x5B2,12,100},
  {0x5B3,13,101},
  {0x5B4,14,102},
  {0x5B5,15,103},
  {0x5B6,16,104},
  {0x5B7,17,105},
  {0x5B8,18,106},
  {0x5B9,19,108},
  {0x5BA,19,109},
  {0x5BB,20,110},
  {0x5BC,21,111},
  {0x5BD,22,112},
  {0x5BF,23,113},
  {0x5C1,24,114},
  {0x5C2,25,115},
  {0x5C4,230,407},
  {0x5C5,220,220},
  {0x5C7,18,107},
  {0x610,230,408},
  {0x611,230,409},
  {0x612,230,410},
  {0x613,230,411},
  {0x614,230,412},
  {0x615,230,413},
  {0x616,230,414},
  {0x617,230,415},
  {0x618,30,123},
  {0x619,31,125},
  {0x61A,32,127},
  {0x64B,27,117},
  {0x64C,28,119},
  {0x64D,29,121},
  {0x64E,30,124},
  {0x64F,31,126},
  {0x650,32,128},
  {0x651,33,129},
  {0x652,34,130},
  {0x653,230,416},
  {0x654,230,417},
  {0x655,220,221},
  {0x656,220,222},
  {0x657,230,418},
  {0x658,230,419},
  {0x659,230,420},
  {0x65A,230,421},
  {0x65B,230,422},
  {0x65C,220,223},
  {0x65D,230,423},
  {0x65E,230,424},
  {0x65F,220,224},
  {0x670,35,131},
  {0x6D6,230,425},
  {0x6D7,230,426},
  {0x6D8,230,427},
  {0x6D9,230,428},
  {0x6DA,230,429},
  {0x6DB,230,430},
  {0x6DC,230,431},
  {0x6DF,230,432},
  {0x6E0,230,433},
  {0x6E1,230,434},
  {0x6E2,230,435},
  {0x6E3,220,225},
  {0x6E4,230,436},
  {0x6E7,230,437},
  {0x6E8,230,438},
  {0x6EA,220,226},
  {0x6EB,230,439},
  {0x6EC,230,440},
  {0x6ED,220,227},
  {0x711,36,132},
  {0x730,230,441},
  {0x731,220,228},
  {0x732,230,442},
  {0x733,230,443},
  {0x734,220,229},
  {0x735,230,444},
  {0x736,230,445},
  {0x737,220,230},
  {0x738,220,231},
  {0x739,220,232},
  {0x73A,230,446},
  {0x73B,220,233},
  {0x73C,220,234},
  {0x73D,230,447},
  {0x73E,220,235},
  {0x73F,230,448},
  {0x740,230,449},
  {0x741,230,450},
  {0x742,220,236},
  {0x743,230,451},
  {0x744,220,237},
  {0x745,230,452},
  {0x746,220,238},
  {0x747,230,453},
  {0x748,220,239},
  {0x749,230,454},
  {0x74A,230,455},
  {0x7EB,230,456},
  {0x7EC,230,457},
  {0x7ED,230,458},
  {0x7EE,230,459},
  {0x7EF,230,460},
  {0x7F0,230,461},
  {0x7F1,230,462},
  {0x7F2,220,240},
  {0x7F3,230,463},
  {0x816,230,464},
  {0x817,230,465},
  {0x818,230,466},
  {0x819,230,467},
  {0x81B,230,468},
  {0x81C,230,469},
  {0x81D,230,470},
  {0x81E,230,471},
  {0x81F,230,472},
  {0x820,230,473},
  {0x821,230,474},
  {0x822,230,475},
  {0x823,230,476},
  {0x825,230,477},
  {0x826,230,478},
  {0x827,230,479},
  {0x829,230,480},
  {0x82A,230,481},
  {0x82B,230,482},
  {0x82C,230,483},
  {0x82D,230,484},
  {0x859,220,241},
  {0x85A,220,242},
  {0x85B,220,243},
  {0x8E4,230,485},
  {0x8E5,230,486},
  {0x8E6,220,244},
  {0x8E7,230,487},
  {0x8E8,230,488},
  {0x8E9,220,245},
  {0x8EA,230,489},
  {0x8EB,230,490},
  {0x8EC,230,491},
  {0x8ED,220,246},
  {0x8EE,220,247},
  {0x8EF,220,248},
  {0x8F0,27,118},
  {0x8F1,28,120},
  {0x8F2,29,122},
  {0x8F3,230,492},
  {0x8F4,230,493},
  {0x8F5,230,494},
  {0x8F6,220,249},
  {0x8F7,230,495},
  {0x8F8,230,496},
  {0x8F9,220,250},
  {0x8FA,220,251},
  {0x8FB,230,497},
  {0x8FC,230,498},
  {0x8FD,230,499},
  {0x8FE,230,500},
  {0x8FF,230,501},
  {0x93C,7,33},
  {0x94D,9,54},
  {0x951,230,502},
  {0x952,220,252},
  {0x953,230,503},
  {0x954,230,504},
  {0x9BC,7,34},
  {0x9CD,9,55},
  {0xA3C,7,35},
  {0xA4D,9,56},
  {0xABC,7,36},
  {0xACD,9,57},
  {0xB3C,7,37},
  {0xB4D,9,58},
  {0xBCD,9,59},
  {0xC4D,9,60},
  {0xC55,84,133},
  {0xC56,91,134},
  {0xCBC,7,38},
  {0xCCD,9,61},
  {0xD4D,9,62},
  {0xDCA,9,63},
  {0xE38,103,135},
  {0xE39,103,136},
  {0xE3A,9,64},
  {0xE48,107,137},
  {0xE49,107,138},
  {0xE4A,107,139},
  {0xE4B,107,140},
  {0xEB8,118,141},
  {0xEB9,118,142},
  {0xEC8,122,143},
  {0xEC9,122,144},
  {0xECA,122,145},
  {0xECB,122,146},
  {0xF18,220,253},
  {0xF19,220,254},
  {0xF35,220,255},
  {0xF37,220,256},
  {0xF39,216,162},
  {0xF71,129,147},
  {0xF72,130,148},
  {0xF74,132,154},
  {0xF7A,130,149},
  {0xF7B,130,150},
  {0xF7C,130,151},
  {0xF7D,130,152},
  {0xF80,130,153},
  {0xF82,230,505},
  {0xF83,230,506},
  {0xF84,9,65},
  {0xF86,230,507},
  {0xF87,230,508},
  {0xFC6,220,257},
  {0x1037,7,39},
  {0x1039,9,66},
  {0x103A,9,67},
  {0x108D,220,258},
  {0x135D,230,509},
  {0x135E,230,510},
  {0x135F,230,511},
  {0x1714,9,68},
  {0x1734,9,69},
  {0x17D2,9,70},
  {0x17DD,230,512},
  {0x18A9,228,331},
  {0x1939,222,325},
  {0x193A,230,513},
  {0x193B,220,259},
  {0x1A17,230,514},
  {0x1A18,220,260},
  {0x1A60,9,71},
  {0x1A75,230,515},
  {0x1A76,230,516},
  {0x1A77,230,517},
  {0x1A78,230,518},
  {0x1A79,230,519},
  {0x1A7A,230,520},
  {0x1A7B,230,521},
  {0x1A7C,230,522},
  {0x1A7F,220,261},
  {0x1AB0,230,523},
  {0x1AB1,230,524},
  {0x1AB2,230,525},
  {0x1AB3,230,526},
  {0x1AB4,230,527},
  {0x1AB5,220,262},
  {0x1AB6,220,263},
  {0x1AB7,220,264},
  {0x1AB8,220,265},
  {0x1AB9,220,266},
  {0x1ABA,220,267},
  {0x1ABB,230,528},
  {0x1ABC,230,529},
  {0x1ABD,220,268},
  {0x1B34,7,40},
  {0x1B44,9,72},
  {0x1B6B,230,530},
  {0x1B6C,220,269},
  {0x1B6D,230,531},
  {0x1B6E,230,532},
  {0x1B6F,230,533},
  {0x1B70,230,534},
  {0x1B71,230,535},
  {0x1B72,230,536},
  {0x1B73,230,537},
  {0x1BAA,9,73},
  {0x1BAB,9,74},
  {0x1BE6,7,41},
  {0x1BF2,9,75},
  {0x1BF3,9,76},
  {0x1C37,7,42},
  {0x1CD0,230,538},
  {0x1CD1,230,539},
  {0x1CD2,230,540},
  {0x1CD4,1,6},
  {0x1CD5,220,270},
  {0x1CD6,220,271},
  {0x1CD7,220,272},
  {0x1CD8,220,273},
  {0x1CD9,220,274},
  {0x1CDA,230,541},
  {0x1CDB,230,542},
  {0x1CDC,220,275},
  {0x1CDD,220,276},
  {0x1CDE,220,277},
  {0x1CDF,220,278},
  {0x1CE0,230,543},
  {0x1CE2,1,7},
  {0x1CE3,1,8},
  {0x1CE4,1,9},
  {0x1CE5,1,10},
  {0x1CE6,1,11},
  {0x1CE7,1,12},
  {0x1CE8,1,13},
  {0x1CED,220,279},
  {0x1CF4,230,544},
  {0x1CF8,230,545},
  {0x1CF9,230,546},
  {0x1DC0,230,547},
  {0x1DC1,230,548},
  {0x1DC2,220,280},
  {0x1DC3,230,549},
  {0x1DC4,230,550},
  {0x1DC5,230,551},
  {0x1DC6,230,552},
  {0x1DC7,230,553},
  {0x1DC8,230,554},
  {0x1DC9,230,555},
  {0x1DCA,220,281},
  {0x1DCB,230,556},
  {0x1DCC,230,557},
  {0x1DCD,234,744},
  {0x1DCE,214,160},
  {0x1DCF,220,282},
  {0x1DD0,202,159},
  {0x1DD1,230,558},
  {0x1DD2,230,559},
  {0x1DD3,230,560},
  {0x1DD4,230,561},
  {0x1DD5,230,562},
  {0x1DD6,230,563},
  {0x1DD7,230,564},
  {0x1DD8,230,565},
  {0x1DD9,230,566},
  {0x1DDA,230,567},
  {0x1DDB,230,568},
  {0x1DDC,230,569},
  {0x1DDD,230,570},
  {0x1DDE,230,571},
  {0x1DDF,230,572},
  {0x1DE0,230,573},
  {0x1DE1,230,574},
  {0x1DE2,230,575},
  {0x1DE3,230,576},
  {0x1DE4,230,577},
  {0x1DE5,230,578},
  {0x1DE6,230,579},
  {0x1DE7,230,580},
  {0x1DE8,230,581},
  {0x1DE9,230,582},
  {0x1DEA,230,583},
  {0x1DEB,230,584},
  {0x1DEC,230,585},
  {0x1DED,230,586},
  {0x1DEE,230,587},
  {0x1DEF,230,588},
  {0x1DF0,230,589},
  {0x1DF1,230,590},
  {0x1DF2,230,591},
  {0x1DF3,230,592},
  {0x1DF4,230,593},
  {0x1DF5,230,594},
  {0x1DFC,233,739},
  {0x1DFD,220,283},
  {0x1DFE,230,595},
  {0x1DFF,220,284},
  {0x20D0,230,596},
  {0x20D1,230,597},
  {0x20D2,1,14},
  {0x20D3,1,15},
  {0x20D4,230,598},
  {0x20D5,230,599},
  {0x20D6,230,600},
  {0x20D7,230,601},
  {0x20D8,1,16},
  {0x20D9,1,17},
  {0x20DA,1,18},
  {0x20DB,230,602},
  {0x20DC,230,603},
  {0x20E1,230,604},
  {0x20E5,1,19},
  {0x20E6,1,20},
  {0x20E7,230,605},
  {0x20E8,220,285},
  {0x20E9,230,606},
  {0x20EA,1,21},
  {0x20EB,1,22},
  {0x20EC,220,286},
  {0x20ED,220,287},
  {0x20EE,220,288},
  {0x20EF,220,289},
  {0x20F0,230,607},
  {0x2CEF,230,608},
  {0x2CF0,230,609},
  {0x2CF1,230,610},
  {0x2D7F,9,77},
  {0x2DE0,230,611},
  {0x2DE1,230,612},
  {0x2DE2,230,613},
  {0x2DE3,230,614},
  {0x2DE4,230,615},
  {0x2DE5,230,616},
  {0x2DE6,230,617},
  {0x2DE7,230,618},
  {0x2DE8,230,619},
  {0x2DE9,230,620},
  {0x2DEA,230,621},
  {0x2DEB,230,622},
  {0x2DEC,230,623},
  {0x2DED,230,624},
  {0x2DEE,230,625},
  {0x2DEF,230,626},
  {0x2DF0,230,627},
  {0x2DF1,230,628},
  {0x2DF2,230,629},
  {0x2DF3,230,630},
  {0x2DF4,230,631},
  {0x2DF5,230,632},
  {0x2DF6,230,633},
  {0x2DF7,230,634},
  {0x2DF8,230,635},
  {0x2DF9,230,636},
  {0x2DFA,230,637},
  {0x2DFB,230,638},
  {0x2DFC,230,639},
  {0x2DFD,230,640},
  {0x2DFE,230,641},
  {0x2DFF,230,642},
  {0x302A,218,170},
  {0x302B,228,332},
  {0x302C,232,735},
  {0x302D,222,326},
  {0x302E,224,327},
  {0x302F,224,328},
  {0x3099,8,52},
  {0x309A,8,53},
  {0xA66F,230,643},
  {0xA674,230,644},
  {0xA675,230,645},
  {0xA676,230,646},
  {0xA677,230,647},
  {0xA678,230,648},
  {0xA679,230,649},
  {0xA67A,230,650},
  {0xA67B,230,651},
  {0xA67C,230,652},
  {0xA67D,230,653},
  {0xA69F,230,654},
  {0xA6F0,230,655},
  {0xA6F1,230,656},
  {0xA806,9,78},
  {0xA8C4,9,79},
  {0xA8E0,230,657},
  {0xA8E1,230,658},
  {0xA8E2,230,659},
  {0xA8E3,230,660},
  {0xA8E4,230,661},
  {0xA8E5,230,662},
  {0xA8E6,230,663},
  {0xA8E7,230,664},
  {0xA8E8,230,665},
  {0xA8E9,230,666},
  {0xA8EA,230,667},
  {0xA8EB,230,668},
  {0xA8EC,230,669},
  {0xA8ED,230,670},
  {0xA8EE,230,671},
  {0xA8EF,230,672},
  {0xA8F0,230,673},
  {0xA8F1,230,674},
  {0xA92B,220,290},
  {0xA92C,220,291},
  {0xA92D,220,292},
  {0xA953,9,80},
  {0xA9B3,7,43},
  {0xA9C0,9,81},
  {0xAAB0,230,675},
  {0xAAB2,230,676},
  {0xAAB3,230,677},
  {0xAAB4,220,293},
  {0xAAB7,230,678},
  {0xAAB8,230,679},
  {0xAABE,230,680},
  {0xAABF,230,681},
  {0xAAC1,230,682},
  {0xAAF6,9,82},
  {0xABED,9,83},
  {0xFB1E,26,116},
  {0xFE20,230,683},
  {0xFE21,230,684},
  {0xFE22,230,685},
  {0xFE23,230,686},
  {0xFE24,230,687},
  {0xFE25,230,688},
  {0xFE26,230,689},
  {0xFE27,220,294},
  {0xFE28,220,295},
  {0xFE29,220,296},
  {0xFE2A,220,297},
  {0xFE2B,220,298},
  {0xFE2C,220,299},
  {0xFE2D,220,300},
  {0x101FD,220,301},
  {0x102E0,220,302},
  {0x10376,230,690},
  {0x10377,230,691},
  {0x10378,230,692},
  {0x10379,230,693},
  {0x1037A,230,694},
  {0x10A0D,220,303},
  {0x10A0F,230,695},
  {0x10A38,230,696},
  {0x10A39,1,23},
  {0x10A3A,220,304},
  {0x10A3F,9,84},
  {0x10AE5,230,697},
  {0x10AE6,220,305},
  {0x11046,9,85},
  {0x1107F,9,86},
  {0x110B9,9,87},
  {0x110BA,7,44},
  {0x11100,230,698},
  {0x11101,230,699},
  {0x11102,230,700},
  {0x11133,9,88},
  {0x11134,9,89},
  {0x11173,7,45},
  {0x111C0,9,90},
  {0x11235,9,91},
  {0x11236,7,46},
  {0x112E9,7,47},
  {0x112EA,9,92},
  {0x1133C,7,48},
  {0x1134D,9,93},
  {0x11366,230,701},
  {0x11367,230,702},
  {0x11368,230,703},
  {0x11369,230,704},
  {0x1136A,230,705},
  {0x1136B,230,706},
  {0x1136C,230,707},
  {0x11370,230,708},
  {0x11371,230,709},
  {0x11372,230,710},
  {0x11373,230,711},
  {0x11374,230,712},
  {0x114C2,9,94},
  {0x114C3,7,49},
  {0x115BF,9,95},
  {0x115C0,7,50},
  {0x1163F,9,96},
  {0x116B6,9,97},
  {0x116B7,7,51},
  {0x16AF0,1,24},
  {0x16AF1,1,25},
  {0x16AF2,1,26},
  {0x16AF3,1,27},
  {0x16AF4,1,28},
  {0x16B30,230,713},
  {0x16B31,230,714},
  {0x16B32,230,715},
  {0x16B33,230,716},
  {0x16B34,230,717},
  {0x16B35,230,718},
  {0x16B36,230,719},
  {0x1BC9E,1,29},
  {0x1D165,216,163},
  {0x1D166,216,164},
  {0x1D167,1,30},
  {0x1D168,1,31},
  {0x1D169,1,32},
  {0x1D16D,226,329},
  {0x1D16E,216,165},
  {0x1D16F,216,166},
  {0x1D170,216,167},
  {0x1D171,216,168},
  {0x1D172,216,169},
  {0x1D17B,220,306},
  {0x1D17C,220,307},
  {0x1D17D,220,308},
  {0x1D17E,220,309},
  {0x1D17F,220,310},
  {0x1D180,220,311},
  {0x1D181,220,312},
  {0x1D182,220,313},
  {0x1D185,230,720},
  {0x1D186,230,721},
  {0x1D187,230,722},
  {0x1D188,230,723},
  {0x1D189,230,724},
  {0x1D18A,220,314},
  {0x1D18B,220,315},
  {0x1D1AA,230,725},
  {0x1D1AB,230,726},
  {0x1D1AC,230,727},
  {0x1D1AD,230,728},
  {0x1D242,230,729},
  {0x1D243,230,730},
  {0x1D244,230,731},
  {0x1E8D0,220,316},
  {0x1E8D1,220,317},
  {0x1E8D2,220,318},
  {0x1E8D3,220,319},
  {0x1E8D4,220,320},
  {0x1E8D5,220,321},
  {0x1E8D6,220,322}
};
