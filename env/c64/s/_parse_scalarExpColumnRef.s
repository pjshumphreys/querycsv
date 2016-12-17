.include "../code2.s"
.export _parse_scalarExpColumnRef
.proc	_parse_scalarExpColumnRef: near


;
; ) {
;
	jsr     pushax
;
; struct expression *expressionPtr = NULL, *expressionColumnPtr;
;
	jsr     push0
;
; struct inputColumn *columnPtr = NULL;
;
	jsr     decsp2
	jsr     push0
;
; if(queryData->parseMode != 1) {
;
	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L14C1
	cmp     #$01
	beq     L0148
;
; return NULL;
;
L14C1:	ldx     #$00
	txa
	jmp     L0145
;
; reallocMsg((void**)(&expressionPtr), sizeof(struct expression));
;
L0148:	lda     #$04
	jsr     leaa0sp
	jsr     pushax
	ldx     #$00
	lda     #$12
	jsr     _reallocMsg
;
; expressionPtr->value = NULL;
;
	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; if(referencePtr->referenceType == REF_COLUMN) {
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     #$00
	jne     L0152
	cmp     #$01
	bne     L0152
;
; expressionPtr->type = EXP_COLUMN;
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
; columnPtr = referencePtr->reference.columnPtr;
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	jsr     stax0sp
;
; expressionPtr->unionPtrs.voidPtr = (void *)(columnPtr);
;
	ldy     #$05
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
; expressionPtr->minColumn = columnPtr->columnIndex;
;
	ldy     #$05
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	jsr     ldax0sp
	jsr     ldaxi
	ldy     #$04
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; expressionPtr->minTable = ((struct inputTable*)columnPtr->inputTablePtr)->fileIndex;
;
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	jsr     ldax0sp
	ldy     #$05
	jsr     ldaxidx
	jsr     ldaxi
	ldy     #$02
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; expressionPtr->containsAggregates = FALSE;
;
	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$06
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; else {
;
	jmp     L14C0
;
; expressionPtr->type = EXP_CALCULATED;
;
L0152:	ldy     #$05
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
; expressionColumnPtr = referencePtr->reference.calculatedPtr.expressionPtr;
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$02
	jsr     staxysp
;
; expressionPtr->unionPtrs.voidPtr = (void *)(expressionColumnPtr);
;
	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$03
	jsr     ldaxysp
	ldy     #$0E
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; expressionPtr->minColumn = expressionColumnPtr->minColumn;
;
	ldy     #$05
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$03
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$04
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; expressionPtr->minTable = expressionColumnPtr->minTable;
;
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$03
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	ldy     #$02
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; expressionPtr->containsAggregates = expressionColumnPtr->containsAggregates;
;
	ldy     #$05
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$03
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
L14C0:	ldy     #$05
	jsr     ldaxysp
;
; }
;
L0145:	ldy     #$0A
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ parse_columnRefUnsuccessful (__near__ struct qryData *, __near__ __near__ struct columnReference * *, __near__ unsigned char *, __near__ unsigned char *)
; ---------------------------------------------------------------


.endproc
