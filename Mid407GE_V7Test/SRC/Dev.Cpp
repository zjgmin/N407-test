//----------------------------------------------------------------------------------------------------
// Header:      设备信息处理程序
// File Name:   Dev.cpp
// Author:      David
// Date:        2012-11-14
//----------------------------------------------------------------------------------------------------
#define  _IN_DEV_CPP
extern "C" 
{
    #include    <wchar.h>
//    #include    <wctype.h>
    #include    <string.h>
    #include    "rtl.h"
    #include    "main.h"
    #include    "led.h"
    #include    "gpio.h"
    #include    "watchdog.h"   
    #include    "spi.h"
    #include    "net_Config.h"
    #include    "sys_config.h"
    #include    "app_config.h"
}
#include    "app_can.hpp"
#include    "Dev.hpp"
#include    "chan.hpp"
#include    "EtherCmd.hpp"
//----------------------------------------------------------------------------------------------------
extern  "C" U8      own_hw_adr[6];
extern  "C" LOCALM  nlocalm[3];
extern  "C" U16     Loc_PORT;
//----------------------------------------------------------------------------------------------------
//  默认设备信息  32 Unicode Chars, 2016-0422
//----------
const wchar_t   DEV_ID[]    = L"BTS/BFS NE";            // 设备号ID
const wchar_t   DEV_NAME[]  = L"金帆电池充放电电源";     // 设备名称
//----------------------------------------------------------------------------------------------------
//构造函数 16-0125更新
//----------------------------------------------------------------------------------------------------
Dev::Dev()
{
    U32 i;

    mNo = 0;
//  mInfo   = DEFAULT_INFO;
    i = PUBLISH_DATE;   // 发布时间处理 16-0422
    mInfo.mark                  = DEVINFO_MARK;
    mInfo.day                   = i%100; i/=100;
    mInfo.mon                   = i%100; i/=100;
    mInfo.year                  = i;                // 生产日期
    for(i=0; i<((DEVID_LEN+1)/2); i++) {
        mInfo.dev_id[2*i]       = DEV_ID[i]&0xff;   // 设备号ID（客户定义）
        mInfo.dev_id[2*i+1]     = DEV_ID[i]>>8;
    }
    for(i=0; i<((DEVNAME_LEN+1)/2); i++) {
        mInfo.dev_name[2*i]     = DEV_NAME[i]&0xff; // 设备名称（厂家定义）
        mInfo.dev_name[2*i+1]   = DEV_NAME[i]>>8;
    }  
    
    mInfo.soft_ver              = SOFT_VERSION;     // 中位机软件版本号（只读）
    mInfo.hard_ver              = HARD_VERSION;     // 中位机硬件版本号（读写）16-0422
  
//    for(i=0; i<CMDVERTBL_LEN; i++) {
//        mInfo.cmd_ver[i]        = CMD_VER_TBL[i];   // 通信命令版本表（只读）
//    }

    // 通道数相关 16-0125 调整
    mInfo.max_node_num          = MAX_NODE_NUM;     // 最大节点数 【下位机电源数，只读】 16-0129
    mInfo.max_chan_num          = MAX_CHAN_NUM;     // 最大通道数
    mInfo.chan_num              = MAX_CHAN_NUM;     // 控制通道数
#ifdef  EN_PARALLEL     // 允许并联 并联通道数默认
    mInfo.par_node_num          = DEF_PNODE_NUM;    // 并联通道数（默认为1，不并联）
#else
    mInfo.par_node_num          = 0;                // 不允许并联，并联数强制为0
#endif
    mInfo.ext_chan_num          = 0;                // 扩展通道数

    mInfo.dev_str_type          = DEV_STR_TYPE;     // 设备结构类型
    
    // 设备结构配置表 （使用时需要设置） 16-0514
    switch (mInfo.dev_str_type)
    {
        default:
        case DS_GENERAL:        // 通用类型设备结构
        {
            mInfo.dev_str_tbl[0]    = 0;
            mInfo.dev_str_tbl[1]    = 0;
            mInfo.dev_str_tbl[2]    = 0;
            mInfo.dev_str_tbl[3]    = 0;
            mInfo.dev_str_tbl[4]    = 0;
            mInfo.dev_str_tbl[5]    = 0;
            break;
        }
    }

    mInfo.max_chr_cur           = DEF_CHR_CUR;                      // 最大充电电流 mA
    mInfo.max_chr_vol           = DEF_CHR_VOL;                      // 最大充电电压 mV
    mInfo.max_chr_pow           = DEF_CHR_CUR * DEF_CHR_VOL / 1000; // 最大充电功率 mW
    mInfo.max_dch_cur           = DEF_DCH_CUR;                      // 最大放电电流 mA
    mInfo.min_dch_vol           = DEF_DCH_VOL;                      // 最小放电电压 mV
    mInfo.max_dch_pow           = DEF_DCH_CUR * DEF_CHR_VOL / 1000; // 最大放电功率 mW 用充电电压计算
    
    mInfo.run_fan_temp.Float32  = RUN_FANON_TEMP;       // 运行--风机开启温度 ℃
    mInfo.prt_fan_temp.Float32  = PROT_FANON_TEMP;      // 保护--风机开启温度 ℃

    mInfo.wmfunc_cfg.word       = DEFAULT_WMFUNC_BIT;   // 工作模式功能开关
    mInfo.exfunc_cfg.word       = DEFAULT_EXFUNC_BIT;   // 设备扩展功能开关
    mInfo.min_rec_time          = CTRL_PERIOD*1000;     // 最小记录时间(uS)

    if (CMD_VER_TBL[CMD_CONNECTION] > 0x0A) {   // Ver 1.1版以后支持--其他项表  15-1112
        mInfo.vi_num        = 0;
        mInfo.ti_num        = 0;
        mInfo.ii_num        = 0;
        mInfo.oi_num        = 0;
        mInfo.xitem_num     = DEF_XITEM_NUM;
        for (i=0; i<XITEMTBL_LEN; i++) {
            mInfo.xitem_tbl[i]  = DEF_XITEM_TBL[i];
        }
    }

    //清全局故障
    RstGblFault(0xffffffff);

    for (int i=0; i<MAX_SLAVE_PC; i++) {
        mPCSlaCtrl[i].flag  = 0;
        mPCSlaCtrl[i].ch    = 0;
        mPCSlaCtrl[i].ip[0] = 0;
        mPCSlaCtrl[i].ip[1] = 0;
        mPCSlaCtrl[i].ip[2] = 0;
        mPCSlaCtrl[i].ip[3] = 0;
        mPCSlaCtrl[i].tout  = SLAVE_TOUT+1;
    }
    //-------------------------
    //  初始化发送控制器
    //-------------------------    
    mTxCtrl.realCh      = 0;                 
    mTxCtrl.recCh       = 0;                 
    mTxCtrl.repeatNum   = 0; 
    for(i=0; i<MAX_CHAN_NUM; i++) {   
        mTxCtrl.packLen[i]   = 0; 
        mTxCtrl.recNum[i]    = 0; 
    }  
    
    SavePackLen = REC_LEN;                              // 当前记录包（记录保存）长度
    MaxSavePack = RECORD_CAP / REC_LEN - 1;             // 缓冲保存的最大记录数
    LoadPackLen = FIXED_DATA_LEN;                       // 上传数据（实时或明细）长度
    MaxLoadPack = MAX_SENDPACK_LEN / FIXED_DATA_LEN;    // 最多上传包个数     
}
//----------------------------------------------------------------------------------------------------
//  联机(读取设备信息)
/*
    参数   *ptr :     待拷贝数据缓冲
    返回   len :      拷贝数据长度
    备注： 0x80命令
*/
//----------------------------------------------------------------------------------------------------
U16 Dev::Rd_DevInfo(U8 *ptr,U8 rem_at)
{
    U32 i;
    U16 len = 0;
    U8 *p_buf;

    // 生产日期 - 只读 16-0422
    i = PUBLISH_DATE;
    *ptr++ = i%100; i/=100;
    len++;
    *ptr++ = i%100; i/=100;
    len++;
    *ptr++ = i & 0xff;
    len++;
    *ptr++ = i >> 8;
    len++;
    
//    *ptr++ = mInfo.day;                 // 生产日期 日-月-年
//    len++;
//    *ptr++ = mInfo.mon;
//    len++;
//    *ptr++ = mInfo.year & 0xff;
//    len++;
//    *ptr++ = mInfo.year >> 8;
//    len++;

    for (i=0; i<DEVID_LEN; i++) {
        *ptr++ = mInfo.dev_id[i];       // 设备id
        len++;
    }
    for (i=0; i<DEVNAME_LEN; i++) {
        *ptr++ = mInfo.dev_name[i];     // 设备名称
        len++;
    }

    *ptr++ = SOFT_VERSION & 0xff;       // 中位机软件版本
    len++;
    *ptr++ = SOFT_VERSION >> 8;
    len++;
    *ptr++ = mInfo.hard_ver & 0xff;     // 中位机硬件版本
    len++;
    *ptr++ = mInfo.hard_ver >> 8;
    len++;
    for (i=0; i<CMDVERTBL_LEN; i++) {
        *ptr++ = CMD_VER_TBL[i];        // 通信命令版本表
        len++;
    }
    
    // 通道数相关 16-0125 调整 
    *ptr++ = MAX_NODE_NUM;              // 最大节点数(常数) mInfo.max_node_num
    len++;    
    *ptr++ = MAX_CHAN_NUM;              // 最大通道数(常数) mInfo.max_chan_num
    len++;
    *ptr++ = mInfo.chan_num;            // 控制通道数
    len++;

#ifdef  EN_PARALLEL     // 允许并联 PC读并联节点数
    *ptr++ = mInfo.par_node_num;        // 并联通道数
#else
    *ptr++ = 0;                         // 不允许并联，并联数强制为0
#endif
    len++;
    
    *ptr++ = mInfo.ext_chan_num;        // 扩展通道数
    len++;    
    *ptr++ = mInfo.dev_str_type;        // 设备结构类型
    len++;
    for (i=0; i<DEVSTR_LEN; i++) {
        *ptr++ = mInfo.dev_str_tbl[i];  // 设备结构配置表
        len++;
    }

    p_buf = (U8 *)&mInfo.max_chr_cur;   // 最大充电电流
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
        len++;
    }
    p_buf = (U8 *)&mInfo.max_chr_vol;   // 最大充电电压
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
        len++;
    }
    p_buf = (U8 *)&mInfo.max_chr_pow;   // 最大充电功率
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
        len++;
    }
    p_buf = (U8 *)&mInfo.max_dch_cur;   // 最大放电电流
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
        len++;
    }
    p_buf = (U8 *)&mInfo.min_dch_vol;   // 最小放电电压
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
        len++;
    }
    p_buf = (U8 *)&mInfo.max_dch_pow;   // 最大放电功率
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
        len++;
    }
    p_buf = (U8 *)&mInfo.run_fan_temp.Float32;  // 运行风机开启温度
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
        len++;
    }
    p_buf = (U8 *)&mInfo.prt_fan_temp.Float32;  // 保护风机开启温度
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
        len++;
    }

    p_buf = (U8 *)&mInfo.wmfunc_cfg.word;   // 工作模式功能开关
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
        len++;
    }
    p_buf = (U8 *)&mInfo.exfunc_cfg.word;   // 设备扩展功能开关
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
        len++;
    }
    p_buf = (U8 *)&mInfo.min_rec_time;  // 最小记录时间(uS)
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
        len++;
    }

    // 网络参数
    switch (rem_at)
    {
    case MASTER_PC:
        for (i=0; i<4; i++) {
            *ptr++ = 0xff;              // 主机IP 255.255.255.255
            len++;
        }
        break;
    case SLAVE_PC:
        for (i=0; i<4; i++) {
            *ptr++ = 0x00;              // 从机IP 0.0.0.0
            len++;
        }
        break;

    default:
        *ptr++ = 0xff;                  // 非主从IP 255.255.0.0
        len++;
        *ptr++ = 0xff;
        len++;
        *ptr++ = 0;
        len++;
        *ptr++ = 0;
        len++;
        break;
    }
    *ptr++ = mHostCtrl.port & 0xff;     // 主机端口
    len++;
    *ptr++ = mHostCtrl.port >> 8;
    len++;
    for (i=0; i<6; i++) {
        *ptr++ = mNetInfo.MAC[i];       // 设备MAC
        len++;
    }
    // 配置表
    if (CMD_VER_TBL[CMD_CONNECTION] == 0x0A) {  // 15-1112 Ver1.0联机配置表
  #if (SCFGTBL_LEN <= MAX_CHAN_NUM) // 辅助通道软配置表长度
        for (i=0; i<SCFGTBL_LEN; i++) {
            *ptr++ = mSampCfg.ch[i].ViNum & 0xff;
            *ptr++ = mSampCfg.ch[i].ViNum >> 8;
            len += 2;
        }
        for (i=0; i<SCFGTBL_LEN; i++) {
            *ptr++ = mSampCfg.ch[i].TiNum & 0xff;
            *ptr++ = mSampCfg.ch[i].TiNum >> 8;
            len += 2;
        }
        for (i=0; i<SCFGTBL_LEN; i++) {
            *ptr++ = mSampCfg.ch[i].IiNum & 0xff;
            len++;
        }
        for (i=0; i<SCFGTBL_LEN; i++) {
            *ptr++ = mSampCfg.ch[i].OiNum & 0xff;
            len++;
        }
        for (i=0; i<SCFGTBL_LEN; i++) {
            *ptr++ = mSampCfg.ch[i].XiNum & 0xff;
            len++;
        }

  #else // 回路不足，补足表格

        for (i=0; i<MAX_CHAN_NUM; i++) {
            *ptr++ = mSampCfg.ch[i].ViNum & 0xff;
            *ptr++ = mSampCfg.ch[i].ViNum >> 8;
            len += 2;
        }
        for (; i<SCFGTBL_LEN; i++) {
            *ptr++ = 0;
            *ptr++ = 0;
            len += 2;
        }
        
        for (i=0; i<MAX_CHAN_NUM; i++) {
            *ptr++ = mSampCfg.ch[i].TiNum & 0xff;
            *ptr++ = mSampCfg.ch[i].TiNum >> 8;
            len += 2;
        }
        for (; i<SCFGTBL_LEN; i++) {
            *ptr++ = 0;
            *ptr++ = 0;
            len += 2;
        }

        for (i=0; i<MAX_CHAN_NUM; i++) {
            *ptr++ = mSampCfg.ch[i].IiNum & 0xff;
            len++;
        }
        for (; i<SCFGTBL_LEN; i++) {
            *ptr++ = 0;
            len++;
        }

        for (i=0; i<MAX_CHAN_NUM; i++) {
            *ptr++ = mSampCfg.ch[i].OiNum & 0xff;
            len++;
        }
        for (; i<SCFGTBL_LEN; i++) {
            *ptr++ = 0;
            len++;
        }

        for (i=0; i<MAX_CHAN_NUM; i++) {
            *ptr++ = mSampCfg.ch[i].XiNum & 0xff;
            len++;
        }
        for (; i<SCFGTBL_LEN; i++) {
            *ptr++ = 0;
            len++;
        }

    #endif
    }
    if (CMD_VER_TBL[CMD_CONNECTION] > 0x0A) {   // Ver 1.1版以后支持--各个辅助项数 15-1116
        *ptr++ = mInfo.vi_num;          // 通道电压数
        len++;
        *ptr++ = mInfo.vi_num >> 8;
        len++;
        *ptr++ = mInfo.ti_num;          // 通道温度数
        len++;
        *ptr++ = mInfo.ti_num >> 8;
        len++;
        *ptr++ = mInfo.ii_num;          // 通道开关量入数
        len++;
        *ptr++ = mInfo.oi_num;          // 通道开关量出数
        len++;
        *ptr++ = mInfo.xitem_num;       // 通道其他项数
        len++;
        *ptr++ = mInfo.xitem_num >> 8;
        len++;
        for (i=0; i<XITEMTBL_LEN; i++) {    // 其他项表
            *ptr++ = mInfo.xitem_tbl[i];
            len++;
        }
    }

