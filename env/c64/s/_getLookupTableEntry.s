.include "../code2.s"
.export _getLookupTableEntry
.proc	_getLookupTableEntry: near


;
; ) {
;
	jsr     pushax
;
; struct hash4Entry *temp = NULL, *temp2 = NULL;
;
	jsr     push0
	jsr     push0
;
; int totalBytes = 0;
;
	jsr     push0
;
; if(isNumberWithGetByteLength(*offset, lastMatchedBytes, firstChar)) {
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     pushw
	ldy     #$0F
	jsr     pushwysp
	ldy     #$0B
	jsr     ldaxysp
	jsr     _isNumberWithGetByteLength
	stx     tmp1
	ora     tmp1
	beq     L1331
;
; return &numberEntry;
;
	lda     #<(_numberEntry)
	ldx     #>(_numberEntry)
	jmp     L1325
;
; temp2 = temp;
;
L132F:	ldy     #$05
	jsr     ldaxysp
	ldy     #$02
	jsr     staxysp
;
; totalBytes += *lastMatchedBytes;
;
	ldy     #$0B
	jsr     ldaxysp
	jsr     ldaxi
	jsr     addeq0sp
;
; (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get))
;
	ldy     #$0B
;
; (offset, str, totalBytes, lastMatchedBytes, get);
;
	jsr     pushwysp
	ldy     #$13
	jsr     pushwysp
	ldy     #$13
	jsr     pushwysp
	ldy     #$09
	jsr     pushwysp
	ldy     #$15
	jsr     pushwysp
	ldy     #$13
	jsr     ldaxysp
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
;
; while((temp = in_word_set((char const *)(*offset), totalBytes+(*lastMatchedBytes)))) {
;
L1331:	ldy     #$0F
	jsr     ldaxysp
	jsr     pushw
	ldy     #$0D
	jsr     ldaxysp
	jsr     ldaxi
	clc
	ldy     #$02
	adc     (sp),y
	pha
	txa
	iny
	adc     (sp),y
	tax
	pla
	jsr     _in_word_set
	ldy     #$04
	jsr     staxysp
	stx     tmp1
	ora     tmp1
	bne     L132F
;
; if(temp2 != NULL) {
;
	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L171A
;
; *lastMatchedBytes = totalBytes;
;
	ldy     #$0B
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
; return temp2;
;
	ldy     #$03
L171A:	jsr     ldaxysp
;
; }
;
L1325:	ldy     #$10
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ isNumberWithGetByteLength (__near__ unsigned char *, __near__ int *, int)
; ---------------------------------------------------------------


.endproc
