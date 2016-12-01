//----------------------------------------------------------------------------------------------------
#ifndef     __app_cfg_H
    #define     __app_cfg_H
//----------------------------------------------------------------------------------------------------
#ifdef      IN_APP_LAYER
    #define     app_ext
#else
    #define     app_ext extern
#endif
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//  旧版本测试机通道工作模式

#define     STATICMODE          0x11    // 静置
#define     EXCTRLMODE          0x12    // 外控模式
#define     HLCHARGEMODE        0x25    // 恒流充
#define     HLDISCHARGEMODE     0x2A    // 恒流放
#define     HPCHARGEMODE        0x35    // 恒功率充
#define     HPDISCHARGEMODE     0x3A    // 恒功率放
#define     HRCHARGEMODE        0x45    // 恒电阻充
#define     HRDISCHARGEMODE     0x4A    // 恒电阻放
#define     XPVCHARGEMODE       0x55    // 斜坡电压充
#define     XPVDISCHARGEMODE    0x5A    // 斜坡电压放
#define     XPCCHARGEMODE       0x65    // 斜坡电流充
#define     XPCDISCHARGEMODE    0x6A    // 斜坡电流放
#define     XPPCHARGEMODE       0x75    // 斜坡功率充    尚未增加
#define     XPPDISCHARGEMODE    0x7A    // 斜坡功率放    尚未增加
#define     WKCHARGEMODE        0x85    // 温度控制充    尚未增加
#define     WKDISCHARGEMODE     0x8A    // 温度控制放    尚未增加
#define     FCHARGEMODE         0x95    // 反充电
#define     FDISCHARGEMODE      0x9A    // 反放电
#define     PULSECHARGEMODE     0xA5    // 脉冲充电（间歇脉冲）
#define     PULSECH_DCHMODE     0xAA    // 脉冲充放电（正负脉冲） 

#define     HYCHARGEMODE        0xB5    // 恒压充  //2014-1010 added
#define     HYDISCHARGEMODE     0xBA    // 恒压放

#define     ENDMODE             0x00    // 结束模式
//----------------------------------------------------------------------------------------------------
//  旧版本测试机下位机故障
//  以下低12位报警  可不停机 有中位机决定 
//  2014 - 0628 断欠流合并 0x0002

#define     Err_ZL          0x0001      // 下位机未启动有流
//#define     Err_DL          0x0002      // 下位机断流
#define     Err_DQL         0x0002      // 下位机断欠流
#define     Err_CL          0x0004      // 下位机过流
//#define     Err_QL          0x0006      // 下位机欠流
#define     Err_PRG         0x0008      // 下位机控制数据错误
#define     Err_HL          0x0010      // 下位机不恒流
#define     Err_HY          0x0020      // 下位机不恒压
#define     Err_GQY         0x0040      // 下位机过欠压
#define     Err_GR          0x0080      // 下位机散热器过热
#define     Err_GRL         0x0100      // 下位机散热器过热5分钟 硬件保护
#define     Err_VFZ         0x0200      // 下位机电压极性反
#define     Err_GY          0x0400      // 下位机保护过压
#define     Err_GL          0x0800      // 下位机保护过流
#define     Err_CUR         0x1000      // 下位机电流校准参数丢失故障
#define     Err_VOL         0x2000      // 下位机电压校准参数丢失故障
#define     Err_PID         0x4000      // 下位机电流PID参数丢失故障
#define     Err_DEV         0x8000      // 下位机设备额定参数丢失故障

#define     Err_Par         (Err_CUR|Err_VOL|Err_PID|Err_DEV)   // 参数错误
//------------------------------------------------------------
//  下位机扩展故障
#define     Err_BUSOV       0x00010000      // 下位机母线过压
#define     Err_BUSUV       0x00020000      // 下位机母线欠压
#define     Err_SENOT       0x00040000      // 下位机传感器高温
#define     Err_SENUT       0x00080000      // 下位机传感器低温
#define     Err_CCCVS       0x00100000      // 下位机恒流充电负斜率
#define     Err_DCCVS       0x00200000      // 下位机恒流放电正斜率
#define     Err_SYSBUSY     0x00400000      // 下位机系统忙--独立看门狗溢出(无后台程序运行超时)或中断消息超时
#define     Err_DRV         0x00800000      // 下位机驱动过流
#define     Err_BUSFB       0x01000000      // 下位机母线倒灌（母线电压小于电池电压）
#define     Err_BUS         0x02000000      // 下位机母线校准参数错
#define     Err_CAP         0x04000000      // 下位机电容校准参数错
#define     Err_BUSFREQ     0x08000000      // 下位机母线频繁故障(不自动恢复)
#define     Err_DFZ         0x10000000      // 下位机电流线反极 15-1202
#define     Err_VBK         0x20000000      // 下位机采样线掉线 15-1202
//  未定义备用扩展故障

