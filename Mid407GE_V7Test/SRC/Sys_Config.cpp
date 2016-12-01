//----------------------------------------------------------------------------------------------------
// Header:      ϵͳ��������
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
//  ����ϵͳ����ĳ�ʼ��
//----------------------------------------------------------------------------------------------------
void Sys_OS_Init(void)
{
//    os_sem_init (COM_wr_sem, 1);    // 15-0815 comment
    os_sem_init (CtrlTick_sem, 0);  //���ƽ������ź������ɶ�ʱ��1�жϲ�����

//    _init_box (COM_mpool, sizeof(COM_mpool), sizeof(CAN_msg)) ;  // 15-0815 comment

    _init_box (CanNet_mpool,    sizeof(CanNet_mpool), sizeof(CAN_msg)) ;
        
    os_mbx_init (MBX_MCan2Net,  sizeof(MBX_MCan2Net));
    os_mbx_init (MBX_SCan2Net,  sizeof(MBX_SCan2Net));
    os_mbx_init (MBX_Net2MCan,  sizeof(MBX_Net2MCan));
    os_mbx_init (MBX_Net2SCan,  sizeof(MBX_Net2SCan));

}
//----------------------------------------------------------------------------------------------------
//  �˹���ת���û�����
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
