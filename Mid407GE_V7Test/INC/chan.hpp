//----------------------------------------------------------------------------------------------------
#ifndef __CHAN_HPP
    #define __CHAN_HPP

#include    "main.h"
#include    "RTX_CAN.h"
//----------------------------------------------------------------------------------------------------
//C++类定义
//-------------------------------------------------------
#ifdef      _IN_CHAN_CPP
    #define     chan_ext
#else
    #define     chan_ext    extern
#endif
//----------------------------------------------------------------------------------------------------
//  RUNDATA.VER
#define     RUNDATA_HMARK           (0x5A00)        // 高字节，初始化标记
#define     RUNDATA_TOTSTEPMARK     (0x01<<7)       // 低字节，运行标记 - 累计工步开始计数标记

//----------
#define     MAX_ELSE                (4)             // 最多连续的‘否则’工步数
//----------
#define     HAVE_NULL               (0x0000)        // 还没有执行充放工艺（充放循环用）
#define     HAVE_CHARGED            (0x5555)        // 最近充放工艺为充电（充放循环用）
#define     HAVE_DISCHARGED         (0xAAAA)        // 最近充放工艺为放电（充放循环用）
//----------------------------------------------------------------------------------------------------
//--------------------------------------------------
//  设定与反馈
//--------------------------------------------------
typedef struct  _S32RefFdb_ 
{
    S32     ref;
    S32     fdb;
    
}   S32RefFdb;
//--------------------------------------------------
typedef struct  _U32RefFdb_  
{
    U32     ref;
    U32     fdb;
    
}   U32RefFdb;
//--------------------------------------------------
typedef struct  _F32RefFdb_
{   
    float   ref;
    float   fdb;
    
}   F32RefFdb;
//----------------------------------------------------------------------------------------------------
//  实时数据
//----------------------------------------------------------------------------------------------------
//--------------------------------------------------
//  其它扩展项位定义（参考【联机】及【修改设备信息】命令）
//--------------------------------------------------
#define     XITEM_POW           (0)     // 功率
#define     XITEM_RES           (1)     // 电阻
#define     XITEM_CACC          (2)     // 累计容量
#define     XITEM_EACC          (3)     // 累计能量
#define     XITEM_PILOT_VI      (4)     // 领示电压
#define     XITEM_MIN_VI        (5)     // 最小单体电压
#define     XITEM_MAX_VI        (6)     // 最大单体电压
#define     XITEM_PILOT_TI      (7)     // 领示温度

#define     XITEM_MIN_TI        (8)     // 最小单体温度
#define     XITEM_MAX_TI        (9)     // 最大单体温度
#define     XITEM_VBUS          (10)    // 母线电压
#define     XITEM_VEX1          (11)    // 扩展电压1(一般主通道采样)
#define     XITEM_VEX2          (12)    // 扩展电压2(一般主通道采样)
#define     XITEM_VSLOPE        (13)    // 电压斜率
#define     XITEM_SWIN          (14)    // 开关量输入
#define     XITEM_SWOUT         (15)    // 开关量输出

#define     XITEM_YEAR          (16)    // 实时时间-年
#define     XITEM_MON           (17)    // 实时时间-月
#define     XITEM_DAY           (18)    // 实时时间-日
#define     XITEM_HOUR          (19)    // 实时时间-时
#define     XITEM_MIN           (20)    // 实时时间-分
#define     XITEM_SEC           (21)    // 实时时间-秒
#define     XITEM_RECCNT        (22)    // 存储记录号
#define     XITEM_LERR          (23)    // 下位机故障位
#define     XITEM_TEX1          (24)    // 扩展温度1(一般主通道采样)
#define     XITEM_TEX2          (25)    // 扩展温度2(一般主通道采样)
#define     XITEM_I_INST        (26)    // 电流瞬时值
#define     XITEM_V_INST        (27)    // 电压瞬时值
#define     XITEM_CSTP          (28)    // 过程（阶段）容量 16-0531

