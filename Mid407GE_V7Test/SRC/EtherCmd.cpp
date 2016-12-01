//----------------------------------------------------------------------------------------------------
// Header:      �����������
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
#include    "parallelchan.hpp"  // 16-0122 ��̫���������ش���
#include    "chan.hpp"
#include    "chanEther.hpp"
#include    "subchan.hpp"
#include    "app_can.hpp"
//----------------------------------------------------------------------------------------------------
//====================================================================================================
//----------------------------------------------------------------------------------------------------
//0x80 - ���������
//----------------------------------------------------------------------------------------------------
U8  CMD_CONNECTION_Proc(U8 *ptr, U16 *ret_len, const U8 *pip)
{
    U8  ret     = 0;
    U16 rem_at  = 0;
    U16 len, port_buf;

    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) == MASTER_PC) {
        port_buf = ptr[0] + ptr[1]*0x100;   //��������˿ں�
        if (port_buf != cDev.mHostCtrl.port) {
            cDev.mHostCtrl.port = port_buf;
            cDev.SaveHostCtrl();
        }
    }
    //��ȡ�豸��Ϣ
    len = cDev.Rd_DevInfo(ptr,(U8)rem_at);

//    if (len == 0) { // ʵ���ܲ��� 16-0802
//        ret = L3_LEN_ERR;
//    }
    *ret_len = len;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x81 - ��ȡʵʱ/��ϸ����  ע����ϸ����¼
