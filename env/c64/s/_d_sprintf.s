.include "../code2.s"
.export _d_sprintf
.proc	_d_sprintf: near


;
; int d_sprintf(char **str, char *format, ...) {
;
	jsr     enter
;
; char *newStr = NULL;
;
	jsr     decsp2
	jsr     push0
;
; if(str == NULL || format == NULL) {
;
	jsr     decsp2
	ldy     #$06
	lda     (sp),y
	jsr     leaa0sp
	jsr     incax5
	jsr     ldaxi
	cpx     #$00
	bne     L1775
	cmp     #$00
	jeq     L13D4
L1775:	ldy     #$06
	lda     (sp),y
	jsr     leaa0sp
	jsr     incax3
	jsr     ldaxi
	cpx     #$00
	bne     L13D6
	cmp     #$00
	jeq     L13D4
;
; va_start(args, format);
;
L13D6:	ldy     #$06
	lda     (sp),y
	jsr     leaa0sp
	jsr     incax3
	jsr     stax0sp
;
; newSize = (size_t)(vsnprintf(NULL, 0, format, args)+1); /* plus '\0' */
;
	jsr     push0
	jsr     push0
	ldy     #$0A
	lda     (sp),y
	jsr     leaa0sp
	jsr     incax7
	jsr     pushw
	ldy     #$07
	jsr     ldaxysp
	jsr     _vsnprintf
	jsr     incax1
	ldy     #$04
	jsr     staxysp
;
; if((newStr = (char*)malloc(newSize)) == NULL) {
;
	jsr     _malloc
	ldy     #$02
	jsr     staxysp
	cpx     #$00
	bne     L13E7
	cmp     #$00
;
; return FALSE;
;
	beq     L13D4
;
; va_start(args, format);
;
L13E7:	ldy     #$06
	lda     (sp),y
	jsr     leaa0sp
	jsr     incax3
	jsr     stax0sp
;
; vsprintf(newStr, format, args);
;
	ldy     #$05
	jsr     pushwysp
	ldy     #$08
	lda     (sp),y
	jsr     leaa0sp
	jsr     incax5
	jsr     pushw
	ldy     #$05
	jsr     ldaxysp
	jsr     _vsprintf
;
; newStr[newSize] = '\0';
;
	ldy     #$05
	jsr     ldaxysp
	clc
	ldy     #$02
	adc     (sp),y
	sta     ptr1
	txa
	iny
	adc     (sp),y
	sta     ptr1+1
	lda     #$00
	tay
	sta     (ptr1),y
;
; free(*str);
;
	ldy     #$06
	lda     (sp),y
	jsr     leaa0sp
	jsr     incax5
	jsr     ldaxi
	jsr     ldaxi
	jsr     _free
;
; *str = newStr;
;
	ldy     #$06
	lda     (sp),y
	jsr     leaa0sp
	jsr     incax5
	jsr     pushw
	ldy     #$05
	jsr     ldaxysp
	ldy     #$00
	jsr     staxspidx
;
; return newSize;
;
	ldy     #$05
	jsr     ldaxysp
;
; }
;
L13D4:	ldy     #$06
	jsr     leavey
	jmp     farret


; ---------------------------------------------------------------
; int __near__ hash_addString (__near__ struct columnReferenceHash *, __near__ unsigned char *, __near__ struct columnReference *)
; ---------------------------------------------------------------


.endproc
