//----------------------------------------------------------------------------------------------------
// Header:      ͨ������λ���ӿڴ������
// File Name:   ChanEther.cpp
// Author:      David
// Date:        2012-11-12
//----------------------------------------------------------------------------------------------------
#define  _IN_CHANETHER_CPP
extern "C" 
{
    #include    "rtl.h"
    #include    "main.h"
    #include    "app_config.h"
    #include    "sys_config.h"
    #include    "spi.h"
//    #include    "led.h"
}
#include    "Dev.hpp"
#include    "parallelchan.hpp"  // 16-0122 ��̫���������ش���
#include    "chan.hpp"
#include    "chanrec.hpp"
#include    "chanether.hpp"
#include    "Subchan.hpp"
#include    "string.h"
#include    "app_can.hpp"
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//���¶���¼ָ���ݴ浽�����Ķ���¼ָ��
/*
    ��ע�� 0x81�������
*/
//----------------------------------------------------------------------------------------------------
void Chan::CoverReadPoint(void)
{
//    if (PdHoldData->NrRData.RdPtrTemp < (PdHoldData->NrRData.MaxSavePack - 1)) 
    {
        PdHoldData->NrRData.ReadPoint = PdHoldData->NrRData.RdPtrTemp;
    }
}
//----------------------------------------------------------------------------------------------------
//�ָ������Ķ���¼ָ�뵽����¼ָ���ݴ�
/*
    ��ע�� 0x81�������
*/
//----------------------------------------------------------------------------------------------------
void Chan::RecoverReadPoint(void)
{
//  if (PdHoldData->NrRData.ReadPoint < (PdHoldData->NrRData.MaxSavePack - 1)) 
    {
        PdHoldData->NrRData.RdPtrTemp = PdHoldData->NrRData.ReadPoint;
    }
}
//----------------------------------------------------------------------------------------------------
//  ��ȡͨ�����ϴ���¼������
/*
    ����  f_len ���ݹ̶����ֳ���
    ����  ��ǰ�����£����һ�����ã���¼������

    ��ע�� 0x81�������
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::GetLoadPackLen(U16 f_len) const
{
    U16 len;

    len = f_len
        + (cDev.mInfo.ti_num*2)
        + (cDev.mInfo.vi_num*2)
        +((cDev.mInfo.xitem_num&0xff)*4);

    return  (len);
//  return  (PdHoldData->NrRData.LoadPackLen);
}
//----------------------------------------------------------------------------------------------------
//  ��ȡδ�����ļ�¼����
/*
    ����  len : δ������¼����

    ��ע�� 0x81�������
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::GetUnReadRecNum(void) const
{
    U16 len = 0;

    len = (PdHoldData->NrRData.WritePoint >= PdHoldData->NrRData.ReadPoint)
        ? (PdHoldData->NrRData.WritePoint - PdHoldData->NrRData.ReadPoint)
        : (PdHoldData->NrRData.WritePoint + cDev.MaxSavePack - PdHoldData->NrRData.ReadPoint);

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  ��ȡ��ǰ�����ļ�¼����
/*
    ����   type : ��¼����

    ��ע�� 0x81�������
*/
//----------------------------------------------------------------------------------------------------
U8  Chan::GetReadRecType(void) const
{
    U8          type    = 0;
    strRecData  *rec_ptr;

    rec_ptr = (strRecData *)((U8 *)&PdHoldData->Record[0].RecMark + (cDev.SavePackLen * PdHoldData->NrRData.RdPtrTemp)); //15-1122    
    type    = ((rec_ptr->RecMark & 0x70) >> 4);

    return  (type);
}
//----------------------------------------------------------------------------------------------------
//  �й���ʱ���⿽��һ��ʵʱ����
/*  
    ����    *ptr :    ���������ݻ���
            abn:      ����Ĺ��ϴ���
    ����    len :     �������ݳ���

    ��ע��   ����ʵʱ���ݣ���SRAM���ϣ�
            0x81�������
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::SetDummyRealDate(U8 *ptr, U8 abn)
{
    U16 len = 0;//FIXED_DATA_LEN;  //����
    U32 i;

    //ʵʱ����ͨ���ż���ǡ�״̬
    *ptr++  = mAddr;
    *ptr++  = REALDATA_FLAG 
            |(NORMAL_REC << 4) 
            |(WORKSTAT_ABNOR);
    len += 2;
    //����ѭ����
    for (i=0; i<4; i++) {
        *ptr++ = 0;
        len++;
    }
    len += 4;
    //������
    *ptr++ = 0;
    *ptr++ = 0;
    len += 2;
    //�ۼƹ�����
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //�ݹ���ģʽ
    *ptr++ = MODE_INIT;
    len += 1;
    //�쳣����
    *ptr++ = abn;
    len += 1;
    //��������
    *ptr++ = 0;
    *ptr++ = 0;
    len += 2;
    //���̺���
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //��ʱ������
    *ptr++ = 0;
    *ptr++ = 0;
    len += 2;
    //��ʱ�����
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //��ѹ
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //����
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //����
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //����
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //�¶�
    for (i=0; i<4; i++) {
        *ptr++ = 0;
    }
    len += 4;
    //�����¶���
    *ptr++ = 0;
    *ptr++ = 0;
    len += 2;
    //�����ѹ��
    *ptr++ = 0;
    *ptr++ = 0;
    len += 2;
    //������
    *ptr++ = 0;
    len += 1;

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  ����һ��ʵʱ����
/*
    ����  *ptr :  ���������ݻ���
    ����  len :   �������ݳ���

    ��ע�� ����ʵʱ����
           0x81�������
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::SetRealDate(U8 *ptr)
{
    U16 len = 0;//FIXED_DATA_LEN;   // �䳤��������ݾ������ݸ���
    U16 xi  = 0, xinx;          // 16-0307
    U8  *p_buf;
    U8  static sRealTick = 0;   // 15-0923
    U32 cnt = 0,i,j,k;

    tsk_lock();

    *ptr++  = mAddr;                                // ͨ����    1
    *ptr++  = REALDATA_FLAG                         // ʵʱ��־  1
            |(NORMAL_REC<<4) 
            | Get_CurStatus();
    len += 2;
    p_buf = (U8 *)&PdHoldData->RunData.CycNum;      // ����ѭ���� 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    *ptr++ = PdHoldData->RunData.Step & 0xff;       // ������   2
    *ptr++ = PdHoldData->RunData.Step >> 8;
    len += 2;
    p_buf = (U8 *)&PdHoldData->RunData.TotStp;      // �ۼƹ����� 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    *ptr++ = PdHoldData->RunData.Mode & 0xff;       // �ݹ���ģʽ 1

    //�쳣�����������״̬���� 15-0129
    if (mAddr == 0) {   // 150923
        sRealTick++;
    }
    switch (PdHoldData->NrRData.CurWkStat)          // �쳣����  1
    {
    default:
    case WORKSTAT_INIT:     // �ϵ��ʼ��״̬
    case WORKSTAT_STOP:     // ֹͣ״̬
    case WORKSTAT_RUN:      // ����״̬
    case WORKSTAT_PAUSE:    // ��ͣ״̬
        *ptr++ = PdHoldData->RunData.AbnCode & 0xff;    // �����쳣����λ��
        break;

    case WORKSTAT_XPAUSE:   // �쳣��ͣ״̬
    case WORKSTAT_ABNOR:    // �쳣״̬
        if (sRealTick & 0x01) {
            *ptr++ = PdHoldData->RunData.AbnCode & 0xff;    // �����쳣����λ��
        } else {
            *ptr++ = PdHoldData->NrRData.AbnCodeBak & 0xff; // �����쳣ʱ�ı����쳣����λ��
        }
        break;
    }

    *ptr++ = PdHoldData->RunData.StpDay & 0xff;     // ��������  2
    *ptr++ = PdHoldData->RunData.StpDay >> 8;
    len += 4;
    p_buf = (U8 *)&PdHoldData->RunData.StpMs;       // ���̺���  4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    *ptr++ = PdHoldData->RunData.TotDay & 0xff;     // ��ʱ������ 2
    *ptr++ = PdHoldData->RunData.TotDay >> 8;
    len += 2;
    p_buf = (U8 *)&PdHoldData->RunData.TotMs;       // ��ʱ����� 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    p_buf = (U8 *)&PdHoldData->RunData.Vol;         // ��ѹ 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    p_buf = (U8 *)&PdHoldData->RunData.Cur;         // ���� 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    p_buf = (U8 *)&PdHoldData->RunData.Engy;        // ���� 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    p_buf = (U8 *)&PdHoldData->RunData.Cap;         // ���� 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;
    p_buf = (U8 *)&PdHoldData->RunData.Temp;        // �¶� 4
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    len += 4;

    // ���е����¶��� 16-0307   // cSubChan.Ti[MAX_TI_BOARD * MAX_BOARDTI_NUM];  // ӳ���ĵ����¶�
    xi  = cDev.mInfo.ti_num;
//    if (xi > cDev.mSampCfg.ch[mAddr].TiNum) {
//        xi = cDev.mSampCfg.ch[mAddr].TiNum;
//    }
    *ptr++ = xi & 0xff;
    *ptr++ = xi >> 8;
    len   += xi * 2 + 2;    // ���ȵ���
    xinx   = xi * mAddr;    // ������ʼ����
    for (i=xinx; i<(xi+xinx); i++) {
        if (i < MAX_TI_BOARD * MAX_BOARDTI_NUM) {
            *ptr++ = cSubChan.Ti[i] & 0xff;
            *ptr++ = cSubChan.Ti[i] >> 8;            
        } else {    // �����¶�����������Χ������ 0xffff
            *ptr++ = 0xff;
            *ptr++ = 0xff;
        }
    }
    // ���е����ѹ�� 16-0307   // cSubChan.Vi[MAX_VI_BOARD * MAX_BOARDVI_NUM];  // ӳ���ĵ����ѹ
    xi  = cDev.mInfo.vi_num;
    *ptr++ = xi & 0xff;
    *ptr++ = xi >> 8;
    len   += xi * 2 + 2;    // ���ȵ���
    xinx   = xi * mAddr;    // ������ʼ����
    for (i=xinx; i<(xi+xinx); i++) {
        if (i < MAX_VI_BOARD * MAX_BOARDVI_NUM) {
            *ptr++ = cSubChan.Vi[i] & 0xff;
            *ptr++ = cSubChan.Vi[i] >> 8;            
        } else {    // �����ѹ����������Χ������ 0xffff
            *ptr++ = 0xff;
            *ptr++ = 0xff;
        }
    }
    // ������������ 15-1112
    xi  = cDev.mInfo.xitem_num & 0xff;
    if (xi > MAX_TEST_XINUM) {
        xi = MAX_TEST_XINUM;
    }
    *ptr++ = xi;            // ���ֽ��ϴ�
    len   += xi * 4 + 1;    // ���ȵ���
    for (i=0; i<XITEMTBL_LEN; i++) {    // ������ѯ������
        if (cnt >= xi) {
            break;
        }
        j = i / 8;  // ��������ֽ�����
        k = i % 8;  // ��������ֽ�λ����
        if (cDev.mInfo.xitem_tbl[j] & B8_MASK_N[k]) {
            cnt++;
            SetXitem (ptr, i);  // ��������
            ptr += 4;           // 4�ֽ�����
        }
    }

    tsk_unlock();

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  �������������� 15-1205��16-0514
/*  ����    ptr : ���ݴ洢��ַ
            inx : ����������
*/
//----------------------------------------------------------------------------------------------------
void    Chan::SetXitem(U8 *ptr, U32 inx)
{
    S32 s32buf;
    S32 *s32ptr;

    // ��λ����չ���ݴ��������豸�ṹ���� 16-0514
    switch (cDev.mInfo.dev_str_type)
    {
        default:
        case DS_GENERAL:        // ͨ�������豸�ṹ
        case DS_NEILSBED:       // �봲�����豸�ṹ
        case DS_POURING:        // ���������豸�ṹ
        case DS_OPENSHELF:      // ���������豸�ṹ 
        {
            switch (inx)
            {
                default:
                    s32buf = 0;
                    break;
                case XITEM_POW:         // ����
                    s32ptr = (S32 *)&PdHoldData->NrRData.Pow; 
                    s32buf = *s32ptr;
                    break;
                case XITEM_RES:         // ����
                    s32ptr = (S32 *)&PdHoldData->NrRData.Res;
                    s32buf = *s32ptr;
                    break;
                case XITEM_CACC:        // �ۼ�����
                    s32ptr = (S32 *)&PdHoldData->NrRData.CapAcc;
                    s32buf = *s32ptr;
                    break;
                case XITEM_EACC:        // �ۼ�����
                    s32ptr = (S32 *)&PdHoldData->NrRData.EngyAcc;
                    s32buf = *s32ptr;
                    break;
                case XITEM_PILOT_VI:    // ��ʾ��ѹ
                    s32buf = 0;
                    break;
                case XITEM_MIN_VI:      // ��С�����ѹ
                    s32buf = PdHoldData->NrRData.MinVi.fdb;
                    break;
                case XITEM_MAX_VI:      // ������ѹ
                    s32buf = PdHoldData->NrRData.MaxVi.fdb;
                    break;
                case XITEM_PILOT_TI:    // ��ʾ�¶�
                    s32buf = 0;
                    break;
                case XITEM_MIN_TI:      // ��С�����¶�
                    s32buf = PdHoldData->NrRData.MinTi.fdb;
                    break;
                case XITEM_MAX_TI:      // ������¶�
                    s32buf = PdHoldData->NrRData.MaxTi.fdb;
                    break;
                case XITEM_VBUS:        // ĸ�ߵ�ѹ
                    s32buf = mCanExData.V2.Vbus;
                    break;
                case XITEM_VEX1:        // ��չ��ѹ1(��������ʾ��ѹ)
                    s32buf = mCanExData.V2.ExtVol1;
                    break;
                case XITEM_VEX2:        // ��չ��ѹ2(�����������ߵ�ѹ)
                    s32buf = mCanExData.V2.ExtVol2;
                    break;    
                case XITEM_VSLOPE:      // ��ѹб��
                case XITEM_SWIN:        // ����������
                case XITEM_SWOUT:       // ���������
                    s32buf = 0;
                    break;
                case XITEM_YEAR:        // ʵʱʱ��-��
                case XITEM_MON:         // ʵʱʱ��-��
                case XITEM_DAY:         // ʵʱʱ��-��
                case XITEM_HOUR:        // ʵʱʱ��-ʱ
                case XITEM_MIN:         // ʵʱʱ��-��
                case XITEM_SEC:         // ʵʱʱ��-��
                case XITEM_RECCNT:      // �洢��¼��
                case XITEM_LERR:        // ��λ������λ
                    s32buf = 0;
                    break;           
                case XITEM_TEX1:        // ��չ�¶�1(һ����ͨ������)
                    s32buf = mCanExData.V2.Temp1;
                    break;
                case XITEM_TEX2:        // ��չ�¶�2(һ����ͨ������)
                    s32buf = mCanExData.V2.Temp2;
                    break;
                case XITEM_I_INST:      // ����˲ʱֵ
                    s32buf = mCanExData.V2.InstCur;
                    break;
                case XITEM_V_INST:      // ��ѹ˲ʱֵ
                    s32buf = mCanExData.V2.InstVol;
                    break;
                case XITEM_CSTP:        // ���̣��׶Σ����� 16-0531
                    s32ptr = (S32 *)&PdHoldData->NrRData.CapStp; 
                    s32buf = *s32ptr;
                    break;
            }
            break;
        }                    
    }
    
    // 4�ֽڿ���
    *ptr++ = s32buf >> 0;
    *ptr++ = s32buf >> 8;
    *ptr++ = s32buf >> 16;
    *ptr++ = s32buf >> 24; 
}
//----------------------------------------------------------------------------------------------------
//  ����������¼ 15-1123 modified
/*
    ����    len : �������ݳ���

    ��ע��  �ӡ�ͨ���š��ֽڿ�ʼ����
            0x81�������
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::SetOneRec(U8 *ptr)
{
    U16         len;            // �ֽڳ��ȣ�������ݾ������ݸ���
    U8          *p_buf;
    U32         i;
    U32         vrec_mark;      // ������¼���
    strRecData  *rec_ptr;       // ָ�����¼ָ��ָ���ַ
    strVarRec   *var_rec_ptr;   // ָ�������¼���ݲ���

    tsk_lock();

    if (PdHoldData->NrRData.RdPtrTemp == PdHoldData->NrRData.WritePoint) {  //��¼��  13-0502
        PdHoldData->NrRData.FullFlag = FALSE;   // ���¼����������
        tsk_unlock();
        return  (0);
    }

    rec_ptr = (strRecData *)((U8 *)&PdHoldData->Record[0].RecMark + (cDev.SavePackLen * PdHoldData->NrRData.RdPtrTemp)); //15-1122

    *ptr++ = mAddr;             // ͨ���� 0
    *ptr++ = rec_ptr->RecMark;  // ��¼��־ 1

    vrec_mark   = rec_ptr->RData.Mark + (rec_ptr->RData.Step<<16);
    // �Ǳ�����¼
    if (vrec_mark != 0xffffffff) 
    {
        p_buf = (U8 *)&(rec_ptr->RData.CycNum); // ����ѭ���� 2
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        *ptr++ = rec_ptr->RData.Step & 0xff;    // ������ 6
        *ptr++ = rec_ptr->RData.Step >> 8;
        len += 2;
        
        p_buf = (U8 *)&rec_ptr->RData.TotStp;   // �ۼƹ����� 8
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        *ptr++ = rec_ptr->RData.Mode    & 0xff; // �ݹ���ģʽ 12
        *ptr++ = rec_ptr->RData.AbnCode & 0xff; // �쳣���� 13
        *ptr++ = rec_ptr->RData.StpDay  & 0xff; // �������� 14
        *ptr++ = rec_ptr->RData.StpDay  >> 8;
        
        p_buf = (U8 *)&rec_ptr->RData.StpMs;    // ���̺��� 16
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        *ptr++ = rec_ptr->RData.TotDay  & 0xff; // ��ʱ������ 20
        *ptr++ = rec_ptr->RData.TotDay  >> 8;
        
        p_buf = (U8 *)&rec_ptr->RData.TotMs;    // ��ʱ����� 22
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        p_buf = (U8 *)&rec_ptr->RData.Vol;      // ��ѹ 26
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        p_buf = (U8 *)&rec_ptr->RData.Cur;      // ���� 30
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        p_buf = (U8 *)&rec_ptr->RData.Engy;     // ���� 34
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        p_buf = (U8 *)&rec_ptr->RData.Cap;      // ���� 38
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        //-----ʵʱ�¶�-----
        p_buf = (U8 *)&rec_ptr->RData.Temp;     // �¶� 42
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
        
        len = 46;   // 15-1123  �̶�����һ���Դ���

        //-----�����¶�-----
        if (cDev.mInfo.ti_num > MAX_TEST_TINUM) {
            cDev.mInfo.ti_num = MAX_TEST_TINUM;
        }
        *ptr++ = cDev.mInfo.ti_num & 0xff;  // �����¶��� 42 /or 42+4 ��ͬ
        *ptr++ = cDev.mInfo.ti_num >> 8;
        len   += cDev.mInfo.ti_num * TI_SIZE + 2; // �����ȵ���
        p_buf  = (U8 *)rec_ptr + REC_LEN;   // ָ�������¶�����
        for (i=0; i<(cDev.mInfo.ti_num*TI_SIZE); i++) {
            *ptr++ = *p_buf++;
        }         
        //-----�����ѹ-----
        if (cDev.mInfo.vi_num > MAX_TEST_VINUM) {
            cDev.mInfo.vi_num = MAX_TEST_VINUM;
        }
        *ptr++ = cDev.mInfo.vi_num & 0xff;  // �����ѹ�� 44
        *ptr++ = cDev.mInfo.vi_num >> 8;        
        len   += cDev.mInfo.vi_num * VI_SIZE + 2; // �����ȵ���
        p_buf  = (U8 *)rec_ptr + REC_LEN + cDev.mInfo.ti_num*TI_SIZE;   // ָ�������ѹ����
        for (i=0; i<(cDev.mInfo.vi_num*VI_SIZE); i++) {
            *ptr++ = *p_buf++;
        } 
        //-----����----- 15-1112
        if (cDev.mInfo.xitem_num > MAX_TEST_XINUM) {
            cDev.mInfo.xitem_num = MAX_TEST_XINUM;
        }
        *ptr++ = cDev.mInfo.xitem_num;      // ����ʵʱ���� 46        
        len   += cDev.mInfo.xitem_num * XI_SIZE + 1; // ���ȵ���        
        p_buf  = (U8 *)rec_ptr + REC_LEN + cDev.mInfo.ti_num*TI_SIZE + cDev.mInfo.vi_num*VI_SIZE;   // ָ����������
        for (i=0; i<(cDev.mInfo.xitem_num*XI_SIZE); i++) {
            *ptr++ = *p_buf++;
        }                
    }
    //������¼ FIRST_VAR_REC,LAST_VAR_REC
    else 
    {
        var_rec_ptr = (strVarRec *)&rec_ptr->RData.Mark;
        
        for (i=0; i<4; i++) {   // ������¼��־��������0xffffffff��2
            *ptr++ = 0xff;
        }
//        len += 4;
        p_buf = (U8 *)&var_rec_ptr->Len;    // ���� 6 15-1123
        for (i=0; i<2; i++) {
            *ptr++ = p_buf[i];
        }  
        len = var_rec_ptr->Len;
//        len += 2;
        p_buf = (U8 *)&var_rec_ptr->V_Cyc1; // ѭ��1 6
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
//        len += 4;
        p_buf = (U8 *)&var_rec_ptr->V_Cyc2; // ѭ��2 10
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
//        len += 4;
        *ptr++ = var_rec_ptr->V_Day & 0xff; // ���� 14
        *ptr++ = var_rec_ptr->V_Day >> 8;
//        len += 2;
        p_buf = (U8 *)&var_rec_ptr->V_Ms;   // ������ 16
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
//        len += 4;
        p_buf = (U8 *)&var_rec_ptr->V_Cap;  // ���� 20
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
//        len += 4;
        p_buf = (U8 *)&var_rec_ptr->V_Cyc3; // ѭ��3 24
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
//        len += 4;
        p_buf = (U8 *)&var_rec_ptr->V_Cyc4; // ѭ��4 28
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
//        len += 4;
        p_buf = (U8 *)&var_rec_ptr->V_Cyc5; // ѭ��5 32
        for (i=0; i<4; i++) {
            *ptr++ = p_buf[i];
        }
//        len += 8;   //��Ҫͬ�Ǳ�����¼�ȳ������㳤��(����ʵʱ�¶�4�ֽ�) 36-
//                    //����δ���ǵ����ѹ�¶�����,����δ����
    }
    //���¼�¼�����ָ��
    //13-0502 �����ݴ�ָ�룬��PCȷ���յ��ٴζ���¼�Ǹ��������Ķ�ָ��
    if (PdHoldData->NrRData.RdPtrTemp < (cDev.MaxSavePack - 1)) {       
        PdHoldData->NrRData.RdPtrTemp++;
    } else {    // ��ת
        PdHoldData->NrRData.RdPtrTemp = 0;
    }   

    tsk_unlock();

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  �������̼�¼ �����̼�¼���ȹ̶���
/*
    ����    len :  �������ݳ���

    ��ע��  0x81�������
            �ӹ�������ͨ���ſ�ʼ���� 2013-0413
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::SetProcRec(U8 *ptr)
{
    U16         len = PROCDATA_LEN;
    U8          *p_buf;
    U32         i;
    strRecData  *rec_ptr;       //ָ�����¼ָ��ָ���ַ
    strProcRec  *proc_rec_ptr;  //ָ����̼�¼���ݲ���

    tsk_lock();

    if (PdHoldData->NrRData.RdPtrTemp == PdHoldData->NrRData.WritePoint) {  //��¼�� 13-0502
        PdHoldData->NrRData.FullFlag = FALSE;   //���¼����������
        tsk_unlock();
        return  (0);
    }

    rec_ptr = (strRecData *)((U8 *)&PdHoldData->Record[0].RecMark + (cDev.SavePackLen * PdHoldData->NrRData.RdPtrTemp));    //15-1122   
    proc_rec_ptr = (strProcRec *)&rec_ptr->RData.Mark;

    *ptr++  = mAddr;                            //ͨ���� 0
    *ptr++  = rec_ptr->RecMark;                 //��¼��־ 1
    p_buf   = (U8 *)&(proc_rec_ptr->P_CycNum);  //����ѭ���� 2
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    *ptr++  = proc_rec_ptr->P_Stp & 0xff;       //������ 6
    *ptr++  = proc_rec_ptr->P_Stp >> 8;
    p_buf   = (U8 *)&proc_rec_ptr->P_TotStp;    //�ۼƹ����� 8
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    *ptr++  = proc_rec_ptr->P_Mode   & 0xff;    //�ݹ���ģʽ 12
    *ptr++  = proc_rec_ptr->P_AbnCode& 0xff;    //�쳣���� 13
    *ptr++  = proc_rec_ptr->P_StpDay & 0xff;    //�������� 14
    *ptr++  = proc_rec_ptr->P_StpDay >> 8;
    p_buf   = (U8 *)&proc_rec_ptr->P_StpMs;     //���̺��� 16
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    *ptr++  = proc_rec_ptr->P_TotDay & 0xff;    //��ʱ������ 20
    *ptr++  = proc_rec_ptr->P_TotDay >> 8;
    p_buf   = (U8 *)&proc_rec_ptr->P_TotMs;     //��ʱ����� 22
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    p_buf   = (U8 *)&proc_rec_ptr->P_Engy;      //���� 26
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    p_buf   = (U8 *)&proc_rec_ptr->P_Cap;       //���� 30
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }
    p_buf   = (U8 *)&proc_rec_ptr->P_Res;       //��̬���� 34
    for (i=0; i<4; i++) {
        *ptr++ = p_buf[i];
    }

    //���¼�¼�����ָ��
    //13-0502 �����ݴ�ָ�룬��PCȷ���յ��ٴζ���¼�Ǹ��������Ķ�ָ��
    if (PdHoldData->NrRData.RdPtrTemp < (cDev.MaxSavePack - 1)) {
        PdHoldData->NrRData.RdPtrTemp++;
    } else {    //��ת
        PdHoldData->NrRData.RdPtrTemp = 0;
    }

    tsk_unlock();

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  �����޶���󳤶ȵ���������¼
/*
    ����  max_len :   �޶���󳤶�
    ����  ret_len :   �������ݳ���

    ��ע�� 0x81�������
*/
//----------------------------------------------------------------------------------------------------
//U16 Chan::SetRecData(U16 max_len)
//{
//    U16   ret_len = 0;