#define     EXE_BIT30       0x40000000      //
#define     EXE_BIT31       0x80000000      //
//------------------------------------------------------------
#define     MAX_LERR_NUM    32              // 当前下位机故障数目
//----------------------------------------------------------------------------------------------------
//  通道工作模式定义

#define     MODE_INIT           0x00        // 开机初始化
#define     MODE_CCC            0x01        // 恒流充电
#define     MODE_CVC            0x02        // 恒压充电
#define     MODE_CCD            0x03        // 恒流放电
#define     MODE_CVD            0x04        // 恒压放电
#define     MODE_CPD            0x05        // 恒功率放电
#define     MODE_CPC            0x06        // 恒功率充电
#define     MODE_CRD            0x07        // 恒电阻放电
#define     MODE_STATIC         0x08        // 搁置
#define     MODE_PUL_C          0x09        // 脉冲充电
#define     MODE_PUL_D          0x0a        // 脉冲放电
#define     MODE_PUL_STATIC     0x0b        // 脉冲搁置
#define     MODE_LINEAR_D       0x0c        // 线性放电
#define     MODE_IF             0x0d        // 如果
#define     MODE_ELSE           0x0e        // 否则
#define     MODE_VAR_OP         0x0f        // 变量操作
#define     MODE_AND            0x10        // 与

#define     MODE_PUL_JX         0x20        // 间歇脉冲
#define     MODE_PUL_ZF         0x21        // 正负脉冲（属于充电）
#define     MODE_EXT_CTRL       0x22        // 外控工艺（配合酸循环）
#define     MODE_CRC            0x23        // 恒电阻充电 16-0422 added

#define     MODE_END            0xff        // 运行结束

#define     MODE_LOWERERR       0xfe00      // 来自下位机故障标记,虚拟
                                            // 用于判断是中位机还是下位机导致故障的，去掉故障时使用 2014-0628
//----------------------
#define     MIN_ERR_MODE       (MODE_PUL_ZF+1)  // 最小错误模式代码
#define     MAX_ERR_MODE       (MODE_END-1)     // 最大错误模式代码
//----------------------------------------------------------------------------------------------------
//  实时数据明细(记录)数据区分位
#define     REALDATA_FLAG       ((unsigned char)(0 << 7))  // 实时数据标记
#define     RECDATA_FLAG        ((unsigned char)(1 << 7))  // 明细数据标记
//----------------------------------------------------------------------------------------------------
//  记录标志

#define     FIRST_REC           ((unsigned char)0)         // 首记录标志
#define     LAST_REC            ((unsigned char)1)         // 尾记录标志
#define     PLAT1_REC           ((unsigned char)2)         // 平台记录1标志
#define     PLAT2_REC           ((unsigned char)3)         // 平台记录2标志
#define     PLAT3_REC           ((unsigned char)4)         // 平台记录3标志
#define     FINISH_REC          ((unsigned char)5)         // 测试完成标志
#define     PROC_REC            ((unsigned char)6)         // 过程记录标志
#define     NORMAL_REC          ((unsigned char)7)         // 普通记录标志

#define     FIRST_VAR_REC       ((unsigned char)8)         // 首变量记录
#define     LAST_VAR_REC        ((unsigned char)9)         // 尾变量记录

//----------------------------------------------------------------------------------------------------
//  通道工作状态定义

