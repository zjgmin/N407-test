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
//  �ɰ汾���Ի�ͨ������ģʽ

#define     STATICMODE          0x11    // ����
#define     EXCTRLMODE          0x12    // ���ģʽ
#define     HLCHARGEMODE        0x25    // ������
#define     HLDISCHARGEMODE     0x2A    // ������
#define     HPCHARGEMODE        0x35    // �㹦�ʳ�
#define     HPDISCHARGEMODE     0x3A    // �㹦�ʷ�
#define     HRCHARGEMODE        0x45    // ������
#define     HRDISCHARGEMODE     0x4A    // ������
#define     XPVCHARGEMODE       0x55    // б�µ�ѹ��
#define     XPVDISCHARGEMODE    0x5A    // б�µ�ѹ��
#define     XPCCHARGEMODE       0x65    // б�µ�����
#define     XPCDISCHARGEMODE    0x6A    // б�µ�����
#define     XPPCHARGEMODE       0x75    // б�¹��ʳ�    ��δ����
#define     XPPDISCHARGEMODE    0x7A    // б�¹��ʷ�    ��δ����
#define     WKCHARGEMODE        0x85    // �¶ȿ��Ƴ�    ��δ����
#define     WKDISCHARGEMODE     0x8A    // �¶ȿ��Ʒ�    ��δ����
#define     FCHARGEMODE         0x95    // �����
#define     FDISCHARGEMODE      0x9A    // ���ŵ�
#define     PULSECHARGEMODE     0xA5    // �����磨��Ъ���壩
#define     PULSECH_DCHMODE     0xAA    // �����ŵ磨�������壩 

#define     HYCHARGEMODE        0xB5    // ��ѹ��  //2014-1010 added
#define     HYDISCHARGEMODE     0xBA    // ��ѹ��

#define     ENDMODE             0x00    // ����ģʽ
//----------------------------------------------------------------------------------------------------
//  �ɰ汾���Ի���λ������
//  ���µ�12λ����  �ɲ�ͣ�� ����λ������ 
//  2014 - 0628 ��Ƿ���ϲ� 0x0002

#define     Err_ZL          0x0001      // ��λ��δ��������
//#define     Err_DL          0x0002      // ��λ������
#define     Err_DQL         0x0002      // ��λ����Ƿ��
#define     Err_CL          0x0004      // ��λ������
//#define     Err_QL          0x0006      // ��λ��Ƿ��
#define     Err_PRG         0x0008      // ��λ���������ݴ���
#define     Err_HL          0x0010      // ��λ��������
#define     Err_HY          0x0020      // ��λ������ѹ
#define     Err_GQY         0x0040      // ��λ����Ƿѹ
#define     Err_GR          0x0080      // ��λ��ɢ��������
#define     Err_GRL         0x0100      // ��λ��ɢ��������5���� Ӳ������
#define     Err_VFZ         0x0200      // ��λ����ѹ���Է�
#define     Err_GY          0x0400      // ��λ��������ѹ
#define     Err_GL          0x0800      // ��λ����������
#define     Err_CUR         0x1000      // ��λ������У׼������ʧ����
#define     Err_VOL         0x2000      // ��λ����ѹУ׼������ʧ����
#define     Err_PID         0x4000      // ��λ������PID������ʧ����
#define     Err_DEV         0x8000      // ��λ���豸�������ʧ����

#define     Err_Par         (Err_CUR|Err_VOL|Err_PID|Err_DEV)   // ��������
//------------------------------------------------------------
//  ��λ����չ����
#define     Err_BUSOV       0x00010000      // ��λ��ĸ�߹�ѹ
#define     Err_BUSUV       0x00020000      // ��λ��ĸ��Ƿѹ
#define     Err_SENOT       0x00040000      // ��λ������������
#define     Err_SENUT       0x00080000      // ��λ������������
#define     Err_CCCVS       0x00100000      // ��λ��������縺б��
#define     Err_DCCVS       0x00200000      // ��λ�������ŵ���б��
#define     Err_SYSBUSY     0x00400000      // ��λ��ϵͳæ--�������Ź����(�޺�̨�������г�ʱ)���ж���Ϣ��ʱ
#define     Err_DRV         0x00800000      // ��λ����������
#define     Err_BUSFB       0x01000000      // ��λ��ĸ�ߵ��ࣨĸ�ߵ�ѹС�ڵ�ص�ѹ��
#define     Err_BUS         0x02000000      // ��λ��ĸ��У׼������
#define     Err_CAP         0x04000000      // ��λ������У׼������
#define     Err_BUSFREQ     0x08000000      // ��λ��ĸ��Ƶ������(���Զ��ָ�)
#define     Err_DFZ         0x10000000      // ��λ�������߷��� 15-1202
#define     Err_VBK         0x20000000      // ��λ�������ߵ��� 15-1202
//  δ���屸����չ����

#define     EXE_BIT30       0x40000000      //
#define     EXE_BIT31       0x80000000      //
//------------------------------------------------------------
#define     MAX_LERR_NUM    32              // ��ǰ��λ��������Ŀ
//----------------------------------------------------------------------------------------------------
//  ͨ������ģʽ����

#define     MODE_INIT           0x00        // ������ʼ��
#define     MODE_CCC            0x01        // �������
#define     MODE_CVC            0x02        // ��ѹ���
#define     MODE_CCD            0x03        // �����ŵ�
#define     MODE_CVD            0x04        // ��ѹ�ŵ�
#define     MODE_CPD            0x05        // �㹦�ʷŵ�
#define     MODE_CPC            0x06        // �㹦�ʳ��
#define     MODE_CRD            0x07        // �����ŵ�
#define     MODE_STATIC         0x08        // ����
#define     MODE_PUL_C          0x09        // ������
#define     MODE_PUL_D          0x0a        // ����ŵ�
#define     MODE_PUL_STATIC     0x0b        // �������
#define     MODE_LINEAR_D       0x0c        // ���Էŵ�
#define     MODE_IF             0x0d        // ���
#define     MODE_ELSE           0x0e        // ����
#define     MODE_VAR_OP         0x0f        // ��������
#define     MODE_AND            0x10        // ��

#define     MODE_PUL_JX         0x20        // ��Ъ����
#define     MODE_PUL_ZF         0x21        // �������壨���ڳ�磩
#define     MODE_EXT_CTRL       0x22        // ��ع��գ������ѭ����
#define     MODE_CRC            0x23        // ������� 16-0422 added

#define     MODE_END            0xff        // ���н���

#define     MODE_LOWERERR       0xfe00      // ������λ�����ϱ��,����
                                            // �����ж�����λ��������λ�����¹��ϵģ�ȥ������ʱʹ�� 2014-0628
//----------------------
#define     MIN_ERR_MODE       (MODE_PUL_ZF+1)  // ��С����ģʽ����
#define     MAX_ERR_MODE       (MODE_END-1)     // ������ģʽ����
//----------------------------------------------------------------------------------------------------
//  ʵʱ������ϸ(��¼)��������λ
#define     REALDATA_FLAG       ((unsigned char)(0 << 7))  // ʵʱ���ݱ��
#define     RECDATA_FLAG        ((unsigned char)(1 << 7))  // ��ϸ���ݱ��
//----------------------------------------------------------------------------------------------------
//  ��¼��־

#define     FIRST_REC           ((unsigned char)0)         // �׼�¼��־
#define     LAST_REC            ((unsigned char)1)         // β��¼��־
#define     PLAT1_REC           ((unsigned char)2)         // ƽ̨��¼1��־
#define     PLAT2_REC           ((unsigned char)3)         // ƽ̨��¼2��־
#define     PLAT3_REC           ((unsigned char)4)         // ƽ̨��¼3��־
#define     FINISH_REC          ((unsigned char)5)         // ������ɱ�־
#define     PROC_REC            ((unsigned char)6)         // ���̼�¼��־
#define     NORMAL_REC          ((unsigned char)7)         // ��ͨ��¼��־

#define     FIRST_VAR_REC       ((unsigned char)8)         // �ױ�����¼
#define     LAST_VAR_REC        ((unsigned char)9)         // β������¼

//----------------------------------------------------------------------------------------------------
//  ͨ������״̬����

#define     WORKSTAT_INIT       0x00            // ��ʼ��״̬ �����жϹ��ϣ�������PC��Ԥ���
#define     WORKSTAT_STOP       0x01            // ֹͣ״̬
#define     WORKSTAT_RUN        0x02            // ����״̬
#define     WORKSTAT_PAUSE      0x03            // ��ͣ״̬ ������ʱ�˹���ͣ��
#define     WORKSTAT_XPAUSE     0x04            // �쳣��ͣ״̬ ������ʱ���쳣��
#define     WORKSTAT_ABNOR      0x05            // �쳣״̬ ��������ʱ���쳣��

#define     MIN_ERR_WKSTAT      (WORKSTAT_ABNOR+1)    // ��С������̬���

//#define     WORKSTAT_POWDN      (0x06)                // ����״̬ ������ʱ���磩
//#define     MIN_ERR_WKSTAT      (WORKSTAT_POWDN+1)    // ��С������̬���
//----------------------------------------------------------------------------------------------------
//  ͨ���쳣����

#define     ABNOR_NULL          0x00        // ���쳣

#define     ABNOR_OV            0x01        // ��ѹ
#define     ABNOR_UV            0x02        // Ƿѹ
#define     ABNOR_OC            0x03        // ����
#define     ABNOR_UC            0x04        // ��Ƿ��
#define     ABNOR_OT            0x05        // ����
#define     ABNOR_UT            0x06        // Ƿ��
#define     ABNOR_COM_Vi        0x07        // �����ѹͨ���쳣
#define     ABNOR_COM_Ti        0x08        // �¶�ͨ���쳣
#define     ABNOR_POWEROFF      0x0A        // ����
#define     ABNOR_DATAOF        0x0B        // �����������¼�������
#define     ABNOR_OP            0x0C        // ������
#define     ABNOR_NUM_Vi        0x0D        // �����ѹ����δ����
#define     ABNOR_TotalV        0x0E        // �����ѹ���쳣
#define     ABNOR_EEPROM        0x0F        // EEPROM�����쳣
#define     ABNOR_OVi           0x10        // �����ѹ
#define     ABNOR_UVi           0x11        // ����Ƿѹ
#define     ABNOR_OCAP          0x12        // ������
#define     ABNOR_STOPDATA      0x13        // ֹͣ������δ���꡾�������쳣״̬��
#define     ABNOR_IN_CALIB      0x14        // У׼������
#define     ABNOR_LC            0x15        // ©����
#define     ABNOR_POLAR         0x16        // ����
#define     ABNOR_OUV           0x17        // ��Ƿѹ
#define     ABNOR_CV            0x18        // ����ѹ
#define     ABNOR_CC            0x19        // ������
#define     ABNOR_CTRLDAT       0x1A        // �������ݴ���
#define     ABNOR_REV           0x1B        // REV
#define     ABNOR_SYSCUR        0x1C        // ����У׼���ݴ���      // ĸ��Ƿѹ 
#define     ABNOR_SYSVOL        0x1D        // ��ѹУ׼���ݴ���      // ���������� 
#define     ABNOR_SYSPID        0x1E        // PID��������ݴ���     // ����������
#define     ABNOR_SYSDEV        0x1F        // ϵͳ���ݴ���

//  ��չ �����쳣����
#define     ERR_GETWKLINE       0x20        // ��ȡ�����кŴ���
#define     ERR_PARVAR          0x21        // �Ƿ����������
#define     ERR_OPT             0x22        // �Ƿ�������
#define     ERR_NOLIMIT         0x23        // ��������δ�趨
#define     ERR_STPPRG          0x24        // �׶ι��ռ�����
#define     ERR_GBLPRG          0x25        // ȫ�ֹ������ݴ���
#define     ERR_ELSENUM         0x26        // ������
#define     ERR_MODE            0x27        // δ֪����ģʽ
#define     ERR_CCMAX           0x28        // ����������
#define     ERR_DCMAX           0x29        // �����ŵ����
#define     ERR_CUMAX           0x2A        // ��������ѹ
#define     ERR_DUMIN           0x2B        // ����С�ŵ��ѹ
#define     ERR_CPMAX           0x2C        // ������繦��
#define     ERR_DPMAX           0x2D        // �����ŵ繦��
#define     ERR_STPINFINITE     0x2E        // �׶���ת��ѭ��

//-----
#define     ERR_MCANOFFLINE     0x30        // ����CAN�ѻ�
#define     ERR_SCANOFFLINE     0x31        // ����CAN�ѻ�
#define     EER_LOWER_NORMAL    0x32        // ��λ����ͨ���ϣ���λ������ͣ��
#define     EER_LOWER_FATAL     0x33        // ��λ����Ҫ���ϣ���λ����ͣ��������
#define     EER_SYNC            0x34        // �׶�����ʱ����λ��ͬ��ʧ��
#define     ERR_PCANOFFLINE     0x35        // ����CAN�ѻ� 15-1213
#define     ERR_MCANCONNECT     0x36        // ����CAN��������

//  ��λ������չ�쳣
#define     ABNOR_BUSOV         0x40        // ĸ�߹�ѹ
#define     ABNOR_BUSUV         0x41        // ĸ��Ƿѹ
#define     ABNOR_SENOT         0x42        // ����������
#define     ABNOR_SENUT         0x43        // ����������
#define     ABNOR_CCCVS         0x44        // ��������ѹ��б��
#define     ABNOR_DCCVS         0x45        // �����ŵ��ѹ��б��
#define     ABNOR_SYSBUSY       0x46        // ϵͳæ�����������
#define     ABNOR_DRVOC         0x47        // ���������� 15-0622
#define     ABNOR_BUSFB         0x48        // ĸ�ߵ��� 15-0808
#define     ABNOR_BUS           0x49        // ĸ��У׼������
#define     ABNOR_CAP           0x4A        // ����У׼������
#define     ABNOR_BUSFREQ       0x4B        // ĸ��Ƶ������
#define     ABNOR_DFZ           0x4C        // �����߷��� 15-1202
#define     ABNOR_VBK           0x4D        // �����ߵ��� 15-1202

#define     ABNOR_BIT30         0x4E        //
#define     ABNOR_BIT31         0x4F        //

//----------
#define     ABNOR_OTi           0x50        // �������
#define     ABNOR_UTi           0x51        // ����Ƿ��
#define     ABNOR_SMOKE         0x52        // �̸б���
#define     ABNOR_OFFPLACE      0x53        // ����δ��λ
//----------
#define     ERR_WKSTAT          0x80        // δ֪����̬����
#define     EER_SPACEOVF        0x81        // �ռ����
#define     ERR_SRAM            0x82        // SRAMӲ������
#define     ERR_PDDATA          0x83        // �������ݴ���

#define     ABNOR_SELFDEL       0xFF        // �����ǰ����
//----------------------------------------------------------------------------------------------------
#define     STEPINIT_OK         0xff        // �׶ι��ճ�ʼ���ɹ�
#define     STEPINIT_IF         0xfe        // �׶ι�����IF�ĳ�ʼ��
#define     STEPINIT_ELSE       0xfd        // �׶ι�����ELSE�ĳ�ʼ��
#define     STEPINIT_VAR        0xfc        // �׶ι�����VAR(��������)�ĳ�ʼ��
#define     STEPINIT_AND        0xfb        // �׶ι�����AND�ĳ�ʼ�� 15-0929 added
//----------------------------------------------------------------------------------------------------
//����������

#define     OP_NULL             0x00        // δ�趨
#define     OP_EQU              0x01        // ����     ==
#define     OP_MORE             0x02        // ����     >
#define     OP_MORE_EQU         0x03        // ���ڵ��� >=
#define     OP_LESS             0x04        // С��     <
#define     OP_LESS_EQU         0x05        // С�ڵ��� <=
#define     OP_ASSIGN           0x06        // ��ֵ     =
#define     OP_PLUS             0x07        // ��       +=
#define     OP_MINUS            0x08        // ��       -=
#define     OP_INC              0x09        // ����     ++
#define     OP_DEC              0x0a        // �ݼ�     --

//----------
#define     MIN_ERR_OP          (OP_DEC+1)  // ��С ����������

//----------------------------------------------------------------------------------------------------
//��������������

