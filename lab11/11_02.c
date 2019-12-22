
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define PWM_PERIODS (62500 / 10)

volatile uint16_t adcResult;
char strtmp[20] = {'\0'};

const Timer_A_UpModeConfig TA0 =
    {
        TIMER_A_CLOCKSOURCE_SMCLK,           // 选用SMCLK作为时钟源
        TIMER_A_CLOCKSOURCE_DIVIDER_1,       // 分频系数为1，Timer_A0=SMCLK/1=62.5k
        PWM_PERIODS - 1,                     // 周期时长=PWM_PERIODS/Timer_A0=20ms(注：视觉暂留时间约为100ms至400ms)，因从零开始计数，建议做减一操作；
        TIMER_A_TAIE_INTERRUPT_DISABLE,      // 禁用定时器中断
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE, // 禁用定时器CCR0中断
        TIMER_A_DO_CLEAR                     // 清空数值
};

Timer_A_CompareModeConfig TA0_CCR1_PWM =
    {
        TIMER_A_CAPTURECOMPARE_REGISTER_1,        // CCR1通道
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE, // 禁用CCR中断
        TIMER_A_OUTPUTMODE_RESET_SET,             // 采用复位/置位
        PWM_PERIODS                               // 占空比，此处不用减一，当定时器数值val<PWM_PERIODS时，输出高电平（即0到PWM_PERIODS-1，共PWM_PERIODS）；当val>=PWM_PERIODS时输出低电平
};

Timer_A_CompareModeConfig TA0_CCR2_PWM =
    {
        TIMER_A_CAPTURECOMPARE_REGISTER_2,        // CCR2通道
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE, // 禁用CCR中断
        TIMER_A_OUTPUTMODE_RESET_SET,             // 采用复位/置位
        PWM_PERIODS};

Timer_A_CompareModeConfig TA0_CCR3_PWM =
    {
        TIMER_A_CAPTURECOMPARE_REGISTER_3,        // CCR3通道
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE, // 禁用CCR中断
        TIMER_A_OUTPUTMODE_RESET_SET,             // 采用复位/置位
        PWM_PERIODS};

const eUSCI_UART_Config uartConfig =
    {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,               //选用SMCLK（1M）时钟源
        26,                                           // BRDIV = 26 ，clockPrescalar时钟分频系数
        1,                                            // UCxBRF = 8  firstModReg （BRDIV、UCxBRF、 UCxBRS和SMCLK，用于设置串口波特率）
        0,                                            // UCxBRS = 17 secondModReg
        EUSCI_A_UART_NO_PARITY,                       // 校验位None
        EUSCI_A_UART_LSB_FIRST,                       // 低位优先，小端模式
        EUSCI_A_UART_ONE_STOP_BIT,                    // 停止位1位
        EUSCI_A_UART_MODE,                            // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION // 设置为过采样，该数值为1
};

int main(void)
{
    uint8_t port_mapping[] =
        {
            PM_TA0CCR1A, //映射至Timer_A0_CCR1，与Pin0对应
            PM_TA0CCR2A, //映射至Timer_A0_CCR2，与Pin1对应
            PM_TA0CCR3A, //映射至Timer_A0_CCR3，与Pin2对应
            PM_NONE,     //不映射，映射为空
            PM_NONE,
            PM_NONE,
            PM_NONE,
            PM_NONE};

    /* 停用开门狗 */
    MAP_WDT_A_holdTimer();
    MAP_FPU_enableModule();
    MAP_CS_setDCOFrequency(4000000);
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);

    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);

    MAP_UART_enableModule(EUSCI_A0_BASE);

    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_4, 0);

    ADC14_configureSingleSampleMode(ADC_MEM0, true);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A0, 0);
    ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);
    ADC14_enableInterrupt(ADC_INT0);

    Interrupt_enableInterrupt(INT_ADC14); 
    Interrupt_enableMaster();  
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();

    MAP_PMAP_configurePorts((const uint8_t *)port_mapping, PMAP_P2MAP, 1, PMAP_DISABLE_RECONFIGURATION);
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_Timer_A_configureUpMode(TIMER_A0_BASE, &TA0);

    MAP_Interrupt_enableInterrupt(INT_TA1_0); 
    MAP_Interrupt_enableMaster();   

    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    while (1)
    {
    }
}

/* 当转换完成并放置到ADC_MEM0时，此中断发生。 */
void ADC14_IRQHandler(void)
{
    uint64_t status;
    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    if (status & ADC_INT0)
    {
        adcResult = ADC14_getResult(ADC_MEM0); /*将ADC_MEM0中的值赋值给adcResult*/
        //adcResult = adcResult*1.0/16384;
        float c = adcResult * 1.0 / 16384;

        TA0_CCR1_PWM.compareValue = PWM_PERIODS * c;
        MAP_Timer_A_initCompare(TIMER_A0_BASE, &TA0_CCR1_PWM); /*使用随机数设置TimerA0 CCR1（连接LED2的红灯）的PWM占空比*/

        TA0_CCR2_PWM.compareValue = PWM_PERIODS * c;
        MAP_Timer_A_initCompare(TIMER_A0_BASE, &TA0_CCR2_PWM); /*使用随机数设置TimerA0 CCR2（连接LED2的绿灯）的PWM占空比*/

        TA0_CCR3_PWM.compareValue = PWM_PERIODS * c;
        MAP_Timer_A_initCompare(TIMER_A0_BASE, &TA0_CCR3_PWM); /*使用随机数设置TimerA0 CCR3（连接LED2的蓝灯）的PWM占空比*/

        ADC14_toggleConversionTrigger(); /*触发ADC模块开始采样*/
    }
}
