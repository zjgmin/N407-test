//----------------------------------------------------------------------------------------------------
#ifndef __app_can_Hpp
#define __app_can_Hpp
//----------------------------------------------------------------------------------------------------
#ifdef  IN_CAN_APP
    #define     ex_app_can
#else  
    #define     ex_app_can  extern 
#endif
//----------------------------------------------------------------------------------------------------
#include    "rtx_can.h"
//----------------------------------------------------------------------------------------------------
#define     BROADCAST_ADDR          (0x00)          // 广播地址 15-1203
#define     CONNECT_VERSION         (0x01)          // 联机命令版本 16-0702    
//----------------------------------------------------------------------------------------------------
//老版本测试机CAN通信命令宏定义
//-----------------------------------
//主控制通道CAN命令（下行）
//-------------------------
#define     MCAN_RD_CONNECT         (0x3F)          // 联机命令 16-0702
    
#define     MCAN_RD_REALDATA1       (0x01)          // 读实时数据1
#define     MCAN_WR_REALDATA1       (0x01|(1<<6))   // 写控制数据1，返回实时数据1

#define     MCAN_RD_REALDATA2       (0x02)          // 读实时数据2
#define     MCAN_WR_REALDATA2       (0x02|(1<<6))   // 写控制数据2，返回实时数据2

#define     MCAN_RD_REALDATA3       (0x03)          // 读实时数据3 15-0812
#define     MCAN_WR_REALDATA3       (0x03|(1<<6))   // 写控制数据3，返回实时数据3 15-0812

//#define     MCAN_RD_STEPSYNC1       (0x06)          //同步读实时数据1
//#define     MCAN_WR_STEPSYNC1       (0x06|(1<<6))   //同步写控制数据1，返回实时数据1

//#define     MCAN_RD_STEPSYNC2       (0x07)          //同步读实时数据2
//#define     MCAN_WR_STEPSYNC2       (0x07|(1<<6))   //同步写控制数据2，返回实时数据2

#define     MCAN_RD_IRATE           (0x10)          // 读额定电流
#define     MCAN_WR_IRATE           (0x10|(1<<6))   // 写额定电流

#define     MCAN_RD_URATE           (0x11)          // 读额定电压
#define     MCAN_WR_URATE           (0x11|(1<<6))   // 写额定电压

#define     MCAN_RD_IPID            (0x12)          // 读电流PID
#define     MCAN_WR_IPID            (0x12|(1<<6))   // 写电流PID

#define     MCAN_RD_UPID            (0x13)          // 读电压PID
#define     MCAN_WR_UPID            (0x13|(1<<6))   // 写电压PID

#define     MCAN_RD_VBUSDATA        (0x14)          // 读母线数据 15-1223
#define     MCAN_WR_VBUSDATA        (0x14|(1<<6))   // 写母线数据 15-1223

#define     MCAN_RD_FREQDATA        (0x15)          // 读频率数据 15-1223
#define     MCAN_WR_FREQDATA        (0x15|(1<<6))   // 写频率数据 15-1223

#define     MCAN_RD_REGMD           (0x16)          // 读调节器模式
#define     MCAN_WR_REGMD           (0x16|(1<<6))   // 写调节器模式

#define     MCAN_CLRERRBIT          (0x17)          // 清故障  (2014-0705 新增)
#define     MCAN_CALIBSET           (0x17|(1<<6))   // 校准数据保存恢复处理

#define     MCAN_RESTART            (0x18)          // 重启   (2015-0909 新增)


#define     MCAN_RD_TPARA           (0x19)          // 读时间阈值参数
#define     MCAN_WR_TPARA           (0x19|(1<<6))   // 写时间阈值参数

#define     MCAN_RD_SPARA           (0x1A)          // 读报警阈值参数
#define     MCAN_WR_SPARA           (0x1A|(1<<6))   // 写报警阈值参数

#define     MCAN_RD_SYSBIT          (0x1B)          // 读系统功能位（报警屏蔽位及功能控制开关）
#define     MCAN_WR_SYSBIT          (0x1B|(1<<6))   // 写系统功能位（报警屏蔽位及功能控制开关）

