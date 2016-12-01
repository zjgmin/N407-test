//----------------------------------------------------------------------------------------------------
#ifndef __app_can_Hpp
#define __app_can_Hpp
//----------------------------------------------------------------------------------------------------
#ifdef  IN_CAN_APP
    #define     ex_app_can
#else  
    #define     ex_app_can  extern 
#endif
//----------------------------------------------------------------------------------------------------
#include    "rtx_can.h"
//----------------------------------------------------------------------------------------------------
#define     BROADCAST_ADDR          (0x00)          // �㲥��ַ 15-1203
#define     CONNECT_VERSION         (0x01)          // ��������汾 16-0702    
//----------------------------------------------------------------------------------------------------
//�ϰ汾���Ի�CANͨ������궨��
//-----------------------------------
//������ͨ��CAN������У�
//-------------------------
#define     MCAN_RD_CONNECT         (0x3F)          // �������� 16-0702
    
#define     MCAN_RD_REALDATA1       (0x01)          // ��ʵʱ����1
#define     MCAN_WR_REALDATA1       (0x01|(1<<6))   // д��������1������ʵʱ����1

#define     MCAN_RD_REALDATA2       (0x02)          // ��ʵʱ����2
#define     MCAN_WR_REALDATA2       (0x02|(1<<6))   // д��������2������ʵʱ����2

#define     MCAN_RD_REALDATA3       (0x03)          // ��ʵʱ����3 15-0812
#define     MCAN_WR_REALDATA3       (0x03|(1<<6))   // д��������3������ʵʱ����3 15-0812

//#define     MCAN_RD_STEPSYNC1       (0x06)          //ͬ����ʵʱ����1
//#define     MCAN_WR_STEPSYNC1       (0x06|(1<<6))   //ͬ��д��������1������ʵʱ����1

//#define     MCAN_RD_STEPSYNC2       (0x07)          //ͬ����ʵʱ����2
//#define     MCAN_WR_STEPSYNC2       (0x07|(1<<6))   //ͬ��д��������2������ʵʱ����2

#define     MCAN_RD_IRATE           (0x10)          // �������
#define     MCAN_WR_IRATE           (0x10|(1<<6))   // д�����

#define     MCAN_RD_URATE           (0x11)          // �����ѹ
#define     MCAN_WR_URATE           (0x11|(1<<6))   // д���ѹ

#define     MCAN_RD_IPID            (0x12)          // ������PID
#define     MCAN_WR_IPID            (0x12|(1<<6))   // д����PID

#define     MCAN_RD_UPID            (0x13)          // ����ѹPID
#define     MCAN_WR_UPID            (0x13|(1<<6))   // д��ѹPID

#define     MCAN_RD_VBUSDATA        (0x14)          // ��ĸ������ 15-1223
#define     MCAN_WR_VBUSDATA        (0x14|(1<<6))   // дĸ������ 15-1223

#define     MCAN_RD_FREQDATA        (0x15)          // ��Ƶ������ 15-1223
#define     MCAN_WR_FREQDATA        (0x15|(1<<6))   // дƵ������ 15-1223

#define     MCAN_RD_REGMD           (0x16)          // ��������ģʽ
#define     MCAN_WR_REGMD           (0x16|(1<<6))   // д������ģʽ

#define     MCAN_CLRERRBIT          (0x17)          // �����  (2014-0705 ����)
#define     MCAN_CALIBSET           (0x17|(1<<6))   // У׼���ݱ���ָ�����

#define     MCAN_RESTART            (0x18)          // ����   (2015-0909 ����)


#define     MCAN_RD_TPARA           (0x19)          // ��ʱ����ֵ����
#define     MCAN_WR_TPARA           (0x19|(1<<6))   // дʱ����ֵ����

#define     MCAN_RD_SPARA           (0x1A)          // ��������ֵ����
#define     MCAN_WR_SPARA           (0x1A|(1<<6))   // д������ֵ����

#define     MCAN_RD_SYSBIT          (0x1B)          // ��ϵͳ����λ����������λ�����ܿ��ƿ��أ�
#define     MCAN_WR_SYSBIT          (0x1B|(1<<6))   // дϵͳ����λ����������λ�����ܿ��ƿ��أ�

