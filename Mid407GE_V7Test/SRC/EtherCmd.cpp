//----------------------------------------------------------------------------------------------------
// Header:      具体命令解析
// File Name:   APP_CMD_Proc.c
// Author:      David
// Date:        2012-10-09
//----------------------------------------------------------------------------------------------------
#define     IN_ETHERCMD
extern "C" 
{
    #include    "rtl.h"
    #include    <string.h>
    #include    "main.h"
    #include    "rtx_can.h"
    #include    "app_config.h"
    #include    "sys_config.h"
    #include    "spi.h"
    #include    "watchdog.h"
    #include    "led.h"
}
#include    "ethercmd.hpp"
#include    "Dev.hpp"
#include    "parallelchan.hpp"  // 16-0122 以太网命令并联相关处理
#include    "chan.hpp"
#include    "chanEther.hpp"
#include    "subchan.hpp"
#include    "app_can.hpp"
//----------------------------------------------------------------------------------------------------
//====================================================================================================
//----------------------------------------------------------------------------------------------------
//0x80 - 联机命令处理
//----------------------------------------------------------------------------------------------------
U8  CMD_CONNECTION_Proc(U8 *ptr, U16 *ret_len, const U8 *pip)
{
    U8  ret     = 0;
    U16 rem_at  = 0;
    U16 len, port_buf;

    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) == MASTER_PC) {
        port_buf = ptr[0] + ptr[1]*0x100;   //主机保存端口号
        if (port_buf != cDev.mHostCtrl.port) {
            cDev.mHostCtrl.port = port_buf;
            cDev.SaveHostCtrl();
        }
    }
    //读取设备信息
    len = cDev.Rd_DevInfo(ptr,(U8)rem_at);

//    if (len == 0) { // 实际跑不到 16-0802
//        ret = L3_LEN_ERR;
//    }
    *ret_len = len;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x81 - 读取实时/明细数据  注：明细即记录
