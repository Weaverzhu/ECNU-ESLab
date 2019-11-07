#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

//延时函数
void MyDelay(unsigned int count)
{
    volatile uint32_t i;
    for(i=10000*count; i>0; i--);
    return ;
}

int main(void)
{  
		volatile uint32_t i;
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Set P1.0 to output direction
    GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);
	

	//*-----------在此下区域修改-----------*//
	//首先让红色LED常亮（数值为600000），常暗（数值为300000）	
	//然后周期性的让红色LED常亮（数值为100000），常暗（数值为50000）
	GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0);
	MyDelay(60);
	GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0);
	MyDelay(30);

	while (1) {
			GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0);
			MyDelay(10);
			GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0);
			MyDelay(50);
	}
	
      
	//*-----------在此上区域修改-----------*//
}