#define     WORKSTAT_INIT       0x00            // 初始化状态 （不判断故障，不接受PC干预命令）
#define     WORKSTAT_STOP       0x01            // 停止状态
#define     WORKSTAT_RUN        0x02            // 运行状态
#define     WORKSTAT_PAUSE      0x03            // 暂停状态 （运行时人工暂停）
#define     WORKSTAT_XPAUSE     0x04            // 异常暂停状态 （运行时有异常）
#define     WORKSTAT_ABNOR      0x05            // 异常状态 （非运行时有异常）

#define     MIN_ERR_WKSTAT      (WORKSTAT_ABNOR+1)    // 最小错误工作态序号

//#define     WORKSTAT_POWDN      (0x06)                // 掉电状态 （运行时掉电）
//#define     MIN_ERR_WKSTAT      (WORKSTAT_POWDN+1)    // 最小错误工作态序号
//----------------------------------------------------------------------------------------------------
//  通道异常定义

#define     ABNOR_NULL          0x00        // 无异常

#define     ABNOR_OV            0x01        // 过压
#define     ABNOR_UV            0x02        // 欠压
#define     ABNOR_OC            0x03        // 过流
#define     ABNOR_UC            0x04        // 断欠流
#define     ABNOR_OT            0x05        // 过热
#define     ABNOR_UT            0x06        // 欠温
#define     ABNOR_COM_Vi        0x07        // 单体电压通信异常
#define     ABNOR_COM_Ti        0x08        // 温度通信异常
#define     ABNOR_POWEROFF      0x0A        // 掉电
#define     ABNOR_DATAOF        0x0B        // 数据溢出【记录溢出错误】
#define     ABNOR_OP            0x0C        // 超功率
#define     ABNOR_NUM_Vi        0x0D        // 单体电压个数未设置
#define     ABNOR_TotalV        0x0E        // 单体电压和异常
#define     ABNOR_EEPROM        0x0F        // EEPROM数据异常
#define     ABNOR_OVi           0x10        // 单体过压
#define     ABNOR_UVi           0x11        // 单体欠压
#define     ABNOR_OCAP          0x12        // 超容量
#define     ABNOR_STOPDATA      0x13        // 停止后数据未传完【不进入异常状态】
#define     ABNOR_IN_CALIB      0x14        // 校准数据中
#define     ABNOR_LC            0x15        // 漏电流
#define     ABNOR_POLAR         0x16        // 反极
#define     ABNOR_OUV           0x17        // 过欠压
#define     ABNOR_CV            0x18        // 不恒压
#define     ABNOR_CC            0x19        // 不恒流
#define     ABNOR_CTRLDAT       0x1A        // 控制数据错误
#define     ABNOR_REV           0x1B        // REV
#define     ABNOR_SYSCUR        0x1C        // 电流校准数据错误      // 母线欠压 
#define     ABNOR_SYSVOL        0x1D        // 电压校准数据错误      // 传感器高温 
#define     ABNOR_SYSPID        0x1E        // PID及相关数据错误     // 传感器低温
#define     ABNOR_SYSDEV        0x1F        // 系统数据错误

//  扩展 工作异常定义
#define     ERR_GETWKLINE       0x20        // 获取工艺行号错误
#define     ERR_PARVAR          0x21        // 非法变量或参数
#define     ERR_OPT             0x22        // 非法操作符
#define     ERR_NOLIMIT         0x23        // 限制条件未设定
#define     ERR_STPPRG          0x24        // 阶段工艺检查错误
#define     ERR_GBLPRG          0x25        // 全局工艺数据错误
#define     ERR_ELSENUM         0x26        // 否则超数
#define     ERR_MODE            0x27        // 未知工作模式
#define     ERR_CCMAX           0x28        // 超最大充电电流
#define     ERR_DCMAX           0x29        // 超最大放电电流
#define     ERR_CUMAX           0x2A        // 超最大充电电压
#define     ERR_DUMIN           0x2B        // 超最小放电电压
#define     ERR_CPMAX           0x2C        // 超最大充电功率
#define     ERR_DPMAX           0x2D        // 超最大放电功率
#define     ERR_STPINFINITE     0x2E        // 阶段跳转死循环