/*
    实时、明细数据调度方法
    (1) 实时数据采用一次传所有通道(程序暂时没有考虑溢出分片处理，假定一次都能传完)
    (2) 明细数据采用回扫方式，直到数据包长度不够一条通道记录(可能最后扫到的通道记录长度很长，缓冲不够用它用但够其他通道用，
        程序也会结束打包并上传)
    (3) 如果遇到任何一个通道的过程记录，不管长度有没有用完，结束打包并上传
    (4) 明细数据第一个回扫通道为上次命令扫到通道的下一个
2015-0314
    (5) 实时数据，明细数据分时处理--即每次收到要么处理实时，要么处理明细
2015-0318   调整
2015-1123   长度固定,由设备信息中设定,一旦设定就固化
*/
//----------------------------------------------------------------------------------------------------
#define     REALDATA_MARK   (0x01)      // 读实时数据标记
#define     RECDATA_MARK    (0x02)      // 读记录数据标记
#define     INITREC_MARK    (0x03)      // 读记录数据(并初始化翻滚)标记
//-----------------------------
#define     MAX_REPEAT_NUM  (4)         // 明细数据最大重传次数（达到最大值，清零更新记录缓冲）
//-----------------------------
U8  ProcDataBuf[1500];      // 过程记录数据缓冲
//--------------------------------------------------
U8  CMD_RD_REALDATA_Proc(U8 *ptr, U16 *ret_len, U16 cutlen, const U8 *pip)
{
    U8  ret = 0;
    U8  i;
    U8  emptyCh = 0;        // 空记录通道计数器
    U8  procNum = 0;        // 过程记录条数
    U8  type    = ptr[0];   // 数据类型
    U8  *pRecN  =&ptr[3];   // 数据条数
    U16 comCnt  = ptr[1] + ptr[2]*0x100;    // 通信计数器，原值返回，本机成功发送后加1保存
    U16 len, 
        tempLen;
    U16 leftLen;            // 打包中剩余长度 暂时定1300
    U16 rem_at;             // PC主从机属性

    if (cutlen < MAX_SENDPACK_LEN/2) {
        leftLen = MAX_SENDPACK_LEN-cutlen;
    } else {
        return  (PACK_LEN_ERR); // 封装长度错误
    }
    //--SRAM异常直接返回--//
    if (cDev.GetGblFault() & GBL_SRAM_FAULT) {  // SRAM硬件错误
        ptr[0]  = 0x01;     // 只返回实时数据
        ptr[1]  = ptr[1];
        ptr[2]  = ptr[2];
        ptr[3]  = 0;        // 返回数据条数初始化0
        len     = 4;        // 指向第一条数据(通道号)
        ptr    += 4;        
        for (i=0; i<(cDev.mInfo.chan_num); i++) {
            if ((cDev.mTxCtrl.realCh >= cDev.mInfo.chan_num)
            ||  (cDev.mTxCtrl.realCh >= MAX_CHAN_NUM)) {   // 本次命令 －> 读实时起始通道
                cDev.mTxCtrl.realCh = 0;
            }
            if ((len + FIXED_DATA_LEN) > leftLen) { // 长度不够则跳转
                break;
            }
            cChan[cDev.mTxCtrl.realCh].SetDummyRealDate(ptr,ERR_SRAM);  // SRAM错误，返回
            len += cDev.LoadPackLen;
            ptr += cDev.LoadPackLen;
            
            *pRecN  += 1;               // 实时数据条数加一
            cDev.mTxCtrl.realCh += 1;   // 实时数据通道加一
        }
    } 
    //--正常：无SRAM故障情况--//
    else {
        //--PC主从机判断--//
        rem_at  = cDev.Test_RemotePC(pip);
        switch (rem_at & 0xff) 
        {
            default:        // 非主非从
            {
                ptr[0]  = 0x01;     // 返回数据类型
                *pRecN  = 0;        // 不返回数据
                len     = 4;        // 指向第一条数据(通道号)
                break;
            }
            case MASTER_PC: //主机
            {
                //==================== 读记录处理 ====================
                /*  数据调度方法 --  
                        碰到取过程数据，把过程数据放缓冲，长度累加，继续取数据，发现长度不够后把缓冲中的过程数据补到末尾。
                        单个通道取完，长度到结束返回，不到取下一个，以此类推。
                */
                if ((type == RECDATA_MARK) 
                ||  (type == INITREC_MARK)) {
                    // 记录翻滚计数器处理
                    if (type == INITREC_MARK) {     // 上位机初始化重传
                        comCnt  = 0;                // 初始化翻滚计数器
                        cDev.mTxCtrl.repeatNum  = 0;                                // 清重传次数
                        for (i=0; i<(cDev.mInfo.chan_num); i++) {                   // 更新读暂存记录指针
                            cChan[i].RecoverReadPoint();                            // 恢复实际读指针到临时读指针
                            cDev.mTxCtrl.recNum[i]  = cChan[i].GetUnReadRecNum();   // 初始化重传 - 获取各通道记录个数
                        }
                    } 
                    // 普通重传
                    else if ((pRecBackUp->RecCnt != comCnt)
                    &&       (pRecBackUp->RecCnt != 0)) {
                        LED_Flash(LD4); // 重传指示
                        cDev.mTxCtrl.repeatNum++;
                        if (cDev.mTxCtrl.repeatNum >= MAX_REPEAT_NUM) { // 达到最大值，取消重传
                            cDev.mTxCtrl.repeatNum = 0;
                            for (i=0; i<(cDev.mInfo.chan_num); i++) {
                                cChan[i].CoverReadPoint();                              // 更新临时读指针到实际读指针
                                cDev.mTxCtrl.recNum[i] = cChan[i].GetUnReadRecNum();    // 重传超时 - 获取各通道记录个数
                            }
                        } else {
                            for (i=0; i<(cDev.mInfo.chan_num); i++) {                   // 上次记录未成功接收，更新读暂存记录指针
                                cChan[i].RecoverReadPoint();                            // 恢复实际读指针到临时读指针
                                cDev.mTxCtrl.recNum[i] = cChan[i].GetUnReadRecNum();    // 重传 - 获取各通道记录个数
                            }
                        }
                    } 
                    // 上次记录成功接收或中位机初始化翻滚计数器，更新读记录指针
                    else {
                        cDev.mTxCtrl.repeatNum = 0;     // 清重传次数
                        for (i=0; i<(cDev.mInfo.chan_num); i++) {
                            cChan[i].CoverReadPoint();                              // 更新临时读指针到实际读指针
                            cDev.mTxCtrl.recNum[i] = cChan[i].GetUnReadRecNum();    // 正常 - 获取各通道记录个数
                        }
                    }
                    if (comCnt == 65535) {
                        pRecBackUp->RecCnt = 1;             // 翻滚计数从1开始，0为初始化计数
                    } else {
                        pRecBackUp->RecCnt = comCnt + 1;    // 翻滚计数++
                    }
                    //--------------------------------------------------
                    // 记录数据部分处理
                    if((cDev.mTxCtrl.recCh >= MAX_CHAN_NUM)
                    || (cDev.mTxCtrl.recCh >= cDev.mInfo.chan_num)) {   // 读记录起始通道合法性判断
                        cDev.mTxCtrl.recCh = 0;
                    }                    
//                  ptr[0]  = type;                         // 0 - 数据类型:记录
                    ptr[1]  = pRecBackUp->RecCnt & 0xff;    // 1 - 记录翻滚低
                    ptr[2]  = pRecBackUp->RecCnt >> 8;      // 2 - 记录翻滚高
                    ptr[3]  = 0;                            // 3 - 记录条数初始化
                    len     = 4;                            // 指向 4 - 第一条数据(通道号)
                    ptr    += 4;
                    while (leftLen >= cDev.LoadPackLen) {   // 判断明细长度
                        
                        if (cDev.mTxCtrl.recNum[cDev.mTxCtrl.recCh] > 0) {              //当前有记录未送
                            if (cChan[cDev.mTxCtrl.recCh].GetReadRecType() != PROC_REC) {   //非过程记录
                                cChan[cDev.mTxCtrl.recCh].SetOneRec(ptr);               //这里没有记录情况不可能发生
                                leftLen -= cDev.LoadPackLen;
                                len += cDev.LoadPackLen;
                                ptr += cDev.LoadPackLen;
                                
                                *pRecN  = *pRecN + 1;       // 明细数据条数++
                                cDev.mTxCtrl.recNum[cDev.mTxCtrl.recCh]--;
                            } else {                        // 过程记录处理
                                leftLen -= PROCDATA_LEN;    // 固定长度
                                cChan[cDev.mTxCtrl.recCh].SetProcRec(&ProcDataBuf[procNum*PROCDATA_LEN]);
                                procNum++;                  // 过程记录条数增加
                                cDev.mTxCtrl.recNum[cDev.mTxCtrl.recCh]--;
                            }
                        } else {
                            emptyCh++;  //
                            if (emptyCh >= cDev.mInfo.chan_num) {   // 通道记录都取空退出
                                break;          
                            } else {
                                cDev.mTxCtrl.recCh++;               // 否则取下一个通道记录,并更新记录数据长度
                                if (cDev.mTxCtrl.recCh >= cDev.mInfo.chan_num) {
                                    cDev.mTxCtrl.recCh = 0;
                                }
                            }
                        }
                    }
                    // 有过程记录，加到数据尾部
                    if (procNum > 0) {
                        *ptr++  = procNum;  // 过程数据个数 U16, 不考虑超过255条
                        *ptr++  = 0;
                        for (U16 x=0; x<(procNum * PROCDATA_LEN); x++) {    // 拷贝数据
                            *ptr++  = ProcDataBuf[x];
                            len++;
                        }
                        // 长度在这里不增加，在前面保存缓冲时已经处理
                        ;
                    } else {    // 无过程数据
                        *ptr++  = 0;    // 过程数据个数低
                        *ptr++  = 0;    // 过程数据个数高
                        len    += 2;
                    }
                    if ((emptyCh == 0) 
                    &&  (*pRecN) != 0) {    //本次命令只取了一个通道，通道号需要增加，防止阻塞
                        cDev.mTxCtrl.recCh++;
                    }
                }
                //==================== 读实时处理 ====================
                /*
                    实时数据调度方法 -- 一次传完足够多实时数据
                */
                else {  // 默认type,传实时数据
//--------------------------------------------------
//                  ptr[0]  = type;     // 0 - 数据类型:实时
//                  ptr[1]  = ptr[1];   // 1 - 记录翻滚低，实时无此项
//                  ptr[2]  = ptr[2];   // 2 - 记录翻滚高，实时无此项
                    ptr[3]  = 0;        // 3 - 实时条数初始化
                    len     = 4;        // 指向 4 - 第一条数据(通道号)
                    ptr    += 4;
                    for (i=0; i<(cDev.mInfo.chan_num); i++) {
                        if((cDev.mTxCtrl.realCh >= MAX_CHAN_NUM)
                        || (cDev.mTxCtrl.realCh >= cDev.mInfo.chan_num)) {  // 读实时起始通道合法性判断
                            cDev.mTxCtrl.realCh = 0;
                        }
                        if ((len + cDev.LoadPackLen) > leftLen) {       // 长度不够则跳转
                            break;
                        }
                        cChan[cDev.mTxCtrl.realCh].SetRealDate(ptr);    // 拷贝通道数据
                        len += cDev.LoadPackLen;
                        ptr += cDev.LoadPackLen; 
                        
                        *pRecN  += 1;               // 实时数据条数加一
                        cDev.mTxCtrl.realCh += 1;   // 实时数据通道加一
                    }
                    // 实时数据中：空过程数据长度需要加入 15-0516
                    *ptr++  = 0;    // 过程数据个数低
                    *ptr++  = 0;    // 过程数据个数高
                    len    += 2;
                }
                break;
            }
            case SLAVE_PC:  // 从机    // 15-0119
            {
                U8  inx = rem_at >> 8;  // 从机在缓冲中注册号
            
                ptr[0]  = 0x01; // 返回数据类型-实时
                *pRecN  = 0;    // 返回记录或实时条数
                len     = 4;    // 指向第一条数据(通道号)
                ptr    += 4;
                if (inx >= MAX_SLAVE_PC) {
                    inx = 0;
                }
                for (i=0; i<(cDev.mInfo.chan_num); i++) {
                    if (cDev.mPCSlaCtrl[inx].ch >= MAX_CHAN_NUM) {  // 读实时起始通道
                        cDev.mPCSlaCtrl[inx].ch = 0;
                    }
                    if ((len + cDev.LoadPackLen) > leftLen) {       // 长度不够则跳转
                        leftLen -= len; // 剩余可用长度 15-0119
                        break;
                    }
                    cChan[cDev.mPCSlaCtrl[inx].ch].SetRealDate(ptr);    // 拷贝通道实时数据
                    len += cDev.LoadPackLen;
                    ptr += cDev.LoadPackLen; 
                    
                    *pRecN  += 1;                   // 实时数据条数加一
                    cDev.mPCSlaCtrl[inx].ch += 1;   // 实时数据通道加一
                }
                //实时数据中：空过程数据长度需要加入 15-0516
                *ptr++  = 0;    // 过程数据个数低
                *ptr++  = 0;    // 过程数据个数高
                len    += 2;
                
                leftLen -= len; // 剩余可用长度
                break;
            }
        }
    }
    // ----- 通用返回部分 -----
    // 返回恒流恒压信号
//  test    
//    cDev.SetUIMark(ptr);
//    ptr    += 4;    
    *ptr++  = 0x01;
    *ptr++  = 0x02;
    *ptr++  = 0x03;
    *ptr++  = 0x04;

    len    += 4;
    //返回IO数据
    tempLen = cSubChan.SetIOData(ptr, cDev.mInfo.dev_str_type); // 对应当前的硬件版本，中位机配置时需要对应
    len    += tempLen;
    ptr    += tempLen;
    //返回保留字
    *ptr++ = 0;
    *ptr++ = 0;
    len   += 2;

    *ret_len = len;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x82 - 通道控制发送
//  16-0125调整，允许操作未开放的通道（当设定的通道数 < MAX_CHAN_NUM）
//  16-0128调整，通道处理返回，需要进一部处理回路。
//----------------------------------------------------------------------------------------------------
U8  CMD_CIRCUIT_CTRL_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0;
    U8  i,j,k;
    U16 chan_tmp;

    U16 rem_at  = cDev.Test_RemotePC(pip);

    if ((rem_at&0xff) != MASTER_PC) {   // 非主机（这里未屏蔽，上位机限制 16-0802）
        ;
    }

    for (i=0; i<(MAX_CHAN_NUM/8+1); i++) {  // 选中的通道
        if (i == (MAX_CHAN_NUM/8)) {        // 最后几个通道处理
            k = MAX_CHAN_NUM & 0x07;
        } else {
            k = 8;
        }
        for (j=0; j<k; j++) {
            chan_tmp = i*8 + j;
            if (chan_tmp >= MAX_CHAN_NUM) {   // 通道边界限制 16-0122
                break;
            }
            if (ptr[i] & B8_MASK_N[j]) {    // 通道选中
                ret = cChan[chan_tmp].ManualCtrl(&ptr[32]);
                // 根据返回情况处理
                switch (ret) 
                {
                    case 0x00:      // 正确，继续下一通道
                    {
                        break;
                    }
                    case 0x01:      // 错误，直接返回，不再进一步处理
                    {
                        return (ret);
                    }
                    case 0xf1:      // 清下位机故障，然后继续
                    {
                        U32 myid;
#ifdef  EN_PARALLEL     // 使能并联，上位机停止（通道有异常时），中位机转发各个回路清故障 16-0122
                        U32 i;
                        U16 node_first, node_last;

                        node_first  = cDev.mInfo.par_node_num * chan_tmp;   // 并联对应独立首回路地址
                        node_last   = cDev.mInfo.par_node_num + node_first; // 并联对应独立末回路地址
                        for (i=node_first; i<node_last; i++) { // 并联组回路扫描
                            if (i < MAX_NODE_NUM) {

                               // 传统CANID，地址+命令
                                myid = (MCAN_CLRERRBIT << 0) | ((i + 1) << 8);
                                // 发送到下位机并等待应答
                                CanNet_CmdSend(myid, MBX_Net2MCan, 2);
                            }
                        }
#else       // 非并联
                        // 传统CANID，地址+命令
                        myid = (MCAN_CLRERRBIT << 0) | ((chan_tmp + 1) << 8);

                        // 发送到下位机并等待应答
                        CanNet_CmdSend(myid, MBX_Net2MCan, 2);
#endif
                        ret = 0;
                        break;
                    }
                }
            }
        }
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
/*  检查启动命令数据头
    参数  *pstr   数据缓冲指针
    返回  ret     0:数据不合法 1：数据合法
*/
//----------------------------------------------------------------------------------------------------
U8  Chk_StartHead(strStartCmd *pstr)
{
    U8  ret = 1;

    if ((pstr->global.Start_mode != CONTINUE_START)
    &&  (pstr->global.Start_mode > pstr->pageline.TotLine)) {
        return  (0);        // 非续接启动的启动行号错误
    }
    if ((pstr->global.Tst_ViNum > MAX_TEST_VINUM) 
    ||  (pstr->global.Tst_TiNum > MAX_TEST_TINUM)
    ||  (pstr->global.Tst_XiNum > MAX_TEST_XINUM)) {
        return  (0);        // 测试辅助单体错误
    }
    if ((pstr->pageline.TotPage     == 0)
    ||  (pstr->pageline.PageNo      > pstr->pageline.TotPage)
    ||  (pstr->pageline.TotLine     == 0)
    ||  (pstr->pageline.PageLine    == 0)
    ||  (pstr->pageline.TotLine     < pstr->pageline.PageLine)
    ||  (pstr->pageline.TotLine     > MAX_WKLINE)) {
        return  (0);        // 页，行数目错误
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
/*  检查阶段工艺数据
    参数  prg     数据缓冲
          inx     阶段索引
    返回  ret 0:数据不合法 1：数据合法
*/
//----------------------------------------------------------------------------------------------------
U8  Chk_StpProg(strStpPrg prg, U8 inx)
{
    U8  ret = 1;

    if (prg.No > (inx+1)) {
        return  (0);        //工步号不符合
    }
    if (prg.Mode == MODE_AND) { // AND结构不判断
        return  (1);    
    }
    if ((prg.Mode >= MIN_ERR_MODE)
    &&  (prg.Mode <= MAX_ERR_MODE)) {
        return  (0);        //非法模式
    }
    if ((prg.Pv_type1 >= MIN_ERR_VAR_PAR) 
    ||  (prg.Pv_type2 >= MIN_ERR_VAR_PAR)) {
        return  (0);        //非法参数或变量
    }
    if ((prg.Pv_opt1 >= MIN_ERR_OP)
    ||  (prg.Pv_opt2 >= MIN_ERR_OP)) {
        return  (0);        //非法操作符
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x83 - 通道启动发送
//----------------------------------------------------------------------------------------------------
U8  CMD_CIRCUIT_START_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0;
    U32 i;
    strStartCmd *pBuf   = (strStartCmd *)ptr;
    U16         rem_at  = cDev.Test_RemotePC(pip);

    if ((rem_at&0xff) != MASTER_PC) {   //非主机（这里未屏蔽，上位机限制 16-0802）
        ;
    }
    if (Chk_StartHead(pBuf) == 0) {
        return  (1);    // 数据头错误，直接返回
    }
    //接收状态散转
    switch (PrgRcvCtrl.RcvStat) 
    {
    case RECV_IDLE:     //空闲，接收 -> 通道表，全局保护，保存总页数，保存续接参数(若有)，置相应标记
        if (pBuf->pageline.PageNo == 0) {   // 必须是第0页数据
            memcpy(&PrgRcvCtrl.DataBuf.ch[0], ptr, sizeof(strStartHead));
            PrgRcvCtrl.CurPage  = 0;    // 接收页初始化
            PrgRcvCtrl.CurLine  = 0;    // 阶段行初始化
            // 保存阶段工艺数据
            
            for (i=0; i<pBuf->pageline.PageLine; i++) {
                memcpy ((U8 *)&PrgRcvCtrl.DataBuf.step[i].No, 
                        &ptr[sizeof(strStartHead)+sizeof(strStpPrg)*i], 
                        sizeof(strStpPrg)); // 保存阶段工艺数据
                if (Chk_StpProg(PrgRcvCtrl.DataBuf.step[i], i) == 0) {
                    return  (1);    // 第一包：阶段工艺错返回
                }
                PrgRcvCtrl.CurLine++;   // 更新接收行号，备用下一页命令（若有）
            }
            PrgRcvCtrl.CurPage++;       // 接收了第一页
            // 判断是否结束
            if (PrgRcvCtrl.CurPage >= PrgRcvCtrl.DataBuf.pageline.TotPage) {
                PrgRcvCtrl.RcvStat  = RECV_END;
            } else {
                PrgRcvCtrl.RcvStat  = RECV_GOING;
                PrgRcvCtrl.ToutCnt  = PRG_RCV_TICK; //初始化超时计数器
            }
        } else {
            return  (1);    // 第1页：非第1页 错误返回
        }
        break;
        
    case RECV_GOING:    //接收中，若到最后一页置相应标记
        if (pBuf->pageline.PageNo == PrgRcvCtrl.CurPage) {
            memcpy(&PrgRcvCtrl.DataBuf.ch[0], ptr, sizeof(strStartHead));
        } else {    //当前页非要求接收页
            PrgRcvCtrl.RcvStat  = RECV_IDLE;
            PrgRcvCtrl.ToutCnt  = 0;    // 清超时计数器
            return (1);     // 第n页：非第n页 错误返回
        }
        // 保存阶段工艺数据
        for (i=0; i<pBuf->pageline.PageLine; i++) {
            memcpy ((U8 *)&PrgRcvCtrl.DataBuf.step[i+PrgRcvCtrl.CurLine].No, 
                    &ptr[sizeof(strStartHead)+sizeof(strStpPrg)*i], 
                    sizeof(strStpPrg));         // 保存阶段工艺数据
            if (Chk_StpProg(PrgRcvCtrl.DataBuf.step[i+PrgRcvCtrl.CurLine], i+PrgRcvCtrl.CurLine) == 0) {
                PrgRcvCtrl.RcvStat  = RECV_IDLE;
                PrgRcvCtrl.ToutCnt  = 0;    // 清超时计数器
                return  (1);    // 第一包：阶段工艺错返回
            }
        }
        PrgRcvCtrl.CurLine += pBuf->pageline.PageLine;  // 更新接收行号，备用下一页命令（若有）
        PrgRcvCtrl.CurPage++;   //当前接收页更新
        //判断是否结束
        if (PrgRcvCtrl.CurPage >= PrgRcvCtrl.DataBuf.pageline.TotPage) {
            PrgRcvCtrl.RcvStat  = RECV_END;
        } else {
            PrgRcvCtrl.ToutCnt  = PRG_RCV_TICK; //初始化超时计数器
        }
        break;
        
    case RECV_END:      //接收结束，等待启动，不再接收任何工艺
        ret = 2;
        break;
        
    default:            //异常，回到空闲等待状态
        PrgRcvCtrl.RcvStat  = RECV_IDLE;
        PrgRcvCtrl.ToutCnt  = 0;
        PrgRcvCtrl.CurPage  = 0;
        PrgRcvCtrl.CurLine  = 0;
        ret = 1;
        break;
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x84 - CMD_MODIFY_DEVINFO 修改设备信息 
//  16-0129 修改设备信息成功后，更新并联节点映射表
//----------------------------------------------------------------------------------------------------
U8  CMD_MODIFY_DEVINFO_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x89 - MAC及IP地址配置
//----------------------------------------------------------------------------------------------------
U8  CMD_CFG_MAC_IP_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x8a - 读取指定设备号的MAC及IP
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_DEV_MAC_IP_Proc(U8 *ptr, U16 *ret_len)
{
    U8  ret = 0;

    *ret_len = cDev.GetMACIP(ptr);

    *ret_len += 2;  //保留字段2字节

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x8b - 读取PC主机IP地址以及设备MAC
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_HIP_SMAC_Proc(U8 *ptr, U16 *ret_len, const U8 *pip)
{
    U8  ret = 0;
    U16 len;
    U16 rem_at = 0;

    rem_at  = cDev.Test_RemotePC(pip);

    len = cDev.RD_HIP_SMAC_Proc(ptr, (U8)rem_at);
    *ret_len = len;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x8c - 申请主控
//----------------------------------------------------------------------------------------------------
U8  CMD_RQ_HOSTCTRL_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0, i;
    U16 rem_at = 0, port_buf;

    port_buf = ptr[0] + ptr[1]*0x100;
    rem_at = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) == SLAVE_PC) {    //只有从机可以申请主控
        for (i=0; i<4; i++) {
            cDev.mHostCtrl.ip[i] = pip[i];
        }
        cDev.mHostCtrl.port = port_buf;
        cDev.SaveHostCtrl();
        //更新PC从机缓冲
        cDev.EmptyPCSlaCtrl(rem_at>>8);
        ret = 0x00;
    } else {    // 主机不用申请主控
        ret = 0x01;
    }

    return  (ret);
}
//====================================================================================================
//  主通道命令
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//0x90 - 读取故障判断屏蔽位
//0x92 - 读取校准数据
//0x94 - 读取报警屏蔽及调节器
//0x96 - 读取额定电流
//0x98 - 读取额定电压
//0x9A - 读取电流PID
//0x9C - 读取电压PID
//0x9E - 读取延时时间T
//0xA0 - 读取报警阈值S
//0xA2 - 读取软件版本
//0xBA - 读取扩展延时时间
//0xBC - 读取扩展报警阈值
//0xC0 - 读取母线参数 15-1223
//0xC2 - 读取频率等参数 15-1223
//0xC4 - 读取占空比参数 15-1223
//----------------------------------------------------------------------------------------------------
//  不管并联与否，透明发送到下位机，地址限制按并联宏处理 16-0129
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_ChanInfo_Proc(U8 cmd, U8 *ptr, U16 *ret_len)
{
    U8  ret     = 0x00;
    U8  chan    = ptr[0]+1; // 主通道号 15-1203
    U32 myid;
    static  CAN_msg s_tmsg,s_rmsg;
    static  CAN_msg *rmsg;  //,*tmsg;

#ifdef  EN_PARALLEL     // 允许并联，地址限制为最大节点
    if (chan > cDev.mInfo.chan_num * cDev.mInfo.par_node_num) {
        return  (RET_ADDR_ABN);
    }
#else                   // 非并联，地址限制为设置通道数
    if (chan > cDev.mInfo.chan_num) {
        return  (RET_ADDR_ABN);
    }
#endif

    if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, 0) == OS_R_OK) { // 发送之前先读接收缓冲，清空 15-1223
        _free_box (CanNet_mpool, rmsg);
    }

    //PC命令映射到CAN命令
    switch (cmd) 
    {
    case CMD_RD_SYSBIT:             //0x90-读系统功能位（报警屏蔽位及功能控制开关）
        myid = MCAN_RD_SYSBIT | chan<<8;
        break;
    case CMD_RD_CALIB:              //0x92-读取校准数据
        myid = MCAN_RD_CALIB | chan<<8;
        break;
    case CMD_RD_MASKREG:            //0x94-读取报警屏蔽及调节器
        myid = MCAN_RD_REGMD | chan<<8;
        break;
    case CMD_RD_IRATE:              //0x96-读取额定电流
        myid = MCAN_RD_IRATE | chan<<8;
        break;
    case CMD_RD_URATE:              //0x98-读取额定电压
        myid = MCAN_RD_URATE | chan<<8;
        break;
    case CMD_RD_IPID:               //0x9A-读取电流PID
        myid = MCAN_RD_IPID | chan<<8;
        break;
    case CMD_RD_UPID:               //0x9C-读取电压PID
        myid = MCAN_RD_UPID | chan<<8;
        break;
    case CMD_RD_TVALUE:             //0x9E-读取延时时间
        myid = MCAN_RD_TPARA | chan<<8;
        break;
    case CMD_RD_SVALUE:             //0xA0-读取报警阈值
        myid = MCAN_RD_SPARA | chan<<8;
        break;
    case CMD_RD_VERSION:            //0xA2-读取软件版本
        myid = MCAN_RD_VER | chan<<8;
        break;
    case CMD_RD_TVALUE_EX:          //0xBA-读取扩展延时时间
        myid = MCAN_RD_TPARA_EX | chan<<8;
        break;
    case CMD_RD_SVALUE_EX:          //0xBC-读取扩展报警阈值
        myid = MCAN_RD_SPARA_EX | chan<<8;
        break;
    case CMD_RD_BUSDATA:            //0xC0-读取母线参数 15-1223
        myid = MCAN_RD_VBUSDATA | chan<<8;
        break;
    case CMD_RD_FREQDATA:           //0xC2-读取频率等参数 15-1223
        myid = MCAN_RD_FREQDATA | chan<<8;
        break;
    case CMD_RD_DUTYDATA:           //0xC4-读取占空比参数 15-1223
        myid = MCAN_RD_DUTYDATA | chan<<8;
        break;
    default:    //无效命令
        myid = 0;
        ret  = RET_FRAME_ERR;
        goto RD_ChanInfo_Retloop;
    }
    
    // 发送到下位机并等待应答 16-0122：调整为通过函数处理消息发送
    if (cmd == CMD_RD_CALIB) {  // 读校准数据需要特殊处理
        for(int i=0; i<2; i++) {
            s_tmsg.data[i] = ptr[1+i];
        }
        ret = CanNet_MsgSend(s_tmsg, MBX_Net2MCan, 2);
    } else {
        ret = CanNet_CmdSend(myid, MBX_Net2MCan, 2);
    }
    if (ret) {
        goto RD_ChanInfo_Retloop;
    }
    //等待来自下位机的应答
    if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, CTRL_05S) == OS_R_TMO) { //读下位机超时
        ret = RET_RECV_TOUT;
    } else {
        s_rmsg = *rmsg;
        _free_box (CanNet_mpool, rmsg);
        
        //应答不相符,直接返回

        if (s_rmsg.id != (myid|0x80)) {

            ret = RET_DATA_ERR;
        }
    }

    if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, 0) == OS_R_OK) { //读下位机应答消息，再取一次，防止阻塞
        _free_box (CanNet_mpool, rmsg);
    }
    //打包回传PC
RD_ChanInfo_Retloop:

    ptr[1]  = ret;  // 故障码
    //有误返回
    if (ptr[1]) {
        ptr[2]  = 
        ptr[3]  = 
        ptr[4]  = 
        ptr[5]  = 
        ptr[6]  = 
        ptr[7]  = 
        ptr[8]  =
        ptr[9]  = 0;
    } else {
        ptr[2]  = s_rmsg.data[0];
        ptr[3]  = s_rmsg.data[1];
        ptr[4]  = s_rmsg.data[2];
        ptr[5]  = s_rmsg.data[3];
        ptr[6]  = s_rmsg.data[4];
        ptr[7]  = s_rmsg.data[5];
        ptr[8]  = s_rmsg.data[6];
        ptr[9]  = s_rmsg.data[7];
    }
    *ret_len = 10;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x91 - 写入故障判断屏蔽位
//0x93 - 写入校准数据
//0x95 - 报警屏蔽及调节器设置
//0x97 - 写入额定电流
//0x99 - 写入额定电压
//0x9B - 写入电流PID
//0x9D - 写入电压PID
//0x9F - 写入延时时间
//0xA1 - 写入报警阈值
//0xA3 - 写入CAN地址
//0xA5 - 保存或恢复校准参数
//0xC1 - 写入母线参数   15-1223
//0xC3 - 写入频率等参数 15-1223
//0xC4 - 写入占空比参数 15-1223
//----------------------------------------------------------------------------------------------------
//  加入广播处理 15-1203
//  下发命令调整，与并联无关，直接对回路操作 16-0129
//----------------------------------------------------------------------------------------------------
U8  CMD_WR_ChanInfo_Proc(U8 cmd, U8 *ptr, const U8 *pip)
{
    U8  ret     = 0x00;
    U8  rd_flag = 0;        // 刷新实时数据标记
    U8  chan    = ptr[0]+1; // 主通道号 15-1203
    U16 rem_at  = 0;
    U16 tout    = 0;        // 等待下位机应答时间
    U32 myid;               // 下位机应答比较用
    static  CAN_msg s_tmsg,s_rmsg;
    static  CAN_msg *rmsg;//,*tmsg;     // 16-0122

    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   // 非主机直接退出
        ptr[1]  = RET_NOTMASTERPC;
        return  (RET_NOTMASTERPC);
    }

#ifdef  EN_PARALLEL     // 允许并联，地址限制为最大节点
    if (chan > cDev.mInfo.chan_num * cDev.mInfo.par_node_num) {
        return  (RET_ADDR_ABN);
    }
    if (cmd == CMD_WR_CANADDR) {
        if ((ptr[1] > cDev.mInfo.chan_num * cDev.mInfo.par_node_num)
        ||  (ptr[1] == 0)) {    // 设定的地址溢出
            return  (RET_ADDR_ABN);
        }
    }
#else                   // 非并联，地址限制为设置通道数
    if (chan > cDev.mInfo.chan_num) {
        return  (RET_ADDR_ABN);
    }
    if (cmd == CMD_WR_CANADDR) {
        if ((ptr[1] > cDev.mInfo.chan_num)
        ||  (ptr[1] == 0)) {    // 设定的地址溢出
            return  (RET_ADDR_ABN);
        }
    }
#endif

#ifdef  EN_CAN_BROADCAST   // 使能CAN命令广播 16-0113    
    if (gFunSelBit == FUNC_BOARDCAST) { // 广播命令，地址调整为广播地址
        chan = BROADCAST_ADDR;
    }
#endif
    
    if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, 0) == OS_R_OK) { // 发送之前先读接收缓冲，清空 15-1223
        _free_box (CanNet_mpool, rmsg);
    }
    
    // PC命令映射到CAN命令
    
