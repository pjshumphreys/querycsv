.include "../code2.s"
.export _cleanup_resultColumns
.proc	_cleanup_resultColumns: near


;
; void cleanup_resultColumns(struct resultColumn *currentResultColumn) {
;
	jsr     pushax
;
; while(currentResultColumn != NULL) {
;
	jsr     decsp2
	jmp     L13AF
;
; next = currentResultColumn->nextColumnInResults;
;
L1772:	jsr     ldaxysp
	ldy     #$17
	jsr     ldaxidx
	jsr     stax0sp
;
; free(currentResultColumn->resultColumnName);
;
	ldy     #$03
	jsr     ldaxysp
	ldy     #$13
	jsr     ldaxidx
	jsr     _free
;
; free(currentResultColumn);
;
	ldy     #$03
	jsr     ldaxysp
	jsr     _free
;
; currentResultColumn = next;
;
	jsr     ldax0sp
	ldy     #$02
	jsr     staxysp
;
; while(currentResultColumn != NULL) {
;
L13AF:	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L1772
;
; }
;
	jsr     incsp4
	jmp     farret


; ---------------------------------------------------------------
; int __near__ strAppend (unsigned char, __near__ __near__ unsigned char * *, __near__ unsigned int *)
; ---------------------------------------------------------------


.endproc