#define     XITEM_B29           (29)    // 
#define     XITEM_B30           (30)    // 
#define     XITEM_B31           (31)    // 
//------------------------------------------------------------------------------------------------
//--------------------------------------------------
//  CAN 实时扩展数据索引 15-1126
//--------------------------------------------------
#define     EX_INX_VBUS         (0)     // 实时母线电压
#define     EX_INX_SYSTMP       (1)     // 系统温度(传感器温度等)
#define     EX_INX_REALT1       (2)     // 实时温度1
#define     EX_INX_REALT2       (3)     // 实时温度2
#define     EX_INX_FDBCUR       (4)     // 反馈电流瞬时值
#define     EX_INX_FDBVOL       (5)     // 反馈电压瞬时值
#define     EX_INX_REFCUR       (6)     // 参考电流
#define     EX_INX_REFVOL       (7)     // 参考电压
#define     EX_INX_VEXT1        (8)     // 实时扩展电压1
#define     EX_INX_VEXT2        (9)     // 实时扩展电压2
//--------------------------------------------------
#define     EXT_REALDATA_NUM    (EX_INX_VEXT2)      // 上传扩展实时数据个数（辅助显示/测试/诊断等）
//--------------------------------------------------
//  需要记录的实时数据   15-1215(已经去掉单体电压，温度，其他项数)
//--------------------------------------------------
typedef struct _RealData_ 
{                           //offset
    U16     Mark;           //00    标记
    U16     Step;           //02    工步号(阶段) 0x01开始，工艺'否则'行号为0x00，区别于工艺行号，一般工艺行号>=工步数
    U16     Mode;           //04    工作模式 U8 -> U16
    U16     AbnCode;        //06    故障暂停代码 U8 -> U16
    
    U32     TotStp;         //08    总累计工步
    U32     CycNum;         //12    充放循环数（参数操作‘10’）
    
    U16     StpDay;         //16    过程时间天数
    U16     TotDay;         //18    总时间天数
    
    U32     StpMs;          //20    过程时间毫秒
    U32     TotMs;          //24    总时间毫秒

    float   Vol;            //28    电压(mV)
    float   Cur;            //32    电流(mA)
    float   Engy;           //36    能量(mWH)
    float   Cap;            //40    容量(mAH)
    float   Temp;           //44    温度(℃)
    
    U16     Rev;            //48    其他项 U8 -> U16
    
    U16     Crc;            //50    校验
    
}   RealData;
//--------------------------------------------------
//  不需要记录的实时数据
//--------------------------------------------------
typedef struct _NrRealData_ 
{
    U8          PreWkStat;          // 通道之前运行状态机
    U8          CurWkStat;          // 通道当前运行状态机
    U8          NxtWkStat;          // 通道下步运行状态机

    U8          ElseNum;            // '否则'判读计数

    U16         PreWkLine;          // 通道之前运行工艺行    0x00开始，连续，不允许ELSE行的索引
    U16         CurWkLine;          // 通道当前运行工艺行    0x00开始，连续，不允许ELSE行的索引
    U16         NxtWkLine;          // 通道下步运行工艺行    0x00开始，连续，不允许ELSE行的索引
    U16         JumpFlag;           // 工艺跳转标记
    
    U8          CabCtrlFlag;        // 柜体控制量标记字节（柜体结构相关）
    U8          FullFlag;           // 记录缓冲溢出标记
    U16         ReadPoint;          // 读记录缓冲指针
    U16         WritePoint;         // 写记录缓冲指针
    U16         RdPtrTemp;          // 读记录缓冲指针暂存(作为更新读记录指针用)

    U32         RecNum;             // 记录数  -> 暂作测试用
    U16         bChDchMark;         // 充放标记 0x55-先前有充电，0xAA-先前有放电 0-无充放
    U16         PdAbnBak;           // 掉电时的异常备份，退出掉电延时后清零 15-0404
    U32         LowerStat;          // 来自下位机故障状态
    U16         AbnCodeBak;         // 实时数据的故障备份保存

    U16         VT1_mS;             // 时间变量1毫秒计数
    U32         V_Time1;            // 时间变量1(100mS分辨率)
    float       V_Cap1;             // 容量变量1
    U32         V_Cyc[5];           // 循环变量
    float       Pow;                // 功率
    float       Res;                // 电阻
    float       CapStp;             // 阶段容量 16-0531    
    float       CapAcc;             // 累计容量 15-1114
    float       EngyAcc;            // 累计能量 15-1114
    
//    float           Temp;           // 温度
    float       Slope;              // 电压斜率
    float       MinusSlope;         // 电压负斜率
    
    float       MaxVolt;            // 最大过程电压 16-0716 用于判断充电电压跌落
    float       MinVolt;            // 最小过程电压 16-0716 用于判断充电电压跌落
    
    float       MaxIi;              // 最大电流设定
    float       MinIi;              // 最小电流设定
    S32RefFdb   MaxVi;              // 最大单体电压设定与反馈
    S32RefFdb   MinVi;              // 最小单体电压设定与反馈
    S32RefFdb   MaxTi;              // 最大单体温度设定与反馈
    S32RefFdb   MinTi;              // 最小单体温度设定与反馈

}   NrRealData;
//----------------------------------------------------------------------------------------------------
//#define     REALDATA_LEN        (sizeof(RealData))  // 实时数据长度 52
#define     PROCDATA_LEN        (38)                // 过程数据长度
//----------------------------------------------------------------------------------------------------
//  记录(明细)数据
//----------------------------------------------------------------------------------------------------
typedef struct _RecData_ 
{
    U16         RecMark;    // 本明细的标志(b6:4)及工作状态(b3:0)
    RealData    RData;      // 具体记录内容
    
}   strRecData;
//----------
#define     REC_LEN                 (sizeof(strRecData))    // 记录部分长度 56
#define     FIXED_DATA_LEN          (51)                    // 上传实时或明细固定部分长度 从【通道号】到【其他项】
#define     MAX_SENDPACK_LEN        (1300)                  // 最大封装长度
#define     MAX_SENDREC_NUM         (250)                   // 最大单包记录长度

#define     VI_SIZE                 (2)                     // 单体电压 数据字节长度
#define     TI_SIZE                 (2)                     // 单体温度 数据字节长度
#define     II_SIZE                 (4)                     // 输入 数据字节长度
#define     OI_SIZE                 (4)                     // 输出 数据字节长度
#define     XI_SIZE                 (4)                     // 其他项 数据字节长度