//--------------------------------------------------
  // 传统CANID
//--------------------------------------------------
    switch (cmd) 
    {
    case CMD_WR_SYSBIT:         //0x91-写入系统功能位
        s_tmsg.id = MCAN_WR_SYSBIT;
        break;
    case CMD_WR_CALIB:          //0x93-写入校准数据
        s_tmsg.id = MCAN_WR_CALIB;
        tout = CTRL_2S; //下位机可能执行时间较长,这里增加等待 15-0814
        rd_flag = 1;    // 刷新实时
        break;
    case CMD_WR_MASKREG:        //0x95-写入报警屏蔽及调节器模式
        s_tmsg.id = MCAN_WR_REGMD;
        break;
    case CMD_WR_IRATE:          //0x97-写入额定电流
        s_tmsg.id = MCAN_WR_IRATE;
        break;
    case CMD_WR_URATE:          //0x99-写入额定电压
        s_tmsg.id = MCAN_WR_URATE;
        break;
    case CMD_WR_IPID:           //0x9B-写入电流PID
        s_tmsg.id = MCAN_WR_IPID;
        break;
    case CMD_WR_UPID:           //0x9D-写入电压PID
        s_tmsg.id = MCAN_WR_UPID;
        break;
    case CMD_WR_TVALUE:         //0x9F-写入延时时间
        s_tmsg.id = MCAN_WR_TPARA;
        break;
    case CMD_WR_SVALUE:         //0xA1-写入报警阈值
        s_tmsg.id = MCAN_WR_SPARA;
        break;
    case CMD_WR_TVALUE_EX:      //0xBB-写入扩展延时时间
        s_tmsg.id = MCAN_WR_TPARA_EX;
        break;
    case CMD_WR_SVALUE_EX:      //0xBD-写入扩展报警阈值
        s_tmsg.id = MCAN_WR_SPARA_EX;
        break;
    case CMD_WR_CANADDR:        //0xA3-写入CAN地址
        s_tmsg.id = MCAN_WR_ADDR;
        break;
    case CMD_CALIBSET:          //0xA5-保存或恢复校准参数
        s_tmsg.id = MCAN_CALIBSET;
        tout = CTRL_2S; //下位机可能执行时间较长,这里增加等待
        break;
    case CMD_WR_BUSDATA:        //0xC0-写入母线参数 15-1223
        s_tmsg.id = MCAN_WR_VBUSDATA;
        break;
    case CMD_WR_FREQDATA:       //0xC2-写入频率等参数 15-1223
        s_tmsg.id = MCAN_WR_FREQDATA;
        break;
    case CMD_WR_DUTYDATA:       //0xC4-写入占空比参数 15-1223
        s_tmsg.id = MCAN_WR_DUTYDATA;
        break;
    default:
        break;
    }
    // 保存通道或主通道的canid以备下面比较   
    myid = s_tmsg.id | (chan << 8);

    myid |= 0x80;   // CAN应答命令标记位    

    // 发送到下位机并等待应答，先拷贝数据内容
    s_tmsg.len      = 8;
    s_tmsg.ch       = 0;
    s_tmsg.format   = EXTENDED_FORMAT;
    s_tmsg.type     = DATA_FRAME;
