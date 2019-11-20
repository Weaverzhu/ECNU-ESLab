// #define __MSP432P401R__
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

int timer, color, speed;

#define Green 0
#define Yellow 1
#define Red 2
#define SIZE 3

#define CLOCK CS_DCOCLK_SELECT

const int Delay[] = {
    600000, 300000, 1200000
};

const uint32_t Frequency[] = {
    CS_CLOCK_DIVIDER_1,
    CS_CLOCK_DIVIDER_2,
    CS_CLOCK_DIVIDER_4,
    CS_CLOCK_DIVIDER_8,
    CS_CLOCK_DIVIDER_16,
    CS_CLOCK_DIVIDER_32,
    CS_CLOCK_DIVIDER_64
};

void changecolor(int color) {
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
    switch (color)
    {
    case Green:
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
        break;
    case Yellow:
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
        break;
    case Red:
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
        break;
    
    default:
        break;
    }
}

int main(void)
{
    MAP_WDT_A_holdTimer();  /* 停用看门狗定时器 */
		
	/*
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);   
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    // MAP_CS_initClockSignal(CS_MCLK, CLOCK, Frequency[speed]); 
	*/
	MAP_PCM_setCoreVoltageLevel(PCM_VCORE0);    
    FlashCtl_setWaitState(FLASH_BANK0, 1);
    FlashCtl_setWaitState(FLASH_BANK1, 1);
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    speed = 0;
    MAP_CS_initClockSignal(CS_MCLK, CLOCK, Frequency[speed]); 

     
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);  //设置P1.1（s1按键）为输入模式并拉高  
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);  //设置P1.4（s2按键）为输入模式并拉高
    
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);  //使能GPIO1.1中断
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);   //清除P1.1中断 
    
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);  //使能GPIO1.4中断
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);   //清除P1.1中断 
    
    MAP_Interrupt_enableInterrupt(INT_PORT1); //使能PORT1中断
    MAP_Interrupt_enableMaster();  //开启中断总开关  
    
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
    
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
    
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN2);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
    
    color = Green;
    timer = Delay[color];
    changecolor(color);
    while (1) {

        while (timer--);
        color = (color + 1) % SIZE;
        changecolor(color);
        timer = Delay[color];
    }
}

void PORT1_IRQHandler(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if (status & GPIO_PIN1) {
        speed = (speed + 1) % 7;
        MAP_CS_initClockSignal(CS_MCLK, CLOCK, Frequency[speed]);
    }
    if (status & GPIO_PIN4) {
        if (color != Green) {
            color = Green;
            changecolor(color);
            timer = Delay[color];
        }
    }
}
