.include "../code2.s"
.export _d_strftime
.proc	_d_strftime: near


;
; int d_strftime(char **ptr, char *format, struct tm *timeptr) {
;
	jsr     pushax
;
; size_t length = 32; /* starting value */
;
	lda     #$20
	jsr     pusha0
;
; size_t length2 = 0;
;
	jsr     push0
;
; char *output = NULL;
;
	jsr     push0
;
; char *output2 = NULL;
;
	jsr     push0
;
; if(ptr == NULL || format == NULL) {
;
	ldy     #$0C
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L097E
	ldy     #$0A
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L097E
	jmp     L098B
;
; output2 = realloc((void*)output, length*sizeof(char));
;
L0989:	ldy     #$05
	jsr     pushwysp
	ldy     #$09
	jsr     ldaxysp
	jsr     _realloc
	jsr     stax0sp
;
; if(output2 == NULL) {
;
	cpx     #$00
	bne     L0992
	cmp     #$00
	bne     L0992
;
; freeAndZero(output);
;
	ldy     #$03
	jsr     ldaxysp
	jsr     _free
	ldx     #$00
	txa
	ldy     #$02
	jsr     staxysp
;
; return FALSE;
;
	jmp     L097E
;
; output = output2;
;
L0992:	jsr     ldax0sp
	ldy     #$02
	jsr     staxysp
;
; length2 = strftime(output, length, format, timeptr);
;
	jsr     pushax
	ldy     #$0B
	jsr     pushwysp
	ldy     #$11
	jsr     pushwysp
	ldy     #$0F
	jsr     ldaxysp
	jsr     _strftime
	ldy     #$04
	jsr     staxysp
;
; length *= 2;
;
	ldy     #$07
	jsr     ldaxysp
	jsr     shlax1
	ldy     #$06
	jsr     staxysp
;
; while (length2 == 0) {
;
L098B:	ldy     #$04
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0989
;
; output2 = realloc((void*)output, (length2+1)*sizeof(char));
;
	jsr     pushwysp
	ldy     #$07
	jsr     ldaxysp
	jsr     incax1
	jsr     _realloc
	jsr     stax0sp
;
; if(output2 == NULL) {
;
	cpx     #$00
	bne     L09A9
	cmp     #$00
	bne     L09A9
;
; freeAndZero(output);
;
	ldy     #$03
	jsr     ldaxysp
	jsr     _free
	ldx     #$00
	txa
	ldy     #$02
	jsr     staxysp
;
; return FALSE;
;
	jmp     L097E
;
; freeAndZero(*ptr);
;
L09A9:	ldy     #$0D
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _free
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	tay
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; *ptr = output2;
;
	ldy     #$0D
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
; return TRUE;
;
	ldx     #$00
	tya
;
; }
;
L097E:	ldy     #$0E
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ getColumnValue (__near__ unsigned char *, long, int)
; ---------------------------------------------------------------


.endproc
