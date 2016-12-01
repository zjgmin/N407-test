//----------------------------------------------------------------------------------------------------
#ifndef __DEV_HPP
    #define __DEV_HPP

#include    "RTX_CAN.h"
#include    "main.h"
//----------------------------------------------------------------------------------------------------
//C++类定义
//-------------------------------------------------------
#ifdef    _IN_DEV_CPP
    #define   dev_ext
#else
    #define   dev_ext extern
#endif
//----------------------------------------------------------------------------------------------------
#define   DEVINFO_MARK      (0x00020000|(MAX_CHAN_NUM))     // 设备信息标记字 2016-0125

//#define   CTRLCFG_MARK      (0x00040000|(MAX_CHAN_NUM))     // 设备控制配置标记字 2016-0125
#define   CTRLCFG_MARK      (0x00050000|DEV_STR_TYPE)       // 设备控制配置标记字 2016-0607 与结构相关
    
#define   SUBCHCFG_MARK     (0x00010001)                    // 辅助通道硬件配置标记字

#define   VIHDCFG_MARK      (0x00020000|DEV_STR_TYPE)       // 单体电压硬件配置标记字(与硬件结构相关) 16-0125
#define   TIHDCFG_MARK      (0x00020000|DEV_STR_TYPE)       // 单体温度硬件配置标记字(与硬件结构相关) 16-0125
#define   IIHDCFG_MARK      (0x00020000|DEV_STR_TYPE)       // 开关量入硬件配置标记字(与硬件结构相关) 16-0125
#define   OIHDCFG_MARK      (0x00020000|DEV_STR_TYPE)       // 开关量出硬件配置标记字(与硬件结构相关) 16-0125
#define   XIHDCFG_MARK      (0x00020000|DEV_STR_TYPE)       // 其他辅助硬件配置标记字(与硬件结构相关) 16-0125

#define   SAMPCFG_MARK      (0x00010002)                    // 辅助通道配置表标记字

#define   TEMPTBL_MARK      (0x00010002)                    // 温度配置表标记字

#define   NETINFO_MARK      (0x00010002)                    // 网络信息表标记字

#define   HOSTINF_FLAG      (0x0001)                        // 主机信息表标记字

#define   NODETBL_MARK      (0x00010000)                    // 通道映射查找表标记字 16-0129

//----------------------------------------------------------------------------------------------------

#define   NULL32_VAL        (0xffffffff)      // 参数设定无效的定义值
                                              // 1 - 全局保护值
                                              // 2 - 某些设备参数（如风机开启温度）

//----------------------------------------------------------------------------------------------------
//各种辅助参数通道特征码，用于命令区分
#define     VI_ID       0x01    // 电压单体辅助通道特征码
#define     TI_ID       0x02    // 温度单体辅助通道特征码
#define     II_ID       0x03    // 开关量入辅助通道特征码
#define     OI_ID       0x04    // 开关量出辅助通道特征码
#define     XI_ID       0x05    // 其他辅助通道特征码

#define     EBI_ID      0x81    // 扩展位输入辅助通道特征码
#define     EBO_ID      0x82    // 扩展位输出辅助通道特征码
#define     EDI_ID      0x83    // 扩展数据输入辅助通道特征码
#define     EDO_ID      0x84    // 扩展数据输出辅助通道特征码
//----------------------------------------------------------------------------------------------------

//  全局故障标记 GblFault
#define   GBL_SRAM_FAULT    (1<<0)          // SRAM故障

//====================================================================================================
//  中位机控制配置参数相关宏定义
//====================================================================================================

#define   PDRESUME_DEF    (1)     // 掉电恢复与否 默认：1 - 允许
#define   STEP_SYNC_DEF   (1)     // 阶段同步与否 默认：1 - 允许
#define   PARALLEL_DEF    (0)     // 允许并联与否 默认：0 - 禁止
#define   ABNRESUME_DEF   (0)     // 故障恢复与否 默认：0 - 禁止
//----------
#define   MIDEV_FLAG      (0xff)  // 对中位机操作的标记
//----------------------------------------------------------------------------------------------------
//  基本辅助通道      14-0724 modified
//----------
#define   MAX_SUBCHAN_BOARD   (240)

#define   MAX_VI_BOARD        (40)    // 最大电压单元板数
#define   MAX_BOARDVI_NUM     (32)    // 单板最大单体数 total 1280 points
#define   MAX_CHANVI_NUM      (240)   // 通道最大配置数

#define   MAX_TI_BOARD        (40)    // 最大温度通道板数
#define   MAX_BOARDTI_NUM     (32)    // 单板最大单体数 total 1280 points
#define   MAX_CHANTI_NUM      (120)   // 通道最大配置数

#define   MAX_II_BOARD        (20)    // 最大开关量入板数
#define   MAX_BOARDII_NUM     (64)    // 单板最大开关量入 total 1280 points
#define   MAX_CHANII_NUM      (40)    // 通道最大配置数

#define   MAX_OI_BOARD        (20)    // 最大开关量出板数
#define   MAX_BOARDOI_NUM     (64)    // 单板最大开关量出 total 1280 points
#define   MAX_CHANOI_NUM      (40)    // 通道最大配置数

#define   MAX_XI_BOARD        (4)     // 最大其他扩展板数(输入+输出各2块) 16-0308
#define   MAX_BOARDXI_NUM     (32)    // 单板最大扩展数32点IO 15-1229
#define   MAX_CHANXI_NUM      (16)    // 通道最大配置数 15-1116 4 -> 16

//----------
//  扩展辅助通道
#define   MAX_EBI_BOARD       (8)     // 最大扩展板数
#define   MAX_BOARDEBI_NUM    (64)    // 扩展单板最大数
#define   MAX_CHANEBI_NUM     (8)     // 扩展通道最大配置数

#define   MAX_EBO_BOARD       (8)     // 最大扩展板数
#define   MAX_BOARDEBO_NUM    (64)    // 扩展单板最大数
#define   MAX_CHANEBO_NUM     (8)     // 扩展通道最大配置数

#define   MAX_EDI_BOARD       (8)     // 最大扩展板数
#define   MAX_BOARDEDI_NUM    (32)    // 扩展单板最大数
#define   MAX_CHANEDI_NUM     (16)    // 扩展通道最大配置数

