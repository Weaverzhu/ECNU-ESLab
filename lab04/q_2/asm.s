	AREA Reset,DATA,READONLY
	DCD 0X12345678
	DCD   Reset_Handler
	AREA CODE_SEGMET,CODE,READONLY
Reset_Handler  proc
	export Reset_Handler    [weak]
    MOV R0,0xAB ; input data, you can input whatever int you want
    MOV R1,R0
    MOV R2,#0 ; store the number of bit of the input number, this loop is to determine k
NUMBER_BIT
	ADD R2,R2,#1
    LSRS R1,R1,#1
    BGT NUMBER_BIT
	MOV R3,#0 ; R3 = r, R2 = k, 2^r-r-1 >= K, R4 for tmp, this loop is to determine r
LOOPRK
	ADD R3,R3,#1
	MOV R4, #1
	LSL R4, R4, R3
	SUB R4,R4,R3
	SUB R4,R4,#1
	SUBS R4,R4,R2
	BLT LOOPRK

	; dump k bit int to k+r bit Hamming Code in R1, R7,R8,R9,R10 for tmp
	MOV R1,#0
	MOV R4, #1 ;R4 stands for the next 2^n
	MOV R5, #1 ;R5 stands for the current bit in hamming code
	MOV R6, #0 ;R5 stands for the current bit in original int
DUMP
	CMP R5,R4
	LSLEQ R4,R4,#1 ; should prepare for the next parity code, instead of setting information bit
	MOV R7,#1
	LSL R7,R7,R6 ; R7 = 2^{R6} & R0, get the R6-th bit in original int
	ANDS R7,R7,R0
	MOV R7,#0
	MOVNE R7,#1 ; R6 = 2^{R5-1}, prepare to set the R5-th bit in hamming code
	SUB R5,R5,#1
	LSL R7,R7,R5
	ADD R5,R5,#1
	CMP R4,R5,LSL #1
	BEQ CONT
	EOR R1,R1,R7 ; copy the $R6-th bit in original int to the $R5-th bit in hamming-code
	ADD R6,R6,#1
	CBZ R7,CONT
	MOV R8,R5
	MOV R10,#0
LOOP
	ANDS R7,R8,#1
	MOV R9,#0
	MOVNE R9,#1
	LSLS R9,R9,R10
	MOV R7,#1
	SUBNE R9,R9,#1
	LSLNE R9,R7,R9
	EOR R1,R1,R9
	
	LSRS R8,R8,#1
	ADD R10,R10,#1
	BNE LOOP
CONT
	ADD R5,R5,#1
	CMP R6,R2
	BLT DUMP
	
	
	MOV R11,R1 ; backup the parity code
	MOV R8,#1
	EOR R0,R8,LSL #3; change the 3-th bit of R0
	
	; same again, calc the parity code for new hamming code
	; dump k bit int to k+r bit Hamming Code in R1, R7,R8,R9,R10 for tmp
	MOV R1,#0
	MOV R4, #1 ;R4 stands for the next 2^n
	MOV R5, #1 ;R5 stands for the current bit in hamming code
	MOV R6, #0 ;R5 stands for the current bit in original int
DUMP2
	CMP R5,R4
	LSLEQ R4,R4,#1 ; should prepare for the next parity code, instead of setting information bit
	MOV R7,#1
	LSL R7,R7,R6 ; R7 = 2^{R6} & R0, get the R6-th bit in original int
	ANDS R7,R7,R0
	MOV R7,#0
	MOVNE R7,#1 ; R6 = 2^{R5-1}, prepare to set the R5-th bit in hamming code
	SUB R5,R5,#1
	LSL R7,R7,R5
	ADD R5,R5,#1
	CMP R4,R5,LSL #1
	BEQ CONT2
	EOR R1,R1,R7 ; copy the $R6-th bit in original int to the $R5-th bit in hamming-code
	ADD R6,R6,#1
	CBZ R7,CONT2
	MOV R8,R5
	MOV R10,#0
LOOP2
	ANDS R7,R8,#1
	MOV R9,#0
	MOVNE R9,#1
	LSLS R9,R9,R10
	MOV R7,#1
	SUBNE R9,R9,#1
	LSLNE R9,R7,R9
	EOR R1,R1,R9
	
	LSRS R8,R8,#1
	ADD R10,R10,#1
	BNE LOOP2
CONT2
	ADD R5,R5,#1
	CMP R6,R2
	BLT DUMP2

	MOV R4,#0; R4 store the position of the error bit
	MOV R5,#0
FLOOP
	MOV R6,#1
	LSL R6,R6,R5
	AND R7,R1,R6
	AND R8,R11,R6
	EORS R6,R7,R8
	ADDNE R4,R4,R5
	CMP R5,R3
	ADD R5,R5,#1
	BLT FLOOP

	MOV R5,#1
	LSL R4,R5,R4
	EOR R0,R0,R4 ; now R0 should be corrected

    NOP
	ENDP
	END
		

