.include "../code2.s"
.export _reallocMsg
.proc	_reallocMsg: near


;
; void reallocMsg(void **mem, size_t size) {
;
	jsr     pushax
;
; void *temp = NULL;
;
	jsr     push0
;
; if((temp = realloc(*mem, size)) == NULL) {
;
	ldy     #$05
	jsr     ldaxysp
	jsr     pushw
	ldy     #$05
	jsr     ldaxysp
	jsr     _realloc
	jsr     stax0sp
	cpx     #$00
	bne     L0CC4
	cmp     #$00
	bne     L0CC4
;
; fputs(TDB_MALLOC_FAILED, stderr);
;
	lda     #<(l0ccc)
	ldx     #>(l0ccc)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; exit(EXIT_FAILURE);
;
	ldx     #$00
	lda     #$01
	jsr     _exit
;
; *mem = temp;
;
L0CC4:	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	ldy     #$00
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; }
;
	jsr     incsp6
	jmp     farret


; ---------------------------------------------------------------
; int __near__ consumeCombiningChars (__near__ __near__ unsigned char * *, __near__ __near__ unsigned char * *, __near__ __near__ unsigned char * *, __near__ __near__ unsigned char * *, __near__ __near__ function returning void *, __near__ __near__ function returning void *, __near__ int *, __near__ int *, __near__ int *)
; ---------------------------------------------------------------


.endproc
