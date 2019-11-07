#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
//延时函数
void MyDelay(unsigned int count)
{
    volatile uint32_t i;
    for(i=1000*count; i>0; i--);
    return ;
}

int main(void)
{
    /* 停用看门狗定时器 */
    MAP_WDT_A_holdTimer();
    
    /* 配置时钟频率MCLK=3M（主时钟频率） DCO默认频率3M*/   
    MAP_CS_initClockSignal(CS_MCLK, CS_MODOSC_SELECT, CS_CLOCK_DIVIDER_128);    
           
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);  //设置P1.1（s1按键）为输入模式并拉高
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);   //清除P1.1中断 
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);  //使能GPIO1.1中断
    MAP_Interrupt_enableInterrupt(INT_PORT1); //使能PORT1中断    
    MAP_Interrupt_enableMaster();  //开启中断总开关  
    
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1); //配置P2.1为输出模式
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1); //使LED2中绿色灯初始为低电平（常暗）
    while(1)
    {    
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
        MyDelay(10);        
    }
}

void PORT1_IRQHandler(void)
{
    uint32_t status;
    static int count=0;
    unsigned int tmp=0; 
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1); //获取端口P1中断状态的值
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status); //清空中断端口P1标志

    if(status & GPIO_PIN1) //判断是否是P1.1发生了中断
    {       
        switch(count)
        {
            case 0:
                tmp=CS_CLOCK_DIVIDER_128; //不分频
                break;
            case 1:
                tmp=CS_CLOCK_DIVIDER_64; //2分频,以下类推
                break;
            case 2:
                tmp=CS_CLOCK_DIVIDER_32;
                break;
            case 3:
                tmp=CS_CLOCK_DIVIDER_16;
                break;
            case 4:
                tmp=CS_CLOCK_DIVIDER_8;
                break;
            case 5:
                tmp=CS_CLOCK_DIVIDER_4;
                break;           
            default:
                return ;
        }
        MAP_CS_initClockSignal(CS_MCLK, CS_MODOSC_SELECT, tmp); //主时钟频率MCLK=3M/tmp;
        count++;
        count%=6;
    }
}
