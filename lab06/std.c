#define __MSP432P401R__

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <string.h>

#define BANKl_S29 (0x3D000)
#define SIZE (4096)
#define STU_NUM (0x56)
#define COUNT (1200000) //大致25ms按键检测

unsigned char array[SIZE]; 
unsigned char condition=0; /*0时为按键检测状态，1为S1按下，2为S2按下，3为S1和S2同时按下*/
int time=COUNT;

int main(void)
{
    volatile unsigned char S29=0x00;/*添加volatile，防止编译优化此变量，并初始化为0x00*/    
    
    memset(array,STU_NUM,SIZE);/*将要写入数值，单字节初始化为 0x56*/
    
    MAP_WDT_A_holdTimer();  /* 停用看门狗定时器 */
    
    /*为了快速编程.设置 MCLK 为 48 MHz*/    
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);    
    FlashCtl_setWaitState(FLASH_BANK0, 1);
    FlashCtl_setWaitState(FLASH_BANK1, 1);
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
     
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);  //设置P1.1（s1按键）为输入模式并拉高  
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);  //设置P1.4（s2按键）为输入模式并拉高
    
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);  //使能GPIO1.1中断
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);   //清除P1.1中断 
    
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN4);  //使能GPIO1.4中断
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN4);   //清除P1.1中断 
    
    MAP_Interrupt_enableInterrupt(INT_PORT1); //使能PORT1中断
    MAP_Interrupt_enableMaster();  //开启中断总开关  
    
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);
    
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
    
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
    
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN2);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
                    
    /*关闭Bankl 中的扇区 29的写保护*/
    MAP_FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1,FLASH_SECTOR29);
        
    while(1)
    { 
        if (condition==0)
        {
            while(time--)
            {
                if (condition==3) break;
            }
            time=COUNT;
        }
        else
        {
            if(condition==1) 
            {
                MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
                MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
                if(!MAP_FlashCtl_performMassErase()) 
                {
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
                }
                else 
                {   
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2); 
                }
            }
            else if(condition==2)
            {
                MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
                MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2); 
                if(!MAP_FlashCtl_programMemory(array,(void*)BANKl_S29,SIZE)) 
                {   
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);
                    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN1);
                }
                else 
                {
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
                }
            }
            else if(condition==3)
            {                
                S29=*((unsigned char*)(BANKl_S29+0x123));/*读取NOR Flash中地址为0x3D123处的单个字节存入S29*/ 
                if (S29==STU_NUM) 
                {
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2); 
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);                    
                }
                else 
                {
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
                    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);                     
                    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0);                           
                }
            }
            condition=0;
        }
    }
}

void PORT1_IRQHandler(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
    
    if((status & GPIO_PIN1) &&  (status & GPIO_PIN4))
    {
        condition=3;
    } 
    else if (status & GPIO_PIN1)
    {
        if (condition==0)
        {
            condition=1;  
            time=COUNT;
        }
        else if(condition==2){ //S2键先松开，S1键后松开
            condition=3;
        }
    }
    else if (status & GPIO_PIN4)
    {
        if (condition==0)
        {
            condition=2;  
            time=COUNT;
        }
        else if(condition==1) //S1键先松开，S2键后松开
            condition=3;        
    }
    
}