//    *ptr++ = 0; //添加保留字 U16
//    len++;
//    *ptr++ = 0;
//    len++;

    return (len);
}
//----------------------------------------------------------------------------------------------------
//  (修改设备信息)
/*
    参数  *ptr :  新的设备信息数据缓冲
    返回  ret :   0－成功    1－失败

    备注： 0x84命令
*/
//----------------------------------------------------------------------------------------------------
U8 Dev::Wr_DevInfo(U8 *ptr)
{
    U8  ret = 0;
    U32 f_buf;
    U32 i, j;

    j = 0;                              // 生产日期
    mInfoBak.day    = ptr[j++];         // 日
    mInfoBak.mon    = ptr[j++];         // 月
    mInfoBak.year   = *(U16 *)&ptr[j];  // 年
    j += 2;

    for (i=0; i<DEVID_LEN; i++) {
        mInfoBak.dev_id[i]  = ptr[j+i];     // 设备ID
    }
    j += DEVID_LEN;
    for (i=0; i<DEVNAME_LEN; i++) {
        mInfoBak.dev_name[i]    = ptr[j+i]; // 设备名称
    }
    j += DEVNAME_LEN;

    mInfoBak.soft_ver       = SOFT_VERSION;         // 中位机软件版本号（只读），PC下发值不管 16-0202
    j += 2;
    mInfoBak.hard_ver       = *(U16 *) &ptr[j];     // 中位机硬件版本号
    j += 2;
    
    // 通道数相关 16-0125 调整
    mInfoBak.max_node_num   = MAX_NODE_NUM;         // 最大节点数(只读，恒为MAX_NODE_NUM)，PC下发值不管
    j++;    
    mInfoBak.max_chan_num   = MAX_CHAN_NUM;         // 最大通道数(只读，恒为MAX_CHAN_NUM)，PC下发值不管
    j++;
    mInfoBak.chan_num       = ptr[j++];             // 控制通道数

#ifdef  EN_PARALLEL     // 允许并联 并联节点数设定
    mInfoBak.par_node_num   = ptr[j++];             // 并联通道数
#else
    mInfoBak.par_node_num   = 0;                    // 不允许并联，并联数强制为0，PC下发值不管
    j++;
#endif

    mInfoBak.ext_chan_num   = ptr[j++];             // 扩展通道数
    mInfoBak.dev_str_type   = ptr[j++];             // 设备结构类型
    for (i=0; i<DEVSTR_LEN; i++) {
        mInfoBak.dev_str_tbl[i] = ptr[j+i];         // 设备结构配置表
    }
    j += DEVSTR_LEN;

    f_buf   = *(U32 *) &ptr[j];
    mInfoBak.max_chr_cur    = *((float *)&f_buf);   // 最大充电电流
    j += 4;
    f_buf   = *(U32 *) &ptr[j];
    mInfoBak.max_chr_vol    = *((float *)&f_buf);   // 最大充电电压
    j += 4;
    f_buf   = *(U32 *) &ptr[j];
    mInfoBak.max_chr_pow    = *((float *)&f_buf);   // 最大充电功率
    j += 4;
    f_buf   = *(U32 *) &ptr[j];
    mInfoBak.max_dch_cur    = *((float *)&f_buf);   // 最大放电电流
    j += 4;
    f_buf   = *(U32 *) &ptr[j];
    mInfoBak.min_dch_vol    = *((float *)&f_buf);   // 最小放电电压
    j += 4;
    f_buf   = *(U32 *) &ptr[j];
    mInfoBak.max_dch_pow    = *((float *)&f_buf);   // 最大放电功率
    j += 4;
    f_buf   = *(U32 *) &ptr[j];
    mInfoBak.run_fan_temp.Float32   = *((float *)&f_buf);   // 运行风机开启温度
    j += 4;
    f_buf   = *(U32 *) &ptr[j];
    mInfoBak.prt_fan_temp.Float32   = *((float *)&f_buf);   // 保护风机开启温度
    j += 4;

    mInfoBak.wmfunc_cfg.word    = *(U32 *) &ptr[j]; // 工作模式功能开关
    j += 4;
    mInfoBak.exfunc_cfg.word    = *(U32 *) &ptr[j]; // 设备扩展功能开关
    j += 4;
    mInfoBak.min_rec_time       = *(U32 *) &ptr[j]; // 最小记录时间(uS)
    j += 4;

    if (CMD_VER_TBL[CMD_CONNECTION] > 0x0A) {       // Ver 1.1版以后支持--其他项表 15-1112
        mInfoBak.vi_num     = *(U16 *) &ptr[j];     // 单体电压数
        j += 2;
        mInfoBak.ti_num     = *(U16 *) &ptr[j];     // 单体温度数
        j += 2;
        mInfoBak.ii_num     = ptr[j++];             // 开关量输入数
        mInfoBak.oi_num     = ptr[j++];             // 开关量输入数
        mInfoBak.xitem_num  = *(U16 *) &ptr[j];     // 其他项数
        j += 2;
        for (i=0; i<XITEMTBL_LEN; i++) {
            mInfoBak.xitem_tbl[i]   = ptr[j++];
        }
    }
//    // 单体其他项等变化，需要初始化记录指针
//    if ((mInfo.vi_num != mInfoBak.vi_num)
//    ||  (mInfo.ti_num != mInfoBak.ti_num)
//    ||  (mInfo.xitem_num != mInfoBak.xitem_num)) {
//        for (i=0; i<cDev.mInfo.chan_num; i++) { // 判断是否工作（运行，暂停，异常暂停）
//            if ((cChan[i].Get_CurStatus() == WORKSTAT_RUN)
//            ||  (cChan[i].Get_CurStatus() == WORKSTAT_PAUSE)
//            ||  (cChan[i].Get_CurStatus() == WORKSTAT_XPAUSE)) {
//                return  (1);    // 有运行的通道，不允许修改设备其他项信息
//            }
//        }   
//    }    
    // 处理
    mInfoBak.mark   = DEVINFO_MARK;
    if (CheckDevInfo(&mInfoBak)) {  // 检验合法
        mInfoBak.crc    = Crc16((U8 *)&mInfoBak, sizeof(strDevInfo)-2);
//          mInfo   = mInfoBak;
        if (os_mut_wait (SPI_mutex, CTRL_TICK*2) != OS_R_TMO) {
            MRAM_Wr(CHIP1, (U8 *)&mInfoBak, DEVINF_ADDR, sizeof(strDevInfo));
            MRAM_Rd(CHIP1, (U8 *)&mInfoBak, DEVINF_ADDR, sizeof(strDevInfo));
            os_mut_release (SPI_mutex);
            if (Crc16((U8 *)&mInfoBak, sizeof(mInfoBak)) != 0) {
                ret = 1;
                /*  test
                
                置错误标记
                
                */
            } else {
                // 单体其他项等变换，需要初始化记录指针
                if ((mInfo.vi_num != mInfoBak.vi_num)
                ||  (mInfo.ti_num != mInfoBak.ti_num)
                ||  (mInfo.xitem_num != mInfoBak.xitem_num)) {              
                    ChanRecPtrRstProc();
                }
                mInfo = mInfoBak;
                  
                tsk_lock();
                PackLenSetProc();
                tsk_unlock();
            }
        } else {
            ret = 1;    //SPI被占用
        }
    } else {
        ret = 1;    //有错误
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  (实时明细数据包长度限制)
/*
    参数  无
    返回  无
*/
//----------------------------------------------------------------------------------------------------
void Dev::PackLenSetProc(void)
{
    U16 exlen = 0;
    
    if (mInfo.vi_num > MAX_TEST_VINUM) {
        exlen += MAX_TEST_VINUM*VI_SIZE;
    } else {
        exlen += mInfo.vi_num*VI_SIZE;
    }
    if (mInfo.ti_num > MAX_TEST_TINUM) {
        exlen += MAX_TEST_TINUM*TI_SIZE;
    } else {
        exlen += mInfo.ti_num*TI_SIZE;
    }    
    if (mInfo.xitem_num > MAX_TEST_XINUM) {
        exlen += MAX_TEST_XINUM*XI_SIZE;
    } else {
        exlen += mInfo.xitem_num*XI_SIZE;
    }    
  
    SavePackLen = REC_LEN + exlen;                  // 当前记录包（记录保存）长度
    MaxSavePack = RECORD_CAP / SavePackLen - 1;     // 缓冲保存的最大记录数
    LoadPackLen = FIXED_DATA_LEN + exlen;           // 上传数据（实时或明细）长度
    MaxLoadPack = MAX_SENDPACK_LEN / SavePackLen;   // 最多上传包个数
}
//----------------------------------------------------------------------------------------------------
//  拷贝恒流恒压字
/*
    返回  len:    拷贝数据长度

    备注： 0x81    命令调用
*/
//----------------------------------------------------------------------------------------------------
U16 Dev::SetUIMark(U8 *ptr)
{
    U16 len = sizeof(mCUI_Mark);
    U8  *p_buf;
    U32 i;

//      tsk_lock();

    p_buf = (U8 *)&(mCUI_Mark);
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }

//      tsk_unlock();

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  (从MRAM加载设备信息)
/*
    参数  无
    返回  ret :   1－成功    0－失败

    备注：
*/
//----------------------------------------------------------------------------------------------------
U8  Dev::LoadDevInfo(void) 
{
    U8  ret = 0;

    MRAM_Rd(CHIP1, (U8 *)&mInfoBak, DEVINF_ADDR, sizeof(strDevInfo));   //15-1214
    if ((Crc16((U8 *)&mInfoBak, sizeof(strDevInfo)) == 0)   // 校验通过
    &&  (mInfoBak.mark == DEVINFO_MARK)) {                  // 标记准确
        if (CheckDevInfo(&mInfoBak)) {
            mInfo   = mInfoBak;
            ret = 1;
        } 
    }
    //------------------------------
    // 回路数限制 13-1026
    
    if (mInfo.chan_num > MAX_CHAN_NUM) {
        mInfo.chan_num  = MAX_CHAN_NUM;
    } else if (mInfo.chan_num == 0) {
        mInfo.chan_num  = 1;
    }

    // 实时明细等数据长度配置 15-1121   
    PackLenSetProc();   
    //test 2012-0627
    // 巡检配置
    for (int i=0; i<MAX_CHAN_NUM; i++) {
        mSampCfg.ch[i].ViOffset = 1 * i;    // 单体电压号偏移
        mSampCfg.ch[i].ViNum    = 1;        // 单体电压数

        mSampCfg.ch[i].TiOffset = 1 * i;    // 单体温度号偏移
        mSampCfg.ch[i].TiNum    = 1;        // 单体温度数

        mSampCfg.ch[i].IiOffset = 0;        // 开关量输入偏移
        mSampCfg.ch[i].IiNum    = 0;        // 开关量输入数

        mSampCfg.ch[i].OiOffset = 0;        // 开关量输出偏移
        mSampCfg.ch[i].OiNum    = 0;        // 开关量输出数

        mSampCfg.ch[i].XiOffset = 1 * i;    // 其他扩展数据偏移
        mSampCfg.ch[i].XiNum    = 1;        // 其他扩展数据数
    }
    // 回路数限制
    //------------------------------

    return  (ret);
}
//--------------------------------------------------
//  (保存设备信息到MRAM)
/*
    参数  无
    返回  ret :   1－成功    0－失败

    备注：
*/
//--------------------------------------------------
U8  Dev::SaveDevInfo(void) 
{
    U8  ret = 1;

    mInfo.mark = DEVINFO_MARK;
    if (CheckDevInfo(&mInfo)) { // 检验合法
        mInfo.crc = Crc16((U8 *)&mInfo, sizeof(mInfo)-2);
        if (os_mut_wait (SPI_mutex, CTRL_TICK*2) != OS_R_TMO) {
            //写入
            MRAM_Wr(CHIP1, (U8 *)&mInfo, DEVINF_ADDR, sizeof(mInfo));
            //读出校验
            MRAM_Rd(CHIP1, (U8 *)&mInfoBak, DEVINF_ADDR, sizeof(mInfoBak));
            os_mut_release (SPI_mutex);
            if (Crc16((U8 *)&mInfoBak, sizeof(mInfoBak)) != 0) {
                ret = 1;
                /*  test
                
                置错误标记
                
                */
            }
        } else {
            ret = 1;    //SPI被占用
        }
    } else {
        ret = 1;    //信息有错误
    }

    return  (ret);
}
//--------------------------------------------------
//  判断设备信息的合法性
/*
    返回  ret : 1－合法  0－非法
    16-0125 加入并联后的判断调整
*/
//--------------------------------------------------
U8  Dev::CheckDevInfo(const strDevInfo *pstr)
{
    if (pstr->mark != DEVINFO_MARK) {
        return  (0);    // 标记 错
    }
//    if ((pstr->max_chan_num > MAX_CHAN_NUM)
//    ||  (pstr->max_chan_num == 0)) {
//        return  (0);    // 最大通道数 错
//    }    
    if ((pstr->chan_num > MAX_CHAN_NUM)
    ||  (pstr->chan_num == 0)) {
        return  (0);    // 控制通道数 错
    }

#ifdef  EN_PARALLEL     // 允许并联,设备信息的并联节点数判断
    if ((pstr->par_node_num > MAX_CHAN_NUM)
    ||  (pstr->par_node_num == 0)) {
        return  (0);    // 并联节点数 错
    }
#else
    if (pstr->par_node_num != 0) {
        return  (0);    // 不允许并联，并联数须为0
    }
#endif

    if ((pstr->chan_num*pstr->par_node_num) > MAX_NODE_NUM) {
        return  (0);    // 并联后总节点数 错
    }
    if (pstr->ext_chan_num > MAX_EXT_CHAN_NUM) {
        return  (0);    // 扩展通道数错
    }
    if ((pstr->max_chr_cur > MAX_CHR_CUR)
    ||  (pstr->max_chr_cur <= 0)) {
        return  (0);    // 最大充电电流 错
    }
    if ((pstr->max_dch_cur > MAX_DCH_CUR)
    ||  (pstr->max_dch_cur <= 0)) {
        return  (0);    // 最大放电电流 错
    }
    if ((pstr->max_chr_vol > MAX_CHR_VOL)
    ||  (pstr->max_chr_vol <= 0)) {
        return  (0);    // 最大充电电压 错
    }
    if ((pstr->min_dch_vol < MIN_DCH_VOL)
    ||  (pstr->min_dch_vol >= pstr->max_chr_vol)) {
        return  (0);    // 最小放电电压 错
    }
    if ((pstr->max_chr_pow > (MAX_CHR_CUR*MAX_CHR_VOL/1000))
    ||  (pstr->max_chr_pow <= 0)) {
        return  (0);    // 最大充电功率 错
    }
    if ((pstr->max_dch_pow > (MAX_DCH_CUR*MAX_CHR_VOL/1000))
    ||  (pstr->max_dch_pow <= 0)) {
        return  (0);    // 最大放电功率 错 (用充电电压判)
    }
    if (((U32)pstr->run_fan_temp.Uint32 != 0xffffffff)
    && ((pstr->run_fan_temp.Float32 > MAX_FANON_TEMP)
    ||  (pstr->run_fan_temp.Float32 < MIN_FANON_TEMP))) {
        return  (0);    // 运行风机开启温度 错
    }
    if (((U32)pstr->prt_fan_temp.Uint32 != 0xffffffff)
    && ((pstr->prt_fan_temp.Float32 > MAX_FANON_TEMP)
    ||  (pstr->prt_fan_temp.Float32 < MIN_FANON_TEMP))) {
        return  (0);    // 保护风机开启温度 错
    }
    if (pstr->run_fan_temp.Float32 > pstr->prt_fan_temp.Float32) {
        return  (0);    // 风机开启温度：运行大于保护 错
    }
    if (pstr->wmfunc_cfg.word == 0) {
        return  (0);    // 工艺模式功能开关 错
    }
    if (pstr->min_rec_time < CTRL_PERIOD*1000) {
        return  (0);    // 最小记录时间(uS) 错
    }

    if (CMD_VER_TBL[CMD_CONNECTION] > 0x0A) {   // Ver 1.1版以后支持--其他项表 15-1112
        if (pstr->vi_num > MAX_TEST_VINUM) {
            return  (0);    // 超出允许的最大数
        }
        if (pstr->ti_num > MAX_TEST_TINUM) {
            return  (0);    // 超出允许的最大数
        }
        if (pstr->ii_num > MAX_TEST_IINUM) {
            return  (0);    // 超出允许的最大数
        }
        if (pstr->oi_num > MAX_TEST_OINUM) {
            return  (0);    // 超出允许的最大数
        }
        if (pstr->xitem_num > MAX_TEST_XINUM) {
            return  (0);    // 超出允许的最大项数
        }
    }

    return  (1);
}
//----------------------------------------------------------------------------------------------------
//  检查所有通道，是否有运行的通道
//  有 - 返回 0x01 ； 无 - 返回 0x00
//----------------------------------------------------------------------------------------------------
U8 Dev::Test_AnyChRunning(void)
{
    U8  ret = 0, i;

    for (i=0; i<mInfo.chan_num; i++) {
        if ((cChan[i].PdHoldData->NrRData.CurWkStat != WORKSTAT_STOP) 
        &&  (cChan[i].PdHoldData->NrRData.CurWkStat != WORKSTAT_INIT)
        &&  (cChan[i].PdHoldData->NrRData.CurWkStat != WORKSTAT_ABNOR)) {
            ret = 0x01;
            break;
        }
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  (主机信息初始化)
/*
    参数  无
    备注：
*/
//----------------------------------------------------------------------------------------------------
void Dev::InitHostCtrl(void) 
{
    mHostCtrl.flag  = HOSTINF_FLAG;
    mHostCtrl.port  = 1001;
    mHostCtrl.ip[0] = 127;
    mHostCtrl.ip[1] = 0;
    mHostCtrl.ip[2] = 0;
    mHostCtrl.ip[3] = 2;
    mHostCtrl.rev   = 0;
}
//----------------------------------------------------------------------------------------------------
//  (从MRAM加载主机信息)
/*
    参数  无
    返回  ret :   1－成功    0－失败

    备注：
*/
//----------------------------------------------------------------------------------------------------
U8  Dev::LoadHostCtrl(void) 
{
    U8  ret = 0;
    strHostCtrl tmpHostCtrl;

    MRAM_Rd(CHIP1, (U8 *)&tmpHostCtrl, HOSTINF_ADDR, sizeof(strHostCtrl));
    if ((Crc16((U8 *)&tmpHostCtrl, sizeof(strHostCtrl)) == 0)   // 校验通过
    &&  (tmpHostCtrl.flag == HOSTINF_FLAG)) {                   // 标记准确
        if (CheckHostCtrl(&tmpHostCtrl)) {
            mHostCtrl   = tmpHostCtrl;
            ret = 1;
        } else {
            InitHostCtrl();
        }
    } else {
        InitHostCtrl();
    }

    return  (ret);
}
//--------------------------------------------------
//  (保存主机信息到MRAM)
/*
    参数  无
    返回  ret :   1－成功    0－失败

    备注：
*/
//--------------------------------------------------
U8  Dev::SaveHostCtrl(void) 
{

    U8  ret = 1;

    mHostCtrl.flag = HOSTINF_FLAG;
    if (CheckHostCtrl(&mHostCtrl)) {    // 检验合法
        mHostCtrl.crc = Crc16((U8 *)&mHostCtrl, sizeof(mHostCtrl)-2);
        if (os_mut_wait (SPI_mutex, CTRL_TICK*2) != OS_R_TMO) {
            //写入
            MRAM_Wr(CHIP1, (U8 *)&mHostCtrl, HOSTINF_ADDR, sizeof(mHostCtrl));
            os_mut_release (SPI_mutex);
        } else {
            ret = 1;    //SPI被占用
        }
    } else {
        ret = 1;    //信息有错误
    }

    return  (ret);
}
//--------------------------------------------------
//  判断主机信息的合法性
/*
    返回  ret :   1－合法    0－非法
*/
//--------------------------------------------------
U8  Dev::CheckHostCtrl(const strHostCtrl *pstr)
{
    if (pstr->flag != HOSTINF_FLAG) return  (0);    // 标记错

    return  (1);
}
//----------------------------------------------------------------------------------------------------
//  设置IP及设备MAC(0x89)
//----------------------------------------------------------------------------------------------------
U8 Dev::SetMACIP(U8 *ptr) 
{
    U8  ret, i;

    if ((ptr[6] == 0xff)
    ||  (ptr[7] == 0xff)
    ||  (ptr[8] == 0xff)
    ||  (ptr[9] == 0xff)) { // 不允许设置广播IP地址
        ret = 0x01;
    } else {
        if (os_mut_wait (SPI_mutex, CTRL_TICK*2) != OS_R_TMO) {
            for (i=0; i<6; i++) {
                mNetInfo.MAC[i] = *ptr++;   // MAC地址
            }
            for (i=0; i<4; i++) {
                mNetInfo.Ip[i]  = *ptr++;   // IP地址
            }
            mNetInfo.Port   = *ptr++;       // 端口号
            mNetInfo.Port   += *ptr * 256;
            
            mNetInfo.crc    = Crc16((U8 *)&mNetInfo, sizeof(mNetInfo)-2);
            MRAM_Wr(CHIP1, (U8 *)&mNetInfo, NETINFO_ADDR, sizeof(mNetInfo));
            
            ReflashNetCfg();    //刷新
            
            ret = 0x00;
        } else {
            ret = 0x01;
        }
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  获取IP及设备MAC(0x8a)
//----------------------------------------------------------------------------------------------------
U16 Dev::GetMACIP(U8 *ptr) 
{
    U16 retlen  = 0;
    U8  i;

    for (i=0; i<6; i++) {
        *ptr++ = mNetInfo.MAC[i];   // MAC地址
        retlen++;
    }
    for (i=0; i<4; i++) {
        *ptr++ = mNetInfo.Ip[i];    // IP地址
        retlen++;
    }
    *ptr++  = mNetInfo.Port;        // 端口号
    retlen++;
    *ptr    = mNetInfo.Port>>8;
    retlen++;

    return  (retlen);
}
//----------------------------------------------------------------------------------------------------
//  读主机IP及设备MAC(0x8B)
//----------------------------------------------------------------------------------------------------
U16 Dev::RD_HIP_SMAC_Proc(U8 *ptr, U8 rem_at)
{
    U32 i;
    U16 len = 0;

    for (i=0; i<6; i++) {
        *ptr++ = own_hw_adr[i];
        len++;
    }
    switch (rem_at)
    {
    case MASTER_PC:
        for (i=0; i<4; i++) {
            *ptr++ = 0xff;      // 主机IP 255.255.255.255
            len++;
        }
        break;
    case SLAVE_PC:
        for (i=0; i<4; i++) {
            *ptr++ = mHostCtrl.ip[i];   // 从机IP 返回主机IP
            len++;
        }
        break;

    default:
        *ptr++ = 0xff;          // 非主从IP 255.255.0.0
        len++;
        *ptr++ = 0xff;
        len++;
        *ptr++ = 0;
        len++;
        *ptr++ = 0;
        len++;
        break;
    }
    *ptr++ = cDev.mHostCtrl.port & 0xff;    // 主机端口
    len++;
    *ptr++ = cDev.mHostCtrl.port >> 8;
    len++;

    *ptr++ = 0; // 添加保留字 U16
    len++;
    *ptr++ = 0;
    len++;

    return (len);
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//  加载控制配置参数
/*
    参数  无
    返回  ret :   1－成功    0－失败
    备注：
*/
//--------------------------------------------------
U8  Dev::LoadCtrlCfg(void)
{
    U8  ret = 0;
    U8  *pbox = (U8 *)&mCtrlCfg.Mark;

    //读取系统控制配置
    MRAM_Rd(CHIP1, pbox, DEVCFG_ADDR, sizeof(mCtrlCfg));
    if ((Crc16(pbox, sizeof(mCtrlCfg)) != 0) 
    ||  (CheckCtrlCfg((strCtrlCfg*)pbox))
    ||  (mCtrlCfg.Mark != CTRLCFG_MARK)) 
    {
        mCtrlCfg.Mark           = CTRLCFG_MARK;
        mCtrlCfg.bPdResumeMode  = PDRESUME_DEF;     // 掉电恢复标记 - 默认恢复
        mCtrlCfg.bStepSyncFlag  = STEP_SYNC_DEF;    // 同步标记 - 默认同步
        mCtrlCfg.bParallel      = PARALLEL_DEF;     // 并联标记 - 默认禁止
        mCtrlCfg.bAbnResume     = ABNRESUME_DEF;    // 异常恢复标记 - 默认禁止
        
        mCtrlCfg.MaxViBdNum     = MAX_VI_BOARD;     // 最大巡检电压板数
        mCtrlCfg.MaxTiBdNum     = MAX_TI_BOARD;     // 最大巡检温度板数
        mCtrlCfg.MaxIiBdNum     = MAX_II_BOARD;     // 最大开关量输入板数
        mCtrlCfg.MaxOiBdNum     = MAX_OI_BOARD;     // 最大开关量输出板数

        mCtrlCfg.MaxEBiBdNum    = MAX_EBI_BOARD;    // 最大扩展位输入板数
        mCtrlCfg.MaxEBoBdNum    = MAX_EBO_BOARD;    // 最大扩展位输出板数
        mCtrlCfg.MaxEDiBdNum    = MAX_EDI_BOARD;    // 最大扩展数据输入板数
        mCtrlCfg.MaxEDoBdNum    = MAX_EDO_BOARD;    // 最大扩展数据输出板数
        
        mCtrlCfg.BoardViNum     = DEF_BOARDVI_NUM;  // 板级巡检电压数
        mCtrlCfg.BoardTiNum     = DEF_BOARDTI_NUM;  // 板级巡检温度数
        mCtrlCfg.BoardIiNum     = DEF_BOARDII_NUM;  // 板级开关量输入数
        mCtrlCfg.BoardOiNum     = DEF_BOARDOI_NUM;  // 板级开关量输出数

        mCtrlCfg.BoardExBiNum   = DEF_BOARDEBI_NUM; // 板级扩展位输入数
        mCtrlCfg.BoardExBoNum   = DEF_BOARDEBO_NUM; // 板级扩展位输出数
        mCtrlCfg.BoardExDiNum   = DEF_BOARDEDI_NUM; // 板级扩展数据输入数
        mCtrlCfg.BoardExDoNum   = DEF_BOARDEDO_NUM; // 板级扩展数据输出数
        
        mCtrlCfg.CanOff_n       = CAN_TO_N;         // CAN超时计数器（收不到任何通道的包）
        mCtrlCfg.CtrlRetry_n    = CTRL_RETRY_N;     //（控制故障时）重试次数
        mCtrlCfg.CanRetry_n     = CAN_RETRY_N;      //（通信故障时）重试次数
        mCtrlCfg.CtrlRetry_t    = CTRL_RETRY_T;     //（控制故障时）重试间隔（mS）
        mCtrlCfg.CanRetry_t     = CAN_RETRY_T;      //（通信故障时）重试间隔（mS）
        mCtrlCfg.PDResume_t     = PD_RESUME_T;      // 掉电恢复周期  0表示不恢复，否则代表恢复倒计数时间 单位0.1S
        mCtrlCfg.Vdelta         = LI_VDELTA;        // 5mV
        mCtrlCfg.StartDly_t     = START_DLY_T;      // 阶段启动延时（通道启动间隔）
        
        mCtrlCfg.XCurJdg_t      = XCUR_JDG_T;       // 电流 异常判断周期 (mS)
        mCtrlCfg.XVolJdg_t      = XVOL_JDG_T;       // 电压 异常判断周期 (mS)
        mCtrlCfg.XTempJdg_t     = XTEMP_JDG_T;      // 温度 异常判断周期 (mS)
        mCtrlCfg.XViJdg_t       = XVI_JDG_T;        // 单体电压 异常判断周期 (mS)
        mCtrlCfg.XTiJdg_t       = XTI_JDG_T;        // 单体温度 异常判断周期 (mS)
        
        mCtrlCfg.IOJdg_t        = IO_JDG_T;         // 开关量 判断响应周期 (mS)
        mCtrlCfg.VpotJdg_t      = VPOT_JDG_T;       // 平台电压 判断响应周期 (mS)
        mCtrlCfg.CurJdg_t       = CUR_JDG_T;        // 电流 判断响应周期 (mS)
        mCtrlCfg.VolJdg_t       = VOL_JDG_T;        // 电压 判断响应周期 (mS)
        mCtrlCfg.PowJdg_t       = POW_JDG_T;        // 功率 判断响应周期 (mS)
        mCtrlCfg.EngyJdg_t      = ENGY_JDG_T;       // 能量 判断响应周期 (mS)
        mCtrlCfg.ViJdg_t        = VI_JDG_T;         // 单体电压 判断响应周期 (mS)
        mCtrlCfg.TiJdg_t        = TI_JDG_T;         // 单体温度 判断响应周期 (mS)
        
        mCtrlCfg.Record_t       = RECORD_T;         // 默认记录间隔 (mS)
        
        mCtrlCfg.Vi_Dot         = VI_DOT;           // 单体电压三位小数
        mCtrlCfg.Ti_Dot         = TI_DOT;           // 单体温度两位小数
        mCtrlCfg.bAutoReset     = 0x00;             // 自动复位与否 默认：0 - 禁止
        mCtrlCfg.bMainSubOrder  = 0x00;             // 主从CAN次序 默认：0 - CAN1主，CAN2从

        mCtrlCfg.MainChanType   = 0x00;             // 主控设备类型 默认：0 - 通用
        mCtrlCfg.SubChanType    = DEV_STR_TYPE;     // 辅助设备类型 有柜体结构决定
        
        //test
        mCtrlCfg.crc    = Crc16((U8 *)&mCtrlCfg, sizeof(strCtrlCfg)-2);
//15-1214   MRAM_Wr(CHIP1, pbox, DEVCFG_ADDR, sizeof(strCtrlCfg));
    } else {
        ret = 1;    //读取配置成功
    }

    return  (ret);
}
//--------------------------------------------------
//  拷贝控制配置参数
/*
    参数  无
    返回  ret :   数据长度

    备注：
*/
//--------------------------------------------------
U16 Dev::CopyCtrlCfg(U8 *ptr)
{
    U16 len = 0x00;
    U8  i;
    U8  *u8ptr = &mCtrlCfg.bPdResumeMode;

    for (i=0; i< sizeof(mCtrlCfg)-6; i++) { // 长度去MARK+CRC共6字节
        ptr[i] = u8ptr[i] ;
        len++;
    }

    return  (len);
}
//--------------------------------------------------
//  保存控制配置参数
/*
    参数  无
    返回  ret :   1－成功    0－失败

    备注：
*/
//--------------------------------------------------
U8  Dev::SaveCtrlCfg(U8 *ptr)
{
    U8  ret = 0x00;
    U8  i;
    U8  *u8ptr = &mCtrlCfg.bPdResumeMode;

    for (i=0; i< sizeof(mCtrlCfg)-6; i++) {
        u8ptr[i] = ptr[i];
    }
    u8ptr = (U8 *)&mCtrlCfg.Mark;
    mCtrlCfg.crc = Crc16((U8 *)&mCtrlCfg, sizeof(mCtrlCfg)-2);
    if (os_mut_wait (SPI_mutex, CTRL_TICK*2) != OS_R_TMO) {
        MRAM_Wr(CHIP1, u8ptr, DEVCFG_ADDR, sizeof(mCtrlCfg));
    } else {
        ret = 0x01;
    }

    return  (ret);
}
//--------------------------------------------------
//  判断设备（中位机）控制配置的合法性
/*
    返回  ret :   1－合法    0－非法
*/
//--------------------------------------------------
U8  Dev::CheckCtrlCfg(const strCtrlCfg *pstr)   // 检查控制配置合法性
{
    U8  ret = 0x00;

    if (pstr->MaxViBdNum > MAX_VI_BOARD)        return  (0x01); // 巡检电压板数
    if (pstr->MaxTiBdNum > MAX_TI_BOARD)        return  (0x01); // 巡检温度板数
    if (pstr->MaxIiBdNum > MAX_II_BOARD)        return  (0x01); // 开关量输入板数
    if (pstr->MaxOiBdNum > MAX_OI_BOARD)        return  (0x01); // 开关量输出板数

    if (pstr->MaxEBiBdNum > MAX_EBI_BOARD)      return  (0x01); // 扩展位输入板数
    if (pstr->MaxEBoBdNum > MAX_EBO_BOARD)      return  (0x01); // 扩展位输出板数
    if (pstr->MaxEDiBdNum > MAX_EDI_BOARD)      return  (0x01); // 扩展数据输入板数
    if (pstr->MaxEDoBdNum > MAX_EDO_BOARD)      return  (0x01); // 扩展数据输出板数

    if (pstr->BoardViNum > MAX_BOARDVI_NUM)     return  (0x01); // 板级巡检电压数
    if (pstr->BoardTiNum > MAX_BOARDTI_NUM)     return  (0x01); // 板级巡检温度数
    if (pstr->BoardIiNum > MAX_BOARDII_NUM)     return  (0x01); // 板级开关量输入数
    if (pstr->BoardOiNum > MAX_BOARDOI_NUM)     return  (0x01); // 板级开关量输出数

    if (pstr->BoardExBiNum > MAX_BOARDEBI_NUM)  return  (0x01); // 板级扩展位输入数
    if (pstr->BoardExBoNum > MAX_BOARDEBO_NUM)  return  (0x01); // 板级扩展位输出数
    if (pstr->BoardExDiNum > MAX_BOARDEDI_NUM)  return  (0x01); // 板级扩展数据输入数
    if (pstr->BoardExDoNum > MAX_BOARDEDO_NUM)  return  (0x01); // 板级扩展数据输出数

    if (pstr->CanOff_n > 100*CAN_TO_N)          return  (0x01); // CAN超时计数器
    if (pstr->CtrlRetry_n > 10*CTRL_RETRY_N)    return  (0x01); //（控制故障时）重试次数
    if (pstr->CanRetry_n > 10*CAN_RETRY_N)      return  (0x01); //（通信故障时）重试次数
                            
    if ((pstr->CtrlRetry_t > MAX_JDG_T) 
    ||  (pstr->CtrlRetry_t < CTRL_PERIOD))      return  (0x01); //（控制故障时）重试间隔（mS）
                            
    if ((pstr->CanRetry_t > MAX_JDG_T) 
    ||  (pstr->CanRetry_t < CTRL_PERIOD))       return  (0x01); //（通信故障时）重试间隔（mS）
                            
    if ((pstr->PDResume_t > MAX_JDG_T) 
    ||  (pstr->PDResume_t < CTRL_PERIOD))       return  (0x01); // 掉电恢复周期（mS）
                                
    if ((pstr->Vdelta > MAX_VDELTA)                             // 16-0113
    ||  (pstr->Vdelta < MIN_VDELTA))            return  (0x01); // 恒流转换电压控制差值（mV）
                            
    if (pstr->StartDly_t > MAX_JDG_T)           return  (0x01); // 启动间隔延时（mS）
                            
    if ((pstr->XCurJdg_t > MAX_JDG_T) 
    ||  (pstr->XCurJdg_t < SCAN_PERIOD))        return  (0x01); // 电流 异常判断周期 (mS)
                            
    if ((pstr->XVolJdg_t > MAX_JDG_T) 
    ||  (pstr->XVolJdg_t < SCAN_PERIOD))        return  (0x01); // 电压 异常判断周期 (mS)
                            
    if ((pstr->XTempJdg_t > MAX_JDG_T) 
    ||  (pstr->XTempJdg_t < SCAN_PERIOD))       return  (0x01); // 温度 异常判断周期 (mS)
                                
    if ((pstr->XViJdg_t > MAX_JDG_T)
    ||  (pstr->XViJdg_t < SCAN_PERIOD))         return  (0x01); // 单体电压 异常判断周期 (mS)
                                
    if ((pstr->XTiJdg_t > MAX_JDG_T) 
    ||  (pstr->XTiJdg_t < SCAN_PERIOD))         return  (0x01); // 单体温度 异常判断周期 (mS)
                                
    if ((pstr->IOJdg_t > MAX_JDG_T) 
    ||  (pstr->IOJdg_t < SCAN_PERIOD))          return  (0x01); // 开关量 判断响应周期 (mS)

    if ((pstr->VpotJdg_t > MAX_JDG_T) 
    ||  (pstr->VpotJdg_t < SCAN_PERIOD))        return  (0x01); // 平台电压 判断响应周期 (mS)

    if ((pstr->CurJdg_t > MAX_JDG_T) 
    ||  (pstr->CurJdg_t < SCAN_PERIOD))         return  (0x01); // 电流 判断响应周期 (mS)
                            
    if ((pstr->VolJdg_t > MAX_JDG_T) 
    ||  (pstr->VolJdg_t < SCAN_PERIOD))         return  (0x01); // 电压 判断响应周期 (mS)

    if ((pstr->PowJdg_t > MAX_JDG_T) 
    ||  (pstr->PowJdg_t < SCAN_PERIOD))         return  (0x01); // 功率 判断响应周期 (mS)
                            
    if ((pstr->EngyJdg_t > MAX_JDG_T) 
    ||  (pstr->EngyJdg_t < SCAN_PERIOD))        return  (0x01); // 瓦时 判断响应周期 (mS)

    if ((pstr->ViJdg_t > MAX_JDG_T) 
    ||  (pstr->ViJdg_t < SCAN_PERIOD))          return  (0x01); // 单体电压 判断响应周期 (mS)
                            
    if ((pstr->TiJdg_t > MAX_JDG_T) 
    ||  (pstr->TiJdg_t < SCAN_PERIOD))          return  (0x01); // 单体温度 判断响应周期 (mS)
                                
    if ((pstr->Record_t > MAX_JDG_T)
    ||  (pstr->Record_t < CTRL_PERIOD))         return  (0x01); // 默认记录间隔 (mS)

    if (pstr->Vi_Dot > MAX_DOT)                 return  (0x01); // 单体电压小数位数
    if (pstr->Ti_Dot > MAX_DOT)                 return  (0x01); // 单体温度小数位数

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//  加载辅助通道硬件配置
/*
    参数  无
    返回  ret :   1－成功    0－失败
    备注：
*/
//--------------------------------------------------
U8  Dev::LoadSubChCfg(void)
{
    U8  ret     = 0;
    U8  *pbox   = (U8 *)&mSubChCfg.Mark;

    MRAM_Rd(CHIP1, pbox, SUBCHANCFG_ADDR, sizeof(strSubChCfg));
    if ((Crc16(pbox, sizeof(strSubChCfg)) != 0) 
    ||  (mSubChCfg.Mark != SUBCHCFG_MARK)) 
    {
        mSubChCfg.Mark  = SUBCHCFG_MARK;    // 标记
//        mSubChCfg.ViBoardNum    = 0;
//        mSubChCfg.TiBoardNum    = 0;
//        mSubChCfg.OiBoardNum    = 0;
//        mSubChCfg.IiBoardNum    = 0;
        //test
        mSubChCfg.crc = Crc16((U8 *)&mSubChCfg, sizeof(strSubChCfg)-2);
//15-1214   MRAM_Wr(CHIP1, pbox, SUBCHANCFG_ADDR, sizeof(strSubChCfg));
    } else {
        ret = 1;    // 读取配置成功
    }

    return  (ret);
}
//--------------------------------------------------
//  判断辅助通道配置的合法性
/*
    返回  ret :   1－合法    0－非法
*/
//--------------------------------------------------
U8  Dev::CheckSubChCfg(const strSubChCfg *pstr) // 检查辅助通道硬件配置合法性
{
    if (pstr->Mark  != SUBCHCFG_MARK)           return  (0);
//    if (pstr->ViBoardNum  > MAX_VI_BOARD)       return  (0);    // 单体电压板数目
//    if (pstr->TiBoardNum  > MAX_TI_BOARD)       return  (0);    // 单体温度板数目
//    if (pstr->IiBoardNum  > MAX_II_BOARD)       return  (0);    // 开关量出板数目
//    if (pstr->OiBoardNum  > MAX_OI_BOARD)       return  (0);    // 开关量入板数目
//    if (pstr->XiBoardNum  > MAX_XI_BOARD)       return  (0);    // 其他扩展板数目

    return  (1);
}
//--------------------------------------------------
//  拷贝辅助通道硬件配置参数
/*
    参数  无
    返回  ret :   数据长度

    备注：
*/
//--------------------------------------------------
U16 Dev::CopySubHdCfg(U8 *ptr, U8 sid)
{
    U16 len     = 0x00;
    U16 tot_pts = 0;    // 指定通道类型的采样总点数
    U8  bd_num;         // 指定通道类型的最大板s数
    U8  i;
    U8  *u8ptr;

    switch (sid)
    {
    case VI_ID:
        bd_num  = mViHdCfg.UseNum & 0xff;
        u8ptr   = &mViHdCfg.HdTbl[0];
        break;

    case TI_ID:
        bd_num  = mTiHdCfg.UseNum & 0xff;
        u8ptr   = &mTiHdCfg.HdTbl[0];
        break;

    case II_ID:
        bd_num  = mIiHdCfg.UseNum & 0xff;
        u8ptr   = &mIiHdCfg.HdTbl[0];
        break;

    case OI_ID:
        bd_num  = mOiHdCfg.UseNum & 0xff;
        u8ptr   = &mOiHdCfg.HdTbl[0];
        break;

    case XI_ID:
        bd_num  = mXiHdCfg.UseNum & 0xff;
        u8ptr   = &mXiHdCfg.HdTbl[0];
        break;

    default:
        return  (0);
    }
    // 返回总点数
    *ptr++ = bd_num;
    for (i=0; i<bd_num; i++) {
        tot_pts += u8ptr[i];
    }
    *ptr++ = (tot_pts >> 0) & 0xff;
    *ptr++ = (tot_pts >> 8) & 0xff;
    len += 3;
    // 返回硬件配置表
    for (i=0; i< bd_num; i++) {
        *ptr++ = u8ptr[i] ;
        len++;
    }

    return  (len);
}
//--------------------------------------------------
//  保存辅助通道硬件配置参数
/*
    参数  无
    返回  ret :   1－成功    0－失败

    备注：
*/
//--------------------------------------------------
U8  Dev::SaveSubHdCfg(U8 *ptr, U8 sid)
{
    U8  ret = 0x00;
    U16 tot_pts1,
        tot_pts2;   // 指定通道类型的采样总点数
    U8  bd_num;     // 指定通道类型的最大板s数
    U8  max_pts = 0;// 配置的板级最大点数
    U8  i;
    U8  *u8ptr;
    U16 len;
    U32 adr;

    bd_num   = ptr[0];
    tot_pts1 = ptr[1] + (ptr[2] << 8);
    ptr     += 3;
    tot_pts2 = 0;
    for (i=0; i<bd_num; i++) {
        if (ptr[i] == 0) {      // 配置点数不能为0
            return  (RET_FRAME_ERR);
        }
        if (max_pts < ptr[i]) {
            max_pts = ptr[i];
        }
        tot_pts2 += ptr[i];
    }
    if (tot_pts2 != tot_pts1) { // PC下发的总点数与累计出的总点数不等
        return  (RET_FRAME_ERR);
    }
    switch (sid)
    {
    case VI_ID:
        if ((bd_num > MAX_VI_BOARD)         // 板数超界
        ||  (max_pts > MAX_BOARDVI_NUM)) {  // 板级点数超界
            return  (RET_FRAME_ERR);
        }
        mViHdCfg.UseNum = bd_num;
        for (i=0; i<bd_num; i++) {          // 拷贝硬件配置表
            mViHdCfg.HdTbl[i] = ptr[i];
        }
        mViHdChk.GblInx[0]  = 0;            // 生成索引
        for (i=1; i<bd_num; i++) {
            mViHdChk.GblInx[i]  = mViHdChk.GblInx[i-1] + mViHdCfg.HdTbl[i-1];
        }
        for (i=bd_num; i<MAX_VI_BOARD; i++) {   // 尾部未配置的表格处理
            mViHdCfg.HdTbl[i]   = 0;
            mViHdChk.GblInx[i]  = mViHdChk.GblInx[bd_num-1];
        }
        u8ptr = (U8 *)&mViHdCfg.Mark;
        len = sizeof(mViHdCfg);
        adr = VIHDCFG_ADDR;
        break;

    case TI_ID:
        if ((bd_num > MAX_TI_BOARD)         // 板数超界
        ||  (max_pts > MAX_BOARDTI_NUM)) {  // 板级点数超界
            return  (RET_FRAME_ERR);
        }
        mTiHdCfg.UseNum = bd_num;
        for (i=0; i<bd_num; i++) {          // 拷贝硬件配置表
            mTiHdCfg.HdTbl[i] = ptr[i];
        }
        mTiHdChk.GblInx[0]  = 0;            // 生成索引
        for (i=1; i<bd_num; i++)  {
            mTiHdChk.GblInx[i]  = mTiHdChk.GblInx[i-1] + mTiHdCfg.HdTbl[i-1];
        }
        for (i=bd_num; i<MAX_TI_BOARD; i++) {   // 尾部未配置的表格处理
            mTiHdCfg.HdTbl[i]   = 0;
            mTiHdChk.GblInx[i]  = mTiHdChk.GblInx[bd_num-1];
        }
        u8ptr = (U8 *)&mTiHdCfg.Mark;
        len = sizeof(mTiHdCfg);
        adr = TIHDCFG_ADDR;
        break;

    case II_ID:
        if ((bd_num > MAX_II_BOARD)         // 板数超界
        ||  (max_pts > MAX_BOARDII_NUM)) {  // 板级点数超界
            return  (RET_FRAME_ERR);
        }
        mIiHdCfg.UseNum = bd_num;
        for (i=0; i<bd_num; i++) {          // 拷贝硬件配置表
            mIiHdCfg.HdTbl[i] = ptr[i];
        }
        mIiHdChk.GblInx[0]  = 0;            // 生成索引
        for (i=1; i<bd_num; i++)  {
            mIiHdChk.GblInx[i]  = mIiHdChk.GblInx[i-1] + mIiHdCfg.HdTbl[i-1];
        }
        for (i=bd_num; i<MAX_II_BOARD; i++) {   // 尾部未配置的表格处理
            mIiHdCfg.HdTbl[i]   = 0;
            mIiHdChk.GblInx[i]  = mIiHdChk.GblInx[bd_num-1];
        }
        u8ptr = (U8 *)&mIiHdCfg.Mark;
        len = sizeof(mIiHdCfg);
        adr = IIHDCFG_ADDR;
        break;

    case OI_ID:
        if ((bd_num > MAX_OI_BOARD)         // 板数超界
        ||  (max_pts > MAX_BOARDOI_NUM)) {  // 板级点数超界
            return  (RET_FRAME_ERR);
        }
        mOiHdCfg.UseNum = bd_num;
        for (i=0; i<bd_num; i++) {          // 拷贝硬件配置表
            mOiHdCfg.HdTbl[i] = ptr[i];
        }
        mOiHdChk.GblInx[0]  = 0;            // 生成索引
        for (i=1; i<bd_num; i++)  {
            mOiHdChk.GblInx[i]  = mOiHdChk.GblInx[i-1] + mOiHdCfg.HdTbl[i-1];
        }
        for (i=bd_num; i<MAX_OI_BOARD; i++) {   // 尾部未配置的表格处理
            mOiHdCfg.HdTbl[i]   = 0;
            mOiHdChk.GblInx[i]  = mOiHdChk.GblInx[bd_num-1];
        }
        u8ptr = (U8 *)&mOiHdCfg.Mark;
        len = sizeof(mOiHdCfg);
        adr = OIHDCFG_ADDR;
        break;

    case XI_ID:
        if ((bd_num > MAX_XI_BOARD)         // 板数超界
        ||  (max_pts > MAX_BOARDXI_NUM)) {  // 板级点数超界
            return  (RET_FRAME_ERR);
        }
        mXiHdCfg.UseNum = bd_num;
        for (i=0; i<bd_num; i++) {          // 拷贝硬件配置表
            mXiHdCfg.HdTbl[i] = ptr[i];
        }
        mXiHdChk.GblInx[0]  = 0;            // 生成索引
        for (i=1; i<bd_num; i++) {
            mXiHdChk.GblInx[i]  = mXiHdChk.GblInx[i-1] + mXiHdCfg.HdTbl[i-1];
        }
        for (i=bd_num; i<MAX_XI_BOARD; i++) {   // 尾部未配置的表格处理
            mXiHdCfg.HdTbl[i]   = 0;
            mXiHdChk.GblInx[i]  = mXiHdChk.GblInx[bd_num-1];
        }
        u8ptr = (U8 *)&mXiHdCfg.Mark;
        len = sizeof(mXiHdCfg);
        adr = XIHDCFG_ADDR;
        break;

    default:
        return  (RET_FRAME_ERR);
    }
    // 保存到MRAM
    U16 crc_buf;

    crc_buf = Crc16(u8ptr, len-2);
    u8ptr[len-2] = (crc_buf >> 0) & 0xff;
    u8ptr[len-1] = (crc_buf >> 8) & 0xff;
    if (os_mut_wait (SPI_mutex, CTRL_TICK*2) != OS_R_TMO) {
        MRAM_Wr(CHIP1, u8ptr, adr, len);
        os_mut_release (SPI_mutex);
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//  加载指定辅助通道硬件配置
/*
    参数  无
    返回  ret :   1－成功    0－失败

    备注：
*/
//--------------------------------------------------
U8  Dev::LoadSchHdCfg(U8 type)
{
    U8  ret = 0;
    U8  *ptr;
    U16 len, i;
    U32 adr, mark;

    switch (type) 
    {
    case VI_ID:
        ptr = (U8 *)&mViHdCfg.Mark;
        len = sizeof(mViHdCfg);
        adr = VIHDCFG_ADDR;
        mark= VIHDCFG_MARK;
        break;
    case TI_ID:
        ptr = (U8 *)&mTiHdCfg.Mark;
        len = sizeof(mTiHdCfg);
        adr = TIHDCFG_ADDR;
        mark= TIHDCFG_MARK;
        break;
    case II_ID:
        ptr = (U8 *)&mIiHdCfg.Mark;
        len = sizeof(mIiHdCfg);
        adr = IIHDCFG_ADDR;
        mark= IIHDCFG_MARK;
        break;
    case OI_ID:
        ptr = (U8 *)&mOiHdCfg.Mark;
        len = sizeof(mOiHdCfg);
        adr = OIHDCFG_ADDR;
        mark= OIHDCFG_MARK;
        break;
    case XI_ID:
        ptr = (U8 *)&mXiHdCfg.Mark;
        len = sizeof(mXiHdCfg);
        adr = XIHDCFG_ADDR;
        mark= XIHDCFG_MARK;
        break;
    default:
        return  (0);
    }

    //读取系统控制配置
    MRAM_Rd(CHIP1, ptr, adr, len);
    if ((Crc16(ptr, len) != 0) 
    ||  (CheckSchHdCfg(type, ptr))
    ||  (*(U32*)ptr != mark)) {
        switch (type) 
        {
        // 单体电压硬件配置
        case VI_ID:
            mViHdCfg.Mark   = mark;
            mViHdCfg.UseNum = DEF_VI_BOARD;//MAX_VI_BOARD;  // 配置数
            for (i=0; i<MAX_VI_BOARD; i++) {    // 板点数
                mViHdCfg.HdTbl[i] = DEF_BOARDVI_NUM;
            }
            break;

        // 单体温度硬件配置
        case TI_ID:
            mTiHdCfg.Mark   = mark;
            // 根据当前设备结构初始化单体温度硬件配置 16-0514
            switch (cDev.mInfo.dev_str_type)
            {
                default:
                case DS_GENERAL:        // 通用类型设备结构
                {
                    mTiHdCfg.UseNum = DEF_TI_BOARD;     // 配置板数
                    for (i=0; i<MAX_TI_BOARD; i++) {    // 板级点数
                        mTiHdCfg.HdTbl[i] = DEF_BOARDTI_NUM;
                    }
                    break;
                }
            }
            break;

        // 开关量输入硬件配置
        case II_ID:
            mIiHdCfg.Mark = mark;
            // 根据当前设备结构初始化开关量入硬件配置 16-0514
            switch (cDev.mInfo.dev_str_type)
            {
                default:
                case DS_GENERAL:        // 通用类型设备结构
                {
                    mIiHdCfg.UseNum = DEF_II_BOARD;     // 配置板数
                    for (i=0; i<MAX_II_BOARD; i++)  {   // 板级点数
                        mIiHdCfg.HdTbl[i] = DEF_BOARDII_NUM;
                    }
                    break;
                }
            }
            break;
            
        // 开关量输出硬件配置
        case OI_ID:
            mOiHdCfg.Mark = mark;
            // 根据当前设备结构初始化开关量出硬件配置 16-0514
            switch (cDev.mInfo.dev_str_type)
            {
                default:
                case DS_GENERAL:        // 通用类型设备结构
                {
                    mOiHdCfg.UseNum = DEF_OI_BOARD;     // 配置板数
                    for (i=0; i<MAX_OI_BOARD; i++)  {   // 板级点数
                        mOiHdCfg.HdTbl[i] = DEF_BOARDOI_NUM;
                    }
                    break;
                }
            }        
        
////==================================================
//#if     (DEV_STR_TYPE == DS_GENERAL)    // 通用结构 开关量输出硬件配置
////==================================================

//            mOiHdCfg.UseNum = DEF_OI_BOARD;     // 配置板数
//            for (i=0; i<MAX_OI_BOARD; i++)  {   // 板级点数
//                mOiHdCfg.HdTbl[i] = DEF_BOARDOI_NUM;
//            }

////==================================================
//#elif   (DEV_STR_TYPE == DS_NEILSBED)   // 针床式 开关量输出硬件配置
////==================================================

//            mOiHdCfg.UseNum = CAB_ZONE_NUM;     // 配置板数 = 针床区数
//            for (i=0; i<MAX_OI_BOARD; i++)  {   // 板点数 = 区通道数
//                mOiHdCfg.HdTbl[i] = ZONE_CHAN_NUM;
//            }

////==================================================
//#elif   (DEV_STR_TYPE == DS_POURING)    // 倒出式 开关量输出硬件配置
////==================================================

//            mOiHdCfg.UseNum = DEF_OI_BOARD;     // 配置板数
//            for (i=0; i<MAX_OI_BOARD; i++)  {   // 板级点数
//                mOiHdCfg.HdTbl[i] = DEF_BOARDOI_NUM;
//            }

////==================================================
//#elif   (DEV_STR_TYPE == DS_OPENSHELF)  // 开架式 开关量输出硬件配置
////==================================================

//            mOiHdCfg.UseNum = SHELF_TOTBD_NUM;    // 配置板数 = 托架数(实际为辅助PCB板数)
//            for (i=0; i<MAX_OI_BOARD; i++)  {   // 板点数 = 托架通道数(实际为辅助PCB板级开关量数)
//                mOiHdCfg.HdTbl[i] = SHELF_LOUT_NUM;
//            }

////==================================================            
//#else       // 其他，默认
////==================================================            
//  
//            mOiHdCfg.UseNum = DEF_OI_BOARD;     // 配置板数
//            for (i=0; i<MAX_OI_BOARD; i++)  {   // 板级点数
//                mOiHdCfg.HdTbl[i] = DEF_BOARDOI_NUM;
//            }
//            
//#endif
            break;
            
        // 其他项硬件配置 15-1229 用作扩展IO一一映射
        case XI_ID:
            mXiHdCfg.Mark   = mark;
            mXiHdCfg.UseNum = DEF_XI_BOARD; //MAX_XI_BOARD; // 配置数
            for (i=1; i<MAX_XI_BOARD; i++)  {   // 板点数
                mXiHdCfg.HdTbl[i] = DEF_BOARDXI_NUM;
            }
            break;
        default:
            return  (0);
        }
            
        //test
        U16 crc_buf;
        
        crc_buf = Crc16(ptr, len-2);
        ptr[len-2] = (crc_buf >> 0) & 0xff;
        ptr[len-1] = (crc_buf >> 8) & 0xff;
//15-1214   MRAM_Wr(CHIP1, ptr, adr, len);
    }
    // 生成索引表
    switch (type) 
    {
    case VI_ID:
        mViHdChk.GblInx[0] = 0;     // 全局索引
        for (i=1; i<MAX_VI_BOARD; i++)  {
            mViHdChk.GblInx[i] = mViHdChk.GblInx[i-1] + mViHdCfg.HdTbl[i-1];
        }
        break;
    case TI_ID:
        mTiHdChk.GblInx[0] = 0;     // 全局索引
        for (i=1; i<MAX_TI_BOARD; i++)  {
            mTiHdChk.GblInx[i] = mTiHdChk.GblInx[i-1] + mTiHdCfg.HdTbl[i-1];
        }
        break;
    case II_ID:
        mIiHdChk.GblInx[0] = 0;     // 全局索引
        for (i=1; i<MAX_II_BOARD; i++)  {
            mIiHdChk.GblInx[i] = mIiHdChk.GblInx[i-1] + mIiHdCfg.HdTbl[i-1];
        }
        break;
    case OI_ID:
        mOiHdChk.GblInx[0] = 0;     // 全局索引
        for (i=1; i<MAX_OI_BOARD; i++)  {
            mOiHdChk.GblInx[i] = mOiHdChk.GblInx[i-1] + mOiHdCfg.HdTbl[i-1];
        }
        break;
    case XI_ID:
        mXiHdChk.GblInx[0] = 0;     // 全局索引
        for (i=1; i<MAX_XI_BOARD; i++)  {
            mXiHdChk.GblInx[i] = mXiHdChk.GblInx[i-1] + mXiHdCfg.HdTbl[i-1];
        }
        break;
    default:
        return  (0);
    }

    return  (ret);
}
//--------------------------------------------------
//  拷贝指定辅助通道配置参数
/*
    参数  无
    返回  ret :   数据长度

    备注：
*/
//--------------------------------------------------
U16 Dev::CopySchHdCfg(U8 type, U8 *)
{
    U16 len = 0;

    return  (len);
}
//--------------------------------------------------
//  保存指定辅助通道配置参数
/*
    参数  无
    返回  ret :   1－成功    0－失败

    备注：
*/
//--------------------------------------------------
U8  Dev::SaveSchHdCfg(U8 type, U8 *)
{
    U8  ret = 0;

    return  (ret);
}
//--------------------------------------------------
//  判断辅助通道配置的合法性
/*
    返回  ret :   1－合法    0－非法
*/
//--------------------------------------------------
U8  Dev::CheckSchHdCfg(U8 type, const U8 *ptr)
{
    U8  ret = 0;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//  初始化加载辅助通道软配置表
/*
    参数  无
    返回  ret :   1－成功    0－失败
    备注： 指给主通道配置的各个辅助通道采样点的信息表
*/
//--------------------------------------------------
U8  Dev::LoadSampCfg(void)
{
    U8  ret     = 0;
    U8  *pbox   = (U8 *)&mSampCfg.Mark;
    U32 i;

    MRAM_Rd(CHIP1, pbox, SAMPTBLCFG_ADDR, sizeof(strSampCfg));
    if ((Crc16(pbox, sizeof(strSampCfg)) != 0) 
    ||  (mSampCfg.Mark != SAMPCFG_MARK)
    ||  (CheckSampTbl(&mSampCfg.ch[0], MAX_CHAN_NUM))) // 15-0119
    {
        mSampCfg.Mark   = SAMPCFG_MARK;     // 标记
        mSampCfg.rev    = 0;
        for (i=0; i<MAX_CHAN_NUM; i++) {    // 初始化辅助通道软配置表
            mSampCfg.ch[i].ViOffset = 0;    // 单体电压号偏移
            mSampCfg.ch[i].ViNum    = 0;    // 单体电压数

            mSampCfg.ch[i].TiOffset = 0;    // 单体温度号偏移
            mSampCfg.ch[i].TiNum    = 0;    // 单体温度数

            mSampCfg.ch[i].IiOffset = 0;    // 开关量输入偏移
            mSampCfg.ch[i].IiNum    = 0;    // 开关量输入数

            mSampCfg.ch[i].OiOffset = 0;    // 开关量输出偏移
            mSampCfg.ch[i].OiNum    = 0;    // 开关量输出数

            mSampCfg.ch[i].XiOffset = 0;    // 其他扩展数据偏移
            mSampCfg.ch[i].XiNum    = 0;    // 其他扩展数据数
        }
        //test
        mSampCfg.crc = Crc16((U8 *)&mSampCfg, sizeof(strSampCfg)-2);
//15-1214   MRAM_Wr(CHIP1, pbox, SAMPTBLCFG_ADDR, sizeof(strSampCfg));
    } else {
        ret = 1;    // 读取配置成功
    }

    return  (ret);
}
//--------------------------------------------------
//  PC获取辅助通道软配置参数
/*
    参数  无
    返回  ret :   数据长度
    备注：
*/
//--------------------------------------------------
U16 Dev::CopySampCfg(U8 *ptr)
{
    U16 len = 0x00;
//    U8  i;
//    U8  *u8ptr = &mCtrlCfg.bPdResumeMode;

//    for (i=0; i< sizeof(mCtrlCfg)-6; i++) { // 长度去MARK+CRC共6字节
//        ptr[i]  = u8ptr[i] ;
//        len++;
//    }

    return  (len);
}
//--------------------------------------------------
//  保存辅助通道软配置表
/*
    参数  无
    返回  ret :   1－成功    0－失败
    备注：
*/
//--------------------------------------------------
U8  Dev::SaveSampCfg(U8 *ptr)
{
    U8  ret = 0;

    return  (ret);
}
//--------------------------------------------------
//  检查辅助通道软配置表合法性
/*
    返回  ret :   0－合法    非零－非法
    2015-0119 modified
*/
//--------------------------------------------------
U8  Dev::CheckSampTbl(const strSampTbl *pstr, U8 num)
{
    U32 i, sum[5] = {0,0,0,0,0};

    if ((num > MAX_CHAN_NUM) 
    ||  (num == 0)) {   // 点数错误
        return  (0x01);
    }
    for (i=0; i<num; i++) {
        // 主通道对应配置点数判断
        if ((pstr[i].ViNum > MAX_CHANVI_NUM)
        ||  (pstr[i].TiNum > MAX_CHANTI_NUM)
        ||  (pstr[i].IiNum > MAX_CHANII_NUM)
        ||  (pstr[i].OiNum > MAX_CHANOI_NUM)
        ||  (pstr[i].XiNum > MAX_CHANXI_NUM)) {
            return  (0x02); // 主通道点数超界
        }
        // 主通道对应配置第一点在总硬件表中偏移判断
        if (i == 0) {       // 第一主通道
            if ((pstr[i].ViOffset != 0)
            ||  (pstr[i].TiOffset != 0)
            ||  (pstr[i].IiOffset != 0)
            ||  (pstr[i].OiOffset != 0)
            ||  (pstr[i].XiOffset != 0)) {
                return  (0x03); // 主通道索引错误
            }
        } else {    // 其他主通道
            if ((pstr[i].ViOffset != pstr[i-1].ViOffset + pstr[i-1].ViNum)
            ||  (pstr[i].TiOffset != pstr[i-1].TiOffset + pstr[i-1].TiNum)
            ||  (pstr[i].IiOffset != pstr[i-1].IiOffset + pstr[i-1].IiNum)
            ||  (pstr[i].OiOffset != pstr[i-1].OiOffset + pstr[i-1].OiNum)
            ||  (pstr[i].XiOffset != pstr[i-1].XiOffset + pstr[i-1].XiNum)) {
                return  (0x03); // 主通道索引错误
            }
        }
        sum[0] += pstr[i].ViNum;
        sum[1] += pstr[i].TiNum;
        sum[2] += pstr[i].IiNum;
        sum[3] += pstr[i].OiNum;
        sum[4] += pstr[i].XiNum;
    }
    // 总点数判断
    if ((sum[0] > MAX_VI_BOARD * MAX_BOARDVI_NUM) 
    ||  (sum[1] > MAX_TI_BOARD * MAX_BOARDTI_NUM)
    ||  (sum[2] > MAX_II_BOARD * MAX_BOARDII_NUM)
    ||  (sum[3] > MAX_OI_BOARD * MAX_BOARDOI_NUM)
    ||  (sum[4] > MAX_XI_BOARD * MAX_BOARDXI_NUM)) {
        return  (0x04); // 主通道总点数错误
    }

    return  (0x00);
}
////----------------------------------------------------------------------------------------------------
////    加载温度配置表
////----------------------------------------------------------------------------------------------------
//U8  Dev::LoadTempTbl(void)
//{
//    U8  ret     = 0;
//    U8  *pbox   = (U8 *)&mTempTbl.Mark;
//    U16 i,j;


//    MRAM_Rd(CHIP1, pbox, TEMPTBL_ADDR, sizeof(mTempTbl));
//    if ((Crc16(pbox, sizeof(mTempTbl)) != 0) 
//    ||  (mTempTbl.Mark != TEMPTBL_MARK)) 
//    {   //有误，设定默认参数
//        mTempTbl.Mark = TEMPTBL_MARK; //标记
//        for (i=0; i<MAX_CHAN_NUM; i++) {
//            cChan[i].SetTiNum(0);
//            for (j=0; j<MAX_CHANTI_NUM; j++) {
//                mTempTbl.tbl[i][j] = 0;
//            }
//        }
//        //test
//        mTempTbl.crc = Crc16((U8 *)&mTempTbl, sizeof(mTempTbl)-2);
//        MRAM_Wr(CHIP1, pbox, TEMPTBL_ADDR, sizeof(mTempTbl));
//    } else {
//        ret = 1;    //读取配置成功
//    }

//    return  (ret);
//}
////----------------------------------------------------------------------------------------------------
////  保存温度配置表
////----------------------------------------------------------------------------------------------------
//U8  Dev::SaveTempTbl(U8 *ptr)
//{
//    U8  ret = 0;

//    return  (ret);
//}
////----------------------------------------------------------------------------------------------------
////  检查温度配置表合法性
////----------------------------------------------------------------------------------------------------
//U8  Dev::CheckTempTbl(const strTempTbl *pstr)
//{
//    U8  ret = 0;

//    return  (ret);
//}
//----------------------------------------------------------------------------------------------------
//  初始化网络层信息
/*
    参数  无
    返回  ret :   1－成功    0－失败
    备注：
*/
//----------------------------------------------------------------------------------------------------
U8  Dev::LoadNetInfo(void)
{
    U8  ret     = 0;
    U8  *pbox   = (U8 *)&mNetInfo.Mark;

    MRAM_Rd(CHIP1, pbox, NETINFO_ADDR, sizeof(mNetInfo));
    if ((Crc16(pbox, sizeof(mNetInfo)) != 0) 
    ||  (mNetInfo.Mark != NETINFO_MARK)
    ||  (mNetInfo.Ip[0] == 0xff)
    ||  (mNetInfo.Ip[1] == 0xff)
    ||  (mNetInfo.Ip[2] == 0xff)
    ||  (mNetInfo.Ip[3] == 0xff)    // 不允许设置广播IP地址
    ||  (mNetInfo.Port  == 0x0000)) // 端口号为0
    {
        mNetInfo = DEFAULT_NETINFO;
        mNetInfo.crc = Crc16((U8 *)&mNetInfo, sizeof(mNetInfo)-2);
        MRAM_Wr(CHIP1, pbox, NETINFO_ADDR, sizeof(mNetInfo));
    } else {
        ret = 1;    //读取配置成功
    }
    //刷新网络配置
    ReflashNetCfg();

    return  (ret);
}
//--------------------------------------------------
//  刷新网络配置
/*
    参数  无
    返回  无
    备注：
*/
//--------------------------------------------------
void Dev::ReflashNetCfg(void)
{
    U8  i;

    for (i=0; i<6; i++) {
        own_hw_adr[i] = cDev.mNetInfo.MAC[i];   //cDev.mInfo.dev_mac[i];
    }

    for (i=0; i<4; i++) {
        nlocalm[0].IpAdr[i]     = cDev.mNetInfo.Ip[i];
        nlocalm[0].DefGW[i]     = cDev.mNetInfo.GW[i];
        nlocalm[0].NetMask[i]   = cDev.mNetInfo.Mask[i];
        nlocalm[0].PriDNS[i]    = cDev.mNetInfo.DNS1[i];
        nlocalm[0].SecDNS[i]    = cDev.mNetInfo.DNS2[i];
    }
    Loc_PORT = cDev.mNetInfo.Port;
}
//--------------------------------------------------
//  判断网络层信息的合法性
/*
    返回  ret :   1－合法    0－非法
*/
//--------------------------------------------------
U8  Dev::CheckNetInfo(const strLocalM *pstr)
{
    if (pstr->Mark != NETINFO_MARK) return  (0);

    return  (1);
}
////--------------------------------------------------
////  实时记录数据收发控制初始化
///*
//    返回  null
//*/
////--------------------------------------------------
//void Dev::Init_RealRecCtrl(void)
//{
//    U32 i;

//    mRealRecCtrl.realCh = 0;    // 实时数据扫描通道号
//    mRealRecCtrl.recCh  = 0;    // 上传记录起扫描通道号
//    for (i=0; i<mInfo.chan_num; i++) {
//        mRealRecCtrl.packLen[i] = cChan[i].GetLoadPackLen(FIXED_DATA_LEN);  // 实时或明细包长度
//        mRealRecCtrl.recNum[i]  = 0;    // 缓冲中未读记录个数
//    }
//}
//--------------------------------------------------
//  获取实时时钟
/*
    返回  null
*/
//--------------------------------------------------
void Dev::GetRTC(void)
{

}

//--------------------------------------------------
//  远程PC - 主从机判断
//  返回    低8位：远程PC属性 - 主机，从机，或其他 
//          高8位：从机所注册的缓冲号
//-------------------------------------------------
U16 Dev::Test_RemotePC(const U8 *ipaddr)
{
    U16 ret = 0x0000;
    U8  idle_img = 0;

    if ((ipaddr[0] == mHostCtrl.ip[0])
    &&  (ipaddr[1] == mHostCtrl.ip[1])
    &&  (ipaddr[2] == mHostCtrl.ip[2])
    &&  (ipaddr[3] == mHostCtrl.ip[3])) {   // 是主机
        ret = MASTER_PC;
    } else {                                // 非主机
        for (int i=0; i<MAX_SLAVE_PC; i++) {
            if (mPCSlaCtrl[i].flag == 0x01) {   // 已经注册，判断IP地址是否一致
                if ((ipaddr[0] == mPCSlaCtrl[i].ip[0])
                &&  (ipaddr[1] == mPCSlaCtrl[i].ip[1])
                &&  (ipaddr[2] == mPCSlaCtrl[i].ip[2])
                &&  (ipaddr[3] == mPCSlaCtrl[i].ip[3])) {   // 是从机,清超时
                    mPCSlaCtrl[i].tout  = 0;
                    ret = SLAVE_PC + U16HMSK[i];
                    break;
                }
            } else {    // 没有注册，置相应位
                idle_img |= B8_MASK_N[i];   // 标记空闲从机缓冲
            }
        }
        if ((ret == 0) && (idle_img != 0)) {    // 注册空闲的缓冲
            for (int i=0; i<MAX_SLAVE_PC; i++) {
                if (idle_img & B8_MASK_N[i]) {
                    mPCSlaCtrl[i].flag  = 0x01; // 注册
                    mPCSlaCtrl[i].ch    = 0;
                    mPCSlaCtrl[i].ip[0] = ipaddr[0];
                    mPCSlaCtrl[i].ip[1] = ipaddr[1];
                    mPCSlaCtrl[i].ip[2] = ipaddr[2];
                    mPCSlaCtrl[i].ip[3] = ipaddr[3];
                    mPCSlaCtrl[i].tout  = 0;
                    ret = SLAVE_PC + U16HMSK[i];
                    break;
                }
            }
        }
    }

    return  (ret);
}
//--------------------------------------------------
//  清空PC从机控制缓冲
/*
    参数  ctrl_no - 缓冲号
*/
//--------------------------------------------------
U8 Dev::EmptyPCSlaCtrl(U8 ctrl_no)
{
    U8 ret = 0;

    if (ctrl_no < MAX_SLAVE_PC) {
        mPCSlaCtrl[ctrl_no].flag    = 0x00; // 注册
        mPCSlaCtrl[ctrl_no].ch      = 0;
        mPCSlaCtrl[ctrl_no].ip[0]   = 0;
        mPCSlaCtrl[ctrl_no].ip[1]   = 0;
        mPCSlaCtrl[ctrl_no].ip[2]   = 0;
        mPCSlaCtrl[ctrl_no].ip[3]   = 0;
        mPCSlaCtrl[ctrl_no].tout    = SLAVE_TOUT + 1;
        ret = 0x01;
    }

    return  (ret);
}
//--------------------------------------------------
//  PC从机通信超时控制
/*
    返回  null
*/
//--------------------------------------------------
void Dev::PCSlaTout(void)
{
    for (int i=0; i<MAX_SLAVE_PC; i++) {
        if (mPCSlaCtrl[i].flag == 0x01) {
            if (++mPCSlaCtrl[i].tout >= SLAVE_TOUT) {   //PC从机超时清标记
                mPCSlaCtrl[i].flag  = 0;
                mPCSlaCtrl[i].ch    = 0;
                mPCSlaCtrl[i].ip[0] = 0;
                mPCSlaCtrl[i].ip[1] = 0;
                mPCSlaCtrl[i].ip[2] = 0;
                mPCSlaCtrl[i].ip[3] = 0;
            }
        }
    }
}
//--------------------------------------------------
//  全局故障置位
//--------------------------------------------------
void Dev::SetGblFault(U32 img)
{
    mGblFault   |= img;
}
//--------------------------------------------------
//  全局故障复位
//--------------------------------------------------
void Dev::RstGblFault(U32 img)
{
    mGblFault &= ~img;
}
//--------------------------------------------------
//  获取全局故障信息
//--------------------------------------------------
U32 Dev::GetGblFault(void) const
{
    return  (mGblFault);
}
//--------------------------------------------------
//  获取扩展功能位信息
//--------------------------------------------------
U8  TstDevExfun(U32 fbit)
{
    U8  ret = 0;
    
    if (cDev.mInfo.exfunc_cfg.word & fbit) {
        ret = 1;
    }
    
    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  外部SRAM硬件故障测试
//----------------------------------------------------------------------------------------------------
void SRAM_Test(void)
{
    U8 i;

//--------------------------------------------------    
#ifdef  EN_SRAM_TEST  // 使能SRAM测试 

    const U32 B32MSK_TBL[32] = 
    {
      0x00000001,0x00000002,0x00000004,0x00000008,0x00000010,0x00000020,0x00000040,0x00000080,
      0x00000100,0x00000200,0x00000400,0x00000800,0x00001000,0x00002000,0x00004000,0x00008000,
      0x00010000,0x00020000,0x00040000,0x00080000,0x00100000,0x00200000,0x00400000,0x00800000,
      0x01000000,0x02000000,0x04000000,0x08000000,0x10000000,0x20000000,0x40000000,0x80000000
    };
    static  U32 err[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    U32 addr,
        *p32wr,
        *p32rd;

    gSramErr = 0;// 15-0629
    LED_AllOn();

    p32wr =
    p32rd = (U32 *)LSB_ADDR;

    for (int i=0; i<32; i++) {
        for (addr = LSB_ADDR; addr < MSB_ADDR; addr += 4) {
            *p32wr = B32MSK_TBL[i]; // 0x461c4000 ~ float: 10.000
            if (*p32rd != B32MSK_TBL[i]) {
                err[i]++;
                gSramErr = 1;
            }
            p32wr++;
            p32rd++;
        }
        p32wr =
        p32rd = (U32 *)LSB_ADDR;
        for (addr = LSB_ADDR; addr < MSB_ADDR; addr += 4) {
            *p32wr = ~B32MSK_TBL[i];
            if (*p32rd != ~B32MSK_TBL[i]) {
                err[i]++;
                gSramErr = 1;
            }
            p32wr++;
            p32rd++;
        }
    }

#endif
//--------------------------------------------------
    
    for (i=0; i<cDev.mInfo.chan_num; i++) {
        if (cChan[i].Test_PdMark() == 0) {
            cDev.SetGblFault(GBL_SRAM_FAULT);   //SRAM硬件故障
            break;
        }
    }
}
//----------------------------------------------------------------------------------------------------
//  IP地址复位判断    16-0602
//----------------------------------------------------------------------------------------------------
void IP_Reset_Test(void)
{
    static  U32 tst_cnt = 0;
            U32 tst_bit = FuncSel_ReadWord();
    
    if ((cDev.mNetInfo.Ip[0] == 192)
    &&  (cDev.mNetInfo.Ip[1] == 168)
    &&  (cDev.mNetInfo.Ip[2] == 0)) {   // 192.168.0 段地址不允许复位
        tst_cnt = 0;
        return  ;
    }
//    if (cDev.Test_AnyChRunning()) {     // 有通道运行，不允许复位
//        tst_cnt = 0;
//        return  ;      
//    }
    if (tst_bit & FUNC_IPADDRRST) {
        if (tst_cnt++ > 30) {  // 滤波到，复位
            tst_cnt = 31;
            
            if (os_mut_wait (SPI_mutex, 0) != OS_R_TMO) {
       
                cDev.mNetInfo.Ip[0]  = 192;   // IP地址
                cDev.mNetInfo.Ip[1]  = 168;
                cDev.mNetInfo.Ip[2]  = 0;
                cDev.mNetInfo.Ip[3]  = 195;
                
                cDev.mNetInfo.crc    = Crc16((U8 *)&cDev.mNetInfo, sizeof(cDev.mNetInfo)-2);
                MRAM_Wr(CHIP1, (U8 *)&cDev.mNetInfo, NETINFO_ADDR, sizeof(cDev.mNetInfo));
                
                LED_On(LD2);
                LED_On(LD3);
                LED_On(LD4);
                LED_On(LD5);
                LED_On(LD6);
                LED_On(LD7);
                LED_On(LD8);
                LED_On(LD9);
                
                // 设置成功，开启看门狗，等待复位
                IWdg_Init();
            }            
        }
    } else {
        tst_cnt = 0;
    }
}
//----------------------------------------------------------------------------------------------------
//  初始化设备信息
//----------------------------------------------------------------------------------------------------
void Init_Dev(void)
{
//  ManualJump(cDev.GetGblFault()); //causing hard default

    cDev.LoadCtrlCfg();         // 加载控制配置参数

    cDev.LoadDevInfo();         // 加载设备信息
    cDev.LoadHostCtrl();        // 加载主机信息
    cDev.LoadSubChCfg();        // 加载辅助通道配置

    cDev.LoadSchHdCfg(VI_ID);   // 加载电压通道硬件配置
    cDev.LoadSchHdCfg(TI_ID);   // 加载温度通道硬件配置
    cDev.LoadSchHdCfg(II_ID);   // 加载输入通道硬件配置
    cDev.LoadSchHdCfg(OI_ID);   // 加载输出通道硬件配置
    cDev.LoadSchHdCfg(XI_ID);   // 加载扩展通道硬件配置 15-1229 added

    cDev.LoadSampCfg();         // 加载辅助通道配置表

    cDev.LoadNetInfo();         // 加载本机MAC,IP,端口
}
//----------------------------------------------------------------------------------------------------
//eof
