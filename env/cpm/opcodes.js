const fs = require('fs');

const replacements = [
  [
    " MOV A,A",
    " ld      a,a"
  ],
  [
    " MOV A,B",
    " ld      a,b"
  ],
  [
    " MOV A,C",
    " ld      a,c"
  ],
  [
    " MOV A,D",
    " ld      a,d"
  ],
  [
    " MOV A,E",
    " ld      a,e"
  ],
  [
    " MOV A,H",
    " ld      a,h"
  ],
  [
    " MOV A,L",
    " ld      a,l"
  ],
  [
    " MOV A,M",
    " ld      a,\\(hl\\)"
  ],
  [
    " LDAX B",
    " ld      a,\\(bc\\)"
  ],
  [
    " LDAX D",
    " ld      a,\\(de\\)"
  ],
  [
    " LDA $1",
    " ld      a,\\(([^)]+)\\)"
  ],
  [
    " MOV B,A",
    " ld      b,a"
  ],
  [
    " MOV B,B",
    " ld      b,b"
  ],
  [
    " MOV B,C",
    " ld      b,c"
  ],
  [
    " MOV B,D",
    " ld      b,d"
  ],
  [
    " MOV B,E",
    " ld      b,e"
  ],
  [
    " MOV B,H",
    " ld      b,h"
  ],
  [
    " MOV B,L",
    " ld      b,l"
  ],
  [
    " MOV B,M",
    " ld      b,\\(hl\\)"
  ],
  [
    " MOV C,A",
    " ld      c,a"
  ],
  [
    " MOV C,B",
    " ld      c,b"
  ],
  [
    " MOV C,C",
    " ld      c,c"
  ],
  [
    " MOV C,D",
    " ld      c,d"
  ],
  [
    " MOV C,E",
    " ld      c,e"
  ],
  [
    " MOV C,H",
    " ld      c,h"
  ],
  [
    " MOV C,L",
    " ld      c,l"
  ],
  [
    " MOV C,M",
    " ld      c,\\(hl\\)"
  ],
  [
    " MOV D,A",
    " ld      d,a"
  ],
  [
    " MOV D,B",
    " ld      d,b"
  ],
  [
    " MOV D,C",
    " ld      d,c"
  ],
  [
    " MOV D,D",
    " ld      d,d"
  ],
  [
    " MOV D,E",
    " ld      d,e"
  ],
  [
    " MOV D,H",
    " ld      d,h"
  ],
  [
    " MOV D,L",
    " ld      d,l"
  ],
  [
    " MOV D,M",
    " ld      d,\\(hl\\)"
  ],
  [
    " MOV E,A",
    " ld      e,a"
  ],
  [
    " MOV E,B",
    " ld      e,b"
  ],
  [
    " MOV E,C",
    " ld      e,c"
  ],
  [
    " MOV E,D",
    " ld      e,d"
  ],
  [
    " MOV E,E",
    " ld      e,e"
  ],
  [
    " MOV E,H",
    " ld      e,h"
  ],
  [
    " MOV E,L",
    " ld      e,l"
  ],
  [
    " MOV E,M",
    " ld      e,\\(hl\\)"
  ],
  [
    " MOV H,A",
    " ld      h,a"
  ],
  [
    " MOV H,B",
    " ld      h,b"
  ],
  [
    " MOV H,C",
    " ld      h,c"
  ],
  [
    " MOV H,D",
    " ld      h,d"
  ],
  [
    " MOV H,E",
    " ld      h,e"
  ],
  [
    " MOV H,H",
    " ld      h,h"
  ],
  [
    " MOV H,L",
    " ld      h,l"
  ],
  [
    " MOV H,M",
    " ld      h,\\(hl\\)"
  ],
  [
    " MOV L,A",
    " ld      l,a"
  ],
  [
    " MOV L,B",
    " ld      l,b"
  ],
  [
    " MOV L,C",
    " ld      l,c"
  ],
  [
    " MOV L,D",
    " ld      l,d"
  ],
  [
    " MOV L,E",
    " ld      l,e"
  ],
  [
    " MOV L,H",
    " ld      l,h"
  ],
  [
    " MOV L,L",
    " ld      l,l"
  ],
  [
    " MOV L,M",
    " ld      l,\\(hl\\)"
  ],
  [
    " MOV M,A",
    " ld      \\(hl\\),a"
  ],
  [
    " MOV M,B",
    " ld      \\(hl\\),b"
  ],
  [
    " MOV M,C",
    " ld      \\(hl\\),c"
  ],
  [
    " MOV M,D",
    " ld      \\(hl\\),d"
  ],
  [
    " MOV M,E",
    " ld      \\(hl\\),e"
  ],
  [
    " MOV M,H",
    " ld      \\(hl\\),h"
  ],
  [
    " MOV M,L",
    " ld      \\(hl\\),l"
  ],
  [
    " MVI A,$1",
    " ld      a,([^ ;()]+)"
  ],
  [
    " MVI B,$1",
    " ld      b,([^ ;()]+)"
  ],
  [
    " MVI C,$1",
    " ld      c,([^ ;()]+)"
  ],
  [
    " MVI D,$1",
    " ld      d,([^ ;()]+)"
  ],
  [
    " MVI E,$1",
    " ld      e,([^ ;()]+)"
  ],
  [
    " MVI H,$1",
    " ld      h,([^ ;()]+)"
  ],
  [
    " MVI L,$1",
    " ld      l,([^ ;()]+)"
  ],
  [
    " MVI M,$1",
    " ld      \\(hl\\),([^ ;()]+)"
  ],
  [
    " STAX B",
    " ld      \\(bc\\),a"
  ],
  [
    " STAX D",
    " ld      \\(de\\),a"
  ],
  [
    " STA $1",
    " ld      \\(([^)]+)\\),a"
  ],
  [
    " LXI B,$1",
    " ld      bc,([^ ;()]+)"
  ],
  [
    " LXI D,$1",
    " ld      de,([^ ;()]+)"
  ],
  [
    " LXI H,$1",
    " ld      hl,([^ ();]+)"
  ],
  [
    " LXI SP,$1",
    " ld      sp,([^ ();]+)"
  ],
  [
    " LHLD $1",
    " ld      hl,\\(([^)]+)\\)"
  ],
  [
    " SHLD $1",
    " ld      \\(([^)]+)\\),hl"
  ],
  [
    " SPHL",
    " ld      sp,hl"
  ],
  [
    " XCHG",
    " ex      de,hl"
  ],
  [
    " XTHL",
    " ex      \\(sp\\),hl"
  ],
  [
    " ADD A ",
    " add     a "
  ],
  [
    " ADD B ",
    " add     b "
  ],
  [
    " ADD C ",
    " add     c "
  ],
  [
    " ADD D ",
    " add     d "
  ],
  [
    " ADD E ",
    " add     e "
  ],
  [
    " ADD H ",
    " add     h "
  ],
  [
    " ADD L ",
    " add     l "
  ],
  [
    " ADD M ",
    " add     \\(hl\\) "
  ],
  [
    " ADI $1 ",
    " add     ([^ ;,()]+) "
  ],
  [
    " ADC A ",
    " adc     a "
  ],
  [
    " ADC B ",
    " adc     b "
  ],
  [
    " ADC C ",
    " adc     c "
  ],
  [
    " ADC D ",
    " adc     d "
  ],
  [
    " ADC E ",
    " adc     e "
  ],
  [
    " ADC H ",
    " adc     h "
  ],
  [
    " ADC L ",
    " adc     l "
  ],
  [
    " ADC M ",
    " adc     \\(hl\\) "
  ],
  [
    " ACI $1 ",
    " adc     ([^ ;,()]+) "
  ],
  [
    " SUB A ",
    " sub     a "
  ],
  [
    " SUB B ",
    " sub     b "
  ],
  [
    " SUB C ",
    " sub     c "
  ],
  [
    " SUB D ",
    " sub     d "
  ],
  [
    " SUB E ",
    " sub     e "
  ],
  [
    " SUB H ",
    " sub     h "
  ],
  [
    " SUB L ",
    " sub     l "
  ],
  [
    " SUB M ",
    " sub     \\(hl\\) "
  ],
  [
    " SUI $1 ",
    " sub     ([^ ;()]+) "
  ],
  [
    " SBB A ",
    " sbc     a "
  ],
  [
    " SBB B ",
    " sbc     b "
  ],
  [
    " SBB C ",
    " sbc     c "
  ],
  [
    " SBB D ",
    " sbc     d "
  ],
  [
    " SBB E ",
    " sbc     e "
  ],
  [
    " SBB H ",
    " sbc     h "
  ],
  [
    " SBB L ",
    " sbc     l "
  ],
  [
    " SBB M ",
    " sbc     \\(hl\\) "
  ],
  [
    " SBI $$$1 ",
    " sbc     (\\$[^ ;,()]+) "
  ],
  [
    " DAD B",
    " add     hl,bc"
  ],
  [
    " DAD D",
    " add     hl,de"
  ],
  [
    " DAD H",
    " add     hl,hl"
  ],
  [
    " DAD SP",
    " add     hl,sp"
  ],
  [
    " DI",
    " di"
  ],
  [
    " EI",
    " ei"
  ],
  [
    " NOP",
    " nop"
  ],
  [
    " HLT",
    " hlt"
  ],
  [
    " INR A ",
    " inc     a "
  ],
  [
    " INR B ",
    " inc     b "
  ],
  [
    " INR C ",
    " inc     c "
  ],
  [
    " INR D ",
    " inc     d "
  ],
  [
    " INR E ",
    " inc     e "
  ],
  [
    " INR H ",
    " inc     h "
  ],
  [
    " INR L ",
    " inc     l "
  ],
  [
    " INR M ",
    " inc     \\(hl\\) "
  ],
  [
    " DCR A ",
    " dec     a "
  ],
  [
    " DCR B ",
    " dec     b "
  ],
  [
    " DCR C ",
    " dec     c "
  ],
  [
    " DCR D ",
    " dec     d "
  ],
  [
    " DCR E ",
    " dec     e "
  ],
  [
    " DCR H ",
    " dec     h "
  ],
  [
    " DCR L ",
    " dec     l "
  ],
  [
    " DCR M ",
    " dec     \\(hl\\) "
  ],
  [
    " INX B",
    " inc     bc"
  ],
  [
    " INX D",
    " inc     de"
  ],
  [
    " INX H",
    " inc     hl"
  ],
  [
    " INX SP",
    " inc     sp"
  ],
  [
    " DCX B",
    " dec     bc"
  ],
  [
    " DCX D",
    " dec     de"
  ],
  [
    " DCX H",
    " dec     hl"
  ],
  [
    " DCX SP",
    " dec     sp"
  ],
  [
    " DAA",
    " daa"
  ],
  [
    " CMA",
    " cpl"
  ],
  [
    " STC",
    " scf"
  ],
  [
    " CMC",
    " ccf"
  ],
  [
    " RLC",
    " rlca"
  ],
  [
    " RRC",
    " rrca"
  ],
  [
    " RAL",
    " rla"
  ],
  [
    " RAR",
    " rra"
  ],
  [
    " ANA A",
    " and     a"
  ],
  [
    " ANA B",
    " and     b"
  ],
  [
    " ANA C",
    " and     c"
  ],
  [
    " ANA D",
    " and     d"
  ],
  [
    " ANA E",
    " and     e"
  ],
  [
    " ANA H",
    " and     h"
  ],
  [
    " ANA L",
    " and     l"
  ],
  [
    " ANA M",
    " and     \\(hl\\)"
  ],
  [
    " ANI $1",
    " and     ([^ ;()]+)"
  ],
  [
    " XRA A",
    " xor     a"
  ],
  [
    " XRA B",
    " xor     b"
  ],
  [
    " XRA C",
    " xor     c"
  ],
  [
    " XRA D",
    " xor     d"
  ],
  [
    " XRA E",
    " xor     e"
  ],
  [
    " XRA H",
    " xor     h"
  ],
  [
    " XRA L",
    " xor     l"
  ],
  [
    " XRA M",
    " xor     \\(hl\\)"
  ],
  [
    " XRI $1",
    " xor     ([^ ;()]+)"
  ],
  [
    " ORA A",
    " or      a"
  ],
  [
    " ORA B",
    " or      b"
  ],
  [
    " ORA C",
    " or      c"
  ],
  [
    " ORA D",
    " or      d"
  ],
  [
    " ORA E",
    " or      e"
  ],
  [
    " ORA H",
    " or      h"
  ],
  [
    " ORA L",
    " or      l"
  ],
  [
    " ORA M",
    " or      \\(hl\\)"
  ],
  [
    " ORI $1",
    " or      ([^ ;()]+)"
  ],
  [
    " CMP A",
    " cp      a"
  ],
  [
    " CMP B",
    " cp      b"
  ],
  [
    " CMP C",
    " cp      c"
  ],
  [
    " CMP D",
    " cp      d"
  ],
  [
    " CMP E",
    " cp      e"
  ],
  [
    " CMP H",
    " cp      h"
  ],
  [
    " CMP L",
    " cp      l"
  ],
  [
    " CMP M",
    " cp      \\(hl\\)"
  ],
  [
    " CPI $1",
    " cp      ([^ ;()]+)"
  ],
  [
    " JMP $1",
    " jp[ ]+([^,;]+;)"
  ],
  [
    " JNZ $1",
    " jp      nz,([^ ;()]+)"
  ],
  [
    " JZ $1",
    " jp      z,([^ ;()]+)"
  ],
  [
    " JNC $1",
    " jp      nc,([^ ;()]+)"
  ],
  [
    " JC $1",
    " jp      c,([^ ;()]+)"
  ],
  [
    " JPO $1",
    " jp      po,([^ ;()]+)"
  ],
  [
    " JPE $1",
    " jp      pe,([^ ;()]+)"
  ],
  [
    " JP $1",
    " jp      p,([^ ;()]+)"
  ],
  [
    " JM $1",
    " jp      m,([^ ;()]+)"
  ],
  [
    " PCHL",
    " jp      \\(hl\\)"
  ],
  [
    " CALL $1",
    " call[ ]+([^, ;]+[ ;])"
  ],
  [
    " CNZ $1",
    " call    nz,([^ ;()]+)"
  ],
  [
    " CZ $1",
    " call    z,([^ ;()]+)"
  ],
  [
    " CNC $1",
    " call    nc,([^ ;()]+)"
  ],
  [
    " CC $1",
    " call    c,([^ ;()]+)"
  ],
  [
    " CPO $1",
    " call    po,([^ ;()]+)"
  ],
  [
    " CPE $1",
    " call    pe,([^ ;()]+)"
  ],
  [
    " CP $1",
    " call    p,([^ ;()]+)"
  ],
  [
    " CM $1",
    " call    m,([^ ;()]+)"
  ],
  [
    " RET$1",
    " ret([ ]+;)"
  ],
  [
    " RNZ",
    " ret     nz"
  ],
  [
    " RZ",
    " ret     z"
  ],
  [
    " RNC",
    " ret     nc"
  ],
  [
    " RC",
    " ret     c"
  ],
  [
    " RPO",
    " ret     po"
  ],
  [
    " RPE",
    " ret     pe"
  ],
  [
    " RP",
    " ret     p"
  ],
  [
    " RM",
    " ret     m"
  ],
  [
    " RST 0",
    " rst     0"
  ],
  [
    " RST 1",
    " rst     8"
  ],
  [
    " RST 2",
    " rst     \\$10"
  ],
  [
    " RST 3",
    " rst     \\$18"
  ],
  [
    " RST 4",
    " rst     \\$20"
  ],
  [
    " RST 5",
    " rst     \\$28"
  ],
  [
    " RST 6",
    " rst     \\$30"
  ],
  [
    " RST 7",
    " rst     \\$38"
  ],
  [
    " PUSH B",
    " push    bc"
  ],
  [
    " PUSH D",
    " push    de"
  ],
  [
    " PUSH H",
    " push    hl"
  ],
  [
    " PUSH PSW",
    " push    af"
  ],
  [
    " POP B",
    " pop     bc"
  ],
  [
    " POP D",
    " pop     de"
  ],
  [
    " POP H",
    " pop     hl"
  ],
  [
    " POP PSW",
    " pop     af"
  ],
  [
    " IN $1",
    " in      a,\\(([^)]+)\\)"
  ],
  [
    " OUT $1",
    " out     \\(([^)]+)\\),a"
  ]
];

function replaceBulk( str, findArray, replaceArray) {
  const findArray2 = findArray.map(item => new RegExp(`^${item}$`));
  let regex = [];
  let map = {}; 

  str = str.replace(
    new RegExp(findArray.join('|'), 'g'),
    matched => {
      const index = findArray2.findIndex(item => matched.match(item));
      //console.log(index);

      if(index != -1) {
        const text = matched.replace(findArray2[index], replaceArray[index]);
        //console.log(matched, findArray[index], replaceArray[index], text);
        return text;
      }

      return matched;
    }
  );
  return str;
}


const file = fs.readFileSync('qrycsv1a.asm', 'utf-8');
const replaced = replaceBulk(file, replacements.map(item => item[1]), replacements.map(item => item[0]));

console.log(replaced);
