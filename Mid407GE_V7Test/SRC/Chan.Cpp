//----------------------------------------------------------------------------------------------------
// Header:      通道运行处理程序
// File Name:   Chan.cpp
// Author:      David
// Date:        2012-10-14
//----------------------------------------------------------------------------------------------------
#define  _IN_CHAN_CPP

extern "C" 
{
    #include    <string.h>
    #include    "rtl.h"
    #include    "main.h"
    #include    "app_config.h"
}
#include    "chan.hpp"
#include    "subchan.hpp"
#include    "Dev.hpp"
//--------------
//----------------------------------------------------------------------------------------------------
//构造函数
//----------------------------------------------------------------------------------------------------
Chan::Chan()
{
    static char s_chan_inx = 0;
    U32         pdAddr;     // 通道掉电区地址
    U32         residual;   // 模4余数

    //上传记录备份 0x60000000
    if (s_chan_inx == 0) {
        pRecBackUp = (strRecBackUp *) REC_BACK_OFFSET;  // 上传记录数据帧备份，重传用
        if (pRecBackUp->Mark != PDHDATA_MARK) {         // 初始化接收备份
            pRecBackUp->Mark    = PDHDATA_MARK;
            pRecBackUp->RecCnt  = 0;
        }
//        // 线性拟合库测试 16-0715
//        LinearFix tst(Y_Bufer,5);
//        float kkk,rrr;
//        
//        for (int i=0; i<20; i++) {
//            tst.SetY(TEST_YBUF[i]);
//            
//            if (tst.GetBufStat()) {
//                tst.Norm_Line_Fit();
//                kkk = tst.GetK();
//                rrr = tst.GetR();
//                kkk = kkk;
//                rrr = rrr;
//            }
//        }        
                
    }
    //掉电数据指针
    pdAddr      = s_chan_inx * CH_PDH_CAP + (LSB_ADDR + SYS_USED_CAP);
    residual    = pdAddr % 4;
    pdAddr     -= residual;             // 字节对齐
    PdHoldData  = (strPdMem *)pdAddr;   // 掉电指针赋值

    mAddr       = s_chan_inx;
    s_chan_inx++;

    mSubOffFlag = 0;            // 清辅助脱机报警要求 16-0311
    mRecFlag    = 0x0000;
    mCntDnTick  = 0xffffffff;   // 状态恢复倒计时  

    //清下位机CAN数据
    ClrCanCtrlData((U8 *)&mCanCtrlData);
    ClrCanExData();
    //判断掉电区数据
    if (Test_PdMark() == 0) {       // 标记有误，初始化掉电区
        Init_PdMark();
        Init_PdData();
        Init_WkData();
        Clear_Program();
    } 
    else if (Test_PdData() == 0) {  // 掉电数据有误，初始化掉电数据
        Init_PdData();
        Init_WkData();
        Clear_Program();
    }
    else {                          // 掉电数据无误，判断掉电状态并处理
        // 备份掉电前的故障状态
        if (PdHoldData->RunData.AbnCode != ABNOR_POWEROFF) {
            PdHoldData->NrRData.PdAbnBak = PdHoldData->RunData.AbnCode;
        }
        switch (PdHoldData->NrRData.CurWkStat)
        {
            default:
            {
                PdHoldData->NrRData.PreWkStat   = WORKSTAT_STOP;
                PdHoldData->NrRData.NxtWkStat   = 
                PdHoldData->NrRData.CurWkStat   = WORKSTAT_INIT;
                break;
            }
            case WORKSTAT_INIT:     // 掉电恢复前又掉电
            {
                if (PdHoldData->NrRData.PreWkStat == WORKSTAT_RUN) {
                    PdHoldData->RunData.AbnCode     = ABNOR_POWEROFF;   // 置掉电故障，若掉电前有故障尚处于运行将被覆盖？;
                    PdHoldData->NrRData.NxtWkStat   = WORKSTAT_INIT;
                } else {
                    Init_WkData();
                }
                break;
            }
            case WORKSTAT_RUN:      // 运行时掉电
            {
                if ((PdHoldData->NrRData.CurWkLine != LINE_FOR_STOP)
                &&  (PdHoldData->RunStpPrg.Mode != MODE_END)) {
                    PdHoldData->RunData.AbnCode     = ABNOR_POWEROFF;               // 置掉电故障，若掉电前有故障尚处于运行将被覆盖？
                    PdHoldData->NrRData.PreWkStat   = PdHoldData->NrRData.CurWkStat;// 备份掉电状态，准备恢复
                    PdHoldData->NrRData.NxtWkStat   =
                    PdHoldData->NrRData.CurWkStat   = WORKSTAT_INIT;
                } else {
                    PdHoldData->NrRData.PreWkStat   = WORKSTAT_STOP;
                    PdHoldData->NrRData.NxtWkStat   = 
                    PdHoldData->NrRData.CurWkStat   = WORKSTAT_INIT;
                }
                // 取出掉电阶段 16-0520
                mCanCtrlData.Step = PdHoldData->RunData.Step;
                break;
            }
            case WORKSTAT_PAUSE:    // 暂停状态     （运行时人工暂停）
            case WORKSTAT_XPAUSE:   // 异常暂停状态 （运行时有异常）
            {
                // 取出掉电阶段 16-0520
                mCanCtrlData.Step = PdHoldData->RunData.Step;
                break;
            }
            case WORKSTAT_ABNOR:    // 异常状态     （非运行时有异常）
            {
//  上述状态掉电直接恢复，不进入掉电状态  15-0805
//              PdHoldData->RunData.AbnCode     = ABNOR_POWEROFF;               // 置掉电故障
//              PdHoldData->NrRData.PreWkStat   = PdHoldData->NrRData.CurWkStat;// 备份掉电状态，准备恢复
//              PdHoldData->NrRData.NxtWkStat   =
//              PdHoldData->NrRData.CurWkStat   = WORKSTAT_INIT;
                break;
            }
        }
    }

    Init_PdCntDn(); //恢复倒计时
    
    os_sem_init (mStpSyncSem, 0);       // 下位机阶段跳阶段后同步信号
//    os_sem_init (mDisProtSem, 0);       // 下位机屏蔽保护信号
    
//#if (DEV_STR_TYPE == DS_NEILSBED)   // 针床设备相关信号量处理  15-0603
    
    // 不管什么设备结构，都初始化针床信号量 16-0514
    os_sem_init (mSmokeSem, 0);         // 烟感报警信号量
    os_sem_init (mRqInplaceSem, 0);     // 请求针床托盘到位状态 信号量
    os_sem_init (mInplaceSem, 0);       // 托盘到位信号量
    
//#endif
}
//----------------------------------------------------------------------------------------------------
//  通道测试（断点）
//----------------------------------------------------------------------------------------------------
inline  U8 Chan::ChanBreak(void)
{
    return  (mAddr);
}
//----------------------------------------------------------------------------------------------------
//  掉电区标记判断
/*
    返回值 0 - 掉电区标记错误或未标记
            1 - 掉电区有合法标记
*/
//----------------------------------------------------------------------------------------------------
U8  Chan::Test_PdMark()
{
    if (PdHoldData->Mark != PDHDATA_MARK) {
        return  (0);
    } else {
        return  (1);
    }
}
//----------------------------------------------------------------------------------------------------
//  掉电区标记初始化
//----------------------------------------------------------------------------------------------------
void Chan::Init_PdMark()
{
    PdHoldData->Mark = PDHDATA_MARK;
}
//----------------------------------------------------------------------------------------------------
//  掉电区数据判断
/*
    返回    0 - 掉电区数据错误或未标记
            1 - 掉电区有合法数据
*/
//----------------------------------------------------------------------------------------------------
U8  Chan::Test_PdData()
{
    U8  ret = 1;  
    
    if ((PdHoldData->RunData.Mark&0xff00) != RUNDATA_HMARK) {   // 高字节标记错
        ret = 0;
    }
    if (((PdHoldData->RunData.Mode&0xff) >= MIN_ERR_MODE)
    &&  ((PdHoldData->RunData.Mode&0xff) <= MAX_ERR_MODE)) {    // 工作模式错误
        ret = 0;
    }    
    if (PdHoldData->RunData.TotStp > 10000000) {    // 总累计工步 限制为1000万步
        ret = 0;
    }
    if (PdHoldData->RunData.CycNum > 10000000) {    // 充放循环数 限制为1000万步
        ret = 0;
    }   
    if (PdHoldData->RunData.StpDay > 7300) {        // 过程时间天数 限制20年
        ret = 0;
    }
    if (PdHoldData->RunData.TotDay > 7300) {        // 总时间天数 限制20年
        ret = 0;
    }    
    if (PdHoldData->RunData.StpMs > (25*3600000)) { // 过程时间毫秒 限制25小时
        ret = 0;
    }    
    if (PdHoldData->RunData.TotMs > (25*3600000)) { // 总时间毫秒 限制25小时    
        ret = 0;
    }       

    // 有故障
    if (ret == 0) {
        PdHoldData->RunData.AbnCode = ERR_PDDATA;
    }
    
    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  掉电区数据初始化 15-1215 modified
//----------------------------------------------------------------------------------------------------
void Chan::Init_PdData()
{
    PdHoldData->NrRData.FullFlag    = FALSE;            // 记录缓冲溢出标记
    PdHoldData->NrRData.ReadPoint   = 0;                // 读记录缓冲指针
    PdHoldData->NrRData.WritePoint  = 0;                // 写记录缓冲指针
    PdHoldData->NrRData.RdPtrTemp   = 0;                // 读指针暂存
    PdHoldData->NrRData.RecNum      = 0;                // 记录数 -> 暂作测试用
    PdHoldData->NrRData.bChDchMark  = HAVE_NULL;        // 充放标记
    PdHoldData->NrRData.PdAbnBak    = 0;                // 掉电时的异常备份，退出掉电延时后清零
    PdHoldData->NrRData.LowerStat   = 0;                // 来自下位机故障状态
    PdHoldData->NrRData.AbnCodeBak  = 0;                // 实时数据的故障备份保存
    PdHoldData->NrRData.VT1_mS      = 0;                // 时间变量1毫秒计数
    PdHoldData->NrRData.V_Time1     = 0;                // 时间变量1(100mS分辨率)
    PdHoldData->NrRData.V_Cap1      = 0;                // 容量变量1
    PdHoldData->NrRData.V_Cyc[0]    = 0;                // 循环变量1
    PdHoldData->NrRData.V_Cyc[1]    = 0;                // 循环变量2
    PdHoldData->NrRData.V_Cyc[2]    = 0;                // 循环变量3
    PdHoldData->NrRData.V_Cyc[3]    = 0;                // 循环变量4
    PdHoldData->NrRData.V_Cyc[4]    = 0;                // 循环变量5
    PdHoldData->NrRData.Pow         = 0;                // 功率
    PdHoldData->NrRData.Res         = 0;                // 电阻
    PdHoldData->NrRData.CapStp      = 0;                // 阶段容量 16-0531
    PdHoldData->NrRData.CapAcc      = 0;                // 累计容量
    PdHoldData->NrRData.EngyAcc     = 0;                // 累计能量
    PdHoldData->NrRData.Slope       = 0;                // 电压斜率
    PdHoldData->NrRData.MinusSlope  = 0;                // 电压负斜率
    PdHoldData->NrRData.MaxVolt     = 0;                // 最大过程电压 16-0716
    PdHoldData->NrRData.MinVolt     = 0;                // 最小过程电压 16-0716
    PdHoldData->NrRData.MaxIi       = 0;                // 最大电流
    PdHoldData->NrRData.MinIi       = 0;                // 最小电流
    PdHoldData->NrRData.MaxVi.ref   = 0;                // 最大单体电压 设定
    PdHoldData->NrRData.MinVi.ref   = 0;                // 最小单体电压 设定
    PdHoldData->NrRData.MaxTi.ref   = 0;                // 最大单体温度 设定
    PdHoldData->NrRData.MinTi.ref   = 0;                // 最小单体温度 设定

    PdHoldData->RunData.Mark        = RUNDATA_HMARK;    // 注入数据标志
    PdHoldData->RunData.Step        = 1;                // 0;
    PdHoldData->RunData.Mode        = MODE_INIT;
    if (PdHoldData->RunData.AbnCode!= ERR_PDDATA) {     // 没有掉电数据故障
        PdHoldData->RunData.AbnCode = ABNOR_NULL;
    }
    PdHoldData->RunData.TotStp      = 0;
    PdHoldData->RunData.CycNum      = 0;
    PdHoldData->RunData.StpDay      = 0;
    PdHoldData->RunData.TotDay      = 0;
    PdHoldData->RunData.StpMs       = 0;
    PdHoldData->RunData.TotMs       = 0;    
    PdHoldData->RunData.Vol         = 0;
    PdHoldData->RunData.Cur         = 0;
    PdHoldData->RunData.Engy        = 0;
    PdHoldData->RunData.Cap         = 0;
    PdHoldData->RunData.Temp        = DEF_TMP_FLOAT;
    
}
//----------------------------------------------------------------------------------------------------
//  运行状态控制判断
/*
    返回值 0 - 不符合运行状况
            1 - 符合运行状况(运行，暂停，异常，故障暂停,上电，停止)
*/
//----------------------------------------------------------------------------------------------------
U8  Chan::Test_WkStat(U8 mark)
{
    U16 stat;
    
    switch (mark) 
    {
        case CUR_MARK:  
        default:        // 默认当前
        {              
            stat = PdHoldData->NrRData.CurWkStat;
            break;
        }
        case NXT_MARK:  // 下一个
        {
            stat = PdHoldData->NrRData.NxtWkStat;
            break;
        }
        case PRE_MARK:  // 先前
        {
            stat = PdHoldData->NrRData.PreWkStat;
            break;
        }
    }
    if (stat > MIN_ERR_WKSTAT)  return  (0);
    
    return  (1);
}
//----------------------------------------------------------------------------------------------------
//  清零当前运行工艺缓冲 (包括重置时暂存的运行中工艺)
//----------------------------------------------------------------------------------------------------
void Chan::Clear_Program(void)
{
    U16 *point;
    
    point = (U16 *)&(PdHoldData->Program.global.Init_phase);
    while (point <= (U16 *)&PdHoldData->RunStpPrg.Stp_act) {
        *point++ = 0;
    }
    PdHoldData->Program.TotLine = 1;    //14-1018
}
//----------------------------------------------------------------------------------------------------
//  运行控制常规上电初始化
//----------------------------------------------------------------------------------------------------
void Chan::Init_WkData()
{
    PdHoldData->NrRData.PreWkStat   = WORKSTAT_STOP;
    PdHoldData->NrRData.NxtWkStat   =
    PdHoldData->NrRData.CurWkStat   = WORKSTAT_INIT;
    PdHoldData->NrRData.ElseNum     = 0;                // '否则'判读计数
    PdHoldData->NrRData.PreWkLine   = 0;                // 通道之前运行工艺行
    PdHoldData->NrRData.CurWkLine   = 0;                // 通道当前运行工艺行
    PdHoldData->NrRData.NxtWkLine   = 0;                // 通道下步运行工艺行
    PdHoldData->NrRData.JumpFlag    = 0;                // 清重置工艺行标记

    PdHoldData->RunData.Mode        = MODE_INIT;
//    PdHoldData->RunData.AbnCode     = ABNOR_NULL;       // 14-1018
    if (PdHoldData->NrRData.WritePoint == PdHoldData->NrRData.ReadPoint) {  // 记录空
        PdHoldData->NrRData.FullFlag    = FALSE;                            // 清记录缓冲溢出标记
    }
//    Clear_Program();    //清零工艺缓冲
}
//----------------------------------------------------------------------------------------------------
//  运行掉电，上电控制状态设定
//----------------------------------------------------------------------------------------------------
void Chan::Set_PdWkState()
{
    // 掉电保护的当前工作状态
    switch (PdHoldData->NrRData.CurWkStat) 
    {
        default:
        {
            Init_WkData();      //  其他，can omit
            break;
        }
        case WORKSTAT_INIT:     // 在掉电态又一次掉电什么都不做，等待恢复运行
        {
            break;
        }    
        case WORKSTAT_RUN:
        {       
            PdHoldData->RunData.AbnCode     = ABNOR_POWEROFF;   // 置掉电故障，若掉电前有故障尚处于运行将被覆盖？
            PdHoldData->NrRData.PreWkStat   = PdHoldData->NrRData.CurWkStat;
            PdHoldData->NrRData.NxtWkStat   =
            PdHoldData->NrRData.CurWkStat   = WORKSTAT_INIT;
            break;
        }
        case WORKSTAT_PAUSE:
        {
            PdHoldData->NrRData.PreWkStat   = PdHoldData->NrRData.CurWkStat;
            PdHoldData->NrRData.NxtWkStat   =
            PdHoldData->NrRData.CurWkStat   = WORKSTAT_INIT;
            break;
        }
        case WORKSTAT_XPAUSE:
        {
            PdHoldData->NrRData.PreWkStat   = PdHoldData->NrRData.CurWkStat;
            PdHoldData->NrRData.NxtWkStat   =
            PdHoldData->NrRData.CurWkStat   = WORKSTAT_INIT;
            break;
        }    
        case WORKSTAT_ABNOR:
        {
            PdHoldData->NrRData.PreWkStat   = PdHoldData->NrRData.CurWkStat;
            PdHoldData->NrRData.NxtWkStat   =
            PdHoldData->NrRData.CurWkStat   = WORKSTAT_INIT;
            break;
        }
        case WORKSTAT_STOP:
        {
            PdHoldData->NrRData.PreWkStat   = PdHoldData->NrRData.CurWkStat;
            PdHoldData->NrRData.NxtWkStat   =
            PdHoldData->NrRData.CurWkStat   = WORKSTAT_INIT;
            break;
        }
    }
    if (PdHoldData->NrRData.WritePoint == PdHoldData->NrRData.ReadPoint) {  // 记录空
        PdHoldData->NrRData.FullFlag    = FALSE;                            // 清记录缓冲溢出标记
    }
}
//----------------------------------------------------------------------------------------------------
//  掉电自动恢复模式判断
/*
    判断中位机配置，如果允许，再判断全局工艺
    返回     0 - 手动恢复
             1 - 自动恢复
*/
//----------------------------------------------------------------------------------------------------
U8  Chan::Test_PdResumeMode()   // 14-1018 modified 
{
    U8  ret = 0;
    
    if (cDev.mCtrlCfg.bPdResumeMode) {                      // 中位机允许自动恢复
        if (PdHoldData->Program.global.Pdwk_mode == 0) {    // Pdwk_mode=0 ,全局工艺允许自动恢复
            ret = 1;
        }
    } 
    
    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  掉电态倒计时初始化(按地址逐个恢复)
//----------------------------------------------------------------------------------------------------
void Chan::Init_PdCntDn()
{
    mCntDnTick = (mAddr * CTRL_3S / MAX_CHAN_NUM) + (PD_RESUME_T / CTRL_PERIOD);    // 5秒全员掉电恢复周期
}
//----------------------------------------------------------------------------------------------------
//  设置温度数 15-1215
//----------------------------------------------------------------------------------------------------
void Chan::SetTiNum(U8 num)
{
    ;
}
//----------------------------------------------------------------------------------------------------
//  保存工艺    从工艺接收控制缓冲中取出工艺
//----------------------------------------------------------------------------------------------------
U8  Chan::Save_Program(void)
{
    U8  i, ret = 0;
    
    tsk_lock();
    PdHoldData->Program.global  = PrgRcvCtrl.DataBuf.global;
    PdHoldData->Program.TotLine = PrgRcvCtrl.DataBuf.pageline.TotLine;
    for (i=0; i<PdHoldData->Program.TotLine; i++) {
        PdHoldData->Program.step[i] = PrgRcvCtrl.DataBuf.step[i];
    }
    tsk_unlock();
    
    for (; i<MAX_WKLINE; i++) { // 后面的工步数据清零
        PdHoldData->Program.step[i].No              = 0;    // 工步序号
        PdHoldData->Program.step[i].Mode            = 0;    // 工作模式
        PdHoldData->Program.step[i].Mpar1           = 0;    // 主参数1
        PdHoldData->Program.step[i].Mpar2.Float32   = 0;    // 主参数2
        PdHoldData->Program.step[i].Pv_type1        = 0;    // 参数或变量类型1
        PdHoldData->Program.step[i].Pv_opt1         = 0;    // 参数或变量操作符1
        PdHoldData->Program.step[i].Pv_val1.Float32 = 0;    // 参数或变量值1
        PdHoldData->Program.step[i].Pv_type2        = 0;    // 参数或变量类型2
        PdHoldData->Program.step[i].Pv_opt2         = 0;    // 参数或变量操作符2
        PdHoldData->Program.step[i].Pv_val2.Float32 = 0;    // 参数或变量值2
        PdHoldData->Program.step[i].Delta_t         = 0;    // 时间差
        PdHoldData->Program.step[i].Delta_u         = 0;    // 电压差
        PdHoldData->Program.step[i].Delta_i         = 0;    // 电流差
        PdHoldData->Program.step[i].Stp_act         = 0;    // 阶段结束动作 0:下一步，0xffff:结束, 其他：目标阶段
        PdHoldData->Program.step[i].Rev16a          = 0;    // 保留
    }
    
    return (ret);
}
//----------------------------------------------------------------------------------------------------
//  保存重置工艺后后的跳转目标阶段
//  参数  重置后当前阶段工艺的跳转目标阶段号
//----------------------------------------------------------------------------------------------------
void Chan::Save_RenewStp(U16 obj_stp)
{
    PdHoldData->RunStpPrg.Stp_act = obj_stp;    //目标阶段号，非工艺行号
}
//----------------------------------------------------------------------------------------------------
//  工艺接收结束后的启动处理
/*
    start_mode  0x0000      - 正常启动
                0xffff      - 续接启动
                其他合法值   - 重置启动
*/ 
//----------------------------------------------------------------------------------------------------
U8  Chan::PrgRcvStartCtrl(U16 start_mode)
{
    U8  ret = 0;
    
    switch (PdHoldData->NrRData.CurWkStat)
    {
        case WORKSTAT_INIT:     // 上电初始化
        {
            break;
        }
        case WORKSTAT_STOP:     // 停止状态
        {
            if (start_mode == NORMAL_START) {   // 新的启动
                PdHoldData->NrRData.NxtWkStat   = WORKSTAT_RUN; // 启动
            
            } else if (start_mode == CONTINUE_START) {  // 续接启动
                PdHoldData->NrRData.NxtWkStat   = WORKSTAT_RUN; // 启动
            
            }
            mFirstIn = 0;       // 清标记，准备延时启动 15-1125
            mStartRunDly = 0;   // 启动延时清零
            break;
        }
        case WORKSTAT_RUN:      // 运行状态 
        {
            if ((start_mode != NORMAL_START)
            &&  (start_mode != CONTINUE_START)) {   // 重置方案
            
            }
            break;
        }
        case WORKSTAT_PAUSE:    // 暂停状态
        {
            if ((start_mode != NORMAL_START)
            &&  (start_mode != CONTINUE_START)) {   // 重置方案
            
            }
            break;
        }
        case WORKSTAT_XPAUSE:   // 异常暂停状态
        {
            if ((start_mode != NORMAL_START)
            &&  (start_mode != CONTINUE_START)) {   // 重置方案
            
            }   
            break;
        }
        case WORKSTAT_ABNOR:    // 异常状态
        {
            if ((start_mode != NORMAL_START)
            &&  (start_mode != CONTINUE_START)) {   // 重置方案
            
            }
            break;
        }
        default:                // 未定义工作状态
        {
            PdHoldData->NrRData.NxtWkStat = WORKSTAT_XPAUSE;
            break;
        }
    }
    
    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  根据重置目标工步获取所在接收工艺行索引
/*
    参数  prog_stp    实际工步号(从1开始，为0对应工作模式‘否则’，在这里为非法参数)
    返回  指定工步号prog_stp所在工艺缓冲的行号，以备转阶段取工艺时用
*/
//----------------------------------------------------------------------------------------------------
U16 Get_RenewLine(U16 prog_stp)
{
    U16 retLine = LINE_ERR;
    U16 i;

    if ((prog_stp == 0) 
    ||  (prog_stp > PrgRcvCtrl.DataBuf.pageline.TotLine)) { // 非法的工步号
        return  (retLine);  // 错误返回
    }
    for (i=0; i<PrgRcvCtrl.DataBuf.pageline.TotLine; i++) {
        if (PrgRcvCtrl.DataBuf.step[i].No == prog_stp) {
            retLine = i;
            break;
        }
    }
    
    return  (retLine);
}
//----------------------------------------------------------------------------------------------------
//获取通道实时异常数据
//----------------------------------------------------------------------------------------------------
U16 Chan::Test_RealAbn(void)
{
    return  (PdHoldData->RunData.AbnCode);
}
//----------------------------------------------------------------------------------------------------
//  保存实时异常数据
//----------------------------------------------------------------------------------------------------
void Chan::Set_RealAbn(U16 val)
{
    PdHoldData->RunData.AbnCode = val;
}
//----------------------------------------------------------------------------------------------------
//  保存实时电流数据 (放电加负号)
//  参数改为浮点 16-0708
//----------------------------------------------------------------------------------------------------
void Chan::Set_RealCur(float val) 
{
    // 下位机扩展数据处理（根据设备结构处理） 16-0514
    switch (cDev.mInfo.dev_str_type)
    {
        default:
        case DS_GENERAL:        // 通用类型设备结构
        case DS_NEILSBED:       // 针床类型设备结构
        case DS_POURING:        // 倒出类型设备结构
        case DS_OPENSHELF:      // 开架类型设备结构 
        {
            PdHoldData->RunData.Cur = val;
            break;
        }
    }
}
//----------------------------------------------------------------------------------------------------
//  保存瞬时电流数据
//----------------------------------------------------------------------------------------------------
void Chan::Set_InstCur(S32 val) 
{
    // 下位机瞬时电流处理（根据设备结构处理） 16-0514
    switch (cDev.mInfo.dev_str_type)
    {
        default:
        case DS_GENERAL:        // 通用类型设备结构
        case DS_NEILSBED:       // 针床类型设备结构
        case DS_POURING:        // 倒出类型设备结构
        case DS_OPENSHELF:      // 开架类型设备结构 
        {
            mCanExData.V2.InstCur  = val;
            break;
        }
    }
}
//----------------------------------------------------------------------------------------------------
//  获取模拟设定电压数据（取保护电压中间值）
//----------------------------------------------------------------------------------------------------
#ifdef  EN_SIM_CHAN
    S32 Chan::GetSimSetVol(void)
    {
        S32 mid;
    
        mid =(PdHoldData->RunGblPrg.Ov_val.Float32 
            + PdHoldData->RunGblPrg.Uv_val.Float32) / 2;
        if (mid > cDev.mInfo.max_chr_vol) {
            mid = cDev.mInfo.max_chr_vol;
        } else if (mid < cDev.mInfo.min_dch_vol) {
            mid = cDev.mInfo.min_dch_vol;
        }
        return  (mid);
    }
#endif
//----------------------------------------------------------------------------------------------------
//  保存实时电压数据
//  参数改为浮点 16-0708
//----------------------------------------------------------------------------------------------------
void Chan::Set_RealVol(float val)
{
    PdHoldData->RunData.Vol = val;
}
//----------------------------------------------------------------------------------------------------
//  保存瞬时电压数据
//----------------------------------------------------------------------------------------------------
void Chan::Set_InstVol(S32 val) 
{
    // 下位机瞬时电压处理（根据设备结构处理） 16-0514
    switch (cDev.mInfo.dev_str_type)
    {
        default:
        case DS_GENERAL:        // 通用类型设备结构
        case DS_NEILSBED:       // 针床类型设备结构
        case DS_POURING:        // 倒出类型设备结构
        case DS_OPENSHELF:      // 开架类型设备结构 
        {
            mCanExData.V2.InstVol  = val;
            break;
        }
    }
}
//----------------------------------------------------------------------------------------------------
//  保存实时温度数据(0.01摄氏度)
//----------------------------------------------------------------------------------------------------
void Chan::Set_RealTemp(S16 val)
{
    float ftmp;

    ftmp = val;
    PdHoldData->RunData.Temp = ftmp/100;
}
//----------------------------------------------------------------------------------------------------
//  保存实时功率数据
//----------------------------------------------------------------------------------------------------
void Chan::Set_RealPow(void)
{
//    float cur_abs;    // 电流绝对值
//    
//    if (PdHoldData->RunData.Cur < 0) {
//        cur_abs = -PdHoldData->RunData.Cur;
//    } else {
//        cur_abs = PdHoldData->RunData.Cur;
//    }
//    
//    PdHoldData->NrRData.Pow = cur_abs * PdHoldData->RunData.Vol / 1000;   // 功率: mW
}
//----------------------------------------------------------------------------------------------------
//  保存实时电阻数据
//----------------------------------------------------------------------------------------------------
void Chan::Set_RealRes(void)
{
//    float cur_abs;    // 电流绝对值

//    if (PdHoldData->RunData.Cur < 0) {
//        cur_abs = -PdHoldData->RunData.Cur;
//    } else if (PdHoldData->RunData.Cur > 0) {
//        cur_abs = PdHoldData->RunData.Cur;
//    } else {
//        PdHoldData->NrRData.Res   = 0;    // 电流为0，强制电阻为0
//        return;
//    }

//    PdHoldData->NrRData.Res = PdHoldData->RunData.Vol / cur_abs;  // 电阻 mΩ
}
//----------------------------------------------------------------------------------------------------
//  保存实时能量数据    （测试用）
//----------------------------------------------------------------------------------------------------
void Chan::Set_RealEngy(float val)
{
    PdHoldData->RunData.Engy = val;
}
//----------------------------------------------------------------------------------------------------
//  保存单体电压极值
//----------------------------------------------------------------------------------------------------
void Chan::Set_RealMVi(S32 val)
{
    PdHoldData->NrRData.MinVi.fdb = val&0xffff; // 最小单体电压
    PdHoldData->NrRData.MaxVi.fdb = val>>16;    // 最大单体电压
}
//----------------------------------------------------------------------------------------------------
//  保存单体温度极值
//----------------------------------------------------------------------------------------------------
void Chan::Set_RealMTi(S32 val)
{
    PdHoldData->NrRData.MinTi.fdb = val&0xffff; // 最小单体温度
    PdHoldData->NrRData.MaxTi.fdb = val>>16;    // 最大单体温度
}
//----------------------------------------------------------------------------------------------------
//  保存下位机故障
//----------------------------------------------------------------------------------------------------
void Chan::Set_Stat(U32 val)
{
    PdHoldData->NrRData.LowerStat = val;
}
//----------------------------------------------------------------------------------------------------
//  保存下位机扩展数据 15-1126
//----------------------------------------------------------------------------------------------------
void Chan::Set_ExData(S32 rda, S32 rdb)
{
    // 下位机扩展数据处理（根据设备结构处理） 16-0514
    switch (cDev.mInfo.dev_str_type)
    {
        default:
        case DS_GENERAL:        // 通用类型设备结构
        case DS_NEILSBED:       // 针床类型设备结构
        case DS_POURING:        // 倒出类型设备结构
        case DS_OPENSHELF:      // 开架类型设备结构 
        {
            U8  inx     = (rdb >>  0) & 0xff;   // 扩展数据索引
            U8  stp     = (rdb >>  8) & 0xff;   // 下位机实时运行阶段
            S16 s16buf  = (rdb >> 16) & 0xffff; // 扩展数据

            PdHoldData->NrRData.LowerStat   = rda;
            mCanExData.V2.RealErr           = rda;
            if (stp == PdHoldData->RunData.Step) {
                if (mSSCtrl.SyncFlag) {             // 下位机上传阶段同中位机请求阶段相同
                    os_sem_send(mStpSyncSem);       // 发送阶段同步信号量
                }
            }            

            // 锂电下位机CAN数据   //15-1214
            switch (inx)
            {
                case 3:     // 实时母线电压
                {
                    mCanExData.V2.Vbus = s16buf;
                    break;
                }
                case 2:     // 系统温度(传感器温度等)
                {
                    mCanExData.V2.Temp1 = (s16buf+5)/10;
                    break;
                }
                default:
                {
                    break;
                }
            }            

            break;
        }
    }    
}
//----------------------------------------------------------------------------------------------------
//  发送下位机控制数据清零 15-1019 带指针参数调用
//----------------------------------------------------------------------------------------------------
void  Chan::ClrCanCtrlData(U8 *ptr)
{
    memset (ptr, 0, sizeof(mCanCtrlData));
}
//----------------------------------------------------------------------------------------------------
//  接收下位机扩展数据清零
//----------------------------------------------------------------------------------------------------
void  Chan::ClrCanExData(void)
{
    U8  *ptr;
  
    ptr = (U8 *)&mCanExData;
    memset (ptr, 0, sizeof(mCanExData));
}
//----------------------------------------------------------------------------------------------------
//  获取通道控制数据
//----------------------------------------------------------------------------------------------------
strCanCtrl Chan::GetCtrlData(void) const
{
    return mCanCtrlData;
}
//----------------------------------------------------------------------------------------------------
//  获取下位机开关量输出数据 16-0514
//----------------------------------------------------------------------------------------------------
U8  Chan::GetOutBit(U8 sync)
{
    unOutBit out;
    
    out.byte = 0;
    if (Get_CabCtrlFlag()) {    // 容检指示
        out.bits.led0 = OUT_ON;//OUT_BLINK;
    } else {
        switch (PdHoldData->NrRData.CurWkStat)
        {
            default:
            case WORKSTAT_INIT:     // 初始化状态 （不判断故障，不接受PC干预命令）
            case WORKSTAT_STOP:     // 停止状态
            {
                out.bits.led0 = OUT_OFF;
                break;
            }
            case WORKSTAT_RUN:      // 运行状态
            case WORKSTAT_PAUSE:    // 暂停状态 （运行时人工暂停）
            {
                out.bits.led0 = OUT_ON;
                break;
            }            
            case WORKSTAT_XPAUSE:   // 异常暂停状态 （运行时有异常）
            {
                if (sync) {
                    out.bits.led0 = OUT_ON;
                } else {
                    out.bits.led0 = OUT_OFF;
                }
//                out.bits.led0 = OUT_BLINK;
                break;
            }
            case WORKSTAT_ABNOR:    // 异常状态 （非运行时有异常）
            {
                out.bits.led0 = OUT_OFF;
                break;
            }
        }
    }
    
    return  (out.byte);
}
//----------------------------------------------------------------------------------------------------
//  获取通道单体极值
//  计算单体极值，采用平均分配后调整，并按全局工艺测试点数限制 16-0311
//----------------------------------------------------------------------------------------------------
void Chan::Get_ViTiExtremum(void)
{
#ifdef  USE_EXT_MAXMIN  // 使用外部的单体极值,同主通道地址一一对应，区别于软配置后的单体极值 
  
    PdHoldData->NrRData.MaxVi.fdb   = cSubChan.mMaxVi[mAddr];
    PdHoldData->NrRData.MinVi.fdb   = cSubChan.mMinVi[mAddr];
    
    PdHoldData->NrRData.MaxTi.fdb   = cSubChan.mMaxTi[mAddr];
    PdHoldData->NrRData.MinTi.fdb   = cSubChan.mMinTi[mAddr];

#else

// 16-0311 comment
//    U16 i;
//    S16 max = cSubChan.Vi[cDev.mSampCfg.ch[mAddr].ViOffset];
//    S16 min = cSubChan.Vi[cDev.mSampCfg.ch[mAddr].ViOffset];

//    for (i=0; i<cDev.mInfo.vi_num; i++) {
//        if (cSubChan.Vi[cDev.mSampCfg.ch[mAddr].ViOffset+i] > max) {
//            max = cSubChan.Vi[cDev.mSampCfg.ch[mAddr].ViOffset+i];
//        } else if (cSubChan.Vi[cDev.mSampCfg.ch[mAddr].ViOffset+i] < min) {
//            min = cSubChan.Vi[cDev.mSampCfg.ch[mAddr].ViOffset+i];
//        }
//    }
//    PdHoldData->NrRData.MaxVi.fdb = max;
//    PdHoldData->NrRData.MinVi.fdb = min;

//    max = cSubChan.Ti[cDev.mSampCfg.ch[mAddr].TiOffset];
//    min = cSubChan.Ti[cDev.mSampCfg.ch[mAddr].TiOffset];

//    for (i=0; i<cDev.mInfo.ti_num; i++) {
//        if (cSubChan.Ti[cDev.mSampCfg.ch[mAddr].TiOffset+i] > max) {
//            max = cSubChan.Ti[cDev.mSampCfg.ch[mAddr].TiOffset+i];
//        } else if (cSubChan.Ti[cDev.mSampCfg.ch[mAddr].TiOffset+i] < min) {
//            min = cSubChan.Ti[cDev.mSampCfg.ch[mAddr].TiOffset+i];
//        }
//    }
//    PdHoldData->NrRData.MaxTi.fdb = max;
//    PdHoldData->NrRData.MinTi.fdb = min;

    U16 i,
        tst_vinum,
        tst_tinum,
        start_inx;
    S16 max;
    S16 min;

    switch (PdHoldData->NrRData.CurWkStat)
    {
        case WORKSTAT_RUN:      // 运行状态 
        case WORKSTAT_PAUSE:    // 暂停状态
        case WORKSTAT_XPAUSE:   // 异常暂停状态
        {
            if ((PdHoldData->RunGblPrg.Tst_ViNum < cDev.mInfo.vi_num)
            &&  (PdHoldData->RunGblPrg.Tst_ViNum != 0)) {
                tst_vinum = PdHoldData->RunGblPrg.Tst_ViNum;
            } else {
                tst_vinum = cDev.mInfo.vi_num;
            }
            if ((PdHoldData->RunGblPrg.Tst_TiNum < cDev.mInfo.ti_num)
            &&  (PdHoldData->RunGblPrg.Tst_TiNum != 0)) {
                tst_tinum = PdHoldData->RunGblPrg.Tst_TiNum;
            } else {
                tst_tinum = cDev.mInfo.ti_num;
            }
            break;
        }
        case WORKSTAT_ABNOR:    // 异常状态
        case WORKSTAT_INIT:     // 上电初始化不接受PC控制
        case WORKSTAT_STOP:     // 停止状态
        default:
        {
            tst_vinum = cDev.mInfo.vi_num;
            tst_tinum = cDev.mInfo.ti_num;
            break;
        }
    }
    
    // 单体电压极值判断  
    start_inx = cDev.mInfo.vi_num*mAddr;
    max =
    min = cSubChan.Vi[start_inx];
    for (i=0; i<tst_vinum; i++) {
        if (cSubChan.Vi[start_inx+i] > max) {
            max = cSubChan.Vi[start_inx+i];
        } else if (cSubChan.Vi[start_inx+i] < min) {
            min = cSubChan.Vi[start_inx+i];
        }
    }
    PdHoldData->NrRData.MaxVi.fdb = max;
    PdHoldData->NrRData.MinVi.fdb = min;

    // 单体温度极值判断
    start_inx = cDev.mInfo.ti_num*mAddr;
    max =
    min = cSubChan.Ti[start_inx];
    for (i=0; i<tst_tinum; i++) {
        if (cSubChan.Ti[start_inx+i] > max) {
            max = cSubChan.Ti[start_inx+i];
        } else if (cSubChan.Ti[start_inx+i] < min) {
            min = cSubChan.Ti[start_inx+i];
        }
    }
    PdHoldData->NrRData.MaxTi.fdb = max;
    PdHoldData->NrRData.MinTi.fdb = min;
    
#endif
}
//----------------------------------------------------------------------------------------------------
//  发送烟感报警信号量
//----------------------------------------------------------------------------------------------------
//#if (DEV_STR_TYPE == DS_NEILSBED)   // 针床设备 -- 发送烟感信号量 16-0514
void Chan::SmokeSemSend(void)
{
    os_sem_send (mSmokeSem);
}
//#endif
//----------------------------------------------------------------------------------------------------
//  发送针床托盘到位信号量 
//----------------------------------------------------------------------------------------------------
//#if (DEV_STR_TYPE == DS_NEILSBED)   // 针床设备 -- 发送托盘到位信号量 16-0514
void Chan::InPlaceSemSend(void)
{
    os_sem_send (mInplaceSem);
}
//#endif  
//----------------------------------------------------------------------------------------------------
// 清除记录指针（修改明细数据单体，其他项等后）
//----------------------------------------------------------------------------------------------------
void Chan::RecPtrRst(void)
{ 
    PdHoldData->NrRData.RecNum      = 0;
    PdHoldData->NrRData.FullFlag    = FALSE;    // 记录缓冲溢出标记
    PdHoldData->NrRData.ReadPoint   = 0;        // 读记录缓冲指针
    PdHoldData->NrRData.WritePoint  = 0;        // 写记录缓冲指针
    PdHoldData->NrRData.RdPtrTemp   = 0;        // 读记录缓冲指针暂存(作为更新读记录指针用)
}    
//----------------------------------------------------------------------------------------------------
//  定时间主控程序 状态机
//----------------------------------------------------------------------------------------------------
void Chan::TickControlProc(void)
{
    //--------------------------------------------------
    // 状态转换，附带初始化
    if (PdHoldData->NrRData.CurWkStat != PdHoldData->NrRData.NxtWkStat) {
//        if (State_Init(PdHoldData->NrRData.CurWkStat, PdHoldData->NrRData.NxtWkStat)) 
        {
            if (PdHoldData->NrRData.NxtWkStat == WORKSTAT_STOP) {   // 转到结束，先前状态强制结束
                PdHoldData->NrRData.PreWkStat = 
                PdHoldData->NrRData.CurWkStat = PdHoldData->NrRData.NxtWkStat;
            } else {
                PdHoldData->NrRData.PreWkStat = PdHoldData->NrRData.CurWkStat;
                PdHoldData->NrRData.CurWkStat = PdHoldData->NrRData.NxtWkStat;
            }
        }
    }
    //--------------------------------------------------
    //获取单体电压，温度极值
    Get_ViTiExtremum();

    //运行状态处理程序
    switch (PdHoldData->NrRData.CurWkStat)
    {
        case WORKSTAT_INIT:     // 上电初始化状态
        {
//            PowerOnCtrlProc();  // 通道上电态控制
            break;
        }        
        case WORKSTAT_STOP:     // 停止状态
        {
            if((mFirstIn & (1<<WORKSTAT_STOP)) == 0) {  // 首次进入停止态
                mFirstIn = (1<<WORKSTAT_STOP);
//                Stop_Init();
            }
//            StopCtrlProc();     // 通道停止态控制
            break;
        }        
        case WORKSTAT_RUN:      // 运行状态
        {
            if((mFirstIn & (1<<WORKSTAT_RUN)) == 0) {       // 首次进入运行态
                if (mStartRunDly < cDev.mCtrlCfg.StartDly_t*(mAddr+1)) {  // 延时未到
                    mStartRunDly+= CTRL_PERIOD;
                    break;
                } else {                                        // 延时到，正式启动
                    mFirstIn = (1<<WORKSTAT_RUN);
                    mStartRunDly = 0;
                    PdHoldData->NrRData.CabCtrlFlag = 0;        // 启动，关容检标记
                }
            }
            //****************************************
//            RunCtrlProc();  // 运行控制处理
            //****************************************
            break;
        }
        case WORKSTAT_PAUSE:    // 暂停状态
        {
            if((mFirstIn & (1<<WORKSTAT_PAUSE)) == 0) {     // 首次进入暂停态
                mFirstIn = (1<<WORKSTAT_PAUSE);
            }
//            PauseCtrlProc();    // 通道暂停态控制
            break;
        }
        case WORKSTAT_XPAUSE:   // 异常暂停状态
        {
            if((mFirstIn & (1<<WORKSTAT_XPAUSE)) == 0) {    // 首次进入异常暂停态
                mFirstIn = (1<<WORKSTAT_XPAUSE);
                PdHoldData->NrRData.AbnCodeBak = PdHoldData->RunData.AbnCode;   // 进入异常暂停，备份故障状态
            }
//            XPauseCtrlProc();   // 通道异常暂停态控制
            break;
        }
        case WORKSTAT_ABNOR:    // 异常状态
        {
            if((mFirstIn & (1<<WORKSTAT_ABNOR)) == 0) {     // 首次进入异常态
                mFirstIn = (1<<WORKSTAT_ABNOR);
                PdHoldData->NrRData.AbnCodeBak = PdHoldData->RunData.AbnCode;   // 进入异常，备份故障状态
            }
//            AbnorCtrlProc();    // 通道异常态控制
            break;
        }
        default:                // 未定义工作状态
        {
            mFirstIn = 0;
            PdHoldData->RunData.AbnCode = (PdHoldData->RunData.Mode<<8) | ERR_WKSTAT;   // 未知工作状态错误
            PdHoldData->NrRData.NxtWkStat = WORKSTAT_XPAUSE;    // 进入未知工作状态错误
            break;
        }
    }
}
//----------------------------------------------------------------------------------------------------
// 通道明细数据长度复位 15-1122
//----------------------------------------------------------------------------------------------------
void ChanRecPtrRstProc(void)
{
    U32 i;
    
    for (i=0; i<cDev.mInfo.chan_num; i++) {
        cChan[i].RecPtrRst();
    }
}
//----------------------------------------------------------------------------------------------------
//  除了工步索引从1开始，其他如页，行索引都从0开始
//------------------------------------------------------------
//  100ms/50ms/进行控制程序,
//------------------------------------------------------------
void ControlProc(void)
{
    U32 i;
    
    //PC从机超时控制
    cDev.PCSlaTout();
    //通道节拍控制
    for (i=0; i<cDev.mInfo.chan_num; i++) {
        cChan[i].TickControlProc();
    }
}
//----------------------------------------------------------------------------------------------------
//eof
