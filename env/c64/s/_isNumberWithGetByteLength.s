.include "../code2.s"
.export _isNumberWithGetByteLength
.proc	_isNumberWithGetByteLength: near


;
; ) {
;
	jsr     pushax
;
; int decimalNotFound = TRUE;
;
	lda     #$01
	jsr     pusha0
;
; unsigned char *string = offset;
;
	ldy     #$09
	jsr     pushwysp
;
; (*offset >= '0' && *offset <= '9') ||
;
	ldy     #$09
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	ldx     #$00
	bcc     L171B
	ldy     #$09
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$3A
	jcc     L1760
	ldx     #$00
;
; (firstChar && (
;
L171B:	ldy     #$05
	lda     (sp),y
	dey
	ora     (sp),y
	jeq     L13A0
;
; (*offset == '.' && (*(offset+1) >= '0' && *(offset+1) <= '9')) ||
;
	ldy     #$09
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$2E
	bne     L171C
	ldy     #$09
	jsr     ldaxysp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	bcc     L171C
	ldy     #$09
	jsr     ldaxysp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$3A
	jcc     L1760
;
; (*offset == '-' && ((*(offset+1) >= '0' && *(offset+1) <= '9') || (*(offset+1) == '.' && (*(offset+2) >= '0' && *(offset+2) <= '9')))) ||
;
L171C:	ldy     #$09
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$2D
	bne     L171F
	ldy     #$09
	jsr     ldaxysp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	bcc     L171D
	ldy     #$09
	jsr     ldaxysp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$3A
	jcc     L1760
L171D:	ldy     #$09
	jsr     ldaxysp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$2E
	bne     L171F
	ldy     #$09
	jsr     ldaxysp
	jsr     incax2
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	bcc     L171F
	ldy     #$09
	jsr     ldaxysp
	jsr     incax2
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$3A
	jcc     L1760
;
; (*offset == '+' && ((*(offset+1) >= '0' && *(offset+1) <= '9') || (*(offset+1) == '.' && (*(offset+2) >= '0' && *(offset+2) <= '9'))))
;
L171F:	ldy     #$09
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	cmp     #$2B
	bne     L1751
	ldy     #$09
	jsr     ldaxysp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	bcc     L1720
	ldy     #$09
	jsr     ldaxysp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$3A
	bcc     L1760
L1720:	ldy     #$09
	jsr     ldaxysp
	jsr     incax1
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	cmp     #$2E
	bne     L1749
	ldy     #$09
	jsr     ldaxysp
	jsr     incax2
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	tya
	tax
	jcc     L13A0
	ldy     #$09
	jsr     ldaxysp
	jsr     incax2
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$3A
	bcc     L1760
	ldx     #$00
	txa
	jmp     L13A0
L1749:	txa
	jmp     L13A0
L1751:	txa
	jmp     L13A0
;
; if(*string == '-' || *string == '+') {
;
L1760:	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$2D
	beq     L138B
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$2B
	bne     L138D
;
; string++;
;
L138B:	jsr     ldax0sp
	jsr     incax1
	jsr     stax0sp
;
; (*string >= '0' && *string <= '9') ||
;
L138D:	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	bcc     L1724
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$3A
	bcc     L138B
;
; (decimalNotFound && (*string == '.' || *string == ',') &&
;
L1724:	ldy     #$03
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L1726
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$2E
	beq     L1765
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$2C
	bne     L1726
;
; !(decimalNotFound = FALSE))
;
L1765:	ldx     #$00
	txa
	ldy     #$02
	jsr     staxysp
	jmp     L138B
;
; *(lastMatchedBytes)+=(string-offset)-1;
;
L1726:	ldy     #$07
	jsr     ldaxysp
	jsr     pushax
	jsr     ldaxi
	sta     ptr1
	stx     ptr1+1
	ldy     #$03
	jsr     ldaxysp
	sec
	ldy     #$0A
	sbc     (sp),y
	pha
	txa
	iny
	sbc     (sp),y
	tax
	pla
	jsr     decax1
	clc
	adc     ptr1
	pha
	txa
	adc     ptr1+1
	tax
	pla
	ldy     #$00
	jsr     staxspidx
;
; return TRUE;
;
	ldx     #$00
	lda     #$01
;
; }
;
L13A0:	ldy     #$0A
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; void __near__ cleanup_resultColumns (__near__ struct resultColumn *)
; ---------------------------------------------------------------


.endproc
