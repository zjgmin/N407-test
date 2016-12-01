/*----------------------------------------------------------------------------
 *      RL-ARM - TCPnet
 *----------------------------------------------------------------------------
 *      Name:    CLIENT.C
 *      Purpose: LED Control Client demo example
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2011 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/  
//----------------------------------------------------------------------------------------------------
#define     IN_MAIN
//----------------------------------------------------------------------------------------------------
#include    "main.h"
#include    "rtl.h"
#include    "rtx_can.h" 
#include    "stm32f4xx.h"
#include    "GPIO.h"
#include    "spi.h"
#include    "timer.h"
#include    "UartSimCan.h"  // 16-0107
#include    "led.h"
#include    "net_config.h"
#include    "sys_config.h"
#include    "app_can.hpp"
#include    "Dev.hpp"
#include    "chan.hpp"
#include    "parallelchan.hpp"
//----------------------------------------------------------------------------------------------------
U64     tcpudp_stack[1024/8];       /* A bigger stack for tcp_task */
U64     ctr_stack   [1024/8];
U64     tsk_cab_stk [512 /8];
U64     tsk_SPI_stk [512 /8];
U64     tsk_CAN1_stk[1024/8];       //2013-1126
U64     tsk_CAN2_stk[512 /8];
//U8      socket_tcp;
U8      socket_udp;
U16     udp_callback (U8 soc, U8 *rip, U16 rport, U8 *buf, U16 len);    //UDP�ص�����

extern  U16     Loc_PORT;
//----------------------------------------------------------------------------------------------------
//  ��̫�����ݴ�������
//----------------------------------------------------------------------------------------------------
__task void tcpudp_task (void) 
{
//    Loc_PORT    = 1001;

//TCP
//    socket_tcp = tcp_get_socket (TCP_TYPE_SERVER, 0, 10, tcp_callback);
//    if (socket_tcp != 0) {
//        tcp_listen (socket_tcp, Loc_PORT);
//    }

    //UDP:��ʼ��ϵͳ �� ���� һ���õ�UDP Socket������� ͨ��
    socket_udp = udp_get_socket (0, UDP_OPT_SEND_CS | UDP_OPT_CHK_CS, udp_callback);
    if (socket_udp != 0) {
        udp_open (socket_udp, Loc_PORT);
    }
    while (1) {
//        os_sem_wait (semETH, 20);           //100mS
        os_sem_wait (semETH, ENET_TICK);    //20mS
//        time_poll()
        main_TcpNet ();
    }
}
//----------------------------------------------------------------------------------------------------
//  ��������
//----------------------------------------------------------------------------------------------------
__task void Ctrl_task (void) 
{
    static  U8  flash_cnt = 0;

    TIM1_Init();    // ���ƽ��Ķ�ʱ����ʼ��
    SRAM_Test();    // �ⲿSRAMӲ�����ϲ���
    while (1) 
    {
        if (os_sem_wait(CtrlTick_sem, CTRL_TICK*2) != OS_R_TMO) {
            if (++flash_cnt > CTRL_05S) {
                flash_cnt = 0;
                LED_Flash(LD5); // �����ƽ��� LD5��1�룩
        
#ifdef  EN_SRAM_TEST    // ʹ��SRAM����
                if (gSramErr) {
                    Test_LED_Flash(0);
                    Test_LED_Flash(1);
                    LED_Flash(LD4);
                }
#endif

            }
            ControlProc();
        } else {
            LED_Flash(LD4); //�����ƽ��ĳ�ʱ LD4
        }
//        os_dly_wait(CTRL_TICK);
    }
}
//----------------------------------------------------------------------------------------------------
// Task SPI MRAM����
//----------------------------------------------------------------------------------------------------
__task void TASK_SPI (void) 
{
    for (;;)
    {
        os_dly_wait(12000);
    }
}
//----------------------------------------------------------------------------------------------------
// Task CAN1 �շ�
//----------------------------------------------------------------------------------------------------
__task void TASK_CAN1 (void) 
{
    CAN_msg can_msg_rcv;
    static  U32 led_cnt = 0, off_cnt = 0;

    for (;;)
    {
        if (CAN_receive(1, &can_msg_rcv, CAN1_TICK) == CAN_OK) {    // CAN����(������λ��) 1 tick ��ʱ
            CAN_Recv_Process(CAN_NO1, &can_msg_rcv);
            off_cnt = 0;
            if (++led_cnt > (8)) {  //((1000 * CAN1_TICK) / TICK_IMG)) {
                led_cnt = 0;
        #ifdef  EN_CAN_BROADCAST   // ʹ��CAN����㲥 16-0113
                if (gFunSelBit == FUNC_BOARDCAST) { // �㲥�����ַ����Ϊ�㲥��ַ
                    LED_Flash(LD2); //CAN����㲥ָʾ
                    LED_Flash(LD3); //CAN����㲥ָʾ
                }
        #endif
                LED_Flash(LD7); //CAN1�շ�ָʾ LD7
            }
        } else {
            off_cnt++;
            if (off_cnt > CTRL_10S) {   //��ʱ��λCAN
                off_cnt = 0;
//                CAN_Init(B8_MASK_N[CAN_NO1]);
            }
            CAN_Send_Process(CAN_NO1);
        }
    }
}
//----------------------------------------------------------------------------------------------------
//  Task CAN2 �շ�
//----------------------------------------------------------------------------------------------------
__task void TASK_CAN2 (void) 
{
    CAN_msg can_msg_rcv;
    static  U32 led_cnt = 0, off_cnt = 0;

    for (;;)
    {
        if (CAN_receive(2, &can_msg_rcv, CAN2_TICK) == CAN_OK) {//CAN����(������λ��) 1 tick ��ʱ
            CAN_Recv_Process(CAN_NO2, &can_msg_rcv);
            off_cnt = 0;
            if (++led_cnt > (8)) {  //((1000 * CAN1_TICK) / TICK_IMG)) {
                led_cnt = 0;
                LED_Flash(LD6); //CAN2�շ�ָʾ LD6
            }
        } else {
            off_cnt++;
            if (off_cnt > CTRL_10S) {//��ʱ��λCAN
                off_cnt = 0;
//                CAN_Init(B8_MASK_N[CAN_NO2]);
            }
            CAN_Send_Process(CAN_NO2);
        }
    }
}  
//----------------------------------------------------------------------------------------------------
//  ��̨���� (MRAM��д)
//----------------------------------------------------------------------------------------------------
__task void main_task (void) 
{
    // U16 *PTR=((U16 *) (0x60000000));
    /* Add System initialisation code here */ 
    os_sem_init (semETH, 0);
    os_mut_init (SPI_mutex);
    gSimCanBit = SimCan_ReadBit();      // ģ��CANӲ���ж� 16-0107
    GPIO_Init();
    SPI_Init();
    SimCan_Init(gSimCanBit);            // ����ģ��CANӲ����ʼ�� 16-0107 
    LED_Init();
    Init_Dev();
    Init_ParallelChan();
    init_TcpNet();
    CAN_Init(B8_MASK_N[CAN_NO1] | B8_MASK_N[CAN_NO2]);
    
#ifdef  EN_RTC  // 15-0629
    RTC_Set();
#endif
    
    Sys_OS_Init();
    gEtherTout  = 0;
    gFunSelBit  = FuncSel_ReadWord();   // ȫ�����߹���λ
    /* Initialize Tasks */
    os_tsk_prio_self (100); //��
    CtrltskId       = os_tsk_create_user (Ctrl_task,    30, &ctr_stack,     sizeof(ctr_stack));     // ��������
    UDPTsk_Id       = os_tsk_create_user (tcpudp_task,  50, &tcpudp_stack,  sizeof(tcpudp_stack));  // �����շ�
    os_tsk_prio_self (20);  //��

//    CabTskId        = os_tsk_create_user (TASK_CabCtrl, 46, &tsk_cab_stk,   sizeof(tsk_cab_stk));   // �봲�����
    SPITsKId        = os_tsk_create_user (TASK_SPI,     60, &tsk_SPI_stk,   sizeof(tsk_SPI_stk));   // SPI����
    CANTsk_Id[0]    = os_tsk_create_user (TASK_CAN1,    47, &tsk_CAN1_stk,  sizeof(tsk_CAN1_stk));  // CAN1�շ�
    CANTsk_Id[1]    = os_tsk_create_user (TASK_CAN2,    45, &tsk_CAN2_stk,  sizeof(tsk_CAN2_stk));  // CAN2�շ�

    while(1) {
        if (gEtherTout++ > 100) {   //10������̫������
            gEtherTout = 101;
//            init_ethernet();
            // IP��ַ��λ�ж� 16-0602
            IP_Reset_Test();
        }
        //----------
        os_dly_wait(20);
        timer_tick (); /* TCP/IP  Timer tick every 100 ms */    
    }
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
int main (void) 
{
    /* Start with 'init' task. */
    os_sys_init(main_task);
    while(1);
}
//----------------------------------------------------------------------------------------------------
//  ��ʱ����
//----------------------------------------------------------------------------------------------------
void Delay(U32 nCount)  //10000 --> 820uS @ 64MHz
{
    for (; nCount != 0; nCount--);
}

//====================================================================================================
 #ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
    /* 
    User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line)
    */

    /* Infinite loop */
    while (1) {
    
    }
}
#endif

