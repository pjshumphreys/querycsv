.include "../code2.s"
.export _stringGet
.proc	_stringGet: near


;
; ) {
;
	jsr     pushax
;
; if(str != NULL) {
;
	ldy     #$04
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0E10
;
; *str = (unsigned char *)(strdup((char const *)(field->value)));
;
	ldy     #$07
	jsr     pushwysp
	ldy     #$05
	jsr     ldaxysp
	ldy     #$0D
	jsr     ldaxidx
	jsr     _strdup
	ldy     #$00
	jsr     staxspidx
;
; }
;
L0E10:	jsr     incsp6
	jmp     farret


; ---------------------------------------------------------------
; void __near__ cleanup_inputTables (__near__ struct inputTable *)
; ---------------------------------------------------------------


.endproc
