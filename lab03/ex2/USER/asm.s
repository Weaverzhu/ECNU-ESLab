	AREA Reset,DATA,READONLY
	DCD 0X12345678
	DCD   Reset_Handler
	AREA CODE_SEGMET,CODE,READONLY
Reset_Handler  proc
	export Reset_Handler    [weak]
	MOV R0,#0x20000000
	LDR R4,=ARRAY
	MOV R8,#5
	MOV R1,#0
	MOV R2,R8
COPY
	LSL R5,R1,#2
	LDR R6,[R4,R5]
	STR R6,[R0,R5]
	ADD R1,R1,#1
	CMP R1,R2
	BNE COPY
	
		
    MOV R1,#0
	SUB R2,R8,#1
LOOPI
	SUB R3,R8,#1
LOOPJ
    LSL R4,R3,#2
	ADD R4,R0,R4
    LDR R5,[R4]
    LDR R6,[R4,#-4]

    CMP R5,R6
    BGT NOSWAP
    MOV R7,R5
    MOV R5,R6
    MOV R6,R7
	STR R5,[R4]
	STR R6,[R4,#-4]
NOSWAP
    SUB R3,R3,#1
    SUBS R7,R3,R1
    BGT LOOPJ


    ADDS R1,R1,#1
    CMP R1,R2
    BLT LOOPI
EXIT
	NOP
	ENDP
ARRAY DCD 3,2,4,5,1,6
	END