#define     VAR_NULL            0x00        // δ����
#define     VAR_TIME1           0x01        // ʱ�����1(����0.1S)
#define     VAR_CAP1            0x02        // ��������1
#define     VAR_CYC1            0x03        // ѭ������1(����)
#define     VAR_CYC2            0x04        // ѭ������2(����)
#define     PAR_TIME            0x05        // ����ʱ��
#define     PAR_VOL             0x06        // ��ѹ
#define     PAR_CUR             0x07        // ����
#define     PAR_TEMP            0x08        // �¶�
#define     PAR_CAP             0x09        // ����
#define     PAR_CYCNUM          0x0a        // ѭ������(�޷�������)
#define     PAR_V_NSLOPE        0x0b        // ��ѹ��б��
#define     PAR_SLOPE           0x0c        // б��
#define     PAR_MAXVi           0x0d        // ������ѹ
#define     PAR_MINVi           0x0e        // ��С�����ѹ
#define     PAR_MAXI            0x0f        // ������
#define     PAR_MINI            0x10        // ��С����
#define     PAR_MAXTi           0x11        // ������¶�
#define     PAR_MINTi           0x12        // ��С�����¶�
#define     PAR_POWER           0x13        // ����
#define     PAR_ENERGY          0x14        // ����
#define     VAR_CYC3            0x15        // ѭ������3(����)
#define     VAR_CYC4            0x16        // ѭ������4(����)
#define     VAR_CYC5            0x17        // ѭ������5(����)
#define     VAR_TCOEF           0x18        // �¶�ϵ�� 16-0531
#define     VAR_STPCAP          0x19        // �׶Σ����̣����� 16-0531
#define     VAR_CVOLTDROP       0x20        // ����ѹ���� 16-0716
//----------
#define     MIN_ERR_VAR_PAR     (VAR_CVOLTDROP+1)    // ��С ���������������� 16-0716

//----------------------------------------------------------------------------------------------------
//  �����ּ���Ӧ�汾����
//----------------------------------------------------------------------------------------------------
/*
    ����汾��   ������Ӧ������ݶ�Ӧ�汾
                ͨ�ð汾��0x0A��0xC7(Ver 1.0 ~ Ver19.9)
                ����汾��0xC8��0xFE(Ver20.0 ~ Ver25.4)
                0~9������չ
                0xff��ʾ��֧������
    ���¼�¼��
            15-0923 ���� 0x81 - ��ʵʱ��ϸ����  ������ V1.1(0x0B)
                    ���� 0x83 - ͨ����������    ������ V1.1(0x0B)
            15-1116 ���� 0x80 - ��������        ������ V1.1(0x0B)
                    ���� 0x84 - д�豸��Ϣ����  ������ V1.1(0x0B)
            15-1223 ���� 0xC0~C5 - V1.0 ����
            16-0125 ���� 0x80 - ��������        ������ V1.2(0x0C) ֧���������
            16-0125 ���� 0x84 - д�豸��Ϣ����  ������ V1.2(0x0C) ֧���������
            16-0801 ���� 0xF1 - ͸���������� V1.0
*/
//----------------------------------
#ifdef  IN_APP_LAYER

    #define   NVER      (0xFF)      // �в�֧�ֵ�����汾
    
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
#define     CMD_CONNECTION          0x80        // ����
#define     CMD_RD_REALDATA         0x81        // ��ȡʵʱ����
#define     CMD_CIRCUIT_CTRL        0x82        // ͨ������
#define     CMD_CIRCUIT_START       0x83        // ͨ������
#define     CMD_MODIFY_DEVINFO      0x84        // �޸��豸��Ϣ
#define     CMD_RD_PRECISION        0x85        // ��ȡ����ϵ��
#define     CMD_CAL_PRECISION       0x86        // ���ȵ���
#define     CMD_WR_PRECISION        0x87        // д�뾫��ϵ��
#define     CMD_ALLOC_TEMP          0x88        // �¶ȷ���
#define     CMD_CFG_MAC_IP          0x89        // MAC��IP����
#define     CMD_RD_DEV_MAC_IP       0x8a        // ��ȡָ���豸�ŵ�MAC��IP
#define     CMD_RD_HIP_SMAC         0x8b        // ��ȡ����IP��ַ�Լ��豸MAC
#define     CMD_RQ_HOSTCTRL         0x8c        // �������ط���Э��
#define     CMD_TEST_HARDWARE       0x8d        // ����AD,DA,�Ӵ���,LED
#define     CMD_RD_EEPROM           0x8e        // ��ȡEEPROM
#define     CMD_WR_EEPROM           0x8f        // д��EEPROM

