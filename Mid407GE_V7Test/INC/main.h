//----------------------------------------------------------------------------------------------------
#ifndef __main_H
#define __main_H
//----------------------------------------------------------------------------------------------------
#ifdef     IN_MAIN
    #define     ex_main
#else  
    #define     ex_main     extern
#endif
//====================================================================================================
//  中位机类型定义
#define     MAX_CHAN_NUM        (48)        // 中位机支持通道数
//----------------------------------------------------------------------------------------------------
//  模拟通道测试宏 及 测试版本宏
#define     EN_SIM_CHAN         // CAN脱机时模拟产生CAN数据
//----------------------------------------------------------------------------------------------------
//  设备结构布局宏（参考联机命令）
//-------------------------------------------------------
#define     DS_GENERAL          (0)         // 通用类型设备结构
#define     DS_NEILSBED         (1)         // 针床类型设备结构
#define     DS_POURING          (2)         // 倒出类型设备结构
#define     DS_OPENSHELF        (3)         // 开架类型设备结构
    
//--------------------------------------------------------
//  设备结构
#define     DEV_STR_TYPE        (DS_GENERAL)    // 设备结构类型                                                    
//----------------------------------------------------------------------------------------------------
//  中位机硬件版本（参见设备信息） 16-0422
#define     HARD_VERSION        (DEV_STR_TYPE*1000 + MAX_CHAN_NUM)  // 设备结构+回路数
//----------------------------------------------------------------------------------------------------
//  中位机软件版本（参见设备信息） 16-0422
//  高两位[结构信息+测试信息](范围:0~64)--> 0~49 正式版本(设备结构);50~63 测试版本(50+设备结构[暂时0~9]);64为模拟通道测试
//  低两位[具体软件代码版本] (范围:0~999)
//----------------------------------------------------------------------------------------------------
#define     MAIN_VERSION        (DEV_STR_TYPE)      // 主版本 协议修改升级    （范围：1~64）0:通用，1：针床, 2：开架，......
#define     SUB_VERSION         (28)                // 次版本 编译发布升级     48路倒出式（南都JFK1 100A/350V）
#define     PUBLISH_DATE        (20160924)          // 生产日期
//----------------------------------------------------------------------------------------------------
#if     defined(EN_SIM_CHAN)
    #define     SOFT_VERSION    (64000 + SUB_VERSION)                       // 模拟CAN版本
#else
    #define     SOFT_VERSION    (MAIN_VERSION*1000 + SUB_VERSION)           // 正式版本（去掉 EN_SIM_CHAN & TEST_VERSION 宏）
#endif
//====================================================================================================

    #define     TICK_IMG        (5)                 // OS节拍（mS）
    #define     DEF_REC_NUM     (600)               // 定义记录数（条） Record[DEF_REC_NUM] 15-0617
    #define     MAX_WKLINE      (250)               // 最大工步行数（步）
    #define     CTRL_PERIOD     (50)                // 控制节拍（mS）
    #define     CAN1_PERIOD     (5)                 // CAN1 调度节拍（mS）
    #define     CAN2_PERIOD     (5)                 // CAN2 调度节拍（mS）
    #define     ENET_PERIOD     (10)                // ENET 网络节拍（mS）

//-------------------------------------------------------
#define     SUB_PRG_NUM         (4)                 // 辅助工艺行数（当前最大1+3个ELSE） 16-0507

//====================================================================================================
//  Cabinet 控制宏，与通道解耦 16-0514
//-------------------------------------------------------
//  结构相关全局宏定义
//------------------------------
#define     CABCTRL_PERIOD      (50)                            // 柜控制节拍（mS）
#define     SCAN_PERIOD         (MAX_CHAN_NUM * CAN1_PERIOD)    // 所有通道扫描周期（mS）
        
//====================================================================================================
//====================================================================================================
// 并联相关宏定义
//#define     EN_PARALLEL                     // 允许并联
//-------------------------------------------------------
#define     MAX_NODE_NUM            (192)   // 最大节点数（下位机主控单元）15-1213
#define     MAX_EXT_CHAN_NUM        (8)     // 最大扩展通道数（逆变主电源等）
//--------------------------------------------------
#ifdef  EN_PARALLEL

    #define     DEF_PNODE_NUM       (1)     // 默认并联节点数 16-0129
    #if ((MAX_CHAN_NUM * DEF_PNODE_NUM) > MAX_NODE_NUM)
        #error  "并联后所有节点数大于最大节点数！"
    #endif
    
#endif
//====================================================================================================
//    测试或配置用宏定义
//----------------------------------------------------------------------------------------------------
#define     USE_SIM_CANCMD_ACK      // 模拟应答CAN命令（烧写测试用）16-1008
//#define     EN_CONNECT_CMD          // 使用联机命令
#define     USE_SUBPRG              // 运行阶段辅助工艺备份 16-0507
#define     EN_CANTX_INT            // 使能CAN发送中断
#define     EN_CANRX_FILTER         // 使能CAN接收滤波
//#define     EN_CAN_BROADCAST        // 使能CAN命令广播（默认屏蔽） 16-0113 
//---------------------------------------------
#define     EN_VALVE_NC             // 使能气缸信号常闭(没有下压时闭合) 15-0616
//---------------------------------------------
//====================================================================================================
//  CAN接收滤波ID中的CAN地址滤波掩码
//-----------------------------------