/*
    ʵʱ����ϸ���ݵ��ȷ���
    (1) ʵʱ���ݲ���һ�δ�����ͨ��(������ʱû�п��������Ƭ�����ٶ�һ�ζ��ܴ���)
    (2) ��ϸ���ݲ��û�ɨ��ʽ��ֱ�����ݰ����Ȳ���һ��ͨ����¼(�������ɨ����ͨ����¼���Ⱥܳ������岻�������õ�������ͨ���ã�
        ����Ҳ�����������ϴ�)
    (3) ��������κ�һ��ͨ���Ĺ��̼�¼�����ܳ�����û�����꣬����������ϴ�
    (4) ��ϸ���ݵ�һ����ɨͨ��Ϊ�ϴ�����ɨ��ͨ������һ��
2015-0314
    (5) ʵʱ���ݣ���ϸ���ݷ�ʱ����--��ÿ���յ�Ҫô����ʵʱ��Ҫô������ϸ
2015-0318   ����
2015-1123   ���ȹ̶�,���豸��Ϣ���趨,һ���趨�͹̻�
*/
//----------------------------------------------------------------------------------------------------
#define     REALDATA_MARK   (0x01)      // ��ʵʱ���ݱ��
#define     RECDATA_MARK    (0x02)      // ����¼���ݱ��
#define     INITREC_MARK    (0x03)      // ����¼����(����ʼ������)���
//-----------------------------
#define     MAX_REPEAT_NUM  (4)         // ��ϸ��������ش��������ﵽ���ֵ��������¼�¼���壩
//-----------------------------
U8  ProcDataBuf[1500];      // ���̼�¼���ݻ���
//--------------------------------------------------
U8  CMD_RD_REALDATA_Proc(U8 *ptr, U16 *ret_len, U16 cutlen, const U8 *pip)
{
    U8  ret = 0;
    U8  i;
    U8  emptyCh = 0;        // �ռ�¼ͨ��������
    U8  procNum = 0;        // ���̼�¼����
    U8  type    = ptr[0];   // ��������
    U8  *pRecN  =&ptr[3];   // ��������
    U16 comCnt  = ptr[1] + ptr[2]*0x100;    // ͨ�ż�������ԭֵ���أ������ɹ����ͺ��1����
    U16 len, 
        tempLen;
    U16 leftLen;            // �����ʣ�೤�� ��ʱ��1300
    U16 rem_at;             // PC���ӻ�����

    if (cutlen < MAX_SENDPACK_LEN/2) {
        leftLen = MAX_SENDPACK_LEN-cutlen;
    } else {
        return  (PACK_LEN_ERR); // ��װ���ȴ���
    }
    //--SRAM�쳣ֱ�ӷ���--//
    if (cDev.GetGblFault() & GBL_SRAM_FAULT) {  // SRAMӲ������
        ptr[0]  = 0x01;     // ֻ����ʵʱ����
        ptr[1]  = ptr[1];
        ptr[2]  = ptr[2];
        ptr[3]  = 0;        // ��������������ʼ��0
        len     = 4;        // ָ���һ������(ͨ����)
        ptr    += 4;        
        for (i=0; i<(cDev.mInfo.chan_num); i++) {
            if ((cDev.mTxCtrl.realCh >= cDev.mInfo.chan_num)
            ||  (cDev.mTxCtrl.realCh >= MAX_CHAN_NUM)) {   // �������� ��> ��ʵʱ��ʼͨ��
                cDev.mTxCtrl.realCh = 0;
            }
            if ((len + FIXED_DATA_LEN) > leftLen) { // ���Ȳ�������ת
                break;
            }
            cChan[cDev.mTxCtrl.realCh].SetDummyRealDate(ptr,ERR_SRAM);  // SRAM���󣬷���
            len += cDev.LoadPackLen;
            ptr += cDev.LoadPackLen;
            
            *pRecN  += 1;               // ʵʱ����������һ
            cDev.mTxCtrl.realCh += 1;   // ʵʱ����ͨ����һ
        }
    } 
    //--��������SRAM�������--//
    else {
        //--PC���ӻ��ж�--//
        rem_at  = cDev.Test_RemotePC(pip);
        switch (rem_at & 0xff) 
        {
            default:        // �����Ǵ�
            {
                ptr[0]  = 0x01;     // ������������
                *pRecN  = 0;        // ����������
                len     = 4;        // ָ���һ������(ͨ����)
                break;
            }
            case MASTER_PC: //����
            {
                //==================== ����¼���� ====================
                /*  ���ݵ��ȷ��� --  
                        ����ȡ�������ݣ��ѹ������ݷŻ��壬�����ۼӣ�����ȡ���ݣ����ֳ��Ȳ�����ѻ����еĹ������ݲ���ĩβ��
                        ����ͨ��ȡ�꣬���ȵ��������أ�����ȡ��һ�����Դ����ơ�
                */
                if ((type == RECDATA_MARK) 
                ||  (type == INITREC_MARK)) {
                    // ��¼��������������
                    if (type == INITREC_MARK) {     // ��λ����ʼ���ش�
                        comCnt  = 0;                // ��ʼ������������
                        cDev.mTxCtrl.repeatNum  = 0;                                // ���ش�����
                        for (i=0; i<(cDev.mInfo.chan_num); i++) {                   // ���¶��ݴ��¼ָ��
                            cChan[i].RecoverReadPoint();                            // �ָ�ʵ�ʶ�ָ�뵽��ʱ��ָ��
                            cDev.mTxCtrl.recNum[i]  = cChan[i].GetUnReadRecNum();   // ��ʼ���ش� - ��ȡ��ͨ����¼����
                        }
                    } 
                    // ��ͨ�ش�
                    else if ((pRecBackUp->RecCnt != comCnt)
                    &&       (pRecBackUp->RecCnt != 0)) {
                        LED_Flash(LD4); // �ش�ָʾ
                        cDev.mTxCtrl.repeatNum++;
                        if (cDev.mTxCtrl.repeatNum >= MAX_REPEAT_NUM) { // �ﵽ���ֵ��ȡ���ش�
                            cDev.mTxCtrl.repeatNum = 0;
                            for (i=0; i<(cDev.mInfo.chan_num); i++) {
                                cChan[i].CoverReadPoint();                              // ������ʱ��ָ�뵽ʵ�ʶ�ָ��
                                cDev.mTxCtrl.recNum[i] = cChan[i].GetUnReadRecNum();    // �ش���ʱ - ��ȡ��ͨ����¼����
                            }
                        } else {
                            for (i=0; i<(cDev.mInfo.chan_num); i++) {                   // �ϴμ�¼δ�ɹ����գ����¶��ݴ��¼ָ��
                                cChan[i].RecoverReadPoint();                            // �ָ�ʵ�ʶ�ָ�뵽��ʱ��ָ��
                                cDev.mTxCtrl.recNum[i] = cChan[i].GetUnReadRecNum();    // �ش� - ��ȡ��ͨ����¼����
                            }
                        }
                    } 
                    // �ϴμ�¼�ɹ����ջ���λ����ʼ�����������������¶���¼ָ��
                    else {
                        cDev.mTxCtrl.repeatNum = 0;     // ���ش�����
                        for (i=0; i<(cDev.mInfo.chan_num); i++) {
                            cChan[i].CoverReadPoint();                              // ������ʱ��ָ�뵽ʵ�ʶ�ָ��
                            cDev.mTxCtrl.recNum[i] = cChan[i].GetUnReadRecNum();    // ���� - ��ȡ��ͨ����¼����
                        }
                    }
                    if (comCnt == 65535) {
                        pRecBackUp->RecCnt = 1;             // ����������1��ʼ��0Ϊ��ʼ������
                    } else {
                        pRecBackUp->RecCnt = comCnt + 1;    // ��������++
                    }
                    //--------------------------------------------------
                    // ��¼���ݲ��ִ���
                    if((cDev.mTxCtrl.recCh >= MAX_CHAN_NUM)
                    || (cDev.mTxCtrl.recCh >= cDev.mInfo.chan_num)) {   // ����¼��ʼͨ���Ϸ����ж�
                        cDev.mTxCtrl.recCh = 0;
                    }                    
//                  ptr[0]  = type;                         // 0 - ��������:��¼
                    ptr[1]  = pRecBackUp->RecCnt & 0xff;    // 1 - ��¼������
                    ptr[2]  = pRecBackUp->RecCnt >> 8;      // 2 - ��¼������
                    ptr[3]  = 0;                            // 3 - ��¼������ʼ��
                    len     = 4;                            // ָ�� 4 - ��һ������(ͨ����)
                    ptr    += 4;
                    while (leftLen >= cDev.LoadPackLen) {   // �ж���ϸ����
                        
                        if (cDev.mTxCtrl.recNum[cDev.mTxCtrl.recCh] > 0) {              //��ǰ�м�¼δ��
                            if (cChan[cDev.mTxCtrl.recCh].GetReadRecType() != PROC_REC) {   //�ǹ��̼�¼
                                cChan[cDev.mTxCtrl.recCh].SetOneRec(ptr);               //����û�м�¼��������ܷ���
                                leftLen -= cDev.LoadPackLen;
                                len += cDev.LoadPackLen;
                                ptr += cDev.LoadPackLen;
                                
                                *pRecN  = *pRecN + 1;       // ��ϸ��������++
                                cDev.mTxCtrl.recNum[cDev.mTxCtrl.recCh]--;
                            } else {                        // ���̼�¼����
                                leftLen -= PROCDATA_LEN;    // �̶�����
                                cChan[cDev.mTxCtrl.recCh].SetProcRec(&ProcDataBuf[procNum*PROCDATA_LEN]);
                                procNum++;                  // ���̼�¼��������
                                cDev.mTxCtrl.recNum[cDev.mTxCtrl.recCh]--;
                            }
                        } else {
                            emptyCh++;  //
                            if (emptyCh >= cDev.mInfo.chan_num) {   // ͨ����¼��ȡ���˳�
                                break;          
                            } else {
                                cDev.mTxCtrl.recCh++;               // ����ȡ��һ��ͨ����¼,�����¼�¼���ݳ���
                                if (cDev.mTxCtrl.recCh >= cDev.mInfo.chan_num) {
                                    cDev.mTxCtrl.recCh = 0;
                                }
                            }
                        }
                    }
                    // �й��̼�¼���ӵ�����β��
                    if (procNum > 0) {
                        *ptr++  = procNum;  // �������ݸ��� U16, �����ǳ���255��
                        *ptr++  = 0;
                        for (U16 x=0; x<(procNum * PROCDATA_LEN); x++) {    // ��������
                            *ptr++  = ProcDataBuf[x];
                            len++;
                        }
                        // ���������ﲻ���ӣ���ǰ�汣�滺��ʱ�Ѿ�����
                        ;
                    } else {    // �޹�������
                        *ptr++  = 0;    // �������ݸ�����
                        *ptr++  = 0;    // �������ݸ�����
                        len    += 2;
                    }
                    if ((emptyCh == 0) 
                    &&  (*pRecN) != 0) {    //��������ֻȡ��һ��ͨ����ͨ������Ҫ���ӣ���ֹ����
                        cDev.mTxCtrl.recCh++;
                    }
                }
                //==================== ��ʵʱ���� ====================
                /*
                    ʵʱ���ݵ��ȷ��� -- һ�δ����㹻��ʵʱ����
                */
                else {  // Ĭ��type,��ʵʱ����
//--------------------------------------------------
//                  ptr[0]  = type;     // 0 - ��������:ʵʱ
//                  ptr[1]  = ptr[1];   // 1 - ��¼�����ͣ�ʵʱ�޴���
//                  ptr[2]  = ptr[2];   // 2 - ��¼�����ߣ�ʵʱ�޴���
                    ptr[3]  = 0;        // 3 - ʵʱ������ʼ��
                    len     = 4;        // ָ�� 4 - ��һ������(ͨ����)
                    ptr    += 4;
                    for (i=0; i<(cDev.mInfo.chan_num); i++) {
                        if((cDev.mTxCtrl.realCh >= MAX_CHAN_NUM)
                        || (cDev.mTxCtrl.realCh >= cDev.mInfo.chan_num)) {  // ��ʵʱ��ʼͨ���Ϸ����ж�
                            cDev.mTxCtrl.realCh = 0;
                        }
                        if ((len + cDev.LoadPackLen) > leftLen) {       // ���Ȳ�������ת
                            break;
                        }
                        cChan[cDev.mTxCtrl.realCh].SetRealDate(ptr);    // ����ͨ������
                        len += cDev.LoadPackLen;
                        ptr += cDev.LoadPackLen; 
                        
                        *pRecN  += 1;               // ʵʱ����������һ
                        cDev.mTxCtrl.realCh += 1;   // ʵʱ����ͨ����һ
                    }
                    // ʵʱ�����У��չ������ݳ�����Ҫ���� 15-0516
                    *ptr++  = 0;    // �������ݸ�����
                    *ptr++  = 0;    // �������ݸ�����
                    len    += 2;
                }
                break;
            }
            case SLAVE_PC:  // �ӻ�    // 15-0119
            {
                U8  inx = rem_at >> 8;  // �ӻ��ڻ�����ע���
            
                ptr[0]  = 0x01; // ������������-ʵʱ
                *pRecN  = 0;    // ���ؼ�¼��ʵʱ����
                len     = 4;    // ָ���һ������(ͨ����)
                ptr    += 4;
                if (inx >= MAX_SLAVE_PC) {
                    inx = 0;
                }
                for (i=0; i<(cDev.mInfo.chan_num); i++) {
                    if (cDev.mPCSlaCtrl[inx].ch >= MAX_CHAN_NUM) {  // ��ʵʱ��ʼͨ��
                        cDev.mPCSlaCtrl[inx].ch = 0;
                    }
                    if ((len + cDev.LoadPackLen) > leftLen) {       // ���Ȳ�������ת
                        leftLen -= len; // ʣ����ó��� 15-0119
                        break;
                    }
                    cChan[cDev.mPCSlaCtrl[inx].ch].SetRealDate(ptr);    // ����ͨ��ʵʱ����
                    len += cDev.LoadPackLen;
                    ptr += cDev.LoadPackLen; 
                    
                    *pRecN  += 1;                   // ʵʱ����������һ
                    cDev.mPCSlaCtrl[inx].ch += 1;   // ʵʱ����ͨ����һ
                }
                //ʵʱ�����У��չ������ݳ�����Ҫ���� 15-0516
                *ptr++  = 0;    // �������ݸ�����
                *ptr++  = 0;    // �������ݸ�����
                len    += 2;
                
                leftLen -= len; // ʣ����ó���
                break;
            }
        }
    }
    // ----- ͨ�÷��ز��� -----
    // ���غ�����ѹ�ź�
//  test    
//    cDev.SetUIMark(ptr);
//    ptr    += 4;    
    *ptr++  = 0x01;
    *ptr++  = 0x02;
    *ptr++  = 0x03;
    *ptr++  = 0x04;

    len    += 4;
    //����IO����
    tempLen = cSubChan.SetIOData(ptr, cDev.mInfo.dev_str_type); // ��Ӧ��ǰ��Ӳ���汾����λ������ʱ��Ҫ��Ӧ
    len    += tempLen;
    ptr    += tempLen;
    //���ر�����
    *ptr++ = 0;
    *ptr++ = 0;
    len   += 2;

    *ret_len = len;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x82 - ͨ�����Ʒ���
//  16-0125�������������δ���ŵ�ͨ�������趨��ͨ���� < MAX_CHAN_NUM��
//  16-0128������ͨ�������أ���Ҫ��һ�������·��
//----------------------------------------------------------------------------------------------------
U8  CMD_CIRCUIT_CTRL_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0;
    U8  i,j,k;
    U16 chan_tmp;

    U16 rem_at  = cDev.Test_RemotePC(pip);

    if ((rem_at&0xff) != MASTER_PC) {   // ������������δ���Σ���λ������ 16-0802��
        ;
    }

    for (i=0; i<(MAX_CHAN_NUM/8+1); i++) {  // ѡ�е�ͨ��
        if (i == (MAX_CHAN_NUM/8)) {        // ��󼸸�ͨ������
            k = MAX_CHAN_NUM & 0x07;
        } else {
            k = 8;
        }
        for (j=0; j<k; j++) {
            chan_tmp = i*8 + j;
            if (chan_tmp >= MAX_CHAN_NUM) {   // ͨ���߽����� 16-0122
                break;
            }
            if (ptr[i] & B8_MASK_N[j]) {    // ͨ��ѡ��
                ret = cChan[chan_tmp].ManualCtrl(&ptr[32]);
                // ���ݷ����������
                switch (ret) 
                {
                    case 0x00:      // ��ȷ��������һͨ��
                    {
                        break;
                    }
                    case 0x01:      // ����ֱ�ӷ��أ����ٽ�һ������
                    {
                        return (ret);
                    }
                    case 0xf1:      // ����λ�����ϣ�Ȼ�����
                    {
                        U32 myid;
#ifdef  EN_PARALLEL     // ʹ�ܲ�������λ��ֹͣ��ͨ�����쳣ʱ������λ��ת��������·����� 16-0122
                        U32 i;
                        U16 node_first, node_last;

                        node_first  = cDev.mInfo.par_node_num * chan_tmp;   // ������Ӧ�����׻�·��ַ
                        node_last   = cDev.mInfo.par_node_num + node_first; // ������Ӧ����ĩ��·��ַ
                        for (i=node_first; i<node_last; i++) { // �������·ɨ��
                            if (i < MAX_NODE_NUM) {

                               // ��ͳCANID����ַ+����
                                myid = (MCAN_CLRERRBIT << 0) | ((i + 1) << 8);
                                // ���͵���λ�����ȴ�Ӧ��
                                CanNet_CmdSend(myid, MBX_Net2MCan, 2);
                            }
                        }
#else       // �ǲ���
                        // ��ͳCANID����ַ+����
                        myid = (MCAN_CLRERRBIT << 0) | ((chan_tmp + 1) << 8);

                        // ���͵���λ�����ȴ�Ӧ��
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
/*  ���������������ͷ
    ����  *pstr   ���ݻ���ָ��
    ����  ret     0:���ݲ��Ϸ� 1�����ݺϷ�
*/
//----------------------------------------------------------------------------------------------------
U8  Chk_StartHead(strStartCmd *pstr)
{
    U8  ret = 1;

    if ((pstr->global.Start_mode != CONTINUE_START)
    &&  (pstr->global.Start_mode > pstr->pageline.TotLine)) {
        return  (0);        // �����������������кŴ���
    }
    if ((pstr->global.Tst_ViNum > MAX_TEST_VINUM) 
    ||  (pstr->global.Tst_TiNum > MAX_TEST_TINUM)
    ||  (pstr->global.Tst_XiNum > MAX_TEST_XINUM)) {
        return  (0);        // ���Ը����������
    }
    if ((pstr->pageline.TotPage     == 0)
    ||  (pstr->pageline.PageNo      > pstr->pageline.TotPage)
    ||  (pstr->pageline.TotLine     == 0)
    ||  (pstr->pageline.PageLine    == 0)
    ||  (pstr->pageline.TotLine     < pstr->pageline.PageLine)
    ||  (pstr->pageline.TotLine     > MAX_WKLINE)) {
        return  (0);        // ҳ������Ŀ����
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
/*  ���׶ι�������
    ����  prg     ���ݻ���
          inx     �׶�����
    ����  ret 0:���ݲ��Ϸ� 1�����ݺϷ�
*/
//----------------------------------------------------------------------------------------------------
U8  Chk_StpProg(strStpPrg prg, U8 inx)
{
    U8  ret = 1;

    if (prg.No > (inx+1)) {
        return  (0);        //�����Ų�����
    }
    if (prg.Mode == MODE_AND) { // AND�ṹ���ж�
        return  (1);    
    }
    if ((prg.Mode >= MIN_ERR_MODE)
    &&  (prg.Mode <= MAX_ERR_MODE)) {
        return  (0);        //�Ƿ�ģʽ
    }
    if ((prg.Pv_type1 >= MIN_ERR_VAR_PAR) 
    ||  (prg.Pv_type2 >= MIN_ERR_VAR_PAR)) {
        return  (0);        //�Ƿ����������
    }
    if ((prg.Pv_opt1 >= MIN_ERR_OP)
    ||  (prg.Pv_opt2 >= MIN_ERR_OP)) {
        return  (0);        //�Ƿ�������
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x83 - ͨ����������
//----------------------------------------------------------------------------------------------------
U8  CMD_CIRCUIT_START_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0;
    U32 i;
    strStartCmd *pBuf   = (strStartCmd *)ptr;
    U16         rem_at  = cDev.Test_RemotePC(pip);

    if ((rem_at&0xff) != MASTER_PC) {   //������������δ���Σ���λ������ 16-0802��
        ;
    }
    if (Chk_StartHead(pBuf) == 0) {
        return  (1);    // ����ͷ����ֱ�ӷ���
    }
    //����״̬ɢת
    switch (PrgRcvCtrl.RcvStat) 
    {
    case RECV_IDLE:     //���У����� -> ͨ����ȫ�ֱ�����������ҳ�����������Ӳ���(����)������Ӧ���
        if (pBuf->pageline.PageNo == 0) {   // �����ǵ�0ҳ����
            memcpy(&PrgRcvCtrl.DataBuf.ch[0], ptr, sizeof(strStartHead));
            PrgRcvCtrl.CurPage  = 0;    // ����ҳ��ʼ��
            PrgRcvCtrl.CurLine  = 0;    // �׶��г�ʼ��
            // ����׶ι�������
            
            for (i=0; i<pBuf->pageline.PageLine; i++) {
                memcpy ((U8 *)&PrgRcvCtrl.DataBuf.step[i].No, 
                        &ptr[sizeof(strStartHead)+sizeof(strStpPrg)*i], 
                        sizeof(strStpPrg)); // ����׶ι�������
                if (Chk_StpProg(PrgRcvCtrl.DataBuf.step[i], i) == 0) {
                    return  (1);    // ��һ�����׶ι��մ���
                }
                PrgRcvCtrl.CurLine++;   // ���½����кţ�������һҳ������У�
            }
            PrgRcvCtrl.CurPage++;       // �����˵�һҳ
            // �ж��Ƿ����
            if (PrgRcvCtrl.CurPage >= PrgRcvCtrl.DataBuf.pageline.TotPage) {
                PrgRcvCtrl.RcvStat  = RECV_END;
            } else {
                PrgRcvCtrl.RcvStat  = RECV_GOING;
                PrgRcvCtrl.ToutCnt  = PRG_RCV_TICK; //��ʼ����ʱ������
            }
        } else {
            return  (1);    // ��1ҳ���ǵ�1ҳ ���󷵻�
        }
        break;
        
    case RECV_GOING:    //�����У��������һҳ����Ӧ���
        if (pBuf->pageline.PageNo == PrgRcvCtrl.CurPage) {
            memcpy(&PrgRcvCtrl.DataBuf.ch[0], ptr, sizeof(strStartHead));
        } else {    //��ǰҳ��Ҫ�����ҳ
            PrgRcvCtrl.RcvStat  = RECV_IDLE;
            PrgRcvCtrl.ToutCnt  = 0;    // �峬ʱ������
            return (1);     // ��nҳ���ǵ�nҳ ���󷵻�
        }
        // ����׶ι�������
        for (i=0; i<pBuf->pageline.PageLine; i++) {
            memcpy ((U8 *)&PrgRcvCtrl.DataBuf.step[i+PrgRcvCtrl.CurLine].No, 
                    &ptr[sizeof(strStartHead)+sizeof(strStpPrg)*i], 
                    sizeof(strStpPrg));         // ����׶ι�������
            if (Chk_StpProg(PrgRcvCtrl.DataBuf.step[i+PrgRcvCtrl.CurLine], i+PrgRcvCtrl.CurLine) == 0) {
                PrgRcvCtrl.RcvStat  = RECV_IDLE;
                PrgRcvCtrl.ToutCnt  = 0;    // �峬ʱ������
                return  (1);    // ��һ�����׶ι��մ���
            }
        }
        PrgRcvCtrl.CurLine += pBuf->pageline.PageLine;  // ���½����кţ�������һҳ������У�
        PrgRcvCtrl.CurPage++;   //��ǰ����ҳ����
        //�ж��Ƿ����
        if (PrgRcvCtrl.CurPage >= PrgRcvCtrl.DataBuf.pageline.TotPage) {
            PrgRcvCtrl.RcvStat  = RECV_END;
        } else {
            PrgRcvCtrl.ToutCnt  = PRG_RCV_TICK; //��ʼ����ʱ������
        }
        break;
        
    case RECV_END:      //���ս������ȴ����������ٽ����κι���
        ret = 2;
        break;
        
    default:            //�쳣���ص����еȴ�״̬
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
//0x84 - CMD_MODIFY_DEVINFO �޸��豸��Ϣ 
//  16-0129 �޸��豸��Ϣ�ɹ��󣬸��²����ڵ�ӳ���
//----------------------------------------------------------------------------------------------------
U8  CMD_MODIFY_DEVINFO_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x89 - MAC��IP��ַ����
//----------------------------------------------------------------------------------------------------
U8  CMD_CFG_MAC_IP_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x8a - ��ȡָ���豸�ŵ�MAC��IP
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_DEV_MAC_IP_Proc(U8 *ptr, U16 *ret_len)
{
    U8  ret = 0;

    *ret_len = cDev.GetMACIP(ptr);

    *ret_len += 2;  //�����ֶ�2�ֽ�

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0x8b - ��ȡPC����IP��ַ�Լ��豸MAC
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
//0x8c - ��������
//----------------------------------------------------------------------------------------------------
U8  CMD_RQ_HOSTCTRL_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0, i;
    U16 rem_at = 0, port_buf;

    port_buf = ptr[0] + ptr[1]*0x100;
    rem_at = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) == SLAVE_PC) {    //ֻ�дӻ�������������
        for (i=0; i<4; i++) {
            cDev.mHostCtrl.ip[i] = pip[i];
        }
        cDev.mHostCtrl.port = port_buf;
        cDev.SaveHostCtrl();
        //����PC�ӻ�����
        cDev.EmptyPCSlaCtrl(rem_at>>8);
        ret = 0x00;
    } else {    // ����������������
        ret = 0x01;
    }

    return  (ret);
}
//====================================================================================================
//  ��ͨ������
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//0x90 - ��ȡ�����ж�����λ
//0x92 - ��ȡУ׼����
//0x94 - ��ȡ�������μ�������
//0x96 - ��ȡ�����
//0x98 - ��ȡ���ѹ
//0x9A - ��ȡ����PID
//0x9C - ��ȡ��ѹPID
//0x9E - ��ȡ��ʱʱ��T
//0xA0 - ��ȡ������ֵS
//0xA2 - ��ȡ����汾
//0xBA - ��ȡ��չ��ʱʱ��
//0xBC - ��ȡ��չ������ֵ
//0xC0 - ��ȡĸ�߲��� 15-1223
//0xC2 - ��ȡƵ�ʵȲ��� 15-1223
//0xC4 - ��ȡռ�ձȲ��� 15-1223
//----------------------------------------------------------------------------------------------------
//  ���ܲ������͸�����͵���λ������ַ���ư������괦�� 16-0129
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_ChanInfo_Proc(U8 cmd, U8 *ptr, U16 *ret_len)
{
    U8  ret     = 0x00;
    U8  chan    = ptr[0]+1; // ��ͨ���� 15-1203
    U32 myid;
    static  CAN_msg s_tmsg,s_rmsg;
    static  CAN_msg *rmsg;  //,*tmsg;

#ifdef  EN_PARALLEL     // ����������ַ����Ϊ���ڵ�
    if (chan > cDev.mInfo.chan_num * cDev.mInfo.par_node_num) {
        return  (RET_ADDR_ABN);
    }
#else                   // �ǲ�������ַ����Ϊ����ͨ����
    if (chan > cDev.mInfo.chan_num) {
        return  (RET_ADDR_ABN);
    }
#endif

    if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, 0) == OS_R_OK) { // ����֮ǰ�ȶ����ջ��壬��� 15-1223
        _free_box (CanNet_mpool, rmsg);
    }

    //PC����ӳ�䵽CAN����
    switch (cmd) 
    {
    case CMD_RD_SYSBIT:             //0x90-��ϵͳ����λ����������λ�����ܿ��ƿ��أ�
        myid = MCAN_RD_SYSBIT | chan<<8;
        break;
    case CMD_RD_CALIB:              //0x92-��ȡУ׼����
        myid = MCAN_RD_CALIB | chan<<8;
        break;
    case CMD_RD_MASKREG:            //0x94-��ȡ�������μ�������
        myid = MCAN_RD_REGMD | chan<<8;
        break;
    case CMD_RD_IRATE:              //0x96-��ȡ�����
        myid = MCAN_RD_IRATE | chan<<8;
        break;
    case CMD_RD_URATE:              //0x98-��ȡ���ѹ
        myid = MCAN_RD_URATE | chan<<8;
        break;
    case CMD_RD_IPID:               //0x9A-��ȡ����PID
        myid = MCAN_RD_IPID | chan<<8;
        break;
    case CMD_RD_UPID:               //0x9C-��ȡ��ѹPID
        myid = MCAN_RD_UPID | chan<<8;
        break;
    case CMD_RD_TVALUE:             //0x9E-��ȡ��ʱʱ��
        myid = MCAN_RD_TPARA | chan<<8;
        break;
    case CMD_RD_SVALUE:             //0xA0-��ȡ������ֵ
        myid = MCAN_RD_SPARA | chan<<8;
        break;
    case CMD_RD_VERSION:            //0xA2-��ȡ����汾
        myid = MCAN_RD_VER | chan<<8;
        break;
    case CMD_RD_TVALUE_EX:          //0xBA-��ȡ��չ��ʱʱ��
        myid = MCAN_RD_TPARA_EX | chan<<8;
        break;
    case CMD_RD_SVALUE_EX:          //0xBC-��ȡ��չ������ֵ
        myid = MCAN_RD_SPARA_EX | chan<<8;
        break;
    case CMD_RD_BUSDATA:            //0xC0-��ȡĸ�߲��� 15-1223
        myid = MCAN_RD_VBUSDATA | chan<<8;
        break;
    case CMD_RD_FREQDATA:           //0xC2-��ȡƵ�ʵȲ��� 15-1223
        myid = MCAN_RD_FREQDATA | chan<<8;
        break;
    case CMD_RD_DUTYDATA:           //0xC4-��ȡռ�ձȲ��� 15-1223
        myid = MCAN_RD_DUTYDATA | chan<<8;
        break;
    default:    //��Ч����
        myid = 0;
        ret  = RET_FRAME_ERR;
        goto RD_ChanInfo_Retloop;
    }
    
    // ���͵���λ�����ȴ�Ӧ�� 16-0122������Ϊͨ������������Ϣ����
    if (cmd == CMD_RD_CALIB) {  // ��У׼������Ҫ���⴦��
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
    //�ȴ�������λ����Ӧ��
    if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, CTRL_05S) == OS_R_TMO) { //����λ����ʱ
        ret = RET_RECV_TOUT;
    } else {
        s_rmsg = *rmsg;
        _free_box (CanNet_mpool, rmsg);
        
        //Ӧ�����,ֱ�ӷ���

        if (s_rmsg.id != (myid|0x80)) {

            ret = RET_DATA_ERR;
        }
    }

    if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, 0) == OS_R_OK) { //����λ��Ӧ����Ϣ����ȡһ�Σ���ֹ����
        _free_box (CanNet_mpool, rmsg);
    }
    //����ش�PC
RD_ChanInfo_Retloop:

    ptr[1]  = ret;  // ������
    //���󷵻�
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
//0x91 - д������ж�����λ
//0x93 - д��У׼����
//0x95 - �������μ�����������
//0x97 - д������
//0x99 - д����ѹ
//0x9B - д�����PID
//0x9D - д���ѹPID
//0x9F - д����ʱʱ��
//0xA1 - д�뱨����ֵ
//0xA3 - д��CAN��ַ
//0xA5 - �����ָ�У׼����
//0xC1 - д��ĸ�߲���   15-1223
//0xC3 - д��Ƶ�ʵȲ��� 15-1223
//0xC4 - д��ռ�ձȲ��� 15-1223
//----------------------------------------------------------------------------------------------------
//  ����㲥���� 15-1203
//  �·�����������벢���޹أ�ֱ�ӶԻ�·���� 16-0129
//----------------------------------------------------------------------------------------------------
U8  CMD_WR_ChanInfo_Proc(U8 cmd, U8 *ptr, const U8 *pip)
{
    U8  ret     = 0x00;
    U8  rd_flag = 0;        // ˢ��ʵʱ���ݱ��
    U8  chan    = ptr[0]+1; // ��ͨ���� 15-1203
    U16 rem_at  = 0;
    U16 tout    = 0;        // �ȴ���λ��Ӧ��ʱ��
    U32 myid;               // ��λ��Ӧ��Ƚ���
    static  CAN_msg s_tmsg,s_rmsg;
    static  CAN_msg *rmsg;//,*tmsg;     // 16-0122

    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   // ������ֱ���˳�
        ptr[1]  = RET_NOTMASTERPC;
        return  (RET_NOTMASTERPC);
    }

#ifdef  EN_PARALLEL     // ����������ַ����Ϊ���ڵ�
    if (chan > cDev.mInfo.chan_num * cDev.mInfo.par_node_num) {
        return  (RET_ADDR_ABN);
    }
    if (cmd == CMD_WR_CANADDR) {
        if ((ptr[1] > cDev.mInfo.chan_num * cDev.mInfo.par_node_num)
        ||  (ptr[1] == 0)) {    // �趨�ĵ�ַ���
            return  (RET_ADDR_ABN);
        }
    }
#else                   // �ǲ�������ַ����Ϊ����ͨ����
    if (chan > cDev.mInfo.chan_num) {
        return  (RET_ADDR_ABN);
    }
    if (cmd == CMD_WR_CANADDR) {
        if ((ptr[1] > cDev.mInfo.chan_num)
        ||  (ptr[1] == 0)) {    // �趨�ĵ�ַ���
            return  (RET_ADDR_ABN);
        }
    }
#endif

#ifdef  EN_CAN_BROADCAST   // ʹ��CAN����㲥 16-0113    
    if (gFunSelBit == FUNC_BOARDCAST) { // �㲥�����ַ����Ϊ�㲥��ַ
        chan = BROADCAST_ADDR;
    }
#endif
    
    if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, 0) == OS_R_OK) { // ����֮ǰ�ȶ����ջ��壬��� 15-1223
        _free_box (CanNet_mpool, rmsg);
    }
    
    // PC����ӳ�䵽CAN����
    
//--------------------------------------------------
  // ��ͳCANID
//--------------------------------------------------
    switch (cmd) 
    {
    case CMD_WR_SYSBIT:         //0x91-д��ϵͳ����λ
        s_tmsg.id = MCAN_WR_SYSBIT;
        break;
    case CMD_WR_CALIB:          //0x93-д��У׼����
        s_tmsg.id = MCAN_WR_CALIB;
        tout = CTRL_2S; //��λ������ִ��ʱ��ϳ�,�������ӵȴ� 15-0814
        rd_flag = 1;    // ˢ��ʵʱ
        break;
    case CMD_WR_MASKREG:        //0x95-д�뱨�����μ�������ģʽ
        s_tmsg.id = MCAN_WR_REGMD;
        break;
    case CMD_WR_IRATE:          //0x97-д������
        s_tmsg.id = MCAN_WR_IRATE;
        break;
    case CMD_WR_URATE:          //0x99-д����ѹ
        s_tmsg.id = MCAN_WR_URATE;
        break;
    case CMD_WR_IPID:           //0x9B-д�����PID
        s_tmsg.id = MCAN_WR_IPID;
        break;
    case CMD_WR_UPID:           //0x9D-д���ѹPID
        s_tmsg.id = MCAN_WR_UPID;
        break;
    case CMD_WR_TVALUE:         //0x9F-д����ʱʱ��
        s_tmsg.id = MCAN_WR_TPARA;
        break;
    case CMD_WR_SVALUE:         //0xA1-д�뱨����ֵ
        s_tmsg.id = MCAN_WR_SPARA;
        break;
    case CMD_WR_TVALUE_EX:      //0xBB-д����չ��ʱʱ��
        s_tmsg.id = MCAN_WR_TPARA_EX;
        break;
    case CMD_WR_SVALUE_EX:      //0xBD-д����չ������ֵ
        s_tmsg.id = MCAN_WR_SPARA_EX;
        break;
    case CMD_WR_CANADDR:        //0xA3-д��CAN��ַ
        s_tmsg.id = MCAN_WR_ADDR;
        break;
    case CMD_CALIBSET:          //0xA5-�����ָ�У׼����
        s_tmsg.id = MCAN_CALIBSET;
        tout = CTRL_2S; //��λ������ִ��ʱ��ϳ�,�������ӵȴ�
        break;
    case CMD_WR_BUSDATA:        //0xC0-д��ĸ�߲��� 15-1223
        s_tmsg.id = MCAN_WR_VBUSDATA;
        break;
    case CMD_WR_FREQDATA:       //0xC2-д��Ƶ�ʵȲ��� 15-1223
        s_tmsg.id = MCAN_WR_FREQDATA;
        break;
    case CMD_WR_DUTYDATA:       //0xC4-д��ռ�ձȲ��� 15-1223
        s_tmsg.id = MCAN_WR_DUTYDATA;
        break;
    default:
        break;
    }
    // ����ͨ������ͨ����canid�Ա�����Ƚ�   
    myid = s_tmsg.id | (chan << 8);

    myid |= 0x80;   // CANӦ��������λ    

    // ���͵���λ�����ȴ�Ӧ���ȿ�����������
    s_tmsg.len      = 8;
    s_tmsg.ch       = 0;
    s_tmsg.format   = EXTENDED_FORMAT;
    s_tmsg.type     = DATA_FRAME;
#ifdef  UCALIB_UNION    // ��ŵ�ѹǿ������У׼
    if (cmd == CMD_WR_CALIB) {
        if (ptr[1] == 0x02) {   //��ѹУ׼
            ptr[2] |= 0x30;
        }
    }
#endif
    for(int i=0; i<8; i++) {
        s_tmsg.data[i] = ptr[1+i];
    }    

    // PC����ӳ�䵽CAN����
                      // ��ͳCANID����ַ+����
    s_tmsg.id |= (chan << 8);
      
    // ���͵���λ�����ȴ�Ӧ��
    ret = CanNet_MsgSend(s_tmsg, MBX_Net2MCan, 2);

    if (ret) {
        goto WR_ChanInfo_Retloop;
    } 
    else if (chan == BROADCAST_ADDR) {  // �㲥������Ϣ��ֱ�ӷ��� 15-1203
        goto WR_ChanInfo_Retloop;
    }
    // �ȴ�������λ����Ӧ��
    if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, (CTRL_05S+tout)) == OS_R_TMO) {  //д��λ����ʱ
        ret = RET_RECV_TOUT;
    } else {
        s_rmsg = *rmsg;
        _free_box (CanNet_mpool, rmsg);
        // Ӧ�����,ֱ�ӷ���
        if (s_rmsg.id != myid) {    // Ӧ�����������������Ϊ������Ӧ��16-0122
            ret = RET_DATA_ERR;
        }
    }
    if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, 0) == OS_R_OK) { // д��λ����Ϣ����ȡһ�Σ���ֹ����
        _free_box (CanNet_mpool, rmsg);
    }
    // �ж�ȡʵʱ����Ҫ�� 14-1018 û���ж�����CAN���� tbc......
    if (rd_flag) {        
        // �·���ʵʱ���ݣ�����ʱ��Ϊ�������ӻ���δ���� 16-0122��
// ��ͳCANID
        s_tmsg.id = MCAN_RD_REALDATA1 | chan<<8;
      
        // ���͵���λ�������ȴ�Ӧ��
        CanNet_CmdSend(s_tmsg.id, MBX_Net2MCan, 0);
    }