#ifdef  UCALIB_UNION    // 充放电压强制联合校准
    if (cmd == CMD_WR_CALIB) {
        if (ptr[1] == 0x02) {   //电压校准
            ptr[2] |= 0x30;
        }
    }
#endif
    for(int i=0; i<8; i++) {
        s_tmsg.data[i] = ptr[1+i];
    }    

    // PC命令映射到CAN命令
                      // 传统CANID，地址+命令
    s_tmsg.id |= (chan << 8);
      
    // 发送到下位机并等待应答
    ret = CanNet_MsgSend(s_tmsg, MBX_Net2MCan, 2);

    if (ret) {
        goto WR_ChanInfo_Retloop;
    } 
    else if (chan == BROADCAST_ADDR) {  // 广播发送消息后直接返回 15-1203
        goto WR_ChanInfo_Retloop;
    }
    // 等待来自下位机的应答
    if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, (CTRL_05S+tout)) == OS_R_TMO) {  //写下位机超时
        ret = RET_RECV_TOUT;
    } else {
        s_rmsg = *rmsg;
        _free_box (CanNet_mpool, rmsg);
        // 应答不相符,直接返回
        if (s_rmsg.id != myid) {    // 应答不相符（若并联，则为主机的应答）16-0122
            ret = RET_DATA_ERR;
        }
    }
    if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, 0) == OS_R_OK) { // 写下位机信息，再取一次，防止阻塞
        _free_box (CanNet_mpool, rmsg);
    }
    // 有读取实时数据要求 14-1018 没有判读主从CAN！！ tbc......
    if (rd_flag) {        
        // 下发读实时数据（并联时则为主机，从机暂未处理 16-0122）
// 传统CANID
        s_tmsg.id = MCAN_RD_REALDATA1 | chan<<8;
      
        // 发送到下位机，不等待应答
        CanNet_CmdSend(s_tmsg.id, MBX_Net2MCan, 0);
    }

