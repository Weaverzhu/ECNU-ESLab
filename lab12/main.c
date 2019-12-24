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

#define PWM_PERIODS (62500 / 10)

const Timer_A_UpModeConfig TA0 =
    {
        TIMER_A_CLOCKSOURCE_SMCLK,           // ѡ��SMCLK��Ϊʱ��Դ
        TIMER_A_CLOCKSOURCE_DIVIDER_1,       // ��Ƶϵ��Ϊ1��Timer_A0=SMCLK/1=62.5k
        PWM_PERIODS - 1,                     // ����ʱ��=PWM_PERIODS/Timer_A0=20ms(ע���Ӿ�����ʱ��ԼΪ100ms��400ms)������㿪ʼ��������������һ������
        TIMER_A_TAIE_INTERRUPT_DISABLE,      // ���ö�ʱ���ж�
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE, // ���ö�ʱ��CCR0�ж�
        TIMER_A_DO_CLEAR                     // �����ֵ
};

Timer_A_CompareModeConfig TA0_CCR1_PWM =
    {
        TIMER_A_CAPTURECOMPARE_REGISTER_1,        // CCR1ͨ��
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE, // ����CCR�ж�
        TIMER_A_OUTPUTMODE_RESET_SET,             // ���ø�λ/��λ
        PWM_PERIODS                               // ռ�ձȣ��˴����ü�һ������ʱ����ֵval<PWM_PERIODSʱ������ߵ�ƽ����0��PWM_PERIODS-1����PWM_PERIODS������val>=PWM_PERIODSʱ����͵�ƽ
};

Timer_A_CompareModeConfig TA0_CCR2_PWM =
    {
        TIMER_A_CAPTURECOMPARE_REGISTER_2,        // CCR2ͨ��
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE, // ����CCR�ж�
        TIMER_A_OUTPUTMODE_RESET_SET,             // ���ø�λ/��λ
        PWM_PERIODS};

Timer_A_CompareModeConfig TA0_CCR3_PWM =
    {
        TIMER_A_CAPTURECOMPARE_REGISTER_3,        // CCR3ͨ��
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE, // ����CCR�ж�
        TIMER_A_OUTPUTMODE_RESET_SET,             // ���ø�λ/��λ
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
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION 
};

int cur;
int output;
char buf[N];
int len;

void outputStuNo() {
    
}

int main(void)
{  
     uint8_t port_mapping[] =
        {
            PM_TA0CCR1A, //ӳ����Timer_A0_CCR1����Pin0��Ӧ
            PM_TA0CCR2A, //ӳ����Timer_A0_CCR2����Pin1��Ӧ
            PM_TA0CCR3A, //ӳ����Timer_A0_CCR3����Pin2��Ӧ
            PM_NONE,     //��ӳ�䣬ӳ��Ϊ��
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

    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
    MAP_UART_enableModule(EUSCI_A0_BASE);
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);

    MAP_PMAP_configurePorts((const uint8_t *)port_mapping, PMAP_P2MAP, 1, PMAP_DISABLE_RECONFIGURATION);
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_Timer_A_configureUpMode(TIMER_A0_BASE, &TA0);

    MAP_Interrupt_enableInterrupt(INT_TA1_0); 

    output = 0;

    MAP_Interrupt_enableMaster();     
    while(1);
}

void EUSCIA0_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    if (output) return; // don't bother when outputing
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {          
        char ch = MAP_UART_receiveData(EUSCI_A0_BASE);
        if (ch == ' ') {
            len = cur;
            cur = 0;
            output = 1;
            MAP_Timer32_setCount(TIMER32_0_BASE, Duration);
            MAP_Timer32_startTimer(TIMER32_0_BASE, false);
        } else {
            buf[cur++] = ch;
        }
    }   
}



void T32_INT1_IRQHandler(void)
{
    MAP_Timer32_clearInterruptFlag(TIMER32_0_BASE);
    char ch = buf[cur++];
    if (ch == ' ') {
        output = 0;
        MAP_Timer32_haltTimer(TIMER32_0_BASE);
        cur = 0;
        return;
    }
    int digit = ch - '0';
    if (digit == 0) digit = 10;
    double coff = 1.0 * digit / 10;
    TA0_CCR3_PWM.compareValue = PWM_PERIODS * coff;
    MAP_Timer_A_initCompare(TIMER_A0_BASE, &TA0_CCR3_PWM);
    
    MAP_Timer32_setCount(TIMER32_0_BASE, Duration);
    MAP_Timer32_startTimer(TIMER32_0_BASE, false);
}

