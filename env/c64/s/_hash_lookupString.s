.include "../code2.s"
.export _hash_lookupString
.proc	_hash_lookupString: near


;
; ) {
;
	jsr     pushax
;
; unsigned int hashval = hash_compare(hashtable, str);
;
	jsr     decsp2
	ldy     #$07
	jsr     pushwysp
	ldy     #$05
	jsr     ldaxysp
	jsr     _hash_compare
	jsr     pushax
;
; for(list = hashtable->table[hashval]; list != NULL; list = list->nextReferenceInHash) {
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	jsr     aslax1
	clc
	adc     ptr1
	pha
	txa
	adc     ptr1+1
	tax
	pla
	ldy     #$01
L164B:	jsr     ldaxidx
	ldy     #$02
	jsr     staxysp
	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0CB1
;
; if (strcmp(str, list->referenceName) == 0) {
;
	ldy     #$07
	jsr     pushwysp
	ldy     #$05
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _strcmp
	cpx     #$00
	bne     L0CB2
	cmp     #$00
	bne     L0CB2
;
; return list->content;
;
	ldy     #$03
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jmp     L0CAC
;
; for(list = hashtable->table[hashval]; list != NULL; list = list->nextReferenceInHash) {
;
L0CB2:	ldy     #$03
	jsr     ldaxysp
	ldy     #$05
	jmp     L164B
;
; return NULL;
;
L0CB1:	tax
;
; }
;
L0CAC:	jsr     incsp8
	jmp     farret


; ---------------------------------------------------------------
; void __near__ reallocMsg (__near__ __near__ void * *, unsigned int)
; ---------------------------------------------------------------


.endproc
