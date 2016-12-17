.include "../code2.s"
.export _hash_addString
.proc	_hash_addString: near


;
; ) {
;
	jsr     pushax
;
; unsigned int hashval = hash_compare(hashtable, str);
;
	jsr     decsp4
	ldy     #$0B
	jsr     pushwysp
	ldy     #$09
	jsr     ldaxysp
	jsr     _hash_compare
	jsr     pushax
;
; current_list = hash_lookupString(hashtable, str);
;
	ldy     #$0D
	jsr     pushwysp
	ldy     #$0B
	jsr     ldaxysp
	jsr     _hash_lookupString
	ldy     #$04
	jsr     staxysp
;
; if (current_list != NULL) {
;
	cpx     #$00
	bne     L1778
	cmp     #$00
	beq     L1779
;
; return 2;
;
L1778:	ldx     #$00
	lda     #$02
	jmp     L13FE
;
; if ((new_list2 = malloc(sizeof(struct columnRefHashEntry))) == NULL) {
;
L1779:	lda     #$06
	jsr     _malloc
	ldy     #$02
	jsr     staxysp
	cpx     #$00
	bne     L140A
	cmp     #$00
	bne     L140A
;
; return 1;
;
	lda     #$01
	jmp     L13FE
;
; new_list2->referenceName = str;
;
L140A:	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$09
	jsr     ldaxysp
	ldy     #$00
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; new_list2->content = new_list;
;
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$02
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; new_list2->nextReferenceInHash = hashtable->table[hashval];
;
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$0B
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
	jsr     ldaxi
	ldy     #$04
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; hashtable->table[hashval] = new_list2;
;
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	jsr     aslax1
	clc
	adc     ptr1
	sta     ptr1
	txa
	adc     ptr1+1
	sta     ptr1+1
	ldy     #$03
	jsr     ldaxysp
	ldy     #$00
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; return 0;
;
	ldx     #$00
	txa
;
; }
;
L13FE:	ldy     #$0C
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; void __near__ strFree (__near__ __near__ unsigned char * *)
; ---------------------------------------------------------------


.endproc
