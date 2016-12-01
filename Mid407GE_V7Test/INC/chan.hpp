//----------------------------------------------------------------------------------------------------
#ifndef __CHAN_HPP
    #define __CHAN_HPP

#include    "main.h"
#include    "RTX_CAN.h"
//----------------------------------------------------------------------------------------------------
//C++�ඨ��
//-------------------------------------------------------
#ifdef      _IN_CHAN_CPP
    #define     chan_ext
#else
    #define     chan_ext    extern
#endif
//----------------------------------------------------------------------------------------------------
//  RUNDATA.VER
#define     RUNDATA_HMARK           (0x5A00)        // ���ֽڣ���ʼ�����
#define     RUNDATA_TOTSTEPMARK     (0x01<<7)       // ���ֽڣ����б�� - �ۼƹ�����ʼ�������

//----------
#define     MAX_ELSE                (4)             // ��������ġ����򡯹�����
//----------
#define     HAVE_NULL               (0x0000)        // ��û��ִ�г�Ź��գ����ѭ���ã�
#define     HAVE_CHARGED            (0x5555)        // �����Ź���Ϊ��磨���ѭ���ã�
#define     HAVE_DISCHARGED         (0xAAAA)        // �����Ź���Ϊ�ŵ磨���ѭ���ã�
//----------------------------------------------------------------------------------------------------
//--------------------------------------------------
//  �趨�뷴��
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
//  ʵʱ����
//----------------------------------------------------------------------------------------------------
//--------------------------------------------------
//  ������չ��λ���壨�ο��������������޸��豸��Ϣ�����
//--------------------------------------------------
#define     XITEM_POW           (0)     // ����
#define     XITEM_RES           (1)     // ����
#define     XITEM_CACC          (2)     // �ۼ�����
#define     XITEM_EACC          (3)     // �ۼ�����
#define     XITEM_PILOT_VI      (4)     // ��ʾ��ѹ
#define     XITEM_MIN_VI        (5)     // ��С�����ѹ
#define     XITEM_MAX_VI        (6)     // ������ѹ
#define     XITEM_PILOT_TI      (7)     // ��ʾ�¶�

#define     XITEM_MIN_TI        (8)     // ��С�����¶�
#define     XITEM_MAX_TI        (9)     // ������¶�
#define     XITEM_VBUS          (10)    // ĸ�ߵ�ѹ
#define     XITEM_VEX1          (11)    // ��չ��ѹ1(һ����ͨ������)
#define     XITEM_VEX2          (12)    // ��չ��ѹ2(һ����ͨ������)
#define     XITEM_VSLOPE        (13)    // ��ѹб��
#define     XITEM_SWIN          (14)    // ����������
#define     XITEM_SWOUT         (15)    // ���������

#define     XITEM_YEAR          (16)    // ʵʱʱ��-��
#define     XITEM_MON           (17)    // ʵʱʱ��-��
#define     XITEM_DAY           (18)    // ʵʱʱ��-��
#define     XITEM_HOUR          (19)    // ʵʱʱ��-ʱ
#define     XITEM_MIN           (20)    // ʵʱʱ��-��
#define     XITEM_SEC           (21)    // ʵʱʱ��-��
#define     XITEM_RECCNT        (22)    // �洢��¼��
#define     XITEM_LERR          (23)    // ��λ������λ
#define     XITEM_TEX1          (24)    // ��չ�¶�1(һ����ͨ������)
#define     XITEM_TEX2          (25)    // ��չ�¶�2(һ����ͨ������)
#define     XITEM_I_INST        (26)    // ����˲ʱֵ
#define     XITEM_V_INST        (27)    // ��ѹ˲ʱֵ
#define     XITEM_CSTP          (28)    // ���̣��׶Σ����� 16-0531

