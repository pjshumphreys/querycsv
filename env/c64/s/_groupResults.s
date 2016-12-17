.include "../code2.s"
.export _groupResults
.proc	_groupResults: near


;
; void groupResults(struct qryData *query) {
;
	jsr     pushax
;
; resultsOrig = query->resultSet;
;
	jsr     decsp4
	ldy     #$05
	jsr     ldaxysp
	ldy     #$25
	jsr     ldaxidx
	ldy     #$02
	jsr     staxysp
;
; query->resultSet = NULL;
;
	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$24
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->match = getFirstRecord(
;
	ldy     #$07
	jsr     pushwysp
;
; resultsOrig,
;
	ldy     #$07
	jsr     pushwysp
;
; );
;
	ldy     #$09
	jsr     ldaxysp
	jsr     _getFirstRecord
	ldy     #$26
	jsr     staxspidx
;
; updateRunningCounts(query, query->match);
;
	ldy     #$07
	jsr     pushwysp
	ldy     #$07
	jsr     ldaxysp
	ldy     #$27
	jsr     ldaxidx
	jsr     _updateRunningCounts
;
; query,
;
	ldy     #$07
	jsr     pushwysp
;
; &(resultsOrig),
;
	lda     #$04
	jsr     leaa0sp
	jsr     pushax
;
; );
;
	lda     #<(_groupResultsInner)
	ldx     #>(_groupResultsInner)
	jsr     _tree_walkAndCleanup
;
; currentResultColumn = query->firstResultColumn;
;
	ldy     #$05
	jsr     ldaxysp
	ldy     #$1D
;
; while(currentResultColumn != NULL) {
;
	jmp     L1557
;
; currentResultColumn->groupingDone = TRUE;
;
L1555:	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$10
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; currentResultColumn = currentResultColumn->nextColumnInResults;
;
	jsr     ldax0sp
	ldy     #$17
L1557:	jsr     ldaxidx
	jsr     stax0sp
;
; while(currentResultColumn != NULL) {
;
	ldy     #$00
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L1555
;
; }
;
	jsr     incsp6
	jmp     farret


; ---------------------------------------------------------------
; int __near__ getCsvColumn (__near__ __near__ struct _FILE * *, __near__ __near__ unsigned char * *, __near__ unsigned int *, __near__ int *, __near__ long *, int)
; ---------------------------------------------------------------


.endproc