#define     MCAN_RD_TPARA_EX        (0x1C)          // 读扩展时间阈值参数	(2015-0723 新增) 
#define     MCAN_WR_TPARA_EX        (0x1C|(1<<6))   // 写扩展时间阈值参数	(2015-0723 新增)

#define     MCAN_RD_SPARA_EX        (0x1D)          // 读扩展报警阈值参数	(2015-0723 新增)
#define     MCAN_WR_SPARA_EX        (0x1D|(1<<6))   // 写扩展报警阈值参数	(2015-0723 新增)

#define     MCAN_RD_DUTYDATA        (0x1E)          // 读占空比数据 15-1223
#define     MCAN_WR_DUTYDATA        (0x1E|(1<<6))   // 写占空比数据 15-1223

#define     MCAN_RD_CALIB           (0x20)          // 读校准参数
#define     MCAN_WR_CALIB           (0x20|(1<<6))   // 写校准参数

//----- flash 烧写相关 ----- 16-0729 added

#define     CMD_ONLINE_CHK          (0x21)          // 在线检测，返回版本信息
#define     CMD_SYSTEM_RST          (0x22)          // 系统复位
#define     CMD_ERASE_FLASH         (0x23)          // 擦除 Flash，每次擦除一个扇区（1K）
#define     CMD_SET_BAUDRATE        (0x24)          // 设置波特率
#define     CMD_UPDATE_PROGPAR      (0x25)          // 更新程序参数：起始地址，长度（固定1024+2字节CRC）
#define     CMD_PROGRAM_FLASH       (0x26)          // Flash 编程（最后一帧才真正开始）
#define     CMD_EXECUTE_APP         (0x27)          // 跳转到APP执行

//--------------------------------------------------

#define     MCAN_RD_VER             (0x30)          // 读软件版本
#define     MCAN_WR_ADDR            (0x30|(1<<6))   // 写CAN地址

#define     MCAN_PARAL_SYNC         (0x31|(1<<6))   // 并联同步

//新添加命令
#define     MCAN_STEP_SYNC          (0x32)          // 阶段同步命令

//-------------------------
//辅助通道CAN命令
//-------------------------
#define     SCAN_RD_REALDATA1       (0x01)          //读实时数据1
#define     SCAN_WR_REALDATA1       (0x01|(1<<6))   //写控制数据1，返回实时数据1

#define     SCAN_RD_REALDATA2       (0x02)          //读实时数据2
#define     SCAN_WR_REALDATA2       (0x02|(1<<6))   //写控制数据2，返回实时数据2

#define     SCAN_RD_REALDATA3       (0x03)          //读实时数据3
#define     SCAN_WR_REALDATA3       (0x03|(1<<6))   //写控制数据3，返回实时数据3

#define     SCAN_RD_RATE            (0x10)          //读额定
#define     SCAN_WR_RATE            (0x10|(1<<6))   //写额定

#define     SCAN_RD_CALIB           (0x20)          //读校准参数
#define     SCAN_WR_CALIB           (0x20|(1<<6))   //写校准参数

#define     SCAN_RD_EDATA           (0x30)          //读扩展参数（地址，版本，AD控制）
#define     SCAN_WR_EDATA           (0x30|(1<<6))   //写扩展参数（地址，版本，AD控制）