#define     XITEM_B29           (29)    // 
#define     XITEM_B30           (30)    // 
#define     XITEM_B31           (31)    // 
//------------------------------------------------------------------------------------------------
//--------------------------------------------------
//  CAN ʵʱ��չ�������� 15-1126
//--------------------------------------------------
#define     EX_INX_VBUS         (0)     // ʵʱĸ�ߵ�ѹ
#define     EX_INX_SYSTMP       (1)     // ϵͳ�¶�(�������¶ȵ�)
#define     EX_INX_REALT1       (2)     // ʵʱ�¶�1
#define     EX_INX_REALT2       (3)     // ʵʱ�¶�2
#define     EX_INX_FDBCUR       (4)     // ��������˲ʱֵ
#define     EX_INX_FDBVOL       (5)     // ������ѹ˲ʱֵ
#define     EX_INX_REFCUR       (6)     // �ο�����
#define     EX_INX_REFVOL       (7)     // �ο���ѹ
#define     EX_INX_VEXT1        (8)     // ʵʱ��չ��ѹ1
#define     EX_INX_VEXT2        (9)     // ʵʱ��չ��ѹ2
//--------------------------------------------------
#define     EXT_REALDATA_NUM    (EX_INX_VEXT2)      // �ϴ���չʵʱ���ݸ�����������ʾ/����/��ϵȣ�
//--------------------------------------------------
//  ��Ҫ��¼��ʵʱ����   15-1215(�Ѿ�ȥ�������ѹ���¶ȣ���������)
//--------------------------------------------------
typedef struct _RealData_ 
{                           //offset
    U16     Mark;           //00    ���
    U16     Step;           //02    ������(�׶�) 0x01��ʼ������'����'�к�Ϊ0x00�������ڹ����кţ�һ�㹤���к�>=������
    U16     Mode;           //04    ����ģʽ U8 -> U16
    U16     AbnCode;        //06    ������ͣ���� U8 -> U16
    
    U32     TotStp;         //08    ���ۼƹ���
    U32     CycNum;         //12    ���ѭ����������������10����
    
    U16     StpDay;         //16    ����ʱ������
    U16     TotDay;         //18    ��ʱ������
    
    U32     StpMs;          //20    ����ʱ�����
    U32     TotMs;          //24    ��ʱ�����

    float   Vol;            //28    ��ѹ(mV)
    float   Cur;            //32    ����(mA)
    float   Engy;           //36    ����(mWH)
    float   Cap;            //40    ����(mAH)
    float   Temp;           //44    �¶�(��)
    
    U16     Rev;            //48    ������ U8 -> U16
    
    U16     Crc;            //50    У��
    
}   RealData;
//--------------------------------------------------
//  ����Ҫ��¼��ʵʱ����
//--------------------------------------------------
typedef struct _NrRealData_ 
{
    U8          PreWkStat;          // ͨ��֮ǰ����״̬��
    U8          CurWkStat;          // ͨ����ǰ����״̬��
    U8          NxtWkStat;          // ͨ���²�����״̬��

    U8          ElseNum;            // '����'�ж�����

    U16         PreWkLine;          // ͨ��֮ǰ���й�����    0x00��ʼ��������������ELSE�е�����
    U16         CurWkLine;          // ͨ����ǰ���й�����    0x00��ʼ��������������ELSE�е�����
    U16         NxtWkLine;          // ͨ���²����й�����    0x00��ʼ��������������ELSE�е�����
    U16         JumpFlag;           // ������ת���
    
    U8          CabCtrlFlag;        // �������������ֽڣ�����ṹ��أ�
    U8          FullFlag;           // ��¼����������
    U16         ReadPoint;          // ����¼����ָ��
    U16         WritePoint;         // д��¼����ָ��
    U16         RdPtrTemp;          // ����¼����ָ���ݴ�(��Ϊ���¶���¼ָ����)

    U32         RecNum;             // ��¼��  -> ����������
    U16         bChDchMark;         // ��ű�� 0x55-��ǰ�г�磬0xAA-��ǰ�зŵ� 0-�޳��
    U16         PdAbnBak;           // ����ʱ���쳣���ݣ��˳�������ʱ������ 15-0404
    U32         LowerStat;          // ������λ������״̬
    U16         AbnCodeBak;         // ʵʱ���ݵĹ��ϱ��ݱ���

    U16         VT1_mS;             // ʱ�����1�������
    U32         V_Time1;            // ʱ�����1(100mS�ֱ���)
    float       V_Cap1;             // ��������1
    U32         V_Cyc[5];           // ѭ������
    float       Pow;                // ����
    float       Res;                // ����
    float       CapStp;             // �׶����� 16-0531    
    float       CapAcc;             // �ۼ����� 15-1114
    float       EngyAcc;            // �ۼ����� 15-1114
    
//    float           Temp;           // �¶�
    float       Slope;              // ��ѹб��
    float       MinusSlope;         // ��ѹ��б��
    
    float       MaxVolt;            // �����̵�ѹ 16-0716 �����жϳ���ѹ����
    float       MinVolt;            // ��С���̵�ѹ 16-0716 �����жϳ���ѹ����
    
    float       MaxIi;              // �������趨
    float       MinIi;              // ��С�����趨
    S32RefFdb   MaxVi;              // ������ѹ�趨�뷴��
    S32RefFdb   MinVi;              // ��С�����ѹ�趨�뷴��
    S32RefFdb   MaxTi;              // ������¶��趨�뷴��
    S32RefFdb   MinTi;              // ��С�����¶��趨�뷴��

}   NrRealData;
//----------------------------------------------------------------------------------------------------
//#define     REALDATA_LEN        (sizeof(RealData))  // ʵʱ���ݳ��� 52
#define     PROCDATA_LEN        (38)                // �������ݳ���
//----------------------------------------------------------------------------------------------------
//  ��¼(��ϸ)����
//----------------------------------------------------------------------------------------------------
typedef struct _RecData_ 
{
    U16         RecMark;    // ����ϸ�ı�־(b6:4)������״̬(b3:0)
    RealData    RData;      // �����¼����
    
}   strRecData;
//----------
#define     REC_LEN                 (sizeof(strRecData))    // ��¼���ֳ��� 56
#define     FIXED_DATA_LEN          (51)                    // �ϴ�ʵʱ����ϸ�̶����ֳ��� �ӡ�ͨ���š����������
#define     MAX_SENDPACK_LEN        (1300)                  // ����װ����
#define     MAX_SENDREC_NUM         (250)                   // ��󵥰���¼����

#define     VI_SIZE                 (2)                     // �����ѹ �����ֽڳ���
#define     TI_SIZE                 (2)                     // �����¶� �����ֽڳ���
#define     II_SIZE                 (4)                     // ���� �����ֽڳ���
#define     OI_SIZE                 (4)                     // ��� �����ֽڳ���
#define     XI_SIZE                 (4)                     // ������ �����ֽڳ���

#define     MAX_TEST_VINUM          MAX_CHANVI_NUM          // (240) �����Ե����ѹ��
#define     MAX_TEST_TINUM          MAX_CHANTI_NUM          // (120) �����Ե����¶���
#define     MAX_TEST_IINUM          MAX_CHANII_NUM          // (40)  ������������
#define     MAX_TEST_OINUM          MAX_CHANOI_NUM          // (40)  �����������
#define     MAX_TEST_XINUM          MAX_CHANXI_NUM          // (16)  �������������� 15-1112
//--------------------------------------------------
typedef struct _RecBackUp_ 
{
    U32     Mark;           // ���
    U16     RecCnt;         // ���������� (0����Ч)
    U16     Rev;            // ����
//    U8      Buf[0x100];     // ����
}   strRecBackUp;
//----------------------------------------------------------------------------------------------------
//  �����ṹ
//----------------------------------------------------------------------------------------------------
//  ��ͨ���� ������44�ֽڣ�
//-----------------------
typedef struct _strStpPrg_ 
{
    U16         No;             // �������
    U16         Mode;           // ����ģ
    U32         Mpar1;          // ������1 U16->U32 15-1209
    Union32     Mpar2;          // ������2 float->UF32
    U16         Pv_type1;       // �������������1
    U16         Pv_opt1;        // ���������������1
    Union32     Pv_val1;        // ���������ֵ1
    U16         Pv_type2;       // �������������2
    U16         Pv_opt2;        // ���������������2
    Union32     Pv_val2;        // ���������ֵ2
    U32         Delta_t;        // ʱ���
    float       Delta_u;        // ��ѹ��
    float       Delta_i;        // ������
    U16         Stp_act;        // �׶ν������� 0:��һ����0xffff:����, ������Ŀ��׶�
    U16         Rev16a;         // ����
    
} strStpPrg;
//-----------------------
//  AND���� ������44�ֽڣ�15-1125
//-----------------------
//  ����+AND
typedef struct _strPulseAndPrg_ 
{
    S32         Cur2;           // �ڶ�����
    S32         VolLmt;         // ���Ƶ�ѹ
    S32         Tidle1;         // ��Ъ1ʱ��
    S32         Tpw1;           // ��һ����
    S32         Tpw2;           // �ڶ�����
    S32         Tupc;           // ���������   
    S32         Tdnc;           // ����½���
    S32         Tidle2;         // ��Ъ1ʱ��
    
} strPulseAndPrg;
//--------------------------------------------------
//  ȫ�ֹ������ݽṹ
typedef struct _strGblPrg_ 
{                               //offset
    U16         Start_mode;     // ������ʽ
    U16         rev16a;         // ����
    U8          Pdwk_mode;      // ����ָ�ģʽ(���Զ�) 
    U8          DeProtect;      // �׹����Ƿ���Ա��� 
    U16         Tst_ViNum;      // ���Ե�����
    U16         Tst_TiNum;      // �����¶���
    U16         Tst_XiNum;      // ����������

    U32         Init_phase;     // ������ʱ����(ms)
    U32         Init_rec_t;     // ������ʱ���¼�����ms��
    U32         Prot_time;      // ������ʱʱ��(mS)

    Union32     V_pot1;         // �����ѹ1(mV)
    Union32     V_pot2;         // �����ѹ2(mV)
    Union32     V_pot3;         // �����ѹ3(mV)

    Union32     Ov_val;         // ��ѹ����ֵ
    Union32     Uv_val;         // Ƿѹ����ֵ
    Union32     Ot_val;         // ���±���ֵ
    Union32     Ut_val;         // ���±���ֵ
    Union32     Ccap_val;       // �����������ֵ
    Union32     Dcap_val;       // �ŵ���������ֵ
    Union32     Ovi_val;        // �����ѹ��ѹ����ֵ
    Union32     Uvi_val;        // �����ѹǷѹ����ֵ
    Union32     Oti_val;        // ���峬�±���ֵ
    Union32     Uti_val;        // ������±���ֵ

}   strGblPrg;
//--------------------------------------------------
//������������ ����
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
//  ҳ����Ϣ���ݽṹ
typedef struct _strPageLine_ 
{   
    U8          TotPage;        // ��ҳ��
    U8          PageNo;         // ҳ���
    U16         TotLine;        // ������
    U32         PageLine;       // ҳ����(��ǰ)

}   strPageLine;
//--------------------------------------------------
//���幤�սṹ
typedef struct _strPrg_ 
{
    strGblPrg       global;             // ȫ�ֱ���
    U32             TotLine;            // ����������    (��������)
    strStpPrg       step[MAX_WKLINE];   // ���幤������

}   strPrg;
//--------------------------------------------------
//������������ͷ�������׶ι��գ��ṹ
typedef struct _strStartHead_ 
{
    U8              ch[32];         // ͨ��λ���
    strConPrg       conti;          // ���Ӳ���
    strGblPrg       global;         // ȫ�ֱ���
    strPageLine     pageline;       // ҳ����Ϣ

}   strStartHead;
//--------------------------------------------------
//�������������ṹ
typedef struct _strStartCmd_ 
{
//    strStartHead    head;               // �������ݽṹͷ
    U8              ch[32];             // ͨ��λ���
    strConPrg       conti;              // ���Ӳ���
    strGblPrg       global;             // ȫ�ֱ���
    strPageLine     pageline;           // ҳ����Ϣ
    strStpPrg       step[MAX_WKLINE];   // ���幤������

}   strStartCmd;
//--------------------------------------------------
//  ���ս��տ������ݽṹ
//--------------------
//  ����״̬�� (RcvStat)
#define     RECV_IDLE           (0x00)          // ���տ��У�������չ���
#define     RECV_GOING          (0x01)          // ���ս�����
#define     RECV_END            (0x02)          // ���ս����ȴ�������������ص�����
//#define     RECV_WTSTART        (0x03)          // ���յȴ�����
//--------------------
//  ������ʽ (StartMode) ��(��Э��ӳ��)
#define     NORMAL_START        (0x0000)        // ��������
#define     CONTINUE_START      (0xffff)        // ��������
#define     REFRESH_START       (0x5a5a)        // ���÷���
//--------------------
//  ���ճ�ʱ
#define     PRG_RCV_TICK        (10*CTRL_1S)    // ���ս��ճ�ʱ
//--------------------
typedef struct _strPrgRcvCtrl_ 
{
    U8              RcvStat;        // ����״̬��־
    U8              Rev;            // ����
    U16             ToutCnt;        // ���ճ�ʱ������
    U16             CurPage;        // �Ѿ����յ�ҳ�����ɽ��ճ�����㣩
    U16             CurLine;        // ��ǰ�к�     ���ɽ��ճ������,0��ʼ��
    strStartCmd     DataBuf;        // �������ݻ���

}   strPrgRcvCtrl;
//--------------------------------------------------
//  ʱ��ṹ
typedef struct _strTime_ 
{   
    U16     Day;        // ����
    float   Ms;         // ������

}   strTime;
//--------------------------------------------------
//  ��¼�б����� 16-0421  modified
typedef struct _strRecCompData_
{
    U32         Ms;         // ��¼�������
    float       RecVol;     // ��ǰ��¼��ѹ
    float       RecCur;     // ��ǰ��¼����    
//    float       MaxVol;     // ��ǰ��¼ ����ѹ
//    float       MinVol;     // ��ǰ��¼ ��С��ѹ
//    float       MaxCur;     // ��ǰ��¼ ������
//    float       MinCur;     // ��ǰ��¼ ��С����

}   strRecCompData;
//--------------------------------------------------
//  ���������쳣�бȼ�����
//----------
#define     DELTA_I         (0.05)      // �����ж�Ĭ����ֵ +/-5%
#define     DELTA_V         (0.05)      // ��ѹ�ж�Ĭ����ֵ +/-5%
#define     DELTA_T         (0.05)      // �¶��ж�Ĭ����ֵ +/-5%
#define     DELTA_P         (0.05)      // �����ж�Ĭ����ֵ +/-5%
#define     DELTA_W         (0.05)      // �����ж�Ĭ����ֵ +/-5%
#define     DELTA_VI        (0.05)      // �����ѹ�ж�Ĭ����ֵ +/-5%
#define     DELTA_TI        (0.05)      // �����¶��ж�Ĭ����ֵ +/-5%
//----------
typedef struct _strJdgCnt_
{
    U16     ORateCur_n;     // ��������ж���Ӧ������
    U16     ORateVol_n;     // �������ѹ�ж���Ӧ������
    U16     URateVol_n;     // Ƿ��ŵ��ѹ�ж���Ӧ������
    //-----
    U16     OCur_n;         // �������ж���Ӧ������
    U16     UCur_n;         // Ƿ�����ж���Ӧ������ 

    U16     OVol_n;         // ����ѹ�ж���Ӧ������
    U16     UVol_n;         // Ƿ��ѹ�ж���Ӧ������

    U16     OTemp_n;        // ���¶��ж���Ӧ������
    U16     UTemp_n;        // Ƿ�¶��ж���Ӧ������

    U16     OVi_n;          // �������ѹ��Ӧ������
    U16     UVi_n;          // ����Ƿ��ѹ��Ӧ������

    U16     OTi_n;          // ������¶���Ӧ������
    U16     UTi_n;          // ����Ƿ�¶���Ӧ������
    //-----
    U16     IO_n;           // ��������Ӧ������
    //-----
    U16     Vpot1_n;        // ƽ̨��ѹ1��Ӧ������
    U16     Vpot2_n;        // ƽ̨��ѹ2��Ӧ������
    U16     Vpot3_n;        // ƽ̨��ѹ3��Ӧ������
    //-----
    U16     Cur_n;          // ����ת���ж���Ӧ������
    U16     Vol_n;          // ��ѹת���ж���Ӧ������
    U16     Temp_n;         // �¶�ת���ж���Ӧ������
    U16     Pow_n;          // ����ת���ж���Ӧ������
    U16     Engy_n;         // ��ʱ�ж���Ӧ������
    U16     MaxVi_n;        // �����ѹת����Ӧ������
    U16     MinVi_n;        // �����ѹת����Ӧ������
    U16     MaxTi_n;        // �����¶�ת����Ӧ������
    U16     MinTi_n;        // �����¶�ת����Ӧ������
//-----
    
}   strJdgCnt;
//----------------------------------------------------------------------------------------------------
//  ���籣�����ݽṹ
//--------------------------------------------------
typedef struct _strPdmem 
{
    U32         Dummy;                  // ��һ���ֲ��� 14-0707
    U32         Mark;                   // ������������� 
    RealData    RunData;                // ��¼ʵʱ
    NrRealData  NrRData;                // �Ǽ�¼ʵʱ
    
    //���������ṹ����������ʼ���������㣩
    strPrg      Program;                // ���幤��
    strGblPrg   RunGblPrg;              // ���籣�ֵĵ�ǰ����ȫ�ֱ������� 2012-0417
    strStpPrg   RunStpPrg;              // ���籣�ֵĵ�ǰ���н׶ι���
#ifdef  USE_SUBPRG  // ���н׶θ������ձ��� 16-0507
    strStpPrg   StpSubPrg[SUB_PRG_NUM]; // ��ǰ�׶κ������գ����У����֧��3�������ڡ�ELSE�����գ���δ�������壩
#endif    
    strRecData  Record[DEF_REC_NUM];    // ע ��������ܼ����ݶ���
    
}   strPdMem;
//--------------------------------------------------
//  ȥ����¼��ĵ������ݽṹ������������ݽṹͬ�����������ڼ����¼�ռ䣬����ʵ��ʹ�á���
//--------------------
typedef struct _strPdmemExRec_ 
{
    U32         dummy;                  // ��һ���ֲ���
    U32         mark;                   // ������������� 
    RealData    runData;                // ��¼ʵʱ
    NrRealData  nrRData;                // �Ǽ�¼ʵʱ
    
    strPrg      program;                // ���幤��
    strGblPrg   runGblPrg;              // ���籣�ֵĵ�ǰ����ȫ�ֱ�������
    strStpPrg   runStpPrg;              // ���籣�ֵĵ�ǰ���н׶ι���
#ifdef  USE_SUBPRG  // ���н׶θ������ձ��� 16-0507
    strStpPrg   stpSubPrg[SUB_PRG_NUM]; // ��ǰ�׶κ������գ����У����֧��3�������ڡ�ELSE�����գ���δ�������壩
#endif    
}   strPdmemExRec;
//--------------------------------------------------
//  ϵͳʹ�õ����ݽṹ�����ڼ����¼�ռ䣬����ʵ��ʹ�á���
//--------------------
typedef struct _strPdmemSys_ 
{
    strRecBackUp    recBackup;                          // ���籣��Ķ���¼����������
    U8              buffer[0x100-sizeof(strRecBackUp)]; // δ��0x100������
//    S16             vol[];            // �����ѹ������չ
//    S16             tmp[];            // �����ѹ������չ
}   strPdmemSys;

