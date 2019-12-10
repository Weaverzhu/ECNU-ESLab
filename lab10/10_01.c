#define __MSP432P401R__

///******************************************************************************
// *
// *               MSP432P401
// *             -----------------
// *            |                 |
// *            |                 |
// *            |                 |
// *       RST -|     P1.3/UCA0TXD|----> PC 
// *            |                 |
// *            |                 |
// *            |     P1.2/UCA0RXD|<---- PC
// *            |                 |
// *
// *******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/*
http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
*/
const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          //选用SMCLK（1M）时钟源
        6,                                       // BRDIV = 6 ，clockPrescalar时钟分频系数 
        8,                                       // UCxBRF = 8  firstModReg （BRDIV、UCxBRF、 UCxBRS和SMCLK，用于设置串口波特率）
        17,                                      // UCxBRS = 17 secondModReg
        EUSCI_A_UART_NO_PARITY,                  // 校验位None
        EUSCI_A_UART_LSB_FIRST,                  // 低位优先，小端模式
        EUSCI_A_UART_ONE_STOP_BIT,               // 停止位1位
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // 设置为过采样，该数值为1
};

int main(void)
{  
    /* 停用开门狗 */
    MAP_WDT_A_holdTimer();
    
    //![Simple FPU Config]   
    MAP_FPU_enableModule();/*启用FPU加快DCO频率计算，注：DCO是内部数字控制振荡器，默认是3M频率*/    
    MAP_CS_setDCOFrequency(4000000);/* 设置DCO频率为指定频率，此处DCO=4M*/
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);/*设置MCLK（主时钟，可用于CPU主频等），MCLK=DCO频率/时钟分频系数，此处MCLK=DCO=4M*/
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_4);/*设置HSMCLK（子系统主时钟），HSMCLK=DCO频率/时钟分频系数，此处HSMCLK=DCO/4=1M*/
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_4);/*设置SMCLK（低速子系统主时钟，可用TimerA频率），SMCLK=DCO频率/时钟分频系数，此处SMCLK=DCO/4=1M*/    
    //![Simple FPU Config]

    /* 选用P1.2和P1.3使用UART模式 */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    
    /* 使用uartConfig配置UART_A0 */
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* 使能UART_A0 */
    MAP_UART_enableModule(EUSCI_A0_BASE);
    
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT); /*使能UART_A0串口接收中断*/ 
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0); /*使能UART_A0中断*/  
    MAP_Interrupt_enableMaster();  /*使能中断总开关*/      
    while(1);
}

/*UART_A0中断函数*/
void EUSCIA0_IRQHandler(void)
{
    char tmp[3]={'\0'};
    int i=0;        
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE); /*将UART_A0中断标志位的值赋值给status*/
    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);/*清空UART_A0中断标志位*/
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) /*判断是否为UART_A0接受中断*/
    {          
        sprintf(tmp,"%x",MAP_UART_receiveData(EUSCI_A0_BASE));/*将从UART_A0中接收到的单个字节以十六进制打印至tmp中*/
        /*将tmp字符串通过UART_A0逐个字符发出*/
        for (i=0;i<strlen(tmp);i++) 
            MAP_UART_transmitData(EUSCI_A0_BASE, tmp[i]); 
    }   
}