#define   MAX_EDO_BOARD       (8)     // 最大扩展板数
#define   MAX_BOARDEDO_NUM    (32)    // 扩展单板最大数 
#define   MAX_CHANEDO_NUM     (16)    // 扩展通道最大配置数
//----------
//  辅助通道默认设定值
#define   DEF_VI_BOARD        (1)     // 默认电压板数
#define   DEF_TI_BOARD        (1)     // 默认温度板数
#define   DEF_II_BOARD        (1)     // 默认输入板数
#define   DEF_OI_BOARD        (1)     // 默认输出板数

#define   DEF_XI_BOARD        (4)     // 默认其他单板点数0->4 16-0803 用作扩展IO一一映射

#define   DEF_EBI_BOARD       (0)     // 默认扩展板数
#define   DEF_EBO_BOARD       (0)     // 默认扩展板数
#define   DEF_EDI_BOARD       (0)     // 默认扩展板数
#define   DEF_EDO_BOARD       (0)     // 默认扩展板数

#define   DEF_BOARDVI_NUM     (24)    // 默认单板电压点数
#define   DEF_BOARDTI_NUM     (32)    // 默认单板温度点数
#define   DEF_BOARDII_NUM     (32)    // 默认单板输入点数
#define   DEF_BOARDOI_NUM     (32)    // 默认单板输出点数

#define   DEF_BOARDXI_NUM     (32)     // 默认其他单板点数1->32 15-1228 用作扩展IO一一映射

#define   DEF_BOARDEBI_NUM    (1)     // 默认单板扩展位输入点数
#define   DEF_BOARDEBO_NUM    (1)     // 默认单板扩展位输出点数
#define   DEF_BOARDEDI_NUM    (1)     // 默认单板扩展数据输入点数
#define   DEF_BOARDEDO_NUM    (1)     // 默认单板扩展数据输出点数

//----------------------------------------------------------------------------------------------------
//  控制配置默认参数 
//----------
//  change with 'CTRLCFG_MARK'  and Channel Numbers
//------------------------------
//  (1)     判断延时及计数倍率   （同下位机通道数目相关）

#if   (MAX_CHAN_NUM == 0)
    #error    "CAN device number can't not be zero!!"
#elif (MAX_CHAN_NUM <= 16)
    #define   JDG_TIMES     (1)
#elif (MAX_CHAN_NUM <= 32)
    #define   JDG_TIMES     (1)
#elif (MAX_CHAN_NUM <= 64)
    #define   JDG_TIMES     (2)
#elif (MAX_CHAN_NUM <= 96)
    #define   JDG_TIMES     (2)
#elif (MAX_CHAN_NUM <= 128)
    #define   JDG_TIMES     (3)
#elif (MAX_CHAN_NUM <= 192)     // 16-0519
    #define   JDG_TIMES     (4)    
#else
    #error  "Too many channels!"
#endif
//------------------------------
//  (2)     判断延时值及计数值

#define   MAX_JDG_T       (65000)               // 最大判断响应周期

#define   CAN_TO_N        (50)  //(20)          // CAN无接收超时计数 16-0528
#define   CTRL_RETRY_N    (3)                   // 控制故障重试次数
#define   CAN_RETRY_N     (5)                   // 通信故障重试次数
#define   CTRL_RETRY_T    (2000)                // 控制故障重试间隔 (mS)
#define   CAN_RETRY_T     (2000)                // 通信故障重试间隔 (mS)
#define   PD_RESUME_T     (5000)                // 掉电恢复延时 (mS)
#define   CTRL_VDELTA     (1000)                // 控制转换电压差值 (mV) 16-0113
    #define   LI_VDELTA     (5)                 // 控制转换电压差值 (万分之一) 16-0607
    #define   MAX_VDELTA    (50000)                // 最大控制转换电压差值 (mV) 16-0113
    #define   MIN_VDELTA    (1)                    // 最小控制转换电压差值 (mV) 16-0113
#define   START_DLY_T     (0)                   // 默认启动延时 (mS)

#define   XCUR_JDG_T      (1000 * JDG_TIMES)    // 电流异常判断响应周期   (mS)
#define   XVOL_JDG_T      (1000 * JDG_TIMES)    // 电压异常判断响应周期   (mS)
#define   XTEMP_JDG_T     (1000 * JDG_TIMES)    // 温度判断响应周期 (mS)
#define   XVI_JDG_T       (1000 * JDG_TIMES)    // 单体电压判断响应周期   (mS)
#define   XTI_JDG_T       (1000 * JDG_TIMES)    // 单体温度判断响应周期   (mS)

//2013-1013 电压电流判断加快2000mS->200，30mS 
//2014-0703 改回 1000
#define   IO_JDG_T        (1000 * JDG_TIMES)    // 开关量判断响应周期    (mS)
#define   VPOT_JDG_T      (500 * JDG_TIMES)     // 平台电压判断响应周期   (mS)
#define   CUR_JDG_T       (500 * JDG_TIMES)     // 电流判断响应周期 (mS)
#define   VOL_JDG_T       (500 * JDG_TIMES)     // 电压判断响应周期 (mS)
//#define   TEMP_JDG_T      (1000 * JDG_TIMES)    // 温度判断响应周期   (mS)
#define   POW_JDG_T       (1000 * JDG_TIMES)    // 功率响应周期   (mS)
#define   ENGY_JDG_T      (1000 * JDG_TIMES)    // 能量判断响应周期 (mS)
#define   VI_JDG_T        (1000 * JDG_TIMES)    // 单体电压判断响应周期   (mS)
#define   TI_JDG_T        (1000 * JDG_TIMES)    // 单体温度判断响应周期   (mS)

#define   RECORD_T        (100)                 // 记录间隔 (mS)

#define     MAX_DOT         (5)         // 最大允许小数点      15-1203
#define     VI_DOT          (3)         // 默认单体电压3位小数(S16)
#define     TI_DOT          (2)         // 默认单体温度2位小数(S16)
//--------------------------------------------------
#define     DEF_TMP_FLOAT   (-273.15)   // 默认温度（浮点） 
//--------------------
//  小数点相关表格 15-1203
//--------------------
// 整数 -> 5位小数 单体电压与单体温度配置
const unsigned int  DOT_DIV_TBL[MAX_DOT+1]  = { 1, 10, 100, 1000, 10000, 100000 };              // 采样值除数值   
const int           DEF_TMP_INT[MAX_DOT+1]  = {-273,-2732,-27315,-273150,-2731500,-27315000 };  // 默认温度（整数）  
//--------------------
//  (3)     判断合理性
#if     ((XCUR_JDG_T  <= 2*SCAN_PERIOD) || (XCUR_JDG_T  > MAX_JDG_T))
  #error    "'XCUR_JDG_T' is set error !"
