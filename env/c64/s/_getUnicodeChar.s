.include "../code2.s"
.export _getUnicodeChar
.proc	_getUnicodeChar: near


;
; ) {
;
	jsr     pushax
;
; struct hash2Entry* entry = NULL;
;
	jsr     push0
;
; int bytesread = 0;
;
	jsr     push0
;
; unsigned char *temp = (unsigned char *)((*offset) + plusBytes);
;
	jsr     decsp4
	ldy     #$11
	jsr     ldaxysp
	jsr     ldaxi
	sta     ptr1
	stx     ptr1+1
	ldy     #$0D
	jsr     ldaxysp
	clc
	adc     ptr1
	pha
	txa
	adc     ptr1+1
	tax
	pla
	jsr     pushax
;
; if(*temp < 0x80) {
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$80
	bcs     L0399
;
; *bytesMatched = 1;
;
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; return (long)(*temp);
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	ldx     #$00
	lda     (ptr1),y
	jsr     aulong
	jsr     axulong
	jmp     L0393
;
; else if(*temp > 0xC1) {
;
L0399:	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$C2
	jcc     L040A
;
; if (*temp < 0xE0) { 
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$E0
	jcs     L03A2
;
; (*(temp+1) & 0xC0) == 0x80
;
	jsr     ldax0sp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	and     #$C0
;
; ) {
;
	cmp     #$80
	jne     L040A
;
; codepoint = ((long)(*(temp)) << 6) + *(temp+1) - 0x3080;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	ldx     #$00
	lda     (ptr1),y
	jsr     aulong
	jsr     axulong
	jsr     asleax4
	jsr     asleax2
	jsr     pusheax
	ldy     #$05
	jsr     ldaxysp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	ldx     #$00
	lda     (ptr1),y
	jsr     axulong
	jsr     tosaddeax
	jsr     pusheax
	ldx     #$30
	lda     #$80
	jsr     tossub0ax
	ldy     #$02
	jsr     steaxysp
;
; if((entry = isInHash2(codepoint))) {
;
	ldy     #$05
	jsr     ldeaxysp
	jsr     _isInHash2
	ldy     #$08
	jsr     staxysp
	stx     tmp1
	ora     tmp1
	beq     L03AF
;
; bytesread = 2;
;
	ldx     #$00
	lda     #$02
	ldy     #$06
	jsr     staxysp
;
; else {
;
	jmp     L040A
;
; *bytesMatched = 2;
;
L03AF:	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$02
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; return codepoint;
;
	ldy     #$05
	jsr     ldeaxysp
	jmp     L0393
;
; else if (*temp < 0xF0) {
;
L03A2:	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$F0
	jcs     L03BB
;
; (*(temp+1) & 0xC0) == 0x80 &&
;
	jsr     ldax0sp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	and     #$C0
	cmp     #$80
	jne     L040A
;
; (*(temp) != 0xE0 || *(temp+1) > 0x9F) &&
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$E0
	bne     L1539
	jsr     ldax0sp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$A0
	jcc     L040A
;
; (*(temp+2) & 0xC0) == 0x80
;
L1539:	jsr     ldax0sp
	jsr     incax2
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	and     #$C0
;
; ) {
;
	cmp     #$80
	jne     L040A
;
; codepoint = ((long)(*(temp)) << 12) + ((long)(*(temp+1)) << 6) + (*(temp+2)) - 0xE2080;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	ldx     #$00
	lda     (ptr1),y
	jsr     aulong
	jsr     axulong
	ldy     sreg
	sty     sreg+1
	stx     sreg
	tax
	lda     #$00
	jsr     asleax4
	jsr     pusheax
	ldy     #$05
	jsr     ldaxysp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	ldx     #$00
	lda     (ptr1),y
	jsr     aulong
	jsr     axulong
	jsr     asleax4
	jsr     asleax2
	jsr     tosaddeax
	jsr     pusheax
	ldy     #$05
	jsr     ldaxysp
	jsr     incax2
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	ldx     #$00
	lda     (ptr1),y
	jsr     axulong
	jsr     tosaddeax
	jsr     pusheax
	ldx     #$20
	lda     #$0E
	sta     sreg
	lda     #$00
	sta     sreg+1
	lda     #$80
	jsr     tossubeax
	ldy     #$02
	jsr     steaxysp
;
; if((entry = isInHash2(codepoint))) {
;
	ldy     #$05
	jsr     ldeaxysp
	jsr     _isInHash2
	ldy     #$08
	jsr     staxysp
	stx     tmp1
	ora     tmp1
	beq     L03D5
;
; bytesread = 3;
;
	ldx     #$00
	lda     #$03
	ldy     #$06
	jsr     staxysp
;
; else {
;
	jmp     L040A
;
; *bytesMatched = 3;
;
L03D5:	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$03
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; return codepoint;
;
	ldy     #$05
	jsr     ldeaxysp
	jmp     L0393
;
; else if (*temp < 0xF5) {
;
L03BB:	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$F5
	jcs     L040A
;
; (*(temp+1) & 0xC0) == 0x80 &&
;
	jsr     ldax0sp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	and     #$C0
	cmp     #$80
	jne     L040A
;
; (*temp != 0xF0 || *(temp+1) > 0x8F) &&
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$F0
	bne     L153D
	jsr     ldax0sp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$90
	jcc     L040A
;
; (*temp != 0xF4 || *(temp+1) < 0x90) &&
;
L153D:	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$F4
	bne     L153E
	jsr     ldax0sp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$90
	jcs     L040A
;
; (*(temp+2) & 0xC0) == 0x80 &&
;
L153E:	jsr     ldax0sp
	jsr     incax2
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	and     #$C0
	cmp     #$80
	jne     L040A
;
; (*(temp+3) & 0xC0) == 0x80
;
	jsr     ldax0sp
	jsr     incax3
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	and     #$C0
;
; ) {
;
	cmp     #$80
	jne     L040A
;
; codepoint = (((long)(*temp)) << 18) + ((long)(*(temp+1)) << 12) + ((long)(*(temp+2)) << 6) + (*(temp+3)) - 0x3C82080;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	ldx     #$00
	lda     (ptr1),y
	jsr     aulong
	jsr     axulong
	stx     sreg+1
	sta     sreg
	lda     #$00
	tax
	jsr     asleax2
	jsr     pusheax
	ldy     #$05
	jsr     ldaxysp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	ldx     #$00
	lda     (ptr1),y
	jsr     aulong
	jsr     axulong
	ldy     sreg
	sty     sreg+1
	stx     sreg
	tax
	lda     #$00
	jsr     asleax4
	jsr     tosaddeax
	jsr     pusheax
	ldy     #$05
	jsr     ldaxysp
	jsr     incax2
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	ldx     #$00
	lda     (ptr1),y
	jsr     aulong
	jsr     axulong
	jsr     asleax4
	jsr     asleax2
	jsr     tosaddeax
	jsr     pusheax
	ldy     #$05
	jsr     ldaxysp
	jsr     incax3
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	ldx     #$00
	lda     (ptr1),y
	jsr     axulong
	jsr     tosaddeax
	jsr     pusheax
	ldx     #$20
	lda     #$C8
	sta     sreg
	lda     #$03
	sta     sreg+1
	lda     #$80
	jsr     tossubeax
	ldy     #$02
	jsr     steaxysp
;
; if((entry = isInHash2(codepoint))) {
;
	ldy     #$05
	jsr     ldeaxysp
	jsr     _isInHash2
	ldy     #$08
	jsr     staxysp
	stx     tmp1
	ora     tmp1
	beq     L0403
;
; bytesread = 4;
;
	ldx     #$00
	lda     #$04
	ldy     #$06
	jsr     staxysp
;
; else {
;
	jmp     L040A
;
; *bytesMatched = 4;
;
L0403:	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$04
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; return codepoint;
;
	ldy     #$05
	jsr     ldeaxysp
	jmp     L0393
;
; get = (void (*)())&getUnicodeCharFast;
;
L040A:	lda     #<(_getUnicodeCharFast)
	ldx     #>(_getUnicodeCharFast)
	ldy     #$0A
	jsr     staxysp
;
; return normaliseAndGet(offset, str, plusBytes, bytesMatched, bytesread, entry);
;
	ldy     #$15
	jsr     pushwysp
	ldy     #$15
	jsr     pushwysp
	ldy     #$15
	jsr     pushwysp
	ldy     #$15
	jsr     pushwysp
	ldy     #$11
	jsr     pushwysp
	ldy     #$13
	jsr     ldaxysp
	jsr     _normaliseAndGet
	jsr     axlong
;
; }
;
L0393:	ldy     #$14
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; void __near__ outputHeader (__near__ struct qryData *)
; ---------------------------------------------------------------


.endproc
