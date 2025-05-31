; Bitwise AND: $32 = $30 & $31

    LDA $30      ; Load value from $30
    AND $31      ; Bitwise AND with value at $31
    STA $32      ; Store result at $32
    BRK          ; End of program

; Example:
; $30 = %10101010
; $31 = %11001100
; After execution, $32 = %10001000