#elif   ((XVOL_JDG_T  <= 2*SCAN_PERIOD) || (XVOL_JDG_T  > MAX_JDG_T))
  #error    "'XVOL_JDG_T' is set error !"
#elif   ((XTEMP_JDG_T <= 2*SCAN_PERIOD) || (XTEMP_JDG_T > MAX_JDG_T)) 
  #error    "'XTEMP_JDG_T' is set error !"
#elif   ((XVI_JDG_T   <= 2*SCAN_PERIOD) || (XVI_JDG_T   > MAX_JDG_T)) 
  #error    "'XVI_JDG_T' is set error !"
#elif   ((XTI_JDG_T   <= 2*SCAN_PERIOD) || (XTI_JDG_T   > MAX_JDG_T)) 
  #error    "'XTI_JDG_T' is set error !"
#elif   ((IO_JDG_T    <= 2*SCAN_PERIOD) || (IO_JDG_T    > MAX_JDG_T)) 
  #error    "'IO_JDG_T' is set error !"
#elif   ((VPOT_JDG_T  <= 2*SCAN_PERIOD) || (VPOT_JDG_T  > MAX_JDG_T)) 
  #error    "'VPOT_JDG_T' is set error !"
#elif   ((CUR_JDG_T   <= 2*SCAN_PERIOD) || (CUR_JDG_T   > MAX_JDG_T)) 
  #error    "'CUR_JDG_T' is set error !"
#elif   ((VOL_JDG_T   <= 2*SCAN_PERIOD) || (VOL_JDG_T   > MAX_JDG_T)) 
  #error    "'VOL_JDG_T' is set error !"
// #elif    ((TEMP_JDG_T <= 2*SCAN_PERIOD) || (TEMP_JDG_T > MAX_JDG_T)) 
//  #error    "'TEMP_JDG_T' is set error !"
#elif   ((POW_JDG_T   <= 2*SCAN_PERIOD) || (POW_JDG_T   > MAX_JDG_T)) 
  #error    "'POW_JDG_T' is set error !"
#elif   ((ENGY_JDG_T  <= 2*SCAN_PERIOD) || (ENGY_JDG_T  > MAX_JDG_T)) 
  #error    "'ENGY_JDG_T' is set error !"
#elif   ((VI_JDG_T    <= 2*SCAN_PERIOD) || (VI_JDG_T    > MAX_JDG_T)) 
  #error    "'VI_JDG_T' is set error !"
#elif   ((TI_JDG_T    <= 2*SCAN_PERIOD) || (TI_JDG_T    > MAX_JDG_T)) 
  #error    "'TI_JDG_T' is set error !"
#else

#endif
//----------------------------------------------------------------------------------------------------
//  主控设备代码（MainChanType）
//----------
#define   MAIN_GENERAL          0x00  /* 通用设备   */

#define   MAIN_LI_FORMATER_A    0x01  /* 锂电化成设备     */
#define   MAIN_LI_TESTER_A      0x02  /* 锂电测试设备     */

//----------
#define   MAIN_PB_TESTER_A      0x81  /* 铅酸测试机下位机   */

//----------------------------------------------------------------------------------------------------
//  辅助设备代码（SubChanType）
//----------
#define   SUB_GENERAL           0x00  /* 通用设备   */

#define   SUB_NEEDLE_PALLET_A   0x01  /* 针床托盘A: 
                                        96回路：3层针床，4托盘每层，8电池每托盘。
                                        每层(区)一个温度开关量采集控制板，共3块板
                                        温度开关量采集控制板：32个温度，12个输入，2个交流输出，7个直流输出。
                                        温度：1个电池1个温度，共32个，放在实时数据中；
                                        输入：1个阀门信号，8个用在托盘到位信号，2个用在烟感探头，1个用在启停
                                        输出：2交流开关量控制风机，1个直流用在气阀，运行、故障、红、
                                        黄、绿、蜂鸣指示共6个直流。*/
//----------------------------------------------------------------------------------------------------
const   unsigned char   B8_MASK_P[8]    = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};
const   unsigned char   B8_MASK_N[8]    = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
//--------------------------------------------------
//const U8      U8MSK[8]        = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
const unsigned short    U16HMSK[8]      = {0x0000,0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700};
//----------------------------------------------------------------------------------------------------
//  设备额定默认与限制 16-0125
#define     DEF_CHR_CUR         (10000.0L)      // 默认最大充电电流(mA)
#define     DEF_DCH_CUR         (10000.0L)      // 默认最大放电电流(mA)
#define     DEF_CHR_VOL         (5000.0L)       // 默认最大充电电压(mV)
#define     DEF_DCH_VOL         (0.0L)          // 默认最小放电电压(mV)

#define     MAX_CHR_CUR         (20000000.0L)   // 允许最大充电电流(mA)
#define     MAX_DCH_CUR         (20000000.0L)   // 允许最大放电电流(mA)
#define     MAX_CHR_VOL         (1000000.0L)    // 允许最大充电电压(mV)
#define     MIN_DCH_VOL         (-100000.0L)    // 允许最小放电电压(mV)
//--------------------------------------------------
#define     RUN_FANON_TEMP      (25.0L)         // 风机正常开启温度(℃)
#define     PROT_FANON_TEMP     (65.0L)         // 风机保护开启温度(℃)

