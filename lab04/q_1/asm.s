	AREA Reset,DATA,READONLY
	DCD 0X12345678
	DCD   Reset_Handler
	AREA CODE_SEGMET,CODE,READONLY
Reset_Handler  proc
	export Reset_Handler    [weak]
    ;MOV R0,#0xA     ;用例1输入值0xA，结果0x15
    ;MOV R0,#0xBA     ;用例2输入值0xAB，结果0x174
    ;MOV R0,#0xCBA   ;用例3输入值0xABC，结果0x1974
    MOV R0,#0x7654  ;用例4输入值0x7654，结果0xECA9
    MOV R1,R0       ;R1存储结果
    MOV R2,#0       ;R2存储1的个数
    MOV R3,#0       ;R3作为临时寄存器，最多只能使用R0、R1、R2、R3及xPSR寄存器
    ;//---------------请在以下空白区域内编写代码------------//

LOOPI
	AND R3,R0,#1
	ADD R2,R2,R3
	LSRS R0,R0,#1
	BGT LOOPI
	AND R3,R2,#1
	EOR R1,R3,#1
    ;//---------------请在以上空白区域内编写代码------------//
    ;最终结果存入R1
    NOP         ;请直接跳转至此
	ENDP
	END
