.include "../code2.s"
.export _needsEscaping
.proc	_needsEscaping: near


;
; int needsEscaping(char *str, int params) {
;
	jsr     pushax
;
; str == NULL ||
;
	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L16B5
;
; strcmp(str, (((params & PRM_EXPORT) == 0)?"\\N":"\\N")) == 0 ||
;
	ldy     #$05
	jsr     pushwysp
	ldy     #$02
	lda     (sp),y
	and     #$08
	bne     L0F86
	lda     #<(l0f85)
	ldx     #>(l0f85)
	jmp     L0F89
L0F86:	lda     #<(l0f88)
	ldx     #>(l0f88)
L0F89:	jsr     _strcmp
	cpx     #$00
	bne     L16B7
	cmp     #$00
	jeq     L16B9
;
; *str == ' ' ||
;
L16B7:	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$20
	jeq     L16B5
;
; *str == '\t') {
;
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$09
	bne     L0F90
;
; return TRUE;
;
	jmp     L16B5
;
; if(*str == '"' || *str == '\n' || *str == ',') {
;
L0F8E:	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$22
	beq     L16B5
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$0D
	beq     L16B5
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$2C
;
; return TRUE;
;
	beq     L16B5
;
; str++;
;
	ldy     #$03
	jsr     ldaxysp
	jsr     incax1
	ldy     #$02
	jsr     staxysp
;
; while(*str) {
;
L0F90:	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	bne     L0F8E
;
; str--;
;
	ldy     #$03
	jsr     ldaxysp
	jsr     decax1
	ldy     #$02
	jsr     staxysp
;
; if(*str == ' ' || *str == '\t') {
;
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$20
	beq     L16B5
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$09
	beq     L16B5
	ldx     #$00
	txa
	jmp     L0F7B
;
; return TRUE;
;
L16B5:	ldx     #$00
L16B9:	lda     #$01
;
; }
;
L0F7B:	jsr     incsp4
	jmp     farret


; ---------------------------------------------------------------
; void __near__ updateRunningCounts (__near__ struct qryData *, __near__ struct resultColumnValue *)
; ---------------------------------------------------------------


.endproc
