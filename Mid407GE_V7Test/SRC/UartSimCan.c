//----------------------------------------------------------------------------------------------------
// Header:      串口模拟CAN通信 底层数据接收后交付CAN应用层，CAN应用层数据发送交付串口
// File Name:   UartSimCan.c
// Author:      Xld
// Date:        2016-01-07
// Editor:      David
//----------------------------------------------------------------------------------------------------
#include    "RTL.h"             /* RTX kernel functions & defines      */
#include    "RTX_CAN.h"         /* CAN Generic functions & defines     */
#include    "stm32f4xx.h"       /* STM32F40x register definitions      */
#include    "UartSimCan.h"
#include    "main.h"
//----------------------------------------------------------------------------------------------------
#define     UARTBAUDRATE    (500000)
#define     U1BAUDRATE      UARTBAUDRATE
#define     U3BAUDRATE      UARTBAUDRATE
#define     PCLK1           (42000000)      //168Mhz/4=42Mhz    APB1    MAX
#define     PCLK2           (84000000)      //168Mhz/2=84Mhz    APB2    MAX

//--------------------
// 模拟CAN重发处理宏 16-0528

#define     TX_RETRY_NUM    (0)                     // 重发次数
#define     TX_RETRY_TIME   (2.5)                   // 重发间隔（mS）
#define     TX_RETRY_TICK   (1000/TX_RETRY_TIME)    // 重发节拍

//----------------------------------------------------------------------------------------------------
U8  U1DmaRxd[0x100];
U8  U1DmaTxd[0x100];
S32 U1ReTxcnt = 0;      // U1重发计数器 16-0521 
extern  U8  bInitMpool; // 内存池初始化标记
/*----------------------------------------------------------------------------
    STM32 USART setup.
    initializes the USARTx register
 *----------------------------------------------------------------------------*/
