.include "../code.s"
.export _main
.proc	_main: near


;
; argc2 = argc;
;
	jsr     decsp4
	ldy     #$07
	jsr     ldaxysp
	ldy     #$02
	jsr     staxysp
;
; argv2 = argv;
;
	ldy     #$05
	jsr     ldaxysp
	jsr     stax0sp
;
; if(getenv(TEMP_VAR) == NULL) {
;
	lda     #<(l04c9)
	ldx     #>(l04c9)
	jsr     _getenv
	cpx     #$00
	bne     L04C7
	cmp     #$00
	bne     L04C7
;
; putenv(DEFAULT_TEMP);
;
	lda     #<(l04cd)
	ldx     #>(l04cd)
	jsr     _putenv
;
; if(getenv("TZ") == NULL) {
;
L04C7:	lda     #<(l04d1)
	ldx     #>(l04d1)
	jsr     _getenv
	cpx     #$00
	bne     L1553
	cmp     #$00
	bne     L1553
;
; putenv(TDB_DEFAULT_TZ);
;
	lda     #<(l04d5)
	ldx     #>(l04d5)
	jsr     _putenv
;
; setlocale(LC_ALL, TDB_LOCALE);
;
L1553:	jsr     push0
	lda     #<(l04d9)
	ldx     #>(l04d9)
	jsr     _setlocale
;
; if(argc2 == 2) {
;
	ldy     #$03
	lda     (sp),y
	bne     L04DB
	dey
	lda     (sp),y
	cmp     #$02
	bne     L04DB
;
; return runQuery(argv2[1]);
;
	jsr     ldax0sp
	ldy     #$03
	jsr     ldaxidx
	jsr     _runQuery
	jmp     L04C2
;
; fputs(TDB_INVALID_COMMAND_LINE, stderr);
;
L04DB:	lda     #<(l04e2)
	ldx     #>(l04e2)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; return -1;
;
	ldx     #$FF
	txa
;
; }
;
L04C2:	jsr     incsp8
	jmp     farret


; ---------------------------------------------------------------
; void __near__ groupResults (__near__ struct qryData *)
; ---------------------------------------------------------------


.endproc
