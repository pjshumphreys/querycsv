.include "../code2.s"
.export _parse_newOutputColumn
.proc	_parse_newOutputColumn: near


;
; ) {
;
	jsr     pushax
;
; struct resultColumn *newResultColumn = NULL;
;
	jsr     push0
;
; reallocMsg((void**)(&newResultColumn), sizeof(struct resultColumn));
;
	lda     sp
	ldx     sp+1
	jsr     pushax
	ldx     #$00
	lda     #$18
	jsr     _reallocMsg
;
; if(queryData->firstResultColumn == NULL) {
;
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$1D
	jsr     ldaxidx
	cpx     #$00
	bne     L0959
	cmp     #$00
	bne     L0959
;
; newResultColumn->resultColumnIndex = 0;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	tay
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; queryData->firstResultColumn = newResultColumn;
;
	ldy     #$0B
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	ldy     #$1C
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newResultColumn->nextColumnInResults = newResultColumn;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	ldy     #$16
;
; else {
;
	jmp     L15F8
;
; newResultColumn->resultColumnIndex = queryData->firstResultColumn->resultColumnIndex+1;
;
L0959:	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$1D
	jsr     ldaxidx
	jsr     ldaxi
	jsr     incax1
	ldy     #$00
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; newResultColumn->nextColumnInResults = queryData->firstResultColumn->nextColumnInResults;   /* maintain circularly linked list for now */
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$1D
	jsr     ldaxidx
	ldy     #$17
	jsr     ldaxidx
	ldy     #$16
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; queryData->firstResultColumn->nextColumnInResults = newResultColumn;
;
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$1D
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	ldy     #$16
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; queryData->firstResultColumn = newResultColumn;
;
	ldy     #$0B
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	ldy     #$1C
L15F8:	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newResultColumn->isHidden = isHidden2;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$09
	jsr     ldaxysp
	ldy     #$02
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newResultColumn->isCalculated = isCalculated2;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$04
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newResultColumn->resultColumnName = resultColumnName2;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$05
	jsr     ldaxysp
	ldy     #$12
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newResultColumn->nextColumnInstance = NULL;   /* TODO: this field needs to be filled out properly */
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$14
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; newResultColumn->groupType = aggregationType;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$03
	jsr     ldaxysp
	ldy     #$06
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newResultColumn->groupText = NULL;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	lda     #$00
	ldy     #$08
	sta     (sreg),y
	iny
	sta     (sreg),y
;
; newResultColumn->groupNum = 0.0;
;
	jsr     pushw0sp
	ldx     #$00
	stx     sreg
	stx     sreg+1
	txa
	ldy     #$0A
	jsr     steaxspidx
;
; newResultColumn->groupCount = 0;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0E
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; newResultColumn->groupingDone = FALSE;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$10
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; return newResultColumn;
;
	jsr     ldax0sp
;
; }
;
	ldy     #$0C
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ d_strftime (__near__ __near__ unsigned char * *, __near__ unsigned char *, __near__ struct tm *)
; ---------------------------------------------------------------


.endproc
