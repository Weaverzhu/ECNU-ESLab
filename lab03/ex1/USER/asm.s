	AREA Reset,DATA,READONLY
	DCD 0X12345678
	DCD   Reset_Handler
	AREA CODE_SEGMET,CODE,READONLY
Reset_Handler  proc
	export Reset_Handler    [weak]
	LDR R0,=ARRAY          ;
	MOV R1,#0              
	MOV R3,#0
LOOP
	LDR R2,[R0,R1]  ;
    ADD R1,#4        
NEXT
    CMP R2,#0
    BEQ LOOP
    MOV R4,R2
    MOV R2,R2,LSR#8             
    AND R4,#0x000000FF   
    CMP R4,#0x23 ;'#'
    BEQ EXIT
	CMP R4,#0x30 ;'0'
    BLT NEXT
	CMP R4,#0x39 ;'9'
    BGT NEXT

    SUB R4,R4,#0x30
    TST R4,#1
    ADDNE R3,#1
    B NEXT   
EXIT
	NOP
	ENDP
ARRAY DCB "10175102111#"
	END
