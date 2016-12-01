//----------------------------------------------------------------------------------------------------
#ifndef __gpio_H
    #define __gpio_H

#include "stm32f4xx.h"
//----------------------------------------------------------------------------------------------------
#ifdef  IN_GPIO
    #define     ex_gpio
#else  
    #define     ex_gpio  extern
#endif
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//  功能跳线位定义 15-1203
#define     NET_RST         (1 << 0)        // 网络参数复位   PC0

#define     FUN_SEL0        (1 << 4)        // 功能选择位0   PE4
#define     FUN_SEL1        (1 << 5)        // 功能选择位1   PE5 
#define     FUN_SEL2        (1 << 6)        // 功能选择位2   PF6
#define     FUN_SEL3        (1 << 7)        // 功能选择位3   PF7 
#define     FUN_SEL4        (1 << 8)        // 功能选择位4   PF8 
#define     FUN_SEL5        (1 << 9)        // 功能选择位5   PF0 
#define     FUN_SEL6        (1 <<10)        // 功能选择位6   PF10 
#define     FUN_SEL7        (1 <<11)        // 功能选择位7   PF11

#define     FUN_SEL_PE      (0x03 << 4)     // 功能选择位    PE 
#define     FUN_SEL_PF      (0x3f << 6)     // 功能选择位    PF
//----------------------------------------------------------------------------------------------------
typedef enum
{
    Bit_RESET = 0,
    Bit_SET

}   BitAction;
//----------------------------------------------------------------------------------------------------
ex_gpio void    GPIO_Init(void);
ex_gpio void    GPIO_SetBits(GPIO_TypeDef* GPIOx, u16 GPIO_Pin);
ex_gpio void    GPIO_ResetBits(GPIO_TypeDef* GPIOx, u16 GPIO_Pin);
ex_gpio void    GPIO_WriteBit(GPIO_TypeDef* GPIOx, u16 GPIO_Pin, BitAction BitVal);
ex_gpio void    GPIO_Write(GPIO_TypeDef* GPIOx, u16 PortVal);
ex_gpio unsigned    char    NetRst_Read(void);
ex_gpio unsigned    char    FuncSel_ReadBit(unsigned int BitVal);
ex_gpio unsigned    int     FuncSel_ReadWord(void);
ex_gpio unsigned    char    SimCan_ReadBit(void);
//----------------------------------------------------------------------------------------------------
#endif  /* __gpio_H */