#define     MAX_FANON_TEMP      (200.0L)        // 最大风机开启温度(℃)
#define     MIN_FANON_TEMP      (-20.0L)        // 最小风机开启温度(℃)
//----------------------------------------------------------------------------------------------------
//  cDev.wmfunc_cfg 工作模式功能位配置 （0：屏蔽，1：开启）15-1124
//----------
#define   WMFUNC_BIT_CCC        ((U32)(1 << 0))   // 恒流充电   功能位
#define   WMFUNC_BIT_CVC        ((U32)(1 << 1))   // 恒压充电   功能位
#define   WMFUNC_BIT_CCD        ((U32)(1 << 2))   // 恒流放电   功能位
#define   WMFUNC_BIT_CVD        ((U32)(1 << 3))   // 恒压放电   功能位
#define   WMFUNC_BIT_CPD        ((U32)(1 << 4))   // 恒功率放电  功能位
#define   WMFUNC_BIT_CPC        ((U32)(1 << 5))   // 恒功率充电  功能位
#define   WMFUNC_BIT_CRD        ((U32)(1 << 6))   // 恒电阻放电  功能位
#define   WMFUNC_BIT_STATIC     ((U32)(1 << 7))   // 静置（搁置） 功能位
#define   WMFUNC_BIT_PULSE_C    ((U32)(1 << 8))   // 脉冲充电   功能位
#define   WMFUNC_BIT_PULSE_D    ((U32)(1 << 9))   // 脉冲放电   功能位
#define   WMFUNC_BIT_PULSE_S    ((U32)(1 <<10))   // 脉冲静置   功能位
#define   WMFUNC_BIT_LINE_D     ((U32)(1 <<11))   // 线性放电   功能位
#define   WMFUNC_BIT_IF         ((U32)(1 <<12))   // 如果（虚模式）  功能位
#define   WMFUNC_BIT_ELSE       ((U32)(1 <<13))   // 否则（虚模式）  功能位
#define   WMFUNC_BIT_VAR_OPT    ((U32)(1 <<14))   // 变量操作   功能位
#define   WMFUNC_BIT_CYC        ((U32)(1 <<15))   // 循环
#define   WMFUNC_BIT_PULSE_INT  ((U32)(1 <<16))   // 间歇脉冲
#define   WMFUNC_BIT_PULSE_ON   ((U32)(1 <<17))   // 正负脉冲
#define   WMFUNC_BIT_EXT_CTRL   ((U32)(1 <<18))   // 外控
#define   WMFUNC_BIT_CRC        ((U32)(1 <<19))   // 恒阻充电

//----- 默认开启工作模式 -----
#define   DEFAULT_WMFUNC_BIT    ( WMFUNC_BIT_CCC      \
                                | WMFUNC_BIT_CVC      \
                                | WMFUNC_BIT_CCD      \
                                | WMFUNC_BIT_STATIC   \
                                | WMFUNC_BIT_IF       \
                                | WMFUNC_BIT_ELSE     \
                                | WMFUNC_BIT_VAR_OPT  \
                                )
//----- 数据类型位定义 -----   // 16-0505
typedef union   _unWmFunc_
{
    struct {
        U32 bCCC        : 1;
        U32 bCVC        : 1;
        U32 bCCD        : 1;
        U32 bCVD        : 1;
        U32 bCPD        : 1;
        U32 bCPC        : 1;
        U32 bCRD        : 1;
        U32 bSTATIC     : 1;
        U32 bPUL_C      : 1;
        U32 bPUL_D      : 1;
        U32 bPUL_S      : 1;
        U32 bLINE_D     : 1;
        U32 bIF         : 1;
        U32 bELSE       : 1;
        U32 bVAR_OPT    : 1;
        U32 bCYC        : 1;
        U32 bPUL_INT    : 1;
        U32 bPUL_PN     : 1;
        U32 bEXT_CTRL   : 1;
        U32 bCRC        : 1;
        U32 bREV        : 1;    // 保留位
    } bits;    
    U32 word;    
    
}   unWmFunc;
//----------------------------------------------------------------------------------------------------
//  cDev.exfunc_cfg 设备扩展功能位配置 （0：屏蔽，1：开启）
//----------
#define   EXFUNC_BIT_IR         ((U32)(1 << 0))   // 内阻测试 功能位
#define   EXFUNC_BIT_NOBEEP     ((U32)(1 << 1))   // 屏蔽蜂鸣报警 功能位
#define   EXFUNC_BIT_INPLACE    ((U32)(1 << 2))   // 托盘位到位报警 功能位
#define   EXFUNC_BIT_AUTOLIFT   ((U32)(1 << 3))   // 区结束自动抬升针床 功能位
#define   EXFUNC_BIT_WRUNLIFT   ((U32)(1 << 4))   // 区运行允许抬升针床 功能位
#define   EXFUNC_BIT_MANOUT     ((U32)(1 << 5))   // 人工控制开关量输出 功能位

#define   EXFUNC_BIT_REV        ((U32)(1 << 6))   // 保留 功能位

//----- 默认开启扩展模式 -----
#define   DEFAULT_EXFUNC_BIT     (0)

