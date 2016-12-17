.include "../code2.s"
.export _strFree
.proc	_strFree: near


;
; void strFree(char **str) {
;
	jsr     pushax
;
; free(*str);
;
	jsr     ldax0sp
	jsr     ldaxi
	jsr     _free
;
; *str = NULL;
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
; }
;
	jsr     incsp2
	jmp     farret


; ---------------------------------------------------------------
; void __near__ getGroupedColumns (__near__ struct qryData *)
; ---------------------------------------------------------------


.endproc
