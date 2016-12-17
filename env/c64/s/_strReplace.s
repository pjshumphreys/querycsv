.include "../code2.s"
.export _strReplace
.proc	_strReplace: near


;
; char *strReplace(char *search, char *replace, char *subject) {
;
	jsr     pushax
;
; char *replaced = (char*)calloc(1, 1), *temp = NULL;
;
	lda     #$01
	jsr     pusha0
	jsr     _calloc
	jsr     pushax
	jsr     push0
;
; char *p = subject, *p3 = subject, *p2;
;
	ldy     #$07
	jsr     pushwysp
	ldy     #$09
	jsr     pushwysp
;
; int found = 0;
;
	jsr     decsp2
	jsr     push0
;
; search == NULL ||
;
	ldy     #$10
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L08C0
;
; replace == NULL ||
;
	ldy     #$0E
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L08C0
;
; subject == NULL ||
;
	ldy     #$0C
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L08C0
;
; strlen(search) == 0 ||
;
	ldy     #$11
	jsr     ldaxysp
	jsr     _strlen
	cpx     #$00
	bne     L15EC
	cmp     #$00
	jeq     L08C0
;
; strlen(replace) == 0 ||
;
L15EC:	ldy     #$0F
	jsr     ldaxysp
	jsr     _strlen
	cpx     #$00
	bne     L15ED
	cmp     #$00
	jeq     L08C0
;
; strlen(subject) == 0
;
L15ED:	ldy     #$0D
	jsr     ldaxysp
	jsr     _strlen
;
; ) {
;
	cpx     #$00
	jne     L08D7
	cmp     #$00
	jeq     L08C0
	jmp     L08D7
;
; found = 1;
;
L08D5:	ldx     #$00
L15F2:	lda     #$01
	jsr     stax0sp
;
; temp = realloc(replaced, strlen(replaced) + (p - p3) + strlen(replace));
;
	ldy     #$0D
	jsr     pushwysp
	ldy     #$0D
	jsr     ldaxysp
	jsr     _strlen
	jsr     pushax
	ldy     #$0B
	jsr     ldaxysp
	sec
	ldy     #$08
	sbc     (sp),y
	pha
	txa
	iny
	sbc     (sp),y
	tax
	pla
	jsr     tosaddax
	jsr     pushax
	ldy     #$13
	jsr     ldaxysp
	jsr     _strlen
	jsr     tosaddax
	jsr     _realloc
	ldy     #$08
	jsr     staxysp
;
; if(temp == NULL) {
;
	cpx     #$00
	bne     L08E7
	cmp     #$00
	bne     L08E7
;
; free(replaced);
;
	ldy     #$0B
	jsr     ldaxysp
	jsr     _free
;
; return NULL;
;
	ldx     #$00
	txa
	jmp     L08C0
;
; replaced = temp;
;
L08E7:	ldy     #$09
	jsr     ldaxysp
	ldy     #$0A
	jsr     staxysp
;
; strncat(replaced, p - (p - p3), p - p3);
;
	jsr     pushax
	ldy     #$0B
	jsr     pushwysp
	ldy     #$0B
	jsr     ldaxysp
	sec
	ldy     #$08
	sbc     (sp),y
	pha
	txa
	iny
	sbc     (sp),y
	tax
	pla
	jsr     tossubax
	jsr     pushax
	ldy     #$0B
	jsr     ldaxysp
	sec
	ldy     #$08
	sbc     (sp),y
	pha
	txa
	iny
	sbc     (sp),y
	tax
	pla
	jsr     _strncat
;
; strcat(replaced, replace);
;
	ldy     #$0D
	jsr     pushwysp
	ldy     #$11
	jsr     ldaxysp
	jsr     _strcat
;
; p3 = p + strlen(search);
;
	ldy     #$09
	jsr     pushwysp
	ldy     #$13
	jsr     ldaxysp
	jsr     _strlen
	jsr     tosaddax
	ldy     #$04
	jsr     staxysp
;
; p += strlen(search);
;
	ldy     #$11
	jsr     ldaxysp
	jsr     _strlen
	ldy     #$06
	jsr     addeqysp
;
; p2 = p;
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$02
	jsr     staxysp
;
; while((p = strstr(p, search)) != NULL) {
;
L08D7:	ldy     #$09
	jsr     pushwysp
	ldy     #$13
	jsr     ldaxysp
	jsr     _strstr
	ldy     #$06
	jsr     staxysp
	cpx     #$00
	jne     L08D5
	cmp     #$00
	jne     L15F2
;
; if (found == 1) {
;
	ldy     #$01
	lda     (sp),y
	bne     L08FF
	lda     (sp,x)
	cmp     #$01
	bne     L08FF
;
; if (strlen(p2) > 0) {
;
	ldy     #$03
	jsr     ldaxysp
	jsr     _strlen
	cpx     #$00
	bne     L15EF
	cmp     #$00
	jeq     L091D
;
; temp = realloc(replaced, strlen(replaced) + strlen(p2) + 1);
;
L15EF:	ldy     #$0D
	jsr     pushwysp
	ldy     #$0D
	jsr     ldaxysp
	jsr     _strlen
	jsr     pushax
	ldy     #$07
	jsr     ldaxysp
	jsr     _strlen
	jsr     tosaddax
	jsr     incax1
	jsr     _realloc
	ldy     #$08
	jsr     staxysp
;
; if (temp == NULL) {
;
	cpx     #$00
	bne     L090C
	cmp     #$00
	bne     L090C
;
; free(replaced);
;
	ldy     #$0B
	jsr     ldaxysp
	jsr     _free
;
; return NULL;
;
	ldx     #$00
	txa
	jmp     L08C0
;
; replaced = temp;
;
L090C:	ldy     #$09
	jsr     ldaxysp
	ldy     #$0A
	jsr     staxysp
;
; strcat(replaced, p2);
;
	jsr     pushax
	ldy     #$05
	jsr     ldaxysp
	jsr     _strcat
;
; else {
;
	jmp     L091D
;
; temp = realloc(replaced, strlen(subject) + 1);
;
L08FF:	ldy     #$0D
	jsr     pushwysp
	ldy     #$0F
	jsr     ldaxysp
	jsr     _strlen
	jsr     incax1
	jsr     _realloc
	ldy     #$08
	jsr     staxysp
;
; if (temp != NULL) {
;
	cpx     #$00
	bne     L15F0
	cmp     #$00
	beq     L091D
;
; replaced = temp;
;
L15F0:	ldy     #$09
	jsr     ldaxysp
	ldy     #$0A
	jsr     staxysp
;
; strcpy(replaced, subject);
;
	jsr     pushax
	ldy     #$0F
	jsr     ldaxysp
	jsr     _strcpy
;
; return replaced;
;
L091D:	ldy     #$0B
	jsr     ldaxysp
;
; }
;
L08C0:	ldy     #$12
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; __near__ struct expression * __near__ parse_functionRef (__near__ struct qryData *, long, __near__ struct expression *, int)
; ---------------------------------------------------------------


.endproc
