#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

unsigned int count=1;

//延时函数
void MyDelay(unsigned int count)
{
    volatile uint32_t i;
    for(i=1000*count; i>0; i--);
    return ;
}

int main(void)
{
    unsigned i=4;
    
    /* 停用看门狗定时器 */
    MAP_WDT_A_holdTimer();
    
    /* 配置时钟频率MCLK=128k，HSMCLK=128K，SMCLK=16K*/
    MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    MAP_CS_initClockSignal(CS_MCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_HSMCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_8);
    
    //设置看门狗超时时间为2s，即2s内看门狗计数器未清理零则复位LaunchPad，t=WDT/SMCLK=32K/16K=2
    MAP_WDT_A_initWatchdogTimer(WDT_A_CLOCKSOURCE_SMCLK,WDT_A_CLOCKDIVIDER_32K);
    
    //使LED1初始为低电平（常暗）
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
    
    //使LED2中绿色灯初始为低电平（常暗）
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
        
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);  //设置P1.1（s1按键）为输入模式并拉高
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);   //清除P1.1中断 
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);  //使能GPIO1.1中断
    MAP_Interrupt_enableInterrupt(INT_PORT1); //使能PORT1中断
    
    //绿色LED亮暗若干次    
    while (i--)
    {
        GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN1);        //翻转P2.1电平，LED2的绿灯状态改变
        MyDelay(10);           
    }
    MAP_Interrupt_enableMaster();  //开启中断总开关  
    
    MAP_WDT_A_startTimer(); //开启开门狗
       
    while(1)
    {        
        GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN0); //翻转P1.0电平，LED1状态改变
        MAP_WDT_A_clearTimer(); //看门狗计数清零
        MyDelay(count);        
    }
}

void PORT1_IRQHandler(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if(status & GPIO_PIN4)
    {
       count+=10;//增大count值
    }

}
