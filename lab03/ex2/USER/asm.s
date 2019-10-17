	AREA Reset,DATA,READONLY;?????Reset
	DCD 0X12345678
	DCD   Reset_Handler
	AREA CODE_SEGMET,CODE,READONLY;?????CODE_SEGMENT
	ENTRY
Reset_Handler  proc
	export Reset_Handler    [weak]
	
    MOV R0,#0x20000000
    LDR R1,=NUM
    ;LDRH R1,#0X0600
	LDRH R2,[R1]
	MOV R6,#10
	
LOOP
	UDIV R3,R2,R6
	MUL R4,R3,R6
	SUB R4,R2,R4
	ADD R4,R4,#0x30
	STRB R4,[R0,R5]
	ADD R5,R5,#1
	MOV R2,R3
	CMP R2,#0
	BGT LOOP
	
    NOP
    ENDP

NUM DCW 0x1234
    END
