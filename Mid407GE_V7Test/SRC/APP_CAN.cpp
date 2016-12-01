//----------------------------------------------------------------------------------------------------
// Header:      CANͨ��
// File Name:   APP_CAN.cpp
// Author:      David
// Date:        2012-10-18
//----------------------------------------------------------------------------------------------------
#define     IN_CAN_APP
extern "C" 
{
    #include    "stm32f4xx.h" 
    #include    "rtl.h"
    #include    "main.h"
    #include    "gpio.h"
    #include    "rtx_can.h"
    #include    "led.h"
    #include    "sys_config.h"
    #include    "app_config.h"
}
#include    "Dev.hpp"
#include    "chan.hpp"
#include    "parallelchan.hpp"
#include    "subchan.hpp"
#include    "app_can.hpp"
#include    "ethercmd.hpp"  // 16-0122
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
//���캯��
//----------------------------------------------------------------------------------------------------
CANCtrl::CANCtrl()
{
    static  U8  can_no = 0;

    switch(can_no)
    {
    case 0:
    //  bCommErr    = FALSE;        // ȥͨ�Ź���(���ճ�ʱ)
    //  mCommTout   = 0;            // ͨ�ų�ʱ������
        PhyNo       = can_no + 1;   // ��������
        mChanInx    = 0;            // CAN����Ŀ��ͨ����������ַ��+1��
        mChanObj    = OBJ_MAINCHAN; // CAN����Ŀ��Ӳ��
        mBaudRate   = BAUD_500K;    //BAUD_125K;    // Ĭ��500KHz������ 16-0304 125K�����ʲ���
        mWorkInfo   = MAINCHAN;     // Ĭ�Ͽ���ͨ��
        break;

    case 1:
    //  bCommErr    = FALSE;        // ȥͨ�Ź���(���ճ�ʱ)
    //  mCommTout   = 0;            // ͨ�ų�ʱ������
        PhyNo       = can_no + 1;   // ��������
        mChanInx    = 0;            // CAN����Ŀ��ͨ��
        mChanObj    = OBJ_VICHAN;   // CAN����Ŀ��Ӳ��
        mBaudRate   = BAUD_125K;    // Ĭ��125KHz������
        mWorkInfo   = SUBCHAN;      // Ĭ�ϸ���ͨ��
        break;

    default:break;
    }
    mBootHoldCnt = 0;
    can_no++;
}
//----------------------------------------------------------------------------------------------------
//  ��ȡCAN������Ϣ����������ɼ���Ԫ��
//----------------------------------------------------------------------------------------------------
U8  Get_CANWorkInfo(U8 can_no)
{
    return  (cCAN[can_no].Get_WorkInfo());
}
//----------------------------------------------------------------------------------------------------
//  ��ȡCAN������Ϣ����������ɼ���Ԫ��
//----------------------------------------------------------------------------------------------------
U8  CANCtrl::Get_WorkInfo(void)
{
    return  (mWorkInfo);
}
//----------------------------------------------------------------------------------------------------
//  ������λ����CAN���ݽ���
//----------------------------------------------------------------------------------------------------
void CANCtrl::Recv_In(CAN_msg *RxBuf)
{
    U8  can_adr, can_cmd;

    if (mWorkInfo == MAINCHAN) {

        can_adr = (RxBuf->id >> 8) & 0xff;
        can_cmd = (RxBuf->id >> 0) & 0xff;

        if ((can_adr != 0)
        &&  (can_adr <= cParallelChan.mNodeTbl.node_num)) { // ���սڵ����ɲ������� 16-0128 
            cParallelChan.CanRxProc((can_adr-1), can_cmd, (S32 *)&RxBuf->data[0]);
        } else {
            can_adr = can_adr;
        }
    } else {    // ����ͨ��ɨ��
        can_adr = (RxBuf->id >> 8) & 0xff;
        can_cmd = (RxBuf->id >> 0) & 0xff;
        //�����ѹ
        if ((can_adr > VIADDR_OFFSET)
        &&  (can_adr <= VIADDR_OFFSET+MAX_VI_BOARD)) {

        } 
        //�����¶�
        else if ((can_adr > TIADDR_OFFSET)
        && (can_adr <= TIADDR_OFFSET+MAX_TI_BOARD)) {

        }
        //����������
        else if ((can_adr > IIADDR_OFFSET)
        && (can_adr <= IIADDR_OFFSET+MAX_II_BOARD)) {

        }
        //���������
        else if ((can_adr > OIADDR_OFFSET)
        && (can_adr <= OIADDR_OFFSET+MAX_OI_BOARD)) {
            
        }


        //����
        else {
            ;
        }
    }
}
//----------------------------------------------------------------------------------------------------
//  CAN�����·�����λ��
//----------------------------------------------------------------------------------------------------
U32 CANCtrl::Send_Out(void)
{
    CAN_msg *ptrmsg;
//===================================================
//��������ͨ�����ƻ���λ������Ϣ������Ϣ�����ȴ�Ӧ��
//===================================================
    if (mBootHoldCnt>0) {
        mBootHoldCnt--;
    }
    if (mWorkInfo == MAINCHAN) {
        if (os_mbx_wait (MBX_Net2MCan, (void **)&ptrmsg, 0) == OS_R_OK) 
        {        
            //ȡ��CAN����ֱ���·�
            mTxBuf  = *ptrmsg;         
            
#ifdef  USE_SIM_CANCMD_ACK  // ģ��Ӧ��CAN����
            
            U8  cmd = (mTxBuf.id>>0) & 0xff;
            
            switch(cmd)
            {
                case (CMD_ONLINE_CHK):        // ���߼�⣬���ذ汾��Ϣ
                {
                    mBootHoldCnt = 200; //����
                }
                case (CMD_SYSTEM_RST):        // ϵͳ��λ
                {
                    mBootHoldCnt = 200; //����
                }
                case (CMD_ERASE_FLASH):       // ���� Flash��ÿ�β���һ��������1K��
                {
                    mBootHoldCnt = 200; //����
                }
                case (CMD_SET_BAUDRATE):      // ���ò�����
                {
                    mBootHoldCnt = 200; //����
                }
                case (CMD_UPDATE_PROGPAR):    // ���³����������ʼ��ַ�����ȣ��̶�1024+2�ֽ�CRC��
                {
                    mBootHoldCnt = 200; //����
                }
                case (CMD_PROGRAM_FLASH):     // Flash ��̣����һ֡��������ʼ��
                {
                    mBootHoldCnt = 200; //����
                }
                case (CMD_EXECUTE_APP):       // ��ת��APPִ��
                {   
                    mBootHoldCnt = 200; //����
                    // ����Ӧ����Ϣ����̫��
                    if (os_mbx_send (MBX_MCan2Net, ptrmsg, 2) == OS_R_TMO) {
                        _free_box (CanNet_mpool, ptrmsg);
                    } else {    // �ͷſ��ƣ��ȴ���̫���߳̽�������
                        ;
                    }                          
                    return 0;
                }
                
                default:
                {
                    //�ͷŴ洢��
                    _free_box (CanNet_mpool, ptrmsg); 
                    break;
                }
            }  
#else       //  ������λ��������������ռCAN��
            
            U8  cmd = (mTxBuf.id>>0) & 0xff;
            
            switch(cmd)
            {
                case (CMD_ONLINE_CHK):        // ���߼�⣬���ذ汾��Ϣ
                case (CMD_SYSTEM_RST):        // ϵͳ��λ
                case (CMD_ERASE_FLASH):       // ���� Flash��ÿ�β���һ��������1K��
                case (CMD_SET_BAUDRATE):      // ���ò�����
                case (CMD_UPDATE_PROGPAR):    // ���³����������ʼ��ַ�����ȣ��̶�1024+2�ֽ�CRC��
                case (CMD_PROGRAM_FLASH):     // Flash ��̣����һ֡��������ʼ��
                case (CMD_EXECUTE_APP):       // ��ת��APPִ��
                {   
                    mBootHoldCnt = 200; //����
                    break;
                }                
                default:
                { 
                    break;
                }
            }
            //�ͷŴ洢��
            _free_box (CanNet_mpool, ptrmsg);         
#endif            
        } 
        //û�У���ʱɨ����ͨ��
        else    // 15-0621 �������ʹ���
        {
            if (mBootHoldCnt) { // ֱ�ӷ��أ�����������ѯ����
                return 0;
            }
        //--------------------------------------------------     
        #if (defined(EN_CONNECT_CMD) && !defined(EN_SIM_CHAN))  // ʹ���������� 16-0719 16-0902�޸�
        //-------------------------------------------------- 
            // ������������ 16-0713
            if (cParallelChan.GetConnectStat(mChanInx) == 0) {  // ��δ�������������������ͳ��
                mCanFrameNo = 0;
                cParallelChan.CanTxProc(mChanInx, mCanFrameNo, mFrameIndex, &mTxBuf); // ������������
                if (mChanInx < (cParallelChan.mNodeTbl.node_num - 1)) {     // ������������ɨ��ڵ�
                    mChanInx++;
                } else {
                    mChanInx = 0;
                }
            } else {    // 16-0803 �޸�
                mCanFrameNo++;                
                if (mCanFrameNo <= 2) {  // (1)���ͣ�ʵʱ���Ƶ�������ѹ (2)���ͣ��׶Ρ�ģʽ��
                    cParallelChan.CanTxProc(mChanInx, mCanFrameNo, mFrameIndex, &mTxBuf);
                    if (mCanFrameNo == 2) { // �ڶ�֡��������л�ͨ��
                        mCanFrameNo = 0;
                        if (++mFrameIndex > 2) {
                            mFrameIndex = 1;
                        }
                        if (mChanInx < (cParallelChan.mNodeTbl.node_num - 1)) { // ɨ��ͨ�����ɲ������� 16-0128
                            mChanInx++;
                        } else {
                            mChanInx = 0;
                        }
                    }
                } else {                // ���磬ʵ���ܲ���
                    mCanFrameNo = 2;
                    if (++mFrameIndex > 2) {
                        mFrameIndex = 1;
                    }
                    cParallelChan.CanTxProc(mChanInx, mCanFrameNo, mFrameIndex, &mTxBuf);
                }
            }
        //--------------------------------------------------    
        #else   // ��֧��������ʵʱɨ�������
        //--------------------------------------------------     
            mCanFrameNo++;
            if (mCanFrameNo > 2) {  // �л�ͨ�������ͣ�ʵʱ���Ƶ�������ѹ
                mCanFrameNo = 1;
                if (mChanInx < (cParallelChan.mNodeTbl.node_num - 1)) { // ɨ��ͨ�����ɲ������� 16-0128
                    mChanInx++;
                } else {
                    mChanInx = 0;
                }
            } else {                // ���ͣ��׶Σ�ģʽ��
                mCanFrameNo = 2;
                if (++mFrameIndex > 2) {
                    mFrameIndex = 1;
                }
            }
            // �������ͨ������
            cParallelChan.CanTxProc(mChanInx, mCanFrameNo, mFrameIndex, &mTxBuf);
        //-------------------------------------------------- 
        #endif
        //--------------------------------------------------     

        }
    } 

//===================================================
//�����Ը���ͨ�����ƻ���λ������Ϣ������Ϣ�����ȴ�Ӧ��
//===================================================

    else {
        if (os_mbx_wait (MBX_Net2SCan, (void **)&ptrmsg, 0) == OS_R_OK) 
        {
            //ȡ��CAN����ֱ���·�
            mTxBuf  = *ptrmsg;
            //�ͷŴ洢��
            _free_box (CanNet_mpool, ptrmsg);
            //�б���ȴ���Ӧ����ȴ�
            
        }
        //û�У���ʱɨ�踨��ͨ��
        else
        {
 
              return 0;  

        }
    }

    return (CAN_send(PhyNo, &mTxBuf, 1));
}
//----------------------------------------------------------------------------------------------------
// CANx ��ʼ��
//----------------------------------------------------------------------------------------------------
void CANCtrl::Hard_Init(void)
{
    CAN_init (PhyNo, mBaudRate);

#ifdef  EN_CANRX_FILTER     // CAN�����˲�
    if (mWorkInfo == MAINCHAN) {    // ����CAN
        CAN_hw_rx_object (PhyNo, 0, EXID_CODE, DATA_TYPE | EXTENDED_TYPE);
    } else {                        // ����CAN
        CAN_hw_rx_object (PhyNo, 0, 0, DATA_TYPE | EXTENDED_TYPE);
    }
#else
    CAN_hw_rx_object (PhyNo, 0, 0, DATA_TYPE | EXTENDED_TYPE);
#endif

    CAN_start (PhyNo);
}
//----------------------------------------------------------------------------------------------------
/*  ��̫����CAN����� 16-0122 added

*/
//----------------------------------------------------------------------------------------------------
//  �����ݲ��ֵ�CAN��Ϣ����
//--------------------------------------------------
U32 CanNet_CmdSend(U32 canid, U32 *pmbox, U32 timeout)
{
    U32 err = 0;
    CAN_msg s_tmsg,
            *tmsg;

    s_tmsg.id       = canid;
    s_tmsg.len      = 8;
    s_tmsg.ch       = 0;
    s_tmsg.format   = EXTENDED_FORMAT;
    s_tmsg.type     = DATA_FRAME;
    s_tmsg.data[0]  = 0;
    s_tmsg.data[1]  = 0;
    s_tmsg.data[2]  = 0;
    s_tmsg.data[3]  = 0;
    s_tmsg.data[4]  = 0;
    s_tmsg.data[5]  = 0;
    s_tmsg.data[6]  = 0;
    s_tmsg.data[7]  = 0;

    if (os_mbx_check (CanNet_mpool) > 0) {
        tmsg = (CAN_msg *)_alloc_box (CanNet_mpool);
        if (tmsg) {
            *tmsg = s_tmsg;
            if (os_mbx_send (pmbox, tmsg, timeout) == OS_R_TMO) {  // ��������Ϣ��ʱ���ͷŴ洢��
                _free_box (CanNet_mpool, tmsg);
                err = RET_SEND_TOUT;
            }
        } else {
            err = RET_SYS_ERR;
        }
    } else {
        err = RET_SYS_ERR;
    }     
    
    return  (err);
}
//----------------------------------------------------------------------------------------------------
//  �����ݵ�CAN��Ϣ����
//--------------------------------------------------
U32 CanNet_MsgSend(CAN_msg msg, U32 *pmbox, U32 timeout)
{
    U32 err = 0;
    CAN_msg *tmsg;

    if (os_mbx_check (CanNet_mpool) > 0) {
        tmsg = (CAN_msg *)_alloc_box (CanNet_mpool);
        if (tmsg) {
            *tmsg = msg;
            if (os_mbx_send (pmbox, tmsg, timeout) == OS_R_TMO) {  // ��������Ϣ��ʱ���ͷŴ洢��
                _free_box (CanNet_mpool, tmsg);
                err = RET_SEND_TOUT;
            }
        } else {
            err = RET_SYS_ERR;
        }
    } else {
        err = RET_SYS_ERR;
    }     
    
    return  (err);
}
//----------------------------------------------------------------------------------------------------
//  CANx Ӳ����ʼ��
//----------------------------------------------------------------------------------------------------
void CAN_Init(U8 mask)
{
    U32 i;

    for (i=0; i<CAN_CTRL_MAX_NUM; i++) {
        if (mask & B8_MASK_N[i]) {
            cCAN[i].Hard_Init();
        }
    }
}
//----------------------------------------------------------------------------------------------------
//  CANx    ���մ������
/*
    ����    can_no:   CAN�������� 0��1��... ,CAN_CTRL_MAX_NUM-1
            *RxBuf:   CAN��������Ϣ����
    ����    ��
*/
//----------------------------------------------------------------------------------------------------
void CAN_Recv_Process(U32 can_no, CAN_msg *RxBuf)
{
    if (can_no >= CAN_CTRL_MAX_NUM) return; //�������ų��� 
    cCAN[can_no].Recv_In(RxBuf);
}
//----------------------------------------------------------------------------------------------------
//  CANx    ���ʹ������
/*
    ����  can_no: CAN�������� 0��1��... ,CAN_CTRL_MAX_NUM-1
    ����  ��
*/
//----------------------------------------------------------------------------------------------------
void CAN_Send_Process(U32 can_no)
{
    if (can_no >= CAN_CTRL_MAX_NUM) return; //�������ų���
    cCAN[can_no].Send_Out();
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//eof
