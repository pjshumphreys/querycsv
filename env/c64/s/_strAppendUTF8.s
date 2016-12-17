.include "../code2.s"
.export _strAppendUTF8
.proc	_strAppendUTF8: near


;
; int strAppendUTF8(long codepoint, unsigned char **nfdString, int nfdLength) {
;
	jsr     pushax
;
; if (codepoint < 0x80) {
;
	ldy     #$07
	jsr     ldeaxysp
	jsr     pusheax
	ldx     #$00
	stx     sreg
	stx     sreg+1
	lda     #$80
	jsr     toslteax
	beq     L0EF3
;
; reallocMsg((void**)nfdString, nfdLength+1);
;
	ldy     #$05
	jsr     pushwysp
	ldy     #$03
	jsr     ldaxysp
	jsr     incax1
	jsr     _reallocMsg
;
; (*nfdString)[nfdLength++] = codepoint;
;
	ldy     #$03
	jsr     ldaxysp
	jsr     pushw
	ldy     #$03
	jsr     ldaxysp
	sta     regsave
	stx     regsave+1
	jsr     incax1
	ldy     #$02
	jsr     staxysp
	lda     regsave
	ldx     regsave+1
	jsr     tosaddax
	sta     ptr1
	stx     ptr1+1
	ldy     #$04
	lda     (sp),y
	ldy     #$00
	sta     (ptr1),y
;
; else if (codepoint < 0x800) {
;
	jmp     L0F23
L0EF3:	ldy     #$07
	jsr     ldeaxysp
	jsr     pusheax
	ldx     #$08
	lda     #$00
	sta     sreg
	sta     sreg+1
	jsr     toslteax
	beq     L0EFD
;
; reallocMsg((void**)nfdString, nfdLength+2);
;
	ldy     #$05
	jsr     pushwysp
	ldy     #$03
	jsr     ldaxysp
	jsr     incax2
	jsr     _reallocMsg
;
; (*nfdString)[nfdLength++] = (codepoint >> 6) + 0xC0;
;
	ldy     #$03
	jsr     ldaxysp
	jsr     pushw
	ldy     #$03
	jsr     ldaxysp
	sta     regsave
	stx     regsave+1
	jsr     incax1
	ldy     #$02
	jsr     staxysp
	lda     regsave
	ldx     regsave+1
	jsr     tosaddax
	jsr     pushax
	ldy     #$09
	jsr     ldeaxysp
	jsr     asreax4
	jsr     asreax2
	ldy     #$C0
;
; else if (codepoint < 0x10000) {
;
	jmp     L1696
L0EFD:	ldy     #$07
	jsr     ldeaxysp
	jsr     pusheax
	ldx     #$00
	stx     sreg+1
	lda     #$01
	sta     sreg
	txa
	jsr     toslteax
	beq     L0F0D
;
; reallocMsg((void**)nfdString, nfdLength+3);
;
	ldy     #$05
	jsr     pushwysp
	ldy     #$03
	jsr     ldaxysp
	jsr     incax3
	jsr     _reallocMsg
;
; (*nfdString)[nfdLength++] = (codepoint >> 12) + 0xE0;
;
	ldy     #$03
	jsr     ldaxysp
	jsr     pushw
	ldy     #$03
	jsr     ldaxysp
	sta     regsave
	stx     regsave+1
	jsr     incax1
	ldy     #$02
	jsr     staxysp
	lda     regsave
	ldx     regsave+1
	jsr     tosaddax
	jsr     pushax
	ldy     #$09
	jsr     ldeaxysp
	txa
	ldx     sreg
	ldy     sreg+1
	sty     sreg
	cpy     #$80
	ldy     #$00
	bcc     L0F17
	dey
L0F17:	sty     sreg+1
	jsr     asreax4
	ldy     #$E0
;
; else {
;
	jmp     L16B2
;
; reallocMsg((void**)nfdString, nfdLength+4);
;
L0F0D:	ldy     #$05
	jsr     pushwysp
	ldy     #$03
	jsr     ldaxysp
	jsr     incax4
	jsr     _reallocMsg
;
; (*nfdString)[nfdLength++] = (codepoint >> 18) + 0xF0;
;
	ldy     #$03
	jsr     ldaxysp
	jsr     pushw
	ldy     #$03
	jsr     ldaxysp
	sta     regsave
	stx     regsave+1
	jsr     incax1
	ldy     #$02
	jsr     staxysp
	lda     regsave
	ldx     regsave+1
	jsr     tosaddax
	jsr     pushax
	ldy     #$09
	jsr     ldeaxysp
	ldy     #$00
	ldx     sreg+1
	bpl     L0F2C
	dey
L0F2C:	lda     sreg
	sty     sreg+1
	sty     sreg
	jsr     asreax2
	ldy     #$F0
	jsr     inceaxy
	ldy     #$00
	jsr     staspidx
;
; (*nfdString)[nfdLength++] = ((codepoint >> 12) & 0x3F) + 0x80;
;
	ldy     #$03
	jsr     ldaxysp
	jsr     pushw
	ldy     #$03
	jsr     ldaxysp
	sta     regsave
	stx     regsave+1
	jsr     incax1
	ldy     #$02
	jsr     staxysp
	lda     regsave
	ldx     regsave+1
	jsr     tosaddax
	jsr     pushax
	ldy     #$09
	jsr     ldeaxysp
	txa
	ldx     sreg
	ldy     sreg+1
	sty     sreg
	cpy     #$80
	ldy     #$00
	bcc     L0F33
	dey
L0F33:	sty     sreg+1
	jsr     asreax4
	ldx     #$00
	stx     sreg+1
	stx     sreg
	and     #$3F
	ldy     #$80
L16B2:	jsr     inceaxy
	ldy     #$00
	jsr     staspidx
;
; (*nfdString)[nfdLength++] = ((codepoint >> 6) & 0x3F) + 0x80;
;
	ldy     #$03
	jsr     ldaxysp
	jsr     pushw
	ldy     #$03
	jsr     ldaxysp
	sta     regsave
	stx     regsave+1
	jsr     incax1
	ldy     #$02
	jsr     staxysp
	lda     regsave
	ldx     regsave+1
	jsr     tosaddax
	jsr     pushax
	ldy     #$09
	jsr     ldeaxysp
	jsr     asreax4
	jsr     asreax2
	ldx     #$00
	stx     sreg+1
	stx     sreg
	and     #$3F
	ldy     #$80
L1696:	jsr     inceaxy
	ldy     #$00
	jsr     staspidx
;
; (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
;
	ldy     #$03
	jsr     ldaxysp
	jsr     pushw
	ldy     #$03
	jsr     ldaxysp
	sta     regsave
	stx     regsave+1
	jsr     incax1
	ldy     #$02
	jsr     staxysp
	lda     regsave
	ldx     regsave+1
	jsr     tosaddax
	jsr     pushax
	ldy     #$09
	jsr     ldeaxysp
	ldx     #$00
	stx     sreg+1
	stx     sreg
	and     #$3F
	ldy     #$80
	jsr     inceaxy
	ldy     #$00
	jsr     staspidx
;
; return nfdLength;
;
L0F23:	jsr     ldax0sp
;
; }
;
	jsr     incsp8
	jmp     farret


; ---------------------------------------------------------------
; int __near__ getCurrentDate (void)
; ---------------------------------------------------------------


.endproc