//-----
#define     ERR_MCANOFFLINE     0x30        // 主控CAN脱机
#define     ERR_SCANOFFLINE     0x31        // 辅助CAN脱机
#define     EER_LOWER_NORMAL    0x32        // 下位机普通故障（下位机不暂停）
#define     EER_LOWER_FATAL     0x33        // 下位机重要故障（下位机暂停或不启动）
#define     EER_SYNC            0x34        // 阶段启动时与下位机同步失败
#define     ERR_PCANOFFLINE     0x35        // 并联CAN脱机 15-1213
#define     ERR_MCANCONNECT     0x36        // 主控CAN联机错误

//  下位机的扩展异常
#define     ABNOR_BUSOV         0x40        // 母线过压
#define     ABNOR_BUSUV         0x41        // 母线欠压
#define     ABNOR_SENOT         0x42        // 传感器高温
#define     ABNOR_SENUT         0x43        // 传感器低温
#define     ABNOR_CCCVS         0x44        // 恒流充电电压负斜率
#define     ABNOR_DCCVS         0x45        // 恒流放电电压正斜率
#define     ABNOR_SYSBUSY       0x46        // 系统忙（计算溢出）
#define     ABNOR_DRVOC         0x47        // 驱动级过流 15-0622
#define     ABNOR_BUSFB         0x48        // 母线倒灌 15-0808
#define     ABNOR_BUS           0x49        // 母线校准参数错
#define     ABNOR_CAP           0x4A        // 电容校准参数错
#define     ABNOR_BUSFREQ       0x4B        // 母线频繁故障
#define     ABNOR_DFZ           0x4C        // 电流线反极 15-1202
#define     ABNOR_VBK           0x4D        // 采样线掉线 15-1202

#define     ABNOR_BIT30         0x4E        //
#define     ABNOR_BIT31         0x4F        //

//----------
#define     ABNOR_OTi           0x50        // 单体过热
#define     ABNOR_UTi           0x51        // 单体欠温
#define     ABNOR_SMOKE         0x52        // 烟感报警
#define     ABNOR_OFFPLACE      0x53        // 托盘未到位
//----------
#define     ERR_WKSTAT          0x80        // 未知工作态错误
#define     EER_SPACEOVF        0x81        // 空间溢出
#define     ERR_SRAM            0x82        // SRAM硬件故障
#define     ERR_PDDATA          0x83        // 掉电数据错误

#define     ABNOR_SELFDEL       0xFF        // 清除当前故障
//----------------------------------------------------------------------------------------------------
#define     STEPINIT_OK         0xff        // 阶段工艺初始化成功
#define     STEPINIT_IF         0xfe        // 阶段工艺是IF的初始化
#define     STEPINIT_ELSE       0xfd        // 阶段工艺是ELSE的初始化
#define     STEPINIT_VAR        0xfc        // 阶段工艺是VAR(变量操作)的初始化
#define     STEPINIT_AND        0xfb        // 阶段工艺是AND的初始化 15-0929 added
//----------------------------------------------------------------------------------------------------
//操作符定义

#define     OP_NULL             0x00        // 未设定
#define     OP_EQU              0x01        // 等于     ==
#define     OP_MORE             0x02        // 大于     >
#define     OP_MORE_EQU         0x03        // 大于等于 >=
#define     OP_LESS             0x04        // 小于     <
#define     OP_LESS_EQU         0x05        // 小于等于 <=
#define     OP_ASSIGN           0x06        // 赋值     =
#define     OP_PLUS             0x07        // 加       +=
#define     OP_MINUS            0x08        // 减       -=
#define     OP_INC              0x09        // 递增     ++
#define     OP_DEC              0x0a        // 递减     --

//----------
#define     MIN_ERR_OP          (OP_DEC+1)  // 最小 操作符定义

//----------------------------------------------------------------------------------------------------
//变量及参数定义

