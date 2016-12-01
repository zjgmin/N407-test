//----------------------------------------------------------------------------------------------------
#ifndef __SUBCHAN_HPP
    #define __SUBCHAN_HPP

#include    "rtx_can.h"
#include    "Dev.hpp"
#include    "chan.hpp"
//----------------------------------------------------------------------------------------------------
//C++类定义
//-------------------------------------------------------
#ifdef      _IN_SUBCHAN_CPP
    #define     subchan_ext
#else
    #define     subchan_ext extern
#endif
//----------------------------------------------------------------------------------------------------
//各种辅助参数通道CAN固定起始地址,线性排列
#define     VIADDR_OFFSET       (0x00)                              // 电压单体辅助通道起始CAN偏移地址 1~40
#define     TIADDR_OFFSET       (VIADDR_OFFSET + MAX_VI_BOARD)      // 温度单体辅助通道起始CAN偏移地址 41~80
#define     IIADDR_OFFSET       (TIADDR_OFFSET + MAX_TI_BOARD)      // 开关量入辅助通道起始CAN偏移地址 81~100
#define     OIADDR_OFFSET       (IIADDR_OFFSET + MAX_II_BOARD)      // 开关量出辅助通道起始CAN偏移地址 101~120
#define     XIADDR_OFFSET       (OIADDR_OFFSET + MAX_OI_BOARD)      // 其他扩展辅助通道起始CAN偏移地址 121~128

#define     EBIADDR_OFFSET      (0x80)                              // 扩展位输入辅助通道起始CAN偏移地址 129~
#define     EBOADDR_OFFSET      (EBIADDR_OFFSET + MAX_EBI_BOARD)    // 扩展位输出辅助通道起始CAN偏移地址
#define     EDIADDR_OFFSET      (EBOADDR_OFFSET + MAX_EBO_BOARD)    // 扩展数据输入辅助通道起始CAN偏移地址
#define     EDOADDR_OFFSET      (EDIADDR_OFFSET + MAX_EDI_BOARD)    // 扩展数据输出辅助通道起始CAN偏移地址
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
//----------------------------------------------------------------------------------------------------
//类结构定义
//----------------------------------------------------------------------------------------------------
//定义辅助通道类 15-1119 与主通道一一映射
//----------------------------------------------------------------------------------------------------
class SubChan //: public Chan
{
private:
    U8      mAddr;              // 辅助通道号 

public:

    S16     Vi[MAX_VI_BOARD * MAX_BOARDVI_NUM];         // 映射后的单体电压
    S16     Ti[MAX_TI_BOARD * MAX_BOARDTI_NUM];         // 映射后的单体温度
    U32     Ii[MAX_II_BOARD * MAX_BOARDII_NUM / 32];    // 映射后的开关量输入
    U32     Oi[MAX_OI_BOARD * MAX_BOARDOI_NUM / 32];    // 映射后的开关量输出
    U32     Xi[MAX_XI_BOARD * MAX_BOARDXI_NUM / 32];    // 映射后的其他项 单板32点扩展IO 15-1229

    S16     mMinVi[MAX_CHAN_NUM],
            mMaxVi[MAX_CHAN_NUM];                       // 单体电压极值（同主通道相关）  
    S16     mMinTi[MAX_CHAN_NUM], 
            mMaxTi[MAX_CHAN_NUM];                       // 单体电压极值（同主通道相关）  
public:

    SubChan();
    void    CanSendProc(CAN_msg *msg);                  // 载入下位机辅助通道数据
    U16     SetIOData(U8 *ptr, U8 hver);                // 拷贝开关量输入数据
};
//--------------------------------------------------
subchan_ext     SubChan     cSubChan;   // 对象 -> 辅助通道

//--------------------------------------------------
#endif  // #ifdef   __cplusplus
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#ifdef      __cplusplus
    extern "C" 
    {
#endif
//--------------------------------------------------
//subchan_ext 
const U32 B32IO_MASK[32] = 
{
    0x00000001, 0x00000003, 0x00000007, 0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff, 
    0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff,  
    0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff, 
    0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff
};


//--------------------------------------------------
#ifdef      __cplusplus
    }
#endif
//----------------------------------------------------------------------------------------------------
#endif
