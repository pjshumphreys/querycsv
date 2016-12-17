.include "../code2.s"
.export _strRTrim
.proc	_strRTrim: near


;
; int strRTrim(char **value, size_t *strSize, char *minSize) {
;
	jsr     pushax
;
; if(value == NULL || strSize == NULL || *value == NULL) {
;
	jsr     decsp6
	ldy     #$0A
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0A4F
	ldy     #$08
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0A4F
	ldy     #$0B
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L0A4E
	cmp     #$00
	jeq     L0A4D
	jmp     L0A4E
;
; return FALSE;
;
L0A4F:	tax
	jmp     L0A4D
;
; str = *value;
;
L0A4E:	ldy     #$0B
	jsr     ldaxysp
	jsr     ldaxi
	ldy     #$02
	jsr     staxysp
;
; size = *strSize;
;
	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	jsr     stax0sp
;
; end = str + size - 1;
;
	clc
	ldy     #$02
	adc     (sp),y
	pha
	txa
	iny
	adc     (sp),y
	tax
	pla
	jsr     decax1
	iny
;
; while(end > str && end != minSize && (*end == ' ' || *end == '\t')) {
;
	jmp     L1616
;
; end--;
;
L0A5B:	ldy     #$05
	jsr     ldaxysp
	jsr     decax1
	ldy     #$04
	jsr     staxysp
;
; size--;
;
	jsr     ldax0sp
	jsr     decax1
	ldy     #$00
L1616:	jsr     staxysp
;
; while(end > str && end != minSize && (*end == ' ' || *end == '\t')) {
;
	ldy     #$05
	jsr     ldaxysp
	sec
	ldy     #$02
	sbc     (sp),y
	sta     tmp1
	txa
	iny
	sbc     (sp),y
	ora     tmp1
	bcc     L160E
	beq     L160E
	ldy     #$05
	jsr     ldaxysp
	ldy     #$06
	cmp     (sp),y
	bne     L1615
	txa
	iny
	cmp     (sp),y
	beq     L160E
L1615:	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$20
	beq     L0A5B
	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$09
	beq     L0A5B
;
; *strSize = size;
;
L160E:	ldy     #$09
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	ldy     #$00
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; return TRUE;
;
	ldx     #$00
	tya
;
; }
;
L0A4D:	ldy     #$0C
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ walkRejectRecord (int, __near__ struct expression *, __near__ struct resultColumnParam *)
; ---------------------------------------------------------------


.endproc