//----- 数据类型位定义 -----   // 16-0505
typedef union   _unExFunc_
{
    struct {
        U32 bIR         : 1;
        U32 bNOBEEP     : 1;
        U32 bINPLACE    : 1;
        U32 bAUTOLIFT   : 1;
        U32 bWRUNLIFT   : 1;
        U32 bMANOUT     : 1;
        U32 bREV        : 1;    // 保留位
     
    } bits;    
    U32 word;    
    
}   unExFunc;
//----------------------------------------------------------------------------------------------------
typedef struct _strCtrlCfg_     //14-0724 -> 76Bytes MAX:256Bytes
{
    U32 Mark;             // 标记

    U8  bPdResumeMode;    // 掉电恢复与否 默认：1 - 允许
    U8  bStepSyncFlag;    // 阶段同步与否 默认：1 - 允许
    U8  bParallel;        // 允许并联与否 默认：0 - 禁止
    U8  bAbnResume;       // 故障恢复与否 默认：0 - 禁止

    U8  MaxViBdNum;       // 最大巡检电压板数，N/A
    U8  MaxTiBdNum;       // 最大巡检温度板数，N/A
    U8  MaxIiBdNum;       // 最大开关量输入板数，N/A
    U8  MaxOiBdNum;       // 最大开关量输出板数，N/A

    U8  MaxEBiBdNum;      // 最大扩展位输入板数，N/A
    U8  MaxEBoBdNum;      // 最大扩展位输出板数，N/A
    U8  MaxEDiBdNum;      // 最大扩展数据输入板数，N/A
    U8  MaxEDoBdNum;      // 最大扩展数据输出板数，N/A

    U8  BoardViNum;       // 板级巡检电压数
    U8  BoardTiNum;       // 板级巡检温度数
    U8  BoardIiNum;       // 板级开关量输入数
    U8  BoardOiNum;       // 板级开关量输出数

    U8  BoardExBiNum;     // 板级扩展位输入数
    U8  BoardExBoNum;     // 板级扩展位输出数
    U8  BoardExDiNum;     // 板级扩展数据输入数
    U8  BoardExDoNum;     // 板级扩展数据输出数

    U16 CanOff_n;         // CAN超时计数器（收不到任何通道的包）
    U16 CtrlRetry_n;      //（控制故障时）重试次数
    U16 CanRetry_n;       //（通信故障时）重试次数
    U16 CtrlRetry_t;      //（控制故障时）重试间隔（mS）

    U16 CanRetry_t;       //（通信故障时）重试间隔（mS）
    U16 PDResume_t;       // 掉电恢复周期  0 表示不恢复，否则代表恢复倒计数时间 （mS）
//    U16 CtrlDT;           // 控制死区时间
    U16 Vdelta;           // 恒流转换电压控制差值（mV）：恒流下发控制电压 = 差值+转换电压 15-0113
    U16 StartDly_t;       // 启动延时单元

    U16 XCurJdg_t;        // 电流异常判断周期   (mS)
    U16 XVolJdg_t;        // 电压异常判断周期   (mS)
    U16 XTempJdg_t;       // 温度异常判断周期   (mS)
    U16 XViJdg_t;         // 单体电压异常判断周期 (mS)

    U16 XTiJdg_t;         // 单体温度异常判断周期 (mS)
    U16 IOJdg_t;          // 开关量响应判断周期  (mS)
    U16 VpotJdg_t;        // 平台电压响应判断周期 (mS)
    U16 CurJdg_t;         // 电流 判断响应周期  (mS)

    U16 VolJdg_t;         // 电压 判断响应周期  (mS)
    U16 PowJdg_t;         // 功率 判断响应周期  (mS)
    U16 EngyJdg_t;        // 瓦时 判断响应周期  (mS)
    U16 ViJdg_t;          // 单体电压 判断响应周期 (mS)

    U16 TiJdg_t;          // 单体温度 判断响应周期 (mS)
    U16 Record_t;         // 默认记录间隔 (mS)

    U8  Vi_Dot;           // 单体电压小数位
    U8  Ti_Dot;           // 单体温度小数位
    U8  bAutoReset;       // 自动复位与否 默认：0 - 禁止
    U8  bMainSubOrder;    // 主从CAN次序 默认：0 - CAN1主，CAN2从

    U8  MainChanType;     // 主控设备类型 默认：0 - 通用
    U8  SubChanType;      // 辅助设备类型 默认：0 - 通用

    U16 crc;              // CRC效验和

}   strCtrlCfg;
//----------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 辅助通道配置表
typedef struct _strSubChCfg_    // MAX:256Bytes
{
    U32   Mark;         // 标记

    U16   Rev;
    U16   crc;          // CRC效验和

} strSubChCfg;
//------------------------------------------------------------------------------------------
// 单体电压硬件配置表(上位机设置)
typedef struct _strViHdCfg_     // 48bytes
{
    U32   Mark;                 // 标记
    U8    HdTbl[MAX_VI_BOARD];  // 采样点数
    U16   UseNum;               // 配置板数 必须<MAX_OI_BOARD
    U16   crc;                  // CRC效验和

} strViHdCfg;
//------------------------------
// 单体电压硬件检索表(软件设置)
typedef struct _strViHdChk_
{
    U16   GblInx[MAX_VI_BOARD]; // 全局索引

} strViHdChk;
//------------------------------------------------------------------------------------------
// 单体温度硬件配置表(上位机设置)
typedef struct _strTiHdCfg_     // 48bytes
{
    U32   Mark;                 // 标记
    U8    HdTbl[MAX_TI_BOARD];  // 采样点数
    U16   UseNum;               // 配置板数 必须<MAX_TI_BOARD
    U16   crc;                  // CRC效验和

} strTiHdCfg;
//------------------------------
// 单体温度硬件检索表(软件设置)
typedef struct _strTiHdChk_
{
    U16   GblInx[MAX_TI_BOARD]; // 全局索引

} strTiHdChk;
//------------------------------------------------------------------------------------------
// 开关量入硬件配置表(上位机设置)
typedef struct _strIiHdCfg_     // 28bytes
{
    U32   Mark;                 // 标记
    U8    HdTbl[MAX_II_BOARD];  // 采样点数
    U16   UseNum;               // 配置板数 必须<MAX_II_BOARD
    U16   crc;                  // CRC效验和

} strIiHdCfg;
//------------------------------
// 开关量入硬件检索表(软件设置)
typedef struct _strIiHdChk_
{
    U16   GblInx[MAX_II_BOARD]; // 全局索引

} strIiHdChk;
//------------------------------------------------------------------------------------------
// 开关量出硬件配置表(上位机设置)
typedef struct _strOiHdCfg_     // 28bytes
{
    U32   Mark;                 // 标记
    U8    HdTbl[MAX_OI_BOARD];  // 采样点数
    U16   UseNum;               // 配置板数 必须<MAX_OI_BOARD
    U16   crc;                  // CRC效验和

} strOiHdCfg;
//------------------------------
// 开关量出硬件检索表(软件设置)
typedef struct _strOiHdChk_
{
    U16   GblInx[MAX_OI_BOARD]; // 全局索引

} strOiHdChk;
//------------------------------------------------------------------------------------------
// 其他通道硬件配置表(上位机设置)
typedef struct _strXiHdCfg_     // 28bytes(支持到20路时)
{
    U32   Mark;                 // 标记
    U8    HdTbl[MAX_XI_BOARD];  // 采样点数
    U16   UseNum;               // 配置板数 必须<MAX_OI_BOARD
    U16   crc;                  // CRC效验和

} strXiHdCfg;
//------------------------------
// 其他通道硬件检索表(软件设置)
typedef struct _strXiHdChk_
{
    U16   GblInx[MAX_XI_BOARD];   // 全局索引

} strXiHdChk;
//----------------------------------------------------------------------------------------------------
#define     DEVID_LEN       (32)        // 设备ID长度   （字节）
#define     DEVNAME_LEN     (32)        // 设备名称长度   （字节）
#define     CMDVERTBL_LEN   (256)       // 命令版本表长度  （字节）
#define     DEVSTR_LEN      (6)         // 设备结构表长度  （字节）
#define     SCFGTBL_LEN     (96)        // 辅助通道软配置表长度   （字节）
#define     XITEMTBL_LEN    (32)        // 实施明细其他项配置表长度 （字节）
//--------------------------------------------------------------------------
// 设备信息 Ver 1.0 128字节
// 15-1112：加入明实时明细其他项配置表，版本Ver 1.1
// 16-0125：加入通道并联数配置，版本Ver 1.2 [176字节]
//--------------------------------------------------------------------------
// V1.2
typedef struct _strDevInfo_ //MAX:512Bytes
{
            U32         mark;
/*   0*/    U8          day;
/*   1*/    U8          mon;
/*   2*/    U16         year;                   // 生产日期
/*   4*/    U8          dev_id[DEVID_LEN];      // 设备号ID（客户定义）[32]
/*  36*/    U8          dev_name[DEVNAME_LEN];  // 设备名称（厂家定义）[32]
/*  68*/    U16         soft_ver;               // 中位机软件版本号（只读）
/*  70*/    U16         hard_ver;               // 中位机硬件版本号（读写）
    
/*  72*/    U8          max_node_num;           // 最大节点数 【下位机电源数，只读 == MAX_NODE_NUM】
/*  73*/    U8          max_chan_num;           // 最大通道数 【通道类成员数，只读 == MAX_CHAN_NUM】
/*  74*/    U8          chan_num;               // 控制通道数 【用户界面（实时数据，工艺，校准等相关）】
/*  75*/    U8          par_node_num;           // 并联节点数 【线性并联，有最小数限制；并联禁止时 == 0】
    
/*  76*/    U8          ext_chan_num;           // 扩展通道数 16-0129
/*  77*/    U8          dev_str_type;           // 设备结构类型
/*  78*/    U8          dev_str_tbl[DEVSTR_LEN];// 设备结构配置表[6]

/*  84*/    float       max_chr_cur;            // 最大充电电流
/*  88*/    float       max_chr_vol;            // 最大充电电压
/*  92*/    float       max_chr_pow;            // 最大充电功率
/*  96*/    float       max_dch_cur;            // 最大放电电流
/* 100*/    float       min_dch_vol;            // 最小放电电压
/* 104*/    float       max_dch_pow;            // 最大放电功率
/* 108*/    Union32     run_fan_temp;           // 运行风机开启温度（0xffffffff表示常开）
/* 112*/    Union32     prt_fan_temp;           // 保护风机开启温度（0xffffffff表示常闭）

/* 116*/    unWmFunc    wmfunc_cfg;             // 工作模式功能开关 16-0505
/* 120*/    unExFunc    exfunc_cfg;             // 设备扩展功能开关 16-0505
/* 124*/    U32         min_rec_time;           // 最小记录时间(uS)

/* 128*/    U16         vi_num;                 // 通道电压数
/* 130*/    U16         ti_num;                 // 通道温度数
/* 132*/    U8          ii_num;                 // 通道开关量入数
/* 133*/    U8          oi_num;                 // 通道开关量出数
/* 134*/    U16         xitem_num;              // 其他项数（暂时支持最多16项）
/* 136*/    U8          xitem_tbl[XITEMTBL_LEN];// 其他项配置表[32]
                    
/* 168*/    U8          rev[2];                 // 保留对齐
/* 170*/    U16         crc;                    // crc16 MODBUS-RTU 校验
        
} strDevInfo;

