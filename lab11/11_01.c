/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

volatile uint16_t adcResult;
char strtmp[20] = {'\0'};

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
    int i = 0;
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

    //![Simple ADC14 Configure]
    /* Initializing ADC (MCLK/1/1) */
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

    while (1)
    {
        sprintf(strtmp, "%.3lfv\n", adcResult * 3.3 / 16384); /*将从UART_A0中接收到的单个字节以十六进制打印至strtmp中*/
        /*将strtmp字符串通过UART_A0逐个字符发出*/
        for (i = 0; i < strlen(strtmp); i++)
            MAP_UART_transmitData(EUSCI_A0_BASE, strtmp[i]);
        for (i = 200000; i > 0; i--)
            ;
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
        ADC14_toggleConversionTrigger();       /*触发ADC模块开始采样*/
    }
}
