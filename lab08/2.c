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
    MAP_WDT_A_holdTimer(); /* 停用开门狗 */

    //![Simple FPU Config]
    MAP_FPU_enableModule();           /*启用FPU加快DCO频率计算，注：DCO是内部数字控制振荡器，默认是3M频率*/
    MAP_CS_setDCOFrequency(10000000); /* 设置DCO频率为指定频率，此处DCO=10M*/
    //![Simple FPU Config]

    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); /*设置MCLK（主时钟频率），MCLK=DCO频率/时钟分频系数，此处MCLK=DCO=1M*/

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);     /* 配置P4.0为输出模式 */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0); /*拉高P1.0引脚电压*/

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0);     /* 配置P4.0为输出模式 */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0); /*拉高P1.0引脚电压*/

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);     /* 配置P4.4为输出模式 */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4); /*拉高P4.4引脚电压*/

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN4);     /* 配置P5.4为输出模式 */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN4); /*拉高P5.4引脚电压*/

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    MAP_Timer32_initModule(TIMER32_1_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
		
    MAP_Interrupt_enableInterrupt(INT_T32_INT1);
    MAP_Timer32_enableInterrupt(TIMER32_0_BASE);

    
    MAP_Interrupt_enableInterrupt(INT_T32_INT2);    
    MAP_Timer32_enableInterrupt(TIMER32_1_BASE);
		
		MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);  //设置P1.1（s1按键）为输入模式并拉高  
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);  //设置P1.4（s2按键）为输入模式并拉高

    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);    //使能GPIO1.1中断
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1); //清除P1.1中断

    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);    //使能GPIO1.4中断
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4); //清除P1.1中断

    MAP_Interrupt_enableInterrupt(INT_PORT1); //使能PORT1中断
    MAP_Interrupt_enableMaster();             /*使能中断总开关*/

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

/* Timer32第一个计数器中断函数*/
void T32_INT1_IRQHandler(void)
{
	MAP_Timer32_clearInterruptFlag(TIMER32_0_BASE); 
    switchcolor();
}

void PORT1_IRQHandler(void)
{
    uint32_t status;
    //switchcolor();
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    //status = (status & GPIO_PIN1) | ((status & GPIO_PIN4) << 1);

    if (status & GPIO_PIN1) {
        if (condition == 0) {
            MAP_Timer32_setCount(TIMER32_1_BASE, 1000000);
					
						MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
						MAP_Timer32_startTimer(TIMER32_1_BASE, false);
            condition = 1;
        } else {
            flag1 = flag2 = 0;
            setcolor(color = 0);
						condition = 0;
        }
    } else if (status & GPIO_PIN4) {
        
        if (condition == 0) {
            MAP_Timer32_setCount(TIMER32_1_BASE, 1000000);
						MAP_Timer32_startTimer(TIMER32_1_BASE, false);
						MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
            condition = 2;
        } else {
            flag1 = flag2 = 0;
            setcolor(color = 0);
						condition = 0;
        }
    } else if ((status & GPIO_PIN1) && (status & GPIO_PIN4)) {
        flag1 = flag2 = 0;
        setcolor(color = 0);
				condition = 0;
    }
}

/* Timer32第二个计数器中断函数*/
void T32_INT2_IRQHandler(void)
{
    MAP_Timer32_clearInterruptFlag(TIMER32_1_BASE); /*清除Timer32第二个计数器中断标志*/  
		MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
		if (condition == 1) {
        flag1 = 1;
        flag2 = 0;
        color = 0;
        setcolor(color);
    } else if (condition == 2) {
        flag1 = 0;
        flag2 = 1;
        color = 2;
        setcolor(color);
    }
    condition = 0;
}
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
    MAP_WDT_A_holdTimer(); /* 停用开门狗 */

    //![Simple FPU Config]
    MAP_FPU_enableModule();           /*启用FPU加快DCO频率计算，注：DCO是内部数字控制振荡器，默认是3M频率*/
    MAP_CS_setDCOFrequency(10000000); /* 设置DCO频率为指定频率，此处DCO=10M*/
    //![Simple FPU Config]

    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1); /*设置MCLK（主时钟频率），MCLK=DCO频率/时钟分频系数，此处MCLK=DCO=1M*/

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);     /* 配置P4.0为输出模式 */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0); /*拉高P1.0引脚电压*/

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0);     /* 配置P4.0为输出模式 */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0); /*拉高P1.0引脚电压*/

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);     /* 配置P4.4为输出模式 */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4); /*拉高P4.4引脚电压*/

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN4);     /* 配置P5.4为输出模式 */
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN4); /*拉高P5.4引脚电压*/

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    MAP_Timer32_initModule(TIMER32_1_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
		
    MAP_Interrupt_enableInterrupt(INT_T32_INT1);
    MAP_Timer32_enableInterrupt(TIMER32_0_BASE);

    
    MAP_Interrupt_enableInterrupt(INT_T32_INT2);    
    MAP_Timer32_enableInterrupt(TIMER32_1_BASE);
		
		MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);  //设置P1.1（s1按键）为输入模式并拉高  
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);  //设置P1.4（s2按键）为输入模式并拉高

    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);    //使能GPIO1.1中断
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1); //清除P1.1中断

    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);    //使能GPIO1.4中断
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4); //清除P1.1中断

    MAP_Interrupt_enableInterrupt(INT_PORT1); //使能PORT1中断
    MAP_Interrupt_enableMaster();             /*使能中断总开关*/

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

/* Timer32第一个计数器中断函数*/
void T32_INT1_IRQHandler(void)
{
	MAP_Timer32_clearInterruptFlag(TIMER32_0_BASE); 
    switchcolor();
}

void PORT1_IRQHandler(void)
{
    uint32_t status;
    //switchcolor();
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    //status = (status & GPIO_PIN1) | ((status & GPIO_PIN4) << 1);

    if (status & GPIO_PIN1) {
        if (condition == 0) {
            MAP_Timer32_setCount(TIMER32_1_BASE, 1000000);
					
						MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
						MAP_Timer32_startTimer(TIMER32_1_BASE, false);
            condition = 1;
        } else {
            flag1 = flag2 = 0;
            setcolor(color = 0);
						condition = 0;
        }
    } else if (status & GPIO_PIN4) {
        
        if (condition == 0) {
            MAP_Timer32_setCount(TIMER32_1_BASE, 1000000);
						MAP_Timer32_startTimer(TIMER32_1_BASE, false);
						MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
            condition = 2;
        } else {
            flag1 = flag2 = 0;
            setcolor(color = 0);
						condition = 0;
        }
    } else if ((status & GPIO_PIN1) && (status & GPIO_PIN4)) {
        flag1 = flag2 = 0;
        setcolor(color = 0);
				condition = 0;
    }
}

/* Timer32第二个计数器中断函数*/
void T32_INT2_IRQHandler(void)
{
    MAP_Timer32_clearInterruptFlag(TIMER32_1_BASE); /*清除Timer32第二个计数器中断标志*/  
		MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
		if (condition == 1) {
        flag1 = 1;
        flag2 = 0;
        color = 0;
        setcolor(color);
    } else if (condition == 2) {
        flag1 = 0;
        flag2 = 1;
        color = 2;
        setcolor(color);
    }
    condition = 0;
}
