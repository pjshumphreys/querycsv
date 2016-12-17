.include "../code2.s"
.export _d_tztime
.proc	_d_tztime: near


;
; ) {
;
	jsr     pushax
;
; char *format = "+%02i%02i";
;
	ldy     #$10
	jsr     subysp
	lda     #<(l1209)
	ldx     #>(l1209)
	jsr     pushax
;
; if(now == NULL) {
;
	ldy     #$18
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L120B
;
; return FALSE;
;
	tax
	jmp     L1208
;
; if(local != NULL) {
;
L120B:	ldy     #$16
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L120F
;
; lcl = local;
;
	jsr     ldaxysp
	ldy     #$10
	jsr     staxysp
;
; else if (output != NULL) {
;
	jmp     L1707
L120F:	ldy     #$12
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L121A
;
; lcl = (struct tm *)malloc(sizeof(struct tm));
;
	ldx     #$00
	lda     #$12
	jsr     _malloc
	ldy     #$10
	jsr     staxysp
;
; if(lcl == NULL) {
;
	cpx     #$00
	bne     L1220
	cmp     #$00
;
; return FALSE;
;
	jeq     L1208
;
; memcpy((void*)lcl, (void*)localtime(now), sizeof(struct tm));
;
L1220:	ldy     #$11
	jsr     ldaxysp
L1707:	jsr     pushax
	ldy     #$1B
	jsr     ldaxysp
	jsr     _localtime
	jsr     pushax
	ldx     #$00
	lda     #$12
	jsr     _memcpy
;
; if(utc != NULL) {
;
L121A:	ldy     #$14
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L1229
;
; gm = utc;
;
	jsr     ldaxysp
	ldy     #$0E
	jsr     staxysp
;
; else if(output != NULL) {
;
	jmp     L1708
L1229:	ldy     #$12
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L1234
;
; gm = (struct tm *)malloc(sizeof(struct tm));
;
	ldx     #$00
	lda     #$12
	jsr     _malloc
	ldy     #$0E
	jsr     staxysp
;
; if(gm == NULL) {
;
	cpx     #$00
	bne     L123A
	cmp     #$00
	bne     L123A
;
; if(local == NULL) {
;
	ldy     #$16
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L16FA
;
; free((void*)lcl);
;
	ldy     #$11
	jsr     ldaxysp
	jsr     _free
;
; return FALSE;
;
	ldx     #$00
L16FA:	txa
	jmp     L1208
;
; memcpy((void*)gm, (void*)gmtime(now), sizeof(struct tm));
;
L123A:	ldy     #$0F
	jsr     ldaxysp
L1708:	jsr     pushax
	ldy     #$1B
	jsr     ldaxysp
	jsr     _gmtime
	jsr     pushax
	ldx     #$00
	lda     #$12
	jsr     _memcpy
;
; if(output != NULL) {
;
L1234:	ldy     #$12
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L1248
;
; hourlcl = lcl->tm_hour;
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$0C
	jsr     staxysp
;
; hourutc = gm->tm_hour;
;
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$0A
	jsr     staxysp
;
; output2 = (char*)realloc((void*)*output, 6);
;
	ldy     #$13
	jsr     ldaxysp
	jsr     pushw
	ldx     #$00
	lda     #$06
	jsr     _realloc
	ldy     #$02
	jsr     staxysp
;
; if(output2 == NULL) {
;
	cpx     #$00
	bne     L1253
	cmp     #$00
	bne     L1253
;
; if(local == NULL) {
;
	ldy     #$16
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L1256
;
; free((void*)lcl);
;
	ldy     #$11
	jsr     ldaxysp
	jsr     _free
;
; if(utc == NULL) {
;
L1256:	ldy     #$14
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L125B
;
; free((void*)gm);
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     _free
;
; return FALSE;
;
L125B:	ldx     #$00
	txa
	jmp     L1208
;
; *output = output2;
;
L1253:	ldy     #$13
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
; if(lcl->tm_year > gm->tm_year) {
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0B
	jsr     pushwidx
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0B
	jsr     ldaxidx
	jsr     tosicmp
	bmi     L1263
	beq     L1263
;
; hourlcl+=24;
;
	ldy     #$0C
;
; else if (gm->tm_year > lcl->tm_year) {
;
	jmp     L16FD
L1263:	ldy     #$0F
	jsr     ldaxysp
	ldy     #$0B
	jsr     pushwidx
	ldy     #$13
	jsr     ldaxysp
	ldy     #$0B
	jsr     ldaxidx
	jsr     tosicmp
	bmi     L1268
;
; else if(lcl->tm_yday > gm->tm_yday) {
;
	bne     L1700
L1268:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     pushwidx
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	jsr     tosicmp
	bmi     L126D
	beq     L126D
;
; hourlcl+=24;
;
	ldy     #$0C
;
; else if (gm->tm_yday > lcl->tm_yday) {
;
	jmp     L16FD
L126D:	ldy     #$0F
	jsr     ldaxysp
	ldy     #$0F
	jsr     pushwidx
	ldy     #$13
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	jsr     tosicmp
	bmi     L1272
	beq     L1272
;
; hourutc+=24;
;
L1700:	ldy     #$0A
L16FD:	ldx     #$00
	lda     #$18
	jsr     addeqysp
;
; difference = ((hourlcl - hourutc)*60)+lcl->tm_min - gm->tm_min;
;
L1272:	ldy     #$0D
	jsr     ldaxysp
	sec
	ldy     #$0A
	sbc     (sp),y
	pha
	txa
	iny
	sbc     (sp),y
	tax
	pla
	jsr     pushax
	lda     #$3C
	jsr     tosmula0
	jsr     pushax
	ldy     #$13
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jsr     tosaddax
	jsr     pushax
	ldy     #$11
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jsr     tossubax
	ldy     #$08
	jsr     staxysp
;
; minute_difference = difference % 60;
;
	jsr     pushax
	ldx     #$00
	lda     #$3C
	jsr     tosmoda0
	ldy     #$04
	jsr     staxysp
;
; hour_difference = (difference - minute_difference)/60;
;
	ldy     #$09
	jsr     ldaxysp
	sec
	ldy     #$04
	sbc     (sp),y
	pha
	txa
	iny
	sbc     (sp),y
	tax
	pla
	jsr     pushax
	lda     #$3C
	jsr     tosdiva0
	ldy     #$06
	jsr     staxysp
;
; if(hour_difference < 0) {
;
	cpx     #$80
	bcc     L127F
;
; format = "%03i%02i";
;
	lda     #<(l1282)
	ldx     #>(l1282)
	jsr     stax0sp
;
; sprintf(*output, format, hour_difference, minute_difference);
;
L127F:	ldy     #$13
	jsr     ldaxysp
	jsr     pushw
	ldy     #$05
	jsr     pushwysp
	ldy     #$0D
	jsr     pushwysp
	ldy     #$0D
	jsr     pushwysp
	ldy     #$08
	jsr     _sprintf
;
; if(local == NULL) {
;
L1248:	ldy     #$16
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L1289
;
; free((void*)lcl);
;
	ldy     #$11
	jsr     ldaxysp
	jsr     _free
;
; if(utc == NULL) {
;
L1289:	ldy     #$14
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L128E
;
; free((void*)gm);
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     _free
;
; return TRUE;
;
L128E:	ldx     #$00
	lda     #$01
;
; }
;
L1208:	ldy     #$1A
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; __near__ struct resultColumnValue * __near__ getFirstRecord (__near__ struct resultTree *, __near__ struct qryData *)
; ---------------------------------------------------------------


.endproc