#define     CMD_RD_SYSBIT           0x90        // ��ϵͳ����λ����������λ�����ܿ��ƿ��أ�
#define     CMD_WR_SYSBIT           0x91        // дϵͳ����λ����������λ�����ܿ��ƿ��أ�
#define     CMD_RD_CALIB            0x92        // ��ȡУ׼����
#define     CMD_WR_CALIB            0x93        // д��У׼����
#define     CMD_RD_MASKREG          0x94        // ��ȡ�������μ�������
#define     CMD_WR_MASKREG          0x95        // д�뱨�����μ�������
#define     CMD_RD_IRATE            0x96        // ��ȡ�����
#define     CMD_WR_IRATE            0x97        // д������
#define     CMD_RD_URATE            0x98        // ��ȡ���ѹ
#define     CMD_WR_URATE            0x99        // д����ѹ
#define     CMD_RD_IPID             0x9A        // ��ȡ����PID
#define     CMD_WR_IPID             0x9B        // д�����PID
#define     CMD_RD_UPID             0x9C        // ��ȡ��ѹPID
#define     CMD_WR_UPID             0x9D        // д���ѹPID
#define     CMD_RD_TVALUE           0x9E        // ��ȡ��ʱʱ��
#define     CMD_WR_TVALUE           0x9F        // д����ʱʱ��

#define     CMD_RD_SVALUE           0xA0        // ��ȡ������ֵ
#define     CMD_WR_SVALUE           0xA1        // д�뱨����ֵ
#define     CMD_RD_VERSION          0xA2        // ��ȡ����汾
#define     CMD_WR_CANADDR          0xA3        // д��CAN��ַ
#define     CMD_REV_0xA4            0xA4        // ����
//----------------------------------
//��λ���������
//----------
#define     CMD_CALIBSET            0xA5        // �����ָ�У׼����
#define     CMD_RD_CTRLCFG          0xA6        // ��ȡ��λ���������ò���
#define     CMD_WR_CTRLCFG          0xA7        // д����λ���������ò���
#define     CMD_RD_SCH_HCFG         0xA8        // ��ȡ����ͨ��Ӳ�����ò���
#define     CMD_WR_SCH_HCFG         0xA9        // д�븨��ͨ��Ӳ�����ò���
#define     CMD_RD_SCH_SCFG         0xAA        // ��ȡ����ͨ��������ò���
#define     CMD_WR_SCH_SCFG         0xAB        // д�븨��ͨ��������ò���
#define     CMD_REV_0xAC            0xAC        // ����
#define     CMD_REV_0xAD            0xAD        // ����
#define     CMD_RESET_DEV           0xAE        // ��λ��λ��
#define     CMD_REV_0xAF            0xAF        // ����
//----------------------------------
//����ͨ���������
//----------
#define     CMD_RD_SUBREAL          0xB0        // ��ȡ����ͨ��ʵʱ����
#define     CMD_WR_SUBREAL          0xB1        // д�븨��ͨ��ʵʱ����
#define     CMD_RD_SUBRATE          0xB2        // ��ȡ����ͨ�������
#define     CMD_WR_SUBRATE          0xB3        // д�븨��ͨ�������
#define     CMD_RD_SUBCALIB         0xB4        // ��ȡ����ͨ��У׼����
#define     CMD_WR_SUBCALIB         0xB5        // д�븨��ͨ��У׼����
#define     CMD_RD_SUBEDATA         0xB6        // ��ȡ����ͨ����չ���ݣ�CAN��ַ��AD���Ʋ�����
#define     CMD_WR_SUBEDATA         0xB7        // д�븨��ͨ����չ���ݣ�CAN��ַ��AD���Ʋ�����
#define     CMD_REV_0xB8            0xB8        // ����
#define     CMD_WR_SUBSINGLE        0xB9        // ����ͨ����������
#define     CMD_RD_TVALUE_EX        0xBA        // ��ȡ��չ��ʱʱ��(��ͨ��)
#define     CMD_WR_TVALUE_EX        0xBB        // д����չ��ʱʱ��(��ͨ��)
#define     CMD_RD_SVALUE_EX        0xBC        // ��ȡ��չ������ֵ(��ͨ��)
#define     CMD_WR_SVALUE_EX        0xBD        // д����չ������ֵ(��ͨ��)
#define     CMD_PROTL_TRANS         0xBE        // Э����ת