//====================================================================================================
#define     OS_CLOCK_CNT        (168000000 / 50)
//----------------------------------------------------------------------------
void OsError_Display (void)
{
    static U32  err_cnt = 0;

    ++err_cnt;
    if (err_cnt >= OS_CLOCK_CNT/16)     LED_On(5);  else LED_Off(5);
    if (err_cnt >= OS_CLOCK_CNT/14)     LED_On(4);  else LED_Off(4);
    if (err_cnt >= OS_CLOCK_CNT/12)     LED_On(3);  else LED_Off(3);
    if (err_cnt >= OS_CLOCK_CNT/10)     LED_On(2);  else LED_Off(2);
    if (err_cnt >= OS_CLOCK_CNT/8)      LED_On(1);  else LED_Off(1);
    if (err_cnt >= OS_CLOCK_CNT/6)      LED_On(0);  else LED_Off(0);
    if (++err_cnt >= OS_CLOCK_CNT/4) {
        err_cnt = 0;
        LED_Flash(LD2);
    }
}
//----------------------------------------------------------------------------
void HardFault_Display (void)
{
    static  U32 fault_cnt   = 0;

    while (1) {
        ++fault_cnt;

        if (fault_cnt >= OS_CLOCK_CNT/8) LED_On(0); else LED_Off(0);
        if (fault_cnt >= OS_CLOCK_CNT/7) LED_On(1); else LED_Off(1);
        if (fault_cnt >= OS_CLOCK_CNT/6) LED_On(2); else LED_Off(2);
        if (fault_cnt >= OS_CLOCK_CNT/5) LED_On(3); else LED_Off(3);
        if (fault_cnt >= OS_CLOCK_CNT/4) LED_On(4); else LED_Off(4);
        if (fault_cnt >= OS_CLOCK_CNT/3) LED_On(5); else LED_Off(5);
        if (++fault_cnt >= OS_CLOCK_CNT/2) {
            fault_cnt   = 0;
            LED_Flash(LD3);
        }
    }
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