//----------------------------------------------------------------------------------------------------
#define     PDHDATA_MARK        (0xD1910600 + MAX_CHAN_NUM) // ��������ǣ����ڶ�ȡ�жϣ������ֵ���µ���������
//-------------------------------------------------------
//  �ⲿ��չRAM�����籣�� 2 MBytes
//-------------------------------------------------------
#define     LSB_ADDR            (0x60000000)                // �ⲿSRAM��ʼ�ֽڵ�ַ
#define     MSB_ADDR            (0x60200000)                // �ⲿSRAMĩ�ֽں��ַ

#define     REC_BACK_OFFSET     (LSB_ADDR)                  // ���PC����¼ʱ�ı��ݣ���PC��¼�ش���,��4�ֽڶ���
                                                            // *pRecBackUpָ����ָ
#define     RAM_CAP             (0x200000)                  // �ⲿSRAM�ռ� - 2 MBytes,��4�ֽڶ���
#define     SYS_USED_CAP        sizeof(strPdmemSys)         // ϵͳʹ���ⲿSRAM���� �����ع��ձ��ݣ�128K,��4�ֽڶ���
#define     UNREC_CAP           sizeof(strPdmemExRec)       // ͨ���Ǽ�¼�õ���������,��4�ֽڶ��� 
                                                            // PdHoldData -> record[x]ǰ�ĵ������������ڴ˴�С��Χ��
//-------------------------------------------------------
#if     (MAX_CHAN_NUM > 192)//128) 16-0518
    #error "First CAN should be less than 128 channels!"
#else
    #define     CH_PDH_CAP      ((RAM_CAP - SYS_USED_CAP) / MAX_CHAN_NUM)   // ÿ��ͨ��ʹ�õ��ܵ� RAM ��С��
    #define     RECORD_CAP      (CH_PDH_CAP - UNREC_CAP)                    // ÿ��ͨ��ʹ�õļ�¼ RAM ��С�� 
#endif
//----------------------------------------------------------------------------------------------------
//  �׶�����ͬ���������ݽṹ StepSyncCtrl
//--------------------------------------------------
typedef struct _strSSCtrl
{
    U8      SyncFlag;       // ͬ�����
    U8      CurDirect;      // ��������
    U16     TimeOut;        // ��ʱ������
//    float   CurBuf;         // ��������
    
}   strSSCtrl;
//----------------------------------------------------------------------------------------------------
//  WkStat  WkLine
#define     NXT_MARK                (0x00)          //�����ã���һ������ ���
#define     CUR_MARK                (0x01)          //�����ã���ǰ������ ���
#define     PRE_MARK                (0x02)          //�����ã�ǰһ������ ���

#define     PRG_RENEW_MARK          (0xafff)        // �����ù��ձ�ʶ
#define     PRG_FORCEJP_MARK        (0xbfff)        //���쳣����ͣ���쳣��ͣ��ǿ�����׶�
//--------------------
//  ��������� WkLine
//--------------------
//GetWkLine
#define     LINE_ERR                (0x5a5a)        // ȡ�������� 
//--------------------
#define     JP_STOP_LINE            (0xffff)        // ��ת���ս���
#define     JP_NEXT_LINE            (0x0000)        // ��ת����һ��
//--------------------
#define     LINE_FOR_START          (0xfff0)        // ����к�Ϊ������������
#define     LINE_FOR_CONTINUE       (0xfff2)        // ����к�Ϊ������������
#define     LINE_FOR_RENEW          (0xfff6)        // ����к�Ϊ���������ûָ���
#define     LINE_FOR_RESUME         (0xfff8)        // ����к�Ϊ�����ջָ���
//#define     LINE_FOR_PAUSE          (0xfffa)        // ����к�Ϊ��������ͣ��
#define     LINE_FOR_JPCURRENT      (0xfffc)        // ����к�Ϊ��PC��ת����ǰ�У�
#define     LINE_FOR_STOP           (0xfffe)        // ����к�Ϊ������״̬������
//#define     LINE_FOR_BACK           (0xff00)        //
//----------------------------------------------------------------------------------------------------
//  ���ն������
//--------------------
#define     ACTION_OFF              (0x00)          // ������
#define     ACTION_ON               (0x01)          // ����
#define     ACTION_NULL             (0x02)          // δ�趨����
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//  CAN������չ���ݽṹ 15-1126 modified
//  �ڶ�����չ���� 16-0415
//--------------------------------------------------
//typedef struct _strCanExData_ 
//{
//#if (DEV_STR_TYPE == DS_PBTESTER)   // Ǧ����λ��ר����չ���� 16-0414

