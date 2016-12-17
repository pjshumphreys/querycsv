.include "../code2.s"
.export _parse_inPredicate
.proc	_parse_inPredicate: near


;
; ) {
;
	jsr     pushax
;
; struct expression *expressionPtr = NULL;
;
	jsr     push0
;
; if(queryData->parseMode != 1) {
;
	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L15D9
	cmp     #$01
	beq     L06FC
;
; return NULL;
;
L15D9:	ldx     #$00
	txa
	jmp     L06FA
;
; reallocMsg((void**)(&expressionPtr), sizeof(struct expression));
;
L06FC:	lda     sp
	ldx     sp+1
	jsr     pushax
	ldx     #$00
	lda     #$12
	jsr     _reallocMsg
;
; expressionPtr->type = isNotIn==TRUE?EXP_NOTIN:EXP_IN;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$05
	lda     (sp),y
	bne     L0707
	dey
	lda     (sp),y
	cmp     #$01
	bne     L0707
	lda     #$15
	jmp     L0709
L0707:	lda     #$14
L0709:	ldy     #$00
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; expressionPtr->value = NULL;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; expressionPtr->unionPtrs.inLeaves.leftPtr = leftPtr;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$0E
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; expressionPtr->unionPtrs.inLeaves.lastEntryPtr = lastEntryPtr;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$03
	jsr     ldaxysp
	ldy     #$10
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; expressionPtr->minColumn = leftPtr->minColumn;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$04
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; expressionPtr->minTable = leftPtr->minTable;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	ldy     #$02
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; expressionPtr->containsAggregates = leftPtr->containsAggregates;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$07
	jsr     ldaxidx
	ldy     #$06
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; return expressionPtr;
;
	jsr     ldax0sp
;
; }
;
L06FA:	ldy     #$0A
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; void __near__ exp_divide (__near__ __near__ unsigned char * *, unsigned long, unsigned long)
; ---------------------------------------------------------------


.endproc
