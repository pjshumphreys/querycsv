.include "../code2.s"
.export _skipBom
.proc	_skipBom: near


;
; FILE *skipBom(const char *filename, long* offset) {
;
	jsr     pushax
;
; file = fopen(filename, "rb");
;
	jsr     decsp2
	ldy     #$07
	jsr     pushwysp
	lda     #<(l12d2)
	ldx     #>(l12d2)
	jsr     _fopen
	jsr     stax0sp
;
; if (file != NULL) {
;
	cpx     #$00
	bne     L1714
	cmp     #$00
	beq     L12D4
;
; if(fgetc(file) == 239 && fgetc(file) == 187 && fgetc(file) == 191) {
;
L1714:	jsr     ldax0sp
	jsr     _fgetc
	cpx     #$00
	bne     L12D7
	cmp     #$EF
	bne     L12D7
	jsr     ldax0sp
	jsr     _fgetc
	cpx     #$00
	bne     L12D7
	cmp     #$BB
	bne     L12D7
	jsr     ldax0sp
	jsr     _fgetc
	cpx     #$00
	bne     L12D7
	cmp     #$BF
	bne     L12D7
;
; if(offset) {
;
	ldy     #$03
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L12ED
;
; *offset = 3;
;
	ldy     #$05
	jsr     pushwysp
	ldx     #$00
	stx     sreg
	stx     sreg+1
	lda     #$03
;
; return file;
;
	jmp     L1716
;
; fclose(file);
;
L12D7:	jsr     ldax0sp
	jsr     _fclose
;
; file = fopen(filename, "rb");
;
	ldy     #$07
	jsr     pushwysp
	lda     #<(l12eb)
	ldx     #>(l12eb)
	jsr     _fopen
	jsr     stax0sp
;
; if(offset) {
;
L12D4:	ldy     #$03
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L12ED
;
; *offset = 0;
;
	ldy     #$05
	jsr     pushwysp
	ldx     #$00
	stx     sreg
	stx     sreg+1
	txa
L1716:	ldy     #$00
	jsr     steaxspidx
;
; return file;
;
L12ED:	jsr     ldax0sp
;
; }
;
	jsr     incsp6
	jmp     farret


; ---------------------------------------------------------------
; long __near__ getUnicodeCharFast (__near__ __near__ unsigned char * *, __near__ __near__ unsigned char * *, int, __near__ int *, __near__ __near__ function returning void *)
; ---------------------------------------------------------------


.endproc
