	AREA Reset,DATA,READONLY
	DCD 0X12345678
	DCD   Reset_Handler
	AREA CODE_SEGMET,CODE,READONLY
Reset_Handler  proc
	export Reset_Handler    [weak]
    MOV R0 0xAB ; input data, 8 bit int
    MOV R1,R0
    MOV R2,#0 ; store the number of bit of the input number
NUMBER_BIT
    

    LSRS R1,R1,#1
    BGT NUMBER_BIT




    NOP
	ENDP
ARRAY DCD 5,7,5,4,2,1,5,7,6,4,3,1,4,8,4,3,2,4,8,7,6,5
FIX DCD 3,5,6,7,10,11,12
	END
		

