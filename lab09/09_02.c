/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#define PERIODS (62500)
#define PWM_PERIODS (62500/50)
#define PWM_STEP (PWM_PERIODS/10)

/* Timer_A UpMode Configuration Parameter */
const Timer_A_UpModeConfig TA1 =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // 选用SMCLK作为时钟源
        TIMER_A_CLOCKSOURCE_DIVIDER_1,          // 分频系数为1，Timer_A1=SMCLK/1=62.5k
        PERIODS*3-1,                            // 周期时长=(PERIODS*3)/Timer_A1=3s，因从零开始计数，建议做减一操作；
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // 禁用定时器中断
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // 使能定时器CCR0中断
        TIMER_A_DO_CLEAR                        // 清空数值
};

Timer_A_PWMConfig TA0_PWM =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // 选用SMCLK作为时钟源
        TIMER_A_CLOCKSOURCE_DIVIDER_1,          // 分频系数为1，Timer_A0=SMCLK/1=62.5k
        PWM_PERIODS-1,                          // 周期时长=PWM_PERIODS/Timer_A0=20ms(注：视觉暂留时间约为100ms至400ms)，因从零开始计数，建议做减一操作；
        TIMER_A_CAPTURECOMPARE_REGISTER_2,      // 选用CCR2通道  
        TIMER_A_OUTPUTMODE_RESET_SET,           // 采用复位/置位
        PWM_STEP                                // 占空比，此处不用减一，当定时器数值val<PWM_PERIODS时，输出高电平（即0到PWM_PERIODS-1，共PWM_PERIODS）；当val>=PWM_PERIODS时输出低电平
};
    
int main(void)
{
    uint8_t port_mapping[]=
    {
        PM_NONE,     //不映射，映射为空
        PM_TA0CCR2A, //映射至Timer_A0_CCR2，与Pin1对应
        PM_NONE,
        PM_NONE,
        PM_NONE,
        PM_NONE,
        PM_NONE,
        PM_NONE
    };    

    /* 停用看门狗*/
    MAP_WDT_A_holdTimer();

    //![Simple FPU Config]   
    MAP_FPU_enableModule();/*启用FPU加快DCO频率计算，注：DCO是内部数字控制振荡器，默认是3M频率*/    
    MAP_CS_setDCOFrequency(1000000);/* 设置DCO频率为指定频率，此处DCO=1M*/
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);/*设置MCLK（主时钟，可用于CPU主频等），MCLK=DCO频率/时钟分频系数，此处MCLK=DCO=1M*/
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);/*设置HSMCLK（子系统主时钟），HSMCLK=DCO频率/时钟分频系数，此处HSMCLK=DCO=1M*/
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_16);/*设置SMCLK（低速子系统主时钟，可用TimerA频率），SMCLK=DCO频率/时钟分频系数，此处SMCLK=DCO/16=62.5K*/
    //![Simple FPU Config] 
    
    /*将TimerA0的CCR1、CCR2及CCR3，和P2.0、P2.1及P2.2一一映射，并使能*/
    MAP_PMAP_configurePorts((const uint8_t *)port_mapping,PMAP_P2MAP/*端口2*/,1/*0~7*/,PMAP_DISABLE_RECONFIGURATION);
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN0|GPIO_PIN1|GPIO_PIN2,GPIO_PRIMARY_MODULE_FUNCTION); 
    
    /*TimerA1使用SMCLK作为时钟源，启用CCR0中断，定时周期为3s，增计数模式*/
    MAP_Timer_A_configureUpMode(TIMER_A1_BASE, &TA1);  
    MAP_Interrupt_enableInterrupt(INT_TA1_0);  /*使能TimerA1 CCR0中断*/   
    MAP_Interrupt_enableMaster(); /*使能中断总开关*/
      
    MAP_Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE); /*以增计数模式启动TimerA1*/
    while (1);
}

void TA1_0_IRQHandler(void)
{
    //LED2的绿灯先由暗变亮再由亮变暗，周期性循环往复。
    static int sign=1;
    TA0_PWM.dutyCycle += sign*PWM_STEP;
    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &TA0_PWM); /*设置TimerA0 CCR2（连接LED2的绿灯）的PWM占空比*/ 
 
    if (TA0_PWM.dutyCycle==PWM_PERIODS) sign=-1;
    else if (TA0_PWM.dutyCycle==PWM_STEP) sign=1;          
    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);/*清空TimerA1 CCR0中断标志*/
}
