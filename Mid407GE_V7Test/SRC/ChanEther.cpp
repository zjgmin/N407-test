//----------------------------------------------------------------------------------------------------
// Header:      通道与上位机接口处理程序
// File Name:   ChanEther.cpp
// Author:      David
// Date:        2012-11-12
//----------------------------------------------------------------------------------------------------
#define  _IN_CHANETHER_CPP
extern "C" 
{
    #include    "rtl.h"
    #include    "main.h"
    #include    "app_config.h"
    #include    "sys_config.h"
    #include    "spi.h"
//    #include    "led.h"
}
#include    "Dev.hpp"
#include    "parallelchan.hpp"  // 16-0122 以太网命令并联相关处理
#include    "chan.hpp"
#include    "chanrec.hpp"
#include    "chanether.hpp"
#include    "Subchan.hpp"
#include    "string.h"
#include    "app_can.hpp"
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//更新读记录指针暂存到真正的读记录指针
/*
    备注： 0x81命令调用
*/
//----------------------------------------------------------------------------------------------------
void Chan::CoverReadPoint(void)
{
//    if (PdHoldData->NrRData.RdPtrTemp < (PdHoldData->NrRData.MaxSavePack - 1)) 
    {
        PdHoldData->NrRData.ReadPoint = PdHoldData->NrRData.RdPtrTemp;
    }
}
//----------------------------------------------------------------------------------------------------
//恢复真正的读记录指针到读记录指针暂存
/*
    备注： 0x81命令调用
*/
//----------------------------------------------------------------------------------------------------
void Chan::RecoverReadPoint(void)
{
//  if (PdHoldData->NrRData.ReadPoint < (PdHoldData->NrRData.MaxSavePack - 1)) 
    {
        PdHoldData->NrRData.RdPtrTemp = PdHoldData->NrRData.ReadPoint;
    }
}
//----------------------------------------------------------------------------------------------------
//  获取通道的上传记录包长度
/*
    参数  f_len 数据固定部分长度
    返回  当前工况下（最近一次配置）记录包长度

    备注： 0x81命令调用
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::GetLoadPackLen(U16 f_len) const
{
    U16 len;

    len = f_len
        + (cDev.mInfo.ti_num*2)
        + (cDev.mInfo.vi_num*2)
        +((cDev.mInfo.xitem_num&0xff)*4);

    return  (len);
//  return  (PdHoldData->NrRData.LoadPackLen);
}
//----------------------------------------------------------------------------------------------------
//  获取未读出的记录个数
/*
    返回  len : 未读出记录个数

    备注： 0x81命令调用
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::GetUnReadRecNum(void) const
{
    U16 len = 0;

    len = (PdHoldData->NrRData.WritePoint >= PdHoldData->NrRData.ReadPoint)
        ? (PdHoldData->NrRData.WritePoint - PdHoldData->NrRData.ReadPoint)
        : (PdHoldData->NrRData.WritePoint + cDev.MaxSavePack - PdHoldData->NrRData.ReadPoint);

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  获取当前读出的记录类型
/*
    返回   type : 记录类型

    备注： 0x81命令调用
*/
//----------------------------------------------------------------------------------------------------
U8  Chan::GetReadRecType(void) const
{
    U8          type    = 0;
    strRecData  *rec_ptr;

    rec_ptr = (strRecData *)((U8 *)&PdHoldData->Record[0].RecMark + (cDev.SavePackLen * PdHoldData->NrRData.RdPtrTemp)); //15-1122    
    type    = ((rec_ptr->RecMark & 0x70) >> 4);

    return  (type);
}
//----------------------------------------------------------------------------------------------------
//  有故障时虚拟拷贝一条实时数据
/*  
    参数    *ptr :    代拷贝数据缓冲
            abn:      带入的故障代码
    返回    len :     拷贝数据长度

    备注：   拷贝实时数据（有SRAM故障）
            0x81命令调用
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::SetDummyRealDate(U8 *ptr, U8 abn)
{
    U16 len = 0;//FIXED_DATA_LEN;  //定长
    U32 i;

    //实时数据通道号及标记、状态
    *ptr++  = mAddr;
    *ptr++  = REALDATA_FLAG 
            |(NORMAL_REC << 4) 
            |(WORKSTAT_ABNOR);
    len += 2;
    //数据循环数
    for (i=0; i<4; i++) {
        *ptr++ = 0;
        len++;
    }
    len += 4;
    //工步号
    *ptr++ = 0;
    *ptr++ = 0;
    len += 2;
    //累计工步号
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //据工作模式
    *ptr++ = MODE_INIT;
    len += 1;
    //异常代码
    *ptr++ = abn;
    len += 1;
    //过程天数
    *ptr++ = 0;
    *ptr++ = 0;
    len += 2;
    //过程毫秒
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //总时间天数
    *ptr++ = 0;
    *ptr++ = 0;
    len += 2;
    //总时间毫秒
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //电压
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //电流
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //能量
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //容量
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //温度
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //单体温度数
    *ptr++ = 0;
    *ptr++ = 0;
    len += 2;
    //单体电压数
    *ptr++ = 0;
    *ptr++ = 0;
    len += 2;
    //其他项
    *ptr++ = 0;
    len += 1;

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  拷贝一条实时数据
/*
    参数  *ptr :  代拷贝数据缓冲
    返回  len :   拷贝数据长度

    备注： 拷贝实时数据
           0x81命令调用
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::SetRealDate(U8 *ptr)
{
    U16 len = 0;//FIXED_DATA_LEN;   // 变长，下面根据具体数据更新
    U16 xi  = 0, xinx;          // 16-0307
    U8  *p_buf;
    U8  static sRealTick = 0;   // 15-0923
    U32 cnt = 0,i,j,k;

    tsk_lock();

    *ptr++  = mAddr;                                // 通道号    1
    *ptr++  = REALDATA_FLAG                         // 实时标志  1
            |(NORMAL_REC<<4) 
            | Get_CurStatus();
    len += 2;
    p_buf = (U8 *)&PdHoldData->RunData.CycNum;      // 数据循环数 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    *ptr++ = PdHoldData->RunData.Step & 0xff;       // 工步号   2
    *ptr++ = PdHoldData->RunData.Step >> 8;
    len += 2;
    p_buf = (U8 *)&PdHoldData->RunData.TotStp;      // 累计工步号 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    *ptr++ = PdHoldData->RunData.Mode & 0xff;       // 据工作模式 1

    //异常代码根据运行状态处理 15-0129
    if (mAddr == 0) {   // 150923
        sRealTick++;
    }
    switch (PdHoldData->NrRData.CurWkStat)          // 异常代码  1
    {
    default:
    case WORKSTAT_INIT:     // 上电初始化状态
    case WORKSTAT_STOP:     // 停止状态
    case WORKSTAT_RUN:      // 运行状态
    case WORKSTAT_PAUSE:    // 暂停状态
        *ptr++ = PdHoldData->RunData.AbnCode & 0xff;    // 运行异常送上位机
        break;

    case WORKSTAT_XPAUSE:   // 异常暂停状态
    case WORKSTAT_ABNOR:    // 异常状态
        if (sRealTick & 0x01) {
            *ptr++ = PdHoldData->RunData.AbnCode & 0xff;    // 运行异常送上位机
        } else {
            *ptr++ = PdHoldData->NrRData.AbnCodeBak & 0xff; // 进入异常时的备份异常送上位机
        }
        break;
    }

    *ptr++ = PdHoldData->RunData.StpDay & 0xff;     // 过程天数  2
    *ptr++ = PdHoldData->RunData.StpDay >> 8;
    len += 4;
    p_buf = (U8 *)&PdHoldData->RunData.StpMs;       // 过程毫秒  4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    *ptr++ = PdHoldData->RunData.TotDay & 0xff;     // 总时间天数 2
    *ptr++ = PdHoldData->RunData.TotDay >> 8;
    len += 2;
    p_buf = (U8 *)&PdHoldData->RunData.TotMs;       // 总时间毫秒 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    p_buf = (U8 *)&PdHoldData->RunData.Vol;         // 电压 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    p_buf = (U8 *)&PdHoldData->RunData.Cur;         // 电流 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    p_buf = (U8 *)&PdHoldData->RunData.Engy;        // 能量 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    p_buf = (U8 *)&PdHoldData->RunData.Cap;         // 容量 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    p_buf = (U8 *)&PdHoldData->RunData.Temp;        // 温度 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;

    // 运行单体温度数 16-0307   // cSubChan.Ti[MAX_TI_BOARD * MAX_BOARDTI_NUM];  // 映射后的单体温度
    xi  = cDev.mInfo.ti_num;
//    if (xi > cDev.mSampCfg.ch[mAddr].TiNum) {
//        xi = cDev.mSampCfg.ch[mAddr].TiNum;
//    }
    *ptr++ = xi & 0xff;
    *ptr++ = xi >> 8;
    len   += xi * 2 + 2;    // 长度调整
    xinx   = xi * mAddr;    // 单体起始索引
    for (i=xinx; i<(xi+xinx); i++) {
        if (i < MAX_TI_BOARD * MAX_BOARDTI_NUM) {
            *ptr++ = cSubChan.Ti[i] & 0xff;
            *ptr++ = cSubChan.Ti[i] >> 8;            
        } else {    // 单体温度索引超出范围，发送 0xffff
            *ptr++ = 0xff;
            *ptr++ = 0xff;
        }
    }
    // 运行单体电压数 16-0307   // cSubChan.Vi[MAX_VI_BOARD * MAX_BOARDVI_NUM];  // 映射后的单体电压
    xi  = cDev.mInfo.vi_num;
    *ptr++ = xi & 0xff;
    *ptr++ = xi >> 8;
    len   += xi * 2 + 2;    // 长度调整
    xinx   = xi * mAddr;    // 单体起始索引
    for (i=xinx; i<(xi+xinx); i++) {
        if (i < MAX_VI_BOARD * MAX_BOARDVI_NUM) {
            *ptr++ = cSubChan.Vi[i] & 0xff;
            *ptr++ = cSubChan.Vi[i] >> 8;            
        } else {    // 单体电压索引超出范围，发送 0xffff
            *ptr++ = 0xff;
            *ptr++ = 0xff;
        }
    }
    // 运行其他项数 15-1112
    xi  = cDev.mInfo.xitem_num & 0xff;
    if (xi > MAX_TEST_XINUM) {
        xi = MAX_TEST_XINUM;
    }
    *ptr++ = xi;            // 单字节上传
    len   += xi * 4 + 1;    // 长度调整
    for (i=0; i<XITEMTBL_LEN; i++) {    // 遍历查询其他项
        if (cnt >= xi) {
            break;
        }
        j = i / 8;  // 其他项表字节索引
        k = i % 8;  // 其他项表字节位索引
        if (cDev.mInfo.xitem_tbl[j] & B8_MASK_N[k]) {
            cnt++;
            SetXitem (ptr, i);  // 拷贝数据
            ptr += 4;           // 4字节数据
        }
    }

    tsk_unlock();

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  拷贝其他项数据 15-1205，16-0514
/*  参数    ptr : 数据存储地址
            inx : 其他项索引
*/
//----------------------------------------------------------------------------------------------------
void    Chan::SetXitem(U8 *ptr, U32 inx)
{
    S32 s32buf;
    S32 *s32ptr;

    // 下位机扩展数据处理（根据设备结构处理） 16-0514
    switch (cDev.mInfo.dev_str_type)
    {
        default:
        case DS_GENERAL:        // 通用类型设备结构
        case DS_NEILSBED:       // 针床类型设备结构
        case DS_POURING:        // 倒出类型设备结构
        case DS_OPENSHELF:      // 开架类型设备结构 
        {
            switch (inx)
            {
                default:
                    s32buf = 0;
                    break;
                case XITEM_POW:         // 功率
                    s32ptr = (S32 *)&PdHoldData->NrRData.Pow; 
                    s32buf = *s32ptr;
                    break;
                case XITEM_RES:         // 电阻
                    s32ptr = (S32 *)&PdHoldData->NrRData.Res;
                    s32buf = *s32ptr;
                    break;
                case XITEM_CACC:        // 累计容量
                    s32ptr = (S32 *)&PdHoldData->NrRData.CapAcc;
                    s32buf = *s32ptr;
                    break;
                case XITEM_EACC:        // 累计能量
                    s32ptr = (S32 *)&PdHoldData->NrRData.EngyAcc;
                    s32buf = *s32ptr;
                    break;
                case XITEM_PILOT_VI:    // 领示电压
                    s32buf = 0;
                    break;
                case XITEM_MIN_VI:      // 最小单体电压
                    s32buf = PdHoldData->NrRData.MinVi.fdb;
                    break;
                case XITEM_MAX_VI:      // 最大单体电压
                    s32buf = PdHoldData->NrRData.MaxVi.fdb;
                    break;
                case XITEM_PILOT_TI:    // 领示温度
                    s32buf = 0;
                    break;
                case XITEM_MIN_TI:      // 最小单体温度
                    s32buf = PdHoldData->NrRData.MinTi.fdb;
                    break;
                case XITEM_MAX_TI:      // 最大单体温度
                    s32buf = PdHoldData->NrRData.MaxTi.fdb;
                    break;
                case XITEM_VBUS:        // 母线电压
                    s32buf = mCanExData.V2.Vbus;
                    break;
                case XITEM_VEX1:        // 扩展电压1(可用作领示电压)
                    s32buf = mCanExData.V2.ExtVol1;
                    break;
                case XITEM_VEX2:        // 扩展电压2(可用作电流线电压)
                    s32buf = mCanExData.V2.ExtVol2;
                    break;    
                case XITEM_VSLOPE:      // 电压斜率
                case XITEM_SWIN:        // 开关量输入
                case XITEM_SWOUT:       // 开关量输出
                    s32buf = 0;
                    break;
                case XITEM_YEAR:        // 实时时间-年
                case XITEM_MON:         // 实时时间-月
                case XITEM_DAY:         // 实时时间-日
                case XITEM_HOUR:        // 实时时间-时
                case XITEM_MIN:         // 实时时间-分
                case XITEM_SEC:         // 实时时间-秒
                case XITEM_RECCNT:      // 存储记录号
                case XITEM_LERR:        // 下位机故障位
                    s32buf = 0;
                    break;           
                case XITEM_TEX1:        // 扩展温度1(一般主通道采样)
                    s32buf = mCanExData.V2.Temp1;
                    break;
                case XITEM_TEX2:        // 扩展温度2(一般主通道采样)
                    s32buf = mCanExData.V2.Temp2;
                    break;
                case XITEM_I_INST:      // 电流瞬时值
                    s32buf = mCanExData.V2.InstCur;
                    break;
                case XITEM_V_INST:      // 电压瞬时值
                    s32buf = mCanExData.V2.InstVol;
                    break;
                case XITEM_CSTP:        // 过程（阶段）容量 16-0531
                    s32ptr = (S32 *)&PdHoldData->NrRData.CapStp; 
                    s32buf = *s32ptr;
                    break;
            }
            break;
        }                    
    }
    
    // 4字节拷贝
    *ptr++ = s32buf >> 0;
    *ptr++ = s32buf >> 8;
    *ptr++ = s32buf >> 16;
    *ptr++ = s32buf >> 24; 
}
//----------------------------------------------------------------------------------------------------
//  拷贝单条记录 15-1123 modified
/*
    返回    len : 拷贝数据长度

    备注：  从【通道号】字节开始拷贝
            0x81命令调用
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::SetOneRec(U8 *ptr)
{
    U16         len;            // 字节长度，下面根据具体数据更新
    U8          *p_buf;
    U32         i;
    U32         vrec_mark;      // 变量记录标记
    strRecData  *rec_ptr;       // 指向读记录指针指向地址
    strVarRec   *var_rec_ptr;   // 指向变量记录数据部分

    tsk_lock();

    if (PdHoldData->NrRData.RdPtrTemp == PdHoldData->NrRData.WritePoint) {  //记录空  13-0502
        PdHoldData->NrRData.FullFlag = FALSE;   // 清记录缓冲溢出标记
        tsk_unlock();
        return  (0);
    }

    rec_ptr = (strRecData *)((U8 *)&PdHoldData->Record[0].RecMark + (cDev.SavePackLen * PdHoldData->NrRData.RdPtrTemp)); //15-1122

    *ptr++ = mAddr;             // 通道号 0
    *ptr++ = rec_ptr->RecMark;  // 记录标志 1

    vrec_mark   = rec_ptr->RData.Mark + (rec_ptr->RData.Step<<16);
    // 非变量记录
    if (vrec_mark != 0xffffffff) 
    {
        p_buf = (U8 *)&(rec_ptr->RData.CycNum); // 数据循环数 2
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        *ptr++ = rec_ptr->RData.Step & 0xff;    // 工步号 6
        *ptr++ = rec_ptr->RData.Step >> 8;
        len += 2;
        
        p_buf = (U8 *)&rec_ptr->RData.TotStp;   // 累计工步号 8
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        *ptr++ = rec_ptr->RData.Mode    & 0xff; // 据工作模式 12
        *ptr++ = rec_ptr->RData.AbnCode & 0xff; // 异常代码 13
        *ptr++ = rec_ptr->RData.StpDay  & 0xff; // 过程天数 14
        *ptr++ = rec_ptr->RData.StpDay  >> 8;
        
        p_buf = (U8 *)&rec_ptr->RData.StpMs;    // 过程毫秒 16
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        *ptr++ = rec_ptr->RData.TotDay  & 0xff; // 总时间天数 20
        *ptr++ = rec_ptr->RData.TotDay  >> 8;
        
        p_buf = (U8 *)&rec_ptr->RData.TotMs;    // 总时间毫秒 22
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        p_buf = (U8 *)&rec_ptr->RData.Vol;      // 电压 26
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        p_buf = (U8 *)&rec_ptr->RData.Cur;      // 电流 30
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        p_buf = (U8 *)&rec_ptr->RData.Engy;     // 能量 34
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        p_buf = (U8 *)&rec_ptr->RData.Cap;      // 容量 38
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        //-----实时温度-----
        p_buf = (U8 *)&rec_ptr->RData.Temp;     // 温度 42
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        len = 46;   // 15-1123  固定部分一次性处理

        //-----单体温度-----
        if (cDev.mInfo.ti_num > MAX_TEST_TINUM) {
            cDev.mInfo.ti_num = MAX_TEST_TINUM;
        }
        *ptr++ = cDev.mInfo.ti_num & 0xff;  // 单体温度数 42 /or 42+4 下同
        *ptr++ = cDev.mInfo.ti_num >> 8;
        len   += cDev.mInfo.ti_num * TI_SIZE + 2; // 长度先调整
        p_buf  = (U8 *)rec_ptr + REC_LEN;   // 指到单体温度数据
        for (i=0; i<(cDev.mInfo.ti_num*TI_SIZE); i++) {
            *ptr++ = *p_buf++;
        }         
        //-----单体电压-----
        if (cDev.mInfo.vi_num > MAX_TEST_VINUM) {
            cDev.mInfo.vi_num = MAX_TEST_VINUM;
        }
        *ptr++ = cDev.mInfo.vi_num & 0xff;  // 单体电压数 44
        *ptr++ = cDev.mInfo.vi_num >> 8;        
        len   += cDev.mInfo.vi_num * VI_SIZE + 2; // 长度先调整
        p_buf  = (U8 *)rec_ptr + REC_LEN + cDev.mInfo.ti_num*TI_SIZE;   // 指到单体电压数据
        for (i=0; i<(cDev.mInfo.vi_num*VI_SIZE); i++) {
            *ptr++ = *p_buf++;
        } 
        //-----其他----- 15-1112
        if (cDev.mInfo.xitem_num > MAX_TEST_XINUM) {
            cDev.mInfo.xitem_num = MAX_TEST_XINUM;
        }
        *ptr++ = cDev.mInfo.xitem_num;      // 其他实时数据 46        
        len   += cDev.mInfo.xitem_num * XI_SIZE + 1; // 长度调整        
        p_buf  = (U8 *)rec_ptr + REC_LEN + cDev.mInfo.ti_num*TI_SIZE + cDev.mInfo.vi_num*VI_SIZE;   // 指到其他数据
        for (i=0; i<(cDev.mInfo.xitem_num*XI_SIZE); i++) {
            *ptr++ = *p_buf++;
        }                
    }
    //变量记录 FIRST_VAR_REC,LAST_VAR_REC
    else 
    {
        var_rec_ptr = (strVarRec *)&rec_ptr->RData.Mark;
        
        for (i=0; i<4; i++) {   // 变量记录标志，常数（0xffffffff）2
            *ptr++ = 0xff;
        }
//        len += 4;
        p_buf = (U8 *)&var_rec_ptr->Len;    // 长度 6 15-1123
        for (i=0; i<2; i++) {
            *ptr++ = p_buf[i];
        }  
        len = var_rec_ptr->Len;
//        len += 2;
        p_buf = (U8 *)&var_rec_ptr->V_Cyc1; // 循环1 6
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
//        len += 4;
        p_buf = (U8 *)&var_rec_ptr->V_Cyc2; // 循环2 10
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
//        len += 4;
        *ptr++ = var_rec_ptr->V_Day & 0xff; // 天数 14
        *ptr++ = var_rec_ptr->V_Day >> 8;
//        len += 2;
        p_buf = (U8 *)&var_rec_ptr->V_Ms;   // 毫秒数 16
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
//        len += 4;
        p_buf = (U8 *)&var_rec_ptr->V_Cap;  // 容量 20
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
//        len += 4;
        p_buf = (U8 *)&var_rec_ptr->V_Cyc3; // 循环3 24
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
//        len += 4;
        p_buf = (U8 *)&var_rec_ptr->V_Cyc4; // 循环4 28
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
//        len += 4;
        p_buf = (U8 *)&var_rec_ptr->V_Cyc5; // 循环5 32
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
//        len += 8;   //需要同非变量记录等长，补足长度(加入实时温度4字节) 36-
//                    //补长未考虑单体电压温度数据,数据未清零
    }
    //更新记录缓冲读指针
    //13-0502 更新暂存指针，等PC确认收到再次读记录是更新真正的读指针
    if (PdHoldData->NrRData.RdPtrTemp < (cDev.MaxSavePack - 1)) {       
        PdHoldData->NrRData.RdPtrTemp++;
    } else {    // 翻转
        PdHoldData->NrRData.RdPtrTemp = 0;
    }   

    tsk_unlock();

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  拷贝过程记录 （过程记录长度固定）
/*
    返回    len :  拷贝数据长度

    备注：  0x81命令调用
            从过程数据通道号开始拷贝 2013-0413
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::SetProcRec(U8 *ptr)
{
    U16         len = PROCDATA_LEN;
    U8          *p_buf;
    U32         i;
    strRecData  *rec_ptr;       //指向读记录指针指向地址
    strProcRec  *proc_rec_ptr;  //指向过程记录数据部分

    tsk_lock();

    if (PdHoldData->NrRData.RdPtrTemp == PdHoldData->NrRData.WritePoint) {  //记录空 13-0502
        PdHoldData->NrRData.FullFlag = FALSE;   //清记录缓冲溢出标记
        tsk_unlock();
        return  (0);
    }

    rec_ptr = (strRecData *)((U8 *)&PdHoldData->Record[0].RecMark + (cDev.SavePackLen * PdHoldData->NrRData.RdPtrTemp));    //15-1122   
    proc_rec_ptr = (strProcRec *)&rec_ptr->RData.Mark;

    *ptr++  = mAddr;                            //通道号 0
    *ptr++  = rec_ptr->RecMark;                 //记录标志 1
    p_buf   = (U8 *)&(proc_rec_ptr->P_CycNum);  //数据循环数 2
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    *ptr++  = proc_rec_ptr->P_Stp & 0xff;       //工步号 6
    *ptr++  = proc_rec_ptr->P_Stp >> 8;
    p_buf   = (U8 *)&proc_rec_ptr->P_TotStp;    //累计工步号 8
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    *ptr++  = proc_rec_ptr->P_Mode   & 0xff;    //据工作模式 12
    *ptr++  = proc_rec_ptr->P_AbnCode& 0xff;    //异常代码 13
    *ptr++  = proc_rec_ptr->P_StpDay & 0xff;    //过程天数 14
    *ptr++  = proc_rec_ptr->P_StpDay >> 8;
    p_buf   = (U8 *)&proc_rec_ptr->P_StpMs;     //过程毫秒 16
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    *ptr++  = proc_rec_ptr->P_TotDay & 0xff;    //总时间天数 20
    *ptr++  = proc_rec_ptr->P_TotDay >> 8;
    p_buf   = (U8 *)&proc_rec_ptr->P_TotMs;     //总时间毫秒 22
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    p_buf   = (U8 *)&proc_rec_ptr->P_Engy;      //能量 26
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    p_buf   = (U8 *)&proc_rec_ptr->P_Cap;       //容量 30
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    p_buf   = (U8 *)&proc_rec_ptr->P_Res;       //动态内阻 34
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }

    //更新记录缓冲读指针
    //13-0502 更新暂存指针，等PC确认收到再次读记录是更新真正的读指针
    if (PdHoldData->NrRData.RdPtrTemp < (cDev.MaxSavePack - 1)) {
        PdHoldData->NrRData.RdPtrTemp++;
    } else {    //翻转
        PdHoldData->NrRData.RdPtrTemp = 0;
    }

    tsk_unlock();

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  拷贝限定最大长度的若干条记录
/*
    参数  max_len :   限定最大长度
    返回  ret_len :   拷贝数据长度

    备注： 0x81命令调用
*/
//----------------------------------------------------------------------------------------------------
//U16 Chan::SetRecData(U16 max_len)
//{
//    U16   ret_len = 0;