WR_ChanInfo_Retloop:

    ptr[1]  = ret;  // 故障码

    return  (ret);
}
//====================================================================================================
//  中位机命令
//----------------------------------------------------------------------------------------------------
//0xA6-读取中位机控制配置参数
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_CtrlCfg_Proc(U8 *ptr, U16 *ret_len)
{
    U8  ret = 0x00;
    U8  mid = ptr[0];   // 中位机通道标记

    mid = mid;
    if (mid != MIDEV_FLAG) {
        return  (RET_ADDR_ABN);
    }
    ptr += 1;           // 指向缓冲实体内容首字节
    *ret_len    = 1;    // 中位机标记 1字节
    *ret_len   += cDev.CopyCtrlCfg(ptr);

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xA7-写入中位机控制配置参数
//----------------------------------------------------------------------------------------------------
U8  CMD_WR_CtrlCfg_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0x00;
    U8  mid = ptr[0];   // 中位机通道标记
    U16 rem_at;

    mid = mid;
    if (mid != MIDEV_FLAG) {
        return  (RET_ADDR_ABN);
    }
    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   //非主机直接退出
        return  (RET_NOTMASTERPC);
    }
    //拷贝数据
    if (cDev.CheckCtrlCfg((strCtrlCfg *)(ptr-3)) == 0) {    // 向前偏移3，指向缓冲首字节（标记字段），便于解析
        ret = cDev.SaveCtrlCfg(&ptr[1]);    // 拷贝
    } else {
        ret = 0x01;
    }
    //返回状态
    ptr[1] = ret;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xA8-读取辅助通道硬件配置参数
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_SubChHCfg_Proc(U8 *ptr, U16 *ret_len)
{
    U8  ret = 0x00;
    U8  mid = ptr[0];   // 中位机通道标记
    U8  sid = ptr[1];   // 辅助通道类型

    if (mid != MIDEV_FLAG) {    // 中位机标记错
        return  (RET_ADDR_ABN);
    }
    if ((sid == 0)  // 辅助通道类型错
    ||  (sid > XI_ID)) {
        return  (RET_SUBID_ERR);
    }
    *ret_len = 3;   // 版本1字节
    *ret_len += cDev.CopySubHdCfg(&ptr[2], sid);

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xA9-写入辅助通道硬件配置参数
//----------------------------------------------------------------------------------------------------
U8  CMD_WR_SubChHCfg_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0x00;
    U8  mid = ptr[0];   // 中位机通道标记
    U8  sid = ptr[1];   // 辅助通道类型
    U16 rem_at  = 0;

    if (mid != MIDEV_FLAG) {    // 中位机标记错
        return  (RET_ADDR_ABN);
    }
    if ((sid == 0)  // 辅助通道类型错
    ||  (sid > XI_ID)) {
        return  (RET_SUBID_ERR);
    }
    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   //非主机直接退出
        return  (RET_NOTMASTERPC);
    }
    //拷贝数据
    ret = cDev.SaveSubHdCfg(&ptr[2], sid);
    //状态返回
    ptr[2]  = ret;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xAA-读取辅助通道软件配置参数 15-0924 tbc
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_SubChSCfg_Proc(U8 *ptr, U16 *ret_len)
{
    U8  ret = 0x00;
    U8  mid = ptr[0];   // 中位机通道标记
    U8  sid = ptr[1];   // 辅助通道类型

    if (mid != MIDEV_FLAG) {    // 中位机标记错
        return  (RET_ADDR_ABN);
    }
    if ((sid == 0)  // 辅助通道类型错
    ||  (sid > XI_ID)) {
        return  (RET_SUBID_ERR);
    }
    *ret_len = 3;   // 版本1字节
    *ret_len += cDev.CopySubHdCfg(&ptr[2], sid);

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xAB-写入辅助通道软件配置参数 15-0924 tbc
//----------------------------------------------------------------------------------------------------
U8  CMD_WR_SubChSCfg_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0x00;
    U8  mid = ptr[0];   // 中位机通道标记
    U8  sid = ptr[1];   // 辅助通道类型
    U16 rem_at  = 0;

    if (mid != MIDEV_FLAG) {    // 中位机标记错
        return  (RET_ADDR_ABN);
    }
    if ((sid == 0)  // 辅助通道类型错
    ||  (sid > XI_ID)) {
        return  (RET_SUBID_ERR);
    }
    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   //非主机直接退出
        return  (RET_NOTMASTERPC);
    }
    //拷贝数据
    ret = cDev.SaveSubHdCfg(&ptr[2], sid);
    //状态返回
    ptr[2]  = ret;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xAE-复位中位机