// V1.1
//typedef struct _strDevInfo_ //MAX:512Bytes
//{
//            U32        mark;
///*   0*/    U8         day;
///*   1*/    U8         mon;
///*   2*/    U16        year;                    // 生产日期

///*   4*/    U8         dev_id[DEVID_LEN];       // 设备号ID（厂家定义）
///*  36*/    U8         dev_name[DEVNAME_LEN];   // 设备名称（客户定义）

///*  68*/    U16        soft_ver;                // 中位机软件版本号（只读）
///*  70*/    U16        hard_ver;                // 中位机硬件版本号（读写）

/////*  72*/      U8         cmd_ver[CMDVERTBL_LEN];  // 通信命令版本表

///*  72*/    U8         chan_num;                // 通道数
///*  73*/    U8         dev_str_type;            // 设备结构类型
///*  74*/    U8         dev_str_tbl[DEVSTR_LEN]; // 设备结构配置表

///*  80*/    float      max_chr_cur;             // 最大充电电流
///*  84*/    float      max_chr_vol;             // 最大充电电压
///*  88*/    float      max_chr_pow;             // 最大充电功率
///*  92*/    float      max_dch_cur;             // 最大放电电流
///*  96*/    float      min_dch_vol;             // 最小放电电压
///* 100*/    float      max_dch_pow;             // 最大放电功率
///* 104*/    Union32    run_fan_temp;            // 运行风机开启温度（0xffffffff表示常开）
///* 108*/    Union32    prt_fan_temp;            // 保护风机开启温度（0xffffffff表示常闭）

///* 112*/    U32        wmfunc_cfg;              // 工作模式功能开关
///* 116*/    U32        exfunc_cfg;              // 设备扩展功能开关
///* 120*/    U32        min_rec_time;            // 最小记录时间(uS)

/////* 372*/      U8         host_ip[4];              // 主机IP 0x84命令不修改
/////* 376*/      U16        host_port;               // 主机端口 0x84命令不修改
/////* 378*/      U8         dev_mac[6];              // 设备MAC 0x84命令不修改

/////* 384*/      U16        vi_soft_tbl[SCFGTBL_LEN];    // 通道电压     软件配置表
/////* 576*/      U16        ti_soft_tbl[SCFGTBL_LEN];    // 通道温度     软件配置表
/////* 768*/      U8         ii_soft_tbl[SCFGTBL_LEN];    // 通道开关量入   软件配置表
/////* 864*/      U8         oi_soft_tbl[SCFGTBL_LEN];    // 通道开关量出   软件配置表
/////* 960*/      U8         xi_soft_tbl[SCFGTBL_LEN];    // 通道扩展项    软件配置表

//            U16        vi_num;                  // 通道电压数
//            U16        ti_num;                  // 通道温度数
//            U8         ii_num;                  // 通道开关量入数
//            U8         oi_num;                  // 通道开关量出数
//            U16        xitem_num;               // 其他项数（暂时支持最多16项）
//            U8         xitem_tbl[XITEMTBL_LEN]; // 其他项配置表
//                    
//            U8         rev[2];                  // 保留对齐
//            U16        crc;                     // crc16 MODBUS-RTU 校验
//        
//} strDevInfo;

//----------------------------------------------------------------------------------------------------
//  默认明细数据其他项表 
//----------------------------------------------------------------------------------------------------
#define     DEF_XITEM_NUM       (0)         // 默认其他项数量
//#define       MAX_XITEM_NUM       (16)        // 最大其他项数量

