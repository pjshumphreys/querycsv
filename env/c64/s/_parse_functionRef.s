.include "../code2.s"
.export _parse_functionRef
.proc	_parse_functionRef: near


;
; ) {
;
	jsr     pushax
;
; struct expression *expressionPtr2 = NULL;
;
	jsr     push0
;
; struct resultColumn *columnPtr = NULL;
;
	jsr     push0
;
; if(queryData->parseMode != 1) {
;
	ldy     #$0D
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L15F3
	cmp     #$01
	beq     L0929
;
; return NULL;
;
L15F3:	ldx     #$00
	txa
	jmp     L0926
;
; if(isDistinct) {
;
L0929:	ldy     #$05
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L092D
;
; aggregationType+=GRP_STAR;
;
	stx     sreg
	stx     sreg+1
	lda     #$07
	ldy     #$08
	jsr     laddeqysp
;
; if(expressionPtr->containsAggregates) {
;
L092D:	ldy     #$07
	jsr     ldaxysp
	ldy     #$06
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L0931
;
; fputs("can't aggregate an aggregate", stderr);
;
	lda     #<(l0934)
	ldx     #>(l0934)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; exit(EXIT_FAILURE);
;
	ldx     #$00
	lda     #$01
	jsr     _exit
;
; queryData->hasGrouping = TRUE;  /* at least one, perhaps more */
;
L0931:	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$02
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; columnPtr = parse_expCommaList(queryData, expressionPtr, NULL, aggregationType);   /* parse_exp_commalist returns an output column pointer */
;
	ldy     #$0F
	jsr     pushwysp
	ldy     #$0B
	jsr     pushwysp
	jsr     push0
	ldy     #$0F
	jsr     ldaxysp
	jsr     _parse_expCommaList
	jsr     stax0sp
;
; reallocMsg((void**)(&expressionPtr2), sizeof(struct expression));
;
	lda     #$02
	jsr     leaa0sp
	jsr     pushax
	ldx     #$00
	lda     #$12
	jsr     _reallocMsg
;
; expressionPtr2->type = EXP_GROUP;
;
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$16
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; expressionPtr2->value = NULL;
;
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; expressionPtr2->unionPtrs.voidPtr = (void *)(columnPtr);  /* the expression nodes reference points directly to the hidden column */
;
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	ldy     #$0E
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; expressionPtr2->minColumn = expressionPtr->minColumn;
;
	ldy     #$03
	jsr     ldaxysp
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
; expressionPtr2->minTable = expressionPtr->minTable;
;
	ldy     #$03
	jsr     ldaxysp
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
; expressionPtr2->containsAggregates = TRUE;
;
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$06
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; return expressionPtr2;
;
	ldy     #$03
	jsr     ldaxysp
;
; }
;
L0926:	ldy     #$0E
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; __near__ struct resultColumn * __near__ parse_newOutputColumn (__near__ struct qryData *, int, int, __near__ unsigned char *, int)
; ---------------------------------------------------------------


.endproc