//    return  (ret_len);
//}

//----------------------------------------------------------------------------------------------------
//  PC干预 通道控制 0x82
/*  参数 
    *ptr    控制参数缓冲指针
    返回    0 - 成功  
            1 - 失败 
            0xf1 - 下发清回路故障 16-0128
*/
//----------------------------------------------------------------------------------------------------
U8  Chan::ManualCtrl(U8 *ptr)
{
    U8  opt_code    = ptr[0];           // 操作代码
    U8  ret         = 0;

    if ((opt_code ==0) 
    ||  (opt_code > MAX_OPT_CODE)) {    // 错误的操作字
        return (1);
    }

    tsk_lock();

    switch (opt_code)
    {
    //---------- PC 停止 ----------
    case OPT_STOP:
        // 清故障
        PdHoldData->RunData.AbnCode     = 0;
        PdHoldData->NrRData.LowerStat   = 0;
        PdHoldData->NrRData.CabCtrlFlag = 0;
        switch (PdHoldData->NrRData.CurWkStat)
        {
        case WORKSTAT_RUN:      // 运行状态 －> 停止
            PdHoldData->NrRData.NxtWkStat   = WORKSTAT_STOP;
            break;
            
        case WORKSTAT_PAUSE:    // 暂停状态 －> 停止
            PdHoldData->NrRData.NxtWkStat   = WORKSTAT_STOP;
            break;
            
        case WORKSTAT_XPAUSE:   // 异常暂停状态 －> 停止
        case WORKSTAT_ABNOR:    // 异常状态 －> 停止
            ret = 0xf1;     // 清下位机回路故障 16-0128
        
            // 不等待来自下位机的应答
            ;
            PdHoldData->NrRData.NxtWkStat = WORKSTAT_STOP;
            break;
        
        case WORKSTAT_STOP:     // 停止状态
        case WORKSTAT_INIT:     // 上电初始化不接受PC控制
        default:
            PdHoldData->NrRData.NxtWkStat = WORKSTAT_STOP;  // 其他状态也停止
            break;
        }
        break;
        
    //---------- PC 暂停 ----------
    case OPT_PAUSE:
        switch (PdHoldData->NrRData.CurWkStat)
        {
        case WORKSTAT_RUN:      // 运行状态 －> 暂停
            PdHoldData->NrRData.NxtWkStat   = WORKSTAT_PAUSE;
            break;
            
        case WORKSTAT_XPAUSE:   // 异常暂停状态

            break;
            
        case WORKSTAT_ABNOR:    // 异常状态 －> 暂停

            break;
        
        case WORKSTAT_INIT:     // 上电初始化不接受PC控制
        case WORKSTAT_STOP:     // 停止状态
        case WORKSTAT_PAUSE:    // 暂停状态
            
        default:
            break;
        }
        break;

    //---------- PC 恢复 ----------
    case OPT_RESUME:
        switch (PdHoldData->NrRData.CurWkStat)
        {
        case WORKSTAT_PAUSE:    // 暂停状态 - > 恢复
            PdHoldData->NrRData.NxtWkStat = WORKSTAT_RUN;
            break;
        
        case WORKSTAT_XPAUSE:   // 异常暂停状态 - > 恢复
            switch (PdHoldData->RunData.AbnCode)    // 根据故障决定是否恢复
            {
            default:
                PdHoldData->RunData.AbnCode     = ABNOR_NULL;   // 清故障
                PdHoldData->NrRData.NxtWkStat   = WORKSTAT_RUN; // 转运行
                mLErrCtrl.DlyCnt = CTRL_1S; //CTRL_3S; // 故障恢复时，下位机故障报警延时 15-0618
                break;
            
            case    ABNOR_OV:           // 过压
            case    ABNOR_UV:           // 欠压
            case    ABNOR_OC:           // 过流
            case    ABNOR_UC:           // 欠流
            case    ABNOR_OT:           // 过热
            case    ABNOR_UT:           // 低温
            case    ABNOR_COM_Vi:       // 单体电压通信异常
            case    ABNOR_COM_Ti:       // 温度通信异常
            case    ABNOR_POWEROFF:     // 掉电
            case    ABNOR_DATAOF:       // 数据溢出
            case    ABNOR_OP:           // 超功率
            case    ABNOR_NUM_Vi:       // 单体电压个数未设置
            case    ABNOR_TotalV:       // 单体电压和异常
            case    ABNOR_EEPROM:       // EEPROM数据异常
            case    ABNOR_OVi:          // 单体过压
            case    ABNOR_UVi:          // 单体欠压
            case    ABNOR_OCAP:         // 超容量
            case    ABNOR_STOPDATA:     // 停止后数据未传完
            case    ABNOR_IN_CALIB:     // 校准数据中
            case    ABNOR_LC:           // 漏电流
            case    ABNOR_POLAR:        // 反极
            case    ABNOR_OUV:          // 过欠压
            case    ABNOR_CV:           // 不恒压
            case    ABNOR_CC:           // 不恒流
            case    ABNOR_CTRLDAT:      // 控制数据错误
                
            case    ABNOR_BUSOV:        // 母线过压
            case    ABNOR_BUSUV:        // 母线欠压
            case    ABNOR_SENOT:        // 传感器高温
            case    ABNOR_SENUT:        // 传感器低温
            case    ABNOR_CCCVS:        // 恒流充电电压负斜率
            case    ABNOR_DCCVS:        // 恒流放电电压正斜率
            case    ABNOR_SYSBUSY:      // 系统忙
            case    ABNOR_DRVOC:        // 驱动级过流
            case    ABNOR_BUSFB:        // 母线倒灌
            case    ABNOR_BUS:          // 母线校准参数错
            case    ABNOR_CAP:          // 电容校准参数错
            case    ABNOR_BUSFREQ:      // 母线频繁故障
            case    ABNOR_DFZ:          // 电流线反极
            case    ABNOR_VBK:          // 采样线掉线
        // 下位机未定义扩展故障

            case    ABNOR_BIT30:
            case    ABNOR_BIT31:
        //--------------------
            
            case    ERR_GETWKLINE:      // 获取工艺行号错误
            case    ERR_PARVAR:         // 非法变量或参数
            case    ERR_OPT:            // 非法操作符
            case    ERR_NOLIMIT:        // 限制条件未设定
            case    ERR_STPPRG:         // 阶段工艺检查错误
            case    ERR_GBLPRG:         // 全局工艺数据错误
            case    ERR_ELSENUM:        // 否则超数
            case    ERR_MODE:           // 未知工作模式
            case    ERR_CCMAX:          // 超最大充电电流
            case    ERR_DCMAX:          // 超最大放电电流
            case    ERR_CUMAX:          // 超最大充电电压
            case    ERR_DUMIN:          // 超最小放电电压
            case    ERR_CPMAX:          // 超最大充电功率
            case    ERR_DPMAX:          // 超最大放电功率
            case    ERR_STPINFINITE:    // 阶段跳转死循环
            case    ERR_MCANOFFLINE:    // 主控制CAN脱机
            case    ERR_SCANOFFLINE:    // 辅助CAN脱机
            case    EER_LOWER_NORMAL:   // 下位机普通故障（下位机不暂停）
            case    EER_LOWER_FATAL:    // 下位机重要故障（下位机暂停或不启动）
            case    EER_SYNC:           // 阶段启动时与下位机同步失败
            case    ERR_WKSTAT:         // 未知工作态错误
            case    EER_SPACEOVF:       // 空间溢出
            case    ERR_SRAM:           // SRAM硬件故障
                PdHoldData->RunData.AbnCode     = ABNOR_NULL;       // 清故障
                PdHoldData->NrRData.NxtWkStat   = WORKSTAT_RUN;     // 转运行
                mLErrCtrl.DlyCnt = CTRL_1S; // CTRL_3S;//故障恢复时，下位机故障报警延时 15-0618
                break;
            }
            break;

        case WORKSTAT_INIT:     // 上电初始化不接受PC控制
        case WORKSTAT_STOP:     // 停止状态
        case WORKSTAT_RUN:      // 运行状态
        case WORKSTAT_ABNOR:    // 异常状态 不让恢复 //15-0324 异常不让恢复
        default:
            break;
        }
        break;

    //---------- PC 跳阶段 ----------
    case OPT_JUMP:
        break;

    //---------- PC 单步执行 ----------
    case OPT_SINGLESTEP:
        switch (PdHoldData->NrRData.CurWkStat)
        {
        case WORKSTAT_RUN:      // 运行状态 
            break;            
        case WORKSTAT_PAUSE:    // 暂停状态
            break;            
        case WORKSTAT_XPAUSE:   // 异常暂停状态
            break;            
        case WORKSTAT_ABNOR:    // 异常状态
            break;
        case WORKSTAT_INIT:     // 上电初始化不接受PC控制
        case WORKSTAT_STOP:     // 停止状态
        default:
            break;
        }
        break;
        
    //---------- 闪指示灯 ----------
    case OPT_LED_FLASH:
        PdHoldData->NrRData.CabCtrlFlag = 1;
        break;

    //---------- 关指示灯 ----------
    case OPT_LED_OFF:
        PdHoldData->NrRData.CabCtrlFlag = 0;
        break;

    default:
        break;
    }

    tsk_unlock();

    return (ret);
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//  PC干预 读通道额定 0x90
/*  参数 
    *ptr    读额定参数缓冲指针 (首字节为校准类型)
    返回    数据长度
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::RdRate(U8 *ptr)   // 读通道额定
{
    U8  len     = 0;
    U8  type    = ptr[0];

    type = type;

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  PC干预 写通道额定 0x91
/*  参数 
    *ptr    校准参数缓冲指针 (首字节为校准类型)
    返回    0 - 成功  1 - 失败
*/
//----------------------------------------------------------------------------------------------------
U8 Chan::WrRate(U8 *ptr)    // 写通道额定
{
    U8  ret     = 0;
    U8  type    = ptr[0];

    type = type;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  PC干预 读通道校准 0x92
/*  参数 
    *ptr    校准参数缓冲指针 (首字节为校准类型)
    返回  数据长度
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::RdCalib(U8 *ptr)  // 读校准参数
{
    U8  len     = 0;
    U8  type    = ptr[0];

    type = type;

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  PC干预 通道校准 0x93
/*  参数 
    *ptr    校准参数缓冲指针 (首字节为校准类型)
    返回    0 - 成功    1 - 失败
*/
//----------------------------------------------------------------------------------------------------
U8  Chan::WrCalib(U8 *ptr)
{
    U8  ret     = 0;
    U8  type    = ptr[0];

    type = type;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//eof
