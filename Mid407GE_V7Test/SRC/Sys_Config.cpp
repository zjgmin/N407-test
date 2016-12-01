//----------------------------------------------------------------------------------------------------
// Header:      系统参数配置
// File Name:   Sys_Cfg.c
// Author:      David
// Date:        2012-10-18
//----------------------------------------------------------------------------------------------------
#define     IN_SYS_CFG
    #include    "RTL.h"
    #include    <wchar.h>
    #include    <wctype.h>
    #include    "main.h"
    #include    "Sys_config.h"
    #include    "RTX_CAN.h"
//----------------------------------------------------------------------------------------------------
//  操作系统层面的初始化
//----------------------------------------------------------------------------------------------------
void Sys_OS_Init(void)
{
//    os_sem_init (COM_wr_sem, 1);    // 15-0815 comment
    os_sem_init (CtrlTick_sem, 0);  //控制节拍器信号量（由定时器1中断产生）

//    _init_box (COM_mpool, sizeof(COM_mpool), sizeof(CAN_msg)) ;  // 15-0815 comment

    _init_box (CanNet_mpool,    sizeof(CanNet_mpool), sizeof(CAN_msg)) ;
        
    os_mbx_init (MBX_MCan2Net,  sizeof(MBX_MCan2Net));
    os_mbx_init (MBX_SCan2Net,  sizeof(MBX_SCan2Net));
    os_mbx_init (MBX_Net2MCan,  sizeof(MBX_Net2MCan));
    os_mbx_init (MBX_Net2SCan,  sizeof(MBX_Net2SCan));

}
//----------------------------------------------------------------------------------------------------
//  人工跳转至用户代码
//----------------------------------------------------------------------------------------------------
//void ManualJump(U32 address)
//{
//    pFunc   Jump_To_Application;

//    Jump_To_Application = (pFunc) address;
//    Jump_To_Application();
//}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//eof