WR_ChanInfo_Retloop:

    ptr[1]  = ret;  // ������

    return  (ret);
}
//====================================================================================================
//  ��λ������
//----------------------------------------------------------------------------------------------------
//0xA6-��ȡ��λ���������ò���
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_CtrlCfg_Proc(U8 *ptr, U16 *ret_len)
{
    U8  ret = 0x00;
    U8  mid = ptr[0];   // ��λ��ͨ�����

    mid = mid;
    if (mid != MIDEV_FLAG) {
        return  (RET_ADDR_ABN);
    }
    ptr += 1;           // ָ�򻺳�ʵ���������ֽ�
    *ret_len    = 1;    // ��λ����� 1�ֽ�
    *ret_len   += cDev.CopyCtrlCfg(ptr);

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xA7-д����λ���������ò���
//----------------------------------------------------------------------------------------------------
U8  CMD_WR_CtrlCfg_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0x00;
    U8  mid = ptr[0];   // ��λ��ͨ�����
    U16 rem_at;

    mid = mid;
    if (mid != MIDEV_FLAG) {
        return  (RET_ADDR_ABN);
    }
    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   //������ֱ���˳�
        return  (RET_NOTMASTERPC);
    }
    //��������
    if (cDev.CheckCtrlCfg((strCtrlCfg *)(ptr-3)) == 0) {    // ��ǰƫ��3��ָ�򻺳����ֽڣ�����ֶΣ������ڽ���
        ret = cDev.SaveCtrlCfg(&ptr[1]);    // ����
    } else {
        ret = 0x01;
    }
    //����״̬
    ptr[1] = ret;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xA8-��ȡ����ͨ��Ӳ�����ò���
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_SubChHCfg_Proc(U8 *ptr, U16 *ret_len)
{
    U8  ret = 0x00;
    U8  mid = ptr[0];   // ��λ��ͨ�����
    U8  sid = ptr[1];   // ����ͨ������

    if (mid != MIDEV_FLAG) {    // ��λ����Ǵ�
        return  (RET_ADDR_ABN);
    }
    if ((sid == 0)  // ����ͨ�����ʹ�
    ||  (sid > XI_ID)) {
        return  (RET_SUBID_ERR);
    }
    *ret_len = 3;   // �汾1�ֽ�
    *ret_len += cDev.CopySubHdCfg(&ptr[2], sid);

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xA9-д�븨��ͨ��Ӳ�����ò���
//----------------------------------------------------------------------------------------------------
U8  CMD_WR_SubChHCfg_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0x00;
    U8  mid = ptr[0];   // ��λ��ͨ�����
    U8  sid = ptr[1];   // ����ͨ������
    U16 rem_at  = 0;

    if (mid != MIDEV_FLAG) {    // ��λ����Ǵ�
        return  (RET_ADDR_ABN);
    }
    if ((sid == 0)  // ����ͨ�����ʹ�
    ||  (sid > XI_ID)) {
        return  (RET_SUBID_ERR);
    }
    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   //������ֱ���˳�
        return  (RET_NOTMASTERPC);
    }
    //��������
    ret = cDev.SaveSubHdCfg(&ptr[2], sid);
    //״̬����
    ptr[2]  = ret;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xAA-��ȡ����ͨ��������ò��� 15-0924 tbc
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_SubChSCfg_Proc(U8 *ptr, U16 *ret_len)
{
    U8  ret = 0x00;
    U8  mid = ptr[0];   // ��λ��ͨ�����
    U8  sid = ptr[1];   // ����ͨ������

    if (mid != MIDEV_FLAG) {    // ��λ����Ǵ�
        return  (RET_ADDR_ABN);
    }
    if ((sid == 0)  // ����ͨ�����ʹ�
    ||  (sid > XI_ID)) {
        return  (RET_SUBID_ERR);
    }
    *ret_len = 3;   // �汾1�ֽ�
    *ret_len += cDev.CopySubHdCfg(&ptr[2], sid);

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xAB-д�븨��ͨ��������ò��� 15-0924 tbc
//----------------------------------------------------------------------------------------------------
U8  CMD_WR_SubChSCfg_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0x00;
    U8  mid = ptr[0];   // ��λ��ͨ�����
    U8  sid = ptr[1];   // ����ͨ������
    U16 rem_at  = 0;

    if (mid != MIDEV_FLAG) {    // ��λ����Ǵ�
        return  (RET_ADDR_ABN);
    }
    if ((sid == 0)  // ����ͨ�����ʹ�
    ||  (sid > XI_ID)) {
        return  (RET_SUBID_ERR);
    }
    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   //������ֱ���˳�
        return  (RET_NOTMASTERPC);
    }
    //��������
    ret = cDev.SaveSubHdCfg(&ptr[2], sid);
    //״̬����
    ptr[2]  = ret;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xAE-��λ��λ��
//----------------------------------------------------------------------------------------------------
U8  CMD_RESET_Dev_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret = 0x00;
    U16 rem_at  = 0;

    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   //������ֱ���˳�
        ptr[2]  = RET_NOTMASTERPC;
        return  (RET_NOTMASTERPC);
    }
    ptr++;

    return  (ret);
}
//====================================================================================================
//  ����ͨ������
//====================================================================================================
//----------------------------------------------------------------------------------------------------
//0xB0 - ��ȡ����ͨ��ʵʱ���� 
//16-0311 ������ʹ��Ӳ�����ñ���ʱҲ��������
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_SubChReal_Proc(U8 *ptr, U16 *ret_len)
{
    U8  ret = RET_OK;

    if (ret) {
        ptr[2]  = ret;  // ������
        ptr[3]  = 0;    // 0����������
        *ret_len = 4;   // �������ݳ���
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xB2 - ��ȡ����ͨ�������
//0xB4 - ��ȡ����ͨ��У׼����
//0xB6 - ��ȡ����ͨ����չ���ݣ�CAN��ַ��AD���Ʋ�����
//----------------------------------------------------------------------------------------------------
U8  CMD_RD_SubChInfo_Proc(U8 cmd, U8 *ptr, U16 *ret_len)
{
    U8  ret = RET_OK;
    U8  cid = ptr[0];       // ����ͨ��������
    U8  sch = ptr[1];       // ����ͨ����
    U32 myid;               // �·�id
    static  CAN_msg s_tmsg,s_rmsg;
    static  CAN_msg *rmsg,*tmsg;    // 15-1223

    ret = Check_SubAddr(cid, sch);
    if (ret) {  //��ַ��ID����ֱ�ӷ���
        goto RD_SubChanInfo_Retloop;
    } 
    // ����CAN��ַƫ��
    switch (cid) 
    {
    case VI_ID: sch += VIADDR_OFFSET;   break;//��ѹ���帨��ͨ��
    case TI_ID: sch += TIADDR_OFFSET;   break;//�¶ȵ��帨��ͨ��
    case II_ID: sch += IIADDR_OFFSET;   break;//�������븨��ͨ��
    case OI_ID: sch += OIADDR_OFFSET;   break;//������������ͨ��
    case XI_ID: sch += XIADDR_OFFSET;   break;//������չ����ͨ��
    case EBI_ID:sch += EBIADDR_OFFSET;  break;//��չλ���븨��ͨ��
    case EBO_ID:sch += EBOADDR_OFFSET;  break;//��չλ�������ͨ��
    case EDI_ID:sch += EDIADDR_OFFSET;  break;//��չ�������븨��ͨ��
    case EDO_ID:sch += EDOADDR_OFFSET;  break;//��չ�����������ͨ��
    default:                            break;
    }
    // PC����ӳ�䵽����CAN����
    switch (cmd) 
    {
    case CMD_RD_SUBRATE:        //0xB2-��ȡ����ͨ�������
        myid = SCAN_RD_RATE | (sch + 1)<<8;
        break;
    case CMD_RD_SUBCALIB:       //0xB4-��ȡ����ͨ��У׼����
        myid = SCAN_RD_CALIB | (sch + 1)<<8;
        break;
    case CMD_RD_SUBEDATA:       //0xB6-��ȡ����ͨ����չ���ݣ�CAN��ַ��AD���Ʋ�����
        myid = SCAN_RD_EDATA | (sch + 1)<<8;
        break;
    default:    //δ֧������ - impossible here
        ret = RET_FRAME_ERR;
        goto RD_SubChanInfo_Retloop;
    }
    // ���͵���λ�����ȴ�Ӧ��
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
    // ȡһ�Σ���ջ���
    if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, 0) == OS_R_OK) {
        _free_box (CanNet_mpool, rmsg);
    }
    // ���������ʽ�͸���CAN
    if (os_mbx_check (CanNet_mpool) > 0) {
        tmsg = (CAN_msg *)_alloc_box (CanNet_mpool);
        if (tmsg) {
            *tmsg = s_tmsg;
            if (os_mbx_send (MBX_Net2SCan, tmsg, 2) == OS_R_TMO) {  // ������ͨ��
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
    // �ȴ�������λ����Ӧ��
    if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, CTRL_1S) == OS_R_TMO) {
        ret = RET_RECV_TOUT;
    } else {
        s_rmsg = *rmsg;
        _free_box (CanNet_mpool, rmsg);
        if (s_rmsg.id != (myid|0x80)) { //Ӧ�������
            ret = RET_DATA_ERR;
        }
    }
    // ��ȡһ�Σ���ֹ����
    if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, 0) == OS_R_OK) {
        _free_box (CanNet_mpool, rmsg);
    }
    //����ش�PC
