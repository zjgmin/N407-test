//----------------------------------------------------------------------------------------------------
// Header:      GPIO 操作
// File Name:   GPIO.c
// Author:      David
// Date:        2012-10-15
//----------------------------------------------------------------------------------------------------
#define     IN_GPIO
    #include    "stm32f4xx.h" 
    #include    "RTL.h"
    #include    "main.h"
    #include    "gpio.h"
//----------------------------------------------------------------------------------------------------
//  通用IO初始化（输入输出）15-1111
//  16-0107 修改
//----------------------------------------------------------------------------------------------------
void GPIO_Init(void)
{
    /* Enable clock for GPIOC, GPIOE, GPIOF */
    RCC->AHB1ENR |= (1UL << 2);
    RCC->AHB1ENR |= (1UL << 4);
    RCC->AHB1ENR |= (1UL << 5);

    /* 00: Input (reset state)  01: General purpose output mode  10: Alternate function mode  11: Analog mode */
    GPIOC->MODER   &= ~((3UL << 2* 0));
    GPIOC->MODER   |=  ((0UL << 2* 0)); // PC0  Input
    GPIOE->MODER   &= ~((3UL << 2* 4) | (3UL << 2* 5));
    GPIOE->MODER   |=  ((0UL << 2* 4) | (0UL << 2* 5)); // PE4,5    Input
    GPIOF->MODER   &= ~((3UL << 2* 6) | (3UL << 2* 7) | (3UL << 2* 8) | (3UL << 2* 9) | (3UL << 2*10) | (3UL << 2*11));
    GPIOF->MODER   |=  ((0UL << 2* 6) | (0UL << 2* 7) | (0UL << 2* 8) | (0UL << 2* 9) | (0UL << 2*10) | (0UL << 2*11)); // PF6,7,8,9,10,11  Input

    /*  0: Output push-pull (reset state)   1: Output open-drain*/
    GPIOC->OTYPER  &= ~((1UL <<    0));
    GPIOC->OTYPER  |=  ((0UL <<    0)); // PC0  push-pull
    GPIOE->OTYPER  &= ~((1UL <<    4) | (1UL <<    5));
    GPIOE->OTYPER  |=  ((0UL <<    4) | (0UL <<    5)); // PE4,5    push-pull
    GPIOF->OTYPER  &= ~((1UL <<    6) | (1UL <<    7) | (1UL <<    8) | (1UL <<    8) | (1UL <<   10) | (1UL <<   11));
    GPIOF->OTYPER  |=  ((0UL <<    6) | (0UL <<    7) | (0UL <<    8) | (0UL <<    8) | (0UL <<   10) | (0UL <<   11)); // PF6,7,8,9,10,11  push-pull

    /*  00: (2 MHz Low speed)   01: (25 MHz Medium speed)   10: (50 MHz Fast speed) 11: (100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF))*/
    GPIOC->OSPEEDR &= ~((3UL << 2* 0));
    GPIOC->OSPEEDR |=  ((0UL << 2* 0)); // PC0  Low speed
    GPIOE->OSPEEDR &= ~((3UL << 2* 4) | (3UL << 2* 5));
    GPIOE->OSPEEDR |=  ((0UL << 2* 4) | (0UL << 2* 5)); // PE4,5    Low speed
    GPIOF->OSPEEDR &= ~((3UL << 2* 6) | (3UL << 2* 7) | (3UL << 2* 8) | (3UL << 2* 9) | (3UL << 2*10) | (3UL << 2*11));
    GPIOF->OSPEEDR |=  ((0UL << 2* 6) | (0UL << 2* 7) | (0UL << 2* 8) | (0UL << 2* 9) | (0UL << 2*10) | (0UL << 2*11)); // PF6,7,8,9,10,11  Low speed

    /*  00: (No pull-up, pull-down) 01: (Pull-up) 10: (Pull-down) 11: (Reserved)    */
    GPIOC->PUPDR   &= ~((3UL << 2* 0));
    GPIOC->PUPDR   |=  ((1UL << 2* 0)); // PC0  Pull-up
    GPIOE->PUPDR   &= ~((3UL << 2* 4) | (3UL << 2* 5));
    GPIOE->PUPDR   |=  ((1UL << 2* 4) | (1UL << 2* 5)); // PE4,5    Pull-up
    GPIOF->PUPDR   &= ~((3UL << 2* 6) | (3UL << 2* 7) | (3UL << 2* 8) | (3UL << 2* 9) | (3UL << 2*10) | (3UL << 2*11));
    GPIOF->PUPDR   |=  ((1UL << 2* 6) | (1UL << 2* 7) | (1UL << 2* 8) | (1UL << 2* 9) | (1UL << 2*10) | (1UL << 2*11)); // PF6,7,8,9,10,11  Pull-up

    // 上拉功能选择位
    GPIOC->BSRRL = NET_RST;
    GPIOE->BSRRL = FUN_SEL_PE;
    GPIOF->BSRRL = FUN_SEL_PF;
}
//----------------------------------------------------------------------------------------------------
//  置位GPIO引脚
//----------------------------------------------------------------------------------------------------
void GPIO_SetBits(GPIO_TypeDef* GPIOx, u16 GPIO_Pin)
{
    GPIOx->BSRRL = GPIO_Pin;
}
//----------------------------------------------------------------------------------------------------
//  复位GPIO引脚
//----------------------------------------------------------------------------------------------------
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, u16 GPIO_Pin)
{
    GPIOx->BSRRH = GPIO_Pin;
}
//----------------------------------------------------------------------------------------------------
//  写GPIO引脚
//----------------------------------------------------------------------------------------------------
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, u16 GPIO_Pin, BitAction BitVal)
{  
    if (BitVal != Bit_RESET)
    {
        GPIOx->BSRRL = GPIO_Pin;
    }
    else
    {
        GPIOx->BSRRH = GPIO_Pin;
    }
}
//----------------------------------------------------------------------------------------------------
//  批量写GPIO引脚
//----------------------------------------------------------------------------------------------------
void GPIO_Write(GPIO_TypeDef* GPIOx, u16 PortVal)
{
    GPIOx->ODR = PortVal;
}
//----------------------------------------------------------------------------------------------------
//  读取网络复位引脚状态 15-1111
//----------------------------------------------------------------------------------------------------
U8 NetRst_Read(void)
{
    U8  ret = 0;

    ret = GPIOC->IDR & NET_RST;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  读取功能设定位状态   15-1203
//  16-0107 修改，扩展到32位
//----------------------------------------------------------------------------------------------------
U8  FuncSel_ReadBit(U32 BitVal)
{
    U8  ret = 0;
    U32 flag;

    switch (BitVal)
    {
        case 0x00000000:
        {
            flag = GPIOE->IDR & FUN_SEL0;
            break;
        }
        case 0x00000001:
        {
            flag = GPIOE->IDR & FUN_SEL1;
            break;
        }
        case 0x00000002:
        {
            flag = GPIOF->IDR & FUN_SEL2;
            break;
        }
        case 0x00000003:
        {
            flag = GPIOF->IDR & FUN_SEL3;
            break;
        }
        case 0x00000004:
        {
            flag = GPIOF->IDR & FUN_SEL4;
            break;
        }
        case 0x00000005:
        {
            flag = GPIOF->IDR & FUN_SEL5;
            break;
        }
        case 0x00000006:
        {
            flag = GPIOF->IDR & FUN_SEL6;
            break;
        }
        case 0x00000007:
        {
            flag = GPIOF->IDR & FUN_SEL7;
            break;
        }
        case 0x00000008:    // 网络参数复位
        {
            flag = GPIOC->IDR & NET_RST;
            break;
        }
        default:
        {
            flag = 0;
            break;
        }
    }
    if (flag) {
        ret = 1;
    }
    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  读取功能设定位状态 15-1203
//  16-0107 修改，扩展到32位
//----------------------------------------------------------------------------------------------------
U32 FuncSel_ReadWord(void)
{
    U32 ret = 0;    //16-0602 U8->U32
    U32 flag;

    flag = ((GPIOE->IDR & FUN_SEL_PE) | (GPIOF->IDR & FUN_SEL_PF)) >> 4;
    flag &= 0xff;
    if (GPIOC->IDR & NET_RST) {
        flag |= 0x100;
    }
    ret = flag^0x1ff;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  读取模拟CAN硬件状态 16-0107
//      串口输出，CAN口输入跟随变化，多次读取一致，则认为连接，设置相应标记
/*
// PA 9,10 Uart1 tx rx  :  PB 8,9 CAN1 rx tx
// PC10,11 Uart3 tx rx  :  PB 5,6 CAN2 rx tx
*/
//----------------------------------------------------------------------------------------------------
#define     U1_PIN                  ((U16)((1<< 9)|(1<<10)))
#define     U3_PIN                  ((U16)((1<<10)|(1<<11)))
#define     CAN1_PIN                ((U16)((1<< 8)|(1<< 9)))
#define     CAN2_PIN                ((U16)((1<< 5)|(1<< 6)))

#define     SIMCAN_TEST_TIMES       ((U8)100)
#define     DELAY_TIMES             ((U32)50)
//--------------------------------------------------
U8  SimCan_ReadBit(void)
{
    U8  ret = 0;
    U8  i, j, k;

    /* Enable clock for GPIOA, GPIOB, GPIOC */
    RCC->AHB1ENR |= (1UL << 0);
    RCC->AHB1ENR |= (1UL << 1);
    RCC->AHB1ENR |= (1UL << 2);

    /* 00: Input (reset state)  01: General purpose output mode  10: Alternate function mode  11: Analog mode */
    GPIOA->MODER   &= ~((3UL << 2* 9) | (3UL << 2*10));
    GPIOA->MODER   |=  ((1UL << 2* 9) | (1UL << 2*10));                                 // PA9,10       Output
    GPIOB->MODER   &= ~((3UL << 2* 5) | (3UL << 2* 6) | (3UL << 2* 8) | (3UL << 2* 9));
    GPIOB->MODER   |=  ((0UL << 2* 5) | (0UL << 2* 6) | (0UL << 2* 8) | (0UL << 2* 9)); // PB5,6,8,9    Input
    GPIOC->MODER   &= ~((3UL << 2*10) | (3UL << 2*11));
    GPIOC->MODER   |=  ((1UL << 2*10) | (1UL << 2*11));                                 // PC10,11      Output

    /*  0: Output push-pull (reset state)   1: Output open-drain*/
    GPIOA->OTYPER  &= ~((1UL <<    9) | (1UL <<   10));
    GPIOA->OTYPER  |=  ((0UL <<    9) | (0UL <<   10));                                 // PA9,10       push-pull
    GPIOB->OTYPER  &= ~((1UL <<    5) | (1UL <<    6) | (1UL <<    8) | (1UL <<    9));
    GPIOB->OTYPER  |=  ((0UL <<    5) | (0UL <<    6) | (0UL <<    8) | (0UL <<    9)); // PB5,6,8,9    push-pull
    GPIOC->OTYPER  &= ~((1UL <<   10) | (1UL <<   11));
    GPIOC->OTYPER  |=  ((0UL <<   10) | (0UL <<   11));                                 // PC10,11      push-pull

    /*  00: (2 MHz Low speed)   01: (25 MHz Medium speed)   10: (50 MHz Fast speed) 11: (100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF))*/
    GPIOA->OSPEEDR &= ~((3UL << 2* 9) | (3UL << 2*10));
    GPIOA->OSPEEDR |=  ((0UL << 2* 9) | (0UL << 2*10));                                 // PA9,10       Low speed
    GPIOB->OSPEEDR &= ~((3UL << 2* 5) | (3UL << 2* 6) | (3UL << 2* 8) | (3UL << 2* 9));
    GPIOB->OSPEEDR |=  ((0UL << 2* 5) | (0UL << 2* 6) | (0UL << 2* 8) | (0UL << 2* 9)); // PB5,6,8,9    Low speed
    GPIOC->OSPEEDR &= ~((3UL << 2*10) | (3UL << 2*11));
    GPIOC->OSPEEDR |=  ((0UL << 2*10) | (0UL << 2*11));                                 // PC10,11      Low speed

    /*  00: (No pull-up, pull-down) 01: (Pull-up) 10: (Pull-down) 11: (Reserved)    */
    GPIOA->PUPDR   &= ~((3UL << 2* 9) | (3UL << 2*10));
    GPIOA->PUPDR   |=  ((1UL << 2* 9) | (1UL << 2*10));                                 // PA9,10       Pull-up
    GPIOB->PUPDR   &= ~((3UL << 2* 5) | (3UL << 2* 6) | (3UL << 2* 8) | (3UL << 2* 9));
    GPIOB->PUPDR   |=  ((0UL << 2* 5) | (0UL << 2* 6) | (0UL << 2* 8) | (0UL << 2* 9)); // PB5,6,8,9    no
    GPIOC->PUPDR   &= ~((3UL << 2*10) | (3UL << 2*11));
    GPIOC->PUPDR   |=  ((1UL << 2*10) | (1UL << 2*11));                                 // PC10,11      Pull-up

    // 串口引脚输出，CAN口引脚读取
    j = 0;
    k = 0;
    for (i=0; i<SIMCAN_TEST_TIMES; i++) {     // 判断多次
        if (i & 1) {
            GPIOA->BSRRL = U1_PIN;       // PA 9,10 - UART1 TX,RX 置位
            GPIOC->BSRRL = U3_PIN;       // PC10,11 - UART3 TX,RX 置位
            Delay(DELAY_TIMES);
            if ((GPIOB->IDR & CAN1_PIN) == CAN1_PIN) {
                j++;
            }
            if ((GPIOB->IDR & CAN2_PIN) == CAN2_PIN) {
                k++;
            }
        } else {
            GPIOA->BSRRH = U1_PIN;       // PA 9,10 - UART1 TX,RX 清零
            GPIOC->BSRRH = U3_PIN;       // PC10,11 - UART3 TX,RX 清零
            Delay(DELAY_TIMES);
            if ((GPIOB->IDR & CAN1_PIN) == 0) {
                j++;
            }
            if ((GPIOB->IDR & CAN2_PIN) == 0) {
                k++;
            }
        }
    }
    // 获取结果
    if (j == SIMCAN_TEST_TIMES) {
        ret |= 0x01;
    }
    if (k == SIMCAN_TEST_TIMES) {
        ret |= 0x02;
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//eof
