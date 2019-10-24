	AREA Reset,DATA,READONLY  ;Reset为默认的入口地址
	DCD 0X12345678            ;程序从0x80000000开始运行，
		                      ;而系统将之后四个字节内容分给了堆栈指针
							  ;所以定义0x12345678方便观察
	DCD Reset_Handler         ;定义一个标号，作为后面的输出
	AREA CODE_SEGMENT,CODE,READONLY
Reset_Handler proc
	export Reset_Handler [weak]
Start
	MOV R1,#1
	MOV R2,#2
	ADD R3,R1,R2
	ENDP
	END