//    U8      Step;               // ������λ���Ľ׶�
//    U8      Mode;               // ������λ���ķ�ʽ    
//    U16     ExtErr;             // ������λ����չ����
//    
//    S16     HexCurAvg;          // 0 -����ԭ��ƽ��ֵ
//    S16     HexVolAvg;          // 1 -��ѹԭ��ƽ��ֵ
//    S16     HexDA;              // 2 -���DA
//    S16     SetHexCur;          // 3 -�趨����ԭ��
//    S16     SetHexVol;          // 4 -�趨��ѹԭ��
//    U16     ReadIO;             // 5 -IO
//    U16     HardBit;            // 6 -Ӳ������λ
//    S16     RealT1;             // 7 -�¶�1
//    S16     RealT2;             // 8 -�¶�2
//    S16     RealR1;             // 9 -����1
//    S16     RealR2;             //10 -����2
//    S16     Refh;               //11 -�ο���
//    S16     Refl;               //12 -�ο���   
//    S16     T1Z;                //13 -�¶�1���
//    S16     T2Z;                //14 -�¶�2���
//    S16     T1F;                //15 -�¶�1����
//    S16     T2F;                //16 -�¶�1����
//    U16     SoftVer;            //17-��λ������汾
//    S16     IRes;               //18-����    
//    
//#else
//    U32     RealErr;            // ������λ���������� 15-0717

//    S16     Vbus;               // ĸ�ߵ�ѹ��һλС����
//    S16     SysTemp;            // ϵͳ�¶ȣ�һλС����
//    S16     Temp1;              // �����¶�1��һλС����
//    S16     Temp2;              // �����¶�2��һλС����
//    S16     FdbCur;             // ����������hex��
//    S16     FdbVol;             // ������ѹ��hex��
//    S16     RefCur;             // �ο�������hex��
//    S16     RefVol;             // �ο���ѹ��hex��
//    S16     ExtVol1;            // ��չ��ѹ1��һλС����
//    S16     ExtVol2;            // ��չ��ѹ2��һλС����    
//    S32     InstCur;            // ˲ʱ����
//    S32     InstVol;            // ˲ʱ��ѹ
//    
//#endif

//}   strCanExData;

typedef union   _strCanExData_ 
{
    struct 
    {
        U8      Step;               // ������λ���Ľ׶�
        U8      Mode;               // ������λ���ķ�ʽ    
        U16     ExtErr;             // ������λ����չ����
        
        S16     HexCurAvg;          // 0 -����ԭ��ƽ��ֵ
        S16     HexVolAvg;          // 1 -��ѹԭ��ƽ��ֵ
        S16     HexDA;              // 2 -���DA
        S16     SetHexCur;          // 3 -�趨����ԭ��
        S16     SetHexVol;          // 4 -�趨��ѹԭ��
        U16     ReadIO;             // 5 -IO
        U16     HardBit;            // 6 -Ӳ������λ
        S16     RealT1;             // 7 -�¶�1
        S16     RealT2;             // 8 -�¶�2
        S16     RealR1;             // 9 -����1
        S16     RealR2;             //10 -����2
        S16     Refh;               //11 -�ο���
        S16     Refl;               //12 -�ο���   
        S16     T1Z;                //13 -�¶�1���
        S16     T2Z;                //14 -�¶�2���
        S16     T1F;                //15 -�¶�1����
        S16     T2F;                //16 -�¶�1����
        U16     SoftVer;            //17-��λ������汾
        S16     IRes;               //18-����
        
    }   V1;     // ��һ�� Ǧ����Ի���λ����չ����
    
    struct
    {
        U32     RealErr;            // ������λ���������� 15-0717

        S16     Vbus;               // ĸ�ߵ�ѹ��һλС����
        S16     SysTemp;            // ϵͳ�¶ȣ�һλС����
        S16     Temp1;              // �����¶�1��һλС����
        S16     Temp2;              // �����¶�2��һλС����
        S16     FdbCur;             // ����������hex��
        S16     FdbVol;             // ������ѹ��hex��
        S16     RefCur;             // �ο�������hex��
        S16     RefVol;             // �ο���ѹ��hex��
        S16     ExtVol1;            // ��չ��ѹ1��һλС����
        S16     ExtVol2;            // ��չ��ѹ2��һλС����    
        S32     InstCur;            // ˲ʱ����
        S32     InstVol;            // ˲ʱ��ѹ
    
    }   V2;     // �ڶ��� ﮵�(����չ��˲ʱ)��3KGHϵ��

}   strCanExData;
//----------------------------------------------------------------------------------------------------
//  ��λ�����ϱ����������ݽṹ 15-1125
//--------------------------------------------------
typedef struct _strLErrCtrl_ 
{
    U8      DisInx;         // ��λ��������ʾ����
    U8      Rev0;
    U16     TstDly;         // ��λ��������Ӧ��ʾ����
    U16     ShiftCnt;      // ��λ��������ʾ�л������� 2014-0628
    U16     DlyCnt;        // ��λ��������ʱ������
    
}   strLErrCtrl;

//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
//----------------------------------------------------------------------------------------------------
//��ṹ����
//----------------------------------------------------------------------------------------------------
//������Ի�ͨ����
//----------------------------------------------------------------------------------------------------
class Chan
{
private:
    U8              mAddr;              // ���Ի��ӵ�ַ 0 ~  max-1
//    U8              mLErrDisInx;        // ��λ��������ʾ����
    U16             mRecFlag;           // ��¼Ҫ����
    U16             mFirstIn;           // ״̬�״ν�����
    U32             mStartRunDly;       // ������ʱ���� 15-1125
    U16             mStepAction;        // �׶ζ�������
    U16             mCanOffLineCnt;     // CAN��ʱ�ѻ�������

//    U16             mLowerErrCnt;       // ��λ��������Ӧ������
//    U16             mLErrShiftCnt;      // ��λ��������ʾ�л������� 2014-0628
//    U16             mLErrDlyCnt;        // ��λ��������ʱ������
    U32             mCntDnTick;         // ����ʱ������
    U32             mUnitCap;           // ��λ�������棨mAH��15-0815
    U32             mUnitEngy;          // ��λ�������棨mWH��15-1116
    strLErrCtrl     mLErrCtrl;          // ��λ�����ϴ������

public:
    U8              mSubOffFlag;        // ����ͨ���ѻ���ǣ���Ϲ��մ����ѻ������� 16-0311
    OS_SEM          mStpSyncSem;        // ��λ���׶����׶κ�ͬ���ź� 15-0717
//  OS_SEM          mDisProtSem;        // ��λ�����α����ź� 15-1109

//#if (DEV_STR_TYPE == DS_NEILSBED)   // �봲�豸ר���ź���    15-0603

