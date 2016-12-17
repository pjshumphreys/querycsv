.include "../code2.s"
.export _readParams
.proc	_readParams: near


;
; void readParams(char *string, int *params) {
;
	jsr     pushax
;
; if(string && params) {
;
	ldy     #$03
	lda     (sp),y
	dey
	ora     (sp),y
	jeq     L02CA
	dey
	lda     (sp),y
	dey
	ora     (sp),y
	jne     L02CB
	jmp     L02CA
;
; switch(*string) {
;
L02C9:	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
;
; }
;
	cmp     #$42
	beq     L02E9
	cmp     #$45
	beq     L02E3
	cmp     #$49
	beq     L02DD
	cmp     #$50
	beq     L02D1
	cmp     #$54
	beq     L02D7
	cmp     #$C2
	beq     L02E9
	cmp     #$C5
	beq     L02E3
	cmp     #$C9
	beq     L02DD
	cmp     #$D0
	beq     L02D1
	cmp     #$D4
	beq     L02D7
	jmp     L02CF
;
; *params |= PRM_TRIM;
;
L02D1:	jsr     ldax0sp
	jsr     pushax
	jsr     ldaxi
	ora     #$01
;
; break;
;
	jmp     L1511
;
; *params |= PRM_SPACE;
;
L02D7:	jsr     ldax0sp
	jsr     pushax
	jsr     ldaxi
	ora     #$02
;
; break;
;
	jmp     L1511
;
; *params |= PRM_IMPORT;
;
L02DD:	jsr     ldax0sp
	jsr     pushax
	jsr     ldaxi
	ora     #$04
;
; break;
;
	jmp     L1511
;
; *params |= PRM_EXPORT;
;
L02E3:	jsr     ldax0sp
	jsr     pushax
	jsr     ldaxi
	ora     #$08
;
; break;
;
	jmp     L1511
;
; *params |= PRM_BOM;
;
L02E9:	jsr     ldax0sp
	jsr     pushax
	jsr     ldaxi
	ora     #$10
L1511:	ldy     #$00
	jsr     staxspidx
;
; string++;
;
L02CF:	ldy     #$03
	jsr     ldaxysp
	jsr     incax1
	ldy     #$02
	jsr     staxysp
;
; while(*string) {
;
L02CB:	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	jne     L02C9
;
; }
;
L02CA:	jsr     incsp4
	jmp     farret


; ---------------------------------------------------------------
; int __near__ endOfFile (__near__ struct _FILE *)
; ---------------------------------------------------------------


.endproc