//----------------------------------------------------------------------------------------------------
U8  CMD_RESET_Dev_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0x00;
    U16 rem_at  = 0;

    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   //非主机直接退出
        ptr[2]  = RET_NOTMASTERPC;
        return  (RET_NOTMASTERPC);
    }
    ptr++;

    return  (ret);
}
//====================================================================================================
//  辅助通道命令
//====================================================================================================
//----------------------------------------------------------------------------------------------------
//0xB0 - 读取辅助通道实时数据 
//16-0311 调整，使用硬件配置表，超时也返回数据
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_SubChReal_Proc(U8 *ptr, U16 *ret_len)
{
    U8  ret = RET_OK;

    if (ret) {
        ptr[2]  = ret;  // 故障码
        ptr[3]  = 0;    // 0个后续数据
        *ret_len = 4;   // 返回数据长度
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xB2 - 读取辅助通道额定数据
//0xB4 - 读取辅助通道校准数据
//0xB6 - 读取辅助通道扩展数据（CAN地址及AD控制参数）
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_SubChInfo_Proc(U8 cmd, U8 *ptr, U16 *ret_len)
{
    U8  ret = RET_OK;
    U8  cid = ptr[0];       // 辅助通道特征码
    U8  sch = ptr[1];       // 辅助通道号
    U32 myid;               // 下发id
    static  CAN_msg s_tmsg,s_rmsg;
    static  CAN_msg *rmsg,*tmsg;    // 15-1223

    ret = Check_SubAddr(cid, sch);
    if (ret) {  //地址，ID有误，直接返回
        goto RD_SubChanInfo_Retloop;
    } 
    // 辅助CAN地址偏移
    switch (cid) 
    {
    case VI_ID: sch += VIADDR_OFFSET;   break;//电压单体辅助通道
    case TI_ID: sch += TIADDR_OFFSET;   break;//温度单体辅助通道
    case II_ID: sch += IIADDR_OFFSET;   break;//开关量入辅助通道
    case OI_ID: sch += OIADDR_OFFSET;   break;//开关量出辅助通道
    case XI_ID: sch += XIADDR_OFFSET;   break;//其他扩展辅助通道
    case EBI_ID:sch += EBIADDR_OFFSET;  break;//扩展位输入辅助通道
    case EBO_ID:sch += EBOADDR_OFFSET;  break;//扩展位输出辅助通道
    case EDI_ID:sch += EDIADDR_OFFSET;  break;//扩展数据输入辅助通道
    case EDO_ID:sch += EDOADDR_OFFSET;  break;//扩展数据输出辅助通道
    default:                            break;
    }
    // PC命令映射到辅助CAN命令
    switch (cmd) 
    {
    case CMD_RD_SUBRATE:        //0xB2-读取辅助通道额定数据
        myid = SCAN_RD_RATE | (sch + 1)<<8;
        break;
    case CMD_RD_SUBCALIB:       //0xB4-读取辅助通道校准数据
        myid = SCAN_RD_CALIB | (sch + 1)<<8;
        break;
    case CMD_RD_SUBEDATA:       //0xB6-读取辅助通道扩展数据（CAN地址及AD控制参数）
        myid = SCAN_RD_EDATA | (sch + 1)<<8;
        break;
    default:    //未支持命令 - impossible here
        ret = RET_FRAME_ERR;
        goto RD_SubChanInfo_Retloop;
    }
    // 发送到下位机并等待应答
    s_tmsg.id       = myid;
    s_tmsg.len      = 8;
    s_tmsg.ch       = 0;
    s_tmsg.format   = EXTENDED_FORMAT;
    s_tmsg.type     = DATA_FRAME;
    s_tmsg.data[0]  = 
    s_tmsg.data[1]  = 
    s_tmsg.data[2]  = 
    s_tmsg.data[3]  = 
    s_tmsg.data[4]  = 
    s_tmsg.data[5]  =
    s_tmsg.data[6]  =
    s_tmsg.data[7]  = 0;
    // 取一次，清空缓冲
    if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, 0) == OS_R_OK) {
        _free_box (CanNet_mpool, rmsg);
    }
    // 调整命令格式送辅助CAN
    if (os_mbx_check (CanNet_mpool) > 0) {
        tmsg = (CAN_msg *)_alloc_box (CanNet_mpool);
        if (tmsg) {
            *tmsg = s_tmsg;
            if (os_mbx_send (MBX_Net2SCan, tmsg, 2) == OS_R_TMO) {  // 读辅助通道
                _free_box (CanNet_mpool, tmsg);
                ret = RET_SEND_TOUT;
            }
        } else {
            ret = RET_SYS_ERR;
        }
    } else {
        ret = RET_SYS_ERR;
    }
    if (ret) {
        goto RD_SubChanInfo_Retloop;
    }
    // 等待来自下位机的应答
    if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, CTRL_1S) == OS_R_TMO) {
        ret = RET_RECV_TOUT;
    } else {
        s_rmsg = *rmsg;
        _free_box (CanNet_mpool, rmsg);
        if (s_rmsg.id != (myid|0x80)) { //应答不相符退
            ret = RET_DATA_ERR;
        }
    }
    // 再取一次，防止阻塞
    if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, 0) == OS_R_OK) {
        _free_box (CanNet_mpool, rmsg);
    }
    //打包回传PC
