.include "../code2.s"
.export _cleanup_inputColumns
.proc	_cleanup_inputColumns: near


;
; void cleanup_inputColumns(struct inputColumn *currentInputColumn) {
;
	jsr     pushax
;
; while(currentInputColumn != NULL) {
;
	jsr     decsp2
	jmp     L0626
;
; next = currentInputColumn->nextColumnInTable;
;
L1594:	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	jsr     stax0sp
;
; free(currentInputColumn);
;
	ldy     #$03
	jsr     ldaxysp
	jsr     _free
;
; currentInputColumn = next;
;
	jsr     ldax0sp
	ldy     #$02
	jsr     staxysp
;
; while(currentInputColumn != NULL) {
;
L0626:	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L1594
;
; }
;
	jsr     incsp4
	jmp     farret


; ---------------------------------------------------------------
; void __near__ cleanup_atomList (__near__ struct atomEntry *)
; ---------------------------------------------------------------


.endproc