#define     MAX_TEST_VINUM          MAX_CHANVI_NUM          // (240) 最大测试单体电压数
#define     MAX_TEST_TINUM          MAX_CHANTI_NUM          // (120) 最大测试单体温度数
#define     MAX_TEST_IINUM          MAX_CHANII_NUM          // (40)  最大测试输入数
#define     MAX_TEST_OINUM          MAX_CHANOI_NUM          // (40)  最大测试输出数
#define     MAX_TEST_XINUM          MAX_CHANXI_NUM          // (16)  最大测试其他项数 15-1112
//--------------------------------------------------
typedef struct _RecBackUp_ 
{
    U32     Mark;           // 标记
    U16     RecCnt;         // 翻滚计数器 (0：无效)
    U16     Rev;            // 保留
//    U8      Buf[0x100];     // 保留
}   strRecBackUp;
//----------------------------------------------------------------------------------------------------
//  工步结构
//----------------------------------------------------------------------------------------------------
//  普通工艺 单步（44字节）
//-----------------------
typedef struct _strStpPrg_ 
{
    U16         No;             // 工步序号
    U16         Mode;           // 工作模
    U32         Mpar1;          // 主参数1 U16->U32 15-1209
    Union32     Mpar2;          // 主参数2 float->UF32
    U16         Pv_type1;       // 参数或变量类型1
    U16         Pv_opt1;        // 参数或变量操作符1
    Union32     Pv_val1;        // 参数或变量值1
    U16         Pv_type2;       // 参数或变量类型2
    U16         Pv_opt2;        // 参数或变量操作符2
    Union32     Pv_val2;        // 参数或变量值2
    U32         Delta_t;        // 时间差
    float       Delta_u;        // 电压差
    float       Delta_i;        // 电流差
    U16         Stp_act;        // 阶段结束动作 0:下一步，0xffff:结束, 其他：目标阶段
    U16         Rev16a;         // 保留
    
} strStpPrg;
//-----------------------
//  AND工艺 单步（44字节）15-1125
//-----------------------
//  脉冲+AND
typedef struct _strPulseAndPrg_ 
{
    S32         Cur2;           // 第二电流
    S32         VolLmt;         // 限制电压
    S32         Tidle1;         // 间歇1时间
    S32         Tpw1;           // 第一脉宽
    S32         Tpw2;           // 第二脉冲
    S32         Tupc;           // 充电上升沿   
    S32         Tdnc;           // 充电下降沿
    S32         Tidle2;         // 间歇1时间
    
} strPulseAndPrg;
//--------------------------------------------------
//  全局工艺数据结构
typedef struct _strGblPrg_ 
{                               //offset
    U16         Start_mode;     // 启动方式
    U16         rev16a;         // 保留
    U8          Pdwk_mode;      // 掉电恢复模式(手自动) 
    U8          DeProtect;      // 首过程是否忽略保护 
    U16         Tst_ViNum;      // 测试单体数
    U16         Tst_TiNum;      // 测试温度数
    U16         Tst_XiNum;      // 测试其他项

    U32         Init_phase;     // 充电初期时间宽度(ms)
    U32         Init_rec_t;     // 充电初期时间记录间隔（ms）
    U32         Prot_time;      // 保护延时时间(mS)

    Union32     V_pot1;         // 定点电压1(mV)
    Union32     V_pot2;         // 定点电压2(mV)
    Union32     V_pot3;         // 定点电压3(mV)

    Union32     Ov_val;         // 过压保护值
    Union32     Uv_val;         // 欠压保护值
    Union32     Ot_val;         // 超温保护值
    Union32     Ut_val;         // 低温保护值
    Union32     Ccap_val;       // 充电容量保护值
    Union32     Dcap_val;       // 放电容量保护值
    Union32     Ovi_val;        // 单体电压过压保护值
    Union32     Uvi_val;        // 单体电压欠压保护值
    Union32     Oti_val;        // 单体超温保护值
    Union32     Uti_val;        // 单体低温保护值

}   strGblPrg;
//--------------------------------------------------
//续接启动工艺 参数
typedef struct _strConPrg_ 
{
    U32         V_Cyc1;
    U32         V_Cyc2;
    U32         V_Cyc3;
    U32         V_Cyc4;
    U32         V_Cyc5;
    float       V_Cap1;
    U32         V_Day;
    U32         V_Ms;
    U32         CycNum;
    U32         TotStp;
    U16         Stp;
    U16         rev16a;
    U32         TotDay;
    U32         TotMs;
    float       Cap;
    float       Engy;

}   strConPrg;
//--------------------------------------------------
//  页行信息数据结构
typedef struct _strPageLine_ 
{   
    U8          TotPage;        // 总页数
    U8          PageNo;         // 页序号
    U16         TotLine;        // 总行数
    U32         PageLine;       // 页行数(当前)

}   strPageLine;
//--------------------------------------------------
//整体工艺结构
typedef struct _strPrg_ 
{
    strGblPrg       global;             // 全局保护
    U32             TotLine;            // 工艺总行数    (含‘否则’)
    strStpPrg       step[MAX_WKLINE];   // 具体工步参数

}   strPrg;
//--------------------------------------------------
//启动命令数据头（不含阶段工艺）结构
typedef struct _strStartHead_ 
{
    U8              ch[32];         // 通道位标记
    strConPrg       conti;          // 续接参数
    strGblPrg       global;         // 全局保护
    strPageLine     pageline;       // 页行信息

}   strStartHead;
//--------------------------------------------------
//启动命令完整结构
typedef struct _strStartCmd_ 
{
//    strStartHead    head;               // 启动数据结构头
    U8              ch[32];             // 通道位标记
    strConPrg       conti;              // 续接参数
    strGblPrg       global;             // 全局保护
    strPageLine     pageline;           // 页行信息
    strStpPrg       step[MAX_WKLINE];   // 具体工步参数

}   strStartCmd;
//--------------------------------------------------
//  工艺接收控制数据结构
//--------------------
//  接收状态码 (RcvStat)
#define     RECV_IDLE           (0x00)          // 接收空闲，允许接收工艺
#define     RECV_GOING          (0x01)          // 接收进行中
#define     RECV_END            (0x02)          // 接收结束等待启动，启动后回到空闲
//#define     RECV_WTSTART        (0x03)          // 接收等待启动
//--------------------
//  启动方式 (StartMode) 宏(从协议映射)
#define     NORMAL_START        (0x0000)        // 重新启动
#define     CONTINUE_START      (0xffff)        // 续接启动
#define     REFRESH_START       (0x5a5a)        // 重置方案
//--------------------
//  接收超时
#define     PRG_RCV_TICK        (10*CTRL_1S)    // 工艺接收超时
//--------------------
typedef struct _strPrgRcvCtrl_ 
{
    U8              RcvStat;        // 接收状态标志
    U8              Rev;            // 保留
    U16             ToutCnt;        // 接收超时计数器
    U16             CurPage;        // 已经接收的页数（由接收程序计算）
    U16             CurLine;        // 当前行号     （由接收程序计算,0开始）
    strStartCmd     DataBuf;        // 命令数据缓冲

}   strPrgRcvCtrl;
//--------------------------------------------------
//  时间结构
typedef struct _strTime_ 
{   
    U16     Day;        // 天数
    float   Ms;         // 毫秒数

}   strTime;
//--------------------------------------------------
//  记录判比数据 16-0421  modified
typedef struct _strRecCompData_
{
    U32         Ms;         // 记录毫秒计数
    float       RecVol;     // 当前记录电压
    float       RecCur;     // 当前记录电流    
//    float       MaxVol;     // 当前记录 最大电压
//    float       MinVol;     // 当前记录 最小电压
//    float       MaxCur;     // 当前记录 最大电流
//    float       MinCur;     // 当前记录 最小电流

}   strRecCompData;
//--------------------------------------------------
//  工作参数异常判比计数器
//----------
#define     DELTA_I         (0.05)      // 电流判断默认阈值 +/-5%
#define     DELTA_V         (0.05)      // 电压判断默认阈值 +/-5%
#define     DELTA_T         (0.05)      // 温度判断默认阈值 +/-5%
#define     DELTA_P         (0.05)      // 功率判断默认阈值 +/-5%
#define     DELTA_W         (0.05)      // 能量判断默认阈值 +/-5%
#define     DELTA_VI        (0.05)      // 单体电压判断默认阈值 +/-5%
#define     DELTA_TI        (0.05)      // 单体温度判断默认阈值 +/-5%
//----------
typedef struct _strJdgCnt_
{
    U16     ORateCur_n;     // 过额定电流判断响应计数器
    U16     ORateVol_n;     // 过额定充电电压判断响应计数器
    U16     URateVol_n;     // 欠额定放电电压判断响应计数器
    //-----
    U16     OCur_n;         // 过电流判断响应计数器
    U16     UCur_n;         // 欠电流判断响应计数器 

    U16     OVol_n;         // 过电压判断响应计数器
    U16     UVol_n;         // 欠电压判断响应计数器

    U16     OTemp_n;        // 过温度判断响应计数器
    U16     UTemp_n;        // 欠温度判断响应计数器

    U16     OVi_n;          // 单体过电压响应计数器
    U16     UVi_n;          // 单体欠电压响应计数器

    U16     OTi_n;          // 单体过温度响应计数器
    U16     UTi_n;          // 单体欠温度响应计数器
    //-----
    U16     IO_n;           // 开关量响应计数器
    //-----
    U16     Vpot1_n;        // 平台电压1响应计数器
    U16     Vpot2_n;        // 平台电压2响应计数器
    U16     Vpot3_n;        // 平台电压3响应计数器
    //-----
    U16     Cur_n;          // 电流转换判断响应计数器
    U16     Vol_n;          // 电压转换判断响应计数器
    U16     Temp_n;         // 温度转换判断响应计数器
    U16     Pow_n;          // 功率转换判断响应计数器
    U16     Engy_n;         // 瓦时判断响应计数器
    U16     MaxVi_n;        // 单体电压转换响应计数器
    U16     MinVi_n;        // 单体电压转换响应计数器
    U16     MaxTi_n;        // 单体温度转换响应计数器
    U16     MinTi_n;        // 单体温度转换响应计数器
//-----
    
}   strJdgCnt;
//----------------------------------------------------------------------------------------------------
//  掉电保持数据结构
//--------------------------------------------------
typedef struct _strPdmem 
{
    U32         Dummy;                  // 空一个字不用 14-0707
    U32         Mark;                   // 掉电数据区标记 
    RealData    RunData;                // 记录实时
    NrRealData  NrRData;                // 非记录实时
    
    //以下三个结构须连续（初始化连续清零）
    strPrg      Program;                // 整体工艺
    strGblPrg   RunGblPrg;              // 掉电保持的当前运行全局保护工艺 2012-0417
    strStpPrg   RunStpPrg;              // 掉电保持的当前运行阶段工艺
#ifdef  USE_SUBPRG  // 运行阶段辅助工艺备份 16-0507
    strStpPrg   StpSubPrg[SUB_PRG_NUM]; // 当前阶段后续工艺（若有，最大支持3个，用于‘ELSE’工艺，暂未考虑脉冲）
#endif    
    strRecData  Record[DEF_REC_NUM];    // 注 此数组后不能加数据定义
    
}   strPdMem;
//--------------------------------------------------
//  去除记录后的掉电数据结构，跟随掉电数据结构同步调整，用于计算记录空间，【不实际使用】！
//--------------------
typedef struct _strPdmemExRec_ 
{
    U32         dummy;                  // 空一个字不用
    U32         mark;                   // 掉电数据区标记 
    RealData    runData;                // 记录实时
    NrRealData  nrRData;                // 非记录实时
    
    strPrg      program;                // 整体工艺
    strGblPrg   runGblPrg;              // 掉电保持的当前运行全局保护工艺
    strStpPrg   runStpPrg;              // 掉电保持的当前运行阶段工艺
#ifdef  USE_SUBPRG  // 运行阶段辅助工艺备份 16-0507
    strStpPrg   stpSubPrg[SUB_PRG_NUM]; // 当前阶段后续工艺（若有，最大支持3个，用于‘ELSE’工艺，暂未考虑脉冲）
#endif    
}   strPdmemExRec;
//--------------------------------------------------
//  系统使用的数据结构，用于计算记录空间，【不实际使用】！
//--------------------
typedef struct _strPdmemSys_ 
{
    strRecBackUp    recBackup;                          // 掉电保存的读记录翻滚计数器
    U8              buffer[0x100-sizeof(strRecBackUp)]; // 未满0x100，凑足
//    S16             vol[];            // 单体电压，待扩展
//    S16             tmp[];            // 单体电压，待扩展
}   strPdmemSys;

