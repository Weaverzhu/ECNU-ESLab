	AREA Reset,DATA,READONLY;?????Reset
	DCD 0X12345678
	DCD   Reset_Handler
	AREA CODE_SEGMET,CODE,READONLY;?????CODE_SEGMENT
	ENTRY
Reset_Handler  proc
	export Reset_Handler    [weak]
	MOV R0,#0;?????
	MOV R1,#10;
start
	ANDS R2,R1,#1
    BNE cont
    ADDS R0,R0,R1    
cont
    SUBS R1,R1,#1

	BNE start
    NOP
	ENDP
	END