#ifdef      _IN_DEV_CPP
    const unsigned char DEF_XITEM_TBL[XITEMTBL_LEN] =
    {
/*      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,         */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x0X */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  /* 0x1X */
    };
#else

#endif

//----------------------------------------------------------------------------------------------------
//温度分配表(新版)
typedef struct _strTempTbl_
{
    U32     Mark;
    U16     tbl[MAX_CHAN_NUM][MAX_CHANTI_NUM];
    U16     rev1;
    U16     crc;

} strTempTbl;
//----------------------------------------------------------------------------------------------------
//主通道对应的辅助采样通道配置表(采样点配置)
/*
    同一类所有板采样数据映射到单个数组,默认各个板数据数量相等
*/
typedef struct  _strSampTbl_
{
    U16     ViOffset;   //单体电压号偏移
    U16     ViNum;      //单体电压数

    U16     TiOffset;   //单体温度号偏移
    U16     TiNum;      //单体温度数

    U16     IiOffset;   //开关量输入偏移
    U16     IiNum;      //开关量输入数

    U16     OiOffset;   //开关量输出偏移
    U16     OiNum;      //开关量输出数
        
    U16     XiOffset;   //其他扩展数据偏移
    U16     XiNum;      //其他扩展数据数

}   strSampTbl;
//------------------------------------------------------------
typedef struct  _strSampCfg_    // maxsize: 8 + 128*20 = 2568(0xA08) bytes
{
    U32         Mark;
    strSampTbl  ch[MAX_CHAN_NUM];   // num * 20bytes: 96ch->0x780 bytes
    U16         rev;
    U16         crc;

}   strSampCfg;
//----------------------------------------------------------------------------------------------------
//------------------------------------------------------------
//主机控制
typedef struct  _strHostCtrl_   // 12bytes
{
    U16     flag;       // 标记
    U16     port;       // 主机端口
    U8      ip[4];      // 主机地址
    U16     rev;
    U16     crc;

}   strHostCtrl;
//------------------------------------------------------------
//PC从机控制
typedef struct  _strSlaveCtrl_
{
    U8      flag;       // 注册标记
    U8      ch;         // 实时上传数据通道索引
    U8      ip[4];      // 注册地址
    U16     tout;       // 通信超时计数

}   strPCSlaCtrl;
//------------------------------------------------------------
//本机网络信息
typedef struct _strLocalM_  // 36 bytes
{
    U32     Mark;       // 标记
    U8      Ip[4];      // 本机IP地址
    U8      GW[4];      // 默认网关IP地址
    U8      Mask[4];    // 子网掩码
    U8      DNS1[4];    // 首选DNS服务器IP地址
    U8      DNS2[4];    // 备用DNS服务器IP地址
    U8      MAC[8];     // MAC地址【8个保证字对齐】
    U16     Port;       // 端口
    U16     crc;

} strLocalM;
//------------------------------------------------------------
//change with 'NETINFO_MARK'
static const strLocalM  DEFAULT_NETINFO =
{
    NETINFO_MARK,       // 标记
    {192,168,0,195},    // 本机IP地址
    {192,168,0,254},    // 默认网关IP地址
    {255,255,255,0},    // 子网掩码
    {0,0,0,0},          // 首选DNS服务器IP地址
    {0,0,0,0},          // 备用DNS服务器IP地址
    {0x1e,0x1f,0x20,0x21,0x22,0x23,0x00,0x00},  // MAC地址
    1001,               // 端口号
    0x0000              // 保留
};

//----------------------------------------------------------------------------------------------------
//  实时明细数据发送处理
//----------------------------------------------------------------------------------------------------
typedef struct _strDataTxCtrl_ 
{
    unsigned char   realCh;                 // 实时数据起始循环扫描通道号，在通道多而导致UDP包无法一次传完时用
    unsigned char   recCh;                  // 上传记录起始循环扫描通道号
    unsigned char   repeatNum;              // 翻滚计数器重复次数
    unsigned short  packLen[MAX_CHAN_NUM];  // 实时或明细包长度(当前测试)
    unsigned short  recNum[MAX_CHAN_NUM];   // 缓冲中记录(未读取)个数
    
}   strDataTxCtrl;    

//---------------------------------------------------------------------------------------------------- 
//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
//----------------------------------------------------------------------------------------------------
//类结构定义
//----------------------------------------------------------------------------------------------------
//定义测试机通道类
//----------------------------------------------------------------------------------------------------
class Dev
{
private:
    U8              mNo;          // 设备索引号 
    strDevInfo      mInfoBak;     // 设备信息备份

public:
    strDevInfo      mInfo;        // 设备信息
    strCtrlCfg      mCtrlCfg;     // 控制参数配置
    strSubChCfg     mSubChCfg;    // 辅助通道硬件数配置
    strViHdCfg      mViHdCfg;     // 单体电压硬件配置（与设备结构耦合，未开始）16-0514
    strViHdChk      mViHdChk;     // 单体电压硬件检索
    strTiHdCfg      mTiHdCfg;     // 单体温度硬件配置（与设备结构耦合）16-0514
    strTiHdChk      mTiHdChk;     // 单体温度硬件检索
    strIiHdCfg      mIiHdCfg;     // 开关量入硬件配置（与设备结构耦合）16-0514
    strIiHdChk      mIiHdChk;     // 开关量入硬件检索
    strOiHdCfg      mOiHdCfg;     // 开关量出硬件配置（与设备结构耦合）16-0514
    strOiHdChk      mOiHdChk;     // 开关量出硬件检索
    strXiHdCfg      mXiHdCfg;     // 其他辅助通道硬件配置
    strXiHdChk      mXiHdChk;     // 其他辅助通道硬件检索

    strSampCfg      mSampCfg;       // 辅助通道软件配置表
//      strTempTbl      mTempTbl;     // 温度配置表 150925
    strHostCtrl     mHostCtrl;      // 主机控制
    strPCSlaCtrl    mPCSlaCtrl[MAX_SLAVE_PC]; // PC从机控制
    strLocalM       mNetInfo;       // 网络层相关信息
    U32             mCUI_Mark;      // 恒压恒流通道标志位(最多32个)
    U32             mGblFault;      // 全局故障
    strDataTxCtrl   mTxCtrl;
    
    // 使用联机固定的辅助实时明细数据 15-1122
    U16         SavePackLen;        // 当前记录包（记录保存）长度
    U16         MaxSavePack;        // 缓冲保存的最大记录数
    U16         LoadPackLen;        // 上传数据（实时或明细）长度
    U16         MaxLoadPack;        // 最多上传包个数

public:
    Dev();

