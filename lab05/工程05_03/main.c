#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

//延时函数
void MyDelay(unsigned int count)
{
    volatile uint32_t i;
    for(i=1000*count; i>0; i--);
    return ;
}

int main(void)
{
    //*-----------在此下区域可定义变量-----------*//
    unsigned i=4;
    unsigned int count=1, fn, fn1 = 1, fn2 = 0;
    
    
    //*-----------在此上区域可定义变量-----------*//
    
    /* 停用看门狗定时器 */
    MAP_WDT_A_holdTimer();
    
    /* 配置时钟频率MCLK=128k，HSMCLK=128K，SMCLK=16K*/
    MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    MAP_CS_initClockSignal(CS_MCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_HSMCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_8);   //SMCLK=MCLK/8
    
    //设置看门狗超时时间为2s，即2s内看门狗计数器未清理零则复位LaunchPad，t=WDT/SMCLK=32K/16K=2
    MAP_WDT_A_initWatchdogTimer(WDT_A_CLOCKSOURCE_SMCLK,WDT_A_CLOCKDIVIDER_32K);
    
    //使LED1初始为低电平（常暗）
    GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);
    
    //使LED2中绿色灯初始为低电平（常暗）
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
    
    //设置P1.1（s1按键）为输入模式并拉高
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    
    //绿色LED亮暗若干次
    while (i--)
    {
        GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN1);         //翻转P2.1绿灯电平
        MyDelay(10);           
    }
        
    MAP_WDT_A_startTimer();
    while(1)
    {
        //*-----------在此下区域修改-----------*//
        //检测到P1.1按键被按下
        if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1)==GPIO_INPUT_PIN_LOW)
        {
            count = fn = fn1 + fn2;
						fn2 = fn1;
						fn1 = fn;
        }
        //*-----------在此上区域修改-----------*//
        
        GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0); //翻转P1.0电平，翻转红色LED1电平
        MAP_WDT_A_clearTimer(); //看门狗计数清零
        MyDelay(count);
    }
}