RD_SubChanInfo_Retloop:
    ptr[2] = ret;  // ������
    //���󷵻�
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
    //���󷵻�
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
    *ret_len = 11;  //�̶����ȷ���

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xB1 - д�븨��ͨ��ʵʱ����
//0xB3 - д�븨��ͨ�������
//0xB5 - д�븨��ͨ��У׼����
//0xB7 - д�븨��ͨ����չ���ݣ�CAN��ַ��AD���Ʋ�����
//0xB9 - ����ͨ����������
//----------------------------------------------------------------------------------------------------
// 16-0319      ����У׼CAN��һ�ֽڱ��
#define     CALVIFLAG       4       // У׼��ѹ
#define     CALTIFLAG       5       // У׼�¶�
//----------------------------------------------------------------------------------------------------
U8  CMD_WR_SubChInfo_Proc(U8 cmd, U8 *ptr, const U8 *pip)
{
    U8  ret     = RET_OK;
    U8  cid     = ptr[0];   // ����ͨ��������
    U8  sch     = ptr[1];   // ����ͨ����
    U8  scmd    = ptr[10];  // ������
    U16 rem_at  = 0;
    U32 myid;
    static  CAN_msg s_tmsg,s_rmsg;
    static  CAN_msg *rmsg,*tmsg;    // 15-1223
    
    rem_at = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   //������ֱ���˳�
        ptr[2] = RET_NOTMASTERPC;
        return  (RET_NOTMASTERPC);
    }
    ret = Check_SubAddr(cid, sch);
    if (ret) {  //��ַ��ID����ֱ�ӷ���
        goto WR_SubChanInfo_Retloop;
    } 
    // ����CAN��ַƫ��
    switch (cid) 
    {
    case VI_ID: sch += VIADDR_OFFSET;   break;//��ѹ���帨��ͨ��
    case TI_ID: sch += TIADDR_OFFSET;   break;//�¶ȵ��帨��ͨ��
    case II_ID: sch += IIADDR_OFFSET;   break;//�������븨��ͨ��
#ifdef  USE_EXTIO_RDWR      // ʹ����չIO��д(˫�Ƕ���100A/5V��·����) 16-0418
    case OI_ID:             // XiChan[1]��չ����������ӳ���봲1��2����ѹ����չ[1]:1��2�ſ��������
        if ((ptr[2] == 0x01)
        &&  (sch < 8)) {
            ptr[2] = 1<<sch;        // �봲IO��ַת��Ϊ��չ����������
            sch = XIADDR_OFFSET+1;  // �̶�Ϊ XiChan[1]
        }
        break;
#else        
    case OI_ID: sch += OIADDR_OFFSET;   break;//������������ͨ��
#endif
    case XI_ID: sch += XIADDR_OFFSET;   break;//������չ����ͨ��
    case EBI_ID:sch += EBIADDR_OFFSET;  break;//��չλ���븨��ͨ��
    case EBO_ID:sch += EBOADDR_OFFSET;  break;//��չλ�������ͨ��
    case EDI_ID:sch += EDIADDR_OFFSET;  break;//��չ�������븨��ͨ��
    case EDO_ID:sch += EDOADDR_OFFSET;  break;//��չ�����������ͨ��
    default:                            break;
    }
    // PC����ӳ�䵽����CAN����
    switch (cmd) 
    {
    case CMD_WR_SUBREAL:        //0xB1-д�븨��ͨ��ʵʱ����
        if (((cid == OI_ID) || (cid == EBO_ID))
        &&  (scmd == SCAN_RD_REALDATA2)) {
            myid = SCAN_WR_REALDATA2 | (sch + 1)<<8;    //��תֵ���
        } else {
            myid = SCAN_WR_REALDATA1 | (sch + 1)<<8;    //����ֵ���
        }
        break;
    case CMD_WR_SUBRATE:        //0xB3-д�븨��ͨ�������
        myid = SCAN_WR_RATE | (sch + 1)<<8;
        break;
    case CMD_WR_SUBCALIB:       //0xB5-д�븨��ͨ��У׼����
        myid = SCAN_WR_CALIB | (sch + 1)<<8;
        break;
    case CMD_WR_SUBEDATA:       //0xB7-д�븨��ͨ����չ���ݣ�CAN��ַ��AD���Ʋ�����
        myid = SCAN_WR_EDATA | (sch + 1)<<8;
        break;
    case CMD_WR_SUBSINGLE:      //0xB9-����ͨ����������(����)
        myid = SCAN_WR_LOCK | (sch + 1)<<8;
        break;
    default:
        break;
    }
    //���͵���λ�����ȴ�Ӧ��
    s_tmsg.id       = myid;
    s_tmsg.len      = 8;
    s_tmsg.ch       = 0;
    s_tmsg.format   = EXTENDED_FORMAT;
    s_tmsg.type     = DATA_FRAME;
    if ((cmd == CMD_WR_SUBRATE)     // д�븨��ͨ������� 16-0926
    ||  (cmd == CMD_WR_SUBCALIB)) { // ����У׼����һ�ֽ�������ͱ�� 16-0319
        s_tmsg.data[0]  = cid + 3;  // �����ѹ1->4 �Դ�����
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
    // ȡһ�Σ���ջ��� 15-1223
    if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, 0) == OS_R_OK) {
        _free_box (CanNet_mpool, rmsg);
    }
    // ���������ʽ�͸���CAN
    if (os_mbx_check (CanNet_mpool) > 0) {
        tmsg = (CAN_msg *)_alloc_box (CanNet_mpool);
        if (tmsg) {
            *tmsg = s_tmsg;
            if (os_mbx_send (MBX_Net2SCan, tmsg, 2) == OS_R_TMO) {  // д����ͨ��
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
    //����Ƕ�ȡ����ͨ��ʵʱ,������λ��Ӧ�� 15-1126
    if (cmd == CMD_WR_SUBREAL) {
        goto    WR_SubChanInfo_Retloop;
    }
    // �ȴ�������λ����Ӧ��
    if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, CTRL_05S) == OS_R_TMO) { //��ʱ
        ret = RET_RECV_TOUT;
    } else {
        s_rmsg = *rmsg;
        _free_box (CanNet_mpool, rmsg);
        if (s_rmsg.id != (myid|0x80)) { //Ӧ�������
            ret = RET_DATA_ERR;
        }
    }
    // ��ȡһ�Σ���ֹ����
    if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, 0) == OS_R_OK) {
        _free_box (CanNet_mpool, rmsg);
    }

