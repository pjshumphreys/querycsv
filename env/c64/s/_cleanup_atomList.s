.include "../code2.s"
.export _cleanup_atomList
.proc	_cleanup_atomList: near


;
; void cleanup_atomList(struct atomEntry *currentAtom) {
;
	jsr     pushax
;
; while(currentAtom != NULL) {
;
	jsr     decsp2
	jmp     L0632
;
; next = currentAtom->nextInList;
;
L1595:	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	jsr     stax0sp
;
; free(currentAtom->content);
;
	ldy     #$03
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jsr     _free
;
; free(currentAtom);
;
	ldy     #$03
	jsr     ldaxysp
	jsr     _free
;
; currentAtom = next;
;
	jsr     ldax0sp
	ldy     #$02
	jsr     staxysp
;
; while(currentAtom != NULL) {
;
L0632:	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L1595
;
; }
;
	jsr     incsp4
	jmp     farret


; ---------------------------------------------------------------
; void __near__ exp_uminus (__near__ __near__ unsigned char * *, unsigned long)
; ---------------------------------------------------------------


.endproc