#define     VAR_NULL            0x00        // 未定义
#define     VAR_TIME1           0x01        // 时间变量1(整型0.1S)
#define     VAR_CAP1            0x02        // 容量变量1
#define     VAR_CYC1            0x03        // 循环变量1(整型)
#define     VAR_CYC2            0x04        // 循环变量2(整型)
#define     PAR_TIME            0x05        // 过程时间
#define     PAR_VOL             0x06        // 电压
#define     PAR_CUR             0x07        // 电流
#define     PAR_TEMP            0x08        // 温度
#define     PAR_CAP             0x09        // 容量
#define     PAR_CYCNUM          0x0a        // 循环次数(无符号整型)
#define     PAR_V_NSLOPE        0x0b        // 电压负斜率
#define     PAR_SLOPE           0x0c        // 斜率
#define     PAR_MAXVi           0x0d        // 最大单体电压
#define     PAR_MINVi           0x0e        // 最小单体电压
#define     PAR_MAXI            0x0f        // 最大电流
#define     PAR_MINI            0x10        // 最小电流
#define     PAR_MAXTi           0x11        // 最大单体温度
#define     PAR_MINTi           0x12        // 最小单体温度
#define     PAR_POWER           0x13        // 功率
#define     PAR_ENERGY          0x14        // 能量
#define     VAR_CYC3            0x15        // 循环变量3(整型)
#define     VAR_CYC4            0x16        // 循环变量4(整型)
#define     VAR_CYC5            0x17        // 循环变量5(整型)
#define     VAR_TCOEF           0x18        // 温度系数 16-0531
#define     VAR_STPCAP          0x19        // 阶段（过程）容量 16-0531
#define     VAR_CVOLTDROP       0x20        // 充电电压跌落 16-0716
//----------
#define     MIN_ERR_VAR_PAR     (VAR_CVOLTDROP+1)    // 最小 错误变量或参数定义 16-0716

//----------------------------------------------------------------------------------------------------
//  命令字及相应版本定义
//----------------------------------------------------------------------------------------------------
/*
    命令版本表   索引对应命令，内容对应版本
                通用版本从0x0A～0xC7(Ver 1.0 ~ Ver19.9)
                特殊版本从0xC8～0xFE(Ver20.0 ~ Ver25.4)
                0~9留待扩展
                0xff表示不支持命令
    更新记录：
            15-0923 命令 0x81 - 读实时明细命令  升级至 V1.1(0x0B)
                    命令 0x83 - 通道启动命令    升级至 V1.1(0x0B)
            15-1116 命令 0x80 - 联机命令        升级至 V1.1(0x0B)
                    命令 0x84 - 写设备信息命令  升级至 V1.1(0x0B)
            15-1223 命令 0xC0~C5 - V1.0 新增
            16-0125 命令 0x80 - 联机命令        升级至 V1.2(0x0C) 支持软件并联
            16-0125 命令 0x84 - 写设备信息命令  升级至 V1.2(0x0C) 支持软件并联
            16-0801 命令 0xF1 - 透明传输命令 V1.0
*/
//----------------------------------
#ifdef  IN_APP_LAYER

    #define   NVER      (0xFF)      // 尚不支持的命令版本
    
    const unsigned char CMD_VER_TBL[256] =
    {
/*      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, */
        NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, /* 0x0X */
        NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, /* 0x1X */
        NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, /* 0x2X */
        NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, /* 0x3X */
    
/*      0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, */
        NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, /* 0x4X */
        NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, /* 0x5X */
        NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, /* 0x6X */
        NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, /* 0x7X */
        
/*      0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, */
        0x0C, 0x0B, 0x0A, 0x0B, 0x0C, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, /* 0x8X */
        0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, /* 0x9X */
        0x0A, 0x0A, 0x0A, 0x0A, NVER, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, NVER, NVER, 0x0A, NVER, /* 0xAX */
        0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, NVER, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, NVER, /* 0xBX */
        
/*      0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, */
        0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, /* 0xCX */
        NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, /* 0xDX */
        NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, /* 0xEX */
        NVER, 0x0A, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER, NVER  /* 0xFX */
    };
#else
    app_ext const unsigned char CMD_VER_TBL[256];
