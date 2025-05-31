; Bitwise: $32 = NOT($30 XOR $31)

    LDA $30      ; Load value from $30
    EOR $31      ; XOR with value at $31
    EOR #$FF     ; Invert all bits (NOT)
    STA $32      ; Store result at $32
    BRK          ; End of program

; Example:
; $30 = %10101010
; $31 = %11001100
; $30 XOR $31 = %01100110
; NOT(%01100110) = %10011001
; After execution, $32 = %10011001