/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

int main(void)
{    
    MAP_WDT_A_holdTimer();/* 停用开门狗 */
    
    //![Simple FPU Config]   
    MAP_FPU_enableModule();/*启用FPU加快DCO频率计算，注：DCO是内部数字控制振荡器，默认是3M频率*/    
    MAP_CS_setDCOFrequency(10000000);/* 设置DCO频率为指定频率，此处DCO=10M*/
    //![Simple FPU Config]
        
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);/*设置MCLK（主时钟频率），MCLK=DCO频率/时钟分频系数，此处MCLK=DCO=1M*/
        
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);/* 配置P1.0为输出模式 */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);/*拉高P1.0引脚电压*/
    
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);/* 配置P2.1为输出模式 */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);/*拉高P2.1引脚电压*/
            
    /* 配置Timer32的第一个计数器为32位模式，周期模式，定时器频率=MCLK/定时器分频系数 此处第一个计数器频率=MCLK=1M */
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,TIMER32_PERIODIC_MODE);
    
    /* 配置Timer32的第二个计数器为32位模式，周期模式，定时器频率=MCLK/定时器分频系数 此处第二个计数器频率=MCLK=1M */
    MAP_Timer32_initModule(TIMER32_1_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,TIMER32_PERIODIC_MODE);
    
    /* 使能Timer32第一个计数器中断开关*/
    MAP_Interrupt_enableInterrupt(INT_T32_INT1);    
    MAP_Timer32_enableInterrupt(TIMER32_0_BASE);

    /* 使能Timer32第二个计数器中断开关*/
    MAP_Interrupt_enableInterrupt(INT_T32_INT2);    
    MAP_Timer32_enableInterrupt(TIMER32_1_BASE);
    
    MAP_Interrupt_enableMaster();/*使能中断总开关*/
    
    MAP_Timer32_setCount(TIMER32_0_BASE,10000000); /*设置Timer32第一个计数器计数周期，此处 4M/1M=4s*/    
    MAP_Timer32_setCount(TIMER32_1_BASE,10000000); /*设置Timer32第二个计数器计数周期，此处 1M/1M=1s*/    
    
    MAP_Timer32_startTimer(TIMER32_0_BASE, false);/*Timer32第一个计数器以周期性模式开始计数*/
    MAP_Timer32_startTimer(TIMER32_1_BASE, false);/*Timer32第二个计数器以周期性模式开始计数*/
    
    /*打结，因无其他操作，此处为空循环*/
    while (1);    
}

/* Timer32第一个计数器中断函数*/
void T32_INT1_IRQHandler(void)
{
    MAP_Timer32_clearInterruptFlag(TIMER32_0_BASE); /*清除Timer32第一个计数器中断标志*/  
    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0); /*翻转P1.0的电平*/
}

/* Timer32第二个计数器中断函数*/
void T32_INT2_IRQHandler(void)
{
    MAP_Timer32_clearInterruptFlag(TIMER32_1_BASE); /*清除Timer32第二个计数器中断标志*/  
    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN1); /*翻转P2.1的电平*/
}
