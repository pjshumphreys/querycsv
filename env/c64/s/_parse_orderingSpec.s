.include "../code2.s"
.export _parse_orderingSpec
.proc	_parse_orderingSpec: near


;
; ) {
;
	jsr     pushax
;
; struct sortingList *sortingListPtr = NULL;
;
	jsr     push0
;
; if(queryData->parseMode != 1) {
;
	ldy     #$07
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	jne     L013E
	cmp     #$01
;
; return;
;
	jne     L013E
;
; (void**)&(sortingListPtr),
;
	lda     sp
	ldx     sp+1
	jsr     pushax
;
; );
;
	ldx     #$00
	lda     #$06
	jsr     _reallocMsg
;
; sortingListPtr->expressionPtr = expressionPtr;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$05
	jsr     ldaxysp
	ldy     #$00
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; sortingListPtr->isDescending = isDescending;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$03
	jsr     ldaxysp
	ldy     #$02
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; if(queryData->orderByClause == NULL) {
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$21
	jsr     ldaxidx
	cpx     #$00
	bne     L0137
	cmp     #$00
	bne     L0137
;
; sortingListPtr->nextInList = sortingListPtr;
;
	jsr     ldax0sp
;
; else {
;
	jmp     L14BF
;
; sortingListPtr->nextInList = queryData->orderByClause->nextInList;
;
L0137:	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$21
	jsr     ldaxidx
	ldy     #$05
	jsr     ldaxidx
	ldy     #$04
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; queryData->orderByClause->nextInList = sortingListPtr;
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$21
	jsr     ldaxidx
L14BF:	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	ldy     #$04
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; queryData->orderByClause = sortingListPtr;
;
	ldy     #$07
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	ldy     #$20
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; }
;
L013E:	jsr     incsp8
	jmp     farret


; ---------------------------------------------------------------
; __near__ struct expression * __near__ parse_scalarExpColumnRef (__near__ struct qryData *, __near__ struct columnReference *)
; ---------------------------------------------------------------


.endproc