#endif
//----------------------------------
//----------------------------------
#define     CMD_CONNECTION          0x80        // 联机
#define     CMD_RD_REALDATA         0x81        // 读取实时数据
#define     CMD_CIRCUIT_CTRL        0x82        // 通道控制
#define     CMD_CIRCUIT_START       0x83        // 通道启动
#define     CMD_MODIFY_DEVINFO      0x84        // 修改设备信息
#define     CMD_RD_PRECISION        0x85        // 读取精度系数
#define     CMD_CAL_PRECISION       0x86        // 精度调试
#define     CMD_WR_PRECISION        0x87        // 写入精度系数
#define     CMD_ALLOC_TEMP          0x88        // 温度分配
#define     CMD_CFG_MAC_IP          0x89        // MAC及IP配置
#define     CMD_RD_DEV_MAC_IP       0x8a        // 读取指定设备号的MAC及IP
#define     CMD_RD_HIP_SMAC         0x8b        // 读取主机IP地址以及设备MAC
#define     CMD_RQ_HOSTCTRL         0x8c        // 申请主控发送协议
#define     CMD_TEST_HARDWARE       0x8d        // 测试AD,DA,接触器,LED
#define     CMD_RD_EEPROM           0x8e        // 读取EEPROM
#define     CMD_WR_EEPROM           0x8f        // 写入EEPROM

#define     CMD_RD_SYSBIT           0x90        // 读系统功能位（报警屏蔽位及功能控制开关）
#define     CMD_WR_SYSBIT           0x91        // 写系统功能位（报警屏蔽位及功能控制开关）
#define     CMD_RD_CALIB            0x92        // 读取校准数据
#define     CMD_WR_CALIB            0x93        // 写入校准数据
#define     CMD_RD_MASKREG          0x94        // 读取报警屏蔽及调节器
#define     CMD_WR_MASKREG          0x95        // 写入报警屏蔽及调节器
#define     CMD_RD_IRATE            0x96        // 读取额定电流
#define     CMD_WR_IRATE            0x97        // 写入额定电流
#define     CMD_RD_URATE            0x98        // 读取额定电压
#define     CMD_WR_URATE            0x99        // 写入额定电压
#define     CMD_RD_IPID             0x9A        // 读取电流PID
#define     CMD_WR_IPID             0x9B        // 写入电流PID
#define     CMD_RD_UPID             0x9C        // 读取电压PID
#define     CMD_WR_UPID             0x9D        // 写入电压PID
#define     CMD_RD_TVALUE           0x9E        // 读取延时时间
#define     CMD_WR_TVALUE           0x9F        // 写入延时时间

#define     CMD_RD_SVALUE           0xA0        // 读取报警阈值
#define     CMD_WR_SVALUE           0xA1        // 写入报警阈值
#define     CMD_RD_VERSION          0xA2        // 读取软件版本
#define     CMD_WR_CANADDR          0xA3        // 写入CAN地址
#define     CMD_REV_0xA4            0xA4        // 保留
//----------------------------------
//中位机相关命令
//----------
#define     CMD_CALIBSET            0xA5        // 保存或恢复校准数据
#define     CMD_RD_CTRLCFG          0xA6        // 读取中位机控制配置参数
#define     CMD_WR_CTRLCFG          0xA7        // 写入中位机控制配置参数
#define     CMD_RD_SCH_HCFG         0xA8        // 读取辅助通道硬件配置参数
#define     CMD_WR_SCH_HCFG         0xA9        // 写入辅助通道硬件配置参数
#define     CMD_RD_SCH_SCFG         0xAA        // 读取辅助通道软件配置参数
#define     CMD_WR_SCH_SCFG         0xAB        // 写入辅助通道软件配置参数
#define     CMD_REV_0xAC            0xAC        // 保留
#define     CMD_REV_0xAD            0xAD        // 保留
#define     CMD_RESET_DEV           0xAE        // 复位中位机
#define     CMD_REV_0xAF            0xAF        // 保留
//----------------------------------
//辅助通道相关命令
//----------
#define     CMD_RD_SUBREAL          0xB0        // 读取辅助通道实时数据
#define     CMD_WR_SUBREAL          0xB1        // 写入辅助通道实时数据
#define     CMD_RD_SUBRATE          0xB2        // 读取辅助通道额定数据
#define     CMD_WR_SUBRATE          0xB3        // 写入辅助通道额定数据
#define     CMD_RD_SUBCALIB         0xB4        // 读取辅助通道校准数据
#define     CMD_WR_SUBCALIB         0xB5        // 写入辅助通道校准数据
#define     CMD_RD_SUBEDATA         0xB6        // 读取辅助通道扩展数据（CAN地址及AD控制参数）
#define     CMD_WR_SUBEDATA         0xB7        // 写入辅助通道扩展数据（CAN地址及AD控制参数）
#define     CMD_REV_0xB8            0xB8        // 保留
#define     CMD_WR_SUBSINGLE        0xB9        // 辅助通道单独控制
#define     CMD_RD_TVALUE_EX        0xBA        // 读取扩展延时时间(主通道)
#define     CMD_WR_TVALUE_EX        0xBB        // 写入扩展延时时间(主通道)
#define     CMD_RD_SVALUE_EX        0xBC        // 读取扩展报警阈值(主通道)
#define     CMD_WR_SVALUE_EX        0xBD        // 写入扩展报警阈值(主通道)
#define     CMD_PROTL_TRANS         0xBE        // 协议中转

