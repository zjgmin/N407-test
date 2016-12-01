//----------------------------------------------------------------------------------------------------
// Header:      ͨ��������������
// File Name:   ParalellChan.cpp
// Author:      David
// Date:        2013-05-16
//----------------------------------------------------------------------------------------------------
#define  _IN_PCHAN_CPP
extern "C" 
{
    #include    <string.h>
    #include    "RTL.h"
    #include    "main.h"
    #include    "app_config.h"
    #include    "sys_config.h"
    #include    "spi.h"
    #include    "led.h"
}
#include    "app_can.hpp"
#include    "Dev.hpp"
#include    "chan.hpp"
#include    "parallelchan.hpp"
//----------------------------------------------------------------------------------------------------
//���캯��
//----------------------------------------------------------------------------------------------------
ParallelChan::ParallelChan()
{
    U32 i;

    for (i=0; i<MAX_NODE_NUM; i++) {    // 16-0120
        mConnection[i]  = NO_CONNECT;   // δ����״̬ 16-0702
//        mConnectCnt[i]  = 0;            // �������
        mDataFormat[i]  = FIXED_FORMAT; //FLOAT_FORMAT; // Ĭ�϶������ݸ�ʽ 16-0702 test
        mCanOffCnt[i]   = 0;
    }

#ifdef  EN_PARALLEL     // ����Ĭ��ӳ���

    for (i=0; i<MAX_CHAN_NUM; i++) {
        mNodeTbl.lead_node[i] = i * DEF_PNODE_NUM;      // ����ͨ�����ڵ������������׽ڵ㣩
    }

    for (i=0; i<MAX_NODE_NUM; i++) {
        mNodeTbl.obj_pchan[i] = i / DEF_PNODE_NUM;      // �ڵ�ӳ��Ŀ�겢��ͨ����������������Ӧ�������ƣ�������
        if (mNodeTbl.obj_pchan[i] >= MAX_CHAN_NUM) {    // �ڵ�ӳ�䵽ͨ����ͨ��Խ�紦�� tbc...
            // ......
        }
    }
    mNodeTbl.node_num = MAX_CHAN_NUM * DEF_PNODE_NUM;   // ʵ��Ӧ�õĽڵ���

#else                   // �ǲ����Ľڵ��� == ͨ����
    
    for (i=0; i<MAX_CHAN_NUM; i++) {
        mNodeTbl.lead_node[i] = i;  // �ǲ�����һһӳ��
    }

    for (i=0; i<MAX_NODE_NUM; i++) {
        mNodeTbl.obj_pchan[i] = i;  // �ǲ�����һһӳ��
    }
    mNodeTbl.node_num = MAX_NODE_NUM;

#endif
}
//----------------------------------------------------------------------------------------------------
//  ��ʼ��ͨ�����ڵ㣨CAN��ַ-1��������
/*
    ����  ��
    ����  1���ɹ�  0��ʧ��
    ��ע  15-1212 ��·����Ĭ�ϣ������ã�
          16-0129 ʹ���豸��Ϣ�Ĳ�����ӳ��
*/
//----------------------------------------------------------------------------------------------------
U8  ParallelChan::InitNodeTbl(void)
{
    U32 i;
    U8  ret = 1;

    mNodeTbl.Mark  = NODETBL_MARK;

#ifdef  EN_PARALLEL     // ����ʱ��Ĭ�����ñ�

    if ((cDev.mInfo.chan_num > MAX_CHAN_NUM)
    ||  (cDev.mInfo.chan_num == 0)) {
        return  (0);    // ����ͨ���� ��
    }
    if ((cDev.mInfo.par_node_num > MAX_CHAN_NUM)
    ||  (cDev.mInfo.par_node_num == 0)) {
        return  (0);    // ���������������ڵ�����
    }
    if ((cDev.mInfo.chan_num*cDev.mInfo.par_node_num) > MAX_NODE_NUM) {
        return  (0);    // �ܽڵ��� ��
    }

    for (i=0; i<MAX_CHAN_NUM; i++) {
        mNodeTbl.lead_node[i] = i * cDev.mInfo.par_node_num;   // ����ͨ�����ڵ������������׽ڵ㣩
    }
    for (i=0; i<MAX_NODE_NUM; i++) {
        mNodeTbl.obj_pchan[i] = i / cDev.mInfo.par_node_num;   // �ڵ�ӳ��Ŀ�겢��ͨ����������������Ӧ�������ƣ�������
        if (mNodeTbl.obj_pchan[i] >= MAX_CHAN_NUM) {           // �ڵ�ӳ�䵽ͨ����ͨ��Խ�紦�� tbc...
            // ......
        }
    }
    mNodeTbl.node_num = cDev.mInfo.chan_num * cDev.mInfo.par_node_num;

#else                   // �ǲ����Ľڵ��� == ͨ����
    
    if ((cDev.mInfo.chan_num > MAX_CHAN_NUM)
    ||  (cDev.mInfo.chan_num == 0)) {
        return  (0);    // ����ͨ���� ��
    }
    if (cDev.mInfo.par_node_num != 0) {
        return  (0);    // ��������������������Ϊ0
    }
    if ((cDev.mInfo.chan_num*cDev.mInfo.par_node_num) > MAX_NODE_NUM) {
        return  (0);    // �ܽڵ��� ��
    }    
    for (i=0; i<MAX_CHAN_NUM; i++) {
        mNodeTbl.lead_node[i] = i;   // �ǲ���ͨ�����ڵ��������Լ���
    }    
    mNodeTbl.node_num = cDev.mInfo.chan_num;

#endif

    return (ret);
}
//----------------------------------------------------------------------------------------------------
//  ��ʼ������ͨ�����ڵ㣨CAN��ַ-1��������
/*
    ����      ��
    ����      ret:  1���ɹ�
                    0��ʧ��
    ��ע      16-0129 ����������ʹ�ô洢���ݣ������豸��Ϣ��ͨ�������ڵ���
            
*/
//----------------------------------------------------------------------------------------------------
U8  ParallelChan::LoadNodeTbl(void)
{


    U8  ret = 0;

    ret = InitNodeTbl(); // ��ʼ���������ұ�

    return  (ret);
}
//--------------------------------------------------
//  ����ͨ�����ڵ㣨CAN��ַ-1��������
/*
    ����  ��
    ����  ret:  1���ɹ�
                0��ʧ��
    ��ע��
*/
//--------------------------------------------------
U8  ParallelChan::SaveNodeTbl(U8 *ptr) // ��δʹ��
{
    U8  ret = 0x00;


    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//���ͨ����ڵ������ 15-1212
//  16-0129 ���Բ����������豸��Ϣ��
//----------------------------------------------------------------------------------------------------
U8  ParallelChan::CheckNodeTbl(strNodeTbl *sptr)
{
    U8  ret = 0;
    U32 i, j;
    
    j = cDev.mInfo.chan_num;
    for (i=0; i<j; i++) {
        if (sptr->lead_node[i] >= MAX_NODE_NUM) {       // ���ڵ㳬��
            return  (0x01);
        }
    }
    if (sptr->lead_node[j] > MAX_NODE_NUM) {            // ���ӽڵ㳬��
        return  (0x01);
    }
    j = cDev.mInfo.chan_num * cDev.mInfo.par_node_num;  // �ܽڵ���
    if ((sptr->node_num > MAX_NODE_NUM)
    ||  (sptr->node_num != j)) {                        // �ܽڵ�����
         return  (0x02);
    }
    for (i=0; i<j; i++) {
        if (sptr->obj_pchan[i] >= MAX_CHAN_NUM) {       // Ŀ�겢��ͨ������
            return  (0x04);
        }
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
// ��ȡ����״̬ 16-0714
/*
    ����:  node -�����ڵ� 
    ����:  0x00��δ����
           0x01������
*/
//----------------------------------------------------------------------------------------------------
U8  ParallelChan::GetConnectStat(U8 node)
{
    return  (mConnection[node]);    // ��ȡ����״̬
}
//----------------------------------------------------------------------------------------------------
//  CAN ���߻��ѻ�����
/*
    ����:  0x00������
           0x01���ѻ�
*/
//----------------------------------------------------------------------------------------------------
U8  ParallelChan::CanOffTest(U8 unit, S16 cnt)
{
    U8  ret = 0;

    if (cnt < 0) {
        mCanOffCnt[unit] = 0;
    } else {
        mCanOffCnt[unit] += cnt;
    }
    if (mCanOffCnt[unit] >= cDev.mCtrlCfg.CanOff_n) {   // �ѻ� 
        mCanOffCnt[unit] = cDev.mCtrlCfg.CanOff_n;
        ret = 1;
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  ������λ������������0
//----------------------------------------------------------------------------------------------------
void  ParallelChan::ClrCanData(U8 no)
{
    U8  *ptr;

    ptr = (U8 *)&mCanTxData[no];
    memset (ptr, 0, sizeof(mCanTxData[no]));
}
//----------------------------------------------------------------------------------------------------
//  ��̬������λ����������
//----------------------------------------------------------------------------------------------------
strCanCtrl  ParallelChan::AllocCanData(strCanCtrl ctrldata)
{
    strCanCtrl ret_data;

    return  (ret_data);
}
//----------------------------------------------------------------------------------------------------
//  ������λ���������� 16-0718
/*
  ����  unit    - ���ڲ�������ʾCAN��ַ��CAN��ַ��ͨ���Ų���ͬ��ͨ��������ҵ���������ͨ����
        *msg    - CAN���ͻ���ָ��
*/
//----------------------------------------------------------------------------------------------------
void  ParallelChan::CanConnectTxProc(U8 unit, CAN_msg *msg)
{    
    msg->id         = ((unit + 1) << 8)     // ��ַ
                    | (MCAN_RD_CONNECT);    // ��������
    msg->len        = 8;
    msg->ch         = 0;
    msg->format     = EXTENDED_FORMAT;
    msg->type       = DATA_FRAME;
    msg->data[0]    = 0;
    msg->data[1]    = 0;
    msg->data[2]    = 0;
    msg->data[3]    = 0;
    msg->data[4]    = 0;
    msg->data[5]    = 0;
    msg->data[6]    = 0;
    msg->data[7]    = 0;  

    if (CanOffTest(unit, 0x01) == 0x01) // ������ʱ
    {
    #ifdef  EN_SIM_CHAN     // ģ��CAN���ѻ����������ϣ��������ɹ�
        mConnection[unit] = OK_CONNECT;        
    #else
        
        
    #endif
        
    }

}
//----------------------------------------------------------------------------------------------------
//  ѭ��������λ�����������趨 15-0909
//  ��Ҫ���� 
/*
  ����  unit    - ���ڲ�������ʾCAN��ַ��CAN��ַ��ͨ���Ų���ͬ��ͨ��������ҵ���������ͨ����
                  �ǲ�������ʾʵ�ʵ�ͨ���ţ�CAN��ַ��ͨ����һһ��Ӧ
        f_no    - ʵʱ����֡��(16-0719 ����֡��Ϊ0����������)
        f_inx   - �ڶ�֡ʵʱ�������֡����
        *msg    - CAN���ͻ���ָ��
*/
//----------------------------------------------------------------------------------------------------
void  ParallelChan::CanTxProc(U8 unit, U8 f_no, U8 f_inx, CAN_msg *msg)
{
//----------------------------------------
#ifdef  EN_PARALLEL     // ��������,������λ����������

    static strCanCtrl data_buf;
    U8  pnode_num   = cDev.mInfo.par_node_num;      // ͨ�������ڵ���
    U8  pchan_no    = mNodeTbl.obj_pchan[unit];     // ��Ԫ���ڲ���ͨ���ţ��û����棩
    U8  u_master    = mNodeTbl.lead_node[pchan_no]; // ��ͨ������һͨ����

    if ((pchan_no   >= MAX_CHAN_NUM)
    ||  (pnode_num  == 0)
    ||  (pnode_num  >  MAX_NODE_NUM)
    ||  (unit       >= MAX_NODE_NUM)
    ||  (u_master   >= MAX_NODE_NUM)) {
        return;
    }

    // ��ȡ��������ͨ���Ŀ������ݣ��Ѿ�ӳ�䵽�ɰ����
    data_buf = cChan[pchan_no].GetCtrlData();
    // �����������
    switch (data_buf.Mode)
    {
        default:
        case STATICMODE:            // ����
        case EXCTRLMODE:            // ���ģʽ
        case ENDMODE:               // ����ģʽ
        {
            ClrCanData(unit);
            mCanTxData[unit].Step = data_buf.Step;  // 16-0510 �׶��·�            
            mCanTxData[unit].Mode = data_buf.Mode;  // 16-0510 ģʽ�·�
            break;
        }
        case HLCHARGEMODE:          // ������
        case HLDISCHARGEMODE:       // ������
        case HYCHARGEMODE:          // ��ѹ��
        case HYDISCHARGEMODE:       // ��ѹ��
        case XPCCHARGEMODE:         // б�µ�����
        case XPCDISCHARGEMODE:      // б�µ�����
        {
            //  (0) �򵥲���
            mCanTxData[unit]        = data_buf;
            mCanTxData[unit].Cur    /= pnode_num;
            break;
        }
        case HPCHARGEMODE:          // �㹦�ʳ�
        case HPDISCHARGEMODE:       // �㹦�ʷ�
        case XPPCHARGEMODE:         // б�¹��ʳ�
        case XPPDISCHARGEMODE:      // б�¹��ʷ�
        {
            mCanTxData[unit]        = data_buf;
            mCanTxData[unit].Cur    /= pnode_num;
            mCanTxData[unit].Ext1   /= pnode_num;
            break;
        }
        case HRCHARGEMODE:          // ������
        case HRDISCHARGEMODE:       // ������
        {
            mCanTxData[unit]        = data_buf;
            mCanTxData[unit].Cur    /= pnode_num;
            mCanTxData[unit].Ext1   *= pnode_num;
            break;
        }
        case XPVCHARGEMODE:         // б�µ�ѹ��
        case WKCHARGEMODE:          // �¶ȿ��Ƴ�
        case WKDISCHARGEMODE:       // �¶ȿ��Ʒ�
        case FCHARGEMODE:           // �����
        case FDISCHARGEMODE:        // ���ŵ�
        case PULSECHARGEMODE:       // ������
        case PULSECH_DCHMODE:       // �����ŵ� 
        {
            mCanTxData[unit]        = data_buf;
            mCanTxData[unit].Cur    /= pnode_num;
            break;
        }
    }
//----------------------------------------
#else   // �ǲ���CAN��������
    
    // �ǲ���ʱͨ���߽��ж�
    if (unit > MAX_CHAN_NUM) {  // ����CAN���ͨ������
        return;
    }
    // ���͵�һ֡����ǰ���·��ͻ��幤�� 15-0909
    if (f_no == 0x01) {
        mCanTxData[unit] = cChan[unit].GetCtrlData();
    }
    
#endif  

//----------------------------------------

// ��ͳCANID����ַ+����
    
        // �����0֡��������
    if (f_no == 0x00)   // 0x3F �������� 16-0719
    {
        msg->id         = ((unit + 1) << 8)     // ��ַ
                        | (MCAN_RD_CONNECT);    // ��������
        msg->len        = 8;
        msg->ch         = 0;
        msg->format     = EXTENDED_FORMAT;
        msg->type       = DATA_FRAME;
        msg->data[0]    = 0;
        msg->data[1]    = 0;
        msg->data[2]    = 0;
        msg->data[3]    = 0;
        msg->data[4]    = 0;
        msg->data[5]    = 0;
        msg->data[6]    = 0;
        msg->data[7]    = 0;      
    }
    else if (f_no == 0x01)   //0x41 �趨���Ƶ�����ѹ
    {
        msg->id         = MCAN_WR_REALDATA1 | (unit + 1)<<8;
        msg->data[0]    = mCanTxData[unit].Cur;
        msg->data[1]    = mCanTxData[unit].Cur>>8;
        msg->data[2]    = mCanTxData[unit].Cur>>16;
        msg->data[3]    = mCanTxData[unit].Cur>>24;
        msg->data[4]    = mCanTxData[unit].Vol;
        msg->data[5]    = mCanTxData[unit].Vol>>8;
        msg->data[6]    = mCanTxData[unit].Vol>>16;
        msg->data[7]    = mCanTxData[unit].Vol>>24;
        msg->len        = 8;
        msg->ch         = 0;
        msg->format     = EXTENDED_FORMAT;
        msg->type       = DATA_FRAME;
    }
    else    //if (f_no == 0x02)//0x42 �趨�׶η�ʽ,���ʵ���,��������
    {   //RDA - (�׶�+��ʽ+IO+��������)
        //RDB - (��������Ӧ������)
        //����  = 0 ��ͨ������ʽ
        //      = 1 �㹦��
        //      = 2 �����
        //      >=3 �·����� ����ʾ����
        
        msg->id = MCAN_WR_REALDATA2 | (unit + 1)<<8;
        

        {            // = 2 �͹��ʻ�����趨
                    U8  outbit;     // 16-0514  �ڶ�֡IO�·�
            static  U8  outsync;    // ͬ�����
            
            if (unit == 0) {
                outsync ^= 0x01;
            }
        #ifdef  EN_PARALLEL // ʹ�ܲ��� 16-0518
            outbit = cChan[pchan_no].GetOutBit(outsync);
        #else               // �ǲ���
            outbit = cChan[unit].GetOutBit(outsync);
        #endif
            
            msg->data[0]    = mCanTxData[unit].Step;    // �׶�
            msg->data[1]    = mCanTxData[unit].Mode;    // ��ʽ
            msg->data[2]    = 0x80|outbit;              // ͬ��λ  IO �������λ���� 16-0514
            if ((mCanTxData[unit].Mode == HPCHARGEMODE) 
            ||  (mCanTxData[unit].Mode == HPDISCHARGEMODE)) {   // �㹦��
                msg->data[3]    = 1;    // ���ݱ�־����
                msg->data[4]    = mCanTxData[unit].Ext1;
                msg->data[5]    = mCanTxData[unit].Ext1>>8;
                msg->data[6]    = mCanTxData[unit].Ext1>>16;
                msg->data[7]    = mCanTxData[unit].Ext1>>24;
            } else if ((mCanTxData[unit].Mode == HRCHARGEMODE) 
                    || (mCanTxData[unit].Mode == HRDISCHARGEMODE)) {    // �����
                msg->data[3]    = 2;    // ���ݱ�־����
                msg->data[4]    = mCanTxData[unit].Ext1;
                msg->data[5]    = mCanTxData[unit].Ext1>>8;
                msg->data[6]    = mCanTxData[unit].Ext1>>16;
                msg->data[7]    = mCanTxData[unit].Ext1>>24;
            } else {
                msg->data[3]    = 0;
                msg->data[4]    = 0;
                msg->data[5]    = 0;
                msg->data[6]    = 0;
                msg->data[7]    = 0;
            }
        }
        msg->len    = 8;
        msg->ch     = 0;
        msg->format = EXTENDED_FORMAT;
        msg->type   = DATA_FRAME;
    }

    if (CanOffTest(unit, 0x01) == 0x01) // �ѻ�
    {
        // �ѻ�������״̬ 16-0702
        mConnection[unit]  = NO_CONNECT;
        
//--------------------------------------------------
#ifdef  EN_SIM_CHAN     // �ѻ����������ϣ������Ƹ�ʵʱֵ,�Զ�ͬ��
    //----------------------------------------------

    #ifdef  EN_PARALLEL     // ģ����������--�ѻ�ʱ�������ݴ�������Ϊʵʱ����

        if (u_master == unit) { // 15-1214 ����ģ�ⷵ�����ݣ����տ���Ϊʵʱ����
            if (mCanTxData[unit].Mode & 0x0A) { //16-0520
                cChan[pchan_no].Set_RealCur(-data_buf.Cur); // ���յ����޷��ţ��ŵ���Ҫ�Ӹ���
            } else {
                cChan[pchan_no].Set_RealCur(data_buf.Cur);
            }
            cChan[pchan_no].Set_RealVol(data_buf.Vol);
            cChan[pchan_no].Set_RealPow();
            cChan[pchan_no].Set_RealRes();
            cChan[pchan_no].Clr_RealAbn(ERR_MCANOFFLINE);   // ģ��CAN���,���ѻ�����
        }
    //----------------------------------------------
    #else                   // �ǲ������ѻ�ģ�ⷵ��        
        // ����������Ϊʵʱ���أ�ģ��CAN���أ�- �ǲ��Ի����ŵ�Ӹ���
        if (mCanTxData[unit].Mode & 0x0A) {
            cChan[unit].Set_RealCur(-mCanTxData[unit].Cur);
        } else {
            cChan[unit].Set_RealCur(mCanTxData[unit].Cur);
        }
        if (mCanTxData[unit].Vol == 0) {    // ��ѹδ�趨
            cChan[unit].Set_RealVol(cChan[unit].GetSimSetVol());
        } else {
            cChan[unit].Set_RealVol(mCanTxData[unit].Vol);
        }
        cChan[unit].Set_RealPow();
        cChan[unit].Set_RealRes();
        cChan[unit].Clr_RealAbn(ERR_MCANOFFLINE);   // ģ��CAN���,���ѻ�����
                                                    // 15-0717
        if (cChan[unit].mSSCtrl.SyncFlag) {         // ��ͬ������
            os_sem_send(cChan[unit].mStpSyncSem);   // ģ��CAN���ͽ׶�ͬ���ź���
        }
    #endif
//--------------------------------------------------
#else   // ��ģ��������ݣ�����CAN�ѻ���ʵʱ���ݴ���
    //----------------------------------------------
    #ifdef  EN_PARALLEL     // ��������--�ѻ����� 15-1213

        U8  abn = cChan[pchan_no].Get_RealAbn();    // ��ǰ����ͨ������״̬
        float   f32tmp = 0; // ���ø��㴦��ʵʱ������ѹ 16-0708
        U32 i;
        
        // �ѻ������㵥Ԫʵʱ���ݣ�������
        mCanRxData[unit].Cur    = 0;
        mCanRxData[unit].Vol    = 0;
        
        // �ѻ������½ڵ���������ͨ��ʵʱ����
        for (i=0; i<pnode_num; i++) {
            switch (mDataFormat[i]) // ���������汾������������
            {
                default:
                case FIXED_FORMAT:  // Ĭ�ϸ�ʽ���㣨mA,mV��
                {
                    f32tmp += mCanRxData[u_master+i].Cur;
                    break;
                }
                case FIX10_FORMAT:  // (0.1mA,0.1mV)  
                {
                    f32tmp += mCanRxData[u_master+i].Cur/10;
                    break;                
                }
                case FLOAT_FORMAT:  // �����ʽ
                {
                    f32tmp += *(float *)&mCanRxData[u_master+i].Cur;
                    break;
                }                
            }
        }
        cChan[pchan_no].Set_RealCur(f32tmp);
        if (u_master == unit) {                     // �����ѻ������ѹ���ù���
            if ((abn == 0)
            ||  (abn == ERR_PCANOFFLINE)) {
                cChan[pchan_no].Set_RealAbn(ERR_MCANOFFLINE);   // �����ѻ�����
            }
            cChan[pchan_no].Set_RealVol(0);
        } else {
            if (abn == 0) {
                cChan[pchan_no].Set_RealAbn(ERR_PCANOFFLINE);   // �����ѻ�����
            }
        }
        cChan[pchan_no].Set_RealPow();
        cChan[pchan_no].Set_RealRes();
    //----------------------------------------------
    #else   // ��ֹ����--�ѻ�����
        if (cChan[unit].Get_RealAbn() == 0) {
            cChan[unit].Set_RealAbn(ERR_MCANOFFLINE);   // �ѻ�����
        }
        cChan[unit].Set_RealCur(0);
        cChan[unit].Set_RealVol(0);
        cChan[unit].Set_Stat(0);
        cChan[unit].Set_RealPow();
        cChan[unit].Set_RealRes();
    #endif
    //----------------------------------------------
#endif

    } 
}
//----------------------------------------------------------------------------------------------------
//  ��λ����ͨ���������ݽ���
//  ע���������������unitΪ����ͨ���ţ���CAN��ַ-1�������ֲ�����pchan_no�������ͨ���ţ������������ӻ�����unit��ͬ
//      û�в�����unit����ͨ����
//----------------------------------------------------------------------------------------------------
void ParallelChan::CanRxProc(U8 unit, U8 cmd, S32 *S32data)
{
    CAN_msg *ptrmsg;
    
#ifdef  EN_PARALLEL     // ���������CAN�������ݴ���
    
    U8  pnode_num   = cDev.mInfo.par_node_num;      // ͨ�������ڵ���
    U8  pchan_no    = mNodeTbl.obj_pchan[unit];     // �ڵ����ڲ���ͨ����
    U8  u_master    = mNodeTbl.lead_node[pchan_no]; // ͨ����������
    S32     S32tmp;
    float   f32tmp; // ʵʱ������ѹ���㴦�� 16-0708
    U32 i,j;

    if ((pchan_no   >= MAX_CHAN_NUM)
    ||  (pnode_num  == 0)
    ||  (pnode_num  >  MAX_NODE_NUM)
    ||  (u_master   >= MAX_NODE_NUM)
    ||  (unit       >= MAX_NODE_NUM)) {
        unit = unit;
        return;
    }
    if (CanOffTest(unit, -1) == 0) {    // ����
        if (u_master == unit) { // ��������
            if (cChan[pchan_no].Test_RealAbn() == ERR_MCANOFFLINE) {
                cChan[pchan_no].Set_RealAbn(0); // ����ȥ��CAN�ѻ�����
            }
            j = 0;
            for (i=u_master+1; i<(u_master+pnode_num); i++) {   // ���дӻ��ѻ��ж�
                if (mCanOffCnt[i] >= cDev.mCtrlCfg.CanOff_n) {
                    break;
                } else {
                    j++;
                }
            }
            if (j == (pnode_num-1)) {
                if (cChan[pchan_no].Test_RealAbn() == ERR_PCANOFFLINE) {
                    cChan[pchan_no].Set_RealAbn(0); // ����ȥ����CAN�ѻ����� 15-1214
                }
            }
        } 
    }

#else   // �ǲ��������CAN�������ݴ���+ͨ���߽��ж� 16-0122

    if (unit < MAX_CHAN_NUM) {  // �ǲ���ͨ���߽�����
        if (CanOffTest(unit, -1) == 0) {    // ����
            if (cChan[unit].Test_RealAbn() == ERR_MCANOFFLINE) {
                cChan[unit].Set_RealAbn(0); // ����ȥ��CAN�ѻ�����
            }
        }
    } else {    // ͨ��Խ��
        return;
    }

#endif

    switch(cmd)
    {
    //---------- ����Ϊ д��λ�� ���� ----------
    case (0x80|MCAN_WR_IRATE):      //0x50://���õ����ֵ
    case (0x80|MCAN_WR_URATE):      //0x51://���õ�ѹ�ֵ
    case (0x80|MCAN_WR_IPID):       //0x52://���õ���PID����
    case (0x80|MCAN_WR_UPID):       //0x53://���õ�ѹPID����
    case (0x80|MCAN_WR_VBUSDATA):   //0x54://����ĸ������ 15-1223
    case (0x80|MCAN_WR_FREQDATA):   //0x55://����Ƶ������ 15-1223
    case (0x80|MCAN_WR_REGMD):      //0x56://���ù�������͵�����ģʽ
    case (0x80|MCAN_CALIBSET):      //0x57://�����ָ��ͱ���
    case (0x80|0x40|0x18):          //0x58://���õ�����ƫ N/C
    case (0x80|MCAN_WR_TPARA):      //0x59://дʱ����ֵ���� T1-T8
    case (0x80|MCAN_WR_SPARA):      //0x5A://д������ֵ���� S1-S8
    case (0x80|MCAN_WR_SYSBIT):     //0x5B://��ϵͳ����λ
    case (0x80|MCAN_WR_TPARA_EX):   //0x5C://д��չʱ����ֵ���� T9-T16
    case (0x80|MCAN_WR_SPARA_EX):   //0x5D://д��չ������ֵ���� S9-S16
    case (0x80|MCAN_WR_DUTYDATA):   //0x5E://дռ�ձ����� 15-1223
    case (0x80|0x40|0x1F):          //0x5F://���ð汾�����ڵ� N/C
    case (0x80|MCAN_WR_CALIB):      //0x60://У������
    case (0x80|MCAN_WR_ADDR):       //0x70://���õ�ַ
        
        
    //---------- ����Ϊ ����λ�� ���� (�Ӷ����ʼ���з���) ----------
    case (0x80|MCAN_RD_IRATE):      //0x10://�������
    case (0x80|MCAN_RD_URATE):      //0x11://�����ѹ
    case (0x80|MCAN_RD_IPID):       //0x12://������PID
    case (0x80|MCAN_RD_UPID):       //0x13://����ѹPID
    case (0x80|MCAN_RD_VBUSDATA):   //0x14://��ĸ������ 15-1223
    case (0x80|MCAN_RD_FREQDATA):   //0x15://��Ƶ������ 15-1223
    case (0x80|MCAN_RD_REGMD):      //0x16://����������͵�����ģʽ
        
    case (0x80|MCAN_RD_TPARA):      //0x19://��ʱ����� T1-T8
    case (0x80|MCAN_RD_SPARA):      //0x1A://���������� S1-S8
    case (0x80|MCAN_RD_SYSBIT):     //0x1B://��ϵͳ����λ
    case (0x80|MCAN_RD_TPARA_EX):   //0x1C://����չʱ����� T9-T16
    case (0x80|MCAN_RD_SPARA_EX):   //0x1D://����չ�������� S9-S16
    case (0x80|MCAN_RD_DUTYDATA):   //0x1E://��ռ�ձ����� 15-1223
    case (0x80|0x1F):               //N/C
    case (0x80|MCAN_RD_CALIB):      //0x20://��ȡУ����������
        
    case (0x80|CMD_ONLINE_CHK):         // ���߼�⣬���ذ汾��Ϣ
    case (0x80|CMD_SYSTEM_RST):         // ϵͳ��λ
    case (0x80|CMD_ERASE_FLASH):        // ���� Flash��ÿ�β���һ��������1K��
    case (0x80|CMD_SET_BAUDRATE):       // ���ò�����
    case (0x80|CMD_UPDATE_PROGPAR):     // ���³����������ʼ��ַ�����ȣ��̶�1024+2�ֽ�CRC��
    case (0x80|CMD_PROGRAM_FLASH):      // Flash ��̣����һ֡��������ʼ��
    case (0x80|CMD_EXECUTE_APP):        // ��ת��APPִ��    
    
    case (0x80|MCAN_RD_VER):        //0x30://��ȡ�汾   
    
        // ����Ӧ����Ϣ����̫��
        if (os_mbx_check (CanNet_mpool) > 0) {
            ptrmsg = (CAN_msg *)_alloc_box (CanNet_mpool);
            if (ptrmsg) {
                ptrmsg->id  = cmd | ((unit + 1)<<8);        // ��ͳCANID
                ptrmsg->data[0] = S32data[0] & 0xff;
                ptrmsg->data[1] = S32data[0] >>  8;
                ptrmsg->data[2] = S32data[0] >> 16;
                ptrmsg->data[3] = S32data[0] >> 24;
                ptrmsg->data[4] = S32data[1] & 0xff;
                ptrmsg->data[5] = S32data[1] >> 8;
                ptrmsg->data[6] = S32data[1] >> 16;
                ptrmsg->data[7] = S32data[1] >> 24;
                if (os_mbx_send (MBX_MCan2Net, ptrmsg, 2) == OS_R_TMO) {
                    _free_box (CanNet_mpool, ptrmsg);
                } else {    // �ͷſ��ƣ��ȴ���̫���߳̽�������
                    ;
                }
            }
        }
        break;

    //---------- ����Ϊ ͸���������� ���� (��д������) ----------   16-0802
    case CMD_ONLINE_CHK:            //(0x21)    // ���߼�⣬���ذ汾��Ϣ
    case CMD_SYSTEM_RST:            //(0x22)    // ϵͳ��λ
    case CMD_ERASE_FLASH:           //(0x23)    // ���� Flash��ÿ�β���һ��������1K��
    case CMD_SET_BAUDRATE:          //(0x24)    // ���ò�����
    case CMD_UPDATE_PROGPAR:        //(0x25)    // ���³����������ʼ��ַ�����ȣ��̶�1024+2�ֽ�CRC��
    case CMD_PROGRAM_FLASH:         //(0x26)    // Flash ��̣����һ֡��������ʼ��
    case CMD_EXECUTE_APP:           //(0x27)    // ��ת��APPִ��    
    
        // ����Ӧ����Ϣ����̫��
        if (os_mbx_check (CanNet_mpool) > 0) {
            ptrmsg = (CAN_msg *)_alloc_box (CanNet_mpool);
            if (ptrmsg) {                 
                ptrmsg->id  = cmd | ((unit + 1)<<8);        // ��ͳCANID
                ptrmsg->data[0] = S32data[0] & 0xff;
                ptrmsg->data[1] = S32data[0] >>  8;
                ptrmsg->data[2] = S32data[0] >> 16;
                ptrmsg->data[3] = S32data[0] >> 24;
                ptrmsg->data[4] = S32data[1] & 0xff;
                ptrmsg->data[5] = S32data[1] >> 8;
                ptrmsg->data[6] = S32data[1] >> 16;
                ptrmsg->data[7] = S32data[1] >> 24;
                if (os_mbx_send (MBX_MCan2Net, ptrmsg, 2) == OS_R_TMO) {
                    _free_box (CanNet_mpool, ptrmsg);
                } else {    // �ͷſ��ƣ��ȴ���̫���߳̽�������
                    ;
                }
            }
        }
        break;        
        
    //---------- ���� ɨ������ ----------
    case (0x80|MCAN_RD_CONNECT):    //0x03: �������� 16-0702
        switch (S32data[0] & 0xff)  // ����Ӧ���ʵʱ���ݰ汾��ʽ
        {
            case FIXED_FORMAT:
            case FIX10_FORMAT:
            case FLOAT_FORMAT:
            {
                mConnection[unit] = OK_CONNECT;         // �����ɹ�
                mDataFormat[unit] = S32data[0] & 0xff;  // �������ݸ�ʽ�汾
                break;
            }
            default:    // ���ݸ�ʽ�汾���󣬱���
            {
                mConnection[unit] = NO_CONNECT; // ���ݸ�ʽ�汾�����������ɹ�
                break;
            }
        }        
        break;
    
    case (0x80|MCAN_RD_REALDATA1):  //0x01: ʵʱ����1
    case (0x80|MCAN_WR_REALDATA1):  //0x41:
        
#ifdef  EN_PARALLEL     // ����������ͨ��������ѹ���ݴ��� 15-1213

        mCanRxData[unit].Cur    = S32data[0];
        mCanRxData[unit].Vol    = S32data[1];
        // ����ͨ��ʵʱ����(���ﰴ���Բ���������15-1214)
        if (unit == u_master + pnode_num - 1) {     // ɨ�赽���һ���ӻ�����������1����
            f32tmp = 0;
            for (i=u_master; i<(u_master+pnode_num); i++) {
                switch (mDataFormat[i]) // ���������汾������������ 16-0708
                {
                    default:
                    case FIXED_FORMAT:  // Ĭ�ϸ�ʽ���㣨mA,mV��
                    {
                        f32tmp += mCanRxData[i].Cur;
                        break;
                    }
                    case FIX10_FORMAT:  // (0.1mA,0.1mV)  
                    {
                        f32tmp += mCanRxData[i].Cur/10;
                        break;                
                    }
                    case FLOAT_FORMAT:  // �����ʽ
                    {
                        f32tmp += *(float *)&mCanRxData[i].Cur;
                        break;
                    }
                }
            }
            cChan[pchan_no].Set_RealCur(f32tmp);
            switch (mDataFormat[unit])  // ���������汾����������ѹ 16-0708
            {
                default:
                case FIXED_FORMAT:  // Ĭ�ϸ�ʽ���㣨mA,mV��
                {
                    cChan[pchan_no].Set_RealVol(mCanRxData[u_master].Vol);      // ���������ѹ��ͨ��
                    break;
                }
                case FIX10_FORMAT:  // 0.1mA,0.1mVת����mA,mV��λ 
                {
                    cChan[pchan_no].Set_RealVol(mCanRxData[u_master].Vol/10);   // ���������ѹ��ͨ��
                    break;                
                }
                case FLOAT_FORMAT:  // �����ʽ
                {
                    cChan[pchan_no].Set_RealVol(*(float *)&mCanRxData[u_master].Vol);   // ���������ѹ��ͨ��
                    break;
                }  
            }
            cChan[pchan_no].Set_RealPow();
            cChan[pchan_no].Set_RealRes();
        }

#else   // �ǲ�����ͨ��������ѹ���մ���
        // ���������������������ѹ���� 16-0708
        switch(mDataFormat[unit])
        {
            default:
            case FIXED_FORMAT:  // Ĭ�ϸ�ʽ���㣨mA,mV��
            {
                cChan[unit].Set_RealCur(S32data[0]);
                cChan[unit].Set_RealVol(S32data[1]);
                break;
            }
            case FIX10_FORMAT:  // (0.1mA,0.1mV)  
            {
                cChan[unit].Set_RealCur(S32data[0]/10);
                cChan[unit].Set_RealVol(S32data[1]/10);
                break;                
            }
            case FLOAT_FORMAT:  // �����ʽ
            {
                cChan[unit].Set_RealCur(*(float *)S32data);
                cChan[unit].Set_RealVol(*(float *)(++S32data));
                break;
            }
        }
        cChan[unit].Set_RealPow();
        cChan[unit].Set_RealRes();

#endif
        break;

    case (0x80|MCAN_RD_REALDATA3):  //0x03: ʵʱ����3 15-1126
    case (0x80|MCAN_WR_REALDATA3):  //0x43:    
        cChan[unit].Set_InstCur(S32data[0]);
        cChan[unit].Set_InstVol(S32data[1]);
        break;
        
    case (0x80|MCAN_RD_REALDATA2):  //0x02: //ʵʱ����2
    case (0x80|MCAN_WR_REALDATA2):  //0x42:
        
#ifdef  EN_PARALLEL     // ��������ʱʵʱ����2����
    
        mCanRxData[unit].Stat   = S32data[0];
        // ����ͨ��ʵʱ����(���ﰴ���Բ���������15-1214)
        if (unit == u_master + pnode_num - 1) {         // ɨ�赽���һ���ӻ�����������2����
            S32tmp = 0;
            for (i=u_master; i<u_master + pnode_num; i++) { // ���ϻ�����ϴ�����ͨ��
                S32tmp |= mCanRxData[i].Stat;
                cChan[pchan_no].Set_Stat(S32tmp);
            }
        }
        if (u_master == unit) {     // ���������������ݵ�ͨ�� test
            cChan[pchan_no].Set_ExData(S32data[0], S32data[1]);
        }
//      tbc...

#else   // �ǲ�����ʵʱ����2���� 15-1214���� 
    
//        cChan[unit].Set_Stat(S32data[0]); 16-0414 ���� Set_ExData�д���
        cChan[unit].Set_ExData(S32data[0], S32data[1]);

#endif
        break;

    case (0x80|MCAN_CLRERRBIT): //0x17:����Ϸ��ز���Ӧ��PC
        //��������
        break;
    case (0x80|MCAN_RESTART):   //0x18:�������ز���Ӧ��PC
        //��������
        break;
//    case (0x80|0x03):   //0x83: //������ͣ��δ����
//        break;
//    case (0x80|0x04):   //0x84: //�����ָ���δ����
//        break;
//    case (0x80|0x05):   //0x85: //����������δ����
//        break;
    default:

        break;
    }
}
//----------------------------------------------------------------------------------------------------
//  ��ʼ��������Ϣ
//----------------------------------------------------------------------------------------------------
//-------------------------------------------------------
void Init_ParallelChan(void)
{
    cParallelChan.LoadNodeTbl();   //����ͨ��ӳ����ұ�
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
