 .align	2
	.global fe25519_mulconst_asm
	.type	fe25519_mulconst_asm, %function

@ R0 = r
@ R1 = x (address)
@ R2 = y (value, < 16 bits)
fe25519_mulconst_asm:
	PUSH {R4-R7,LR}

    @ -- Do the top first for faster reduction

    BIC R4, R4 @ Zero register

    LDRH R7, [R1, #28]
    MUL R7, R2

    LDRH R3, [R1, #30]
    MUL R3, R2
    LSL R5, R3, #16
    LSR R6, R3, #16
    ADD R7, R5
    ADC R6, R4

    LSR R3, R7, #31 @ top bit
    LSL R7, #1
    LSR R7, #1 @ top bit cleared
    PUSH {R7}
    LSL R6, #1
    ADD R6, R3

    MOV R3, #19
    MUL R6, R3

    @ -- Now start with the rest

    LDRH R3, [R1]
    MUL R3, R2
    ADD R3, R6

    LDRH R4, [R1, #2]
    MUL R4, R2
    LSR R5, R4, #16
    LSL R4, #16
    ADD R3, R4

    LDRH R4, [R1, #4]
    MUL R4, R2
    ADC R4, R5

    LDRH R5, [R1, #6]
    MUL R5, R2
    LSR R6, R5, #16
    LSL R5, #16
    ADD R4, R5

    LDRH R5, [R1, #8]
    MUL R5, R2
    ADC R5, R6

    LDRH R6, [R1, #10]
    MUL R6, R2
    LSR R7, R6, #16
    LSL R6, #16
    ADD R5, R6

    STM R0!, {R3,R4,R5}

    LDRH R3, [R1, #12]
    MUL R3, R2
    ADC R3, R7

    LDRH R4, [R1, #14]
    MUL R4, R2
    LSR R5, R4, #16
    LSL R4, #16
    ADD R3, R4

    LDRH R4, [R1, #16]
    MUL R4, R2
    ADC R4, R5

    LDRH R5, [R1, #18]
    MUL R5, R2
    LSR R6, R5, #16
    LSL R5, #16
    ADD R4, R5

    LDRH R5, [R1, #20]
    MUL R5, R2
    ADC R5, R6

    LDRH R6, [R1, #22]
    MUL R6, R2
    LSR R7, R6, #16
    LSL R6, #16
    ADD R5, R6

    STM R0!, {R3,R4,R5}

    LDRH R3, [R1, #24]
    MUL R3, R2
    ADC R3, R7

    LDRH R4, [R1, #26]
    MUL R4, R2
    LSR R5, R4, #16
    LSL R4, #16
    ADD R3, R4

    POP {R4}
    ADC R4, R5

    STM R0!, {R3,R4}

    POP {R4-R7,PC}
    BX LR
.size	fe25519_mulconst_asm, .-fe25519_mulconst_asm
