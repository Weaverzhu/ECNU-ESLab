	AREA Reset,DATA,READONLY;?????Reset
	DCD 0X12345678
	DCD   Reset_Handler
	AREA CODE_SEGMET,CODE,READONLY;?????CODE_SEGMENT
	ENTRY
Reset_Handler  proc
	export Reset_Handler    [weak]
	MOV R1,#0
	MOV R2,#1
	MOV R0,#0
	ADD R0,R0,R1
	ADD R0,R0,R2
	MOV R4,#8;
start
	ADD R3,R1,R2
	ADD R0,R0,R3
	MOV R1,R2
	MOV R2,R3
	SUBS R4,R4,#1
	BNE start
    NOP
	ENDP
	END