//----------------------------------------------------------------------------------------------------
#define     PDHDATA_MARK        (0xD1910600 + MAX_CHAN_NUM) // 掉电区标记，周期读取判断，重设此值更新掉电区数据
//-------------------------------------------------------
//  外部扩展RAM，掉电保护 2 MBytes
//-------------------------------------------------------
#define     LSB_ADDR            (0x60000000)                // 外部SRAM起始字节地址
#define     MSB_ADDR            (0x60200000)                // 外部SRAM末字节后地址

#define     REC_BACK_OFFSET     (LSB_ADDR)                  // 存放PC读记录时的备份，供PC记录重传用,须4字节对齐
                                                            // *pRecBackUp指针所指
#define     RAM_CAP             (0x200000)                  // 外部SRAM空间 - 2 MBytes,须4字节对齐
#define     SYS_USED_CAP        sizeof(strPdmemSys)         // 系统使用外部SRAM容量 （下载工艺备份）128K,须4字节对齐
#define     UNREC_CAP           sizeof(strPdmemExRec)       // 通道非记录用掉电区容量,须4字节对齐 
                                                            // PdHoldData -> record[x]前的掉电数据限制在此大小范围内
//-------------------------------------------------------
#if     (MAX_CHAN_NUM > 192)//128) 16-0518
    #error "First CAN should be less than 128 channels!"
