.include "../code2.s"
.export _consumeCombiningChars
.proc	_consumeCombiningChars: near


;
; ) {
;
	jsr     pushax
;
; int combiner1, combiner2, skip1 = FALSE, skip2 = FALSE;
;
	jsr     decsp4
	jsr     push0
	jsr     push0
;
; (*offset1)+=(*bytesMatched1);
;
	ldy     #$15
	jsr     ldaxysp
	jsr     pushax
	jsr     pushw
	ldy     #$11
	jsr     ldaxysp
	jsr     ldaxi
	jsr     tosaddax
	ldy     #$00
	jsr     staxspidx
;
; (*offset2)+=(*bytesMatched2);
;
	ldy     #$13
	jsr     ldaxysp
	jsr     pushax
	jsr     pushw
	ldy     #$0F
	jsr     ldaxysp
	jsr     ldaxi
	jsr     tosaddax
	ldy     #$00
	jsr     staxspidx
;
; if(skip1 == FALSE) {
;
L0CDE:	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L0CE1
;
; (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get1))
;
	ldy     #$13
;
; (offset1, str1, 0, bytesMatched1, get1)
;
	jsr     pushwysp
	ldy     #$19
	jsr     pushwysp
	ldy     #$1F
	jsr     pushwysp
	jsr     push0
	ldy     #$17
	jsr     pushwysp
	ldy     #$1B
	jsr     ldaxysp
;
; );
;
	pha
	ldy     #$08
	lda     (sp),y
	sta     jmpvec+1
	iny
	lda     (sp),y
	sta     jmpvec+2
	pla
	jsr     jmpvec
	jsr     incsp2
	jsr     axlong
	jsr     _isCombiningChar
	ldy     #$06
	jsr     staxysp
;
; if(skip2 == FALSE) {
;
L0CE1:	ldy     #$00
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L0CEE
;
; (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get2))
;
	ldy     #$11
;
; (offset2, str2, 0, bytesMatched2, get2)
;
	jsr     pushwysp
	ldy     #$17
	jsr     pushwysp
	ldy     #$1D
	jsr     pushwysp
	jsr     push0
	ldy     #$15
	jsr     pushwysp
	ldy     #$19
	jsr     ldaxysp
;
; );
;
	pha
	ldy     #$08
	lda     (sp),y
	sta     jmpvec+1
	iny
	lda     (sp),y
	sta     jmpvec+2
	pla
	jsr     jmpvec
	jsr     incsp2
	jsr     axlong
	jsr     _isCombiningChar
	ldy     #$04
	jsr     staxysp
;
; if(skip1 == FALSE && skip2 == FALSE) {
;
L0CEE:	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L0D0C
	tay
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L0D0C
;
; if(combiner1 != combiner2) {
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$04
	cmp     (sp),y
	bne     L164F
	txa
	iny
	cmp     (sp),y
	beq     L0D0C
;
; if(*accentcheck == 2) {
;
L164F:	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L0D03
	cmp     #$02
	bne     L0D03
;
; return (combiner1 > combiner2) ? 1 : -1;
;
	ldy     #$09
	jsr     pushwysp
	ldy     #$07
	jsr     ldaxysp
	jsr     tosicmp
	bmi     L0D08
	beq     L0D08
	ldx     #$00
	lda     #$01
	jmp     L0CD3
L0D08:	ldx     #$FF
	jmp     L1650
;
; *accentcheck = 1;
;
L0D03:	ldy     #$09
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; if(combiner1 == 0) {
;
L0D0C:	ldy     #$06
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L0D0F
;
; skip1 = TRUE; 
;
	tax
	lda     #$01
	ldy     #$02
	jsr     staxysp
;
; else {
;
	jmp     L0D14
;
; (*offset1)+=(*bytesMatched1);
;
L0D0F:	ldy     #$15
	jsr     ldaxysp
	jsr     pushax
	jsr     pushw
	ldy     #$11
	jsr     ldaxysp
	jsr     ldaxi
	jsr     tosaddax
	ldy     #$00
	jsr     staxspidx
;
; if(combiner2 == 0) {
;
L0D14:	ldy     #$04
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L0D19
;
; skip2 = TRUE; 
;
	tax
	lda     #$01
	jsr     stax0sp
;
; else {
;
	jmp     L0D1E
;
; (*offset2)+=(*bytesMatched2);
;
L0D19:	ldy     #$13
	jsr     ldaxysp
	jsr     pushax
	jsr     pushw
	ldy     #$0F
	jsr     ldaxysp
	jsr     ldaxi
	jsr     tosaddax
	ldy     #$00
	jsr     staxspidx
;
; } while (skip1 == FALSE || skip2 == FALSE);
;
L0D1E:	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L0CDE
	ldy     #$00
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L0CDE
	ldx     #$00
L1650:	txa
;
; }
;
L0CD3:	ldy     #$1A
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ getNextRecordOffset (__near__ unsigned char *, long)
; ---------------------------------------------------------------


.endproc
