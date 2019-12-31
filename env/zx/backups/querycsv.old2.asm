;10 CLEAR VAL "46000":RANDOMIZE USR VAL "{BE}23636*256+{BE}23635+70":REM

;10 CLEAR VAL "46000":PRINT"Filename?":INPUT LINE a$:RANDOMIZE USR VAL "49155"

;prevent assembly code from being listed
defb 0x0d
defb 0xff

;ensure bank 0 is selected
ld a, (0x5b5c)  ; Previous value of port
and 0xf8  ; Select bank 0
ld bc, 0x7ffd
di
ld (0x5b5c), a
out (C), a
ei

;load "" code
ld ix,0xc000  ; 14t - memory to load into
ld de,0x30           ; 10t - 613bytes
ld a,255            ; 7t - set type to data
scf             ; 4t - set carry flag for error checking
;; following part replicates the ROM routing so can skip break to basic
inc d     ; 4t - resets zero flag
; 4t - replicate ROM routine
ex af,af'
dec d     ; 4t - restore d
di                        ; 4t - disable interrupts
call 0x0562        ; 17t - call ROM routine LD_BYTES+12 to skip break to basic
ei              ; 4t - re-enable interrupts

;randomize usr 49152
jp c,0xc000    ; 12/7t - if carry then jump to error routine otherwise continue