#else
    #define     CH_PDH_CAP      ((RAM_CAP - SYS_USED_CAP) / MAX_CHAN_NUM)   // 每个通道使用的总的 RAM 大小。
    #define     RECORD_CAP      (CH_PDH_CAP - UNREC_CAP)                    // 每个通道使用的记录 RAM 大小。 
#endif
//----------------------------------------------------------------------------------------------------
//  阶段启动同步控制数据结构 StepSyncCtrl
//--------------------------------------------------
typedef struct _strSSCtrl
{
    U8      SyncFlag;       // 同步标记
    U8      CurDirect;      // 电流趋势
    U16     TimeOut;        // 超时计数器
//    float   CurBuf;         // 电流备份
    
}   strSSCtrl;
//----------------------------------------------------------------------------------------------------
//  WkStat  WkLine
#define     NXT_MARK                (0x00)          //（设置）下一工艺行 标记
#define     CUR_MARK                (0x01)          //（设置）当前工艺行 标记
#define     PRE_MARK                (0x02)          //（设置）前一工艺行 标记

#define     PRG_RENEW_MARK          (0xafff)        // 有重置工艺标识
#define     PRG_FORCEJP_MARK        (0xbfff)        //（异常，暂停，异常暂停）强制跳阶段
//--------------------
//  工艺行相关 WkLine
//--------------------
//GetWkLine
#define     LINE_ERR                (0x5a5a)        // 取出错误行 
//--------------------
#define     JP_STOP_LINE            (0xffff)        // 跳转工艺结束
#define     JP_NEXT_LINE            (0x0000)        // 跳转到下一行
//--------------------
#define     LINE_FOR_START          (0xfff0)        // 标记行号为（运行启动）
#define     LINE_FOR_CONTINUE       (0xfff2)        // 标记行号为（续接启动）
#define     LINE_FOR_RENEW          (0xfff6)        // 标记行号为（工艺重置恢复）
#define     LINE_FOR_RESUME         (0xfff8)        // 标记行号为（工艺恢复）
//#define     LINE_FOR_PAUSE          (0xfffa)        // 标记行号为（工艺暂停）
#define     LINE_FOR_JPCURRENT      (0xfffc)        // 标记行号为（PC跳转到当前行）
#define     LINE_FOR_STOP           (0xfffe)        // 标记行号为结束（状态结束）
//#define     LINE_FOR_BACK           (0xff00)        //
//----------------------------------------------------------------------------------------------------
//  工艺动作相关
//--------------------
#define     ACTION_OFF              (0x00)          // 不动作
#define     ACTION_ON               (0x01)          // 动作
#define     ACTION_NULL             (0x02)          // 未设定动作
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//  CAN接收扩展数据结构 15-1126 modified
//  第二版扩展数据 16-0415
//--------------------------------------------------
//typedef struct _strCanExData_ 
//{
//#if (DEV_STR_TYPE == DS_PBTESTER)   // 铅酸下位机专用扩展数据 16-0414

