//#define __MSP432P401R__

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

//typedef unsigned uint32_t;

bool flag1, flag2;
int color, condition;

const int duration[] = {
    60000000, 15000000, 25000000};

void setcolor(int color)
{
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0);
    switch (color)
    {
    case 0:
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN4);
        break;
    case 1:
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
        break;
    case 2:
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);

    default:
        break;
    }
    MAP_Timer32_setCount(TIMER32_0_BASE, duration[color]);
    MAP_Timer32_startTimer(TIMER32_0_BASE, false);
}

void switchcolor()
{
    if (!flag1 && !flag2)
    {
        color = (color + 1) % 3;
    }
    setcolor(color);
}

int main(void)
{
    MAP_WDT_A_holdTimer();
    MAP_FPU_enableModule();
    MAP_CS_setDCOFrequency(10000000);

    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0);
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN4);
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN4);

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    MAP_Timer32_initModule(TIMER32_1_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);

    MAP_Interrupt_enableInterrupt(INT_T32_INT1);
    MAP_Timer32_enableInterrupt(TIMER32_0_BASE);

    MAP_Interrupt_enableInterrupt(INT_T32_INT2);
    MAP_Timer32_enableInterrupt(TIMER32_1_BASE);

    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);

    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);

    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);

    MAP_Interrupt_enableInterrupt(INT_PORT1);
    MAP_Interrupt_enableMaster();

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0);
    color = 0;
    setcolor(color);
    while (1)
    {
    }
}

void T32_INT1_IRQHandler(void)
{
    MAP_Timer32_clearInterruptFlag(TIMER32_0_BASE);
    switchcolor();
}

void PORT1_IRQHandler(void)
{
    uint32_t status;
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if (status & GPIO_PIN1)
    {
        if (condition == 0)
        {
            MAP_Timer32_setCount(TIMER32_1_BASE, 1000000);

            MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
            MAP_Timer32_startTimer(TIMER32_1_BASE, false);
            condition = 1;
        }
        else
        {
            flag1 = flag2 = 0;
            setcolor(color = 0);
            condition = 0;
        }
    }
    else if (status & GPIO_PIN4)
    {

        if (condition == 0)
        {
            MAP_Timer32_setCount(TIMER32_1_BASE, 1000000);
            MAP_Timer32_startTimer(TIMER32_1_BASE, false);
            MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
            condition = 2;
        }
        else
        {
            flag1 = flag2 = 0;
            setcolor(color = 0);
            condition = 0;
        }
    }
    else if ((status & GPIO_PIN1) && (status & GPIO_PIN4))
    {
        flag1 = flag2 = 0;
        setcolor(color = 0);
        condition = 0;
    }
}

void T32_INT2_IRQHandler(void)
{
    MAP_Timer32_clearInterruptFlag(TIMER32_1_BASE);
    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    if (condition == 1)
    {
        flag1 = 1;
        flag2 = 0;
        color = 0;
        setcolor(color);
    }
    else if (condition == 2)
    {
        flag1 = 0;
        flag2 = 1;
        color = 2;
        setcolor(color);
    }
    condition = 0;
}
