#define __MSP432P401R__

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/devices/msp432p4xx/driverlib/flash.h>
#include <string.h>
#include <stdlib.h>

#define BANKl_S29 (0x3D000)
#define strNum 11
#define BANK_SIZE 4096
#define COUNT (1200000)

unsigned char array[BANK_SIZE];
int button;
int time;

int main(void)
{
    volatile char S29 = 0x00;
    volatile int pos;

    // fill the bank with strNum
    memset(array, strNum, BANK_SIZE);

    // stop the watch dog
    MAP_WDT_A_holdTimer();

    // set MCLK to 48MHz
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
    FlashCtl_setWaitState(FLASH_BANK0, 1);
    FlashCtl_setWaitState(FLASH_BANK1, 1);
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);

    // close the write protection
    MAP_FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1, FLASH_SECTOR29);

    // LED1 R(P1.0) LED2 R(P2.0) LED2 G(P2.1) LED2 B(P2.2)
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);

    // initialize, should be off when start
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    // S1(P1.1) S2(P1.4)
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);

    // interrupt
    MAP_Interrupt_enableInterrupt(INT_PORT1);
    MAP_Interrupt_enableMaster();

    while (1)
    {
        if (!button)
        {
            for (; button != 3; --time)
                ;
            time = COUNT;
        }
        else
        {
            switch (button)
            {
            case 1:
                // turn off the other lights
                MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
                MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);

                if (!MAP_FlashCtl_performMassErase()) // failed to erase
                {
                    // turn off LED1 R, turn on LED2 B
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
                }
                else // succeed to erase
                {
                    // opposite
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
                }
                break;
            case 2:
                // turn off the other lights
                MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
                MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
                if (!MAP_FlashCtl_programMemory(array, (void *)BANKl_S29, BANK_SIZE))
                {
                    // turn off LED2 G, turn on LED1 R
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
                }
                else
                {
                    // opposite
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
                }
                break;
            case 3:

                // randomly select a position in BANK1_S29
                pos = rand() % BANK_SIZE;
                S29 = *((unsigned char *)(BANKl_S29 + pos));

                // if the char is what we want
                if (S29 == strNum)
                {
                    // turn on red, green, blue light on LED2 only, which is white light
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
                }
                else
                {
                    // only red light on LED1 only
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
                }

                break;

            default:
                break;
            }
            button = 0;
        }
    }
}

void PORT1_IRQHandler(void)
{
    uint32_t status;
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    // 1st bit rep for PIN1, 2nd bit for PIN4
    status = (status & GPIO_PIN1) | ((status & GPIO_PIN4) << 1);

    // update the button
    button |= status;

    // start the timer
    if (button == 0)
    {
        time = COUNT;
    }
}