#define     EXID_CODE       (unsigned int)(0x00003f80)
#define     EXID_MASK       (unsigned int)(0xffffc080)

//====================================================================================================
//----------任务调度节拍----------
        
#define     CTRL_TICK        (CTRL_PERIOD / TICK_IMG)        // 控制节拍
#define     CAN1_TICK        (CAN1_PERIOD / TICK_IMG)        // CAN1 调度节拍
#define     CAN2_TICK        (CAN2_PERIOD / TICK_IMG)        // CAN2 调度节拍
#define     ENET_TICK        (ENET_PERIOD / TICK_IMG)        // 网络节拍
#define     CABCTRL_TICK     (CABCTRL_PERIOD / TICK_IMG)     // 柜体控制任务节拍

//----------控制线程计数----------

#define     CTRL_1MIN        (60000 / CTRL_PERIOD)           // 1分   控制计数
#define     CTRL_30S         (30000 / CTRL_PERIOD)           // 30秒  控制计数
#define     CTRL_10S         (10000 / CTRL_PERIOD)           // 10秒  控制计数
#define     CTRL_5S          (5000  / CTRL_PERIOD)           // 5秒   控制计数
#define     CTRL_3S          (3000  / CTRL_PERIOD)           // 3秒   控制计数
#define     CTRL_2S          (2000  / CTRL_PERIOD)           // 2秒   控制计数
#define     CTRL_1S          (1000  / CTRL_PERIOD)           // 1秒   控制计数
#define     CTRL_05S         (500   / CTRL_PERIOD)           // 0.5秒 控制计数
#define     CTRL_02S         (200   / CTRL_PERIOD)           // 0.2秒 控制计数
#define     CTRL_01S         (100   / CTRL_PERIOD)           // 0.1秒 控制计数

//----------下位机异常判断次数----------

#define     LOWERR_CNT       (SCAN_PERIOD / CTRL_PERIOD + 2)     //保证扫描一周

//====================================================================================================
//----------PC从机通信相关----------
#define     MAX_SLAVE_PC     (4)             // 最多挂4台作为从机的PC
#define     MASTER_PC        (0x01)          // 主机
#define     SLAVE_PC         (0x02)          // 从机
#define     SLAVE_TOUT       (CTRL_10S)      // 收不到从机信息 10秒超时

#if         MAX_SLAVE_PC > 8
    #error  "作为从机的数量太多了!!"
#endif
//------------------------------------------------------------
#define     OFFLINE         (1)     // CAN离线
#define     ONLINE          (0)     // CAN在线
//----------------------------------------------------------------------------------------------------
//#define       USE_2MRAMs              // 使用2片磁电RAM
#define     CHIP1           (0)     // U4 of PCB
#define     CHIP2           (1)     // U5 of PCB

//====================================================================================================
/*
    Macro Name     : assert
    Description    : The assert macro is used for function's parameters check.
                     It is used only if the library is compiled in DEBUG mode. 
    Input          : - expr: If expr is false, it calls assert_failed function
                     which reports the name of the source file and the source
                     line number of the call that failed. 
                     If expr is true, it returns no value.
    Return         : None
*/ 
/* Exported macro ------------------------------------------------------------*/
#undef         assert
#ifdef         ASSERT_DEBUG

    #define     assert(expr) ((expr) ? (void)0 : assert_failed((u8 *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
    void         assert_failed(u8* file, u32 line);
#else
    #define     assert(expr) ((void)0)
#endif /* DEBUG */
//====================================================================================================
//----------------------------------------------------------------------------------------------------
#define     TRUE                (1)
#define     FALSE               (0)
//----------------------------------------------------------------------------------------------------
#define     MAX_UDP_PACK_LEN    (1472)
//----------------------------------------------------------------------------------------------------
//  全局跳线功能定义 gFunSelBit
//--------------------------------------------------
#define     FUNC_IPADDRRST      (0x0100)      // IP地址复位 16-0602   
#define     FUNC_BOARDCAST      (0x0080)      // CAN广播功能 16-0113
//#define
//----------------------------------------------------------------------------------------------------
#ifdef    __cplusplus
    extern "C" 
    {
#endif
//----------------------------------------------------------------------------------------------------    
ex_main     unsigned int    gEtherTout;     // 网络超时计数器
ex_main     unsigned int    gFunSelBit;     // 全局跳线功能位（上电读取）
ex_main     unsigned int    gSimCanBit;     // 模拟CAN功能位（上电读取）
        
//ex_main     unsigned char   gTestTbl[MAX_CHAN_NUM];     // 全局测试组
        
#ifdef  EN_SRAM_TEST
  ex_main   unsigned int    gSramErr;       // SRAM错误标记    
#endif        
//----------------------------------------------------------------------------------------------------

ex_main     void            Delay (unsigned int);
ex_main     unsigned short  Crc16 (unsigned char *, unsigned short);
ex_main     void            OsError_Display (void);
ex_main     void            HardFault_Display (void);

//----------------------------------------------------------------------------------------------------
#ifdef    __cplusplus
    }
#endif
//----------------------------------------------------------------------------------------------------
#endif /* __main_H */
