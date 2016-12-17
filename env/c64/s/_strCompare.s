.include "../code2.s"
.export _strCompare
.proc	_strCompare: near


;
; ) {
;
	jsr     pushax
;
; unsigned char *offset1 = *str1, *offset2 = *str2;
;
	ldy     #$09
	jsr     ldaxysp
	jsr     pushw
	ldy     #$09
	jsr     ldaxysp
	jsr     pushw
;
; long char1 = 0, char2 = 0;
;
	ldx     #$00
	stx     sreg
	stx     sreg+1
	jsr     pushl0
	jsr     pushl0
;
; int firstChar = TRUE, comparison = 0, char1found = FALSE;
;
	jsr     decsp4
	lda     #$01
	jsr     pusha0
	jsr     push0
	jsr     push0
;
; int bytesMatched1 = 0, bytesMatched2 = 0;
;
	jsr     push0
	jsr     push0
;
; int accentcheck = 0, combinerResult;
;
	jsr     push0
;
; for( ; ; ) {  /* we'll quit from this function via other means */
;
	jsr     decsp2
;
; if (*offset2 == 0) {
;
L0748:	ldy     #$1B
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	bne     L0749
;
; if(*offset1 == 0) {
;
	ldy     #$1D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	bne     L15DF
;
; if(accentcheck == 0) {
;
	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
;
; return 0;
;
	jeq     L0746
;
; accentcheck = 2;
;
	lda     #$02
	dey
	jsr     staxysp
;
; if(caseSensitive == 2) {
;
	ldy     #$23
	lda     (sp),y
	bne     L0754
	dey
	lda     (sp),y
	cmp     #$02
	bne     L0754
;
; caseSensitive = 1;
;
	lda     #$01
	jsr     staxysp
;
; offset1 = *str1;
;
L0754:	ldy     #$27
	jsr     ldaxysp
	jsr     ldaxi
	ldy     #$1C
	jsr     staxysp
;
; offset2 = *str2;
;
	ldy     #$25
	jsr     ldaxysp
	jsr     ldaxi
	ldy     #$1A
	jsr     staxysp
;
; continue;
;
	jmp     L0748
;
; return 1;
;
L15DF:	lda     #$01
	jmp     L0746
;
; else if(*offset1 == 0) {
;
L0749:	ldy     #$1D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	bne     L075F
;
; return -1;
;
	ldx     #$FF
	txa
	jmp     L0746
;
; else if (char1found == 0) {
;
L075F:	ldy     #$08
	lda     (sp),y
	iny
	ora     (sp),y
	jne     L15E0
;
; char1 = (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get1))(&offset1, str1, 0, &bytesMatched1, get1);
;
	ldy     #$23
	jsr     pushwysp
	lda     #$1E
	jsr     leaa0sp
	jsr     pushax
	ldy     #$2D
	jsr     pushwysp
	jsr     push0
	lda     #$0E
	jsr     leaa0sp
	jsr     pushax
	ldy     #$2B
	jsr     ldaxysp
	pha
	ldy     #$08
	lda     (sp),y
	sta     jmpvec+1
	iny
	lda     (sp),y
	sta     jmpvec+2
	pla
	jsr     jmpvec
	jsr     incsp2
	jsr     axlong
	ldy     #$16
	jsr     steaxysp
;
; if (char1 != 0x34F) {
;
	ldy     #$19
	jsr     ldeaxysp
	jsr     pusheax
	ldx     #$03
	lda     #$00
	sta     sreg
	sta     sreg+1
	lda     #$4F
	jsr     tosneeax
	jeq     L076F
;
; char2 = (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))(&offset2, str2, 0, &bytesMatched2, get2);
;
	ldy     #$21
	jsr     pushwysp
	lda     #$1C
	jsr     leaa0sp
	jsr     pushax
	ldy     #$2B
	jsr     pushwysp
	jsr     push0
	lda     #$0C
	jsr     leaa0sp
	jsr     pushax
	ldy     #$29
	jsr     ldaxysp
	pha
	ldy     #$08
	lda     (sp),y
	sta     jmpvec+1
	iny
	lda     (sp),y
	sta     jmpvec+2
	pla
	jsr     jmpvec
	jsr     incsp2
	jsr     axlong
	ldy     #$12
	jsr     steaxysp
;
; if((entry1 = getLookupTableEntry(&offset1, str1, &bytesMatched1, get1, firstChar))) {
;
	lda     #$1C
	jsr     leaa0sp
	jsr     pushax
	ldy     #$2B
	jsr     pushwysp
	lda     #$0A
	jsr     leaa0sp
	jsr     pushax
	ldy     #$29
	jsr     pushwysp
	ldy     #$15
	jsr     ldaxysp
	jsr     _getLookupTableEntry
	ldy     #$10
	jsr     staxysp
	stx     tmp1
	ora     tmp1
	jeq     L077A
;
; if(char2 != 0x34F) {
;
	ldy     #$15
	jsr     ldeaxysp
	jsr     pusheax
	ldx     #$03
	lda     #$00
	sta     sreg
	sta     sreg+1
	lda     #$4F
	jsr     tosneeax
	jeq     L0783
;
; if((entry2 = getLookupTableEntry(&offset2, str2, &bytesMatched2, get2, firstChar))) {
;
	lda     #$1A
	jsr     leaa0sp
	jsr     pushax
	ldy     #$29
	jsr     pushwysp
	lda     #$08
	jsr     leaa0sp
	jsr     pushax
	ldy     #$27
	jsr     pushwysp
	ldy     #$15
	jsr     ldaxysp
	jsr     _getLookupTableEntry
	ldy     #$0E
	jsr     staxysp
	stx     tmp1
	ora     tmp1
	jeq     L0785
;
; if(entry1->script == entry2->script) {
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	sta     sreg
	stx     sreg+1
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     sreg+1
	jne     L078E
	cmp     sreg
	jne     L078E
;
; if(entry1->index == 0 && entry2->index == 0) {
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	cpx     #$00
	bne     L0790
	cmp     #$00
	bne     L0790
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	cpx     #$00
	bne     L0790
	cmp     #$00
	bne     L0790
;
; comparison = strNumberCompare((char *)offset1, (char *)offset2);
;
	ldy     #$1F
	jsr     pushwysp
	ldy     #$1D
	jsr     ldaxysp
	jsr     _strNumberCompare
	ldy     #$0A
;
; else if(caseSensitive == 1) {
;
	jmp     L15DC
L0790:	ldy     #$23
	lda     (sp),y
	bne     L079B
	dey
	lda     (sp),y
	cmp     #$01
	bne     L079B
;
; comparison = entry1->index - entry2->index;
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     pushwidx
	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	jsr     tossubax
	ldy     #$0A
;
; else {
;
	jmp     L15DC
;
; comparison = (entry1->index - (entry1->islower)) - (entry2->index - (entry2->islower));
;
L079B:	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     pushwidx
	ldy     #$13
	jsr     ldaxysp
	ldy     #$07
	jsr     ldaxidx
	jsr     tossubax
	jsr     pushax
	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     pushwidx
	ldy     #$13
	jsr     ldaxysp
	ldy     #$07
	jsr     ldaxidx
	jsr     tossubax
	jsr     tossubax
	ldy     #$0A
	jsr     staxysp
;
; if((entry1->index - entry2->index) != 0 && caseSensitive == 2) {
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     pushwidx
	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	jsr     tossubax
	cpx     #$00
	bne     L15E1
	cmp     #$00
	beq     L07A7
L15E1:	ldy     #$23
	lda     (sp),y
	bne     L07A7
	dey
	lda     (sp),y
	cmp     #$02
	bne     L07A7
;
; accentcheck = 1;
;
	ldx     #$00
	lda     #$01
	ldy     #$02
L15DC:	jsr     staxysp
;
; if(comparison != 0) {
;
L07A7:	ldy     #$0A
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L07C0
;
; return comparison > 0 ? 1 : -1;
;
	jsr     ldaxysp
	cmp     #$01
	txa
	sbc     #$00
	bvs     L07B4
	eor     #$80
L07B4:	bpl     L07B5
	ldx     #$00
	lda     #$01
	jmp     L0746
L07B5:	ldx     #$FF
	txa
	jmp     L0746
;
; return entry1->script > entry2->script ? 1 : -1;
;
L078E:	ldy     #$11
	jsr     ldaxysp
	ldy     #$03
	jsr     pushwidx
	ldy     #$11
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jsr     tosicmp
	bmi     L07BB
	beq     L07BB
	ldx     #$00
	lda     #$01
	jmp     L0746
L07BB:	ldx     #$FF
	txa
	jmp     L0746
;
; else if(entry1->script != char2) {
;
L0785:	ldy     #$11
	jsr     ldaxysp
	ldy     #$03
	jsr     pushwidx
	ldy     #$17
	jsr     ldeaxysp
	jsr     toslong
	jsr     tosneeax
	beq     L07C0
;
; return (entry1->script > char2) ? 1 : -1;
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$03
	jsr     pushwidx
	ldy     #$17
	jsr     ldeaxysp
	jsr     toslong
	jsr     tosgteax
	beq     L07C4
	ldx     #$00
	lda     #$01
	jmp     L0746
L07C4:	ldx     #$FF
	txa
	jmp     L0746
;
; str1, str2,
;
L07C0:	ldy     #$29
	jsr     pushwysp
	ldy     #$29
	jsr     pushwysp
;
; &offset1, &offset2,
;
	lda     #$20
	jsr     leaa0sp
	jsr     pushax
	lda     #$20
	jsr     leaa0sp
	jsr     pushax
;
; get1, get2,
;
	ldy     #$2B
	jsr     pushwysp
	ldy     #$2B
	jsr     pushwysp
;
; &bytesMatched1, &bytesMatched2,
;
	lda     #$12
	jsr     leaa0sp
	jsr     pushax
	lda     #$12
	jsr     leaa0sp
	jsr     pushax
;
; &accentcheck)) != 0) {
;
	lda     #$12
	jsr     leaa0sp
	jsr     _consumeCombiningChars
	jsr     stax0sp
	cpx     #$00
	bne     L15E3
	cmp     #$00
	beq     L07C8
;
; return combinerResult;
;
L15E3:	jsr     ldax0sp
	jmp     L0746
;
; if(firstChar) {
;
L07C8:	ldy     #$0D
	lda     (sp),y
	dey
	ora     (sp),y
	jeq     L0748
;
; firstChar = FALSE;
;
	txa
	jsr     staxysp
;
; else {
;
	jmp     L0748
;
; char1found = 1;   /* in lookup */
;
L0783:	ldx     #$00
	lda     #$01
	ldy     #$08
	jsr     staxysp
;
; offset2 += bytesMatched2;
;
	ldy     #$05
	jsr     ldaxysp
	ldy     #$1A
	jsr     addeqysp
;
; else if(char2 != 0x34F) {
;
	jmp     L0748
L077A:	ldy     #$15
	jsr     ldeaxysp
	jsr     pusheax
	ldx     #$03
	lda     #$00
	sta     sreg
	sta     sreg+1
	lda     #$4F
	jsr     tosneeax
	jeq     L07E1
;
; if((entry2 = getLookupTableEntry(&offset2, str2, &bytesMatched2, get2, firstChar))){
;
	lda     #$1A
	jsr     leaa0sp
	jsr     pushax
	ldy     #$29
	jsr     pushwysp
	lda     #$08
	jsr     leaa0sp
	jsr     pushax
	ldy     #$27
	jsr     pushwysp
	ldy     #$15
	jsr     ldaxysp
	jsr     _getLookupTableEntry
	ldy     #$0E
	jsr     staxysp
	stx     tmp1
	ora     tmp1
	beq     L07E3
;
; if(char1 != entry2->script) {
;
	ldy     #$19
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$13
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jsr     axlong
	jsr     tosneeax
	beq     L07F5
;
; return (char1 > entry2->script) ? 1: -1;
;
	ldy     #$19
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$13
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jsr     axlong
	jsr     tosgteax
	beq     L07F0
	ldx     #$00
	lda     #$01
	jmp     L0746
L07F0:	ldx     #$FF
	txa
	jmp     L0746
;
; else if(char1 != char2) {
;
L07E3:	ldy     #$19
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$19
	jsr     ldeaxysp
	jsr     tosneeax
	beq     L07F5
;
; return (char1 > char2) ? 1: -1; 
;
	ldy     #$19
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$19
	jsr     ldeaxysp
	jsr     tosgteax
	beq     L07F9
	ldx     #$00
	lda     #$01
	jmp     L0746
L07F9:	ldx     #$FF
	txa
	jmp     L0746
;
; str1, str2,
;
L07F5:	ldy     #$29
	jsr     pushwysp
	ldy     #$29
	jsr     pushwysp
;
; &offset1, &offset2,
;
	lda     #$20
	jsr     leaa0sp
	jsr     pushax
	lda     #$20
	jsr     leaa0sp
	jsr     pushax
;
; get1, get2,
;
	ldy     #$2B
	jsr     pushwysp
	ldy     #$2B
	jsr     pushwysp
;
; &bytesMatched1, &bytesMatched2,
;
	lda     #$12
	jsr     leaa0sp
	jsr     pushax
	lda     #$12
	jsr     leaa0sp
	jsr     pushax
;
; &accentcheck)) != 0) {
;
	lda     #$12
	jsr     leaa0sp
	jsr     _consumeCombiningChars
	jsr     stax0sp
	cpx     #$00
	bne     L15E4
	cmp     #$00
	beq     L07FD
;
; return combinerResult;
;
L15E4:	jsr     ldax0sp
	jmp     L0746
;
; if(firstChar) {
;
L07FD:	ldy     #$0D
	lda     (sp),y
	dey
	ora     (sp),y
	jeq     L0748
;
; firstChar = FALSE;
;
	txa
	jsr     staxysp
;
; else {
;
	jmp     L0748
;
; char1found = 2;   /* in lookup */
;
L07E1:	ldx     #$00
	lda     #$02
	ldy     #$08
	jsr     staxysp
;
; offset2 += bytesMatched2;
;
	ldy     #$05
	jsr     ldaxysp
	ldy     #$1A
	jsr     addeqysp
;
; else {
;
	jmp     L0748
;
; offset1 += bytesMatched1;
;
L076F:	ldy     #$07
	jsr     ldaxysp
	ldy     #$1C
	jsr     addeqysp
;
; else if (char1found == 1) {
;
	jmp     L0748
L15E0:	lda     (sp),y
	jne     L0819
	dey
	lda     (sp),y
	cmp     #$01
	jne     L0819
;
; char2 = (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))(&offset1, str1, 0, &bytesMatched2, get2);
;
	ldy     #$21
	jsr     pushwysp
	lda     #$1E
	jsr     leaa0sp
	jsr     pushax
	ldy     #$2D
	jsr     pushwysp
	jsr     push0
	lda     #$0C
	jsr     leaa0sp
	jsr     pushax
	ldy     #$29
	jsr     ldaxysp
	pha
	ldy     #$08
	lda     (sp),y
	sta     jmpvec+1
	iny
	lda     (sp),y
	sta     jmpvec+2
	pla
	jsr     jmpvec
	jsr     incsp2
	jsr     axlong
	ldy     #$12
	jsr     steaxysp
;
; if(char2 != 0x34F) {
;
	ldy     #$15
	jsr     ldeaxysp
	jsr     pusheax
	ldx     #$03
	lda     #$00
	sta     sreg
	sta     sreg+1
	lda     #$4F
	jsr     tosneeax
	jeq     L0825
;
; if((entry2 = getLookupTableEntry(&offset2, str2, &bytesMatched2, get2, firstChar))) {
;
	lda     #$1A
	jsr     leaa0sp
	jsr     pushax
	ldy     #$29
	jsr     pushwysp
	lda     #$08
	jsr     leaa0sp
	jsr     pushax
	ldy     #$27
	jsr     pushwysp
	ldy     #$15
	jsr     ldaxysp
	jsr     _getLookupTableEntry
	ldy     #$0E
	jsr     staxysp
	stx     tmp1
	ora     tmp1
	jeq     L0827
;
; if(entry1->script == entry2->script) {
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	sta     sreg
	stx     sreg+1
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     sreg+1
	jne     L0830
	cmp     sreg
	jne     L0830
;
; if(entry1->index == 0 && entry2->index == 0) {
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	cpx     #$00
	bne     L0832
	cmp     #$00
	bne     L0832
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	cpx     #$00
	bne     L0832
	cmp     #$00
	bne     L0832
;
; comparison = strNumberCompare((char *)offset1, (char *)offset2);
;
	ldy     #$1F
	jsr     pushwysp
	ldy     #$1D
	jsr     ldaxysp
	jsr     _strNumberCompare
	ldy     #$0A
;
; else if(caseSensitive == 1) {
;
	jmp     L15DD
L0832:	ldy     #$23
	lda     (sp),y
	bne     L083D
	dey
	lda     (sp),y
	cmp     #$01
	bne     L083D
;
; comparison = entry1->index - entry2->index;
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     pushwidx
	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	jsr     tossubax
	ldy     #$0A
;
; else {
;
	jmp     L15DD
;
; comparison = (entry1->index - (entry1->islower)) - (entry2->index - (entry2->islower));
;
L083D:	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     pushwidx
	ldy     #$13
	jsr     ldaxysp
	ldy     #$07
	jsr     ldaxidx
	jsr     tossubax
	jsr     pushax
	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     pushwidx
	ldy     #$13
	jsr     ldaxysp
	ldy     #$07
	jsr     ldaxidx
	jsr     tossubax
	jsr     tossubax
	ldy     #$0A
	jsr     staxysp
;
; if((entry1->index - entry2->index) != 0 && caseSensitive == 2) {
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     pushwidx
	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	jsr     tossubax
	cpx     #$00
	bne     L15E5
	cmp     #$00
	beq     L0849
L15E5:	ldy     #$23
	lda     (sp),y
	bne     L0849
	dey
	lda     (sp),y
	cmp     #$02
	bne     L0849
;
; accentcheck = 1;
;
	ldx     #$00
	lda     #$01
	ldy     #$02
L15DD:	jsr     staxysp
;
; if(comparison != 0) {
;
L0849:	ldy     #$0A
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L0862
;
; return comparison > 0 ? 1 : -1;
;
	jsr     ldaxysp
	cmp     #$01
	txa
	sbc     #$00
	bvs     L0856
	eor     #$80
L0856:	bpl     L0857
	ldx     #$00
	lda     #$01
	jmp     L0746
L0857:	ldx     #$FF
	txa
	jmp     L0746
;
; return entry1->script > entry2->script ? 1 : -1;
;
L0830:	ldy     #$11
	jsr     ldaxysp
	ldy     #$03
	jsr     pushwidx
	ldy     #$11
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jsr     tosicmp
	bmi     L085D
	beq     L085D
	ldx     #$00
	lda     #$01
	jmp     L0746
L085D:	ldx     #$FF
	txa
	jmp     L0746
;
; else if(entry1->script != char2) {
;
L0827:	ldy     #$11
	jsr     ldaxysp
	ldy     #$03
	jsr     pushwidx
	ldy     #$17
	jsr     ldeaxysp
	jsr     toslong
	jsr     tosneeax
	beq     L0862
;
; return (entry1->script > char2) ? 1: -1; 
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$03
	jsr     pushwidx
	ldy     #$17
	jsr     ldeaxysp
	jsr     toslong
	jsr     tosgteax
	beq     L0866
	ldx     #$00
	lda     #$01
	jmp     L0746
L0866:	ldx     #$FF
	txa
	jmp     L0746
;
; str1, str2,
;
L0862:	ldy     #$29
	jsr     pushwysp
	ldy     #$29
	jsr     pushwysp
;
; &offset1, &offset2,
;
	lda     #$20
	jsr     leaa0sp
	jsr     pushax
	lda     #$20
	jsr     leaa0sp
	jsr     pushax
;
; get1, get2,
;
	ldy     #$2B
	jsr     pushwysp
	ldy     #$2B
	jsr     pushwysp
;
; &bytesMatched1, &bytesMatched2,
;
	lda     #$12
	jsr     leaa0sp
	jsr     pushax
	lda     #$12
	jsr     leaa0sp
	jsr     pushax
;
; &accentcheck)) != 0) {
;
	lda     #$12
	jsr     leaa0sp
	jsr     _consumeCombiningChars
	jsr     stax0sp
	cpx     #$00
	bne     L15E7
	cmp     #$00
	beq     L086A
;
; return combinerResult;
;
L15E7:	jsr     ldax0sp
	jmp     L0746
;
; if(firstChar) {
;
L086A:	ldy     #$0D
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L15E9
;
; firstChar = FALSE;
;
	txa
	jsr     staxysp
;
; char1found = 0;
;
L15E9:	ldy     #$08
	jsr     staxysp
;
; else {
;
	jmp     L0748
;
; offset2 += bytesMatched2;
;
L0825:	ldy     #$05
	jsr     ldaxysp
	ldy     #$1A
	jsr     addeqysp
;
; else {  
;
	jmp     L0748
;
; char2 = (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))(&offset2, str2, 0, &bytesMatched2, get2);
;
L0819:	ldy     #$21
	jsr     pushwysp
	lda     #$1C
	jsr     leaa0sp
	jsr     pushax
	ldy     #$2B
	jsr     pushwysp
	jsr     push0
	lda     #$0C
	jsr     leaa0sp
	jsr     pushax
	ldy     #$29
	jsr     ldaxysp
	pha
	ldy     #$08
	lda     (sp),y
	sta     jmpvec+1
	iny
	lda     (sp),y
	sta     jmpvec+2
	pla
	jsr     jmpvec
	jsr     incsp2
	jsr     axlong
	ldy     #$12
	jsr     steaxysp
;
; if(char2 != 0x34F) {
;
	ldy     #$15
	jsr     ldeaxysp
	jsr     pusheax
	ldx     #$03
	lda     #$00
	sta     sreg
	sta     sreg+1
	lda     #$4F
	jsr     tosneeax
	jeq     L088C
;
; if((entry2 = getLookupTableEntry(&offset2, str2, &bytesMatched2, get2, firstChar)) && char1 != entry2->script) {
;
	lda     #$1A
	jsr     leaa0sp
	jsr     pushax
	ldy     #$29
	jsr     pushwysp
	lda     #$08
	jsr     leaa0sp
	jsr     pushax
	ldy     #$27
	jsr     pushwysp
	ldy     #$15
	jsr     ldaxysp
	jsr     _getLookupTableEntry
	ldy     #$0E
	jsr     staxysp
	stx     tmp1
	ora     tmp1
	beq     L088E
	ldy     #$19
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$13
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jsr     axlong
	jsr     tosneeax
	beq     L088E
;
; return (char1 > entry2->script) ? 1: -1; 
;
	ldy     #$19
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$13
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jsr     axlong
	jsr     tosgteax
	beq     L089B
	ldx     #$00
	lda     #$01
	jmp     L0746
L089B:	ldx     #$FF
	txa
	jmp     L0746
;
; else if(char1 != char2) {
;
L088E:	ldy     #$19
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$19
	jsr     ldeaxysp
	jsr     tosneeax
	beq     L08A0
;
; return (char1 > char2) ? 1: -1; 
;
	ldy     #$19
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$19
	jsr     ldeaxysp
	jsr     tosgteax
	beq     L08A4
	ldx     #$00
	lda     #$01
	jmp     L0746
L08A4:	ldx     #$FF
	txa
	jmp     L0746
;
; str1, str2,
;
L08A0:	ldy     #$29
	jsr     pushwysp
	ldy     #$29
	jsr     pushwysp
;
; &offset1, &offset2,
;
	lda     #$20
	jsr     leaa0sp
	jsr     pushax
	lda     #$20
	jsr     leaa0sp
	jsr     pushax
;
; get1, get2,
;
	ldy     #$2B
	jsr     pushwysp
	ldy     #$2B
	jsr     pushwysp
;
; &bytesMatched1, &bytesMatched2,
;
	lda     #$12
	jsr     leaa0sp
	jsr     pushax
	lda     #$12
	jsr     leaa0sp
	jsr     pushax
;
; &accentcheck)) != 0) {
;
	lda     #$12
	jsr     leaa0sp
	jsr     _consumeCombiningChars
	jsr     stax0sp
	cpx     #$00
	bne     L15E8
	cmp     #$00
	beq     L15EA
;
; return combinerResult;
;
L15E8:	jsr     ldax0sp
	jmp     L0746
;
; char1found = 0;
;
L15EA:	ldy     #$08
	jsr     staxysp
;
; if(firstChar) {
;
	ldy     #$0D
	lda     (sp),y
	dey
	ora     (sp),y
	jeq     L0748
;
; firstChar = FALSE;
;
	txa
	jsr     staxysp
;
; else {
;
	jmp     L0748
;
; offset2 += bytesMatched2;
;
L088C:	ldy     #$05
	jsr     ldaxysp
	ldy     #$1A
	jsr     addeqysp
;
; } 
;
	jmp     L0748
;
; }
;
L0746:	ldy     #$28
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; __near__ unsigned char * __near__ strReplace (__near__ unsigned char *, __near__ unsigned char *, __near__ unsigned char *)
; ---------------------------------------------------------------


.endproc
