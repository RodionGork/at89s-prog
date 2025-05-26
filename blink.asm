; This code cyclically rotates 1 through
; pins of the port P1

ORG 0

; main subroutine
START:  MOV P1, #10b ; set bit 1 on Port1

REP:
        MOV A, P1    ; read port into accumulator
        RL A         ; rotate left
        JNC CONT     ; if no overflow skip next instruction
        MOV A, #10b  ; reload initial value for port
CONT:
        MOV P1, A    ; store from accumulator to port
        CALL WAIT
        SJMP REP        ; Jump to START

; subroutine for delay
WAIT:   MOV R2, #1      ; Load Register R2 with 10 (0x0A)
WAIT1:  MOV R3, #100    ; Load Register R3 with 10 (0xC8)
WAIT2:  MOV R4, #100    ; Load Register R4 with 10 (0xC8)
WAIT3:  DJNZ R4, WAIT3  ; Decrement R4 till it is 0
        DJNZ R3, WAIT2  ; Decrement R3 till it is 0
        DJNZ R2, WAIT1  ; Decrement R2 till it is 0
        RET
END
