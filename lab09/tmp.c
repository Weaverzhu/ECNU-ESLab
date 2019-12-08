#define __MSP432P401R__

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>

const int lowval = 920;
const int highval = 3276;

const int DCOFrequency = 1e6;

const int period = 20;

int cur, delta;

int main(void)
{
    MAP_WDT_A_holdTimer();
    MAP_FPU_enableModule();
    MAP_CS_setDCOFrequency(DCOFrequency);
    MAP_CS_initClockdeltaal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_GPIO_setAsDACPin(GPIO_PORT_PX, GPIO_PINY);

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    MAP_Interrupt_enableInterrupt(INT_T32_INT1);
    MAP_Timer32_enableInterrupt(TIMER32_0_BASE);

    MAP_Interrupt_enableMaster();

    MAP_Timer32_setCount(TIMER32_0_BASE, period);
    MAP_Timer32_startTimer(TIMER32_0_BASE, false);

    while (1);
}
void T32_INT1_IRQHandler(void)
{

    MAP_Timer32_clearInterruptFlag(TIMER32_0_BASE);

    MAP_GPIO_DAC(GPIO_PORT_PX, GPIO_PINY, cur);
    cur += delta;

    if (cur == highval || cur == lowval) delta = -delta;

}