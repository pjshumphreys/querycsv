.include "../code2.s"
.export _hash_createTable
.proc	_hash_createTable: near


;
; struct columnReferenceHash *hash_createTable(int size) {
;
	jsr     pushax
;
; if (size < 1) {
;
	jsr     decsp4
	ldy     #$05
	jsr     ldaxysp
	cmp     #$01
	txa
	sbc     #$00
	bvc     L01B8
	eor     #$80
L01B8:	asl     a
	ldx     #$00
	bcc     L14C8
;
; return NULL; /* invalid size for table */
;
	txa
	jmp     L01B5
;
; if ((new_table = malloc(sizeof(struct columnReferenceHash))) == NULL) {
;
L14C8:	lda     #$04
	jsr     _malloc
	ldy     #$02
	jsr     staxysp
	cpx     #$00
	bne     L01BA
	cmp     #$00
;
; return NULL;
;
	jeq     L01B5
;
; if ((new_table->table = malloc(sizeof(struct columnReference *) * size)) == NULL) {
;
L01BA:	ldy     #$05
	jsr     pushwysp
	ldy     #$07
	jsr     ldaxysp
	jsr     shlax1
	jsr     _malloc
	ldy     #$02
	jsr     staxspidx
	cpx     #$00
	bne     L01C1
	cmp     #$00
	bne     L14CA
;
; return NULL;
;
	jmp     L01B5
;
; for(i=0; i<size; i++) {
;
L01C1:	ldx     #$00
L14CA:	txa
L14CB:	jsr     stax0sp
	ldy     #$04
	cmp     (sp),y
	txa
	iny
	sbc     (sp),y
	bvc     L14C7
	eor     #$80
L14C7:	bpl     L01C9
;
; new_table->table[i] = NULL;
;
	ldy     #$03
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
	lda     #$00
	tay
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; for(i=0; i<size; i++) {
;
	jsr     ldax0sp
	jsr     incax1
	jmp     L14CB
;
; new_table->size = size;
;
L01C9:	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$05
	jsr     ldaxysp
	ldy     #$00
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; return new_table;
;
	ldy     #$03
	jsr     ldaxysp
;
; }
;
L01B5:	jsr     incsp6
	jmp     farret


; ---------------------------------------------------------------
; void __near__ parse_tableFactor (__near__ struct qryData *, int, __near__ unsigned char *, __near__ unsigned char *)
; ---------------------------------------------------------------


.endproc
