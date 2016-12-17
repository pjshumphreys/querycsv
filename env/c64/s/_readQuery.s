.include "../code2.s"
.export _readQuery
.proc	_readQuery: near


;
; ) {
;
	jsr     pushax
;
; FILE *queryFile = NULL;
;
	jsr     push0
;
; int inputTableIndex = 2, i;
;
	ldy     #$0C
	jsr     subysp
	lda     #$02
	jsr     pusha0
;
; queryFile = fopen(queryFileName, "r");
;
	jsr     decsp2
	ldy     #$17
	jsr     pushwysp
	lda     #<(l10e9)
	ldx     #>(l10e9)
	jsr     _fopen
	ldy     #$10
	jsr     staxysp
;
; if(queryFile == NULL) {
;
	cpx     #$00
	bne     L10EB
	cmp     #$00
	bne     L10EB
;
; fputs(TDB_COULDNT_OPEN_INPUT, stderr);
;
	lda     #<(l10ef)
	ldx     #>(l10ef)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; exit(EXIT_FAILURE);
;
	ldx     #$00
	lda     #$01
	jsr     _exit
;
; query->parseMode = 0;   /* specify we want to just read the file data for now */
;
L10EB:	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	tay
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->hasGrouping = FALSE;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$02
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->useGroupBy = FALSE;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->columnCount = 0;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$04
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->hiddenColumnCount = 0;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$06
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->recordCount = 0;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$08
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->groupCount = 0;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0A
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->params = 0;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0E
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->intoFileName = NULL;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$10
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->columnReferenceHashTable = hash_createTable(32);
;
	ldy     #$15
	jsr     pushwysp
	ldx     #$00
	lda     #$20
	jsr     _hash_createTable
	ldy     #$16
	jsr     staxspidx
;
; query->firstInputTable = NULL;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$18
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->secondaryInputTable = NULL;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$1A
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->firstResultColumn = NULL;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$1C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->joinsAndWhereClause = NULL;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$1E
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->orderByClause = NULL;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$20
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->groupByClause = NULL;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$22
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->resultSet = NULL;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$24
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; yylex_init(&scanner);
;
	lda     #$0E
	jsr     leaa0sp
	jsr     _yylex_init
;
; yyset_in(queryFile, scanner);
;
	ldy     #$13
	jsr     pushwysp
	ldy     #$11
	jsr     ldaxysp
	jsr     _yyset_in
;
; switch(yyparse(query, scanner)) {
;
	ldy     #$15
	jsr     pushwysp
	ldy     #$11
	jsr     ldaxysp
	jsr     _yyparse
;
; }
;
	cpx     #$00
	bne     L1133
	cmp     #$00
	beq     L1120
	cmp     #$01
	beq     L1124
	cmp     #$02
	beq     L112C
	jmp     L1133
;
; fputs(TDB_PARSER_SYNTAX, stderr);
;
L1124:	lda     #<(l1126)
	ldx     #>(l1126)
;
; break;
;
	jmp     L16ED
;
; fputs(TDB_PARSER_USED_ALL_RAM, stderr);
;
L112C:	lda     #<(l112e)
	ldx     #>(l112e)
;
; break;
;
	jmp     L16ED
;
; fputs(TDB_PARSER_UNKNOWN, stderr);
;
L1133:	lda     #<(l1135)
	ldx     #>(l1135)
L16ED:	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; exit(EXIT_FAILURE);
;
	ldx     #$00
	lda     #$01
	jsr     _exit
;
; query->newLine = query->intoFileName?"\r\n":"\n";
;
L1120:	ldy     #$13
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$10
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L113D
	lda     #<(l113c)
	ldx     #>(l113c)
	jmp     L1140
L113D:	lda     #<(l113f)
	ldx     #>(l113f)
L1140:	iny
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; query->firstInputTable = currentInputTable =
;
	ldy     #$15
	jsr     pushwysp
;
; (query->secondaryInputTable != NULL ?
;
	ldy     #$15
	jsr     ldaxysp
	ldy     #$1B
	jsr     ldaxidx
	cpx     #$00
	bne     L16DC
	cmp     #$00
;
; query->secondaryInputTable :
;
	beq     L1147
L16DC:	ldy     #$15
	jsr     ldaxysp
	ldy     #$1B
;
; query->firstInputTable)->nextInputTable;
;
	jmp     L16D7
L1147:	ldy     #$15
	jsr     ldaxysp
	ldy     #$19
L16D7:	jsr     ldaxidx
	ldy     #$11
	jsr     ldaxidx
	ldy     #$0E
	jsr     staxysp
	ldy     #$18
;
; while (currentInputTable->nextInputTable != query->firstInputTable) {
;
	jmp     L16D8
;
; currentInputTable = currentInputTable->nextInputTable;
;
L114B:	ldy     #$0D
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	ldy     #$0C
	jsr     staxysp
;
; currentInputTable->fileIndex = inputTableIndex++;
;
	jsr     pushax
	ldy     #$05
	jsr     ldaxysp
	sta     regsave
	stx     regsave+1
	jsr     incax1
	ldy     #$04
	jsr     staxysp
	lda     regsave
	ldx     regsave+1
	ldy     #$00
L16D8:	jsr     staxspidx
;
; while (currentInputTable->nextInputTable != query->firstInputTable) {
;
	ldy     #$0D
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	sta     sreg
	stx     sreg+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$19
	jsr     ldaxidx
	cpx     sreg+1
	bne     L114B
	cmp     sreg
	bne     L114B
;
; currentInputTable->nextInputTable = NULL;
;
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$10
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; query->secondaryInputTable = NULL;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$1A
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; for(i=0; i<query->columnReferenceHashTable->size; i++) {
;
	tax
L16E5:	jsr     stax0sp
	jsr     pushax
	ldy     #$15
	jsr     ldaxysp
	ldy     #$17
	jsr     ldaxidx
	jsr     ldaxi
	jsr     tosicmp
	jpl     L1158
;
; currentHashEntry = query->columnReferenceHashTable->table[i];
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$17
	jsr     ldaxidx
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
;
; while(currentHashEntry != NULL) {
;
	jmp     L16E4
;
; if(currentHashEntry->content->nextReferenceWithName) {
;
L16DB:	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	ldy     #$08
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L1167
;
; currentReferenceWithName = currentHashEntry->content->nextReferenceWithName->nextReferenceWithName;
;
	ldy     #$05
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	ldy     #$09
	jsr     ldaxidx
	ldy     #$09
	jsr     ldaxidx
	ldy     #$0A
	jsr     staxysp
;
; currentHashEntry->content->nextReferenceWithName->nextReferenceWithName = NULL;
;
	ldy     #$05
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	ldy     #$09
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$08
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; currentHashEntry->content->nextReferenceWithName = currentReferenceWithName;
;
	ldy     #$05
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$08
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; currentHashEntry = currentHashEntry->nextReferenceInHash;
;
L1167:	ldy     #$05
	jsr     ldaxysp
	ldy     #$05
L16E4:	jsr     ldaxidx
	ldy     #$04
	jsr     staxysp
;
; while(currentHashEntry != NULL) {
;
	ldy     #$04
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L16DB
;
; for(i=0; i<query->columnReferenceHashTable->size; i++) {
;
	jsr     ldax0sp
	jsr     incax1
	jmp     L16E5
;
; yylex_destroy(scanner);
;
L1158:	ldy     #$0F
	jsr     ldaxysp
	jsr     _yylex_destroy
;
; query->parseMode = 1;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; fclose(queryFile);
;
	ldy     #$11
	jsr     ldaxysp
	jsr     _fclose
;
; queryFile = fopen(queryFileName, "r");
;
	ldy     #$17
	jsr     pushwysp
	lda     #<(l117a)
	ldx     #>(l117a)
	jsr     _fopen
	ldy     #$10
	jsr     staxysp
;
; if(queryFile == NULL) {
;
	cpx     #$00
	bne     L117C
	cmp     #$00
	bne     L117C
;
; fputs(TDB_COULDNT_OPEN_INPUT, stderr);
;
	lda     #<(l1180)
	ldx     #>(l1180)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; exit(EXIT_FAILURE);
;
	ldx     #$00
	lda     #$01
	jsr     _exit
;
; yylex_init(&scanner);
;
L117C:	lda     #$0E
	jsr     leaa0sp
	jsr     _yylex_init
;
; yyset_in(queryFile, scanner);
;
	ldy     #$13
	jsr     pushwysp
	ldy     #$11
	jsr     ldaxysp
	jsr     _yyset_in
;
; switch(yyparse(query, scanner)) {
;
	ldy     #$15
	jsr     pushwysp
	ldy     #$11
	jsr     ldaxysp
	jsr     _yyparse
;
; }
;
	cpx     #$00
	bne     L11A1
	cmp     #$00
	beq     L118E
	cmp     #$01
	beq     L1192
	cmp     #$02
	beq     L119A
	jmp     L11A1
;
; fputs(TDB_PARSER_SYNTAX, stderr);
;
L1192:	lda     #<(l1194)
	ldx     #>(l1194)
;
; break;
;
	jmp     L16EE
;
; fputs(TDB_PARSER_USED_ALL_RAM, stderr);
;
L119A:	lda     #<(l119c)
	ldx     #>(l119c)
;
; break;
;
	jmp     L16EE
;
; fputs(TDB_PARSER_UNKNOWN, stderr);
;
L11A1:	lda     #<(l11a3)
	ldx     #>(l11a3)
L16EE:	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; exit(EXIT_FAILURE);
;
	ldx     #$00
	lda     #$01
	jsr     _exit
;
; currentResultColumn = query->firstResultColumn;
;
L118E:	ldy     #$13
	jsr     ldaxysp
	ldy     #$1D
	jsr     ldaxidx
	ldy     #$08
	jsr     staxysp
;
; query->columnCount = currentResultColumn->resultColumnIndex+1;
;
	ldy     #$13
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	jsr     incax1
	ldy     #$04
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; query->firstResultColumn = query->firstResultColumn->nextColumnInResults;
;
	ldy     #$13
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$1D
	jsr     ldaxidx
	ldy     #$17
	jsr     ldaxidx
	ldy     #$1C
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; currentResultColumn->nextColumnInResults = NULL;
;
	ldy     #$09
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$16
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; if(query->orderByClause != NULL) {
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$21
	jsr     ldaxidx
	cpx     #$00
	bne     L16DD
	cmp     #$00
	beq     L11B0
;
; currentSortingList = query->orderByClause;
;
L16DD:	ldy     #$13
	jsr     ldaxysp
	ldy     #$21
	jsr     ldaxidx
	ldy     #$06
	jsr     staxysp
;
; query->orderByClause = query->orderByClause->nextInList;
;
	ldy     #$13
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$21
	jsr     ldaxidx
	ldy     #$05
	jsr     ldaxidx
	ldy     #$20
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; currentSortingList->nextInList = NULL;
;
	ldy     #$07
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$04
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; if(query->groupByClause != NULL) {
;
L11B0:	ldy     #$13
	jsr     ldaxysp
	ldy     #$23
	jsr     ldaxidx
	cpx     #$00
	bne     L16DE
	cmp     #$00
	beq     L11B9
;
; currentSortingList = query->groupByClause;
;
L16DE:	ldy     #$13
	jsr     ldaxysp
	ldy     #$23
	jsr     ldaxidx
	ldy     #$06
	jsr     staxysp
;
; query->groupByClause = query->groupByClause->nextInList;
;
	ldy     #$13
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$23
	jsr     ldaxidx
	ldy     #$05
	jsr     ldaxidx
	ldy     #$22
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; currentSortingList->nextInList = NULL;
;
	ldy     #$07
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$04
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; yylex_destroy(scanner);
;
L11B9:	ldy     #$0F
	jsr     ldaxysp
	jsr     _yylex_destroy
;
; fclose(queryFile);
;
	ldy     #$11
	jsr     ldaxysp
	jsr     _fclose
;
; }
;
	ldy     #$16
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ tree_insert (__near__ struct qryData *, __near__ struct resultColumnValue *, __near__ __near__ struct resultTree * *)
; ---------------------------------------------------------------


.endproc