    inline  U32   Get_CUI_Mark(void) {return (mCUI_Mark);};
    inline  void  Set_CUI_Mark(U32 val) {mCUI_Mark = val;};

//    U8      LoadCfg(U8 type);                       // 加载配置
//    U16     CopyCfg(U8 type, U8 *);                 // 拷贝配置
//    U8      SaveCfg(U8 type, U8 *);                 // 保存配置
//    U8      CheckCfg(U8 type, const strCtrlCfg *);  // 检查配置

    U16     Rd_DevInfo(U8 *, U8);                   // 读取设备信息
    U8      Wr_DevInfo(U8 *);                       // 修改设备信息
    U8      LoadDevInfo(void);                      // 从MRAM加载设备信息
    U8      SaveDevInfo(void);                      // 保存设备信息到MRAM
    U8      CheckDevInfo(const strDevInfo *);       // 检查设备信息合法性
    void    PackLenSetProc(void);                   // 实时明细数据包长度限制

    void    InitHostCtrl(void);                     // 初始化主机信息
    U8      LoadHostCtrl(void);                     // 加载主机信息
    U8      SaveHostCtrl(void);                     // 保存主机信息
    U8      CheckHostCtrl(const strHostCtrl *);     // 主机信息检查

    U8      LoadCtrlCfg(void);                      // 加载控制配置
    U16     CopyCtrlCfg(U8 *);                      // 获取控制配置
    U8      SaveCtrlCfg(U8 *);                      // 保存控制配置
    U8      CheckCtrlCfg(const strCtrlCfg *);       // 检查控制配置

    U8      LoadSubChCfg(void);                     // 加载辅助通道硬件配置
    U16     CopySubHdCfg(U8 *, U8);                 // 获取辅助通道硬件配置
    U8      SaveSubHdCfg(U8 *, U8);                 // 保存辅助通道配置
    U8      CheckSubChCfg(const strSubChCfg *);     // 检查辅助通道配置

    U8      LoadSchHdCfg(U8 type);                  // 加载指定辅助通道配置
    U16     CopySchHdCfg(U8 type, U8 *);            // 拷贝指定辅助通道配置
    U8      SaveSchHdCfg(U8 type, U8 *);            // 保存指定辅助通道配置
    U8      CheckSchHdCfg(U8 type, const U8 *);     // 检查指定辅助通道配置

    U8      LoadViHdCfg(void);                      // 加载单体电压硬配置
    U16     CopyViHdCfg(U8 *);                      // 获取单体电压硬配置
    U8      SaveViHdCfg(U8 *);                      // 保存单体电压硬配置
    U8      CheckViHdCfg(const strViHdCfg *);       // 检查单体电压硬配置

    U8      LoadTiHdCfg(void);                      // 加载单体温度硬配置
    U16     CopyTiHdCfg(U8 *);                      // 获取单体温度硬配置
    U8      SaveTiHdCfg(U8 *);                      // 保存单体温度硬配置
    U8      CheckTiHdCfg(const strTiHdCfg *);       // 检查单体温度硬配置

    U8      LoadIiHdCfg(void);                      // 加载开关量入硬配置
    U16     CopyIiHdCfg(U8 *);                      // 获取开关量入硬配置
    U8      SaveIiHdCfg(U8 *);                      // 保存开关量入硬配置
    U8      CheckIiHdCfg(const strIiHdCfg *);       // 检查开关量入硬配置

    U8      LoadOiHdCfg(void);                      // 加载开关量出硬配置
    U16     CopyOiHdCfg(U8 *);                      // 获取开关量出硬配置
    U8      SaveOiHdCfg(U8 *);                      // 保存开关量出硬配置
    U8      CheckOiHdCfg(const strOiHdCfg *);       // 检查开关量出硬配置

    U8      LoadSampCfg(void);                      // 加载辅助通道软配置
    U16     CopySampCfg(U8 *);                      // 获取辅助通道软配置
    U8      SaveSampCfg(U8 *);                      // 保存辅助通道软配置
    U8      CheckSampTbl(const strSampTbl *, U8);   // 检查辅助通道软配置 15 -0119

//      U8    LoadTempTbl(void);                        // 加载温度配置
//      U16   CopyTempTbl(U8 *);                        // 获取温度配置
//      U8    SaveTempTbl(U8 *);                        // 保存温度配置
//      U8    CheckTempTbl(const strTempTbl *);         // 检查温度配置

    U8    LoadNetInfo(void);                        // 加载网络层信息
    void  ReflashNetCfg(void);                      // 刷新网络配置
    U8    CheckNetInfo(const strLocalM *);          // 检查网络层信息合法性

    U8    Test_AnyChRunning(void);                  // 检查所有通道，是否有运行的通道
    U8    SetMACIP(U8 *ptr);                        // 设置IP及设备MAC(0x89)
    U16   GetMACIP(U8 *ptr);                        // 获取IP及设备MAC(0x89)

    void  Init_RealRecCtrl(void);                   // 实时，记录数据收发控制初始化

    U16   SetUIMark(U8 *ptr);                       // 获取恒压恒流信号
    U16   RD_HIP_SMAC_Proc(U8 *, U8);               // 读主机IP及设备MAC(0x8B)

    void  GetRTC(void);                             // 获取实时时钟
    U16   Test_RemotePC(const U8 *);                // 远程PC - 主从机判断
    U8    EmptyPCSlaCtrl(U8);                       // 清空PC从机控制缓冲
    void  PCSlaTout(void);                          // PC从机通信超时控制

    void  SetGblFault(U32);                         // 全局故障置位
    void  RstGblFault(U32);                         // 全局故障复位
    U32   GetGblFault(void) const;                  // 获取全局故障信息

};
//----------------------------------------------------------------------------------------------------
dev_ext     Dev     cDev;       // 对象 -> 设备

//----------------------------------------------------------------------------------------------------
#endif  // #ifdef   __cplusplus
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
    extern "C" 
    {
#endif
//--------------------------------------------------
dev_ext     U8      TstDevExfun(U32);
dev_ext     void    SRAM_Test(void);
dev_ext     void    IP_Reset_Test(void);    // 16-0602
dev_ext     void    Init_Dev(void);


//--------------------------------------------------
#ifdef      __cplusplus
    }
#endif
//----------------------------------------------------------------------------------------------------
#endif