//    U8      Step;               // 来自下位机的阶段
//    U8      Mode;               // 来自下位机的方式    
//    U16     ExtErr;             // 来自下位机扩展故障
//    
//    S16     HexCurAvg;          // 0 -电流原码平均值
//    S16     HexVolAvg;          // 1 -电压原码平均值
//    S16     HexDA;              // 2 -输出DA
//    S16     SetHexCur;          // 3 -设定电流原码
//    S16     SetHexVol;          // 4 -设定电压原码
//    U16     ReadIO;             // 5 -IO
//    U16     HardBit;            // 6 -硬件故障位
//    S16     RealT1;             // 7 -温度1
//    S16     RealT2;             // 8 -温度2
//    S16     RealR1;             // 9 -电阻1
//    S16     RealR2;             //10 -电阻2
//    S16     Refh;               //11 -参考高
//    S16     Refl;               //12 -参考低   
//    S16     T1Z;                //13 -温度1零点
//    S16     T2Z;                //14 -温度2零点
//    S16     T1F;                //15 -温度1满度
//    S16     T2F;                //16 -温度1满度
//    U16     SoftVer;            //17-下位机软件版本
//    S16     IRes;               //18-内阻    
//    
//#else
//    U32     RealErr;            // 来自下位机基本故障 15-0717

//    S16     Vbus;               // 母线电压（一位小数）
//    S16     SysTemp;            // 系统温度（一位小数）
//    S16     Temp1;              // 采样温度1（一位小数）
//    S16     Temp2;              // 采样温度2（一位小数）
//    S16     FdbCur;             // 反馈电流（hex）
//    S16     FdbVol;             // 反馈电压（hex）
//    S16     RefCur;             // 参考电流（hex）
//    S16     RefVol;             // 参考电压（hex）
//    S16     ExtVol1;            // 扩展电压1（一位小数）
//    S16     ExtVol2;            // 扩展电压2（一位小数）    
//    S32     InstCur;            // 瞬时电流
//    S32     InstVol;            // 瞬时电压
//    
//#endif

//}   strCanExData;

typedef union   _strCanExData_ 
{
    struct 
    {
        U8      Step;               // 来自下位机的阶段
        U8      Mode;               // 来自下位机的方式    
        U16     ExtErr;             // 来自下位机扩展故障
        
        S16     HexCurAvg;          // 0 -电流原码平均值
        S16     HexVolAvg;          // 1 -电压原码平均值
        S16     HexDA;              // 2 -输出DA
        S16     SetHexCur;          // 3 -设定电流原码
        S16     SetHexVol;          // 4 -设定电压原码
        U16     ReadIO;             // 5 -IO
        U16     HardBit;            // 6 -硬件故障位
        S16     RealT1;             // 7 -温度1
        S16     RealT2;             // 8 -温度2
        S16     RealR1;             // 9 -电阻1
        S16     RealR2;             //10 -电阻2
        S16     Refh;               //11 -参考高
        S16     Refl;               //12 -参考低   
        S16     T1Z;                //13 -温度1零点
        S16     T2Z;                //14 -温度2零点
        S16     T1F;                //15 -温度1满度
        S16     T2F;                //16 -温度1满度
        U16     SoftVer;            //17-下位机软件版本
        S16     IRes;               //18-内阻
        
    }   V1;     // 第一版 铅酸测试机下位机扩展数据
    
    struct
    {
        U32     RealErr;            // 来自下位机基本故障 15-0717

        S16     Vbus;               // 母线电压（一位小数）
        S16     SysTemp;            // 系统温度（一位小数）
        S16     Temp1;              // 采样温度1（一位小数）
        S16     Temp2;              // 采样温度2（一位小数）
        S16     FdbCur;             // 反馈电流（hex）
        S16     FdbVol;             // 反馈电压（hex）
        S16     RefCur;             // 参考电流（hex）
        S16     RefVol;             // 参考电压（hex）
        S16     ExtVol1;            // 扩展电压1（一位小数）
        S16     ExtVol2;            // 扩展电压2（一位小数）    
        S32     InstCur;            // 瞬时电流
        S32     InstVol;            // 瞬时电压
    
    }   V2;     // 第二版 锂电(无扩展，瞬时)，3KGH系列

}   strCanExData;
//----------------------------------------------------------------------------------------------------
//  下位机故障报警控制数据结构 15-1125
//--------------------------------------------------
typedef struct _strLErrCtrl_ 
{
    U8      DisInx;         // 下位机报警显示索引
    U8      Rev0;
    U16     TstDly;         // 下位机报警响应演示计数
    U16     ShiftCnt;      // 下位机报警显示切换计数器 2014-0628
    U16     DlyCnt;        // 下位机报警延时计数器
    
}   strLErrCtrl;

//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
//----------------------------------------------------------------------------------------------------
//类结构定义
//----------------------------------------------------------------------------------------------------
//定义测试机通道类
//----------------------------------------------------------------------------------------------------
class Chan
{
private:
    U8              mAddr;              // 测试机子地址 0 ~  max-1
//    U8              mLErrDisInx;        // 下位机报警显示索引
    U16             mRecFlag;           // 记录要求标记
    U16             mFirstIn;           // 状态首次进入标记
    U32             mStartRunDly;       // 启动延时计数 15-1125
    U16             mStepAction;        // 阶段动作缓冲
    U16             mCanOffLineCnt;     // CAN超时脱机计数器

//    U16             mLowerErrCnt;       // 下位机报警响应计数器
//    U16             mLErrShiftCnt;      // 下位机报警显示切换计数器 2014-0628
//    U16             mLErrDlyCnt;        // 下位机报警延时计数器
    U32             mCntDnTick;         // 倒计时计数器
    U32             mUnitCap;           // 单位容量缓存（mAH）15-0815
    U32             mUnitEngy;          // 单位能量缓存（mWH）15-1116
    strLErrCtrl     mLErrCtrl;          // 下位机故障处理控制

public:
    U8              mSubOffFlag;        // 辅助通道脱机标记（配合工艺处理脱机报警） 16-0311
    OS_SEM          mStpSyncSem;        // 下位机阶段跳阶段后同步信号 15-0717
//  OS_SEM          mDisProtSem;        // 下位机屏蔽保护信号 15-1109

//#if (DEV_STR_TYPE == DS_NEILSBED)   // 针床设备专用信号量    15-0603

