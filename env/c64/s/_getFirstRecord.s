.include "../code2.s"
.export _getFirstRecord
.proc	_getFirstRecord: near


;
; ) {
;
	jsr     pushax
;
; if(root == NULL) {
;
	jsr     decsp2
	ldy     #$04
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L170A
;
; return NULL;
;
	tax
	jmp     L1294
;
; currentResult = root;
;
L170A:	jsr     ldaxysp
;
; while(currentResult->left) {
;
	jmp     L170C
;
; currentResult = currentResult->left;
;
L170B:	jsr     ldaxysp
	jsr     ldaxi
L170C:	jsr     stax0sp
;
; while(currentResult->left) {
;
	ldy     #$00
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	bne     L170B
;
; return currentResult->columns;
;
	jsr     ldax0sp
	ldy     #$07
	jsr     ldaxidx
;
; }
;
L1294:	jsr     incsp6
	jmp     farret


; ---------------------------------------------------------------
; void __near__ groupResultsInner (__near__ struct qryData *, __near__ struct resultColumnValue *, int)
; ---------------------------------------------------------------


.endproc
