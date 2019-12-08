/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Application Defines  */
#define TIMER_PERIOD (62500-1) //定时周期

/* Timer_A UpMode Configuration Parameter */
const Timer_A_UpModeConfig upConfig =
{
    TIMER_A_CLOCKSOURCE_SMCLK,              // 选用SMCLK作为时钟源
    TIMER_A_CLOCKSOURCE_DIVIDER_1,         // 分频系数为1，TimerA_CLK=SMCLK/DIV=62.5K/1=62.5K
    TIMER_PERIOD,                           // 设置CCR0的数值val为(TimerA_CLK-1),因为从零开始计数，所以周期=（val+1）/TimerA_CLK=1s
    TIMER_A_TAIE_INTERRUPT_DISABLE,         // 禁用定时器中断
    TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // 使能CCR0中断
    TIMER_A_DO_CLEAR                        // 清空数值
};

int main(void)
{
    MAP_WDT_A_holdTimer();/* 停用开门狗 */
    
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); //配置P1.0引脚为输出模式
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);  //拉低P1.0引脚电压
    
    //![Simple FPU Config]   
    MAP_FPU_enableModule();/*启用FPU加快DCO频率计算，注：DCO是内部数字控制振荡器，默认是3M频率*/    
    MAP_CS_setDCOFrequency(1000000);/* 设置DCO频率为指定频率，此处DCO=1M*/
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);/*设置MCLK（主时钟，可用于CPU主频等），MCLK=DCO频率/时钟分频系数，此处MCLK=DCO=1M*/
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);/*设置HSMCLK（子系统主时钟），HSMCLK=DCO频率/时钟分频系数，此处HSMCLK=DCO=1M*/
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_16);/*设置SMCLK（低速子系统主时钟，可用TimerA频率），SMCLK=DCO频率/时钟分频系数，此处SMCLK=DCO/16=62.5K*/
    //![Simple FPU Config]
       
    MAP_Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);  /*配置TimerA0为增计数模式，并使用upConfig配置具体参数*/
    MAP_Interrupt_enableInterrupt(INT_TA0_0);  /*使能TimerA0 CCR0中断*/   
    MAP_Interrupt_enableMaster(); /*使能中断总开关*/
    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE); /*以增计数模式启动TimerA0*/    
  
    while (1);    
}

/*TimerA0 CCR0中断函数*/
void TA0_0_IRQHandler(void) 
{    
    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); /*翻转P1.0电平*/   
    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0); /*清空TimerA0 CCR0中断标志*/
}