#define     MCAN_RD_TPARA_EX        (0x1C)          // ����չʱ����ֵ����	(2015-0723 ����) 
#define     MCAN_WR_TPARA_EX        (0x1C|(1<<6))   // д��չʱ����ֵ����	(2015-0723 ����)

#define     MCAN_RD_SPARA_EX        (0x1D)          // ����չ������ֵ����	(2015-0723 ����)
#define     MCAN_WR_SPARA_EX        (0x1D|(1<<6))   // д��չ������ֵ����	(2015-0723 ����)

#define     MCAN_RD_DUTYDATA        (0x1E)          // ��ռ�ձ����� 15-1223
#define     MCAN_WR_DUTYDATA        (0x1E|(1<<6))   // дռ�ձ����� 15-1223

#define     MCAN_RD_CALIB           (0x20)          // ��У׼����
#define     MCAN_WR_CALIB           (0x20|(1<<6))   // дУ׼����

//----- flash ��д��� ----- 16-0729 added

#define     CMD_ONLINE_CHK          (0x21)          // ���߼�⣬���ذ汾��Ϣ
#define     CMD_SYSTEM_RST          (0x22)          // ϵͳ��λ
#define     CMD_ERASE_FLASH         (0x23)          // ���� Flash��ÿ�β���һ��������1K��
#define     CMD_SET_BAUDRATE        (0x24)          // ���ò�����
#define     CMD_UPDATE_PROGPAR      (0x25)          // ���³����������ʼ��ַ�����ȣ��̶�1024+2�ֽ�CRC��
#define     CMD_PROGRAM_FLASH       (0x26)          // Flash ��̣����һ֡��������ʼ��
#define     CMD_EXECUTE_APP         (0x27)          // ��ת��APPִ��

//--------------------------------------------------

#define     MCAN_RD_VER             (0x30)          // ������汾
#define     MCAN_WR_ADDR            (0x30|(1<<6))   // дCAN��ַ

#define     MCAN_PARAL_SYNC         (0x31|(1<<6))   // ����ͬ��

//���������
#define     MCAN_STEP_SYNC          (0x32)          // �׶�ͬ������

//-------------------------
//����ͨ��CAN����
//-------------------------
#define     SCAN_RD_REALDATA1       (0x01)          //��ʵʱ����1
#define     SCAN_WR_REALDATA1       (0x01|(1<<6))   //д��������1������ʵʱ����1

#define     SCAN_RD_REALDATA2       (0x02)          //��ʵʱ����2
#define     SCAN_WR_REALDATA2       (0x02|(1<<6))   //д��������2������ʵʱ����2

#define     SCAN_RD_REALDATA3       (0x03)          //��ʵʱ����3
#define     SCAN_WR_REALDATA3       (0x03|(1<<6))   //д��������3������ʵʱ����3

#define     SCAN_RD_RATE            (0x10)          //���
#define     SCAN_WR_RATE            (0x10|(1<<6))   //д�

#define     SCAN_RD_CALIB           (0x20)          //��У׼����
#define     SCAN_WR_CALIB           (0x20|(1<<6))   //дУ׼����

#define     SCAN_RD_EDATA           (0x30)          //����չ��������ַ���汾��AD���ƣ�
#define     SCAN_WR_EDATA           (0x30|(1<<6))   //д��չ��������ַ���汾��AD���ƣ�

#define     SCAN_RD_MAXI            (0x31)          //�������(��ѹ���¶ȵȣ� 15-1116
#define     SCAN_WR_LOCK            (0x31|(1<<6))   //��������(����ͨ��)
////-------------------------
////    ��չ����ͨ��CAN���� @���������� 15-1116
////-------------------------
//#define       EX_SCAN_RD_MAXVI    (0x18360103)    //��������ѹ
//#define       EX_SCAN_RD_MINVI    (0x18380103)    //����С�����ѹ
//#define       EX_SCAN_RD_MAXTI    (0x183A0103)    //��������¶�
//#define       EX_SCAN_RD_MINTI    (0x183C0103)    //����С�����¶�
//----------------------------------------------------------------------------------------------------
//  ��λ������״̬ mConnection����������� 16-0702
//----------------------------------------------------------------------------------------------------
#define     NO_CONNECT          (0x00)      // �ѻ�
#define     OK_CONNECT          (0x01)      // �����ɹ�
#define     OFF_CONNECT         (0xff)      // ������ʱ
//----------------------------------------------------------------------------------------------------
//  ��λ��ʵʱ���ݸ�ʽ�汾�� mDataFormat����������� 16-0702
//----------------------------------------------------------------------------------------------------
#define     FIXED_FORMAT        (0x00)      // �����ʽ ���� ��λ��mV��mA��mW��m��
#define     FIX10_FORMAT        (0x01)      // 10����ʽ ���� ��λ��0.1(mV��mA��mW��m��)
#define     FLOAT_FORMAT        (0x02)      // �����ʽ ���� ��λ��mV��mA��mW��m��
//----------------------------------------------------------------------------------------------------
//CAN ������
enum 
{
    BAUD_1M     = 1000000,
    BAUD_500K   = 500000,
    BAUD_250K   = 250000,
    BAUD_125K   = 125000,
    BAUD_100K   = 100000,   // 15-1111
};
//-------------------------
//CAN ͨ�Ŷ���
enum 
{
    CAN_NO1     = 0, 
    CAN_NO2     = 1
};
//-------------------------
//CAN ͨ�Ŷ���
enum 
{
    MAINCHAN    = 0, 
    SUBCHAN     = 1
};
//----------------------------------------------------------------------------------------------------
//ChanObj   //CAN����Ŀ��Ӳ��
#define     OBJ_MAINCHAN    0       // ��ͨ��
#define     OBJ_VICHAN      0       // �����ѹ
#define     OBJ_TICHAN      1       // �����¶�
#define     OBJ_IICHAN      2       // ������
#define     OBJ_OICHAN      3       // ������
#define     OBJ_XICHAN      4       // ����
//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
//----------------------------------------------------------------------------------------------------
class CANCtrl
{
private:
    U8      PhyNo;          // ����CAN��������
    U8      mChanInx;       // CAN�����շ�Ŀ��ͨ����������ַ��+1��
    U8      mChanObj;       // CAN�����շ�Ŀ��Ӳ��(�����ѹ�壬�¶Ȱ壬IO��)
//    U8      bCommErr;       // ͨ�Ź��ϱ�־
    U8      mWorkInfo;      // ��������0�����Ƶ�Ԫ 1���ɼ���Ԫ��
    U8      mCanFrameNo;    // CAN����֡���
    U8      mFrameIndex;
    U16     mBootHoldCnt;   // CAN��λ��BOOTLOARDER ����
//    U16     mCommTout;      // ͨ�ų�ʱ������	
    U32     mBaudRate;      // ������
    CAN_msg     mTxBuf;     // ���ͻ���
    
public:
    CANCtrl();

    U8      Get_WorkInfo(void);     // ��ȡCAN������Ϣ����������ɼ���Ԫ��
    void    Recv_In(CAN_msg *RxBuf);
    U32     Send_Out(void);	
    void    Recv_Process(CAN_msg *TxBuf);
    void    Send_Process(void);
    void    Hard_Init(void);
    U32     Comm_Test(S16 tick);
};

ex_app_can  CANCtrl     cCAN[CAN_CTRL_MAX_NUM];
//----------------------------------------------------------------------------------------------------
#endif  // #ifdef   __cplusplus
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus     // �ɹ�C����
    extern "C" 
    {
#endif
//----------------------------------------------------------------------------------------------------
//ex_app_can    U8          bHaveCmd[2];        // �в�������
//ex_app_can    U8          FailedNum[2];       // ����������Ӧ�����
//ex_app_can    CAN_msg     TxMsg[2];           // CAN������Ϣ����

//----------------------------------------------------------------------------------------------------
ex_app_can  void    CAN_Init(U8 mask);
ex_app_can  U32     CanNet_CmdSend(U32 canid, U32 *pmbox, U32 timeout);
ex_app_can  U32     CanNet_MsgSend(CAN_msg msg, U32 *pmbox, U32 timeout);
ex_app_can  void    CAN_Recv_Process(U32 can_no, CAN_msg *RxBuf);
ex_app_can  void    CAN_Send_Process(U32 can_no);
ex_app_can  U8      Get_CANWorkInfo(U8 can_no);

//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
    }
#endif
//----------------------------------------------------------------------------------------------------
#endif /* __app_can_Hpp */
