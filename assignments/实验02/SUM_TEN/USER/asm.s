	AREA Reset,DATA,READONLY;创建数据段Reset
	DCD 0X12345678
	DCD   Reset_Handler
	AREA CODE_SEGMET,CODE,READONLY;创建数据段CODE_SEGMENT
	ENTRY
Reset_Handler  proc
	export Reset_Handler    [weak]
	MOV R0,#0;存和的单元
	MOV R1,#10;
start
	ADD R0,R0,R1
	SUBS R1,R1,#1
	BNE start
    NOP
	ENDP
	END