//    return  (ret_len);
//}

//----------------------------------------------------------------------------------------------------
//  PC��Ԥ ͨ������ 0x82
/*  ���� 
    *ptr    ���Ʋ�������ָ��
    ����    0 - �ɹ�  
            1 - ʧ�� 
            0xf1 - �·����·���� 16-0128
*/
//----------------------------------------------------------------------------------------------------
U8  Chan::ManualCtrl(U8 *ptr)
{
    U8  opt_code    = ptr[0];           // ��������
    U8  ret         = 0;

    if ((opt_code ==0) 
    ||  (opt_code > MAX_OPT_CODE)) {    // ����Ĳ�����
        return (1);
    }

    tsk_lock();

    switch (opt_code)
    {
    //---------- PC ֹͣ ----------
    case OPT_STOP:
        // �����
        PdHoldData->RunData.AbnCode     = 0;
        PdHoldData->NrRData.LowerStat   = 0;
        PdHoldData->NrRData.CabCtrlFlag = 0;
        switch (PdHoldData->NrRData.CurWkStat)
        {
        case WORKSTAT_RUN:      // ����״̬ ��> ֹͣ
            PdHoldData->NrRData.NxtWkStat   = WORKSTAT_STOP;
            break;
            
        case WORKSTAT_PAUSE:    // ��ͣ״̬ ��> ֹͣ
            PdHoldData->NrRData.NxtWkStat   = WORKSTAT_STOP;
            break;
            
        case WORKSTAT_XPAUSE:   // �쳣��ͣ״̬ ��> ֹͣ
        case WORKSTAT_ABNOR:    // �쳣״̬ ��> ֹͣ
            ret = 0xf1;     // ����λ����·���� 16-0128
        
            // ���ȴ�������λ����Ӧ��
            ;
            PdHoldData->NrRData.NxtWkStat = WORKSTAT_STOP;
            break;
        
        case WORKSTAT_STOP:     // ֹͣ״̬
        case WORKSTAT_INIT:     // �ϵ��ʼ��������PC����
        default:
            PdHoldData->NrRData.NxtWkStat = WORKSTAT_STOP;  // ����״̬Ҳֹͣ
            break;
        }
        break;
        
    //---------- PC ��ͣ ----------
    case OPT_PAUSE:
        switch (PdHoldData->NrRData.CurWkStat)
        {
        case WORKSTAT_RUN:      // ����״̬ ��> ��ͣ
            PdHoldData->NrRData.NxtWkStat   = WORKSTAT_PAUSE;
            break;
            
        case WORKSTAT_XPAUSE:   // �쳣��ͣ״̬

            break;
            
        case WORKSTAT_ABNOR:    // �쳣״̬ ��> ��ͣ

            break;
        
        case WORKSTAT_INIT:     // �ϵ��ʼ��������PC����
        case WORKSTAT_STOP:     // ֹͣ״̬
        case WORKSTAT_PAUSE:    // ��ͣ״̬
            
        default:
            break;
        }
        break;

    //---------- PC �ָ� ----------
    case OPT_RESUME:
        switch (PdHoldData->NrRData.CurWkStat)
        {
        case WORKSTAT_PAUSE:    // ��ͣ״̬ - > �ָ�
            PdHoldData->NrRData.NxtWkStat = WORKSTAT_RUN;
            break;
        
        case WORKSTAT_XPAUSE:   // �쳣��ͣ״̬ - > �ָ�
            switch (PdHoldData->RunData.AbnCode)    // ���ݹ��Ͼ����Ƿ�ָ�
            {
            default:
                PdHoldData->RunData.AbnCode     = ABNOR_NULL;   // �����
                PdHoldData->NrRData.NxtWkStat   = WORKSTAT_RUN; // ת����
                mLErrCtrl.DlyCnt = CTRL_1S; //CTRL_3S; // ���ϻָ�ʱ����λ�����ϱ�����ʱ 15-0618
                break;
            
            case    ABNOR_OV:           // ��ѹ
            case    ABNOR_UV:           // Ƿѹ
            case    ABNOR_OC:           // ����
            case    ABNOR_UC:           // Ƿ��
            case    ABNOR_OT:           // ����
            case    ABNOR_UT:           // ����
            case    ABNOR_COM_Vi:       // �����ѹͨ���쳣
            case    ABNOR_COM_Ti:       // �¶�ͨ���쳣
            case    ABNOR_POWEROFF:     // ����
            case    ABNOR_DATAOF:       // �������
            case    ABNOR_OP:           // ������
            case    ABNOR_NUM_Vi:       // �����ѹ����δ����
            case    ABNOR_TotalV:       // �����ѹ���쳣
            case    ABNOR_EEPROM:       // EEPROM�����쳣
            case    ABNOR_OVi:          // �����ѹ
            case    ABNOR_UVi:          // ����Ƿѹ
            case    ABNOR_OCAP:         // ������
            case    ABNOR_STOPDATA:     // ֹͣ������δ����
            case    ABNOR_IN_CALIB:     // У׼������
            case    ABNOR_LC:           // ©����
            case    ABNOR_POLAR:        // ����
            case    ABNOR_OUV:          // ��Ƿѹ
            case    ABNOR_CV:           // ����ѹ
            case    ABNOR_CC:           // ������
            case    ABNOR_CTRLDAT:      // �������ݴ���
                
            case    ABNOR_BUSOV:        // ĸ�߹�ѹ
            case    ABNOR_BUSUV:        // ĸ��Ƿѹ
            case    ABNOR_SENOT:        // ����������
            case    ABNOR_SENUT:        // ����������
            case    ABNOR_CCCVS:        // ��������ѹ��б��
            case    ABNOR_DCCVS:        // �����ŵ��ѹ��б��
            case    ABNOR_SYSBUSY:      // ϵͳæ
            case    ABNOR_DRVOC:        // ����������
            case    ABNOR_BUSFB:        // ĸ�ߵ���
            case    ABNOR_BUS:          // ĸ��У׼������
            case    ABNOR_CAP:          // ����У׼������
            case    ABNOR_BUSFREQ:      // ĸ��Ƶ������
            case    ABNOR_DFZ:          // �����߷���
            case    ABNOR_VBK:          // �����ߵ���
        // ��λ��δ������չ����

            case    ABNOR_BIT30:
            case    ABNOR_BIT31:
        //--------------------
            
            case    ERR_GETWKLINE:      // ��ȡ�����кŴ���
            case    ERR_PARVAR:         // �Ƿ����������
            case    ERR_OPT:            // �Ƿ�������
            case    ERR_NOLIMIT:        // ��������δ�趨
            case    ERR_STPPRG:         // �׶ι��ռ�����
            case    ERR_GBLPRG:         // ȫ�ֹ������ݴ���
            case    ERR_ELSENUM:        // ������
            case    ERR_MODE:           // δ֪����ģʽ
            case    ERR_CCMAX:          // ����������
            case    ERR_DCMAX:          // �����ŵ����
            case    ERR_CUMAX:          // ��������ѹ
            case    ERR_DUMIN:          // ����С�ŵ��ѹ
            case    ERR_CPMAX:          // ������繦��
            case    ERR_DPMAX:          // �����ŵ繦��
            case    ERR_STPINFINITE:    // �׶���ת��ѭ��
            case    ERR_MCANOFFLINE:    // ������CAN�ѻ�
            case    ERR_SCANOFFLINE:    // ����CAN�ѻ�
            case    EER_LOWER_NORMAL:   // ��λ����ͨ���ϣ���λ������ͣ��
            case    EER_LOWER_FATAL:    // ��λ����Ҫ���ϣ���λ����ͣ��������
            case    EER_SYNC:           // �׶�����ʱ����λ��ͬ��ʧ��
            case    ERR_WKSTAT:         // δ֪����̬����
            case    EER_SPACEOVF:       // �ռ����
            case    ERR_SRAM:           // SRAMӲ������
                PdHoldData->RunData.AbnCode     = ABNOR_NULL;       // �����
                PdHoldData->NrRData.NxtWkStat   = WORKSTAT_RUN;     // ת����
                mLErrCtrl.DlyCnt = CTRL_1S; // CTRL_3S;//���ϻָ�ʱ����λ�����ϱ�����ʱ 15-0618
                break;
            }
            break;

        case WORKSTAT_INIT:     // �ϵ��ʼ��������PC����
        case WORKSTAT_STOP:     // ֹͣ״̬
        case WORKSTAT_RUN:      // ����״̬
        case WORKSTAT_ABNOR:    // �쳣״̬ ���ûָ� //15-0324 �쳣���ûָ�
        default:
            break;
        }
        break;

    //---------- PC ���׶� ----------
    case OPT_JUMP:
        break;

    //---------- PC ����ִ�� ----------
    case OPT_SINGLESTEP:
        switch (PdHoldData->NrRData.CurWkStat)
        {
        case WORKSTAT_RUN:      // ����״̬ 
            break;            
        case WORKSTAT_PAUSE:    // ��ͣ״̬
            break;            
        case WORKSTAT_XPAUSE:   // �쳣��ͣ״̬
            break;            
        case WORKSTAT_ABNOR:    // �쳣״̬
            break;
        case WORKSTAT_INIT:     // �ϵ��ʼ��������PC����
        case WORKSTAT_STOP:     // ֹͣ״̬
        default:
            break;
        }
        break;
        
    //---------- ��ָʾ�� ----------
    case OPT_LED_FLASH:
        PdHoldData->NrRData.CabCtrlFlag = 1;
        break;

    //---------- ��ָʾ�� ----------
    case OPT_LED_OFF:
        PdHoldData->NrRData.CabCtrlFlag = 0;
        break;

    default:
        break;
    }

    tsk_unlock();

    return (ret);
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//  PC��Ԥ ��ͨ��� 0x90
/*  ���� 
    *ptr    �����������ָ�� (���ֽ�ΪУ׼����)
    ����    ���ݳ���
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::RdRate(U8 *ptr)   // ��ͨ���
{
    U8  len     = 0;
    U8  type    = ptr[0];

    type = type;

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  PC��Ԥ дͨ��� 0x91
/*  ���� 
    *ptr    У׼��������ָ�� (���ֽ�ΪУ׼����)
    ����    0 - �ɹ�  1 - ʧ��
*/
//----------------------------------------------------------------------------------------------------
U8 Chan::WrRate(U8 *ptr)    // дͨ���
{
    U8  ret     = 0;
    U8  type    = ptr[0];

    type = type;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  PC��Ԥ ��ͨ��У׼ 0x92
/*  ���� 
    *ptr    У׼��������ָ�� (���ֽ�ΪУ׼����)
    ����  ���ݳ���
*/
//----------------------------------------------------------------------------------------------------
U16 Chan::RdCalib(U8 *ptr)  // ��У׼����
{
    U8  len     = 0;
    U8  type    = ptr[0];

    type = type;

    return  (len);
}
//----------------------------------------------------------------------------------------------------
//  PC��Ԥ ͨ��У׼ 0x93
/*  ���� 
    *ptr    У׼��������ָ�� (���ֽ�ΪУ׼����)
    ����    0 - �ɹ�    1 - ʧ��
*/
//----------------------------------------------------------------------------------------------------
U8  Chan::WrCalib(U8 *ptr)
{
    U8  ret     = 0;
    U8  type    = ptr[0];

    type = type;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//eof