#define     SCAN_RD_MAXI            (0x31)          //读最大单体(电压，温度等） 15-1116
#define     SCAN_WR_LOCK            (0x31|(1<<6))   //单独控制(锁定通道)
////-------------------------
////    扩展辅助通道CAN命令 @哈尔滨光宇 15-1116
////-------------------------
//#define       EX_SCAN_RD_MAXVI    (0x18360103)    //读最大单体电压
//#define       EX_SCAN_RD_MINVI    (0x18380103)    //读最小单体电压
//#define       EX_SCAN_RD_MAXTI    (0x183A0103)    //读最大单体温度
//#define       EX_SCAN_RD_MINTI    (0x183C0103)    //读最小单体温度
//----------------------------------------------------------------------------------------------------
//  下位机联机状态 mConnection（见联机命令） 16-0702
//----------------------------------------------------------------------------------------------------
#define     NO_CONNECT          (0x00)      // 脱机
#define     OK_CONNECT          (0x01)      // 联机成功
#define     OFF_CONNECT         (0xff)      // 联机超时
//----------------------------------------------------------------------------------------------------
//  下位机实时数据格式版本宏 mDataFormat（见联机命令） 16-0702
//----------------------------------------------------------------------------------------------------
#define     FIXED_FORMAT        (0x00)      // 定点格式 整型 单位：mV、mA、mW、mΩ
#define     FIX10_FORMAT        (0x01)      // 10倍格式 整型 单位：0.1(mV、mA、mW、mΩ)
#define     FLOAT_FORMAT        (0x02)      // 浮点格式 整型 单位：mV、mA、mW、mΩ
//----------------------------------------------------------------------------------------------------
//CAN 波特率
enum 
{
    BAUD_1M     = 1000000,
    BAUD_500K   = 500000,
    BAUD_250K   = 250000,
    BAUD_125K   = 125000,
    BAUD_100K   = 100000,   // 15-1111
};
//-------------------------
//CAN 通信对象
enum 
{
    CAN_NO1     = 0, 
    CAN_NO2     = 1
};
//-------------------------
//CAN 通信对象
enum 
{
    MAINCHAN    = 0, 
    SUBCHAN     = 1
};
//----------------------------------------------------------------------------------------------------
//ChanObj   //CAN发送目标硬件
#define     OBJ_MAINCHAN    0       // 主通道
#define     OBJ_VICHAN      0       // 单体电压
#define     OBJ_TICHAN      1       // 单体温度
#define     OBJ_IICHAN      2       // 开关量
#define     OBJ_OICHAN      3       // 开关量
#define     OBJ_XICHAN      4       // 其他
//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
//----------------------------------------------------------------------------------------------------
class CANCtrl
{
private:
    U8      PhyNo;          // 物理CAN控制器号
    U8      mChanInx;       // CAN数据收发目标通道索引（地址须+1）
    U8      mChanObj;       // CAN数据收发目标硬件(单体电压板，温度板，IO板)
//    U8      bCommErr;       // 通信故障标志
    U8      mWorkInfo;      // 工作对象（0：控制单元 1：采集单元）
    U8      mCanFrameNo;    // CAN数据帧标记
    U8      mFrameIndex;
    U16     mBootHoldCnt;   // CAN下位机BOOTLOARDER 保持
//    U16     mCommTout;      // 通信超时计数器	
    U32     mBaudRate;      // 波特率
    CAN_msg     mTxBuf;     // 发送缓冲
    
public:
    CANCtrl();

    U8      Get_WorkInfo(void);     // 获取CAN工作信息（主机或这采集单元）
    void    Recv_In(CAN_msg *RxBuf);
    U32     Send_Out(void);	
    void    Recv_Process(CAN_msg *TxBuf);
    void    Send_Process(void);
    void    Hard_Init(void);
    U32     Comm_Test(S16 tick);
};

ex_app_can  CANCtrl     cCAN[CAN_CTRL_MAX_NUM];
//----------------------------------------------------------------------------------------------------
#endif  // #ifdef   __cplusplus
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus     // 可供C调用
    extern "C" 
    {
#endif
//----------------------------------------------------------------------------------------------------
//ex_app_can    U8          bHaveCmd[2];        // 有插入命令
//ex_app_can    U8          FailedNum[2];       // 插入命令无应答计数
//ex_app_can    CAN_msg     TxMsg[2];           // CAN发送信息缓冲

//----------------------------------------------------------------------------------------------------
ex_app_can  void    CAN_Init(U8 mask);
ex_app_can  U32     CanNet_CmdSend(U32 canid, U32 *pmbox, U32 timeout);
ex_app_can  U32     CanNet_MsgSend(CAN_msg msg, U32 *pmbox, U32 timeout);
ex_app_can  void    CAN_Recv_Process(U32 can_no, CAN_msg *RxBuf);
ex_app_can  void    CAN_Send_Process(U32 can_no);
ex_app_can  U8      Get_CANWorkInfo(U8 can_no);

//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
    }
#endif
//----------------------------------------------------------------------------------------------------
#endif /* __app_can_Hpp */