WR_SubChanInfo_Retloop:
    ptr[2] = ret;  // ������

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xBE - ͨ����ת
//----------------------------------------------------------------------------------------------------
U8  CMD_CommTransfer_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret     = RET_OK;
    U16 rem_at  = 0;

    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   //������ֱ���˳�
        ptr[2]  = RET_NOTMASTERPC;
        return  (RET_NOTMASTERPC);
    }


    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//0xF1 - ͸������ 16-0801
//----------------------------------------------------------------------------------------------------
U8  CMD_Transparent_Proc(U8 *ptr, const U8 *pip)
{
    U8  ret     = RET_OK;
    U16 rem_at  = 0;

    static  CAN_msg s_tmsg,s_rmsg;
    static  CAN_msg *rmsg;

    rem_at  = cDev.Test_RemotePC(pip);
    if ((rem_at&0xff) != MASTER_PC) {   // ������ֱ���˳�
        return  (RET_NOTMASTERPC);
    }
    
    U8  media   = ptr[0];       // �������
    U8  port    = ptr[1];       // ���ʶ˿�����
    U16 len     = ptr[2] 
                + ptr[3]*0x100; // ���ݳ���
    U32 myid;                   // ��λ��Ӧ��Ƚ���
    
    switch (media)
    {
        case CAN_INTERFACE:     // ͨ��CAN�ڣ��̻�Ϊ����չID + 8�ֽ�����֡��
        {
            if ((port >= CAN_CTRL_MAX_NUM)
            ||  (len != 12)) {  // ����Ӳ�������򳤶Ȳ���
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
            
#ifdef  USE_SIM_CANCMD_ACK  // ģ��Ӧ��CAN����   
            switch(ptr[4])
            {
                case (CMD_ONLINE_CHK):        // ���߼�⣬���ذ汾��Ϣ
                {
                    ptr[4] = ptr[4];
                    break;
                }
                case (CMD_SYSTEM_RST):        // ϵͳ��λ
                {
                    ptr[4] = ptr[4];
                    break;
                }
                case (CMD_ERASE_FLASH):       // ���� Flash��ÿ�β���һ��������1K��
                                    {
                    ptr[4] = ptr[4];
                    break;
                }
                case (CMD_SET_BAUDRATE):      // ���ò�����
                                    {
                    ptr[4] = ptr[4];
                    break;
                }
                case (CMD_UPDATE_PROGPAR):    // ���³����������ʼ��ַ�����ȣ��̶�1024+2�ֽ�CRC��
                                    {
                    ptr[4] = ptr[4];
                    break;
                }
                case (CMD_PROGRAM_FLASH):     // Flash ��̣����һ֡��������ʼ��
                                    {
                    ptr[4] = ptr[4];
                    break;
                }
                case (CMD_EXECUTE_APP):       // ��ת��APPִ��
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
                // ���͵���λ�����ȴ�Ӧ��
                ret = CanNet_MsgSend(s_tmsg, MBX_Net2MCan, 2);
                if (ret) {  // ��Ϣʧ��
                    goto TRANS_RET;
                } 
                // �ȴ�������λ����Ӧ��
                if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, (CTRL_2S)) == OS_R_TMO) {  //д��λ����ʱ
                    ret = RET_RECV_TOUT;
                } else {
                    s_rmsg = *rmsg;
                    _free_box (CanNet_mpool, rmsg);
                    // Ӧ�����,ֱ�ӷ���
                    if (s_rmsg.id != myid) {    // Ӧ�����������������Ϊ������Ӧ��16-0122
                        ret = RET_DATA_ERR;
                    }
                    
                    
                    
                    
                    
                    
                    
                }
                if (os_mbx_wait (MBX_MCan2Net, (void **)&rmsg, 0) == OS_R_OK) { // д��λ����Ϣ����ȡһ�Σ���ֹ����
                    _free_box (CanNet_mpool, rmsg);
                }  
            } else {    //����CANͨ��
                if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, 0) == OS_R_OK) {
                    _free_box (CanNet_mpool, rmsg);
                }                
                // ���͵���λ�����ȴ�Ӧ��
                ret = CanNet_MsgSend(s_tmsg, MBX_Net2SCan, 2);

                if (ret) {  // ��Ϣʧ��
                    goto TRANS_RET;
                }
                // �ȴ�������λ����Ӧ��
                if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, (CTRL_2S)) == OS_R_TMO) {  //д��λ����ʱ
                    ret = RET_RECV_TOUT;
                } else {
                    s_rmsg = *rmsg;
                    _free_box (CanNet_mpool, rmsg);
                    // Ӧ�����,ֱ�ӷ���
                    if (s_rmsg.id != myid) {    // Ӧ�����������������Ϊ������Ӧ��16-0122
                        ret = RET_DATA_ERR;
                    }
                }
                if (os_mbx_wait (MBX_SCan2Net, (void **)&rmsg, 0) == OS_R_OK) { // д��λ����Ϣ����ȡһ�Σ���ֹ����
                    _free_box (CanNet_mpool, rmsg);
                }
            }
            break;
        }
        case UART_INTERFACE:    // ͨ������
        {
            
            break;
        }
        default:                // ��֧�ֽӿ�
        {
            return  (RET_FRAME_ERR);
        }
    }

