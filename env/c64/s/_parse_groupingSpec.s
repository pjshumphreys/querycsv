.include "../code2.s"
.export _parse_groupingSpec
.proc	_parse_groupingSpec: near


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
	ldy     #$05
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	jne     L061C
	cmp     #$01
;
; return;
;
	jne     L061C
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
	ldy     #$03
	jsr     ldaxysp
	ldy     #$00
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; sortingListPtr->isDescending = FALSE;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$02
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; if(queryData->groupByClause == NULL) {
;
	ldy     #$05
	jsr     ldaxysp
	ldy     #$23
	jsr     ldaxidx
	cpx     #$00
	bne     L0615
	cmp     #$00
	bne     L0615
;
; sortingListPtr->nextInList = sortingListPtr;
;
	jsr     ldax0sp
;
; else {
;
	jmp     L1593
;
; sortingListPtr->nextInList = queryData->groupByClause->nextInList;
;
L0615:	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$05
	jsr     ldaxysp
	ldy     #$23
	jsr     ldaxidx
	ldy     #$05
	jsr     ldaxidx
	ldy     #$04
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; queryData->groupByClause->nextInList = sortingListPtr;
;
	jsr     ldaxysp
	ldy     #$23
	jsr     ldaxidx
L1593:	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	ldy     #$04
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; queryData->groupByClause = sortingListPtr;
;
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	ldy     #$22
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; }
;
L061C:	jsr     incsp6
	jmp     farret


; ---------------------------------------------------------------
; void __near__ cleanup_inputColumns (__near__ struct inputColumn *)
; ---------------------------------------------------------------


.endproc
