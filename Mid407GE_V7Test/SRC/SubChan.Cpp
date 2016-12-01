//----------------------------------------------------------------------------------------------------
// Header:      辅助通道处理程序
// File Name:   SubChan.cpp
// Author:      David
// Date:        2012-12-26
//----------------------------------------------------------------------------------------------------
#define  _IN_SUBCHAN_CPP
extern "C" 
{
    #include    "RTL.h"
    #include    "main.h"
    #include    "spi.h"
    #include    "app_config.h"
}
#include    "Dev.hpp"
#include    "app_can.hpp"
#include    "SubChan.hpp"
//----------------------------------------------------------------------------------------------------
//构造函数
//----------------------------------------------------------------------------------------------------
SubChan::SubChan()
{
    static char s_inx = 0;
    U32         i;

    mAddr   = s_inx;
    s_inx++;

    for (i=0; i<(MAX_VI_BOARD * MAX_BOARDVI_NUM); i++) {
        Vi[i]   = 0;
    }
    for (i=0; i<(MAX_TI_BOARD * MAX_BOARDTI_NUM); i++) {
        Ti[i]   = DEF_TMP_INT[1];  // 16-0413 -273
    }
    for (i=0; i<(MAX_II_BOARD * MAX_BOARDII_NUM / 32); i++) {
        Ii[i]   = 0;
    }
    for (i=0; i<(MAX_OI_BOARD * MAX_BOARDOI_NUM / 32); i++) {
        Oi[i]   = 0;
    }
    for (i=0; i<(MAX_XI_BOARD * MAX_BOARDXI_NUM / 32); i++) {   // 15-1229 -> 扩展IO
        Xi[i]   = 0;
    }
    for (i=0; i<MAX_CHAN_NUM; i++) {
        
#ifdef  EN_SIM_CHAN //允许脱机测试 
        mMinVi[i] = 2000+MAX_BOARDVI_NUM-1;
        mMaxVi[i] = 2000;                       
        mMinTi[i] = 250 +MAX_BOARDTI_NUM-1;
        mMaxTi[i] = 250;
#else        
        mMinVi[i] = 0;
        mMaxVi[i] = 0;                       
        mMinTi[i] = DEF_TMP_INT[1]; //15-1203
        mMaxTi[i] = DEF_TMP_INT[1];
#endif        
    }
}
//----------------------------------------------------------------------------------------------------
//  发送默认辅助通道数据扫描命令：读通道1
//----------------------------------------------------------------------------------------------------
void SubChan::CanSendProc(CAN_msg *msg)
{
    msg->id         = SCAN_RD_REALDATA1 | (1<<8);
    msg->data[0]    = 0;
    msg->data[1]    = 0;
    msg->data[2]    = 0;
    msg->data[3]    = 0;
    msg->data[4]    = 0;
    msg->data[5]    = 0;
    msg->data[6]    = 0;
    msg->data[7]    = 0;
    msg->len        = 8;
    msg->format     = EXTENDED_FORMAT;
    msg->type       = DATA_FRAME;
}
//----------------------------------------------------------------------------------------------------
//  拷贝开关量状态数据
/*
    参数    *ptr :  代拷贝数据缓冲（指向：数据循环数）
            hver:   硬件布局版本号
    返回    len :   拷贝数据长度

    备注：  根据命令版本绝对拷贝的数据格式，跟着实时数据后上传PC
            0x81命令调用
*/
//----------------------------------------------------------------------------------------------------
U16 SubChan::SetIOData(U8 *ptr, U8 hver)
{
    U16 len = 0x00;

    switch (hver) 
    {
    default:
    case DS_GENERAL:    // 通用类型设备结构
    case DS_POURING:    // 倒出类型设备结构
    case DS_OPENSHELF:  // 开架类型设备结构
    case DS_NEILSBED:   // 针床类型设备结构 - 96回路，3层针床，4托盘每层，8电池每托盘   
        *ptr++ = 0x00;
        *ptr++ = 0x00;                     

        *ptr++ = 3;                                     //针床层(区)数 - 3个
 
        *ptr++ = 0;    
        *ptr++ = 0;   
        *ptr++ = 0;   
        *ptr++ = 0;    

        *ptr++ = 0;   
        *ptr++ = 0;  
        *ptr++ = 0;
        *ptr++ = 0;

        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;

        len   += 15;
        break;
        
    }

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
//eof