//----------------------------------
#define     CMD_RD_BUSDATA          0xC0        // ��ȡĸ�߲��� 15-1223
#define     CMD_WR_BUSDATA          0xC1        // д��ĸ�߲��� 15-1223
#define     CMD_RD_FREQDATA         0xC2        // ��ȡƵ�ʵȲ��� 15-1223
#define     CMD_WR_FREQDATA         0xC3        // д��Ƶ�ʵȲ��� 15-1223
#define     CMD_RD_DUTYDATA         0xC4        // ��ȡռ�ձȲ��� 15-1223
#define     CMD_WR_DUTYDATA         0xC5        // д��ռ�ձȲ��� 15-1223

//----------------------------------
#define     CMD_TRANSPARENT         0xF1        // ͸������ 16-0801
//----------------------------------------------------------------------------------------------------
//͸������(0xF1)������غ궨�� 16-0801
#define     CAN_INTERFACE           (0x00)  // CAN�ӿ�͸������
#define     UART_INTERFACE          (0x01)  // ���нӿ�͸������

//----------------------------------------------------------------------------------------------------
//CANͨ�Ž׶�ͬ������
#define     SYNC_FAILURE        0x03        // �׶�����ʱ,��λ����δͬ�����
#define     SYNC_CMD1_MASK      0xfe        // �׶�����ʱ,����0x43ͬ����ɱ��
#define     SYNC_CMD2_MASK      0xfd        // �׶�����ʱ,����0x44ͬ����ɱ��
#define     SYNC_SUCCESS        0x00        // �׶�����ʱ,��λ��ͬ����ɱ��
//----------------------------------------------------------------------------------------------------
//���ع�����
//layer-1
#define     ANALYSIS_OK         0x00        // �ɹ����������������
#define     STA_ERR             0x01        // ��ʼ�ִ�
#define     LEN_ERR             0x02        // ���ȴ���
#define     DAT_ERR             0x03        // ���ݴ���
#define     CRC_ERR             0x04        // У�����
#define     STP_ERR             0x05        // �����ִ�
#define     FORMAT_ERR          0x06        // ���ݸ�ʽ����
#define     PACK_LEN_ERR        0x07        // ��װ�󳤶ȴ���
#define     CMD_ERR             0x08        // ��Ч����
//layer-2

//layer-3
#define     DEV_NO_ERR          0x30        // �豸�Ŵ���
#define     L3_LEN_ERR          0x31        // ��װ�󳤶ȴ���(����Ӧ�ò�)

//----------------------------------------------------------------------------------------------------
// ͨ�����Ʒ���
//----------------------------------
//������
#define     OPT_INVALID         0x00        // ��Ч
#define     OPT_STOP            0x01        // ֹͣ
#define     OPT_PAUSE           0x02        // ��ͣ
#define     OPT_RESUME          0x03        // �ָ�
#define     OPT_JUMP            0x04        // ��ת
#define     OPT_SINGLESTEP      0x05        // ��������
#define     OPT_LED_FLASH       0x06        // ��ָʾ��
#define     OPT_LED_OFF         0x07        // ��ָʾ��
//#define     OPT_RESTART         0x08        // �������� 16-0128 ȡ��
//#define     OPT_LOCK            0x09        // ��·���� 16-0128 ȡ��

#define     MAX_OPT_CODE        (OPT_LED_OFF)
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
// Ӧ�ò�����󷵻�����֡�ṹ
typedef struct _strDataBuf
{
    unsigned short  dat_len;
    unsigned short  max_len;
    unsigned char   data[1490];

} strDataBuf;
//----------------------------------------------------------------------------------------------------
// ����֡��֡�ṹ
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