void Uart1SimCan_Init (void) 
{
    // USART1 used   APB2  84MHZ

    RCC->AHB1ENR    |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER    &= 0xFFC3FFFF;          // PA 9,10 Uart1 tx rx
    GPIOA->MODER    |= 0x00280000;
    GPIOA->AFR[1]   &= 0xFFFFF00F;
    GPIOA->AFR[1]   |= 0x00000770;

    NVIC_SetPriority(USART1_IRQn, 2);       // M0优先级  0-3 3最低 
    NVIC_EnableIRQ(USART1_IRQn);

    RCC->APB2RSTR   |= RCC_APB2RSTR_USART1RST;         
    RCC->APB2RSTR   &= ~RCC_APB2RSTR_USART1RST;        
    RCC->APB2ENR    |= RCC_APB2ENR_USART1EN;    /* Enable USART clock */

    USART1->CR1     &= (U32)~((U32)USART_CR1_UE);
    USART1->CR1     |= (U32)(USART_CR1_TE|USART_CR1_RE);
    USART1->CR3     &= (U32)~((U32)(USART_CR3_RTSE | USART_CR3_CTSE));
    USART1->CR3     |= USART_CR3_DMAT |USART_CR3_DMAR;  // DE function is enabled.  DMA enable transmitter & receiver
    /* Write to USART BRR */
    USART1->BRR     = (uint16_t)(PCLK2  / U1BAUDRATE);  
    USART1->CR1     |= (U32)USART_CR1_UE ;      // USART enable
    USART1->CR1     |= (U32)USART_CR1_IDLEIE;   // USART_CR1_RTOIE;//USART_CR1_RXNEIE;

    RCC->AHB1ENR    |= RCC_AHB1ENR_DMA2EN;      /*!< DMA2 clock enable */
    // USART enable
    /* DMA2_Stream5 Config */
    DMA2_Stream5->CR    &= (U16)(~DMA_SxCR_EN); /* Disable the selected DMAy Channelx */
    DMA2_Stream5->CR    = DMA_SxCR_CIRC 
                        | DMA_SxCR_MINC 
                        | DMA_SxCR_PL_1;        /* Write to DMAy Channelx CCR */
    DMA2_Stream5->CR    |= DMA_SxCR_CHSEL_2;    //CHANNLE 4
    DMA2_Stream5->NDTR  = 0x100;                /* Write to DMAy Channelx CNDTR */
    DMA2_Stream5->PAR   = (U32)&(USART1->DR);   /*USART2->RDR Address*/
    DMA2_Stream5->M0AR  = (U32)U1DmaRxd;        /* Write to DMAy Channelx CMAR */
    DMA2_Stream5->CR    |= DMA_SxCR_EN;         /* Enable the selected DMAy Channelx */


    /* DMA1 Channel4 Config */
    NVIC_SetPriority(DMA2_Stream7_IRQn, 3);     //M0???  0-15 15??
    NVIC_EnableIRQ(DMA2_Stream7_IRQn);
    DMA2_Stream7->CR    &= (U16)(~DMA_SxCR_EN); /* Disable the selected DMAy Channelx */
    DMA2_Stream7->CR    = DMA_SxCR_TCIE
                        | DMA_SxCR_DIR_0
                        | DMA_SxCR_MINC
                        | DMA_SxCR_PL_1;        /* Write to DMAy Channelx CCR */
    DMA2_Stream7->CR    |= DMA_SxCR_CHSEL_2;    //CHANNLE 4
    DMA2_Stream7->NDTR  = 0x10;                 /* Write to DMAy Channelx CNDTR */
    DMA2_Stream7->PAR   = (U32)&(USART1->DR);   /*USART1->TDR Address*/
    DMA2_Stream7->M0AR  = (U32)U1DmaTxd;        /* Write to DMAy Channelx CMAR */
    DMA2_Stream7->CR    |= DMA_SxCR_EN;         /* Enable the selected DMAy Channelx */
    
//    if (bInitMpool == 0) {
//        bInitMpool = 1;
//        _init_box (CAN_mpool, sizeof(CAN_mpool), sizeof(CAN_msg));
//    }
//    os_sem_init (wr_sem[0], 1);                             // 初始化，未用
//    os_mbx_init (MBX_tx_ctrl[0], sizeof(MBX_tx_ctrl[0]));   // 初始化，未用
//    os_mbx_init (MBX_rx_ctrl[0], sizeof(MBX_rx_ctrl[0]));
};
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
//  DMA_HISR_TCIF7

