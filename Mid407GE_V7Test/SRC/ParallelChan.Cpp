//----------------------------------------------------------------------------------------------------
// Header:      通道并联处理程序
// File Name:   ParalellChan.cpp
// Author:      David
// Date:        2013-05-16
//----------------------------------------------------------------------------------------------------
#define  _IN_PCHAN_CPP
extern "C" 
{
    #include    <string.h>
    #include    "RTL.h"
    #include    "main.h"
    #include    "app_config.h"
    #include    "sys_config.h"
    #include    "spi.h"
    #include    "led.h"
}
#include    "app_can.hpp"
#include    "Dev.hpp"
#include    "chan.hpp"
#include    "parallelchan.hpp"
//----------------------------------------------------------------------------------------------------
//构造函数
//----------------------------------------------------------------------------------------------------
ParallelChan::ParallelChan()
{
    U32 i;

    for (i=0; i<MAX_NODE_NUM; i++) {    // 16-0120
        mConnection[i]  = NO_CONNECT;   // 未联机状态 16-0702
//        mConnectCnt[i]  = 0;            // 清零计数
        mDataFormat[i]  = FIXED_FORMAT; //FLOAT_FORMAT; // 默认定点数据格式 16-0702 test
        mCanOffCnt[i]   = 0;
    }

#ifdef  EN_PARALLEL     // 并联默认映射表

    for (i=0; i<MAX_CHAN_NUM; i++) {
        mNodeTbl.lead_node[i] = i * DEF_PNODE_NUM;      // 并联通道主节点索引（并联首节点）
    }

    for (i=0; i<MAX_NODE_NUM; i++) {
        mNodeTbl.obj_pchan[i] = i / DEF_PNODE_NUM;      // 节点映射目标并联通道（超出部分须在应用中限制！！！）
        if (mNodeTbl.obj_pchan[i] >= MAX_CHAN_NUM) {    // 节点映射到通道，通道越界处理 tbc...
            // ......
        }
    }
    mNodeTbl.node_num = MAX_CHAN_NUM * DEF_PNODE_NUM;   // 实际应用的节点数

#else                   // 非并联的节点数 == 通道数
    
    for (i=0; i<MAX_CHAN_NUM; i++) {
        mNodeTbl.lead_node[i] = i;  // 非并联，一一映射
    }

    for (i=0; i<MAX_NODE_NUM; i++) {
        mNodeTbl.obj_pchan[i] = i;  // 非并联，一一映射
    }
    mNodeTbl.node_num = MAX_NODE_NUM;

#endif
}
//----------------------------------------------------------------------------------------------------
//  初始化通道到节点（CAN地址-1）检索表
/*
    参数  无
    返回  1：成功  0：失败
    备注  15-1212 两路并联默认，测试用！
          16-0129 使用设备信息的并联数映射
*/
//----------------------------------------------------------------------------------------------------
U8  ParallelChan::InitNodeTbl(void)
{
    U32 i;
    U8  ret = 1;

    mNodeTbl.Mark  = NODETBL_MARK;

#ifdef  EN_PARALLEL     // 并联时的默认配置表

    if ((cDev.mInfo.chan_num > MAX_CHAN_NUM)
    ||  (cDev.mInfo.chan_num == 0)) {
        return  (0);    // 控制通道数 错
    }
    if ((cDev.mInfo.par_node_num > MAX_CHAN_NUM)
    ||  (cDev.mInfo.par_node_num == 0)) {
        return  (0);    // 允许并联，并联节点数错
    }
    if ((cDev.mInfo.chan_num*cDev.mInfo.par_node_num) > MAX_NODE_NUM) {
        return  (0);    // 总节点数 错
    }

    for (i=0; i<MAX_CHAN_NUM; i++) {
        mNodeTbl.lead_node[i] = i * cDev.mInfo.par_node_num;   // 并联通道主节点索引（并联首节点）
    }
    for (i=0; i<MAX_NODE_NUM; i++) {
        mNodeTbl.obj_pchan[i] = i / cDev.mInfo.par_node_num;   // 节点映射目标并联通道（超出部分须在应用中限制！！！）
        if (mNodeTbl.obj_pchan[i] >= MAX_CHAN_NUM) {           // 节点映射到通道，通道越界处理 tbc...
            // ......
        }
    }
    mNodeTbl.node_num = cDev.mInfo.chan_num * cDev.mInfo.par_node_num;

#else                   // 非并联的节点数 == 通道数
    
    if ((cDev.mInfo.chan_num > MAX_CHAN_NUM)
    ||  (cDev.mInfo.chan_num == 0)) {
        return  (0);    // 控制通道数 错
    }
    if (cDev.mInfo.par_node_num != 0) {
        return  (0);    // 不允许并联，并联数须为0
    }
    if ((cDev.mInfo.chan_num*cDev.mInfo.par_node_num) > MAX_NODE_NUM) {
        return  (0);    // 总节点数 错
    }    
    for (i=0; i<MAX_CHAN_NUM; i++) {
        mNodeTbl.lead_node[i] = i;   // 非并联通道主节点索引（自己）
    }    
    mNodeTbl.node_num = cDev.mInfo.chan_num;

#endif

    return (ret);
}
//----------------------------------------------------------------------------------------------------
//  初始化加载通道到节点（CAN地址-1）检索表
/*
    参数      无
    返回      ret:  1－成功
                    0－失败
    备注      16-0129 调整，不再使用存储数据，依赖设备信息的通道并联节点数
            
*/
//----------------------------------------------------------------------------------------------------
U8  ParallelChan::LoadNodeTbl(void)
{


    U8  ret = 0;

    ret = InitNodeTbl(); // 初始化并联查找表

    return  (ret);
}
//--------------------------------------------------
//  保存通道到节点（CAN地址-1）检索表
/*
    参数  无
    返回  ret:  1－成功
                0－失败
    备注：
*/
//--------------------------------------------------
U8  ParallelChan::SaveNodeTbl(U8 *ptr) // 尚未使用
{
    U8  ret = 0x00;


    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//检查通道与节点检索表 15-1212
//  16-0129 线性并联（来自设备信息）
//----------------------------------------------------------------------------------------------------
U8  ParallelChan::CheckNodeTbl(strNodeTbl *sptr)
{
    U8  ret = 0;
    U32 i, j;
    
    j = cDev.mInfo.chan_num;
    for (i=0; i<j; i++) {
        if (sptr->lead_node[i] >= MAX_NODE_NUM) {       // 主节点超界
            return  (0x01);
        }
    }
    if (sptr->lead_node[j] > MAX_NODE_NUM) {            // 最后从节点超界
        return  (0x01);
    }
    j = cDev.mInfo.chan_num * cDev.mInfo.par_node_num;  // 总节点数
    if ((sptr->node_num > MAX_NODE_NUM)
    ||  (sptr->node_num != j)) {                        // 总节点数错
         return  (0x02);
    }
    for (i=0; i<j; i++) {
        if (sptr->obj_pchan[i] >= MAX_CHAN_NUM) {       // 目标并联通道超界
            return  (0x04);
        }
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
// 获取联机状态 16-0714
/*
    参数:  node -联机节点 
    返回:  0x00－未联机
           0x01－联机
*/
//----------------------------------------------------------------------------------------------------
U8  ParallelChan::GetConnectStat(U8 node)
{
    return  (mConnection[node]);    // 获取联机状态
}
//----------------------------------------------------------------------------------------------------
//  CAN 在线或脱机计数
/*
    返回:  0x00－在线
           0x01－脱机
*/
//----------------------------------------------------------------------------------------------------
U8  ParallelChan::CanOffTest(U8 unit, S16 cnt)
{
    U8  ret = 0;

    if (cnt < 0) {
        mCanOffCnt[unit] = 0;
    } else {
        mCanOffCnt[unit] += cnt;
    }
    if (mCanOffCnt[unit] >= cDev.mCtrlCfg.CanOff_n) {   // 脱机 
        mCanOffCnt[unit] = cDev.mCtrlCfg.CanOff_n;
        ret = 1;
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  发送下位机控制数据清0
//----------------------------------------------------------------------------------------------------
void  ParallelChan::ClrCanData(U8 no)
{
    U8  *ptr;

    ptr = (U8 *)&mCanTxData[no];
    memset (ptr, 0, sizeof(mCanTxData[no]));
}
//----------------------------------------------------------------------------------------------------
//  动态分配下位机控制数据
//----------------------------------------------------------------------------------------------------
strCanCtrl  ParallelChan::AllocCanData(strCanCtrl ctrldata)
{
    strCanCtrl ret_data;

    return  (ret_data);
}
//----------------------------------------------------------------------------------------------------
//  发送下位机联机命令 16-0718
/*
  参数  unit    - 对于并联，表示CAN地址，CAN地址与通道号不相同，通过查表可找到所属并联通道号
        *msg    - CAN发送缓冲指针
*/
//----------------------------------------------------------------------------------------------------
void  ParallelChan::CanConnectTxProc(U8 unit, CAN_msg *msg)
{    
    msg->id         = ((unit + 1) << 8)     // 地址
                    | (MCAN_RD_CONNECT);    // 联机命令
    msg->len        = 8;
    msg->ch         = 0;
    msg->format     = EXTENDED_FORMAT;
    msg->type       = DATA_FRAME;
    msg->data[0]    = 0;
    msg->data[1]    = 0;
    msg->data[2]    = 0;
    msg->data[3]    = 0;
    msg->data[4]    = 0;
    msg->data[5]    = 0;
    msg->data[6]    = 0;
    msg->data[7]    = 0;  

    if (CanOffTest(unit, 0x01) == 0x01) // 联机超时
    {
    #ifdef  EN_SIM_CHAN     // 模拟CAN，脱机，不报故障，置联机成功
        mConnection[unit] = OK_CONNECT;        
    #else
        
        
    #endif
        
    }

}
//----------------------------------------------------------------------------------------------------
//  循环发送下位机控制数据设定 15-0909
//  需要分配 
/*
  参数  unit    - 对于并联，表示CAN地址，CAN地址与通道号不相同，通过查表可找到所属并联通道号
                  非并联，表示实际的通道号，CAN地址与通道号一一对应
        f_no    - 实时命令帧号(16-0719 加入帧号为0的联机处理)
        f_inx   - 第二帧实时命令的子帧索引
        *msg    - CAN发送缓冲指针
*/
//----------------------------------------------------------------------------------------------------
void  ParallelChan::CanTxProc(U8 unit, U8 f_no, U8 f_inx, CAN_msg *msg)
{
//----------------------------------------
#ifdef  EN_PARALLEL     // 允许并联,分配下位机控制数据

    static strCanCtrl data_buf;
    U8  pnode_num   = cDev.mInfo.par_node_num;      // 通道并联节点数
    U8  pchan_no    = mNodeTbl.obj_pchan[unit];     // 单元所在并联通道号（用户界面）
    U8  u_master    = mNodeTbl.lead_node[pchan_no]; // 主通道（第一通道）

    if ((pchan_no   >= MAX_CHAN_NUM)
    ||  (pnode_num  == 0)
    ||  (pnode_num  >  MAX_NODE_NUM)
    ||  (unit       >= MAX_NODE_NUM)
    ||  (u_master   >= MAX_NODE_NUM)) {
        return;
    }

    // 获取所属并联通道的控制数据（已经映射到旧版命令）
    data_buf = cChan[pchan_no].GetCtrlData();
    // 分配控制数据
    switch (data_buf.Mode)
    {
        default:
        case STATICMODE:            // 静置
        case EXCTRLMODE:            // 外控模式
        case ENDMODE:               // 结束模式
        {
            ClrCanData(unit);
            mCanTxData[unit].Step = data_buf.Step;  // 16-0510 阶段下发            
            mCanTxData[unit].Mode = data_buf.Mode;  // 16-0510 模式下发
            break;
        }
        case HLCHARGEMODE:          // 恒流充
        case HLDISCHARGEMODE:       // 恒流放
        case HYCHARGEMODE:          // 恒压充
        case HYDISCHARGEMODE:       // 恒压放
        case XPCCHARGEMODE:         // 斜坡电流充
        case XPCDISCHARGEMODE:      // 斜坡电流放
        {
            //  (0) 简单并联
            mCanTxData[unit]        = data_buf;
            mCanTxData[unit].Cur    /= pnode_num;
            break;
        }
        case HPCHARGEMODE:          // 恒功率充
        case HPDISCHARGEMODE:       // 恒功率放
        case XPPCHARGEMODE:         // 斜坡功率充
        case XPPDISCHARGEMODE:      // 斜坡功率放
        {
            mCanTxData[unit]        = data_buf;
            mCanTxData[unit].Cur    /= pnode_num;
            mCanTxData[unit].Ext1   /= pnode_num;
            break;
        }
        case HRCHARGEMODE:          // 恒电阻充
        case HRDISCHARGEMODE:       // 恒电阻放
        {
            mCanTxData[unit]        = data_buf;
            mCanTxData[unit].Cur    /= pnode_num;
            mCanTxData[unit].Ext1   *= pnode_num;
            break;
        }
        case XPVCHARGEMODE:         // 斜坡电压充
        case WKCHARGEMODE:          // 温度控制充
        case WKDISCHARGEMODE:       // 温度控制放
        case FCHARGEMODE:           // 反充电
        case FDISCHARGEMODE:        // 反放电
        case PULSECHARGEMODE:       // 脉冲充电
        case PULSECH_DCHMODE:       // 脉冲充放电 
        {
            mCanTxData[unit]        = data_buf;
            mCanTxData[unit].Cur    /= pnode_num;
            break;
        }
    }
//----------------------------------------
#else   // 非并联CAN发送数据
    
    // 非并联时通道边界判断
    if (unit > MAX_CHAN_NUM) {  // 发送CAN命令，通道限制
        return;
    }
    // 发送第一帧数据前更新发送缓冲工艺 15-0909
    if (f_no == 0x01) {
        mCanTxData[unit] = cChan[unit].GetCtrlData();
    }
    
#endif  

//----------------------------------------

// 传统CANID，地址+命令
    
        // 加入第0帧联机处理
    if (f_no == 0x00)   // 0x3F 联机处理 16-0719
    {
        msg->id         = ((unit + 1) << 8)     // 地址
                        | (MCAN_RD_CONNECT);    // 联机命令
        msg->len        = 8;
        msg->ch         = 0;
        msg->format     = EXTENDED_FORMAT;
        msg->type       = DATA_FRAME;
        msg->data[0]    = 0;
        msg->data[1]    = 0;
        msg->data[2]    = 0;
        msg->data[3]    = 0;
        msg->data[4]    = 0;
        msg->data[5]    = 0;
        msg->data[6]    = 0;
        msg->data[7]    = 0;      
    }
    else if (f_no == 0x01)   //0x41 设定控制电流电压
    {
        msg->id         = MCAN_WR_REALDATA1 | (unit + 1)<<8;
        msg->data[0]    = mCanTxData[unit].Cur;
        msg->data[1]    = mCanTxData[unit].Cur>>8;
        msg->data[2]    = mCanTxData[unit].Cur>>16;
        msg->data[3]    = mCanTxData[unit].Cur>>24;
        msg->data[4]    = mCanTxData[unit].Vol;
        msg->data[5]    = mCanTxData[unit].Vol>>8;
        msg->data[6]    = mCanTxData[unit].Vol>>16;
        msg->data[7]    = mCanTxData[unit].Vol>>24;
        msg->len        = 8;
        msg->ch         = 0;
        msg->format     = EXTENDED_FORMAT;
        msg->type       = DATA_FRAME;
    }
    else    //if (f_no == 0x02)//0x42 设定阶段方式,功率电阻,开关量等
    {   //RDA - (阶段+方式+IO+数据索引)
        //RDB - (与索引对应的数据)
        //索引  = 0 普通工作方式
        //      = 1 恒功率
        //      = 2 恒电阻
        //      >=3 下发数据 供显示板用
        
        msg->id = MCAN_WR_REALDATA2 | (unit + 1)<<8;
        

        {            // = 2 送功率或电阻设定
                    U8  outbit;     // 16-0514  第二帧IO下发
            static  U8  outsync;    // 同步标记
            
            if (unit == 0) {
                outsync ^= 0x01;
            }
        #ifdef  EN_PARALLEL // 使能并联 16-0518
            outbit = cChan[pchan_no].GetOutBit(outsync);
        #else               // 非并联
            outbit = cChan[unit].GetOutBit(outsync);
        #endif
            
            msg->data[0]    = mCanTxData[unit].Step;    // 阶段
            msg->data[1]    = mCanTxData[unit].Mode;    // 方式
            msg->data[2]    = 0x80|outbit;              // 同步位  IO 加入输出位控制 16-0514
            if ((mCanTxData[unit].Mode == HPCHARGEMODE) 
            ||  (mCanTxData[unit].Mode == HPDISCHARGEMODE)) {   // 恒功率
                msg->data[3]    = 1;    // 数据标志索引
                msg->data[4]    = mCanTxData[unit].Ext1;
                msg->data[5]    = mCanTxData[unit].Ext1>>8;
                msg->data[6]    = mCanTxData[unit].Ext1>>16;
                msg->data[7]    = mCanTxData[unit].Ext1>>24;
            } else if ((mCanTxData[unit].Mode == HRCHARGEMODE) 
                    || (mCanTxData[unit].Mode == HRDISCHARGEMODE)) {    // 恒电阻
                msg->data[3]    = 2;    // 数据标志索引
                msg->data[4]    = mCanTxData[unit].Ext1;
                msg->data[5]    = mCanTxData[unit].Ext1>>8;
                msg->data[6]    = mCanTxData[unit].Ext1>>16;
                msg->data[7]    = mCanTxData[unit].Ext1>>24;
            } else {
                msg->data[3]    = 0;
                msg->data[4]    = 0;
                msg->data[5]    = 0;
                msg->data[6]    = 0;
                msg->data[7]    = 0;
            }
        }
        msg->len    = 8;
        msg->ch     = 0;
        msg->format = EXTENDED_FORMAT;
        msg->type   = DATA_FRAME;
    }

    if (CanOffTest(unit, 0x01) == 0x01) // 脱机
    {
        // 脱机清联机状态 16-0702
        mConnection[unit]  = NO_CONNECT;
        
//--------------------------------------------------
#ifdef  EN_SIM_CHAN     // 脱机，不报故障，赋控制给实时值,自动同步
    //----------------------------------------------

    #ifdef  EN_PARALLEL     // 模拟允许并联--脱机时控制数据处理后作为实时返回

        if (u_master == unit) { // 15-1214 并联模拟返回数据，工艺拷贝为实时数据
            if (mCanTxData[unit].Mode & 0x0A) { //16-0520
                cChan[pchan_no].Set_RealCur(-data_buf.Cur); // 工艺电流无符号，放电需要加负号
            } else {
                cChan[pchan_no].Set_RealCur(data_buf.Cur);
            }
            cChan[pchan_no].Set_RealVol(data_buf.Vol);
            cChan[pchan_no].Set_RealPow();
            cChan[pchan_no].Set_RealRes();
            cChan[pchan_no].Clr_RealAbn(ERR_MCANOFFLINE);   // 模拟CAN情况,清脱机故障
        }
    //----------------------------------------------
    #else                   // 非并联，脱机模拟返回        
        // 控制数据作为实时返回（模拟CAN返回）- 非测试机，放电加负号
        if (mCanTxData[unit].Mode & 0x0A) {
            cChan[unit].Set_RealCur(-mCanTxData[unit].Cur);
        } else {
            cChan[unit].Set_RealCur(mCanTxData[unit].Cur);
        }
        if (mCanTxData[unit].Vol == 0) {    // 电压未设定
            cChan[unit].Set_RealVol(cChan[unit].GetSimSetVol());
        } else {
            cChan[unit].Set_RealVol(mCanTxData[unit].Vol);
        }
        cChan[unit].Set_RealPow();
        cChan[unit].Set_RealRes();
        cChan[unit].Clr_RealAbn(ERR_MCANOFFLINE);   // 模拟CAN情况,清脱机故障
                                                    // 15-0717
        if (cChan[unit].mSSCtrl.SyncFlag) {         // 有同步请求
            os_sem_send(cChan[unit].mStpSyncSem);   // 模拟CAN发送阶段同步信号量
        }
    #endif
//--------------------------------------------------
#else   // 非模拟产生数据，物理CAN脱机，实时数据处理
    //----------------------------------------------
    #ifdef  EN_PARALLEL     // 允许并联--脱机处理 15-1213

        U8  abn = cChan[pchan_no].Get_RealAbn();    // 当前并联通道故障状态
        float   f32tmp = 0; // 采用浮点处理实时电流电压 16-0708
        U32 i;
        
        // 脱机，清零单元实时数据，报故障
        mCanRxData[unit].Cur    = 0;
        mCanRxData[unit].Vol    = 0;
        
        // 脱机，更新节点所属并联通道实时数据
        for (i=0; i<pnode_num; i++) {
            switch (mDataFormat[i]) // 根据联机版本处理并联电流
            {
                default:
                case FIXED_FORMAT:  // 默认格式定点（mA,mV）
                {
                    f32tmp += mCanRxData[u_master+i].Cur;
                    break;
                }
                case FIX10_FORMAT:  // (0.1mA,0.1mV)  
                {
                    f32tmp += mCanRxData[u_master+i].Cur/10;
                    break;                
                }
                case FLOAT_FORMAT:  // 浮点格式
                {
                    f32tmp += *(float *)&mCanRxData[u_master+i].Cur;
                    break;
                }                
            }
        }
        cChan[pchan_no].Set_RealCur(f32tmp);
        if (u_master == unit) {                     // 主机脱机，清电压，置故障
            if ((abn == 0)
            ||  (abn == ERR_PCANOFFLINE)) {
                cChan[pchan_no].Set_RealAbn(ERR_MCANOFFLINE);   // 主控脱机故障
            }
            cChan[pchan_no].Set_RealVol(0);
        } else {
            if (abn == 0) {
                cChan[pchan_no].Set_RealAbn(ERR_PCANOFFLINE);   // 并联脱机故障
            }
        }
        cChan[pchan_no].Set_RealPow();
        cChan[pchan_no].Set_RealRes();
    //----------------------------------------------
    #else   // 禁止并联--脱机处理
        if (cChan[unit].Get_RealAbn() == 0) {
            cChan[unit].Set_RealAbn(ERR_MCANOFFLINE);   // 脱机故障
        }
        cChan[unit].Set_RealCur(0);
        cChan[unit].Set_RealVol(0);
        cChan[unit].Set_Stat(0);
        cChan[unit].Set_RealPow();
        cChan[unit].Set_RealRes();
    #endif
    //----------------------------------------------
#endif

    } 
}
//----------------------------------------------------------------------------------------------------
//  下位机主通道接收数据解析
//  注：如果并联，参数unit为物理通道号（即CAN地址-1），而局部变量pchan_no并联后的通道号，包含所有主从机，与unit不同
//      没有并联，unit就是通道号
//----------------------------------------------------------------------------------------------------
void ParallelChan::CanRxProc(U8 unit, U8 cmd, S32 *S32data)
{
    CAN_msg *ptrmsg;
    
#ifdef  EN_PARALLEL     // 并联情况下CAN接收数据处理
    
    U8  pnode_num   = cDev.mInfo.par_node_num;      // 通道并联节点数
    U8  pchan_no    = mNodeTbl.obj_pchan[unit];     // 节点所在并联通道号
    U8  u_master    = mNodeTbl.lead_node[pchan_no]; // 通道并联主机
    S32     S32tmp;
    float   f32tmp; // 实时电流电压浮点处理 16-0708
    U32 i,j;

    if ((pchan_no   >= MAX_CHAN_NUM)
    ||  (pnode_num  == 0)
    ||  (pnode_num  >  MAX_NODE_NUM)
    ||  (u_master   >= MAX_NODE_NUM)
    ||  (unit       >= MAX_NODE_NUM)) {
        unit = unit;
        return;
    }
    if (CanOffTest(unit, -1) == 0) {    // 在线
        if (u_master == unit) { // 主机在线
            if (cChan[pchan_no].Test_RealAbn() == ERR_MCANOFFLINE) {
                cChan[pchan_no].Set_RealAbn(0); // 并联去主CAN脱机故障
            }
            j = 0;
            for (i=u_master+1; i<(u_master+pnode_num); i++) {   // 所有从机脱机判断
                if (mCanOffCnt[i] >= cDev.mCtrlCfg.CanOff_n) {
                    break;
                } else {
                    j++;
                }
            }
            if (j == (pnode_num-1)) {
                if (cChan[pchan_no].Test_RealAbn() == ERR_PCANOFFLINE) {
                    cChan[pchan_no].Set_RealAbn(0); // 并联去并联CAN脱机故障 15-1214
                }
            }
        } 
    }

#else   // 非并联情况下CAN接收数据处理+通道边界判断 16-0122

    if (unit < MAX_CHAN_NUM) {  // 非并联通道边界限制
        if (CanOffTest(unit, -1) == 0) {    // 在线
            if (cChan[unit].Test_RealAbn() == ERR_MCANOFFLINE) {
                cChan[unit].Set_RealAbn(0); // 并联去主CAN脱机故障
            }
        }
    } else {    // 通道越界
        return;
    }

#endif

    switch(cmd)
    {
    //---------- 以下为 写下位机 返回 ----------
    case (0x80|MCAN_WR_IRATE):      //0x50://设置电流额定值
    case (0x80|MCAN_WR_URATE):      //0x51://设置电压额定值
    case (0x80|MCAN_WR_IPID):       //0x52://设置电流PID参数
    case (0x80|MCAN_WR_UPID):       //0x53://设置电压PID参数
    case (0x80|MCAN_WR_VBUSDATA):   //0x54://设置母线数据 15-1223
    case (0x80|MCAN_WR_FREQDATA):   //0x55://设置频率数据 15-1223
    case (0x80|MCAN_WR_REGMD):      //0x56://设置故障掩码和调节器模式
    case (0x80|MCAN_CALIBSET):      //0x57://参数恢复和保存
    case (0x80|0x40|0x18):          //0x58://设置电流零偏 N/C
    case (0x80|MCAN_WR_TPARA):      //0x59://写时间阈值参数 T1-T8
    case (0x80|MCAN_WR_SPARA):      //0x5A://写报警阈值参数 S1-S8
    case (0x80|MCAN_WR_SYSBIT):     //0x5B://读系统功能位
    case (0x80|MCAN_WR_TPARA_EX):   //0x5C://写扩展时间阈值参数 T9-T16
    case (0x80|MCAN_WR_SPARA_EX):   //0x5D://写扩展报警阈值参数 S9-S16
    case (0x80|MCAN_WR_DUTYDATA):   //0x5E://写占空比数据 15-1223
    case (0x80|0x40|0x1F):          //0x5F://设置版本、日期等 N/C
    case (0x80|MCAN_WR_CALIB):      //0x60://校正参数
    case (0x80|MCAN_WR_ADDR):       //0x70://设置地址
        
        
    //---------- 以下为 读下位机 返回 (从读额定开始序列返回) ----------
    case (0x80|MCAN_RD_IRATE):      //0x10://读额定电流
    case (0x80|MCAN_RD_URATE):      //0x11://读额定电压
    case (0x80|MCAN_RD_IPID):       //0x12://读电流PID
    case (0x80|MCAN_RD_UPID):       //0x13://读电压PID
    case (0x80|MCAN_RD_VBUSDATA):   //0x14://读母线数据 15-1223
    case (0x80|MCAN_RD_FREQDATA):   //0x15://读频率数据 15-1223
    case (0x80|MCAN_RD_REGMD):      //0x16://读故障掩码和调节器模式
        
    case (0x80|MCAN_RD_TPARA):      //0x19://读时间参数 T1-T8
    case (0x80|MCAN_RD_SPARA):      //0x1A://读报警参数 S1-S8
    case (0x80|MCAN_RD_SYSBIT):     //0x1B://读系统功能位
    case (0x80|MCAN_RD_TPARA_EX):   //0x1C://读扩展时间参数 T9-T16
    case (0x80|MCAN_RD_SPARA_EX):   //0x1D://读扩展报警参数 S9-S16
    case (0x80|MCAN_RD_DUTYDATA):   //0x1E://读占空比数据 15-1223
    case (0x80|0x1F):               //N/C
    case (0x80|MCAN_RD_CALIB):      //0x20://读取校正参数命令
        
    case (0x80|CMD_ONLINE_CHK):         // 在线检测，返回版本信息
    case (0x80|CMD_SYSTEM_RST):         // 系统复位
    case (0x80|CMD_ERASE_FLASH):        // 擦除 Flash，每次擦除一个扇区（1K）
    case (0x80|CMD_SET_BAUDRATE):       // 设置波特率
    case (0x80|CMD_UPDATE_PROGPAR):     // 更新程序参数：起始地址，长度（固定1024+2字节CRC）
    case (0x80|CMD_PROGRAM_FLASH):      // Flash 编程（最后一帧才真正开始）
    case (0x80|CMD_EXECUTE_APP):        // 跳转到APP执行    
    
    case (0x80|MCAN_RD_VER):        //0x30://读取版本   
    
        // 发送应答消息到以太网
        if (os_mbx_check (CanNet_mpool) > 0) {
            ptrmsg = (CAN_msg *)_alloc_box (CanNet_mpool);
            if (ptrmsg) {
                ptrmsg->id  = cmd | ((unit + 1)<<8);        // 传统CANID
                ptrmsg->data[0] = S32data[0] & 0xff;
                ptrmsg->data[1] = S32data[0] >>  8;
                ptrmsg->data[2] = S32data[0] >> 16;
                ptrmsg->data[3] = S32data[0] >> 24;
                ptrmsg->data[4] = S32data[1] & 0xff;
                ptrmsg->data[5] = S32data[1] >> 8;
                ptrmsg->data[6] = S32data[1] >> 16;
                ptrmsg->data[7] = S32data[1] >> 24;
                if (os_mbx_send (MBX_MCan2Net, ptrmsg, 2) == OS_R_TMO) {
                    _free_box (CanNet_mpool, ptrmsg);
                } else {    // 释放控制，等待以太网线程接收邮箱
                    ;
                }
            }
        }
        break;

    //---------- 以下为 透明传输命令 返回 (烧写程序用) ----------   16-0802
    case CMD_ONLINE_CHK:            //(0x21)    // 在线检测，返回版本信息
    case CMD_SYSTEM_RST:            //(0x22)    // 系统复位
    case CMD_ERASE_FLASH:           //(0x23)    // 擦除 Flash，每次擦除一个扇区（1K）
    case CMD_SET_BAUDRATE:          //(0x24)    // 设置波特率
    case CMD_UPDATE_PROGPAR:        //(0x25)    // 更新程序参数：起始地址，长度（固定1024+2字节CRC）
    case CMD_PROGRAM_FLASH:         //(0x26)    // Flash 编程（最后一帧才真正开始）
    case CMD_EXECUTE_APP:           //(0x27)    // 跳转到APP执行    
    
        // 发送应答消息到以太网
        if (os_mbx_check (CanNet_mpool) > 0) {
            ptrmsg = (CAN_msg *)_alloc_box (CanNet_mpool);
            if (ptrmsg) {                 
                ptrmsg->id  = cmd | ((unit + 1)<<8);        // 传统CANID
                ptrmsg->data[0] = S32data[0] & 0xff;
                ptrmsg->data[1] = S32data[0] >>  8;
                ptrmsg->data[2] = S32data[0] >> 16;
                ptrmsg->data[3] = S32data[0] >> 24;
                ptrmsg->data[4] = S32data[1] & 0xff;
                ptrmsg->data[5] = S32data[1] >> 8;
                ptrmsg->data[6] = S32data[1] >> 16;
                ptrmsg->data[7] = S32data[1] >> 24;
                if (os_mbx_send (MBX_MCan2Net, ptrmsg, 2) == OS_R_TMO) {
                    _free_box (CanNet_mpool, ptrmsg);
                } else {    // 释放控制，等待以太网线程接收邮箱
                    ;
                }
            }
        }
        break;        
        
    //---------- 返回 扫描数据 ----------
    case (0x80|MCAN_RD_CONNECT):    //0x03: 联机命令 16-0702
        switch (S32data[0] & 0xff)  // 联机应答的实时数据版本格式
        {
            case FIXED_FORMAT:
            case FIX10_FORMAT:
            case FLOAT_FORMAT:
            {
                mConnection[unit] = OK_CONNECT;         // 联机成功
                mDataFormat[unit] = S32data[0] & 0xff;  // 保存数据格式版本
                break;
            }
            default:    // 数据格式版本错误，报错
            {
                mConnection[unit] = NO_CONNECT; // 数据格式版本错，清联机成功
                break;
            }
        }        
        break;
    
    case (0x80|MCAN_RD_REALDATA1):  //0x01: 实时数据1
    case (0x80|MCAN_WR_REALDATA1):  //0x41:
        
#ifdef  EN_PARALLEL     // 允许并联，通道电流电压数据处理 15-1213

        mCanRxData[unit].Cur    = S32data[0];
        mCanRxData[unit].Vol    = S32data[1];
        // 更新通道实时数据(这里按线性并联处理！15-1214)
        if (unit == u_master + pnode_num - 1) {     // 扫描到最后一个子机，并联数据1处理
            f32tmp = 0;
            for (i=u_master; i<(u_master+pnode_num); i++) {
                switch (mDataFormat[i]) // 根据联机版本处理并联电流 16-0708
                {
                    default:
                    case FIXED_FORMAT:  // 默认格式定点（mA,mV）
                    {
                        f32tmp += mCanRxData[i].Cur;
                        break;
                    }
                    case FIX10_FORMAT:  // (0.1mA,0.1mV)  
                    {
                        f32tmp += mCanRxData[i].Cur/10;
                        break;                
                    }
                    case FLOAT_FORMAT:  // 浮点格式
                    {
                        f32tmp += *(float *)&mCanRxData[i].Cur;
                        break;
                    }
                }
            }
            cChan[pchan_no].Set_RealCur(f32tmp);
            switch (mDataFormat[unit])  // 根据联机版本处理并联电压 16-0708
            {
                default:
                case FIXED_FORMAT:  // 默认格式定点（mA,mV）
                {
                    cChan[pchan_no].Set_RealVol(mCanRxData[u_master].Vol);      // 主机保存电压到通道
                    break;
                }
                case FIX10_FORMAT:  // 0.1mA,0.1mV转换到mA,mV单位 
                {
                    cChan[pchan_no].Set_RealVol(mCanRxData[u_master].Vol/10);   // 主机保存电压到通道
                    break;                
                }
                case FLOAT_FORMAT:  // 浮点格式
                {
                    cChan[pchan_no].Set_RealVol(*(float *)&mCanRxData[u_master].Vol);   // 主机保存电压到通道
                    break;
                }  
            }
            cChan[pchan_no].Set_RealPow();
            cChan[pchan_no].Set_RealRes();
        }

#else   // 非并联，通道电流电压接收处理
        // 根据联机结果处理电流电压数据 16-0708
        switch(mDataFormat[unit])
        {
            default:
            case FIXED_FORMAT:  // 默认格式定点（mA,mV）
            {
                cChan[unit].Set_RealCur(S32data[0]);
                cChan[unit].Set_RealVol(S32data[1]);
                break;
            }
            case FIX10_FORMAT:  // (0.1mA,0.1mV)  
            {
                cChan[unit].Set_RealCur(S32data[0]/10);
                cChan[unit].Set_RealVol(S32data[1]/10);
                break;                
            }
            case FLOAT_FORMAT:  // 浮点格式
            {
                cChan[unit].Set_RealCur(*(float *)S32data);
                cChan[unit].Set_RealVol(*(float *)(++S32data));
                break;
            }
        }
        cChan[unit].Set_RealPow();
        cChan[unit].Set_RealRes();

#endif
        break;

    case (0x80|MCAN_RD_REALDATA3):  //0x03: 实时数据3 15-1126
    case (0x80|MCAN_WR_REALDATA3):  //0x43:    
        cChan[unit].Set_InstCur(S32data[0]);
        cChan[unit].Set_InstVol(S32data[1]);
        break;
        
    case (0x80|MCAN_RD_REALDATA2):  //0x02: //实时数据2
    case (0x80|MCAN_WR_REALDATA2):  //0x42:
        
#ifdef  EN_PARALLEL     // 允许并联时实时数据2接收
    
        mCanRxData[unit].Stat   = S32data[0];
        // 更新通道实时数据(这里按线性并联处理！15-1214)
        if (unit == u_master + pnode_num - 1) {         // 扫描到最后一个子机，并联数据2处理
            S32tmp = 0;
            for (i=u_master; i<u_master + pnode_num; i++) { // 故障或操作上传并联通道
                S32tmp |= mCanRxData[i].Stat;
                cChan[pchan_no].Set_Stat(S32tmp);
            }
        }
        if (u_master == unit) {     // 主机保存其他数据到通道 test
            cChan[pchan_no].Set_ExData(S32data[0], S32data[1]);
        }
//      tbc...

#else   // 非并联的实时数据2接收 15-1214调整 
    
//        cChan[unit].Set_Stat(S32data[0]); 16-0414 放在 Set_ExData中处理
        cChan[unit].Set_ExData(S32data[0], S32data[1]);

#endif
        break;

    case (0x80|MCAN_CLRERRBIT): //0x17:清故障返回不送应答到PC
        //不做处理
        break;
    case (0x80|MCAN_RESTART):   //0x18:重启返回不送应答到PC
        //不做处理
        break;
//    case (0x80|0x03):   //0x83: //按键暂停，未启用
//        break;
//    case (0x80|0x04):   //0x84: //按键恢复，未启用
//        break;
//    case (0x80|0x05):   //0x85: //按键启动，未启用
//        break;
    default:

        break;
    }
}
//----------------------------------------------------------------------------------------------------
//  初始化并联信息
//----------------------------------------------------------------------------------------------------
//-------------------------------------------------------
void Init_ParallelChan(void)
{
    cParallelChan.LoadNodeTbl();   //加载通道映射查找表
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

