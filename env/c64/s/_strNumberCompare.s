.include "../code2.s"
.export _strNumberCompare
.proc	_strNumberCompare: near


;
; int strNumberCompare(char *input1, char *input2) {
;
	jsr     pushax
;
; char* string1 = input1;
;
	ldy     #$05
	jsr     pushwysp
;
; char* string2 = input2;
;
	ldy     #$05
	jsr     pushwysp
;
; int decimalNotFound1 = TRUE;
;
	lda     #$01
	jsr     pusha0
;
; int decimalNotFound2 = TRUE;
;
	jsr     pusha0
;
; int compare = 0;
;
	jsr     push0
;
; int negate = 1;
;
	lda     #$01
	jsr     pusha0
;
; int do1 = TRUE;
;
	jsr     pusha0
;
; int do2 = TRUE;
;
	jsr     pusha0
;
; if(*string1 == '-') {
;
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$2D
	bne     L0301
;
; string1++;
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     incax1
	ldy     #$0E
	jsr     staxysp
;
; if(*string2 != '-') {
;
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$2D
	beq     L0304
;
; if(*string2 == '+') {
;
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$2B
	bne     L0306
;
; string2++;
;
	ldy     #$0D
	jsr     ldaxysp
	jsr     incax1
	ldy     #$0C
	jsr     staxysp
;
; compare = -1;
;
L0306:	ldx     #$FF
	txa
	ldy     #$06
;
; else {
;
	jmp     L152D
;
; negate = -1;
;
L0304:	ldx     #$FF
	txa
	ldy     #$04
	jsr     staxysp
;
; else {
;
	jmp     L1538
;
; if(*string1 == '+') {
;
L0301:	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$2B
	bne     L0310
;
; string1++;
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     incax1
	ldy     #$0E
	jsr     staxysp
;
; if(*string2 == '-') {
;
L0310:	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$2D
	bne     L0313
;
; string2++;
;
	ldy     #$0D
	jsr     ldaxysp
	jsr     incax1
	ldy     #$0C
	jsr     staxysp
;
; compare = 1;
;
	ldx     #$00
	lda     #$01
	ldy     #$06
;
; else if(*string2 == '+') {
;
	jmp     L152D
L0313:	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$2B
	bne     L0319
;
; string2++;
;
L1538:	ldy     #$0D
	jsr     ldaxysp
	jsr     incax1
	ldy     #$0C
L152D:	jsr     staxysp
;
; if(compare == 0) {
;
L0319:	ldy     #$06
	lda     (sp),y
	iny
	ora     (sp),y
	jne     L152C
;
; while(*string1 == '0') {
;
	jmp     L0321
;
; string1++;
;
L031F:	ldy     #$0F
	jsr     ldaxysp
	jsr     incax1
	ldy     #$0E
	jsr     staxysp
;
; while(*string1 == '0') {
;
L0321:	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	beq     L031F
;
; while(*string2 == '0') {
;
	jmp     L0326
;
; string2++;
;
L0324:	ldy     #$0D
	jsr     ldaxysp
	jsr     incax1
	ldy     #$0C
	jsr     staxysp
;
; while(*string2 == '0') {
;
L0326:	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	beq     L0324
;
; input1 = string1;
;
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$12
	jsr     staxysp
;
; input2 = string2;
;
	ldy     #$0D
	jsr     ldaxysp
	ldy     #$10
;
; while(compare == 0) {
;
	jmp     L152E
;
; if(*string1 > '9' || *string1 < '0') {
;
L032D:	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$3A
	bcs     L1514
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	jcs     L0332
;
; if(*string2 > '9' || *string2 < '0') {
;
L1514:	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$3A
	bcs     L1515
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	jcs     L0335
;
; string1 = input1;
;
L1515:	ldy     #$13
	jsr     ldaxysp
	ldy     #$0E
	jsr     staxysp
;
; string2 = input2;
;
	ldy     #$11
	jsr     ldaxysp
L1535:	ldy     #$0C
L1530:	jsr     staxysp
;
; if(do1) {
;
	ldy     #$03
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L0351
;
; if(decimalNotFound1 && (*string1 == '.' || *string1 == ',')) {
;
	ldy     #$0B
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L0342
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	cmp     #$2E
	beq     L151B
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	cmp     #$2C
	bne     L0342
;
; decimalNotFound1 = FALSE;
;
L151B:	txa
	ldy     #$0A
	jsr     staxysp
;
; else if(*string1 > '9' || *string1 < '0') {
;
	jmp     L0351
L0342:	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$3A
	bcs     L151C
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	bcs     L151D
;
; do1 = FALSE;
;
L151C:	ldx     #$00
	txa
	ldy     #$02
	jsr     staxysp
;
; else if (do2 == FALSE && *string1 != '0') {
;
	jmp     L0351
L151D:	lda     (sp),y
	iny
	ora     (sp),y
	bne     L1531
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
;
; break;
;
	jne     L0335
;
; if(do2) {
;
L0351:	ldy     #$01
L1531:	lda     (sp),y
	dey
	ora     (sp),y
	jeq     L0369
;
; if(decimalNotFound2 && (*string2 == '.' || *string2 == ',')) {
;
	ldy     #$09
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L035A
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	cmp     #$2E
	beq     L1525
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	cmp     #$2C
	bne     L035A
;
; decimalNotFound2 = FALSE;
;
L1525:	txa
	ldy     #$08
	jsr     staxysp
;
; else if(*string2 > '9' || *string2 < '0') {
;
	jmp     L0369
L035A:	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$3A
	bcs     L1526
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	bcs     L0363
;
; do2 = FALSE;
;
L1526:	ldx     #$00
	txa
	jsr     stax0sp
;
; else if (do1 == FALSE && *string2 != '0') {
;
	jmp     L0369
L0363:	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L1532
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	beq     L0369
;
; compare = 1;
;
	ldx     #$00
	lda     #$01
;
; break;
;
	jmp     L152F
;
; if(do1) {
;
L0369:	ldy     #$03
L1532:	lda     (sp),y
	dey
	ora     (sp),y
	jeq     L0370
;
; if(do2) {
;
	dey
	lda     (sp),y
	dey
	ora     (sp),y
	jeq     L0372
;
; if(*string1 >= '0' &&
;
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	jcc     L0374
;
; *string1 <= '9' &&
;
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$3A
	bcs     L0374
;
; *string2 >= '0' &&
;
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	bcc     L0374
;
; *string2 <= '9' &&
;
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$3A
	bcs     L0374
;
; *string1 != *string2) {
;
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	jsr     pusha0
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	jsr     tosicmp0
	beq     L0374
;
; compare = *string1 < *string2?-1:1;
;
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	jsr     pusha0
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	jsr     tosicmp0
	bcc     L0335
	ldx     #$00
	lda     #$01
;
; break;
;
	jmp     L152F
;
; string2++;
;
L0374:	ldy     #$0D
	jsr     ldaxysp
	jsr     incax1
	ldy     #$0C
	jsr     staxysp
;
; string1++;
;
L0372:	ldy     #$0F
	jsr     ldaxysp
	jsr     incax1
	ldy     #$0E
;
; else if(do2) {
;
	jmp     L1530
L0370:	dey
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L0381
;
; string2++;
;
	ldy     #$0D
	jsr     ldaxysp
	jsr     incax1
;
; else {
;
	jmp     L1535
;
; return 0;
;
L0381:	tax
	jmp     L02F8
;
; compare = -1;
;
L0335:	ldx     #$FF
	txa
L152F:	ldy     #$06
;
; else if(*string2 > '9' || *string2 < '0') {
;
	jmp     L152E
L0332:	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$3A
	bcs     L152B
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	cmp     #$30
	bcs     L038A
;
; compare = 1;
;
L152B:	ldx     #$00
	lda     #$01
	ldy     #$06
;
; else {
;
	jmp     L152E
;
; string1++;
;
L038A:	ldy     #$0F
	jsr     ldaxysp
	jsr     incax1
	ldy     #$0E
	jsr     staxysp
;
; string2++;
;
	ldy     #$0D
	jsr     ldaxysp
	jsr     incax1
	ldy     #$0C
L152E:	jsr     staxysp
;
; while(compare == 0) {
;
	ldy     #$06
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L032D
;
; return compare * negate;
;
L152C:	ldy     #$09
	jsr     pushwysp
	ldy     #$07
	jsr     ldaxysp
	jsr     tosmulax
;
; }
;
L02F8:	ldy     #$14
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; long __near__ getUnicodeChar (__near__ __near__ unsigned char * *, __near__ __near__ unsigned char * *, int, __near__ int *, __near__ __near__ function returning void *)
; ---------------------------------------------------------------


.endproc