    // 不管什么设备结构，都定义针床信号量 16-0514    --> 下一步抽象提取
    OS_SEM          mSmokeSem;          // 烟感报警 信号量 (其他任务送主控任务)
    OS_SEM          mInplaceSem;        // 托盘到位 信号量 (其他任务送主控任务)
    OS_SEM          mRqInplaceSem;      // 请求托盘到位 信号量   (主控任务送其他任务)

//#endif

    strCanCtrl      mCanCtrlData;       // CAN控制数据结构
    strSSCtrl       mSSCtrl;            // 阶段软件同步控制结构
    strCanExData    mCanExData;         // 来自下位机的扩展数据

//  strGblPrg       mGblPrgBak;         // 全局保护工艺备份
//  strStpPrg       mStpPrgBak;         // 阶段工艺备份

    strRecData      mRecDateBuf;        // 记录备份缓冲
    strRecCompData  mRecCompData;       // 记录比较数据
    strJdgCnt       mJdgCnt;            // 参数比较数据

    
    strPdMem        *PdHoldData;
public:
    Chan();
    inline  U8  ChanBreak(void);            // 通道断点测试
    U8          Test_PdMark(void);          // 掉电区标记判断
    void        Init_PdMark();              // 掉电区标记初始化
    U8          Test_PdData(void);          // 掉电区数据判断
    void        Init_PdData(void);          // 掉电区数据初始化
    U8          Test_WkStat(U8 mark);       // 是否是运行态
    void        Clear_Program(void);        // 清零工艺缓冲
    void        Init_WkData(void);          // 运行控制常规上电初始化
    U8          Test_PdResumeMode(void);    // 掉电自动恢复模式判断
    void        Set_PdWkState(void);        // 运行掉电，上电控制状态设定
    void        Init_PdCntDn(void);         // 掉电态倒计时初始化
    void        SetTiNum(U8 num);           // 设置温度数

    //----- Ether -----
    U8          ManualCtrl(U8 *ptr);                // PC干预命令
    U8          Save_Program(void);                 // 保存工艺
    void        Save_RenewStp(U16 obj_stp);         // 保存重置后跳转目标阶段
    U8          PrgRcvStartCtrl(U16 start_mode);    // 工艺接收启动控制

    void        CoverReadPoint(void);               // 覆盖读记录指针暂存到真正的读记录指针
    void        RecoverReadPoint(void);             // 恢复真正的读记录指针到读记录指针暂存
    U16         GetLoadPackLen(U16 f_len) const;    // 获取当前上传记录长度
    U16         GetUnReadRecNum(void) const;        // 获取上位机未读记录个数
    U8          GetReadRecType(void) const;         // 获取当前读出的记录类型
    U16         SetDummyRealDate(U8 *ptr, U8 abn);  // 有故障时，虚拟拷贝一组数据给PC
    U16         SetRealDate(U8 *ptr);               // 拷贝实时数据
    void        SetXitem(U8 *ptr, U32 inx);         // 拷贝其他项数据 15-1116
    U16         SetOneRec(U8 *ptr);                 // 拷贝单条记录
    U16         SetProcRec(U8 *ptr);                // 拷贝过程记录
//  U16         SetRecData(U16 max_len);            // 拷贝(限定字节长度)记录

    U16         RdRate(U8 *ptr);                    // 读通道额定
    U8          WrRate(U8 *ptr);                    // 写通道额定
    U16         RdCalib(U8 *ptr);                   // 读校准参数
    U8          WrCalib(U8 *ptr);                   // 写校准参数

    //----- CAN -----
    U8          CanOffLineTest(S32 cnt);            // CAN脱机判断
    void        ClrCanCtrlData(U8 *);               // 清下位机控制数据
    void        ClrCanExData(void);                 // 清下位机扩展数据
    void        CanRecvProc(U8 cmd, S32 *data);     // 下位机控制数据接收解析
    void        CanSendProc(U8, U8, CAN_msg *);     // 载入下位机控制通道数据
    strCanCtrl  GetCtrlData(void) const;            // 获得通道控制数据
    U8          GetOutBit(U8);                      // 获取下位机开关量输出数据 16-0514

    U8          Get_Sync(void) const;               // 获取同步信号
    void        Set_Sync(S32 val);                  // 设置同步信号

    U16         Test_RealAbn(void);                 // 获取通道实时异常数据
    void        Set_RealAbn(U16 val);               // 保存实时异常数据
    
    void        Set_RealCur(float val);             // 保存浮点实时电流数据 16-0708
    void        Set_InstCur(S32 val);               // 保存瞬时电流数据
    void        Set_RealVol(float val);             // 保存浮点实时电压数据 16-0708
    void        Set_InstVol(S32 val);               // 保存瞬时电压数据
    
    S32         GetSimSetVol(void);                 // 获取模拟设定电压数据（取保护电压中间值）
    void        Set_RealTemp(S16 val);              // 保存实时温度数据
    void        Set_RealPow(void);                  // 保存实时功率数据
    void        Set_RealRes(void);                  // 保存实时电阻数据
    void        Set_RealEngy(float val);            // 保存实时电阻数据
    void        Set_RealMVi(S32 val);               // 保存单体电压极值
    void        Set_RealMTi(S32 val);               // 保存单体温度极值
    void        Set_Stat(U32 val);                  // 保存下位机故障
    void        Set_ExData(S32 rda, S32 rdb);       // 保存下位机扩展数据
    void        Get_ViTiExtremum(void);             // 获取通道单体极值

