.include "../code2.s"
.export _cleanup_inputTables
.proc	_cleanup_inputTables: near


;
; void cleanup_inputTables(struct inputTable *currentInputTable) {
;
	jsr     pushax
;
; while(currentInputTable != NULL) {
;
	jsr     decsp2
	jmp     L0E1B
;
; next = currentInputTable->nextInputTable;
;
L166D:	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	jsr     stax0sp
;
; free(currentInputTable->queryTableName);
;
	ldy     #$03
	jsr     ldaxysp
	ldy     #$0D
	jsr     ldaxidx
	jsr     _free
;
; fclose(currentInputTable->fileStream);
;
	ldy     #$03
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	jsr     _fclose
;
; free(currentInputTable);
;
	ldy     #$03
	jsr     ldaxysp
	jsr     _free
;
; currentInputTable = next;
;
	jsr     ldax0sp
	ldy     #$02
	jsr     staxysp
;
; while(currentInputTable != NULL) {
;
L0E1B:	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L166D
;
; }
;
	jsr     incsp4
	jmp     farret


; ---------------------------------------------------------------
; void __near__ outputResult (__near__ struct qryData *, __near__ struct resultColumnValue *, int)
; ---------------------------------------------------------------


.endproc
