.include "../code2.s"
.export _parse_whereClause
.proc	_parse_whereClause: near


;
; ) {
;
	jsr     pushax
;
; if(queryData->parseMode != 1) {
;
	jsr     decsp4
	ldy     #$07
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	jne     L05E2
	cmp     #$01
;
; return;
;
	jne     L05E2
;
; if(queryData->joinsAndWhereClause == NULL) {
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$1F
	jsr     ldaxidx
	cpx     #$00
	bne     L05DD
	cmp     #$00
	bne     L05DD
;
; queryData->joinsAndWhereClause = expressionPtr;
;
	ldy     #$07
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$05
;
; else {
;
	jmp     L157C
;
; currentClauses = queryData->joinsAndWhereClause;
;
L05DD:	ldy     #$07
	jsr     ldaxysp
	ldy     #$1F
	jsr     ldaxidx
	ldy     #$02
	jsr     staxysp
;
; newClause = NULL;
;
	ldx     #$00
	txa
	jsr     stax0sp
;
; (void**)&(newClause),
;
	lda     sp
	ldx     sp+1
	jsr     pushax
;
; );
;
	ldx     #$00
	lda     #$12
	jsr     _reallocMsg
;
; newClause->type = EXP_AND;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	lda     #$0B
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; newClause->value = NULL;
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
; expressionPtr->minTable < currentClauses->minTable ||
;
	ldy     #$05
	jsr     ldaxysp
	ldy     #$03
	jsr     pushwidx
	ldy     #$05
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jsr     tosicmp
	bmi     L1571
;
; (expressionPtr->minTable == currentClauses->minTable &&
;
	ldy     #$05
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	sta     sreg
	stx     sreg+1
	ldy     #$03
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     sreg+1
	bne     L05EF
	cmp     sreg
	bne     L05EF
;
; expressionPtr->minColumn < currentClauses->minColumn)
;
	ldy     #$05
	jsr     ldaxysp
	ldy     #$05
	jsr     pushwidx
	ldy     #$05
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	jsr     tosicmp
	bpl     L05EF
;
; newClause->unionPtrs.leaves.leftPtr = expressionPtr;
;
L1571:	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$05
	jsr     ldaxysp
	ldy     #$0E
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newClause->unionPtrs.leaves.rightPtr = currentClauses;
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
; newClause->minColumn = currentClauses->minColumn;
;
	jsr     ldax0sp
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
; newClause->minTable = currentClauses->minTable;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$03
;
; else {
;
	jmp     L157F
;
; newClause->unionPtrs.leaves.leftPtr = currentClauses;
;
L05EF:	jsr     ldax0sp
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
; newClause->unionPtrs.leaves.rightPtr = expressionPtr;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$05
	jsr     ldaxysp
	ldy     #$10
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newClause->minColumn = expressionPtr->minColumn;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$05
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$04
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; newClause->minTable = expressionPtr->minTable;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$05
L157F:	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	ldy     #$02
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; queryData->joinsAndWhereClause = newClause;
;
	ldy     #$07
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$01
L157C:	jsr     ldaxysp
	ldy     #$1E
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; }
;
L05E2:	jsr     incsp8
	jmp     farret


; ---------------------------------------------------------------
; void __near__ parse_groupingSpec (__near__ struct qryData *, __near__ struct expression *)
; ---------------------------------------------------------------


.endproc
