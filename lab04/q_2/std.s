	AREA Reset,DATA,READONLY
	DCD 0X12345678
	DCD   Reset_Handler
	AREA CODE_SEGMET,CODE,READONLY
Reset_Handler  proc
	export Reset_Handler    [weak]
	MOV R4,#0xab;
	LDR R6,=ARRAY
	MOV R0,#3
	MOV R1,#0x20000000
LOOPJ
	MOV R2,#0
	LDR R7,[R6]
	ADD R6,R6,#4
LOOPI
	LDR R5,[R6]
	ADD R6,R6,#4
	SUB R5,R5,#1
	MOV R8,#1
	LSL R8,R8,R5
	ANDS R8,R8,R4
	EORGT R2,R2,#1
	SUBS R7,R7,#1
	BGT LOOPI
	
	STR R2,[R1,R0,LSL #2]
	SUBS R0,R0,#1
	BGE LOOPJ
	
	MOV R1,#1
	MOV R2,#2
	LSL R2,R1,R2
	
	EOR R4,R4,R2
	; change the 3rd bit of R4
	
	LDR R6,=ARRAY
	MOV R0,#3
	MOV R3,#0
	MOV R1,#0X20000000
LOOPK
	MOV R2,#0
	LDR R7,[R6]
	ADD R6,R6,#4
LOOPL
	LDR R5,[R6]
	ADD R6,R6,#4
	SUB R5,R5,#1
	MOV R8,#1
	LSL R8,R8,R5
	ANDS R8,R8,R4
	EORGT R2,R2,#1
	SUBS R7,R7,#1
	BGT LOOPL
	
	LDR R5,[R1,R0,LSL #2]
	EOR R7,R5,R2
	MOV R5,#3
	SUB R5,R5,R0
	MOV R8,#1
	LSL R8,R8,R5
	CMP R7,#0
	EORNE R3,R3,R8
	SUBS R0,R0,#1
	BGE LOOPK
	
	MOV R0,#0
	LDR R1,=FIX
LOOP
	LDR R2,[R1]
	ADD R1,R1,#4
	CMP R3,R2
	MOV R5,R0
	BEQ QUIT
	CMP R0,#8
	ADD R0,R0,#1
	BLE LOOP
QUIT
	
    NOP         ;请直接跳转至此
	ENDP
ARRAY DCD 5,7,5,4,2,1,5,7,6,4,3,1,4,8,4,3,2,4,8,7,6,5
FIX DCD 3,5,6,7,10,11,12
	END
		

