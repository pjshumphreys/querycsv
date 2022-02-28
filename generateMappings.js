const fs = require('fs');

const matchOperatorsRe = /[|\\\{\}\(\)\[\]^$+*?.]/g;
const replaceAll = (str, mapObj) => {

  const retval = new RegExp(
    Object.
      keys(mapObj).
      map(x => {
        return ""+(x.replace(matchOperatorsRe, "\\$&"))+"";
      }).
      join("|"),
    "gi"
  );

  return str.replace(retval, matched => mapObj[matched])
};


const mappings = {
  /* Unicode combining diacritic characters and their priorities */
  hash3EntryMap: {
    type: 'struct hash3Entry',
    define: 'SIZE_HASH3',
    filename: 'hash3',
    values: [
      [0x0300,/*230,*/334],
      [0x0301,/*230,*/332],
      [0x0302,/*230,*/336],
      [0x0303,/*230,*/337],
      [0x0304,/*230,*/331],
      [0x0305,/*230,*/338],
      [0x0306,/*230,*/335],
      [0x0307,/*230,*/339],
      [0x0308,/*230,*/340],
      [0x0309,/*230,*/341],
      [0x030A,/*230,*/342],
      [0x030B,/*230,*/343],
      [0x030C,/*230,*/333],
      [0x030D,/*230,*/344],
      [0x030E,/*230,*/345],
      [0x030F,/*230,*/346],
      [0x0310,/*230,*/347],
      [0x0311,/*230,*/348],
      [0x0312,/*230,*/349],
      [0x0313,/*230,*/350],
      [0x0314,/*230,*/351],
      [0x0315,/*232,*/732],
      [0x0316,/*220,*/169],
      [0x0317,/*220,*/170],
      [0x0318,/*220,*/171],
      [0x0319,/*220,*/172],
      [0x031A,/*232,*/733],
      [0x031B,/*216,*/159],
      [0x031C,/*220,*/173],
      [0x031D,/*220,*/174],
      [0x031E,/*220,*/175],
      [0x031F,/*220,*/176],
      [0x0320,/*220,*/177],
      [0x0321,/*202,*/155],
      [0x0322,/*202,*/156],
      [0x0323,/*220,*/178],
      [0x0324,/*220,*/179],
      [0x0325,/*220,*/180],
      [0x0326,/*220,*/181],
      [0x0327,/*202,*/352],
      [0x0328,/*202,*/353],
      [0x0329,/*220,*/182],
      [0x032A,/*220,*/183],
      [0x032B,/*220,*/184],
      [0x032C,/*220,*/185],
      [0x032D,/*220,*/186],
      [0x032E,/*220,*/187],
      [0x032F,/*220,*/188],
      [0x0330,/*220,*/189],
      [0x0331,/*220,*/190],
      [0x0332,/*220,*/191],
      [0x0333,/*220,*/192],
      [0x0334,/*1,*/1],
      [0x0335,/*1,*/2],
      [0x0336,/*1,*/3],
      [0x0337,/*1,*/4],
      [0x0338,/*1,*/5],
      [0x0339,/*220,*/193],
      [0x033A,/*220,*/194],
      [0x033B,/*220,*/195],
      [0x033C,/*220,*/196],
      [0x033D,/*230,*/354],
      [0x033E,/*230,*/355],
      [0x033F,/*230,*/356],
      [0x0340,/*230,*/357],
      [0x0341,/*230,*/358],
      [0x0342,/*230,*/359],
      [0x0343,/*230,*/360],
      [0x0344,/*230,*/361],
      [0x0345,/*240,*/745],
      [0x0346,/*230,*/362],
      [0x0347,/*220,*/197],
      [0x0348,/*220,*/198],
      [0x0349,/*220,*/199],
      [0x034A,/*230,*/363],
      [0x034B,/*230,*/364],
      [0x034C,/*230,*/365],
      [0x034D,/*220,*/200],
      [0x034E,/*220,*/201],
      [0x0350,/*230,*/366],
      [0x0351,/*230,*/367],
      [0x0352,/*230,*/368],
      [0x0353,/*220,*/202],
      [0x0354,/*220,*/203],
      [0x0355,/*220,*/204],
      [0x0356,/*220,*/205],
      [0x0357,/*230,*/369],
      [0x0358,/*232,*/734],
      [0x0359,/*220,*/206],
      [0x035A,/*220,*/207],
      [0x035B,/*230,*/370],
      [0x035C,/*233,*/736],
      [0x035D,/*234,*/740],
      [0x035E,/*234,*/741],
      [0x035F,/*233,*/737],
      [0x0360,/*234,*/742],
      [0x0361,/*234,*/743],
      [0x0362,/*233,*/738],
      [0x0363,/*230,*/371],
      [0x0364,/*230,*/372],
      [0x0365,/*230,*/373],
      [0x0366,/*230,*/374],
      [0x0367,/*230,*/375],
      [0x0368,/*230,*/376],
      [0x0369,/*230,*/377],
      [0x036A,/*230,*/378],
      [0x036B,/*230,*/379],
      [0x036C,/*230,*/380],
      [0x036D,/*230,*/381],
      [0x036E,/*230,*/382],
      [0x036F,/*230,*/383],
      [0x0483,/*230,*/384],
      [0x0484,/*230,*/385],
      [0x0485,/*230,*/386],
      [0x0486,/*230,*/387],
      [0x0487,/*230,*/388],
      [0x0591,/*220,*/208],
      [0x0592,/*230,*/389],
      [0x0593,/*230,*/390],
      [0x0594,/*230,*/391],
      [0x0595,/*230,*/392],
      [0x0596,/*220,*/209],
      [0x0597,/*230,*/393],
      [0x0598,/*230,*/394],
      [0x0599,/*230,*/395],
      [0x059A,/*222,*/321],
      [0x059B,/*220,*/210],
      [0x059C,/*230,*/396],
      [0x059D,/*230,*/397],
      [0x059E,/*230,*/398],
      [0x059F,/*230,*/399],
      [0x05A0,/*230,*/400],
      [0x05A1,/*230,*/401],
      [0x05A2,/*220,*/211],
      [0x05A3,/*220,*/212],
      [0x05A4,/*220,*/213],
      [0x05A5,/*220,*/214],
      [0x05A6,/*220,*/215],
      [0x05A7,/*220,*/216],
      [0x05A8,/*230,*/402],
      [0x05A9,/*230,*/403],
      [0x05AA,/*220,*/217],
      [0x05AB,/*230,*/404],
      [0x05AC,/*230,*/405],
      [0x05AD,/*222,*/322],
      [0x05AE,/*228,*/328],
      [0x05AF,/*230,*/406],
      [0x05B0,/*10,*/98],
      [0x05B1,/*11,*/99],
      [0x05B2,/*12,*/100],
      [0x05B3,/*13,*/101],
      [0x05B4,/*14,*/102],
      [0x05B5,/*15,*/103],
      [0x05B6,/*16,*/104],
      [0x05B7,/*17,*/105],
      [0x05B8,/*18,*/106],
      [0x05B9,/*19,*/108],
      [0x05BA,/*19,*/109],
      [0x05BB,/*20,*/110],
      [0x05BC,/*21,*/111],
      [0x05BD,/*22,*/112],
      [0x05BF,/*23,*/113],
      [0x05C1,/*24,*/114],
      [0x05C2,/*25,*/115],
      [0x05C4,/*230,*/407],
      [0x05C5,/*220,*/218],
      [0x05C7,/*18,*/107],
      [0x0610,/*230,*/408],
      [0x0611,/*230,*/409],
      [0x0612,/*230,*/410],
      [0x0613,/*230,*/411],
      [0x0614,/*230,*/412],
      [0x0615,/*230,*/413],
      [0x0616,/*230,*/414],
      [0x0617,/*230,*/415],
      [0x0618,/*30,*/123],
      [0x0619,/*31,*/125],
      [0x061A,/*32,*/127],
      [0x064B,/*27,*/117],
      [0x064C,/*28,*/119],
      [0x064D,/*29,*/121],
      [0x064E,/*30,*/124],
      [0x064F,/*31,*/126],
      [0x0650,/*32,*/128],
      [0x0651,/*33,*/129],
      [0x0652,/*34,*/130],
      [0x0653,/*230,*/416],
      [0x0654,/*230,*/417],
      [0x0655,/*220,*/219],
      [0x0656,/*220,*/220],
      [0x0657,/*230,*/418],
      [0x0658,/*230,*/419],
      [0x0659,/*230,*/420],
      [0x065A,/*230,*/421],
      [0x065B,/*230,*/422],
      [0x065C,/*220,*/221],
      [0x065D,/*230,*/423],
      [0x065E,/*230,*/424],
      [0x065F,/*220,*/222],
      [0x0670,/*35,*/131],
      [0x06D6,/*230,*/425],
      [0x06D7,/*230,*/426],
      [0x06D8,/*230,*/427],
      [0x06D9,/*230,*/428],
      [0x06DA,/*230,*/429],
      [0x06DB,/*230,*/430],
      [0x06DC,/*230,*/431],
      [0x06DF,/*230,*/432],
      [0x06E0,/*230,*/433],
      [0x06E1,/*230,*/434],
      [0x06E2,/*230,*/435],
      [0x06E3,/*220,*/223],
      [0x06E4,/*230,*/436],
      [0x06E7,/*230,*/437],
      [0x06E8,/*230,*/438],
      [0x06EA,/*220,*/224],
      [0x06EB,/*230,*/439],
      [0x06EC,/*230,*/440],
      [0x06ED,/*220,*/225],
      [0x0711,/*36,*/132],
      [0x0730,/*230,*/441],
      [0x0731,/*220,*/226],
      [0x0732,/*230,*/442],
      [0x0733,/*230,*/443],
      [0x0734,/*220,*/227],
      [0x0735,/*230,*/444],
      [0x0736,/*230,*/445],
      [0x0737,/*220,*/228],
      [0x0738,/*220,*/229],
      [0x0739,/*220,*/230],
      [0x073A,/*230,*/446],
      [0x073B,/*220,*/231],
      [0x073C,/*220,*/232],
      [0x073D,/*230,*/447],
      [0x073E,/*220,*/233],
      [0x073F,/*230,*/448],
      [0x0740,/*230,*/449],
      [0x0741,/*230,*/450],
      [0x0742,/*220,*/234],
      [0x0743,/*230,*/451],
      [0x0744,/*220,*/235],
      [0x0745,/*230,*/452],
      [0x0746,/*220,*/236],
      [0x0747,/*230,*/453],
      [0x0748,/*220,*/237],
      [0x0749,/*230,*/454],
      [0x074A,/*230,*/455],
      [0x07EB,/*230,*/456],
      [0x07EC,/*230,*/457],
      [0x07ED,/*230,*/458],
      [0x07EE,/*230,*/459],
      [0x07EF,/*230,*/460],
      [0x07F0,/*230,*/461],
      [0x07F1,/*230,*/462],
      [0x07F2,/*220,*/238],
      [0x07F3,/*230,*/463],
      [0x0816,/*230,*/464],
      [0x0817,/*230,*/465],
      [0x0818,/*230,*/466],
      [0x0819,/*230,*/467],
      [0x081B,/*230,*/468],
      [0x081C,/*230,*/469],
      [0x081D,/*230,*/470],
      [0x081E,/*230,*/471],
      [0x081F,/*230,*/472],
      [0x0820,/*230,*/473],
      [0x0821,/*230,*/474],
      [0x0822,/*230,*/475],
      [0x0823,/*230,*/476],
      [0x0825,/*230,*/477],
      [0x0826,/*230,*/478],
      [0x0827,/*230,*/479],
      [0x0829,/*230,*/480],
      [0x082A,/*230,*/481],
      [0x082B,/*230,*/482],
      [0x082C,/*230,*/483],
      [0x082D,/*230,*/484],
      [0x0859,/*220,*/239],
      [0x085A,/*220,*/240],
      [0x085B,/*220,*/241],
      [0x08E4,/*230,*/485],
      [0x08E5,/*230,*/486],
      [0x08E6,/*220,*/242],
      [0x08E7,/*230,*/487],
      [0x08E8,/*230,*/488],
      [0x08E9,/*220,*/243],
      [0x08EA,/*230,*/489],
      [0x08EB,/*230,*/490],
      [0x08EC,/*230,*/491],
      [0x08ED,/*220,*/244],
      [0x08EE,/*220,*/245],
      [0x08EF,/*220,*/246],
      [0x08F0,/*27,*/118],
      [0x08F1,/*28,*/120],
      [0x08F2,/*29,*/122],
      [0x08F3,/*230,*/492],
      [0x08F4,/*230,*/493],
      [0x08F5,/*230,*/494],
      [0x08F6,/*220,*/247],
      [0x08F7,/*230,*/495],
      [0x08F8,/*230,*/496],
      [0x08F9,/*220,*/248],
      [0x08FA,/*220,*/249],
      [0x08FB,/*230,*/497],
      [0x08FC,/*230,*/498],
      [0x08FD,/*230,*/499],
      [0x08FE,/*230,*/500],
      [0x08FF,/*230,*/501],
      [0x093C,/*7,*/33],
      [0x094D,/*9,*/54],
      [0x0951,/*230,*/502],
      [0x0952,/*220,*/250],
      [0x0953,/*230,*/503],
      [0x0954,/*230,*/504],
      [0x09BC,/*7,*/34],
      [0x09CD,/*9,*/55],
      [0x0A3C,/*7,*/35],
      [0x0A4D,/*9,*/56],
      [0x0ABC,/*7,*/36],
      [0x0ACD,/*9,*/57],
      [0x0B3C,/*7,*/37],
      [0x0B4D,/*9,*/58],
      [0x0BCD,/*9,*/59],
      [0x0C4D,/*9,*/60],
      [0x0C55,/*84,*/133],
      [0x0C56,/*91,*/134],
      [0x0CBC,/*7,*/38],
      [0x0CCD,/*9,*/61],
      [0x0D4D,/*9,*/62],
      [0x0DCA,/*9,*/63],
      [0x0E38,/*103,*/135],
      [0x0E39,/*103,*/136],
      [0x0E3A,/*9,*/64],
      [0x0E48,/*107,*/137],
      [0x0E49,/*107,*/138],
      [0x0E4A,/*107,*/139],
      [0x0E4B,/*107,*/140],
      [0x0EB8,/*118,*/141],
      [0x0EB9,/*118,*/142],
      [0x0EC8,/*122,*/143],
      [0x0EC9,/*122,*/144],
      [0x0ECA,/*122,*/145],
      [0x0ECB,/*122,*/146],
      [0x0F18,/*220,*/251],
      [0x0F19,/*220,*/252],
      [0x0F35,/*220,*/253],
      [0x0F37,/*220,*/254],
      [0x0F39,/*216,*/160],
      [0x0F71,/*129,*/147],
      [0x0F72,/*130,*/148],
      [0x0F74,/*132,*/154],
      [0x0F7A,/*130,*/149],
      [0x0F7B,/*130,*/150],
      [0x0F7C,/*130,*/151],
      [0x0F7D,/*130,*/152],
      [0x0F80,/*130,*/153],
      [0x0F82,/*230,*/505],
      [0x0F83,/*230,*/506],
      [0x0F84,/*9,*/65],
      [0x0F86,/*230,*/507],
      [0x0F87,/*230,*/508],
      [0x0FC6,/*220,*/255],
      [0x1037,/*7,*/39],
      [0x1039,/*9,*/66],
      [0x103A,/*9,*/67],
      [0x108D,/*220,*/256],
      [0x135D,/*230,*/509],
      [0x135E,/*230,*/510],
      [0x135F,/*230,*/511],
      [0x1714,/*9,*/68],
      [0x1734,/*9,*/69],
      [0x17D2,/*9,*/70],
      [0x17DD,/*230,*/512],
      [0x18A9,/*228,*/329],
      [0x1939,/*222,*/323],
      [0x193A,/*230,*/513],
      [0x193B,/*220,*/257],
      [0x1A17,/*230,*/514],
      [0x1A18,/*220,*/258],
      [0x1A60,/*9,*/71],
      [0x1A75,/*230,*/515],
      [0x1A76,/*230,*/516],
      [0x1A77,/*230,*/517],
      [0x1A78,/*230,*/518],
      [0x1A79,/*230,*/519],
      [0x1A7A,/*230,*/520],
      [0x1A7B,/*230,*/521],
      [0x1A7C,/*230,*/522],
      [0x1A7F,/*220,*/259],
      [0x1AB0,/*230,*/523],
      [0x1AB1,/*230,*/524],
      [0x1AB2,/*230,*/525],
      [0x1AB3,/*230,*/526],
      [0x1AB4,/*230,*/527],
      [0x1AB5,/*220,*/260],
      [0x1AB6,/*220,*/261],
      [0x1AB7,/*220,*/262],
      [0x1AB8,/*220,*/263],
      [0x1AB9,/*220,*/264],
      [0x1ABA,/*220,*/265],
      [0x1ABB,/*230,*/528],
      [0x1ABC,/*230,*/529],
      [0x1ABD,/*220,*/266],
      [0x1B34,/*7,*/40],
      [0x1B44,/*9,*/72],
      [0x1B6B,/*230,*/530],
      [0x1B6C,/*220,*/267],
      [0x1B6D,/*230,*/531],
      [0x1B6E,/*230,*/532],
      [0x1B6F,/*230,*/533],
      [0x1B70,/*230,*/534],
      [0x1B71,/*230,*/535],
      [0x1B72,/*230,*/536],
      [0x1B73,/*230,*/537],
      [0x1BAA,/*9,*/73],
      [0x1BAB,/*9,*/74],
      [0x1BE6,/*7,*/41],
      [0x1BF2,/*9,*/75],
      [0x1BF3,/*9,*/76],
      [0x1C37,/*7,*/42],
      [0x1CD0,/*230,*/538],
      [0x1CD1,/*230,*/539],
      [0x1CD2,/*230,*/540],
      [0x1CD4,/*1,*/6],
      [0x1CD5,/*220,*/268],
      [0x1CD6,/*220,*/269],
      [0x1CD7,/*220,*/270],
      [0x1CD8,/*220,*/271],
      [0x1CD9,/*220,*/272],
      [0x1CDA,/*230,*/541],
      [0x1CDB,/*230,*/542],
      [0x1CDC,/*220,*/273],
      [0x1CDD,/*220,*/274],
      [0x1CDE,/*220,*/275],
      [0x1CDF,/*220,*/276],
      [0x1CE0,/*230,*/543],
      [0x1CE2,/*1,*/7],
      [0x1CE3,/*1,*/8],
      [0x1CE4,/*1,*/9],
      [0x1CE5,/*1,*/10],
      [0x1CE6,/*1,*/11],
      [0x1CE7,/*1,*/12],
      [0x1CE8,/*1,*/13],
      [0x1CED,/*220,*/277],
      [0x1CF4,/*230,*/544],
      [0x1CF8,/*230,*/545],
      [0x1CF9,/*230,*/546],
      [0x1DC0,/*230,*/547],
      [0x1DC1,/*230,*/548],
      [0x1DC2,/*220,*/278],
      [0x1DC3,/*230,*/549],
      [0x1DC4,/*230,*/550],
      [0x1DC5,/*230,*/551],
      [0x1DC6,/*230,*/552],
      [0x1DC7,/*230,*/553],
      [0x1DC8,/*230,*/554],
      [0x1DC9,/*230,*/555],
      [0x1DCA,/*220,*/279],
      [0x1DCB,/*230,*/556],
      [0x1DCC,/*230,*/557],
      [0x1DCD,/*234,*/744],
      [0x1DCE,/*214,*/158],
      [0x1DCF,/*220,*/280],
      [0x1DD0,/*202,*/157],
      [0x1DD1,/*230,*/558],
      [0x1DD2,/*230,*/559],
      [0x1DD3,/*230,*/560],
      [0x1DD4,/*230,*/561],
      [0x1DD5,/*230,*/562],
      [0x1DD6,/*230,*/563],
      [0x1DD7,/*230,*/564],
      [0x1DD8,/*230,*/565],
      [0x1DD9,/*230,*/566],
      [0x1DDA,/*230,*/567],
      [0x1DDB,/*230,*/568],
      [0x1DDC,/*230,*/569],
      [0x1DDD,/*230,*/570],
      [0x1DDE,/*230,*/571],
      [0x1DDF,/*230,*/572],
      [0x1DE0,/*230,*/573],
      [0x1DE1,/*230,*/574],
      [0x1DE2,/*230,*/575],
      [0x1DE3,/*230,*/576],
      [0x1DE4,/*230,*/577],
      [0x1DE5,/*230,*/578],
      [0x1DE6,/*230,*/579],
      [0x1DE7,/*230,*/580],
      [0x1DE8,/*230,*/581],
      [0x1DE9,/*230,*/582],
      [0x1DEA,/*230,*/583],
      [0x1DEB,/*230,*/584],
      [0x1DEC,/*230,*/585],
      [0x1DED,/*230,*/586],
      [0x1DEE,/*230,*/587],
      [0x1DEF,/*230,*/588],
      [0x1DF0,/*230,*/589],
      [0x1DF1,/*230,*/590],
      [0x1DF2,/*230,*/591],
      [0x1DF3,/*230,*/592],
      [0x1DF4,/*230,*/593],
      [0x1DF5,/*230,*/594],
      [0x1DFC,/*233,*/739],
      [0x1DFD,/*220,*/281],
      [0x1DFE,/*230,*/595],
      [0x1DFF,/*220,*/282],
      [0x20D0,/*230,*/596],
      [0x20D1,/*230,*/597],
      [0x20D2,/*1,*/14],
      [0x20D3,/*1,*/15],
      [0x20D4,/*230,*/598],
      [0x20D5,/*230,*/599],
      [0x20D6,/*230,*/600],
      [0x20D7,/*230,*/601],
      [0x20D8,/*1,*/16],
      [0x20D9,/*1,*/17],
      [0x20DA,/*1,*/18],
      [0x20DB,/*230,*/602],
      [0x20DC,/*230,*/603],
      [0x20E1,/*230,*/604],
      [0x20E5,/*1,*/19],
      [0x20E6,/*1,*/20],
      [0x20E7,/*230,*/605],
      [0x20E8,/*220,*/283],
      [0x20E9,/*230,*/606],
      [0x20EA,/*1,*/21],
      [0x20EB,/*1,*/22],
      [0x20EC,/*220,*/284],
      [0x20ED,/*220,*/285],
      [0x20EE,/*220,*/286],
      [0x20EF,/*220,*/287],
      [0x20F0,/*230,*/607],
      [0x2CEF,/*230,*/608],
      [0x2CF0,/*230,*/609],
      [0x2CF1,/*230,*/610],
      [0x2D7F,/*9,*/77],
      [0x2DE0,/*230,*/611],
      [0x2DE1,/*230,*/612],
      [0x2DE2,/*230,*/613],
      [0x2DE3,/*230,*/614],
      [0x2DE4,/*230,*/615],
      [0x2DE5,/*230,*/616],
      [0x2DE6,/*230,*/617],
      [0x2DE7,/*230,*/618],
      [0x2DE8,/*230,*/619],
      [0x2DE9,/*230,*/620],
      [0x2DEA,/*230,*/621],
      [0x2DEB,/*230,*/622],
      [0x2DEC,/*230,*/623],
      [0x2DED,/*230,*/624],
      [0x2DEE,/*230,*/625],
      [0x2DEF,/*230,*/626],
      [0x2DF0,/*230,*/627],
      [0x2DF1,/*230,*/628],
      [0x2DF2,/*230,*/629],
      [0x2DF3,/*230,*/630],
      [0x2DF4,/*230,*/631],
      [0x2DF5,/*230,*/632],
      [0x2DF6,/*230,*/633],
      [0x2DF7,/*230,*/634],
      [0x2DF8,/*230,*/635],
      [0x2DF9,/*230,*/636],
      [0x2DFA,/*230,*/637],
      [0x2DFB,/*230,*/638],
      [0x2DFC,/*230,*/639],
      [0x2DFD,/*230,*/640],
      [0x2DFE,/*230,*/641],
      [0x2DFF,/*230,*/642],
      [0x302A,/*218,*/168],
      [0x302B,/*228,*/330],
      [0x302C,/*232,*/735],
      [0x302D,/*222,*/324],
      [0x302E,/*224,*/325],
      [0x302F,/*224,*/326],
      [0x3099,/*8,*/52],
      [0x309A,/*8,*/53],
      [0xA66F,/*230,*/643],
      [0xA674,/*230,*/644],
      [0xA675,/*230,*/645],
      [0xA676,/*230,*/646],
      [0xA677,/*230,*/647],
      [0xA678,/*230,*/648],
      [0xA679,/*230,*/649],
      [0xA67A,/*230,*/650],
      [0xA67B,/*230,*/651],
      [0xA67C,/*230,*/652],
      [0xA67D,/*230,*/653],
      [0xA69F,/*230,*/654],
      [0xA6F0,/*230,*/655],
      [0xA6F1,/*230,*/656],
      [0xA806,/*9,*/78],
      [0xA8C4,/*9,*/79],
      [0xA8E0,/*230,*/657],
      [0xA8E1,/*230,*/658],
      [0xA8E2,/*230,*/659],
      [0xA8E3,/*230,*/660],
      [0xA8E4,/*230,*/661],
      [0xA8E5,/*230,*/662],
      [0xA8E6,/*230,*/663],
      [0xA8E7,/*230,*/664],
      [0xA8E8,/*230,*/665],
      [0xA8E9,/*230,*/666],
      [0xA8EA,/*230,*/667],
      [0xA8EB,/*230,*/668],
      [0xA8EC,/*230,*/669],
      [0xA8ED,/*230,*/670],
      [0xA8EE,/*230,*/671],
      [0xA8EF,/*230,*/672],
      [0xA8F0,/*230,*/673],
      [0xA8F1,/*230,*/674],
      [0xA92B,/*220,*/288],
      [0xA92C,/*220,*/289],
      [0xA92D,/*220,*/290],
      [0xA953,/*9,*/80],
      [0xA9B3,/*7,*/43],
      [0xA9C0,/*9,*/81],
      [0xAAB0,/*230,*/675],
      [0xAAB2,/*230,*/676],
      [0xAAB3,/*230,*/677],
      [0xAAB4,/*220,*/291],
      [0xAAB7,/*230,*/678],
      [0xAAB8,/*230,*/679],
      [0xAABE,/*230,*/680],
      [0xAABF,/*230,*/681],
      [0xAAC1,/*230,*/682],
      [0xAAF6,/*9,*/82],
      [0xABED,/*9,*/83],
      [0xFB1E,/*26,*/116],
      [0xFE20,/*230,*/683],
      [0xFE21,/*230,*/684],
      [0xFE22,/*230,*/685],
      [0xFE23,/*230,*/686],
      [0xFE24,/*230,*/687],
      [0xFE25,/*230,*/688],
      [0xFE26,/*230,*/689],
      [0xFE27,/*220,*/292],
      [0xFE28,/*220,*/293],
      [0xFE29,/*220,*/294],
      [0xFE2A,/*220,*/295],
      [0xFE2B,/*220,*/296],
      [0xFE2C,/*220,*/297],
      [0xFE2D,/*220,*/298],
      [0x101FD,/*220,*/299],
      [0x102E0,/*220,*/300],
      [0x10376,/*230,*/690],
      [0x10377,/*230,*/691],
      [0x10378,/*230,*/692],
      [0x10379,/*230,*/693],
      [0x1037A,/*230,*/694],
      [0x10A0D,/*220,*/301],
      [0x10A0F,/*230,*/695],
      [0x10A38,/*230,*/696],
      [0x10A39,/*1,*/23],
      [0x10A3A,/*220,*/302],
      [0x10A3F,/*9,*/84],
      [0x10AE5,/*230,*/697],
      [0x10AE6,/*220,*/303],
      [0x11046,/*9,*/85],
      [0x1107F,/*9,*/86],
      [0x110B9,/*9,*/87],
      [0x110BA,/*7,*/44],
      [0x11100,/*230,*/698],
      [0x11101,/*230,*/699],
      [0x11102,/*230,*/700],
      [0x11133,/*9,*/88],
      [0x11134,/*9,*/89],
      [0x11173,/*7,*/45],
      [0x111C0,/*9,*/90],
      [0x11235,/*9,*/91],
      [0x11236,/*7,*/46],
      [0x112E9,/*7,*/47],
      [0x112EA,/*9,*/92],
      [0x1133C,/*7,*/48],
      [0x1134D,/*9,*/93],
      [0x11366,/*230,*/701],
      [0x11367,/*230,*/702],
      [0x11368,/*230,*/703],
      [0x11369,/*230,*/704],
      [0x1136A,/*230,*/705],
      [0x1136B,/*230,*/706],
      [0x1136C,/*230,*/707],
      [0x11370,/*230,*/708],
      [0x11371,/*230,*/709],
      [0x11372,/*230,*/710],
      [0x11373,/*230,*/711],
      [0x11374,/*230,*/712],
      [0x114C2,/*9,*/94],
      [0x114C3,/*7,*/49],
      [0x115BF,/*9,*/95],
      [0x115C0,/*7,*/50],
      [0x1163F,/*9,*/96],
      [0x116B6,/*9,*/97],
      [0x116B7,/*7,*/51],
      [0x16AF0,/*1,*/24],
      [0x16AF1,/*1,*/25],
      [0x16AF2,/*1,*/26],
      [0x16AF3,/*1,*/27],
      [0x16AF4,/*1,*/28],
      [0x16B30,/*230,*/713],
      [0x16B31,/*230,*/714],
      [0x16B32,/*230,*/715],
      [0x16B33,/*230,*/716],
      [0x16B34,/*230,*/717],
      [0x16B35,/*230,*/718],
      [0x16B36,/*230,*/719],
      [0x1BC9E,/*1,*/29],
      [0x1D165,/*216,*/161],
      [0x1D166,/*216,*/162],
      [0x1D167,/*1,*/30],
      [0x1D168,/*1,*/31],
      [0x1D169,/*1,*/32],
      [0x1D16D,/*226,*/327],
      [0x1D16E,/*216,*/163],
      [0x1D16F,/*216,*/164],
      [0x1D170,/*216,*/165],
      [0x1D171,/*216,*/166],
      [0x1D172,/*216,*/167],
      [0x1D17B,/*220,*/304],
      [0x1D17C,/*220,*/305],
      [0x1D17D,/*220,*/306],
      [0x1D17E,/*220,*/307],
      [0x1D17F,/*220,*/308],
      [0x1D180,/*220,*/309],
      [0x1D181,/*220,*/310],
      [0x1D182,/*220,*/311],
      [0x1D185,/*230,*/720],
      [0x1D186,/*230,*/721],
      [0x1D187,/*230,*/722],
      [0x1D188,/*230,*/723],
      [0x1D189,/*230,*/724],
      [0x1D18A,/*220,*/312],
      [0x1D18B,/*220,*/313],
      [0x1D1AA,/*230,*/725],
      [0x1D1AB,/*230,*/726],
      [0x1D1AC,/*230,*/727],
      [0x1D1AD,/*230,*/728],
      [0x1D242,/*230,*/729],
      [0x1D243,/*230,*/730],
      [0x1D244,/*230,*/731],
      [0x1E8D0,/*220,*/314],
      [0x1E8D1,/*220,*/315],
      [0x1E8D2,/*220,*/316],
      [0x1E8D3,/*220,*/317],
      [0x1E8D4,/*220,*/318],
      [0x1E8D5,/*220,*/319],
      [0x1E8D6,/*220,*/320]
    ]
  },

  atariBytes: {
    type: 'struct codepointToByte',
    define: 'SIZE_ATARIBYTES',
    filename: 'atarimap',
    values: [
      [0x0006, 0x06], [0x0007, 0x07], [0x0009, 0x09],
      [0x000A, 0x0A], [0x000C, 0x0C], [0x000D, 0x0D],
      [0x000E, 0x0E], [0x000F, 0x0F], [0x0010, 0x10],
      [0x0011, 0x11], [0x0012, 0x12], [0x0013, 0x13],
      [0x0014, 0x14], [0x0015, 0x15], [0x0016, 0x16],
      [0x0017, 0x17], [0x0018, 0x18], [0x0019, 0x19],
      [0x001B, 0x1B], [0x001C, 0x1C], [0x001D, 0x1D],
      [0x001E, 0x01], [0x001F, 0x1F], [0x00A1, 0xAD],
      [0x00A2, 0x9B], [0x00A3, 0x9C], [0x00A5, 0x9D],
      [0x00A7, 0xDD], [0x00A8, 0xB9], [0x00A9, 0xBD],
      [0x00AA, 0xA6], [0x00AB, 0xAE], [0x00AC, 0xAA],
      [0x00AE, 0xBE], [0x00AF, 0xFF], [0x00B0, 0xF8],
      [0x00B1, 0xF1], [0x00B2, 0xFD], [0x00B3, 0xFE],
      [0x00B4, 0xBA], [0x00B5, 0xE6], [0x00B6, 0xBC],
      [0x00B7, 0xFA], [0x00BA, 0xA7], [0x00BB, 0xAF],
      [0x00BC, 0xAC], [0x00BD, 0xAB], [0x00BF, 0xA8],
      [0x00C0, 0xB6], [0x00C3, 0xB7], [0x00C4, 0x8E],
      [0x00C5, 0x8F], [0x00C6, 0x92], [0x00C7, 0x80],
      [0x00C9, 0x90], [0x00D1, 0xA5], [0x00D5, 0xB8],
      [0x00D6, 0x99], [0x00D8, 0xB2], [0x00DC, 0x9A],
      [0x00DF, 0x09], [0x00E0, 0x85], [0x00E1, 0xA0],
      [0x00E2, 0x83], [0x00E3, 0xB0], [0x00E4, 0x84],
      [0x00E5, 0x86], [0x00E6, 0x91], [0x00E7, 0x87],
      [0x00E8, 0x8A], [0x00E9, 0x82], [0x00EA, 0x88],
      [0x00EB, 0x89], [0x00EC, 0x8D], [0x00ED, 0xA1],
      [0x00EE, 0x8C], [0x00EF, 0x8B], [0x00F1, 0xA4],
      [0x00F2, 0x95], [0x00F3, 0xA2], [0x00F4, 0x93],
      [0x00F5, 0xB1], [0x00F6, 0x94], [0x00F7, 0xF6],
      [0x00F8, 0xB3], [0x00F9, 0x97], [0x00FA, 0xA3],
      [0x00FB, 0x96], [0x00FC, 0x81], [0x00FF, 0x98],
      [0x0132, 0xC1], [0x0133, 0xC0], [0x0152, 0xB5],
      [0x0153, 0xB4], [0x0192, 0x9F], [0x0259, 0x1A],
      [0x0393, 0xE2], [0x0398, 0xE9], [0x03A3, 0xE4],
      [0x03A6, 0xE8], [0x03A9, 0xEA], [0x03B1, 0xE0],
      [0x03B2, 0xE1], [0x03B4, 0xEB], [0x03C0, 0xE3],
      [0x03C3, 0xE5], [0x03C4, 0xE7], [0x03D5, 0xED],
      [0x05D0, 0xC2], [0x05D1, 0xC3], [0x05D2, 0xC4],
      [0x05D3, 0xC5], [0x05D4, 0xC6], [0x05D5, 0xC7],
      [0x05D6, 0xC8], [0x05D7, 0xC9], [0x05D8, 0xCA],
      [0x05D9, 0xCB], [0x05DA, 0xD9], [0x05DB, 0xCC],
      [0x05DC, 0xCD], [0x05DD, 0xDA], [0x05DE, 0xCE],
      [0x05DF, 0xD8], [0x05E0, 0xCF], [0x05E1, 0xD0],
      [0x05E2, 0xD1], [0x05E3, 0xDB], [0x05E4, 0xD2],
      [0x05E5, 0xDC], [0x05E6, 0xD3], [0x05E7, 0xD4],
      [0x05E8, 0xD5], [0x05E9, 0xD6], [0x05EA, 0xD7],
      [0x2020, 0xBB], [0x207F, 0xFC], [0x2122, 0xBF],
      [0x21E6, 0x04], [0x21E7, 0x01], [0x21E8, 0x03],
      [0x21E9, 0x02], [0x2208, 0xEE], [0x2219, 0xF9],
      [0x221A, 0xFB], [0x221E, 0xDF], [0x2227, 0xDE],
      [0x2229, 0xEF], [0x222E, 0xEC], [0x2248, 0xF7],
      [0x2261, 0xF0], [0x2264, 0xF3], [0x2265, 0xF2],
      [0x2310, 0xA9], [0x2320, 0xF4], [0x2321, 0xF5],
      [0x266A, 0x0B], [0x2713, 0x08], [0x274E, 0x05]
    ]
  },

  commonBytes: {
    type: 'struct codepointToBytes',
    define: 'SIZE_COMMONBYTES',
    filename: 'cmnmap',
    values: [
      [0x00A0, 0xFF, 0xFF, 0xCA],
      [0x00A1, 0xAD, 0xAD, 0xC1],
      [0x00A2, 0x9B, 0xBD, 0xA2],
      [0x00A3, 0x9C, 0x9C, 0xA3],
      [0x00A4, 0x00, 0xCF, 0x00],
      [0x00A5, 0x9D, 0xBE, 0xB4],
      [0x00A6, 0x00, 0xDD, 0x00],
      [0x00A7, 0x15, 0xF5, 0xA4],
      [0x00A8, 0x00, 0xF9, 0xAC],
      [0x00A9, 0x00, 0xB8, 0xA9],
      [0x00AA, 0xA6, 0xA6, 0xBB],
      [0x00AB, 0xAE, 0xAE, 0xC7],
      [0x00AC, 0xAA, 0xAA, 0xC2],
      [0x00AD, 0x00, 0xF0, 0x00],
      [0x00AE, 0x00, 0xA9, 0xA8],
      [0x00AF, 0x00, 0xEE, 0xF8],
      [0x00B0, 0xF8, 0xF8, 0xA1],
      [0x00B1, 0xF1, 0xF1, 0xB1],
      [0x00B2, 0xFD, 0xFD, 0x00],
      [0x00B3, 0x00, 0xFC, 0x00],
      [0x00B4, 0x00, 0xEF, 0xAB],
      [0x00B5, 0xE6, 0xE6, 0xB5],
      [0x00B6, 0x14, 0xF4, 0xA6],
      [0x00B7, 0xFA, 0xFA, 0xE1],
      [0x00B8, 0x00, 0xF7, 0xFC],
      [0x00B9, 0x00, 0xFB, 0x00],
      [0x00BA, 0xA7, 0xA7, 0xBC],
      [0x00BB, 0xAF, 0xAF, 0xC8],
      [0x00BC, 0xAC, 0xAC, 0x00],
      [0x00BD, 0xAB, 0xAB, 0x00],
      [0x00BE, 0x00, 0xF3, 0x00],
      [0x00BF, 0xA8, 0xA8, 0xC0],
      [0x00C0, 0x00, 0xB7, 0xCB],
      [0x00C1, 0x00, 0xB5, 0xE7],
      [0x00C2, 0x00, 0xB6, 0xE5],
      [0x00C3, 0x00, 0xC7, 0xCC],
      [0x00C4, 0x8E, 0x8E, 0x80],
      [0x00C5, 0x8F, 0x8F, 0x81],
      [0x00C6, 0x92, 0x92, 0xAE],
      [0x00C7, 0x80, 0x80, 0x82],
      [0x00C8, 0x00, 0xD4, 0xE9],
      [0x00C9, 0x90, 0x90, 0x83],
      [0x00CA, 0x00, 0xD2, 0xE6],
      [0x00CB, 0x00, 0xD3, 0xE8],
      [0x00CC, 0x00, 0xDE, 0xED],
      [0x00CD, 0x00, 0xD6, 0xEA],
      [0x00CE, 0x00, 0xD7, 0xEB],
      [0x00CF, 0x00, 0xD8, 0xEC],
      [0x00D0, 0x00, 0xD1, 0x00],
      [0x00D1, 0xA5, 0xA5, 0x84],
      [0x00D2, 0x00, 0xE3, 0xF1],
      [0x00D3, 0x00, 0xE0, 0xEE],
      [0x00D4, 0x00, 0xE2, 0xEF],
      [0x00D5, 0x00, 0xE5, 0xCD],
      [0x00D6, 0x99, 0x99, 0x85],
      [0x00D7, 0x00, 0x9E, 0x00],
      [0x00D8, 0x00, 0x9D, 0xAF],
      [0x00D9, 0x00, 0xEB, 0xF4],
      [0x00DA, 0x00, 0xE9, 0xF2],
      [0x00DB, 0x00, 0xEA, 0xF3],
      [0x00DC, 0x9A, 0x9A, 0x86],
      [0x00DD, 0x00, 0xED, 0x00],
      [0x00DE, 0x00, 0xE8, 0x00],
      [0x00DF, 0xE1, 0xE1, 0xA7],
      [0x00E0, 0x85, 0x85, 0x88],
      [0x00E1, 0xA0, 0xA0, 0x87],
      [0x00E2, 0x83, 0x83, 0x89],
      [0x00E3, 0x00, 0xC6, 0x8B],
      [0x00E4, 0x84, 0x84, 0x8A],
      [0x00E5, 0x86, 0x86, 0x8C],
      [0x00E6, 0x91, 0x91, 0xBE],
      [0x00E7, 0x87, 0x87, 0x8D],
      [0x00E8, 0x8A, 0x8A, 0x8F],
      [0x00E9, 0x82, 0x82, 0x8E],
      [0x00EA, 0x88, 0x88, 0x90],
      [0x00EB, 0x89, 0x89, 0x91],
      [0x00EC, 0x8D, 0x8D, 0x93],
      [0x00ED, 0xA1, 0xA1, 0x92],
      [0x00EE, 0x8C, 0x8C, 0x94],
      [0x00EF, 0x8B, 0x8B, 0x95],
      [0x00F0, 0x00, 0xD0, 0x00],
      [0x00F1, 0xA4, 0xA4, 0x96],
      [0x00F2, 0x95, 0x95, 0x98],
      [0x00F3, 0xA2, 0xA2, 0x97],
      [0x00F4, 0x93, 0x93, 0x99],
      [0x00F5, 0x00, 0xE4, 0x9B],
      [0x00F6, 0x94, 0x94, 0x9A],
      [0x00F7, 0xF6, 0xF6, 0xD6],
      [0x00F8, 0x00, 0x9B, 0xBF],
      [0x00F9, 0x97, 0x97, 0x9D],
      [0x00FA, 0xA3, 0xA3, 0x9C],
      [0x00FB, 0x96, 0x96, 0x9E],
      [0x00FC, 0x81, 0x81, 0x9F],
      [0x00FD, 0x00, 0xEC, 0x00],
      [0x00FE, 0x00, 0xE7, 0x00],
      [0x00FF, 0x98, 0x98, 0xD8],
      [0x0131, 0x00, 0xD5, 0x00], // Wikipedia claims dotless i is 0xF5 for macRoman, but in my experience old mac fonts don't have it at all
      [0x0152, 0x00, 0x00, 0xCE],
      [0x0153, 0x00, 0x00, 0xCF],
      [0x0178, 0x00, 0x00, 0xD9],
      [0x0192, 0x9F, 0x9F, 0xC4],
      [0x02C6, 0x00, 0x00, 0xF6],
      [0x02C7, 0x00, 0x00, 0xFF],
      [0x02D8, 0x00, 0x00, 0xF9],
      [0x02D9, 0x00, 0x00, 0xFA],
      [0x02DA, 0x00, 0x00, 0xFB],
      [0x02DB, 0x00, 0x00, 0xFE],
      [0x02DC, 0x00, 0x00, 0xF7],
      [0x02DD, 0x00, 0x00, 0xFD],
      [0x0393, 0xE2, 0x00, 0x00],
      [0x0398, 0xE9, 0x00, 0x00],
      [0x03A3, 0xE4, 0x00, 0x00],
      [0x03A6, 0xE8, 0x00, 0x00],
      [0x03A9, 0xEA, 0x00, 0xBD],
      [0x03B1, 0xE0, 0x00, 0x00],
      [0x03B4, 0xEB, 0x00, 0x00],
      [0x03B5, 0xEE, 0x00, 0x00],
      [0x03C0, 0xE3, 0x00, 0xB9],
      [0x03C3, 0xE5, 0x00, 0x00],
      [0x03C4, 0xE7, 0x00, 0x00],
      [0x03C6, 0xED, 0x00, 0x00],
      [0x2013, 0x00, 0x00, 0xD0],
      [0x2014, 0x00, 0x00, 0xD1],
      [0x2017, 0x00, 0xF2, 0x00],
      [0x2018, 0x00, 0x00, 0xD4],
      [0x2019, 0x00, 0x00, 0xD5],
      [0x201A, 0x00, 0x00, 0xE2],
      [0x201C, 0x00, 0x00, 0xD2],
      [0x201D, 0x00, 0x00, 0xD3],
      [0x201E, 0x00, 0x00, 0xE3],
      [0x2020, 0x00, 0x00, 0xA0],
      [0x2021, 0x00, 0x00, 0xE0],
      [0x2022, 0x07, 0x07, 0xA5],
      [0x2026, 0x00, 0x00, 0xC9],
      [0x2030, 0x00, 0x00, 0xE4],
      [0x2039, 0x00, 0x00, 0xDC],
      [0x203A, 0x00, 0x00, 0xDD],
      [0x203C, 0x13, 0x13, 0x00],
      [0x2044, 0x00, 0x00, 0xDA],
      [0x207F, 0xFC, 0x00, 0x00],
      [0x20A7, 0x9E, 0x00, 0x00],
      [0x20AC, 0x00, 0x00, 0xDB],
      [0x2122, 0x00, 0x00, 0xAA],
      [0x2190, 0x1B, 0x1B, 0x00],
      [0x2191, 0x18, 0x18, 0x00],
      [0x2193, 0x19, 0x19, 0x00],
      [0x2194, 0x1D, 0x1D, 0x00],
      [0x2195, 0x12, 0x12, 0x00],
      [0x21A8, 0x17, 0x17, 0x00],
      [0x2202, 0x00, 0x00, 0xB6],
      [0x2206, 0x00, 0x00, 0xC6],
      [0x220F, 0x00, 0x00, 0xB8],
      [0x2211, 0x00, 0x00, 0xB7],
      [0x2219, 0xF9, 0x00, 0x00],
      [0x221A, 0xFB, 0x00, 0xC3],
      [0x221E, 0xEC, 0x00, 0xB0],
      [0x221F, 0x1C, 0x1C, 0x00],
      [0x2229, 0xEF, 0x00, 0x00],
      [0x222B, 0x00, 0x00, 0xBA],
      [0x2248, 0xF7, 0x00, 0xC5],
      [0x2260, 0x00, 0x00, 0xAD],
      [0x2261, 0xF0, 0x00, 0x00],
      [0x2264, 0xF3, 0x00, 0xB2],
      [0x2265, 0xF2, 0x00, 0xB3],
      [0x2310, 0xA9, 0x00, 0x00],
      [0x2320, 0xF4, 0x00, 0x00],
      [0x2321, 0xF5, 0x00, 0x00],
      [0x2500, 0xC4, 0xC4, 0x00],
      [0x2502, 0xB3, 0xB3, 0x00],
      [0x250C, 0xDA, 0xDA, 0x00],
      [0x2510, 0xBF, 0xBF, 0x00],
      [0x2514, 0xC0, 0xC0, 0x00],
      [0x2518, 0xD9, 0xD9, 0x00],
      [0x251C, 0xC3, 0xC3, 0x00],
      [0x2524, 0xB4, 0xB4, 0x00],
      [0x252C, 0xC2, 0xC2, 0x00],
      [0x2534, 0xC1, 0xC1, 0x00],
      [0x253C, 0xC5, 0xC5, 0x00],
      [0x2550, 0xCD, 0xCD, 0x00],
      [0x2551, 0xBA, 0xBA, 0x00],
      [0x2552, 0xD5, 0x00, 0x00],
      [0x2553, 0xD6, 0x00, 0x00],
      [0x2554, 0xC9, 0xC9, 0x00],
      [0x2555, 0xB8, 0x00, 0x00],
      [0x2556, 0xB7, 0x00, 0x00],
      [0x2557, 0xBB, 0xBB, 0x00],
      [0x2558, 0xD4, 0x00, 0x00],
      [0x2559, 0xD3, 0x00, 0x00],
      [0x255A, 0xC8, 0xC8, 0x00],
      [0x255B, 0xBE, 0x00, 0x00],
      [0x255C, 0xBD, 0x00, 0x00],
      [0x255D, 0xBC, 0xBC, 0x00],
      [0x255E, 0xC6, 0x00, 0x00],
      [0x255F, 0xC7, 0x00, 0x00],
      [0x2560, 0xCC, 0xCC, 0x00],
      [0x2561, 0xB5, 0x00, 0x00],
      [0x2562, 0xB6, 0x00, 0x00],
      [0x2563, 0xB9, 0xB9, 0x00],
      [0x2564, 0xD1, 0x00, 0x00],
      [0x2565, 0xD2, 0x00, 0x00],
      [0x2566, 0xCB, 0xCB, 0x00],
      [0x2567, 0xCF, 0x00, 0x00],
      [0x2568, 0xD0, 0x00, 0x00],
      [0x2569, 0xCA, 0xCA, 0x00],
      [0x256A, 0xD8, 0x00, 0x00],
      [0x256B, 0xD7, 0x00, 0x00],
      [0x256C, 0xCE, 0xCE, 0x00],
      [0x2580, 0xDF, 0xDF, 0x00],
      [0x2584, 0xDC, 0xDC, 0x00],
      [0x2588, 0xDB, 0xDB, 0x00],
      [0x258C, 0xDD, 0x00, 0x00],
      [0x2590, 0xDE, 0x00, 0x00],
      [0x2591, 0xB0, 0xB0, 0x00],
      [0x2592, 0xB1, 0xB1, 0x00],
      [0x2593, 0xB2, 0xB2, 0x00],
      [0x25A0, 0xFE, 0xFE, 0x00],
      [0x25AC, 0x16, 0x16, 0x00],
      [0x25B2, 0x1E, 0x1E, 0x00],
      [0x25BA, 0x10, 0x10, 0x00],
      [0x25BC, 0x1F, 0x1F, 0x00],
      [0x25C4, 0x11, 0x11, 0x00],
      [0x25CA, 0x00, 0x00, 0xD7],
      [0x25D8, 0x08, 0x08, 0x00],
      [0x263A, 0x01, 0x01, 0x00],
      [0x263B, 0x02, 0x02, 0x00],
      [0x263C, 0x0F, 0x0F, 0x00],
      [0x2640, 0x0C, 0x0C, 0x00],
      [0x2642, 0x0B, 0x0B, 0x00],
      [0x2660, 0x06, 0x06, 0x00],
      [0x2663, 0x05, 0x05, 0x00],
      [0x2665, 0x03, 0x03, 0x00],
      [0x2666, 0x04, 0x04, 0x00],
      [0x266B, 0x0E, 0x0E, 0x00],
      [0xF8FF, 0x00, 0x00, 0xF0],
      [0xFB01, 0x00, 0x00, 0xDE],
      [0xFB02, 0x00, 0x00, 0xDF]
    ]
  },

  cp1252Bytes: {
    type: 'struct codepointToByte',
    define: 'SIZE_CP1252BYTES',
    filename: 'ansimap',
    values: [
      [0x0081, 0x81], [0x008D, 0x8D],
      [0x008F, 0x8F], [0x0090, 0x90],
      [0x009D, 0x9D], [0x0152, 0x8C],
      [0x0153, 0x9C], [0x0160, 0x8A],
      [0x0161, 0x9A], [0x0178, 0x9F],
      [0x017D, 0x8E], [0x017E, 0x9E],
      [0x0192, 0x83], [0x02C6, 0x88],
      [0x02DC, 0x98], [0x2013, 0x96],
      [0x2014, 0x97], [0x2018, 0x91],
      [0x2019, 0x92], [0x201A, 0x82],
      [0x201C, 0x93], [0x201D, 0x94],
      [0x201E, 0x84], [0x2020, 0x86],
      [0x2021, 0x87], [0x2022, 0x95],
      [0x2026, 0x85], [0x2030, 0x89],
      [0x2039, 0x8B], [0x203A, 0x9B],
      [0x20AC, 0x80], [0x2122, 0x99]
    ]
  },

  zxBytes: {
    type: 'struct codepointToByte',
    define: 'SIZE_ZXBYTES',
    filename: 'zxmap',
    values: [
      [0x00A3, 0x60],
      [0x00A9, 0x7F],
      [0x2191, 0x5E],

      [0x2580, 0x83], // Upper half block
      [0x2584, 0x8C], // Lower half block
      [0x2588, 0x8F], // Full block
      [0x258C, 0x8A], // Left half block

      [0x2590, 0x85], // Right half block
      [0x2596, 0x88], // Quadrant lower left
      [0x2597, 0x84], // Quadrant lower right
      [0x2598, 0x82], // Quadrant upper left

      [0x2599, 0x8E], // Quadrant upper left and lower left and lower right
      [0x259A, 0x86], // Quadrant upper left and lower right
      [0x259B, 0x8B], // Quadrant upper left and upper right and lower left
      [0x259C, 0x87], // Quadrant upper left and upper right and lower right

      [0x259D, 0x81], // Quadrant upper right
      [0x259E, 0x89], // Quadrant upper right and lower left
      [0x259F, 0x8D], // Quadrant upper right and lower left and lower right
      [0x25A1, 0x80]  // white square
    ]
  },

  petsciiBytes: {
    type: 'struct codepointToByte',
    define: 'SIZE_PETSCIIBYTES',
    filename: 'petmap',
    values: [
      [0x0041, 0x61], [0x0042, 0x62],
      [0x0043, 0x63], [0x0044, 0x64],
      [0x0045, 0x65], [0x0046, 0x66],
      [0x0047, 0x67], [0x0048, 0x68],
      [0x0049, 0x69], [0x004A, 0x6A],
      [0x004B, 0x6B], [0x004C, 0x6C],
      [0x004D, 0x6D], [0x004E, 0x6E],
      [0x004F, 0x6F], [0x0050, 0x70],
      [0x0051, 0x71], [0x0052, 0x72],
      [0x0053, 0x73], [0x0054, 0x74],
      [0x0055, 0x75], [0x0056, 0x76],
      [0x0057, 0x77], [0x0058, 0x78],
      [0x0059, 0x79], [0x005A, 0x7A],
      [0x005B, 0x5B], [0x005D, 0x5D],
      [0x005F, 0xA4], [0x0061, 0x41],
      [0x0062, 0x42], [0x0063, 0x43],
      [0x0064, 0x44], [0x0065, 0x45],
      [0x0066, 0x46], [0x0067, 0x47],
      [0x0068, 0x48], [0x0069, 0x49],
      [0x006A, 0x4A], [0x006B, 0x4B],
      [0x006C, 0x4C], [0x006D, 0x4D],
      [0x006E, 0x4E], [0x006F, 0x4F],
      [0x0070, 0x50], [0x0071, 0x51],
      [0x0072, 0x52], [0x0073, 0x53],
      [0x0074, 0x54], [0x0075, 0x55],
      [0x0076, 0x56], [0x0077, 0x57],
      [0x0078, 0x58], [0x0079, 0x59],
      [0x007A, 0x5A], [0x007C, 0xAA],
      [0x00A0, 0xA0], [0x00A3, 0x5C],
      [0x2190, 0x5F], [0x2191, 0x5E],
      [0x2500, 0x60], [0x250C, 0xB0],
      [0x2510, 0xAE], [0x2514, 0xAD],
      [0x2518, 0xBD], [0x251C, 0xAB],
      [0x2524, 0xB3], [0x252C, 0xB2],
      [0x2534, 0xB1], [0x253C, 0x7B],
      [0x2581, 0xA4], [0x2582, 0xAF],
      [0x2583, 0xB9], [0x2584, 0xA2],
      [0x258C, 0xA1], [0x258D, 0xB5],
      [0x258E, 0xB4], [0x258F, 0xA5],
      [0x2591, 0x7F], [0x2592, 0x7E],
      [0x2593, 0xA9], [0x2594, 0xA3],
      [0x2595, 0xA7], [0x2596, 0xBB],
      [0x2597, 0xAC], [0x2598, 0xBE],
      [0x259A, 0xBF], [0x259D, 0xBC],
      [0x2713, 0xBA]
    ]
  }
};

