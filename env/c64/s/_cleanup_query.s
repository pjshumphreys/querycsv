.include "../code2.s"
.export _cleanup_query
.proc	_cleanup_query: near


;
; void cleanup_query(struct qryData *query) {
;
	jsr     pushax
;
; cleanup_columnReferences(query->columnReferenceHashTable);
;
	jsr     ldax0sp
	ldy     #$17
	jsr     ldaxidx
	jsr     _cleanup_columnReferences
;
; cleanup_resultColumns(query->firstResultColumn);
;
	jsr     ldax0sp
	ldy     #$1D
	jsr     ldaxidx
	jsr     _cleanup_resultColumns
;
; cleanup_orderByClause(query->groupByClause);
;
	jsr     ldax0sp
	ldy     #$23
	jsr     ldaxidx
	jsr     _cleanup_orderByClause
;
; cleanup_orderByClause(query->orderByClause);
;
	jsr     ldax0sp
	ldy     #$21
	jsr     ldaxidx
	jsr     _cleanup_orderByClause
;
; cleanup_expression(query->joinsAndWhereClause);
;
	jsr     ldax0sp
	ldy     #$1F
	jsr     ldaxidx
	jsr     _cleanup_expression
;
; cleanup_inputTables(query->firstInputTable);
;
	jsr     ldax0sp
	ldy     #$19
	jsr     ldaxidx
	jsr     _cleanup_inputTables
;
; free(query->intoFileName);
;
	jsr     ldax0sp
	ldy     #$11
	jsr     ldaxidx
	jsr     _free
;
; }
;
	jsr     incsp2
	jmp     farret


; ---------------------------------------------------------------
; int __near__ strRTrim (__near__ __near__ unsigned char * *, __near__ unsigned int *, __near__ unsigned char *)
; ---------------------------------------------------------------


.endproc