    // ����ʲô�豸�ṹ���������봲�ź��� 16-0514    --> ��һ��������ȡ
    OS_SEM          mSmokeSem;          // �̸б��� �ź��� (������������������)
    OS_SEM          mInplaceSem;        // ���̵�λ �ź��� (������������������)
    OS_SEM          mRqInplaceSem;      // �������̵�λ �ź���   (������������������)

//#endif

    strCanCtrl      mCanCtrlData;       // CAN�������ݽṹ
    strSSCtrl       mSSCtrl;            // �׶����ͬ�����ƽṹ
    strCanExData    mCanExData;         // ������λ������չ����

//  strGblPrg       mGblPrgBak;         // ȫ�ֱ������ձ���
//  strStpPrg       mStpPrgBak;         // �׶ι��ձ���

    strRecData      mRecDateBuf;        // ��¼���ݻ���
    strRecCompData  mRecCompData;       // ��¼�Ƚ�����
    strJdgCnt       mJdgCnt;            // �����Ƚ�����

    
    strPdMem        *PdHoldData;
public:
    Chan();
    inline  U8  ChanBreak(void);            // ͨ���ϵ����
    U8          Test_PdMark(void);          // ����������ж�
    void        Init_PdMark();              // ��������ǳ�ʼ��
    U8          Test_PdData(void);          // �����������ж�
    void        Init_PdData(void);          // ���������ݳ�ʼ��
    U8          Test_WkStat(U8 mark);       // �Ƿ�������̬
    void        Clear_Program(void);        // ���㹤�ջ���
    void        Init_WkData(void);          // ���п��Ƴ����ϵ��ʼ��
    U8          Test_PdResumeMode(void);    // �����Զ��ָ�ģʽ�ж�
    void        Set_PdWkState(void);        // ���е��磬�ϵ����״̬�趨
    void        Init_PdCntDn(void);         // ����̬����ʱ��ʼ��
    void        SetTiNum(U8 num);           // �����¶���

    //----- Ether -----
    U8          ManualCtrl(U8 *ptr);                // PC��Ԥ����
    U8          Save_Program(void);                 // ���湤��
    void        Save_RenewStp(U16 obj_stp);         // �������ú���תĿ��׶�
    U8          PrgRcvStartCtrl(U16 start_mode);    // ���ս�����������

    void        CoverReadPoint(void);               // ���Ƕ���¼ָ���ݴ浽�����Ķ���¼ָ��
    void        RecoverReadPoint(void);             // �ָ������Ķ���¼ָ�뵽����¼ָ���ݴ�
    U16         GetLoadPackLen(U16 f_len) const;    // ��ȡ��ǰ�ϴ���¼����
    U16         GetUnReadRecNum(void) const;        // ��ȡ��λ��δ����¼����
    U8          GetReadRecType(void) const;         // ��ȡ��ǰ�����ļ�¼����
    U16         SetDummyRealDate(U8 *ptr, U8 abn);  // �й���ʱ�����⿽��һ�����ݸ�PC
    U16         SetRealDate(U8 *ptr);               // ����ʵʱ����
    void        SetXitem(U8 *ptr, U32 inx);         // �������������� 15-1116
    U16         SetOneRec(U8 *ptr);                 // ����������¼
    U16         SetProcRec(U8 *ptr);                // �������̼�¼
//  U16         SetRecData(U16 max_len);            // ����(�޶��ֽڳ���)��¼

    U16         RdRate(U8 *ptr);                    // ��ͨ���
    U8          WrRate(U8 *ptr);                    // дͨ���
    U16         RdCalib(U8 *ptr);                   // ��У׼����
    U8          WrCalib(U8 *ptr);                   // дУ׼����

    //----- CAN -----
    U8          CanOffLineTest(S32 cnt);            // CAN�ѻ��ж�
    void        ClrCanCtrlData(U8 *);               // ����λ����������
    void        ClrCanExData(void);                 // ����λ����չ����
    void        CanRecvProc(U8 cmd, S32 *data);     // ��λ���������ݽ��ս���
    void        CanSendProc(U8, U8, CAN_msg *);     // ������λ������ͨ������
    strCanCtrl  GetCtrlData(void) const;            // ���ͨ����������
    U8          GetOutBit(U8);                      // ��ȡ��λ��������������� 16-0514

    U8          Get_Sync(void) const;               // ��ȡͬ���ź�
    void        Set_Sync(S32 val);                  // ����ͬ���ź�

    U16         Test_RealAbn(void);                 // ��ȡͨ��ʵʱ�쳣����
    void        Set_RealAbn(U16 val);               // ����ʵʱ�쳣����
    
    void        Set_RealCur(float val);             // ���渡��ʵʱ�������� 16-0708
    void        Set_InstCur(S32 val);               // ����˲ʱ��������
    void        Set_RealVol(float val);             // ���渡��ʵʱ��ѹ���� 16-0708
    void        Set_InstVol(S32 val);               // ����˲ʱ��ѹ����
    
    S32         GetSimSetVol(void);                 // ��ȡģ���趨��ѹ���ݣ�ȡ������ѹ�м�ֵ��
    void        Set_RealTemp(S16 val);              // ����ʵʱ�¶�����
    void        Set_RealPow(void);                  // ����ʵʱ��������
    void        Set_RealRes(void);                  // ����ʵʱ��������
    void        Set_RealEngy(float val);            // ����ʵʱ��������
    void        Set_RealMVi(S32 val);               // ���浥���ѹ��ֵ
    void        Set_RealMTi(S32 val);               // ���浥���¶ȼ�ֵ
    void        Set_Stat(U32 val);                  // ������λ������
    void        Set_ExData(S32 rda, S32 rdb);       // ������λ����չ����
    void        Get_ViTiExtremum(void);             // ��ȡͨ�����弫ֵ