//----------------------------------
#define     CMD_RD_BUSDATA          0xC0        // 读取母线参数 15-1223
#define     CMD_WR_BUSDATA          0xC1        // 写入母线参数 15-1223
#define     CMD_RD_FREQDATA         0xC2        // 读取频率等参数 15-1223
#define     CMD_WR_FREQDATA         0xC3        // 写入频率等参数 15-1223
#define     CMD_RD_DUTYDATA         0xC4        // 读取占空比参数 15-1223
#define     CMD_WR_DUTYDATA         0xC5        // 写入占空比参数 15-1223

//----------------------------------
#define     CMD_TRANSPARENT         0xF1        // 透明传输 16-0801
//----------------------------------------------------------------------------------------------------
//透明传输(0xF1)命令相关宏定义 16-0801
#define     CAN_INTERFACE           (0x00)  // CAN接口透明传输
#define     UART_INTERFACE          (0x01)  // 串行接口透明传输

//----------------------------------------------------------------------------------------------------
//CAN通信阶段同步控制
#define     SYNC_FAILURE        0x03        // 阶段启动时,中位机尚未同步完成
#define     SYNC_CMD1_MASK      0xfe        // 阶段启动时,命令0x43同步完成标记
#define     SYNC_CMD2_MASK      0xfd        // 阶段启动时,命令0x44同步完成标记
#define     SYNC_SUCCESS        0x00        // 阶段启动时,中位机同步完成标记
//----------------------------------------------------------------------------------------------------
//返回故障字
//layer-1
#define     ANALYSIS_OK         0x00        // 成功接收命令并解析返回
#define     STA_ERR             0x01        // 起始字错
#define     LEN_ERR             0x02        // 长度错误
#define     DAT_ERR             0x03        // 数据错误
#define     CRC_ERR             0x04        // 校验错误
#define     STP_ERR             0x05        // 结束字错
#define     FORMAT_ERR          0x06        // 数据格式错误
#define     PACK_LEN_ERR        0x07        // 封装后长度错误
#define     CMD_ERR             0x08        // 无效命令
//layer-2

//layer-3
#define     DEV_NO_ERR          0x30        // 设备号错误
#define     L3_LEN_ERR          0x31        // 封装后长度错误(具体应用层)

//----------------------------------------------------------------------------------------------------
// 通道控制发送
//----------------------------------
//操作字
#define     OPT_INVALID         0x00        // 无效
#define     OPT_STOP            0x01        // 停止
#define     OPT_PAUSE           0x02        // 暂停
#define     OPT_RESUME          0x03        // 恢复
#define     OPT_JUMP            0x04        // 跳转
#define     OPT_SINGLESTEP      0x05        // 单步运行
#define     OPT_LED_FLASH       0x06        // 闪指示灯
#define     OPT_LED_OFF         0x07        // 关指示灯
//#define     OPT_RESTART         0x08        // 重新启动 16-0128 取消
//#define     OPT_LOCK            0x09        // 回路锁定 16-0128 取消

#define     MAX_OPT_CODE        (OPT_LED_OFF)
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
// 应用层解析后返回数据帧结构
typedef struct _strDataBuf
{
    unsigned short  dat_len;
    unsigned short  max_len;
    unsigned char   data[1490];

} strDataBuf;
//----------------------------------------------------------------------------------------------------
// 数据帧层帧结构
typedef struct _strDFHead
{
    unsigned char   stamp;
    unsigned char   page_num;
    unsigned char   page_inx;
    unsigned char   rev;
    unsigned int    frame_id;

} strDFHead;
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#endif /* __app_cfg_H */