RD_SubChanInfo_Retloop:
    ptr[2] = ret;  // 故障码
    //有误返回
    if (ptr[2]) {
        ptr[3]  = 
        ptr[4]  = 
        ptr[5]  = 
        ptr[6]  = 
        ptr[7]  = 
        ptr[8]  = 
        ptr[9]  = 
        ptr[10] = 0;
    }
    //无误返回
    else {
        ptr[3]  = s_rmsg.data[0];
        ptr[4]  = s_rmsg.data[1];
        ptr[5]  = s_rmsg.data[2];
        ptr[6]  = s_rmsg.data[3];
        ptr[7]  = s_rmsg.data[4];
        ptr[8]  = s_rmsg.data[5];
        ptr[9]  = s_rmsg.data[6];
        ptr[10] = s_rmsg.data[7];
    }
    *ret_len = 11;  //固定长度返回

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xB1 - 写入辅助通道实时数据
//0xB3 - 写入辅助通道额定数据
//0xB5 - 写入辅助通道校准数据
//0xB7 - 写入辅助通道扩展数据（CAN地址及AD控制参数）
//0xB9 - 辅助通道单独控制
//----------------------------------------------------------------------------------------------------
// 16-0319      单体校准CAN第一字节标记
#define     CALVIFLAG       4       // 校准电压
#define     CALTIFLAG       5       // 校准温度
//----------------------------------------------------------------------------------------------------
U8  CMD_WR_SubChInfo_Proc(U8 cmd, U8 *ptr, const U8 *pip)
{
    U8  ret     = RET_OK;
    U8  cid     = ptr[0];   // 辅助通道特征码
    U8  sch     = ptr[1];   // 辅助通道号
    U8  scmd    = ptr[10];  // 子命令
    U16 rem_at  = 0;
    U32 myid;
    static  CAN_msg s_tmsg,s_rmsg;
    static  CAN_msg *rmsg,*tmsg;    // 15-1223
    
    rem_at = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   //非主机直接退出
        ptr[2] = RET_NOTMASTERPC;
        return  (RET_NOTMASTERPC);
    }
    ret = Check_SubAddr(cid, sch);
    if (ret) {  //地址，ID有误，直接返回
        goto WR_SubChanInfo_Retloop;
    } 
    // 辅助CAN地址偏移
    switch (cid) 
    {
    case VI_ID: sch += VIADDR_OFFSET;   break;//电压单体辅助通道
    case TI_ID: sch += TIADDR_OFFSET;   break;//温度单体辅助通道
    case II_ID: sch += IIADDR_OFFSET;   break;//开关量入辅助通道
#ifdef  USE_EXTIO_RDWR      // 使能扩展IO读写(双登定制100A/5V短路测试) 16-0418
    case OI_ID:             // XiChan[1]扩展开关量出，映射针床1，2层下压到扩展[1]:1，2号开关量输出
        if ((ptr[2] == 0x01)
        &&  (sch < 8)) {
            ptr[2] = 1<<sch;        // 针床IO地址转化为扩展开关量索引
            sch = XIADDR_OFFSET+1;  // 固定为 XiChan[1]
        }
        break;
#else        
    case OI_ID: sch += OIADDR_OFFSET;   break;//开关量出辅助通道
#endif
    case XI_ID: sch += XIADDR_OFFSET;   break;//其他扩展辅助通道
    case EBI_ID:sch += EBIADDR_OFFSET;  break;//扩展位输入辅助通道
    case EBO_ID:sch += EBOADDR_OFFSET;  break;//扩展位输出辅助通道
    case EDI_ID:sch += EDIADDR_OFFSET;  break;//扩展数据输入辅助通道
    case EDO_ID:sch += EDOADDR_OFFSET;  break;//扩展数据输出辅助通道
    default:                            break;
    }
    // PC命令映射到辅助CAN命令
    switch (cmd) 
    {
    case CMD_WR_SUBREAL:        //0xB1-写入辅助通道实时数据
        if (((cid == OI_ID) || (cid == EBO_ID))
        &&  (scmd == SCAN_RD_REALDATA2)) {
            myid = SCAN_WR_REALDATA2 | (sch + 1)<<8;    //翻转值输出
        } else {
            myid = SCAN_WR_REALDATA1 | (sch + 1)<<8;    //绝对值输出
        }
        break;
    case CMD_WR_SUBRATE:        //0xB3-写入辅助通道额定数据
        myid = SCAN_WR_RATE | (sch + 1)<<8;
        break;
    case CMD_WR_SUBCALIB:       //0xB5-写入辅助通道校准数据
        myid = SCAN_WR_CALIB | (sch + 1)<<8;
        break;
    case CMD_WR_SUBEDATA:       //0xB7-写入辅助通道扩展数据（CAN地址及AD控制参数）
        myid = SCAN_WR_EDATA | (sch + 1)<<8;
        break;
    case CMD_WR_SUBSINGLE:      //0xB9-辅助通道单独控制(锁定)
        myid = SCAN_WR_LOCK | (sch + 1)<<8;
        break;
    default:
        break;
    }
    //发送到下位机并等待应答
    s_tmsg.id       = myid;
    s_tmsg.len      = 8;
    s_tmsg.ch       = 0;
    s_tmsg.format   = EXTENDED_FORMAT;
    s_tmsg.type     = DATA_FRAME;
    if ((cmd == CMD_WR_SUBRATE)     // 写入辅助通道额定数据 16-0926
    ||  (cmd == CMD_WR_SUBCALIB)) { // 单体校准，第一字节填充类型标记 16-0319
        s_tmsg.data[0]  = cid + 3;  // 单体电压1->4 以此类推
    } else {
        s_tmsg.data[0]  = ptr[2];
    }
    s_tmsg.data[1]  = ptr[3];
    s_tmsg.data[2]  = ptr[4];
    s_tmsg.data[3]  = ptr[5];
    s_tmsg.data[4]  = ptr[6];
    s_tmsg.data[5]  = ptr[7];
    s_tmsg.data[6]  = ptr[8];
    s_tmsg.data[7]  = ptr[9];
    // 取一次，清空缓冲 15-1223
    if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, 0) == OS_R_OK) {
        _free_box (CanNet_mpool, rmsg);
    }
    // 调整命令格式送辅助CAN
    if (os_mbx_check (CanNet_mpool) > 0) {
        tmsg = (CAN_msg *)_alloc_box (CanNet_mpool);
        if (tmsg) {
            *tmsg = s_tmsg;
            if (os_mbx_send (MBX_Net2SCan, tmsg, 2) == OS_R_TMO) {  // 写辅助通道
                _free_box (CanNet_mpool, tmsg);
                ret = RET_SEND_TOUT;
            }
        } else {
            ret = RET_SYS_ERR;
        }
    } else {
        ret = RET_SYS_ERR;
    }
    if (ret) {
        goto WR_SubChanInfo_Retloop;
    }
    //如果是读取辅助通道实时,不等下位机应答 15-1126
    if (cmd == CMD_WR_SUBREAL) {
        goto    WR_SubChanInfo_Retloop;
    }
    // 等待来自下位机的应答
    if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, CTRL_05S) == OS_R_TMO) { //超时
        ret = RET_RECV_TOUT;
    } else {
        s_rmsg = *rmsg;
        _free_box (CanNet_mpool, rmsg);
        if (s_rmsg.id != (myid|0x80)) { //应答不相符退
            ret = RET_DATA_ERR;
        }
    }
    // 再取一次，防止阻塞
    if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, 0) == OS_R_OK) {
        _free_box (CanNet_mpool, rmsg);
    }