    //---------- REC ----------
    void        RecCompDataInit(void);
//  U8          Read_Record(U8 rec_num);            // PC 读记录  
    U8          Save_Record(U8 rec_mode);           // 保存记录

    //---------- WORKING ----------
    U8          State_Init(U8 oldstate, U8 newstate);   // 工作状态初始化
    void        State_Exit(U8 curstate);

    void        RunCtrlProc(void);                      // 通道运行态控制
    U8          Step_Init(U16 oldline, U16 newline);    // 取工艺初始化
    void        Run_RealDataCalc(void);                 // 运行实时数据计算
    void        ParVar_Set(U16, U8, U8, float);         // 变量或参数初始化赋值
    void        ElsePrg_Set(U16);                       // 阶段工艺 附带是'否则'的连续设定。参数为模式（新 16-0510）
    
#ifdef  USE_SUBPRG  // 运行阶段辅助工艺备份（直接从工艺备份包中去脉冲AND工艺数据） 16-0507
    U8          AndPrg_Set(U16, strCanCtrl *);          // 脉冲阶段工艺扩展设定(固定数据结构)
#else
    U8          AndPrg_Set(U16, U16, strCanCtrl *);     // 脉冲阶段工艺扩展设定(固定数据结构)
#endif

    U8          ParVar_Limit(U16, U8, U8, float);       // 参数或变量限制判断
    U8          ParVar_Operate(U8, U8, float);          // 参数或变量操作
    
#ifdef  USE_SUBPRG  // 运行阶段辅助工艺备份 16-0507
    U8          IfElsePrg_Limit(U16 &, U8);             // 主模式为IF的ELSE判断 16-0507
    U8          ElsePrg_Limit(U8, U16 &, U8);           // 阶段工艺 附带是'否则'的连续判断 16-0507 按主模式及辅助模式 16-0725加入跳转目标阶段引用 
#else
    U8          ElsePrg_Limit(U16, U16 &, U8);          // 阶段工艺 附带是'否则'的连续判断 16-0507 加入主模式参数
#endif

    void        Set_WkLine(U8, U16);                    // 更新工艺行行号
    void        Set_JpFlag(U16 flag);                   // 设定跳转标记
    U16         Get_WkLine(U16);                        // 根据工作工步获取所在工艺行索引
    U8          Get_WkMode(void);                       // 获取运行工艺备份中的工作模式
    U8          Check_GlobalProg(strGblPrg *);          // 检查全局工艺参数合法性
    U8          Check_StepProg(const strStpPrg *);      // 检查过程工艺参数合法性
    U8          Check_CanCtrl(strCanCtrl *);            // 检查CAN控制工艺参数合法性    
    void        ClrJdgCnt(void);                        // 参数判比计数器清零
    U16         LowerStatMapping(U32 stat);             // 下位机故障映射
    U8          AbnorReMapping(U32 stat, U8 abnor);     // 下位机故障重映射

    void        PowerOnCtrlProc(void);                  // 通道上电态控制
    void        StopCtrlProc(void);                     // 通道停止态控制
    void        Stop_Init(void);                        // 通道停止初始化（清零相关实时数据）
    void        Clr_Prog(void);                         // 结束清除工艺
    void        PauseCtrlProc(void);                    // 通道暂停态控制
    void        XPauseCtrlProc(void);                   // 通道异常暂停态控制
    void        AbnorCtrlProc(void);                    // 通道异常态控制

    //----- SYS -----
//#if     (DEV_STR_TYPE == DS_NEILSBED)   // 针床设备专用信号量处理  15-0603
    // 不管什么设备结构，都定义针床信号量 16-0514
    void            SmokeSemSend(void);                     // 发送烟感报警信号量
    void            InPlaceSemSend(void);                   // 发送针床托盘到位信号量
//#endif

    void            RecPtrRst(void);                        // 清除记录指针  
    void            TickControlProc(void);                  // 节拍控制处理
    void            Init_RunData(void);
    inline  U8      Get_CabCtrlFlag(void)   {return (PdHoldData->NrRData.CabCtrlFlag);};
    inline  U8      Get_CurStatus(void)     {return (PdHoldData->NrRData.CurWkStat);};
    inline  U8      Get_RealAbn(void)       {return (PdHoldData->RunData.AbnCode & 0xff);};
    inline  float   Get_RealTmp(void)       {return  PdHoldData->RunData.Temp;};
    inline  void    Clr_RealAbn(U8 code)    {if(code==(PdHoldData->RunData.AbnCode&0xff))PdHoldData->RunData.AbnCode=0;};
    U32             Get_PdStatus(void) const;
    U8              Is_Running(void);                       // 设备运行态判断（运行、暂停、异常暂停）

};
//----------------------------------------------------------------------------------------------------
chan_ext    Chan    cChan[MAX_CHAN_NUM];
//----------------------------------------------------------------------------------------------------
#endif  // #ifdef   __cplusplus
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
    extern "C" 
    {
#endif
//--------------------------------------------------
chan_ext    strPrgRcvCtrl   PrgRcvCtrl;         // 工艺接收控制
chan_ext    strRecBackUp    *pRecBackUp;        // 上传的记录备份（掉电保存）

//-------------------------------------------------- 
chan_ext    void    ChanRecPtrRstProc(void);        
chan_ext    void    ControlProc(void);    
//--------------------------------------------------
#ifdef      __cplusplus
    }
#endif
//----------------------------------------------------------------------------------------------------
#endif
