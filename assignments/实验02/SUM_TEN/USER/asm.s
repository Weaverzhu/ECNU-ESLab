	AREA Reset,DATA,READONLY;�������ݶ�Reset
	DCD 0X12345678
	DCD   Reset_Handler
	AREA CODE_SEGMET,CODE,READONLY;�������ݶ�CODE_SEGMENT
	ENTRY
Reset_Handler  proc
	export Reset_Handler    [weak]
	MOV R0,#0;��͵ĵ�Ԫ
	MOV R1,#10;
start
	ADD R0,R0,R1
	SUBS R1,R1,#1
	BNE start
    NOP
	ENDP
	END