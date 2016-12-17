.include "../code2.s"
.export _getUnicodeCharFast
.proc	_getUnicodeCharFast: near


;
; ) {
;
	jsr     pushax
;
; unsigned char *temp = (unsigned char *)(*(offset+plusBytes));
;
	ldy     #$05
	jsr     ldaxysp
	jsr     aslax1
	clc
	ldy     #$08
	adc     (sp),y
	pha
	txa
	iny
	adc     (sp),y
	tax
	pla
	jsr     pushw
;
; if(*temp < 0x80) {
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$80
	bcs     L12F6
;
; *bytesMatched = 1;
;
	ldy     #$05
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
	jmp     L12F2
;
; else if(*temp < 0xE0) {
;
L12F6:	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$E0
	bcs     L12FD
;
; *bytesMatched = 2;
;
	ldy     #$05
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
; return (long)((*(temp) << 6) + *(temp+1)) - 0x3080;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	jsr     aslax4
	jsr     aslax2
	jsr     pushax
	ldy     #$03
	jsr     ldaxysp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	jsr     tosadda0
	jsr     axulong
	jsr     pusheax
	ldx     #$30
	lda     #$00
	sta     sreg
	jmp     L1719
;
; else if (*temp < 0xF0) {
;
L12FD:	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$F0
	jcs     L1307
;
; *bytesMatched = 3;
;
	ldy     #$05
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
; return ((long)(*temp) << 12) + ((long)(*(temp+1)) << 6) + (long)(*(temp+2)) - 0xE2080;
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
	jsr     aulong
	jsr     axulong
	jsr     tosaddeax
	jsr     pusheax
	ldx     #$20
	lda     #$0E
	sta     sreg
	lda     #$00
	jmp     L1719
;
; *bytesMatched = 4;
;
L1307:	ldy     #$05
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
; return ((long)(*temp) << 18) + ((long)(*(temp+1)) << 12) + ((long)(*(temp+2)) << 6) + (*(temp+3)) - 0x3C82080;
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
L1719:	sta     sreg+1
	lda     #$80
	jsr     tossubeax
;
; }
;
L12F2:	ldy     #$0C
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; void __near__ runCommand (__near__ unsigned char *)
; ---------------------------------------------------------------


.endproc
