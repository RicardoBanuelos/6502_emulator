; Multiplies the values at $30 and $31, stores result at $32
; $33 is used as a counter

    LDA #0        ; Clear accumulator
    STA $32       ; Store 0 at $32 (result)
    LDA $31       ; Load multiplier
    STA $33       ; Store multiplier in $33 (counter)

LoopStart:
    LDA $33       ; Load counter
    BEQ Done      ; If counter is zero, done
    LDA $32       ; Load current result
    CLC
    ADC $30       ; Add multiplicand
    STA $32       ; Store new result
    DEC $33       ; Decrement counter
    JMP LoopStart ; Repeat

Done:
    BRK           ; End of program

; Example data:
; $30 = 4   ; Multiplicand
; $31 = 3   ; Multiplier
; After execution, $32 = 12