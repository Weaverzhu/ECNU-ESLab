/*******************************************************************************
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P1.0  |---> P1.0 LED
 *            |     PJ.0 LFXIN   |---------
 *            |                  |         |
 *            |                  |     < 32khz xTal >
 *            |                  |         |
 *            |     PJ.1 LFXOUT  |---------
 *
 ******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdio.h>
#include <string.h>

const char weekday[][4]={"SUN","MON","TUE","WED","THU","FRI","SAT"};
/* Statics */
static volatile RTC_C_Calendar newTime;
static uint_fast8_t hour=21;
static uint_fast8_t minute=00;
 //![Simple RTC Config]
 /* DATE TIME：2018年12月16日 周日 20:59:50 */
 RTC_C_Calendar currentTime =
 {
     50, //秒，0-59
     59, //分，0-59
     20, //时，0-23
     00, //周日，0为周日，1-6分别为周一至周六
     16, //日，1-31
     12, //月，1-12
     2018 //年
 };
 //![Simple RTC Config]

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
    
    //![Simple RTC Example]   
    MAP_RTC_C_initCalendar(&currentTime, RTC_C_FORMAT_BINARY); /*使用currentTime以二进制形式初始化RTC时间 */ 
    
    /* 设定当时间为21:00发生CLOCK_ALARM中断*/
    MAP_RTC_C_setCalendarAlarm(minute,  /*分*/
                                hour, /*时*/
                                RTC_C_ALARMCONDITION_OFF, /*周几*/
                                RTC_C_ALARMCONDITION_OFF);  /*日*/  
    
    MAP_RTC_C_setCalendarEvent(RTC_C_CALENDAREVENT_MINUTECHANGE); /* 指定每分钟产生一次中断 */
    MAP_RTC_C_clearInterruptFlag(RTC_C_CLOCK_READ_READY_INTERRUPT /*清空RTC CLOCK_READ中断（此处设定每秒发生一次中断）标志位*/
                                | RTC_C_TIME_EVENT_INTERRUPT /*清空RTC TIME_EVENT中断（此处设定为每分钟发生一次中断）标志位*/
                                | RTC_C_CLOCK_ALARM_INTERRUPT); /*清空RTC CLOCK_ALARM中断（此处设定为21:00发生一次中断）标志位*/
    
    MAP_RTC_C_enableInterrupt(RTC_C_CLOCK_READ_READY_INTERRUPT | RTC_C_TIME_EVENT_INTERRUPT| RTC_C_CLOCK_ALARM_INTERRUPT);  /*使能RTC CLOCK_READ、TIME_EVENT和CLOCK_ALARM中断*/  
    MAP_RTC_C_startClock(); /* 启动RTC时钟 */
    MAP_Interrupt_enableInterrupt(INT_RTC_C);/*使能RTC中断*/ 
    //![Simple RTC Example] 
        
    //![Simple FPU Config]   
    MAP_FPU_enableModule();/*启用FPU加快DCO频率计算，注：DCO是内部数字控制振荡器，默认是3M频率*/    
    MAP_CS_setDCOFrequency(4000000);/* 设置DCO频率为指定频率，此处DCO=4M*/
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);/*设置MCLK（主时钟，可用于CPU主频等），MCLK=DCO频率/时钟分频系数，此处MCLK=DCO=4M*/
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_4);/*设置HSMCLK（子系统主时钟），HSMCLK=DCO频率/时钟分频系数，此处HSMCLK=DCO/4=1M*/
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_4);/*设置SMCLK（低速子系统主时钟，可用TimerA频率），SMCLK=DCO频率/时钟分频系数，此处SMCLK=DCO/4=1M*/    
    //![Simple FPU Config]    
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION); /* 选用P1.2和P1.3使用UART模式 */
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);/* 使用uartConfig配置UART_A0 */
    MAP_UART_enableModule(EUSCI_A0_BASE);/* 使能UART_A0 */    
    
    MAP_Interrupt_enableMaster(); /*使能中断总开关*/ 

    while(1);
}

/* RTC ISR */
void RTC_C_IRQHandler(void)
{       
    int i;
    uint32_t status;
    char tmp[100];
    status = MAP_RTC_C_getEnabledInterruptStatus(); /*将RTC中断标志位的值赋值给status*/
    MAP_RTC_C_clearInterruptFlag(status); /*清空UART_A0中断标志位*/

    if (status & RTC_C_CLOCK_READ_READY_INTERRUPT) /*判断是否为RTC CLOCK_READ中断，此处每秒发生一次*/
    {
        newTime = MAP_RTC_C_getCalendarTime(); /*获取当前时间日期*/
        sprintf(tmp,"Interrupts:%d-%d-%d %s %d:%d:%d\n",newTime.year,
                                                        newTime.month,
                                                        newTime.dayOfmonth,
                                                        weekday[newTime.dayOfWeek],
                                                        newTime.hours,
                                                        newTime.minutes,
                                                        newTime.seconds);
        /*将tmp字符串通过UART_A0逐个字符发出*/
        for (i=0;i<strlen(tmp);i++)
            MAP_UART_transmitData(EUSCI_A0_BASE, tmp[i]);          
    }

    if (status & RTC_C_TIME_EVENT_INTERRUPT) /*判断是否为RTC TIME_EVENT中断，此处每分钟发生一次*/
    {
        sprintf(tmp,"Interrupts every minute.\n");
        /*将tmp字符串通过UART_A0逐个字符发出*/
        for (i=0;i<strlen(tmp);i++)
            MAP_UART_transmitData(EUSCI_A0_BASE, tmp[i]);         
    }

    if (status & RTC_C_CLOCK_ALARM_INTERRUPT) /*判断是否为RTC CLOCK_ALARM中断，指定时间发生一次*/
    {
        sprintf(tmp,"Interrupts at %02d:%02d.\n",hour,minute);
        /*将tmp字符串通过UART_A0逐个字符发出*/
        for (i=0;i<strlen(tmp);i++)
            MAP_UART_transmitData(EUSCI_A0_BASE, tmp[i]);
    }

}


