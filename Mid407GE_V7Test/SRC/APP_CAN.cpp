//----------------------------------------------------------------------------------------------------
// Header:      CAN通信
// File Name:   APP_CAN.cpp
// Author:      David
// Date:        2012-10-18
//----------------------------------------------------------------------------------------------------
#define     IN_CAN_APP
extern "C" 
{
    #include    "stm32f4xx.h" 
    #include    "rtl.h"
    #include    "main.h"
    #include    "gpio.h"
    #include    "rtx_can.h"
    #include    "led.h"
    #include    "sys_config.h"
    #include    "app_config.h"
}
#include    "Dev.hpp"
#include    "chan.hpp"
#include    "parallelchan.hpp"
#include    "subchan.hpp"
#include    "app_can.hpp"
#include    "ethercmd.hpp"  // 16-0122
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
//构造函数
//----------------------------------------------------------------------------------------------------
CANCtrl::CANCtrl()
{
    static  U8  can_no = 0;

    switch(can_no)
    {
    case 0:
    //  bCommErr    = FALSE;        // 去通信故障(接收超时)
    //  mCommTout   = 0;            // 通信超时计数器
        PhyNo       = can_no + 1;   // 控制器号
        mChanInx    = 0;            // CAN数据目标通道索引（地址须+1）
        mChanObj    = OBJ_MAINCHAN; // CAN发送目标硬件
        mBaudRate   = BAUD_500K;    //BAUD_125K;    // 默认500KHz波特率 16-0304 125K波特率测试
        mWorkInfo   = MAINCHAN;     // 默认控制通道
        break;

    case 1:
    //  bCommErr    = FALSE;        // 去通信故障(接收超时)
    //  mCommTout   = 0;            // 通信超时计数器
        PhyNo       = can_no + 1;   // 控制器号
        mChanInx    = 0;            // CAN数据目标通道
        mChanObj    = OBJ_VICHAN;   // CAN发送目标硬件
        mBaudRate   = BAUD_125K;    // 默认125KHz波特率
        mWorkInfo   = SUBCHAN;      // 默认辅助通道
        break;

    default:break;
    }
    mBootHoldCnt = 0;
    can_no++;
}
//----------------------------------------------------------------------------------------------------
//  获取CAN工作信息（主机或这采集单元）
//----------------------------------------------------------------------------------------------------
U8  Get_CANWorkInfo(U8 can_no)
{
    return  (cCAN[can_no].Get_WorkInfo());
}
//----------------------------------------------------------------------------------------------------
//  获取CAN工作信息（主机或这采集单元）
//----------------------------------------------------------------------------------------------------
U8  CANCtrl::Get_WorkInfo(void)
{
    return  (mWorkInfo);
}
//----------------------------------------------------------------------------------------------------
//  来自下位机的CAN数据解析
//----------------------------------------------------------------------------------------------------
void CANCtrl::Recv_In(CAN_msg *RxBuf)
{
    U8  can_adr, can_cmd;

    if (mWorkInfo == MAINCHAN) {

        can_adr = (RxBuf->id >> 8) & 0xff;
        can_cmd = (RxBuf->id >> 0) & 0xff;

        if ((can_adr != 0)
        &&  (can_adr <= cParallelChan.mNodeTbl.node_num)) { // 接收节点数由并联给出 16-0128 
            cParallelChan.CanRxProc((can_adr-1), can_cmd, (S32 *)&RxBuf->data[0]);
        } else {
            can_adr = can_adr;
        }
    } else {    // 辅助通道扫描
        can_adr = (RxBuf->id >> 8) & 0xff;
        can_cmd = (RxBuf->id >> 0) & 0xff;
        //单体电压
        if ((can_adr > VIADDR_OFFSET)
        &&  (can_adr <= VIADDR_OFFSET+MAX_VI_BOARD)) {

        } 
        //单体温度
        else if ((can_adr > TIADDR_OFFSET)
        && (can_adr <= TIADDR_OFFSET+MAX_TI_BOARD)) {

        }
        //开关量输入
        else if ((can_adr > IIADDR_OFFSET)
        && (can_adr <= IIADDR_OFFSET+MAX_II_BOARD)) {

        }
        //开关量输出
        else if ((can_adr > OIADDR_OFFSET)
        && (can_adr <= OIADDR_OFFSET+MAX_OI_BOARD)) {
            
        }


        //错误
        else {
            ;
        }
    }
}
//----------------------------------------------------------------------------------------------------
//  CAN数据下发到下位机
//----------------------------------------------------------------------------------------------------
U32 CANCtrl::Send_Out(void)
{
    CAN_msg *ptrmsg;
//===================================================
//有来自主通道控制或上位机的消息则发送消息，并等待应答
//===================================================
    if (mBootHoldCnt>0) {
        mBootHoldCnt--;
    }
    if (mWorkInfo == MAINCHAN) {
        if (os_mbx_wait (MBX_Net2MCan, (void **)&ptrmsg, 0) == OS_R_OK) 
        {        
            //取出CAN命令直接下发
            mTxBuf  = *ptrmsg;         
            
#ifdef  USE_SIM_CANCMD_ACK  // 模拟应答CAN命令
            
            U8  cmd = (mTxBuf.id>>0) & 0xff;
            
            switch(cmd)
            {
                case (CMD_ONLINE_CHK):        // 在线检测，返回版本信息
                {
                    mBootHoldCnt = 200; //保持
                }
                case (CMD_SYSTEM_RST):        // 系统复位
                {
                    mBootHoldCnt = 200; //保持
                }
                case (CMD_ERASE_FLASH):       // 擦除 Flash，每次擦除一个扇区（1K）
                {
                    mBootHoldCnt = 200; //保持
                }
                case (CMD_SET_BAUDRATE):      // 设置波特率
                {
                    mBootHoldCnt = 200; //保持
                }
                case (CMD_UPDATE_PROGPAR):    // 更新程序参数：起始地址，长度（固定1024+2字节CRC）
                {
                    mBootHoldCnt = 200; //保持
                }
                case (CMD_PROGRAM_FLASH):     // Flash 编程（最后一帧才真正开始）
                {
                    mBootHoldCnt = 200; //保持
                }
                case (CMD_EXECUTE_APP):       // 跳转到APP执行
                {   
                    mBootHoldCnt = 200; //保持
                    // 发送应答消息到以太网
                    if (os_mbx_send (MBX_MCan2Net, ptrmsg, 2) == OS_R_TMO) {
                        _free_box (CanNet_mpool, ptrmsg);
                    } else {    // 释放控制，等待以太网线程接收邮箱
                        ;
                    }                          
                    return 0;
                }
                
                default:
                {
                    //释放存储池
                    _free_box (CanNet_mpool, ptrmsg); 
                    break;
                }
            }  
#else       //  发送下位机，特殊命令抢占CAN口
            
            U8  cmd = (mTxBuf.id>>0) & 0xff;
            
            switch(cmd)
            {
                case (CMD_ONLINE_CHK):        // 在线检测，返回版本信息
                case (CMD_SYSTEM_RST):        // 系统复位
                case (CMD_ERASE_FLASH):       // 擦除 Flash，每次擦除一个扇区（1K）
                case (CMD_SET_BAUDRATE):      // 设置波特率
                case (CMD_UPDATE_PROGPAR):    // 更新程序参数：起始地址，长度（固定1024+2字节CRC）
                case (CMD_PROGRAM_FLASH):     // Flash 编程（最后一帧才真正开始）
                case (CMD_EXECUTE_APP):       // 跳转到APP执行
                {   
                    mBootHoldCnt = 200; //保持
                    break;
                }                
                default:
                { 
                    break;
                }
            }
            //释放存储池
            _free_box (CanNet_mpool, ptrmsg);         
#endif            
        } 
        //没有，则定时扫描主通道
        else    // 15-0621 调整发送次序
        {
            if (mBootHoldCnt) { // 直接返回，放弃发送轮询数据
                return 0;
            }
        //--------------------------------------------------     
        #if (defined(EN_CONNECT_CMD) && !defined(EN_SIM_CHAN))  // 使用联机命令 16-0719 16-0902修改
        //-------------------------------------------------- 
            // 加入联机处理 16-0713
            if (cParallelChan.GetConnectStat(mChanInx) == 0) {  // 尚未联机，发送联机命令（传统）
                mCanFrameNo = 0;
                cParallelChan.CanTxProc(mChanInx, mCanFrameNo, mFrameIndex, &mTxBuf); // 载入联机数据
                if (mChanInx < (cParallelChan.mNodeTbl.node_num - 1)) {     // 联机处理后更新扫描节点
                    mChanInx++;
                } else {
                    mChanInx = 0;
                }
            } else {    // 16-0803 修改
                mCanFrameNo++;                
                if (mCanFrameNo <= 2) {  // (1)发送：实时控制电流、电压 (2)发送：阶段、模式等
                    cParallelChan.CanTxProc(mChanInx, mCanFrameNo, mFrameIndex, &mTxBuf);
                    if (mCanFrameNo == 2) { // 第二帧发送完后切换通道
                        mCanFrameNo = 0;
                        if (++mFrameIndex > 2) {
                            mFrameIndex = 1;
                        }
                        if (mChanInx < (cParallelChan.mNodeTbl.node_num - 1)) { // 扫描通道数由并联给出 16-0128
                            mChanInx++;
                        } else {
                            mChanInx = 0;
                        }
                    }
                } else {                // 超界，实际跑不到
                    mCanFrameNo = 2;
                    if (++mFrameIndex > 2) {
                        mFrameIndex = 1;
                    }
                    cParallelChan.CanTxProc(mChanInx, mCanFrameNo, mFrameIndex, &mTxBuf);
                }
            }
        //--------------------------------------------------    
        #else   // 不支持联机的实时扫描命令处理
        //--------------------------------------------------     
            mCanFrameNo++;
            if (mCanFrameNo > 2) {  // 切换通道，发送：实时控制电流，电压
                mCanFrameNo = 1;
                if (mChanInx < (cParallelChan.mNodeTbl.node_num - 1)) { // 扫描通道数由并联给出 16-0128
                    mChanInx++;
                } else {
                    mChanInx = 0;
                }
            } else {                // 发送：阶段，模式等
                mCanFrameNo = 2;
                if (++mFrameIndex > 2) {
                    mFrameIndex = 1;
                }
            }
            // 载入控制通道数据
            cParallelChan.CanTxProc(mChanInx, mCanFrameNo, mFrameIndex, &mTxBuf);
        //-------------------------------------------------- 
        #endif
        //--------------------------------------------------     

        }
    } 

//===================================================
//有来自辅助通道控制或上位机的消息则发送消息，并等待应答
//===================================================

    else {
        if (os_mbx_wait (MBX_Net2SCan, (void **)&ptrmsg, 0) == OS_R_OK) 
        {
            //取出CAN命令直接下发
            mTxBuf  = *ptrmsg;
            //释放存储池
            _free_box (CanNet_mpool, ptrmsg);
            //有必须等待的应答则等待
            
        }
        //没有，则定时扫描辅助通道
        else
        {
 
              return 0;  

        }
    }

    return (CAN_send(PhyNo, &mTxBuf, 1));
}
//----------------------------------------------------------------------------------------------------
// CANx 初始化
//----------------------------------------------------------------------------------------------------
void CANCtrl::Hard_Init(void)
{
    CAN_init (PhyNo, mBaudRate);

#ifdef  EN_CANRX_FILTER     // CAN接收滤波
    if (mWorkInfo == MAINCHAN) {    // 主控CAN
        CAN_hw_rx_object (PhyNo, 0, EXID_CODE, DATA_TYPE | EXTENDED_TYPE);
    } else {                        // 辅助CAN
        CAN_hw_rx_object (PhyNo, 0, 0, DATA_TYPE | EXTENDED_TYPE);
    }
#else
    CAN_hw_rx_object (PhyNo, 0, 0, DATA_TYPE | EXTENDED_TYPE);
#endif

    CAN_start (PhyNo);
}
//----------------------------------------------------------------------------------------------------
/*  以太网与CAN命令交互 16-0122 added

*/
//----------------------------------------------------------------------------------------------------
//  无数据部分的CAN消息发送
//--------------------------------------------------
U32 CanNet_CmdSend(U32 canid, U32 *pmbox, U32 timeout)
{
    U32 err = 0;
    CAN_msg s_tmsg,
            *tmsg;

    s_tmsg.id       = canid;
    s_tmsg.len      = 8;
    s_tmsg.ch       = 0;
    s_tmsg.format   = EXTENDED_FORMAT;
    s_tmsg.type     = DATA_FRAME;
    s_tmsg.data[0]  = 0;
    s_tmsg.data[1]  = 0;
    s_tmsg.data[2]  = 0;
    s_tmsg.data[3]  = 0;
    s_tmsg.data[4]  = 0;
    s_tmsg.data[5]  = 0;
    s_tmsg.data[6]  = 0;
    s_tmsg.data[7]  = 0;

    if (os_mbx_check (CanNet_mpool) > 0) {
        tmsg = (CAN_msg *)_alloc_box (CanNet_mpool);
        if (tmsg) {
            *tmsg = s_tmsg;
            if (os_mbx_send (pmbox, tmsg, timeout) == OS_R_TMO) {  // 若发送消息超时，释放存储池
                _free_box (CanNet_mpool, tmsg);
                err = RET_SEND_TOUT;
            }
        } else {
            err = RET_SYS_ERR;
        }
    } else {
        err = RET_SYS_ERR;
    }     
    
    return  (err);
}
//----------------------------------------------------------------------------------------------------
//  带数据的CAN消息发送
//--------------------------------------------------
U32 CanNet_MsgSend(CAN_msg msg, U32 *pmbox, U32 timeout)
{
    U32 err = 0;
    CAN_msg *tmsg;

    if (os_mbx_check (CanNet_mpool) > 0) {
        tmsg = (CAN_msg *)_alloc_box (CanNet_mpool);
        if (tmsg) {
            *tmsg = msg;
            if (os_mbx_send (pmbox, tmsg, timeout) == OS_R_TMO) {  // 若发送消息超时，释放存储池
                _free_box (CanNet_mpool, tmsg);
                err = RET_SEND_TOUT;
            }
        } else {
            err = RET_SYS_ERR;
        }
    } else {
        err = RET_SYS_ERR;
    }     
    
    return  (err);
}
//----------------------------------------------------------------------------------------------------
//  CANx 硬件初始化
//----------------------------------------------------------------------------------------------------
void CAN_Init(U8 mask)
{
    U32 i;

    for (i=0; i<CAN_CTRL_MAX_NUM; i++) {
        if (mask & B8_MASK_N[i]) {
            cCAN[i].Hard_Init();
        }
    }
}
//----------------------------------------------------------------------------------------------------
//  CANx    接收处理程序
/*
    参数    can_no:   CAN控制器号 0，1，... ,CAN_CTRL_MAX_NUM-1
            *RxBuf:   CAN待处理消息缓冲
    返回    无
*/
//----------------------------------------------------------------------------------------------------
void CAN_Recv_Process(U32 can_no, CAN_msg *RxBuf)
{
    if (can_no >= CAN_CTRL_MAX_NUM) return; //控制器号出错 
    cCAN[can_no].Recv_In(RxBuf);
}
//----------------------------------------------------------------------------------------------------
//  CANx    发送处理程序
/*
    参数  can_no: CAN控制器号 0，1，... ,CAN_CTRL_MAX_NUM-1
    返回  无
*/
//----------------------------------------------------------------------------------------------------
void CAN_Send_Process(U32 can_no)
{
    if (can_no >= CAN_CTRL_MAX_NUM) return; //控制器号出错
    cCAN[can_no].Send_Out();
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//eof
