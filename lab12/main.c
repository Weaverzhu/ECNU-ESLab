#define __MSP432P401R__

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define DCO_Frequency 4000000
#define Duration 1
#define N 100

#define PWM_PERIODS (62500 / 50)

const Timer_A_UpModeConfig TA0 =
    {
        TIMER_A_CLOCKSOURCE_SMCLK,  
        TIMER_A_CLOCKSOURCE_DIVIDER_1,  
        PWM_PERIODS - 1,  
        TIMER_A_TAIE_INTERRUPT_DISABLE,  
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,
        TIMER_A_DO_CLEAR  
};


Timer_A_CompareModeConfig TA0_CCR3_PWM =
    {
        TIMER_A_CAPTURECOMPARE_REGISTER_3,   
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,  
        TIMER_A_OUTPUTMODE_RESET_SET,  
        PWM_PERIODS};

const eUSCI_UART_Config uartConfig =
    {
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        6,
        8,
        17,
        EUSCI_A_UART_NO_PARITY,
        EUSCI_A_UART_LSB_FIRST,
        EUSCI_A_UART_ONE_STOP_BIT,
        EUSCI_A_UART_MODE, 
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION};

int cur;
int output;
char buf[N];
int len, start = 0;


int main(void)
{
  uint8_t port_mapping[] =
      {
          PM_NONE,
          PM_NONE,
          PM_TA0CCR3A,
          PM_NONE,
          PM_NONE,
          PM_NONE,
          PM_NONE,
          PM_NONE};

  MAP_WDT_A_holdTimer();

  //![Simple FPU Config]
  MAP_FPU_enableModule();
  MAP_CS_setDCOFrequency(DCO_Frequency);
  MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
  MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_4);
  MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_4);
  //![Simple FPU Config]

  MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
  MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
  MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
  MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

  MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
  MAP_Interrupt_enableInterrupt(INT_T32_INT1);
  MAP_Timer32_enableInterrupt(TIMER32_0_BASE);
  MAP_Timer32_clearInterruptFlag(TIMER32_0_BASE);

  MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
  MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
  MAP_UART_enableModule(EUSCI_A0_BASE);
  MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
  MAP_Interrupt_enableInterrupt(INT_EUSCIA0);

  MAP_PMAP_configurePorts((const uint8_t *)port_mapping, PMAP_P2MAP /*Á´?Âè?2*/, 1 /*8‰∏?ÂºïËÑö*/, PMAP_DISABLE_RECONFIGURATION);
  MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
  MAP_Timer_A_configureUpMode(TIMER_A0_BASE, &TA0);
  MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

  MAP_Interrupt_enableInterrupt(INT_TA1_0);
  TA0_CCR3_PWM.compareValue = 0;
  output = 0;

  MAP_Interrupt_enableMaster();
  while (1)
    ;
}

void EUSCIA0_IRQHandler(void)
{
  char tmp[100];

  uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
  MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);
  if (output)
  {
    output = 0;
    cur = 0;
    TA0_CCR3_PWM.compareValue = 0;
    start = 0;
    MAP_Timer_A_stopTimer(TIMER_A0_BASE);
    MAP_Timer32_haltTimer(TIMER32_0_BASE);
  }
  if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
  {
    char ch = MAP_UART_receiveData(EUSCI_A0_BASE);
    if (ch == ' ')
    {
      len = cur;
      cur = 0;
      output = 1;

      MAP_Timer32_setCount(TIMER32_0_BASE, Duration * DCO_Frequency);
      MAP_Timer32_startTimer(TIMER32_0_BASE, false);

      sprintf(tmp, "start to timer\n");
      for (int i = 0; i < strlen(tmp); ++i)
      {
        MAP_UART_transmitData(EUSCI_A0_BASE, tmp[i]);
      }
    }
    else
    {
      buf[cur++] = ch;
    }
  }
}

void T32_INT1_IRQHandler(void)
{
  char tmp[100];
  if (output == 0) return;
  MAP_Timer32_clearInterruptFlag(TIMER32_0_BASE);
  char ch = buf[cur++];
  sprintf(tmp, "ch=%c\n", ch);
  for (int i = 0; i < strlen(tmp); ++i)
  {
    MAP_UART_transmitData(EUSCI_A0_BASE, tmp[i]);
  }
  if (cur == len)
  {
    output = 0;
    cur = 0;
    start = 0;
  }
  int digit = ch - '0' + 1;
  // if (digit == 0)
  //   digit = 10;
  double coff = 1.0 * digit / 10;
  TA0_CCR3_PWM.compareValue = PWM_PERIODS * coff;
  MAP_Timer_A_initCompare(TIMER_A0_BASE, &TA0_CCR3_PWM);

  MAP_Timer32_setCount(TIMER32_0_BASE, Duration * DCO_Frequency);
  MAP_Timer32_startTimer(TIMER32_0_BASE, false);
}