void DMA2_Stream7_IRQHandler(void)
{   //USART1  发送结束DMA 中断
    DMA2->HIFCR |= (DMA_HISR_DMEIF7 
                |   DMA_HISR_TCIF7 
                |   DMA_HISR_HTIF7 
                |   DMA_HISR_TEIF7);            /* Reset interrupt pending bits for DMA1 Channel1 */
    DMA2_Stream7->CR    &= ~DMA_SxCR_EN;
	USART1->CR1 |= USART_CR1_IDLEIE;            // 16-0521
}
//----------------------------------------------------------------------------------------------------
//  USART1 接收IDLE 超时中断
//----------------------------------------------------------------------------------------------------
U32 lenU0;
//--------------------------------------------------
void USART1_IRQHandler(void)
{
    CAN_msg *ptrmsg;
    U32     i, len  = 0;
    
    USART1->SR  |= USART_SR_IDLE|USART_SR_RXNE; //USART_ICR_RTOCF;//clear Receiver timeout flag
    USART1->CR1 &= (0xFFFFFFFF ^ USART_CR1_IDLEIE);
    lenU0   =
    len     = 0x100 - DMA2_Stream5->NDTR;
    if ((len == 14) 
    &&  (U1DmaRxd[0] & 0x80)) {     //正常接收的数据  长度
        i = Crc16(U1DmaRxd,12);
        if (i != (U1DmaRxd[12]|(U1DmaRxd[13]<<8))) {
            return;
        }
        U1ReTxcnt = 0;  // 收到合法数据，清超时
        if (isr_mbx_check (MBX_rx_ctrl[0]) > 0) {
            ptrmsg = _alloc_box (CAN_mpool);
            if (ptrmsg) {
                ptrmsg->id      = U1DmaRxd[0]
                                |(U1DmaRxd[1]<<8)
                                |(U1DmaRxd[2]<<16)
                                |(U1DmaRxd[3]<<24);    /* Read received message */
                ptrmsg->data[0] = U1DmaRxd[4];
                ptrmsg->data[1] = U1DmaRxd[5];
                ptrmsg->data[2] = U1DmaRxd[6];
                ptrmsg->data[3] = U1DmaRxd[7];
                ptrmsg->data[4] = U1DmaRxd[8];
                ptrmsg->data[5] = U1DmaRxd[9];
                ptrmsg->data[6] = U1DmaRxd[10];
                ptrmsg->data[7] = U1DmaRxd[11];
                isr_mbx_send (MBX_rx_ctrl[0], ptrmsg);
            }
        }
    }
    DMA2->HIFCR |= (DMA_HISR_DMEIF5 
                |   DMA_HISR_TCIF5 
                |   DMA_HISR_HTIF5 
                |   DMA_HISR_TEIF5);            /* Reset interrupt pending bits for DMA1 Channel1 */
    DMA2_Stream5->CR    &= (U16)(~DMA_SxCR_EN); /* Disable the selected DMAy Channelx */
    DMA2_Stream5->NDTR  = 0x100;                /* Write to DMAy Channelx CNDTR */
    DMA2_Stream5->CR    |= DMA_SxCR_EN;         /* Enable the selected DMAy Channelx */
}
//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------
U8  U3DmaRxd[0x100];
U8  U3DmaTxd[0x100];
S32 U3ReTxcnt = 0;  // U3重发计数器 16-0521
//--------------------------------------------------
void Uart3SimCan_Init (void) 
{
    RCC->AHB1ENR    |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER    &= 0xFF0FFFFF;      // PC10,11 Uart3 tx rx
    GPIOC->MODER    |= 0x00A00000;
    GPIOC->AFR[1]   &= 0xFFFF00FF;
    GPIOC->AFR[1]   |= 0x00007700;


    NVIC_SetPriority(USART3_IRQn, 2);   // M0优先级  0-3 3最低
    NVIC_EnableIRQ(USART3_IRQn);
    // USART2 used
    RCC->APB1RSTR   |= RCC_APB1RSTR_USART3RST;
    RCC->APB1RSTR   &=~RCC_APB1RSTR_USART3RST; 
    RCC->APB1ENR    |= RCC_APB1ENR_USART3EN;        /* Enable USART clock */
    USART3->CR1     &= (U32)~((U32)USART_CR1_UE);
//    USART3->CR2     |= (U32)USART_CR2_STOP_1;     //2STOP bit
    USART3->CR1     |= (U32) USART_CR1_RE | USART_CR1_TE;
    USART3->CR3     &= (U32)~((U32)(USART_CR3_RTSE | USART_CR3_CTSE));
    USART3->CR3     |= USART_CR3_DMAT |USART_CR3_DMAR;  //DE function is enabled.  DMA enable transmitter & receiver

    USART3->BRR     = (uint16_t)(PCLK1 / U3BAUDRATE);  
    USART3->CR1     |= USART_CR1_IDLEIE;            //USART_CR1_RXNEIE;
    USART3->SR      &= (U32)(~USART_SR_TC);
    USART3->CR1     |= USART_CR1_UE;                // USART enable
    // end USART2 used

    RCC->AHB1ENR    |=RCC_AHB1ENR_DMA1EN;           /*!< DMA1 clock enable */
    // USART enable
      /* DMA1_Stream1 Config */
    DMA1_Stream1->CR    &= (U16)(~DMA_SxCR_EN);     /* Disable the selected DMAy Channelx */
    DMA1_Stream1->CR    = DMA_SxCR_CIRC 
                        | DMA_SxCR_MINC 
                        | DMA_SxCR_PL_1;            /* Write to DMAy Channelx CCR */
    DMA1_Stream1->CR    |= DMA_SxCR_CHSEL_2;        //CHANNLE 4
    DMA1_Stream1->NDTR  = 0x100;                    /* Write to DMAy Channelx CNDTR */
    DMA1_Stream1->PAR   = (U32)&(USART3->DR);       /*USART3->RDR Address*/
    DMA1_Stream1->M0AR  = (U32)U3DmaRxd;            /* Write to DMAy Channelx CMAR */
    DMA1_Stream1->CR    |= DMA_SxCR_EN;             /* Enable the selected DMAy Channelx */


    /* DMA1_Stream3 Config */
    NVIC_SetPriority(DMA1_Stream3_IRQn, 3);         //M0???  0-15 15??
    NVIC_EnableIRQ(DMA1_Stream3_IRQn);
    DMA1_Stream3->CR    &= (U16)(~DMA_SxCR_EN);     /* Disable the selected DMAy Channelx */
    DMA1_Stream3->CR    = DMA_SxCR_TCIE
                        | DMA_SxCR_DIR_0 
                        | DMA_SxCR_MINC 
                        | DMA_SxCR_PL_1;            /* Write to DMAy Channelx CCR */
    DMA1_Stream3->CR    |= DMA_SxCR_CHSEL_2;        //CHANNLE 4
    DMA1_Stream3->NDTR  = 0x10;                     /* Write to DMAy Channelx CNDTR */
    DMA1_Stream3->PAR   = (U32)&(USART3->DR);       /*USART3->TDR Address*/
    DMA1_Stream3->M0AR  = (U32)U3DmaTxd;            /* Write to DMAy Channelx CMAR */
    DMA1_Stream3->CR    |= DMA_SxCR_EN;             /* Enable the selected DMAy Channelx */
    
//    if (bInitMpool == 0) {
//        bInitMpool = 1;
//        _init_box (CAN_mpool, sizeof(CAN_mpool), sizeof(CAN_msg));
//    }
//    os_sem_init (wr_sem[1], 1);                             // 初始化，未用
//    os_mbx_init (MBX_tx_ctrl[1], sizeof(MBX_tx_ctrl[1]));   // 初始化，未用
//    os_mbx_init (MBX_rx_ctrl[1], sizeof(MBX_rx_ctrl[1]));
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
void DMA1_Stream3_IRQHandler(void)
{   //USART1  发送结束DMA 中断
    DMA1->LIFCR |= (DMA_LISR_DMEIF3 
                |   DMA_LISR_TCIF3 
                |   DMA_LISR_HTIF3 
                |   DMA_LISR_TEIF3);    /* Reset interrupt pending bits for DMA1_Stream3 */
    DMA1_Stream3->CR    &= ~DMA_SxCR_EN;
    USART3->CR1 |= USART_CR1_IDLEIE;    // U1->U3 16-0521
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
void USART3_IRQHandler(void)
{   //USART3 接收IDLE 超时中断
    CAN_msg *ptrmsg;
    U32     len = 0, i;
    
    USART3->SR  |= USART_SR_IDLE|USART_SR_RXNE;     //USART_ICR_RTOCF;//clear Receiver timeout flag
    USART3->CR1 &= (0xFFFFFFFF ^ USART_CR1_IDLEIE);
    len = 0x100 - DMA1_Stream1->NDTR;
    DMA1->LIFCR |= (DMA_LISR_DMEIF1 
                |   DMA_LISR_TCIF1 
                |   DMA_LISR_HTIF1 
                |   DMA_LISR_TEIF1);                /* Reset interrupt pending bits for DMA1 Channel1 */
    DMA1_Stream1->CR    &= (U16)(~DMA_SxCR_EN);     /* Disable the selected DMAy Channelx */
    DMA1_Stream1->NDTR  = 0x100;                    /* Write to DMAy Channelx CNDTR */
    DMA1_Stream1->CR    |= DMA_SxCR_EN;             /* Enable the selected DMAy Channelx */
    if ((len == 14)
    &&  (U3DmaRxd[0] & 0x80)) {     //正常接收的数据  长度
        i = Crc16(U3DmaRxd,12);
        if (i != (U3DmaRxd[12]|(U3DmaRxd[13]<<8))) {
            return;
        }
        U3ReTxcnt = 0;
        if (isr_mbx_check (MBX_rx_ctrl[1]) > 0) {
            ptrmsg = _alloc_box (CAN_mpool);
            if (ptrmsg) {
                ptrmsg->id      = U3DmaRxd[0]
                                |(U3DmaRxd[1]<<8)
                                |(U3DmaRxd[2]<<16)
                                |(U3DmaRxd[3]<<24);     /* Read received message */
                ptrmsg->data[0] = U3DmaRxd[4];
                ptrmsg->data[1] = U3DmaRxd[5];
                ptrmsg->data[2] = U3DmaRxd[6];
                ptrmsg->data[3] = U3DmaRxd[7];
                ptrmsg->data[4] = U3DmaRxd[8];
                ptrmsg->data[5] = U3DmaRxd[9];
                ptrmsg->data[6] = U3DmaRxd[10];
                ptrmsg->data[7] = U3DmaRxd[11];
                isr_mbx_send (MBX_rx_ctrl[0], ptrmsg);
            }
        }
    }
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
void  TIM2_IRQHandler(void)
{
    TIM2->SR    &= ~(TIM_SR_UIF);  
    USART1->CR1 |= USART_CR1_IDLEIE;
    if(U1ReTxcnt > 0)
    {
        U1ReTxcnt--;
        DMA2_Stream7->NDTR  = 14;               //+2;/* Write to DMAy Channelx CNDTR */
        DMA2_Stream7->CR    |= DMA_SxCR_EN;     /* Enable the selected DMAy Channelx */			
    }
}
/*----------------------------------------------------------------------------
  initialize TIM2  2MS
 *----------------------------------------------------------------------------*/
void TIM2_Init (void) 
{
    // TIM2 used 50khz 定时器  APB1TIMER CLOKS 84MHZ(42MHZX2) 
    RCC->APB1ENR    |= RCC_APB1ENR_TIM2EN;  // enable clock for TIM3
    TIM2->PSC   = 0;                    // set prescaler
    TIM2->CNT   = 0;                    // 清零 16-0521
    TIM2->CR1   = 0;                    // reset command register 1
    TIM2->CR2   = 0;                    // reset command register 2
    TIM2->ARR   = (PCLK1*2)/(TX_RETRY_TICK)-1;    // set auto-reload 超时 1/800 = 1.25mS
    TIM2->CCR1  = 0x10;                 //
    TIM2->CCR2  = 0x10;                 //
    TIM2->CCMR1 = 0x7070;               //
    TIM2->CCER  = TIM_CCER_CC2E 
                | TIM_CCER_CC1E;        // 0x01;
    TIM2->CR1   = TIM_CR1_URS;          // set command register 1
    TIM2->CR2   = 0x0000;               // set command register 2
    // interrupts used
    TIM2->DIER  = TIM_DIER_UIE;         // enable interrupt
    NVIC_SetPriority(TIM2_IRQn, 3);     // M0优先级  0-3 3最低
    NVIC_EnableIRQ(TIM2_IRQn);    
    TIM2->CR1 |= TIM_CR1_CEN;           // enable timer 2
}

//----------------------------------------------------------------------------------------------------
void  TIM3_IRQHandler(void) // 16-0521 added
{
    TIM3->SR    &= ~(TIM_SR_UIF);  
    USART3->CR1 |= USART_CR1_IDLEIE;
    if(U3ReTxcnt > 0)
    {
        U3ReTxcnt--;
        DMA1_Stream3->NDTR  = 14;               //+2;/* Write to DMAy Channelx CNDTR */
        DMA1_Stream3->CR    |= DMA_SxCR_EN;     /* Enable the selected DMAy Channelx */			
    }
}
/*----------------------------------------------------------------------------
  initialize TIM3  1MS
 *----------------------------------------------------------------------------*/
void TIM3_Init (void)       // 16-0521 added
{
    // TIM3 used 1ms 定时器  APB1TIMER CLOKS 84MHZ(42MHZX2) 
    RCC->APB1ENR    |= RCC_APB1ENR_TIM3EN;  // enable clock for TIM3
    TIM3->PSC   = 0;                    // set prescaler
    TIM3->CR1   = 0;                    // reset command register 1
    TIM3->CR2   = 0;                    // reset command register 2
    TIM3->ARR   = (PCLK1*2)/(TX_RETRY_TICK)-1;   // set auto-reload
    TIM3->CCR1  = 0x10;                 //
    TIM3->CCR2  = 0x10;                 //
    TIM3->CCMR1 = 0x7070;               //
    TIM3->CCER  = TIM_CCER_CC2E 
                | TIM_CCER_CC1E;        // 0x01;
    TIM3->CR1   = TIM_CR1_URS;          // set command register 1
    TIM3->CR2   = 0x0000;               // set command register 2
    // interrupts used
    TIM3->DIER  = TIM_DIER_UIE;         // enable interrupt
    NVIC_SetPriority(TIM3_IRQn, 3);     // M0优先级  0-3 3最低
    NVIC_EnableIRQ(TIM3_IRQn);    
    TIM3->CR1 |= TIM_CR1_CEN;           // enable timer 2
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
void SimCan_Init (U32 bits)
{
//    TIM2_Init();
//    TIM3_Init();    // 16-0521
    if (bits & 0x01) {
        TIM2_Init();
        Uart1SimCan_Init();
    }
    if (bits & 0x02) {
        Uart3SimCan_Init();
        TIM3_Init();    // 16-0521
    }
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
CAN_ERROR SimCAN_hw_wr (U32 ctrl, CAN_msg *msg)  
{
    U8  *ptr;
    S32 i;
    static  U32 ccnt = 0;
    
    if (ctrl ==1)
    {
        ptr = ((U8*)msg);
        for (i=0; i<12; i++) {
            U1DmaTxd[i] = ptr[i];
        }
        if (ptr[0] == 0x50) {
            ccnt ++;
        }
        i = Crc16(U1DmaTxd,12);
        U1DmaTxd[12] = i; 
        U1DmaTxd[13] = (i >> 8);
        for (i=0; i<14; i++) {
            ((U8 *)msg)[i] = U1DmaTxd[i];
        }
        DMA2_Stream7->NDTR  = 14;               //+2;/* Write to DMAy Channelx CNDTR */
        DMA2_Stream7->CR    |= DMA_SxCR_EN;     /* Enable the selected DMAy Channelx */
        USART1->CR1         |= USART_CR1_IDLEIE;
    }
    else if (ctrl == 2)
    {
        for (i=0; i<12; i++) {
            U3DmaTxd[i] = ((U8*)msg)[i];
        }
        i = Crc16(U3DmaTxd,12);
        U3DmaTxd[12] = i;
        U3DmaTxd[13] = (i >> 8);
        for (i=0; i<14; i++) {
            ((U8 *)msg)[i]=U3DmaTxd[i];
        }
        DMA1_Stream3->NDTR  = 14;               //+2;/* Write to DMAy Channelx CNDTR */
        DMA1_Stream3->CR    |= DMA_SxCR_EN;     /* Enable the selected DMAy Channelx */
        USART3->CR1         |= USART_CR1_IDLEIE;
    }
    return  CAN_OK; //CAN_TX_BUSY_ERROR
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
 CAN_ERROR SimCAN_push (U32 ctrl, CAN_msg *msg, U16 timeout)  
{
    // 发送超时计数初始化 16-0521
    if (ctrl == 1) {
        U1ReTxcnt = TX_RETRY_NUM;//4;
        TIM2->CNT = 0;
    } 
    else if (ctrl == 2) {
        U3ReTxcnt = TX_RETRY_NUM;//4;
        TIM3->CNT = 0;
    }
    
    SimCAN_hw_wr (ctrl, msg);   /* Send message         */
    
    return CAN_OK;
}
//----------------------------------------------------------------------------------------------------
//eof
