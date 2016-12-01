//----------------------------------------------------------------------------------------------------
// 系统配置
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#ifndef __sys_config_H
    #define __sys_config_H
//----------------------------------------------------------------------------------------------------
#ifdef  IN_SYS_CFG
    #define     ex_sys_cfg
#else  
    #define     ex_sys_cfg  extern 
#endif
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#ifdef      __cplusplus
    extern "C" 
    {
#endif
//----------------------------------------------------------------------------------------------------
#include    "RTX_CAN.h"
#include    "main.h"
//----------------------------------------------------------------------
ex_sys_cfg  OS_TID  CtrltskId;
ex_sys_cfg  OS_TID  UDPTsk_Id;
//ex_sys_cfg  OS_TID  RecTskId;
ex_sys_cfg  OS_TID  CabTskId;
ex_sys_cfg  OS_TID  SPITsKId;
ex_sys_cfg  OS_TID  CANTsk_Id[2];
//----------------------------------------------------------------------
//  消息邮箱数目定义 
//-----------------
//#define   COM_No                40
//----------
//15-0923 modified
#define   MCan2Net_Mbx_NUM      5   //20                // 主CAN到NET邮箱数目 
#define   SCan2Net_Mbx_NUM      5   //10                // 辅CAN到NET邮箱数目
#define   Net2MCan_Mbx_NUM      (MAX_NODE_NUM+1)        // NET到主CAN邮箱数目 16-0122
#define   Net2SCan_Mbx_NUM      5   //20                // NET到辅CAN邮箱数目

#define   CanNet_NUM            (MAX_NODE_NUM+1)        // CAN与NET内存池收发邮箱数目 16-0122

//----------------------------------------------------------------------
/* Declare memory pool for COM messages, both transmit and receive  */
//----------------------------------------------------------------------
//串口邮箱通信内存池
//----------------------------------------------------------------------
//ex_sys_cfg    CAN_msgpool_declare (COM_mpool, (COM_No));  // 15-0815 comment

//----------------------------------------------------------------------
//CAN同NET邮箱通信内存池
//----------------------------------------------------------------------
ex_sys_cfg  CAN_msgpool_declare (CanNet_mpool, (CanNet_NUM));

//----------------------------------------------------------------------
/* Declare mailbox, for COM transmit messages   */

ex_sys_cfg  mbx_declare (MBX_MCan2Net, MCan2Net_Mbx_NUM);   //unsigned int MBX_MCan2Net[20+4];
ex_sys_cfg  mbx_declare (MBX_SCan2Net, SCan2Net_Mbx_NUM);   //unsigned int MBX_SCan2Net[20+4];
ex_sys_cfg  mbx_declare (MBX_Net2MCan, Net2MCan_Mbx_NUM);   //unsigned int MBX_Net2MCan[chan_num+2+4];
ex_sys_cfg  mbx_declare (MBX_Net2SCan, Net2SCan_Mbx_NUM);   //unsigned int MBX_Net2SCan[20+4];


/*ex_sys_cfg    Semaphores used for protecting writing to CAN hardware  */
//----------------------------------------------------------------------------------------------------
//信号量
//ex_sys_cfg  OS_SEM  COM_wr_sem;// 15-0815 comment
ex_sys_cfg  OS_SEM  semETH;
ex_sys_cfg  OS_SEM  CtrlTick_sem;       //控制节拍器信号量

//SPI操作互斥
ex_sys_cfg  OS_MUT  SPI_mutex;

//----------------------------------------------------------------------------------------------------
//--------------------------------------------------

//ex_sys_cfg  _MicroCtrlInfo  MicroCtrlInfo   ;//配置表

//--------------------------------------------------

ex_sys_cfg  void    Sys_OS_Init(void);
ex_sys_cfg  void    ManualJump(unsigned int address);
//--------------------------------------------------
#ifdef      __cplusplus
    }
#endif
//----------------------------------------------------------------------------------------------------
#endif  /* __sys_config_H */
