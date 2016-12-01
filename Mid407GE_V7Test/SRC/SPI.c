//----------------------------------------------------------------------------------------------------
// Header:      SPI��ͨ�� �ŵ�MR25H40 4Mbit 0x80000
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
// �������Ŵ���
//----------------------------------------
#define   MRAM_CS1      (1ul << 1)  // �ŵ�Ƭѡ1  PB
#define   MRAM_CS2      (1ul << 0)  // �ŵ�Ƭѡ2  PB
#define   MRAM_HOLD1    (1ul << 5)  // �ŵ籣��1  PA
#define   MRAM_HOLD2    (1ul << 4)  // �ŵ籣��2  PA
//----------------------------------------------------------------------------------------------------
// Magnetoresistor RAM �������
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
// ״̬�Ĵ���λ
//----------------------------------------
#define   SRWD      (1ul << 7)    // ״̬�Ĵ���д��ֹ(���� /WP = 0 ʱ)
#define   WP_NULL   (0ul << 2)    // д������-NULL
#define   WP_UOQ    (1ul << 2)    // ������ - ��1/4��
#define   WP_HALF   (2ul << 2)    // ������ - һ��
#define   WP_ALL    (3ul << 2)    // ������ - ȫ��
#define   WEL       (1ul << 1)    // дʹ������
#define   nRDY      (1ul << 0)    // д����λ��0��������1��æ��
//----------------------------------------------------------------------------------------------------
//  SPI����   (SPI2)
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

    //PB11 ����ΪSPI2_SCK, PB14 ����ΪSPI2_MISO, PB15 ����ΪSPI2_MOSI,
    GPIOB->AFR[1] &= 0x00fff0ff;
    GPIOB->AFR[1] |= 0x55000500;

    /* Enable clock for SPI2 */
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
//    /* Reset SPI2 */
//    RCC->APB1RSTR |=  (1UL << 14);

    //SPI2 ��ʼ��
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
                | (0UL << 1)        //Clock polarity when idle //0:ʱ�����յ�
                | (0UL << 0);       //Clock phase //0:���ݲ����ڵ�1��ʱ����(��1λ�����ڴ���ǰ�ͽ���)

    SPI2->CR1 |= (1UL << 6);        //��SPI��

    MRAM_Init();
}
//----------------------------------------------------------------------------------------------------
//  Ƭѡ����
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
//  Hold�ߴ���
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
//  SPI �ֽڶ�
//----------------------------------------------------------------------------------------------------
U8  SPI_ByteRd(void)
{
    return (SPI_ByteWr(DUMMY_BYTE));
}
//----------------------------------------------------------------------------------------------------
//  SPI �ֽ�д
//----------------------------------------------------------------------------------------------------
U8 SPI_ByteWr(U8 data)
{
    U8  temp;

#ifdef   USE_EEPROM        // ʹ��EEPROM�洢����Ҫд�ȴ�
  while (0);
#endif
  
    while ((SPI2->SR & 0x02) == 0); //�ȴ����ͻ���� TXE = 1;
    SPI2->DR = data;
    while ((SPI2->SR & 0x01) == 0); //�ȴ����ջ���ǿ� RXNE = 1;
    temp = SPI2->DR;

    return (temp);
}
//----------------------------------------------------------------------------------------------------
//  SPI ��MRAM״̬�Ĵ���
//----------------------------------------------------------------------------------------------------
U8 SPI_RdSR(void)
{
    U8  temp;

    SPI_ByteWr(RDSR);  
    temp = SPI_ByteWr(DUMMY_BYTE);

    return (temp);
}
//----------------------------------------------------------------------------------------------------
//  SPI дMRAM״̬�Ĵ���
//----------------------------------------------------------------------------------------------------
void SPI_WrSR(U8 sr_val)
{
    SPI_ByteWr(WRSR);
    SPI_ByteWr(sr_val);
}
//----------------------------------------------------------------------------------------------------
//  �ж�д״̬æ
//----------------------------------------------------------------------------------------------------
U8  Is_WrBusy(void)
{
    return (SPI_RdSR() & nRDY);
}
//----------------------------------------------------------------------------------------------------
//  SPI ��
//----------------------------------------------------------------------------------------------------
U8  SPI_Rd(U8 *ptr, U32 addr, U16 len)
{
    U8  ret = 0;
    U32 i;

    SPI_ByteWr(READ);
#ifndef   USE_256KBITS  // <=256K�ֽڿռ�  
    SPI_ByteWr((addr&0xff0000) >> 16);  //��ַ ��λ��ǰ
#endif  
    SPI_ByteWr((addr&0xff00) >> 8);
    SPI_ByteWr((addr&0xff));
    for (i=0; i<len; i++) {
        *ptr++ = SPI_ByteRd();
    }

    return (ret);
}
//----------------------------------------------------------------------------------------------------
//  SPI д
//----------------------------------------------------------------------------------------------------
U8  SPI_Wr(U8 *ptr, U32 addr, U16 len)
{
    U8  ret = 0;
    U32 i;

    SPI_ByteWr(WRITE);
#ifndef   USE_256KBITS  // <=256K�ֽڿռ�
    SPI_ByteWr((addr&0xff0000) >> 16);  //��ַ ��λ��ǰ
#endif  
    SPI_ByteWr((addr&0xff00) >> 8);
    SPI_ByteWr((addr&0xff));
    for (i=0; i<len; i++) {
        SPI_ByteWr(*ptr++);
    }

    return (ret);
}
//----------------------------------------------------------------------------------------------------
//  SPI2 ��ϴ������
//----------------------------------------------------------------------------------------------------
void SPI2_IRQHandler(void)
{

}
//----------------------------------------------------------------------------------------------------
//  MR25HXX (����RAM) ��ʼ��
//----------------------------------------------------------------------------------------------------
void MRAM_Init(void)
{
    SPI_HOLD(CHIP1, 1); //hold1���ָ�
    SPI_HOLD(CHIP2, 1); //hold2���ָ�
    SPI_CS(CHIP1,1);    //����CS1
    SPI_CS(CHIP2,1);    //����CS2
    Delay(1000);

    SPI_CS(CHIP1,0);    //����CS1
    SPI_ByteWr(WREN);   //WEL = 1;
    SPI_CS(CHIP1,1);    //����CS1
    Delay(1000);

    SPI_CS(CHIP1,0);    //����CS1
    SPI_WrSR(SRWD);//|WEL); 15-1020
    SPI_CS(CHIP1,1);    //����CS1

#ifdef  USE_2MRAMs
    SPI_CS(CHIP1,1);    //����CS1

    SPI_CS(CHIP2,0);    //����CS2
    SPI_ByteWr(WREN);   //WEL = 1;
    SPI_CS(CHIP2,1);    //����CS2
    Delay(1000);

    SPI_CS(CHIP2,0);    //����CS2
    SPI_WrSR(SRWD);//|WEL); 15-1020
    SPI_CS(CHIP2,1);    //����CS2
#endif
}
//----------------------------------------------------------------------------------------------------
//  SPI �� MRAM_SR
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
//  SPI д MRAM_SR
//----------------------------------------------------------------------------------------------------
void MRAM_WrSR(U8 chip_no, U8 data)
{
    SPI_CS(chip_no, 0); // CSx = 0
    SPI_WrSR(data);
    SPI_CS(chip_no, 1); // CSx = 1
}
//----------------------------------------------------------------------------------------------------
//  SPI ��
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
//  SPI д
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
