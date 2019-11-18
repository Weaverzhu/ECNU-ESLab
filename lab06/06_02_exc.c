
#define __MCU_HAS_FLCTL__
#define __MSP432P401R__

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/devices/msp432p4xx/driverlib/flash.h>
#include <string.h>

#define BANKl_S30 (0x3E000)
#define BANKl_S31 (0x3F000)

/*定义缓冲区大小为 8192,8K=2*4K,即两个扇区大小*/
unsigned char array[8192]; 

int main(void)
{
    volatile unsigned char S30=0x00;/*添加volatile，防止编译优化此变量，并初始化为0x00*/
    volatile unsigned char S31=0x00;/*添加volatile，防止编译优化此变量，并初始化为0x00*/
    memset(array,0x56,8192);/*将要写入数值，单字节初始化为 0x56*/
    
    MAP_WDT_A_holdTimer();  /* 停用看门狗定时器 */
    
    /*为了快速编程.设置 MCLK 为 48 MHz*/    
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);    
    FlashCtl_setWaitState(FLASH_BANK0, 1);
    FlashCtl_setWaitState(FLASH_BANK1, 1);
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
                 
    /*关闭Bankl 中的扇区 30 和扇区 31的写保护*/
    MAP_FlashCtl_unprotectSector(FLASH_MAIN_MEMORY_SPACE_BANK1,FLASH_SECTOR30|FLASH_SECTOR31);
    
    /*在编程前先尝试整体擦除。由于BANK1的的扇区30和扇区31写保护已关，因此可擦除这两个扇区。如果失败，则可设置LED进行提示*/
    if(!MAP_FlashCtl_performMassErase()) while(1)/*可改为控制LED灯闪*/;
    
    /*试图填充数据编程。如果失败，则可设置LED进行提示*/
    if(!MAP_FlashCtl_programMemory(array,(void*)BANKl_S30,8192)) while(1)/*可改为控制LED灯闪*/;
        
    MAP_FlashCtl_protectSector(FLASH_MAIN_MEMORY_SPACE_BANK1,FLASH_SECTOR31); /*设置BANK1的扇区 31 的保护*/
    
    /*再次进行整体擦除。因为扇区 31 受到保护,所以只有扇区 30(0x3E000～0x3EFFF)被擦除
    *该调用后设置一个断点.以便观察调试器中的存储器数据*/
    if(!MAP_FlashCtl_performMassErase()) while(1)/*可改为控制LED灯闪*/;    
    
    S30=*((unsigned char*)(BANKl_S30+0x123));/*读取NOR Flash中地址为0x3E123处的单个字节存入S30*/    
    S31=*((unsigned char*)(BANKl_S31+0x123));/*读取NOR Flash中地址为0x3F123处的单个字节存入S31*/    
   
    while(1);
}
