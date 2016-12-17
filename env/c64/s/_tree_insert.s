.include "../code2.s"
.export _tree_insert
.proc	_tree_insert: near


;
; ) {
;
	jsr     pushax
;
; if(root == NULL) {
;
	jsr     decsp6
	ldy     #$06
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L11C7
;
; return FALSE;
;
	tax
	jmp     L11E6
;
; if((newResult = malloc(sizeof(struct resultTree))) == NULL) {
;
L11C7:	ldx     #$00
	lda     #$0A
	jsr     _malloc
	ldy     #$02
	jsr     staxysp
	cpx     #$00
	bne     L11CB
	cmp     #$00
;
; return FALSE;
;
	jeq     L11E6
;
; newResult->parent = NULL;
;
L11CB:	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$04
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; newResult->left = NULL;
;
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	tay
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; newResult->right = NULL;
;
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$02
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; newResult->columns = columns;
;
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$09
	jsr     ldaxysp
	ldy     #$06
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newResult->type = TRE_BLACK;
;
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$08
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; if(*root == NULL) {
;
	ldy     #$07
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L11DC
	cmp     #$00
	bne     L11DC
;
; *root = newResult;
;
	ldy     #$07
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$03
	jsr     ldaxysp
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
	jmp     L11E6
;
; currentResult = *root;
;
L11DC:	ldy     #$07
L16F3:	jsr     ldaxysp
	ldy     #$01
L16F1:	jsr     ldaxidx
	ldy     #$04
	jsr     staxysp
;
; comparison = recordCompare(currentResult->columns, columns, query);
;
	ldy     #$07
	jsr     pushwidx
	ldy     #$0D
	jsr     pushwysp
	ldy     #$0F
	jsr     ldaxysp
	jsr     _recordCompare
	jsr     stax0sp
;
; if(comparison < 1) {
;
	cmp     #$01
	txa
	sbc     #$00
	bvc     L11F0
	eor     #$80
L11F0:	bpl     L11EE
;
; if(currentResult->left == NULL) {
;
	ldy     #$05
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L11F1
	cmp     #$00
	bne     L11F1
;
; newResult->parent = currentResult;
;
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$05
	jsr     ldaxysp
	ldy     #$04
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; currentResult->left = newResult;
;
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$03
	jsr     ldaxysp
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
	jmp     L11E6
;
; currentResult = currentResult->left;
;
L11F1:	ldy     #$05
;
; else {
;
	jmp     L16F3
;
; if(currentResult->right == NULL) {
;
L11EE:	ldy     #$05
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     #$00
	bne     L11FD
	cmp     #$00
	bne     L11FD
;
; newResult->parent = currentResult;
;
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$05
	jsr     ldaxysp
	ldy     #$04
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; currentResult->right = newResult;
;
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$03
	jsr     ldaxysp
	ldy     #$02
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; return TRUE;
;
	ldx     #$00
	lda     #$01
	jmp     L11E6
;
; currentResult = currentResult->right;
;
L11FD:	ldy     #$05
	jsr     ldaxysp
	ldy     #$03
;
; }
;
	jmp     L16F1
;
; }
;
L11E6:	ldy     #$0C
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ d_tztime (__near__ unsigned long *, __near__ struct tm *, __near__ struct tm *, __near__ __near__ unsigned char * *)
; ---------------------------------------------------------------


.endproc
