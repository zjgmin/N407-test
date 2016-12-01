//----------------------------------------------------------------------------------------------------
// Header:      SPI口通信 磁电MR25H40 4Mbit 0x80000
// File Name:   SPI.c
// Author:      David
// Date:        2012-10-13
//----------------------------------------------------------------------------------------------------
#define     IN_SPI
    #include    "stm32f4xx.h"
    #include    "RTL.h"
    #include    "main.h"
    #include    "gpio.h"
    #include    "spi.h"
    #include    "led.h"
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#define   DUMMY_BYTE    (0xff)
//----------------------------------------
// 功能引脚代码
//----------------------------------------
#define   MRAM_CS1      (1ul << 1)  // 磁电片选1  PB
#define   MRAM_CS2      (1ul << 0)  // 磁电片选2  PB
#define   MRAM_HOLD1    (1ul << 5)  // 磁电保持1  PA
#define   MRAM_HOLD2    (1ul << 4)  // 磁电保持2  PA
//----------------------------------------------------------------------------------------------------
// Magnetoresistor RAM 命令代码
//----------------------------------------
#define   WREN      (0x06)        // Write Enable
#define   WRDI      (0x04)        // Write Disable
#define   RDSR      (0x05)        // Read Status Register
#define   WRSR      (0x01)        // Write Status Register
#define   READ      (0x03)        // Read Data Bytes
#define   WRITE     (0x02)        // Write Data Bytes
#define   SLEEP     (0xb9)        // Enter Sleep Mode
#define   WAKE      (0xab)        // Exit Sleep Mode
//----------------------------------------
// 状态寄存器位
//----------------------------------------
#define   SRWD      (1ul << 7)    // 状态寄存器写禁止(引脚 /WP = 0 时)
#define   WP_NULL   (0ul << 2)    // 写保护区-NULL
#define   WP_UOQ    (1ul << 2)    // 保护区 - 高1/4区
#define   WP_HALF   (2ul << 2)    // 保护区 - 一半
#define   WP_ALL    (3ul << 2)    // 保护区 - 全部
#define   WEL       (1ul << 1)    // 写使能锁存
#define   nRDY      (1ul << 0)    // 写就绪位（0：就绪，1：忙）
//----------------------------------------------------------------------------------------------------
//  SPI配置   (SPI2)
//      PA4:/Hold2, PA5:/Hold1
//      PB0:/CS2, PB1:/CS1, PB10:SCK, PB14:MISO, PB15:MOSI
//----------------------------------------------------------------------------------------------------
void SPI_Init (void) 
{
    /* Enable clock for GPIOA, GPIOB */
    RCC->AHB1ENR |= (1UL << 0);
    RCC->AHB1ENR |= (1UL << 1);

    /* 00: Input (reset state)  01: General purpose output mode  10: Alternate function mode  11: Analog mode */
    GPIOA->MODER   &= ~((3UL << 2* 5) | (3UL << 2* 4));
    GPIOA->MODER   |=  ((1UL << 2* 5) | (1UL << 2* 4)); // PA4,5 Output
    GPIOB->MODER   &= ~((3UL << 2*15) | (3UL << 2*14) | (3UL << 2*10) | (3UL << 2*1) | (3UL << 2*0));
    GPIOB->MODER   |=  ((2UL << 2*15) | (2UL << 2*14) | (2UL << 2*10) | (1UL << 2*1) | (1UL << 2*0));   // PB0,1 Output,PB10,14,15 AF

    /*  0: Output push-pull (reset state)   1: Output open-drain*/
    GPIOA->OTYPER  &= ~((1UL <<    5) | (1UL <<    4));
    GPIOB->OTYPER  &= ~((1UL <<   15) | (1UL <<   14) | (1UL <<   10) | (1UL <<   1) | (1UL <<   0));

    /*  00: (2 MHz Low speed)   01: (25 MHz Medium speed)   10: (50 MHz Fast speed) 11: (100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF))*/
    GPIOA->OSPEEDR &= ~((3UL << 2* 5) | (3UL << 2* 4));
    GPIOA->OSPEEDR |=  ((2UL << 2* 5) | (2UL << 2* 4));
    GPIOB->OSPEEDR &= ~((3UL << 2*15) | (3UL << 2*14) | (3UL << 2*10) | (3UL << 2*1) | (3UL << 2*0));
    GPIOB->OSPEEDR |=  ((2UL << 2*15) | (2UL << 2*14) | (2UL << 2*10) | (2UL << 2*1) | (2UL << 2*0));

    /*  00: (No pull-up, pull-down) 01: (Pull-up) 10: (Pull-down) 11: (Reserved)    */
    GPIOA->PUPDR   &= ~((3UL << 2* 5) | (3UL << 2* 4));
    GPIOA->PUPDR   |=  ((1UL << 2* 5) | (1UL << 2* 4));
    GPIOB->PUPDR   &= ~((3UL << 2*15) | (3UL << 2*14) | (3UL << 2*10) | (3UL << 2*1) | (3UL << 2*0));
    GPIOB->PUPDR   |=  ((1UL << 2*15) | (1UL << 2*14) | (1UL << 2*10) | (1UL << 2*1) | (1UL << 2*0));

    //PB11 配置为SPI2_SCK, PB14 配置为SPI2_MISO, PB15 配置为SPI2_MOSI,
    GPIOB->AFR[1] &= 0x00fff0ff;
    GPIOB->AFR[1] |= 0x55000500;

    /* Enable clock for SPI2 */
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
//    /* Reset SPI2 */
//    RCC->APB1RSTR |=  (1UL << 14);

    //SPI2 初始化
    SPI2->CR1   = (0UL << 15)       //0:2-line unidirectional
                | (0UL << 14)       //Output enable/disable in bidirectional mode
                | (0UL << 13)       //CRC calculation disabled
                | (0UL << 12)       //CRC transfer next
                | (0UL << 11)       //0: 8-bit  1: 16bit
                | (0UL << 10)       //Receive only in bidirectional mode
                | (1UL << 9)        //Software slave management
                | (1UL << 8)        //Internal slave select
                | (0UL << 7)        //1: LSBFIRST 0: MSBFIRST
                | (0UL << 6)        //SPI enable
                | (1UL << 3)        //Baud rate fPCLK/(2^(n+1)) 42M/4
                | (1UL << 2)        //Master selection:1
                | (0UL << 1)        //Clock polarity when idle //0:时钟悬空低
                | (0UL << 0);       //Clock phase //0:数据捕获在第1个时钟沿(第1位数据在此沿前就建立)

    SPI2->CR1 |= (1UL << 6);        //开SPI口

    MRAM_Init();
}
//----------------------------------------------------------------------------------------------------
//  片选处理
//----------------------------------------------------------------------------------------------------
void SPI_CS(U8 chip_no, U8 stat)
{
    if (stat == 0) {
        if (chip_no == 0) {
            GPIO_ResetBits(GPIOB, MRAM_CS1);
        } else {
            GPIO_ResetBits(GPIOB, MRAM_CS2);
        }
    } else {
        if (chip_no == 0) {
            GPIO_SetBits(GPIOB, MRAM_CS1);
        } else {
            GPIO_SetBits(GPIOB, MRAM_CS2);
        }
    }
}
//----------------------------------------------------------------------------------------------------
//  Hold线处理
//----------------------------------------------------------------------------------------------------
void SPI_HOLD(U8 chip_no, U8 stat)
{
    if (stat == 0) {
        if (chip_no == CHIP1) {
            GPIO_ResetBits(GPIOA, MRAM_HOLD1);
        } else {
            GPIO_ResetBits(GPIOA, MRAM_HOLD2);
        }
    } else {
        if (chip_no == CHIP1) {
            GPIO_SetBits(GPIOA, MRAM_HOLD1);
        } else {
            GPIO_SetBits(GPIOA, MRAM_HOLD2);
        }
    }
}
//----------------------------------------------------------------------------------------------------
//  SPI 字节读
//----------------------------------------------------------------------------------------------------
U8  SPI_ByteRd(void)
{
    return (SPI_ByteWr(DUMMY_BYTE));
}
//----------------------------------------------------------------------------------------------------
//  SPI 字节写
//----------------------------------------------------------------------------------------------------
U8 SPI_ByteWr(U8 data)
{
    U8  temp;

#ifdef   USE_EEPROM        // 使用EEPROM存储，需要写等待
  while (0);
#endif
  
    while ((SPI2->SR & 0x02) == 0); //等待发送缓冲空 TXE = 1;
    SPI2->DR = data;
    while ((SPI2->SR & 0x01) == 0); //等待接收缓冲非空 RXNE = 1;
    temp = SPI2->DR;

    return (temp);
}
//----------------------------------------------------------------------------------------------------
//  SPI 读MRAM状态寄存器
//----------------------------------------------------------------------------------------------------
U8 SPI_RdSR(void)
{
    U8  temp;

    SPI_ByteWr(RDSR);  
    temp = SPI_ByteWr(DUMMY_BYTE);

    return (temp);
}
//----------------------------------------------------------------------------------------------------
//  SPI 写MRAM状态寄存器
//----------------------------------------------------------------------------------------------------
void SPI_WrSR(U8 sr_val)
{
    SPI_ByteWr(WRSR);
    SPI_ByteWr(sr_val);
}
//----------------------------------------------------------------------------------------------------
//  判断写状态忙
//----------------------------------------------------------------------------------------------------
U8  Is_WrBusy(void)
{
    return (SPI_RdSR() & nRDY);
}
//----------------------------------------------------------------------------------------------------
//  SPI 读
//----------------------------------------------------------------------------------------------------
U8  SPI_Rd(U8 *ptr, U32 addr, U16 len)
{
    U8  ret = 0;
    U32 i;

    SPI_ByteWr(READ);
#ifndef   USE_256KBITS  // <=256K字节空间  
    SPI_ByteWr((addr&0xff0000) >> 16);  //地址 高位在前
#endif  
    SPI_ByteWr((addr&0xff00) >> 8);
    SPI_ByteWr((addr&0xff));
    for (i=0; i<len; i++) {
        *ptr++ = SPI_ByteRd();
    }

    return (ret);
}
//----------------------------------------------------------------------------------------------------
//  SPI 写
//----------------------------------------------------------------------------------------------------
U8  SPI_Wr(U8 *ptr, U32 addr, U16 len)
{
    U8  ret = 0;
    U32 i;

    SPI_ByteWr(WRITE);
#ifndef   USE_256KBITS  // <=256K字节空间
    SPI_ByteWr((addr&0xff0000) >> 16);  //地址 高位在前
#endif  
    SPI_ByteWr((addr&0xff00) >> 8);
    SPI_ByteWr((addr&0xff));
    for (i=0; i<len; i++) {
        SPI_ByteWr(*ptr++);
    }

    return (ret);
}
//----------------------------------------------------------------------------------------------------
//  SPI2 冲断处理程序
//----------------------------------------------------------------------------------------------------
void SPI2_IRQHandler(void)
{

}
//----------------------------------------------------------------------------------------------------
//  MR25HXX (磁阻RAM) 初始化
//----------------------------------------------------------------------------------------------------
void MRAM_Init(void)
{
    SPI_HOLD(CHIP1, 1); //hold1保持高
    SPI_HOLD(CHIP2, 1); //hold2保持高
    SPI_CS(CHIP1,1);    //拉高CS1
    SPI_CS(CHIP2,1);    //拉高CS2
    Delay(1000);

    SPI_CS(CHIP1,0);    //拉低CS1
    SPI_ByteWr(WREN);   //WEL = 1;
    SPI_CS(CHIP1,1);    //拉高CS1
    Delay(1000);

    SPI_CS(CHIP1,0);    //拉低CS1
    SPI_WrSR(SRWD);//|WEL); 15-1020
    SPI_CS(CHIP1,1);    //拉高CS1

#ifdef  USE_2MRAMs
    SPI_CS(CHIP1,1);    //拉高CS1

    SPI_CS(CHIP2,0);    //拉低CS2
    SPI_ByteWr(WREN);   //WEL = 1;
    SPI_CS(CHIP2,1);    //拉高CS2
    Delay(1000);

    SPI_CS(CHIP2,0);    //拉低CS2
    SPI_WrSR(SRWD);//|WEL); 15-1020
    SPI_CS(CHIP2,1);    //拉高CS2
#endif
}
//----------------------------------------------------------------------------------------------------
//  SPI 读 MRAM_SR
//----------------------------------------------------------------------------------------------------
U8  MRAM_RdSR(U8 chip_no)
{
    U8  ret;

    SPI_CS(chip_no, 0); // CSx = 0
    ret = SPI_RdSR();
    SPI_CS(chip_no, 1); // CSx = 1

    return (ret);
}
//----------------------------------------------------------------------------------------------------
//  SPI 写 MRAM_SR
//----------------------------------------------------------------------------------------------------
void MRAM_WrSR(U8 chip_no, U8 data)
{
    SPI_CS(chip_no, 0); // CSx = 0
    SPI_WrSR(data);
    SPI_CS(chip_no, 1); // CSx = 1
}
//----------------------------------------------------------------------------------------------------
//  SPI 读
//----------------------------------------------------------------------------------------------------
U8* MRAM_Rd(U8 chip_no, U8 *ptr, U32 addr, U16 len)
{
    U8  *p_ret = ptr;

    SPI_CS(chip_no, 0); // CSx = 0
    SPI_Rd(p_ret, addr, len);
    SPI_CS(chip_no, 1); // CSx = 1

    return (p_ret);
}
//----------------------------------------------------------------------------------------------------
//  SPI 写
//----------------------------------------------------------------------------------------------------
U8  MRAM_Wr(U8 chip_no, U8 *ptr, U32 addr, U16 len)
{
    U8  ret = 0;

    SPI_CS(chip_no, 0); // CSx = 0
    SPI_Wr(ptr, addr, len);
    SPI_CS(chip_no, 1); // CSx = 1

    return (ret);
}
//----------------------------------------------------------------------------------------------------
//eof
