// #define __MSP432P401R__

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
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          //??SMCLK(1M)???
        6,                                       // BRDIV = 6 ,clockPrescalar?????? 
        8,                                       // UCxBRF = 8  firstModReg (BRDIV?UCxBRF? UCxBRS?SMCLK,?????????)
        17,                                      // UCxBRS = 17 secondModReg
        EUSCI_A_UART_NO_PARITY,                  // ???None
        EUSCI_A_UART_LSB_FIRST,                  // ????,????
        EUSCI_A_UART_ONE_STOP_BIT,               // ???1?
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // ??????,????1
};

int main(void)
{  
    /* ????? */
    MAP_WDT_A_holdTimer();
    
    //![Simple FPU Config]   
    MAP_FPU_enableModule();/*??FPU??DCO????,?:DCO??????????,???3M??*/    
    MAP_CS_setDCOFrequency(4000000);/* ??DCO???????,??DCO=4M*/
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);/*??MCLK(???,???CPU???),MCLK=DCO??/??????,??MCLK=DCO=4M*/
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_4);/*??HSMCLK(??????),HSMCLK=DCO??/??????,??HSMCLK=DCO/4=1M*/
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_4);/*??SMCLK(????????,??TimerA??),SMCLK=DCO??/??????,??SMCLK=DCO/4=1M*/    
    //![Simple FPU Config]
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    /* ??P1.2?P1.3??UART?? */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    
    /* ??uartConfig??UART_A0 */
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* ??UART_A0 */
    MAP_UART_enableModule(EUSCI_A0_BASE);
    
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT); /*??UART_A0??????*/ 
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0); /*??UART_A0??*/  
    MAP_Interrupt_enableMaster();  /*???????*/      
    while(1);
}

/*UART_A0????*/
void EUSCIA0_IRQHandler(void)
{
    char tmp[3]={'\0'};
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {          
        sprintf(tmp,"%x",MAP_UART_receiveData(EUSCI_A0_BASE));

        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

        if (strcmp(tmp, "31") == 0) {
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
        } else if (strcmp(tmp, "32") == 0) {
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
        } else if (strcmp(tmp, "33") == 0) {
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
        }
    }   
}

