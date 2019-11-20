#define __MSP432P401R__

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <string.h>

unsigned char condition=0; 

int main(void)
{
    
    MAP_WDT_A_holdTimer();  /* 停用看门狗定时器 */
    
    /*为了快速编程.设置 MCLK 为 48 MHz*/    
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);    
    FlashCtl_setWaitState(FLASH_BANK0, 1);
    FlashCtl_setWaitState(FLASH_BANK1, 1);
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
     
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);  //设置P1.1（s1按键）为输入模式并拉高  
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);  //使能GPIO1.1中断
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);   //清除P1.1中断 
    
    
    MAP_Interrupt_enableInterrupt(INT_PORT1); //使能PORT1中断
    MAP_Interrupt_enableMaster();  //开启中断总开关  
    
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
    
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
    
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN2);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);

		while(1){}

}

void PORT1_IRQHandler(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if (status & GPIO_PIN1) {
        GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);
        GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
        GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }

}