TRANS_RET:    
    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  ����ͨ����ַ���
//----------------------------------------------------------------------------------------------------
U8  Check_SubAddr(U8 id, U8 adr)
{
    U8  ret = RET_OK;

    switch (id) 
    {
    case VI_ID:     //��ѹ���帨��ͨ��
        if (adr >= MAX_VI_BOARD) {  //��ַ���
            ret = RET_ADDR_ABN;
        }
        break;
    case TI_ID:     //�¶ȵ��帨��ͨ��
        if (adr >= MAX_TI_BOARD) {  //��ַ���
            ret = RET_ADDR_ABN;
        }
        break;
    case II_ID:     //�������븨��ͨ��
        if (adr >= MAX_II_BOARD) {  //��ַ���
            ret = RET_ADDR_ABN;
        }
        break;
    case OI_ID:     //������������ͨ��
        if (adr >= MAX_OI_BOARD) {  //��ַ���
            ret = RET_ADDR_ABN;
        }
        break;
    case XI_ID:     //������չ����ͨ��
        if (adr >= MAX_XI_BOARD) {  //��ַ���
            ret = RET_ADDR_ABN;
        }
        break;
    case EBI_ID:    //��չλ���븨��ͨ��
        if (adr >= MAX_EBI_BOARD) { //��ַ���
            ret = RET_ADDR_ABN;
        }
        break;
    case EBO_ID:    //��չλ�������ͨ��
        if (adr >= MAX_EBO_BOARD) { //��ַ���
            ret = RET_ADDR_ABN;
        }
        break;
    case EDI_ID:    //��չ�������븨��ͨ��
        if (adr >= MAX_EDI_BOARD) { //��ַ���
            ret = RET_ADDR_ABN;
        }
        break;
    case EDO_ID:    //��չ�����������ͨ��
        if (adr >= MAX_EDO_BOARD) { //��ַ���
            ret = RET_ADDR_ABN;
        }
        break;
    default:
        ret = RET_FRAME_ERR;    //��ʽ����(��֧�ָ���ͨ������)
        break;
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//eof
