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
    CMP R4,#0x24 ;'$'
    BEQ EXIT
	CMP R4,#0x61 ;'a'
    BLT NEXT
	CMP R4,#0x7A ;'z'
    BGT NEXT
    ADD R3,#1    
    B NEXT   
EXIT
	NOP
	ENDP
ARRAY DCB "Hello az 123$"
	END
