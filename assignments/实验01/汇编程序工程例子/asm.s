	AREA Reset,DATA,READONLY  ;ResetΪĬ�ϵ���ڵ�ַ
	DCD 0X12345678            ;�����0x80000000��ʼ���У�
		                      ;��ϵͳ��֮���ĸ��ֽ����ݷָ��˶�ջָ��
							  ;���Զ���0x12345678����۲�
	DCD Reset_Handler         ;����һ����ţ���Ϊ��������
	AREA CODE_SEGMENT,CODE,READONLY
Reset_Handler proc
	export Reset_Handler [weak]
Start
	MOV R1,#1
	MOV R2,#2
	ADD R3,R1,R2
	ENDP
	END