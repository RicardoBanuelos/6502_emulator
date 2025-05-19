; Find the nth prime number
; Input:  A = n (which prime to find)
; Output: Y = nth prime (when BRK is hit)

        *= $0600           ; Start address for Commodore 64, change as needed

; --- Zero page variables ---
n:        !byte 0          ; Input: which prime to find
count:    !byte 0          ; How many primes found so far
candidate:!byte 1          ; Current candidate number (starts at 2)
divisor:  !byte 2          ; Current divisor
is_prime: !byte 1          ; Flag: 1=prime, 0=not prime
tmp:      !byte 0          ; Temp for modulus

; --- Program start ---
start:
        STA n              ; Store input n
        LDA #0
        STA count
next_candidate:
        INC candidate      ; candidate++
        LDA #2
        STA divisor
        LDA #1
        STA is_prime       ; Assume prime

check_divisor:
        LDA divisor
        CMP candidate
        BEQ found_prime    ; If divisor == candidate, candidate is prime

        ; Compute candidate % divisor
        LDA candidate
        JSR mod_divisor
        BEQ not_prime      ; If remainder == 0, not prime

        INC divisor
        JMP check_divisor

not_prime:
        LDA #0
        STA is_prime
        JMP next_candidate

found_prime:
        LDA is_prime
        BEQ next_candidate ; If not prime, try next candidate

        INC count
        LDA count
        CMP n
        BNE next_candidate ; Not nth prime yet

        ; Found nth prime, put in Y and halt
        LDA candidate
        TAY                ; <--- Output in Y instead of X
        BRK

; --- Subroutine: mod_divisor ---
; Input:  A = candidate
;         divisor = divisor
; Output: A = candidate % divisor
mod_divisor:
        STA tmp
mod_loop:
        LDA tmp
        CMP divisor
        BCC mod_done
        SEC
        SBC divisor
        STA tmp
        JMP mod_loop
mod_done:
        LDA tmp
        RTS

; --- End of program ---