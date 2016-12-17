.include "../code2.s"
.export _runCommand
.proc	_runCommand: near


;
; void runCommand(char *string) {
;
	jsr     pushax
;
; }
;
	jsr     incsp2
	jmp     farret


; ---------------------------------------------------------------
; __near__ struct hash4Entry * __near__ getLookupTableEntry (__near__ __near__ unsigned char * *, __near__ __near__ unsigned char * *, __near__ int *, __near__ __near__ function returning void *, int)
; ---------------------------------------------------------------


.endproc