WR_SubChanInfo_Retloop:
    ptr[2] = ret;  // 故障码

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xBE - 通信中转
//----------------------------------------------------------------------------------------------------
U8  CMD_CommTransfer_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret     = RET_OK;
    U16 rem_at  = 0;

    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   //非主机直接退出
        ptr[2]  = RET_NOTMASTERPC;
        return  (RET_NOTMASTERPC);
    }


    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xF1 - 透明传输 16-0801
//----------------------------------------------------------------------------------------------------
U8  CMD_Transparent_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret     = RET_OK;
    U16 rem_at  = 0;

    static  CAN_msg s_tmsg,s_rmsg;
    static  CAN_msg *rmsg;

    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   // 非主机直接退出
        return  (RET_NOTMASTERPC);
    }
    
    U8  media   = ptr[0];       // 传输介质
    U8  port    = ptr[1];       // 介质端口索引
    U16 len     = ptr[2] 
                + ptr[3]*0x100; // 数据长度
    U32 myid;                   // 下位机应答比较用
    
    switch (media)
    {
        case CAN_INTERFACE:     // 通过CAN口（固化为：扩展ID + 8字节数据帧）
        {
            if ((port >= CAN_CTRL_MAX_NUM)
            ||  (len != 12)) {  // 超出硬件索引或长度不对
                return  (RET_FRAME_ERR);
            }
            myid            =
            s_tmsg.id       = ((U32)ptr[4] << 0)
                            | ((U32)ptr[5] << 8)
                            | ((U32)ptr[6] <<16)
                            | ((U32)ptr[7] <<24);
            s_tmsg.len      = 8;
            s_tmsg.ch       = 0;
            s_tmsg.format   = EXTENDED_FORMAT;
            s_tmsg.type     = DATA_FRAME;
            s_tmsg.data[0]  = ptr[8];
            s_tmsg.data[1]  = ptr[9];
            s_tmsg.data[2]  = ptr[10];
            s_tmsg.data[3]  = ptr[11];
            s_tmsg.data[4]  = ptr[12];
            s_tmsg.data[5]  = ptr[13];
            s_tmsg.data[6]  = ptr[14];
            s_tmsg.data[7]  = ptr[15];
            
#ifdef  USE_SIM_CANCMD_ACK  // 模拟应答CAN命令   
            switch(ptr[4])
            {
                case (CMD_ONLINE_CHK):        // 在线检测，返回版本信息
                {
                    ptr[4] = ptr[4];
                    break;
                }
                case (CMD_SYSTEM_RST):        // 系统复位
                {
                    ptr[4] = ptr[4];
                    break;
                }
                case (CMD_ERASE_FLASH):       // 擦除 Flash，每次擦除一个扇区（1K）
                                    {
                    ptr[4] = ptr[4];
                    break;
                }
                case (CMD_SET_BAUDRATE):      // 设置波特率
                                    {
                    ptr[4] = ptr[4];
                    break;
                }
                case (CMD_UPDATE_PROGPAR):    // 更新程序参数：起始地址，长度（固定1024+2字节CRC）
                                    {
                    ptr[4] = ptr[4];
                    break;
                }
                case (CMD_PROGRAM_FLASH):     // Flash 编程（最后一帧才真正开始）
                                    {
                    ptr[4] = ptr[4];
                    break;
                }
                case (CMD_EXECUTE_APP):       // 跳转到APP执行
                {
                    ptr[4] = ptr[4];
                    break;
                }                
                default:
                {
                    ptr[4] = ptr[4];
                    break;
                }
            } 
#endif
            
            if (port == 0) {
                if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, 0) == OS_R_OK) {
                    _free_box (CanNet_mpool, rmsg);
                }
                // 发送到下位机并等待应答
                ret = CanNet_MsgSend(s_tmsg, MBX_Net2MCan, 2);
                if (ret) {  // 消息失败
                    goto TRANS_RET;
                } 
                // 等待来自下位机的应答
                if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, (CTRL_2S)) == OS_R_TMO) {  //写下位机超时
                    ret = RET_RECV_TOUT;
                } else {
                    s_rmsg = *rmsg;
                    _free_box (CanNet_mpool, rmsg);
                    // 应答不相符,直接返回
                    if (s_rmsg.id != myid) {    // 应答不相符（若并联，则为主机的应答）16-0122
                        ret = RET_DATA_ERR;
                    }
                    
                    
                    
                    
                    
                    
                    
                }
                if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, 0) == OS_R_OK) { // 写下位机信息，再取一次，防止阻塞
                    _free_box (CanNet_mpool, rmsg);
                }  
            } else {    //辅助CAN通道
                if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, 0) == OS_R_OK) {
                    _free_box (CanNet_mpool, rmsg);
                }                
                // 发送到下位机并等待应答
                ret = CanNet_MsgSend(s_tmsg, MBX_Net2SCan, 2);

                if (ret) {  // 消息失败
                    goto TRANS_RET;
                }
                // 等待来自下位机的应答
                if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, (CTRL_2S)) == OS_R_TMO) {  //写下位机超时
                    ret = RET_RECV_TOUT;
                } else {
                    s_rmsg = *rmsg;
                    _free_box (CanNet_mpool, rmsg);
                    // 应答不相符,直接返回
                    if (s_rmsg.id != myid) {    // 应答不相符（若并联，则为主机的应答）16-0122
                        ret = RET_DATA_ERR;
                    }
                }
                if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, 0) == OS_R_OK) { // 写下位机信息，再取一次，防止阻塞
                    _free_box (CanNet_mpool, rmsg);
                }
            }
            break;
        }
        case UART_INTERFACE:    // 通过串口
        {
            
            break;
        }
        default:                // 不支持接口
        {
            return  (RET_FRAME_ERR);
        }
    }

TRANS_RET:    
    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  辅助通道地址检测
//----------------------------------------------------------------------------------------------------
U8  Check_SubAddr(U8 id, U8 adr)
{
    U8  ret = RET_OK;

    switch (id) 
    {
    case VI_ID:     //电压单体辅助通道
        if (adr >= MAX_VI_BOARD) {  //地址溢出
            ret = RET_ADDR_ABN;
        }
        break;
    case TI_ID:     //温度单体辅助通道
        if (adr >= MAX_TI_BOARD) {  //地址溢出
            ret = RET_ADDR_ABN;
        }
        break;
    case II_ID:     //开关量入辅助通道
        if (adr >= MAX_II_BOARD) {  //地址溢出
            ret = RET_ADDR_ABN;
        }
        break;
    case OI_ID:     //开关量出辅助通道
        if (adr >= MAX_OI_BOARD) {  //地址溢出
            ret = RET_ADDR_ABN;
        }
        break;
    case XI_ID:     //其他扩展辅助通道
        if (adr >= MAX_XI_BOARD) {  //地址溢出
            ret = RET_ADDR_ABN;
        }
        break;
    case EBI_ID:    //扩展位输入辅助通道
        if (adr >= MAX_EBI_BOARD) { //地址溢出
            ret = RET_ADDR_ABN;
        }
        break;
    case EBO_ID:    //扩展位输出辅助通道
        if (adr >= MAX_EBO_BOARD) { //地址溢出
            ret = RET_ADDR_ABN;
        }
        break;
    case EDI_ID:    //扩展数据输入辅助通道
        if (adr >= MAX_EDI_BOARD) { //地址溢出
            ret = RET_ADDR_ABN;
        }
        break;
    case EDO_ID:    //扩展数据输出辅助通道
        if (adr >= MAX_EDO_BOARD) { //地址溢出
            ret = RET_ADDR_ABN;
        }
        break;
    default:
        ret = RET_FRAME_ERR;    //格式错误(不支持辅助通道类型)
        break;
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//eof
