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
	MOV R3,#0
	ADD R3,R1,R2
start
	ADD R0,R0,R3
	ADD R3,R1,R2
	MOV R1,R2
	MOV R2,R3
	SUBS R4,R3,#10
	BLE start
    NOP
	ENDP
	END