for (let mapping of Object.entries(mappings)) {
  let output = '';

  if(process.argv.length === 2) {
    output += `
#ifndef ${mapping[1].define}
#define ${mapping[1].define} ${mapping[1].values.length}

static const ${mapping[1].type} ${mapping[0]}[${mapping[1].define}] = `;
  output += replaceAll(JSON.stringify(mapping[1].values, (key, value) => {
    if( typeof value === 'number'){
      return '0x' + value.toString(16)
    }
    return value
  }), {
  '[[': '{\n  {',
  '[': '  {',
  ']]': '}\n}',
  '],': '},\n',
  ',': ', ',
  '"': ''

  });

    output += ';\n#endif\n';
  }
  else {
    //z88dk version
    output += `
#ifndef ${mapping[1].define}
#define ${mapping[1].define} ${mapping[1].values.length}

static const ${mapping[1].type}
`;

    for(let i = 0; i < mapping[1].values.length; i++) {
      output += `  ${mapping[0]}${i+1} = ` + replaceAll(JSON.stringify(mapping[1].values[i], (key, value) => {
        if( typeof value === 'number'){
          return '0x' + value.toString(16)
        }
        return value
      }), {
        '[': '{',
        ']': '}',
        ',': ', ',
        '"': ''
      });

      if(i !== mapping[1].values.length-1) {
        output+= ',\n';
      }
      else {
        output += ';\n';
      }
    }

    output += `
static const ${mapping[1].type} * ${mapping[0]}[${mapping[1].define}] = {
`;

    for(let i = 0; i < mapping[1].values.length; i++) {
      output += `  &${mapping[0]}${i+1}`;
      if(i !== mapping[1].values.length-1) {
        output+= ',\n';
      }
      else {
        output += '\n};\n#endif\n';
      }
    }
  }

  fs.writeFileSync(`${mapping[1].filename}.h`, output);
}



