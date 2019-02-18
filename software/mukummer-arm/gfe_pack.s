 .align	2
	.global gfe_pack
	.type	gfe_pack, %function

@ R0 = r
@ R1 = x
gfe_pack:
	push {r4-r5,lr}

    LDM R1!, {R2, R3, R4, R5}

    LSR R1, R5, #31 @ top bit
    LSL R5, #1
    LSR R5, #1

    ADD R2, R1
    BIC R1, R1
    ADC R3, R1
    ADC R4, R1
    ADC R5, R1

    LSR R1, R5, #31 @ top bit
    LSL R5, #1
    LSR R5, #1
    ADD R2, R1

    STM R0!, {R2, R3, R4, R5}

	pop {r4-r5,pc}
	bx	lr
.size	gfe_pack, .-gfe_pack