    //---------- REC ----------
    void        RecCompDataInit(void);
//  U8          Read_Record(U8 rec_num);            // PC ����¼  
    U8          Save_Record(U8 rec_mode);           // �����¼

    //---------- WORKING ----------
    U8          State_Init(U8 oldstate, U8 newstate);   // ����״̬��ʼ��
    void        State_Exit(U8 curstate);

    void        RunCtrlProc(void);                      // ͨ������̬����
    U8          Step_Init(U16 oldline, U16 newline);    // ȡ���ճ�ʼ��
    void        Run_RealDataCalc(void);                 // ����ʵʱ���ݼ���
    void        ParVar_Set(U16, U8, U8, float);         // �����������ʼ����ֵ
    void        ElsePrg_Set(U16);                       // �׶ι��� ������'����'�������趨������Ϊģʽ���� 16-0510��
    
#ifdef  USE_SUBPRG  // ���н׶θ������ձ��ݣ�ֱ�Ӵӹ��ձ��ݰ���ȥ����AND�������ݣ� 16-0507
    U8          AndPrg_Set(U16, strCanCtrl *);          // ����׶ι�����չ�趨(�̶����ݽṹ)
#else
    U8          AndPrg_Set(U16, U16, strCanCtrl *);     // ����׶ι�����չ�趨(�̶����ݽṹ)
#endif

    U8          ParVar_Limit(U16, U8, U8, float);       // ��������������ж�
    U8          ParVar_Operate(U8, U8, float);          // �������������
    
#ifdef  USE_SUBPRG  // ���н׶θ������ձ��� 16-0507
    U8          IfElsePrg_Limit(U16 &, U8);             // ��ģʽΪIF��ELSE�ж� 16-0507
    U8          ElsePrg_Limit(U8, U16 &, U8);           // �׶ι��� ������'����'�������ж� 16-0507 ����ģʽ������ģʽ 16-0725������תĿ��׶����� 
#else
    U8          ElsePrg_Limit(U16, U16 &, U8);          // �׶ι��� ������'����'�������ж� 16-0507 ������ģʽ����
#endif

    void        Set_WkLine(U8, U16);                    // ���¹������к�
    void        Set_JpFlag(U16 flag);                   // �趨��ת���
    U16         Get_WkLine(U16);                        // ���ݹ���������ȡ���ڹ���������
    U8          Get_WkMode(void);                       // ��ȡ���й��ձ����еĹ���ģʽ
    U8          Check_GlobalProg(strGblPrg *);          // ���ȫ�ֹ��ղ����Ϸ���
    U8          Check_StepProg(const strStpPrg *);      // �����̹��ղ����Ϸ���
    U8          Check_CanCtrl(strCanCtrl *);            // ���CAN���ƹ��ղ����Ϸ���    
    void        ClrJdgCnt(void);                        // �����бȼ���������
    U16         LowerStatMapping(U32 stat);             // ��λ������ӳ��
    U8          AbnorReMapping(U32 stat, U8 abnor);     // ��λ��������ӳ��

    void        PowerOnCtrlProc(void);                  // ͨ���ϵ�̬����
    void        StopCtrlProc(void);                     // ͨ��ֹ̬ͣ����
    void        Stop_Init(void);                        // ͨ��ֹͣ��ʼ�����������ʵʱ���ݣ�
    void        Clr_Prog(void);                         // �����������
    void        PauseCtrlProc(void);                    // ͨ����̬ͣ����
    void        XPauseCtrlProc(void);                   // ͨ���쳣��̬ͣ����
    void        AbnorCtrlProc(void);                    // ͨ���쳣̬����

    //----- SYS -----
//#if     (DEV_STR_TYPE == DS_NEILSBED)   // �봲�豸ר���ź�������  15-0603
    // ����ʲô�豸�ṹ���������봲�ź��� 16-0514
    void            SmokeSemSend(void);                     // �����̸б����ź���
    void            InPlaceSemSend(void);                   // �����봲���̵�λ�ź���
//#endif

    void            RecPtrRst(void);                        // �����¼ָ��  
    void            TickControlProc(void);                  // ���Ŀ��ƴ���
    void            Init_RunData(void);
    inline  U8      Get_CabCtrlFlag(void)   {return (PdHoldData->NrRData.CabCtrlFlag);};
    inline  U8      Get_CurStatus(void)     {return (PdHoldData->NrRData.CurWkStat);};
    inline  U8      Get_RealAbn(void)       {return (PdHoldData->RunData.AbnCode & 0xff);};
    inline  float   Get_RealTmp(void)       {return  PdHoldData->RunData.Temp;};
    inline  void    Clr_RealAbn(U8 code)    {if(code==(PdHoldData->RunData.AbnCode&0xff))PdHoldData->RunData.AbnCode=0;};
    U32             Get_PdStatus(void) const;
    U8              Is_Running(void);                       // �豸����̬�жϣ����С���ͣ���쳣��ͣ��

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
chan_ext    strPrgRcvCtrl   PrgRcvCtrl;         // ���ս��տ���
chan_ext    strRecBackUp    *pRecBackUp;        // �ϴ��ļ�¼���ݣ����籣�棩

//-------------------------------------------------- 
chan_ext    void    ChanRecPtrRstProc(void);        
chan_ext    void    ControlProc(void);    
//--------------------------------------------------
#ifdef      __cplusplus
    }
#endif
//----------------------------------------------------------------------------------------------------
#endif
