.include "../code2.s"
.export _cleanup_orderByClause
.proc	_cleanup_orderByClause: near


;
; void cleanup_orderByClause(struct sortingList *currentSortingList) {
;
	jsr     pushax
;
; while(currentSortingList != NULL) {
;
	jsr     decsp2
	jmp     L1483
;
; next = currentSortingList->nextInList;
;
L1789:	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	jsr     stax0sp
;
; cleanup_expression(currentSortingList->expressionPtr);
;
	ldy     #$03
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _cleanup_expression
;
; free(currentSortingList);
;
	ldy     #$03
	jsr     ldaxysp
	jsr     _free
;
; currentSortingList = next;
;
	jsr     ldax0sp
	ldy     #$02
	jsr     staxysp
;
; while(currentSortingList != NULL) {
;
L1483:	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L1789
;
; }
;
	jsr     incsp4
	jmp     farret


; ---------------------------------------------------------------
; void __near__ cleanup_matchValues (__near__ struct qryData *, __near__ __near__ struct resultColumnValue * *)
; ---------------------------------------------------------------


.endproc
