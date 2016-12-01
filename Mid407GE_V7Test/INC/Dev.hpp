//----------------------------------------------------------------------------------------------------
#ifndef __DEV_HPP
    #define __DEV_HPP

#include    "RTX_CAN.h"
#include    "main.h"
//----------------------------------------------------------------------------------------------------
//C++�ඨ��
//-------------------------------------------------------
#ifdef    _IN_DEV_CPP
    #define   dev_ext
#else
    #define   dev_ext extern
#endif
//----------------------------------------------------------------------------------------------------
#define   DEVINFO_MARK      (0x00020000|(MAX_CHAN_NUM))     // �豸��Ϣ����� 2016-0125

//#define   CTRLCFG_MARK      (0x00040000|(MAX_CHAN_NUM))     // �豸�������ñ���� 2016-0125
#define   CTRLCFG_MARK      (0x00050000|DEV_STR_TYPE)       // �豸�������ñ���� 2016-0607 ��ṹ���
    
#define   SUBCHCFG_MARK     (0x00010001)                    // ����ͨ��Ӳ�����ñ����

#define   VIHDCFG_MARK      (0x00020000|DEV_STR_TYPE)       // �����ѹӲ�����ñ����(��Ӳ���ṹ���) 16-0125
#define   TIHDCFG_MARK      (0x00020000|DEV_STR_TYPE)       // �����¶�Ӳ�����ñ����(��Ӳ���ṹ���) 16-0125
#define   IIHDCFG_MARK      (0x00020000|DEV_STR_TYPE)       // ��������Ӳ�����ñ����(��Ӳ���ṹ���) 16-0125
#define   OIHDCFG_MARK      (0x00020000|DEV_STR_TYPE)       // ��������Ӳ�����ñ����(��Ӳ���ṹ���) 16-0125
#define   XIHDCFG_MARK      (0x00020000|DEV_STR_TYPE)       // ��������Ӳ�����ñ����(��Ӳ���ṹ���) 16-0125

#define   SAMPCFG_MARK      (0x00010002)                    // ����ͨ�����ñ�����

#define   TEMPTBL_MARK      (0x00010002)                    // �¶����ñ�����

#define   NETINFO_MARK      (0x00010002)                    // ������Ϣ������

#define   HOSTINF_FLAG      (0x0001)                        // ������Ϣ������

#define   NODETBL_MARK      (0x00010000)                    // ͨ��ӳ����ұ����� 16-0129

//----------------------------------------------------------------------------------------------------

#define   NULL32_VAL        (0xffffffff)      // �����趨��Ч�Ķ���ֵ
                                              // 1 - ȫ�ֱ���ֵ
                                              // 2 - ĳЩ�豸���������������¶ȣ�

//----------------------------------------------------------------------------------------------------
//���ָ�������ͨ�������룬������������
#define     VI_ID       0x01    // ��ѹ���帨��ͨ��������
#define     TI_ID       0x02    // �¶ȵ��帨��ͨ��������
#define     II_ID       0x03    // �������븨��ͨ��������
#define     OI_ID       0x04    // ������������ͨ��������
#define     XI_ID       0x05    // ��������ͨ��������

#define     EBI_ID      0x81    // ��չλ���븨��ͨ��������
#define     EBO_ID      0x82    // ��չλ�������ͨ��������
#define     EDI_ID      0x83    // ��չ�������븨��ͨ��������
#define     EDO_ID      0x84    // ��չ�����������ͨ��������
//----------------------------------------------------------------------------------------------------

//  ȫ�ֹ��ϱ�� GblFault
#define   GBL_SRAM_FAULT    (1<<0)          // SRAM����

//====================================================================================================
//  ��λ���������ò�����غ궨��
//====================================================================================================

#define   PDRESUME_DEF    (1)     // ����ָ���� Ĭ�ϣ�1 - ����
#define   STEP_SYNC_DEF   (1)     // �׶�ͬ����� Ĭ�ϣ�1 - ����
#define   PARALLEL_DEF    (0)     // ��������� Ĭ�ϣ�0 - ��ֹ
#define   ABNRESUME_DEF   (0)     // ���ϻָ���� Ĭ�ϣ�0 - ��ֹ
//----------
#define   MIDEV_FLAG      (0xff)  // ����λ�������ı��
//----------------------------------------------------------------------------------------------------
//  ��������ͨ��      14-0724 modified
//----------
#define   MAX_SUBCHAN_BOARD   (240)

#define   MAX_VI_BOARD        (40)    // ����ѹ��Ԫ����
#define   MAX_BOARDVI_NUM     (32)    // ����������� total 1280 points
#define   MAX_CHANVI_NUM      (240)   // ͨ�����������

#define   MAX_TI_BOARD        (40)    // ����¶�ͨ������
#define   MAX_BOARDTI_NUM     (32)    // ����������� total 1280 points
#define   MAX_CHANTI_NUM      (120)   // ͨ�����������

#define   MAX_II_BOARD        (20)    // ��󿪹��������
#define   MAX_BOARDII_NUM     (64)    // ������󿪹����� total 1280 points
#define   MAX_CHANII_NUM      (40)    // ͨ�����������

#define   MAX_OI_BOARD        (20)    // ��󿪹���������
#define   MAX_BOARDOI_NUM     (64)    // ������󿪹����� total 1280 points
#define   MAX_CHANOI_NUM      (40)    // ͨ�����������

#define   MAX_XI_BOARD        (4)     // ���������չ����(����+�����2��) 16-0308
#define   MAX_BOARDXI_NUM     (32)    // ���������չ��32��IO 15-1229
#define   MAX_CHANXI_NUM      (16)    // ͨ����������� 15-1116 4 -> 16

//----------
//  ��չ����ͨ��
#define   MAX_EBI_BOARD       (8)     // �����չ����
#define   MAX_BOARDEBI_NUM    (64)    // ��չ���������
#define   MAX_CHANEBI_NUM     (8)     // ��չͨ�����������

#define   MAX_EBO_BOARD       (8)     // �����չ����
#define   MAX_BOARDEBO_NUM    (64)    // ��չ���������
#define   MAX_CHANEBO_NUM     (8)     // ��չͨ�����������

#define   MAX_EDI_BOARD       (8)     // �����չ����
#define   MAX_BOARDEDI_NUM    (32)    // ��չ���������
#define   MAX_CHANEDI_NUM     (16)    // ��չͨ�����������

#define   MAX_EDO_BOARD       (8)     // �����չ����
#define   MAX_BOARDEDO_NUM    (32)    // ��չ��������� 
#define   MAX_CHANEDO_NUM     (16)    // ��չͨ�����������
//----------
//  ����ͨ��Ĭ���趨ֵ
#define   DEF_VI_BOARD        (1)     // Ĭ�ϵ�ѹ����
#define   DEF_TI_BOARD        (1)     // Ĭ���¶Ȱ���
#define   DEF_II_BOARD        (1)     // Ĭ���������
#define   DEF_OI_BOARD        (1)     // Ĭ���������

#define   DEF_XI_BOARD        (4)     // Ĭ�������������0->4 16-0803 ������չIOһһӳ��

#define   DEF_EBI_BOARD       (0)     // Ĭ����չ����
#define   DEF_EBO_BOARD       (0)     // Ĭ����չ����
#define   DEF_EDI_BOARD       (0)     // Ĭ����չ����
#define   DEF_EDO_BOARD       (0)     // Ĭ����չ����

#define   DEF_BOARDVI_NUM     (24)    // Ĭ�ϵ����ѹ����
#define   DEF_BOARDTI_NUM     (32)    // Ĭ�ϵ����¶ȵ���
#define   DEF_BOARDII_NUM     (32)    // Ĭ�ϵ����������
#define   DEF_BOARDOI_NUM     (32)    // Ĭ�ϵ����������

#define   DEF_BOARDXI_NUM     (32)     // Ĭ�������������1->32 15-1228 ������չIOһһӳ��

#define   DEF_BOARDEBI_NUM    (1)     // Ĭ�ϵ�����չλ�������
#define   DEF_BOARDEBO_NUM    (1)     // Ĭ�ϵ�����չλ�������
#define   DEF_BOARDEDI_NUM    (1)     // Ĭ�ϵ�����չ�����������
#define   DEF_BOARDEDO_NUM    (1)     // Ĭ�ϵ�����չ�����������

//----------------------------------------------------------------------------------------------------
//  ��������Ĭ�ϲ��� 
//----------
//  change with 'CTRLCFG_MARK'  and Channel Numbers
//------------------------------
//  (1)     �ж���ʱ����������   ��ͬ��λ��ͨ����Ŀ��أ�

#if   (MAX_CHAN_NUM == 0)
    #error    "CAN device number can't not be zero!!"
#elif (MAX_CHAN_NUM <= 16)
    #define   JDG_TIMES     (1)
#elif (MAX_CHAN_NUM <= 32)
    #define   JDG_TIMES     (1)
#elif (MAX_CHAN_NUM <= 64)
    #define   JDG_TIMES     (2)
#elif (MAX_CHAN_NUM <= 96)
    #define   JDG_TIMES     (2)
#elif (MAX_CHAN_NUM <= 128)
    #define   JDG_TIMES     (3)
#elif (MAX_CHAN_NUM <= 192)     // 16-0519
    #define   JDG_TIMES     (4)    
#else
    #error  "Too many channels!"
#endif
//------------------------------
//  (2)     �ж���ʱֵ������ֵ

#define   MAX_JDG_T       (65000)               // ����ж���Ӧ����

#define   CAN_TO_N        (50)  //(20)          // CAN�޽��ճ�ʱ���� 16-0528
#define   CTRL_RETRY_N    (3)                   // ���ƹ������Դ���
#define   CAN_RETRY_N     (5)                   // ͨ�Ź������Դ���
#define   CTRL_RETRY_T    (2000)                // ���ƹ������Լ�� (mS)
#define   CAN_RETRY_T     (2000)                // ͨ�Ź������Լ�� (mS)
#define   PD_RESUME_T     (5000)                // ����ָ���ʱ (mS)
#define   CTRL_VDELTA     (1000)                // ����ת����ѹ��ֵ (mV) 16-0113
    #define   LI_VDELTA     (5)                 // ����ת����ѹ��ֵ (���֮һ) 16-0607
    #define   MAX_VDELTA    (50000)                // ������ת����ѹ��ֵ (mV) 16-0113
    #define   MIN_VDELTA    (1)                    // ��С����ת����ѹ��ֵ (mV) 16-0113
#define   START_DLY_T     (0)                   // Ĭ��������ʱ (mS)

#define   XCUR_JDG_T      (1000 * JDG_TIMES)    // �����쳣�ж���Ӧ����   (mS)
#define   XVOL_JDG_T      (1000 * JDG_TIMES)    // ��ѹ�쳣�ж���Ӧ����   (mS)
#define   XTEMP_JDG_T     (1000 * JDG_TIMES)    // �¶��ж���Ӧ���� (mS)
#define   XVI_JDG_T       (1000 * JDG_TIMES)    // �����ѹ�ж���Ӧ����   (mS)
#define   XTI_JDG_T       (1000 * JDG_TIMES)    // �����¶��ж���Ӧ����   (mS)

//2013-1013 ��ѹ�����жϼӿ�2000mS->200��30mS 
//2014-0703 �Ļ� 1000
#define   IO_JDG_T        (1000 * JDG_TIMES)    // �������ж���Ӧ����    (mS)
#define   VPOT_JDG_T      (500 * JDG_TIMES)     // ƽ̨��ѹ�ж���Ӧ����   (mS)
#define   CUR_JDG_T       (500 * JDG_TIMES)     // �����ж���Ӧ���� (mS)
#define   VOL_JDG_T       (500 * JDG_TIMES)     // ��ѹ�ж���Ӧ���� (mS)
//#define   TEMP_JDG_T      (1000 * JDG_TIMES)    // �¶��ж���Ӧ����   (mS)
#define   POW_JDG_T       (1000 * JDG_TIMES)    // ������Ӧ����   (mS)
#define   ENGY_JDG_T      (1000 * JDG_TIMES)    // �����ж���Ӧ���� (mS)
#define   VI_JDG_T        (1000 * JDG_TIMES)    // �����ѹ�ж���Ӧ����   (mS)
#define   TI_JDG_T        (1000 * JDG_TIMES)    // �����¶��ж���Ӧ����   (mS)

#define   RECORD_T        (100)                 // ��¼��� (mS)

#define     MAX_DOT         (5)         // �������С����      15-1203
#define     VI_DOT          (3)         // Ĭ�ϵ����ѹ3λС��(S16)
#define     TI_DOT          (2)         // Ĭ�ϵ����¶�2λС��(S16)
//--------------------------------------------------
#define     DEF_TMP_FLOAT   (-273.15)   // Ĭ���¶ȣ����㣩 
//--------------------
//  С������ر�� 15-1203
//--------------------
// ���� -> 5λС�� �����ѹ�뵥���¶�����
const unsigned int  DOT_DIV_TBL[MAX_DOT+1]  = { 1, 10, 100, 1000, 10000, 100000 };              // ����ֵ����ֵ   
const int           DEF_TMP_INT[MAX_DOT+1]  = {-273,-2732,-27315,-273150,-2731500,-27315000 };  // Ĭ���¶ȣ�������  
//--------------------
//  (3)     �жϺ�����
#if     ((XCUR_JDG_T  <= 2*SCAN_PERIOD) || (XCUR_JDG_T  > MAX_JDG_T))
  #error    "'XCUR_JDG_T' is set error !"
#elif   ((XVOL_JDG_T  <= 2*SCAN_PERIOD) || (XVOL_JDG_T  > MAX_JDG_T))
  #error    "'XVOL_JDG_T' is set error !"
#elif   ((XTEMP_JDG_T <= 2*SCAN_PERIOD) || (XTEMP_JDG_T > MAX_JDG_T)) 
  #error    "'XTEMP_JDG_T' is set error !"
#elif   ((XVI_JDG_T   <= 2*SCAN_PERIOD) || (XVI_JDG_T   > MAX_JDG_T)) 
  #error    "'XVI_JDG_T' is set error !"
#elif   ((XTI_JDG_T   <= 2*SCAN_PERIOD) || (XTI_JDG_T   > MAX_JDG_T)) 
  #error    "'XTI_JDG_T' is set error !"
#elif   ((IO_JDG_T    <= 2*SCAN_PERIOD) || (IO_JDG_T    > MAX_JDG_T)) 
  #error    "'IO_JDG_T' is set error !"
#elif   ((VPOT_JDG_T  <= 2*SCAN_PERIOD) || (VPOT_JDG_T  > MAX_JDG_T)) 
  #error    "'VPOT_JDG_T' is set error !"
#elif   ((CUR_JDG_T   <= 2*SCAN_PERIOD) || (CUR_JDG_T   > MAX_JDG_T)) 
  #error    "'CUR_JDG_T' is set error !"
#elif   ((VOL_JDG_T   <= 2*SCAN_PERIOD) || (VOL_JDG_T   > MAX_JDG_T)) 
  #error    "'VOL_JDG_T' is set error !"
// #elif    ((TEMP_JDG_T <= 2*SCAN_PERIOD) || (TEMP_JDG_T > MAX_JDG_T)) 
//  #error    "'TEMP_JDG_T' is set error !"
#elif   ((POW_JDG_T   <= 2*SCAN_PERIOD) || (POW_JDG_T   > MAX_JDG_T)) 
  #error    "'POW_JDG_T' is set error !"
#elif   ((ENGY_JDG_T  <= 2*SCAN_PERIOD) || (ENGY_JDG_T  > MAX_JDG_T)) 
  #error    "'ENGY_JDG_T' is set error !"
#elif   ((VI_JDG_T    <= 2*SCAN_PERIOD) || (VI_JDG_T    > MAX_JDG_T)) 
  #error    "'VI_JDG_T' is set error !"
#elif   ((TI_JDG_T    <= 2*SCAN_PERIOD) || (TI_JDG_T    > MAX_JDG_T)) 
  #error    "'TI_JDG_T' is set error !"
#else

#endif
//----------------------------------------------------------------------------------------------------
//  �����豸���루MainChanType��
//----------
#define   MAIN_GENERAL          0x00  /* ͨ���豸   */

#define   MAIN_LI_FORMATER_A    0x01  /* ﮵绯���豸     */
#define   MAIN_LI_TESTER_A      0x02  /* ﮵�����豸     */

//----------
#define   MAIN_PB_TESTER_A      0x81  /* Ǧ����Ի���λ��   */

//----------------------------------------------------------------------------------------------------
//  �����豸���루SubChanType��
//----------
#define   SUB_GENERAL           0x00  /* ͨ���豸   */

#define   SUB_NEEDLE_PALLET_A   0x01  /* �봲����A: 
                                        96��·��3���봲��4����ÿ�㣬8���ÿ���̡�
                                        ÿ��(��)һ���¶ȿ������ɼ����ư壬��3���
                                        �¶ȿ������ɼ����ư壺32���¶ȣ�12�����룬2�����������7��ֱ�������
                                        �¶ȣ�1�����1���¶ȣ���32��������ʵʱ�����У�
                                        ���룺1�������źţ�8���������̵�λ�źţ�2�������̸�̽ͷ��1��������ͣ
                                        �����2�������������Ʒ����1��ֱ���������������С����ϡ��졢
                                        �ơ��̡�����ָʾ��6��ֱ����*/
//----------------------------------------------------------------------------------------------------
const   unsigned char   B8_MASK_P[8]    = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};
const   unsigned char   B8_MASK_N[8]    = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
//--------------------------------------------------
//const U8      U8MSK[8]        = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
const unsigned short    U16HMSK[8]      = {0x0000,0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700};
//----------------------------------------------------------------------------------------------------
//  �豸�Ĭ�������� 16-0125
#define     DEF_CHR_CUR         (10000.0L)      // Ĭ����������(mA)
#define     DEF_DCH_CUR         (10000.0L)      // Ĭ�����ŵ����(mA)
#define     DEF_CHR_VOL         (5000.0L)       // Ĭ��������ѹ(mV)
#define     DEF_DCH_VOL         (0.0L)          // Ĭ����С�ŵ��ѹ(mV)

#define     MAX_CHR_CUR         (20000000.0L)   // ������������(mA)
#define     MAX_DCH_CUR         (20000000.0L)   // �������ŵ����(mA)
#define     MAX_CHR_VOL         (1000000.0L)    // ����������ѹ(mV)
#define     MIN_DCH_VOL         (-100000.0L)    // ������С�ŵ��ѹ(mV)
//--------------------------------------------------
#define     RUN_FANON_TEMP      (25.0L)         // ������������¶�(��)
#define     PROT_FANON_TEMP     (65.0L)         // ������������¶�(��)

#define     MAX_FANON_TEMP      (200.0L)        // ����������¶�(��)
#define     MIN_FANON_TEMP      (-20.0L)        // ��С��������¶�(��)
//----------------------------------------------------------------------------------------------------
//  cDev.wmfunc_cfg ����ģʽ����λ���� ��0�����Σ�1��������15-1124
//----------
#define   WMFUNC_BIT_CCC        ((U32)(1 << 0))   // �������   ����λ
#define   WMFUNC_BIT_CVC        ((U32)(1 << 1))   // ��ѹ���   ����λ
#define   WMFUNC_BIT_CCD        ((U32)(1 << 2))   // �����ŵ�   ����λ
#define   WMFUNC_BIT_CVD        ((U32)(1 << 3))   // ��ѹ�ŵ�   ����λ
#define   WMFUNC_BIT_CPD        ((U32)(1 << 4))   // �㹦�ʷŵ�  ����λ
#define   WMFUNC_BIT_CPC        ((U32)(1 << 5))   // �㹦�ʳ��  ����λ
#define   WMFUNC_BIT_CRD        ((U32)(1 << 6))   // �����ŵ�  ����λ
#define   WMFUNC_BIT_STATIC     ((U32)(1 << 7))   // ���ã����ã� ����λ
#define   WMFUNC_BIT_PULSE_C    ((U32)(1 << 8))   // ������   ����λ
#define   WMFUNC_BIT_PULSE_D    ((U32)(1 << 9))   // ����ŵ�   ����λ
#define   WMFUNC_BIT_PULSE_S    ((U32)(1 <<10))   // ���徲��   ����λ
#define   WMFUNC_BIT_LINE_D     ((U32)(1 <<11))   // ���Էŵ�   ����λ
#define   WMFUNC_BIT_IF         ((U32)(1 <<12))   // �������ģʽ��  ����λ
#define   WMFUNC_BIT_ELSE       ((U32)(1 <<13))   // ������ģʽ��  ����λ
#define   WMFUNC_BIT_VAR_OPT    ((U32)(1 <<14))   // ��������   ����λ
#define   WMFUNC_BIT_CYC        ((U32)(1 <<15))   // ѭ��
#define   WMFUNC_BIT_PULSE_INT  ((U32)(1 <<16))   // ��Ъ����
#define   WMFUNC_BIT_PULSE_ON   ((U32)(1 <<17))   // ��������
#define   WMFUNC_BIT_EXT_CTRL   ((U32)(1 <<18))   // ���
#define   WMFUNC_BIT_CRC        ((U32)(1 <<19))   // ������

//----- Ĭ�Ͽ�������ģʽ -----
#define   DEFAULT_WMFUNC_BIT    ( WMFUNC_BIT_CCC      \
                                | WMFUNC_BIT_CVC      \
                                | WMFUNC_BIT_CCD      \
                                | WMFUNC_BIT_STATIC   \
                                | WMFUNC_BIT_IF       \
                                | WMFUNC_BIT_ELSE     \
                                | WMFUNC_BIT_VAR_OPT  \
                                )
//----- ��������λ���� -----   // 16-0505
typedef union   _unWmFunc_
{
    struct {
        U32 bCCC        : 1;
        U32 bCVC        : 1;
        U32 bCCD        : 1;
        U32 bCVD        : 1;
        U32 bCPD        : 1;
        U32 bCPC        : 1;
        U32 bCRD        : 1;
        U32 bSTATIC     : 1;
        U32 bPUL_C      : 1;
        U32 bPUL_D      : 1;
        U32 bPUL_S      : 1;
        U32 bLINE_D     : 1;
        U32 bIF         : 1;
        U32 bELSE       : 1;
        U32 bVAR_OPT    : 1;
        U32 bCYC        : 1;
        U32 bPUL_INT    : 1;
        U32 bPUL_PN     : 1;
        U32 bEXT_CTRL   : 1;
        U32 bCRC        : 1;
        U32 bREV        : 1;    // ����λ
    } bits;    
    U32 word;    
    
}   unWmFunc;
//----------------------------------------------------------------------------------------------------
//  cDev.exfunc_cfg �豸��չ����λ���� ��0�����Σ�1��������
//----------
#define   EXFUNC_BIT_IR         ((U32)(1 << 0))   // ������� ����λ
#define   EXFUNC_BIT_NOBEEP     ((U32)(1 << 1))   // ���η������� ����λ
#define   EXFUNC_BIT_INPLACE    ((U32)(1 << 2))   // ����λ��λ���� ����λ
#define   EXFUNC_BIT_AUTOLIFT   ((U32)(1 << 3))   // �������Զ�̧���봲 ����λ
#define   EXFUNC_BIT_WRUNLIFT   ((U32)(1 << 4))   // ����������̧���봲 ����λ
#define   EXFUNC_BIT_MANOUT     ((U32)(1 << 5))   // �˹����ƿ�������� ����λ

#define   EXFUNC_BIT_REV        ((U32)(1 << 6))   // ���� ����λ

//----- Ĭ�Ͽ�����չģʽ -----
#define   DEFAULT_EXFUNC_BIT     (0)

//----- ��������λ���� -----   // 16-0505
typedef union   _unExFunc_
{
    struct {
        U32 bIR         : 1;
        U32 bNOBEEP     : 1;
        U32 bINPLACE    : 1;
        U32 bAUTOLIFT   : 1;
        U32 bWRUNLIFT   : 1;
        U32 bMANOUT     : 1;
        U32 bREV        : 1;    // ����λ
     
    } bits;    
    U32 word;    
    
}   unExFunc;
//----------------------------------------------------------------------------------------------------
typedef struct _strCtrlCfg_     //14-0724 -> 76Bytes MAX:256Bytes
{
    U32 Mark;             // ���

    U8  bPdResumeMode;    // ����ָ���� Ĭ�ϣ�1 - ����
    U8  bStepSyncFlag;    // �׶�ͬ����� Ĭ�ϣ�1 - ����
    U8  bParallel;        // ��������� Ĭ�ϣ�0 - ��ֹ
    U8  bAbnResume;       // ���ϻָ���� Ĭ�ϣ�0 - ��ֹ

    U8  MaxViBdNum;       // ���Ѳ���ѹ������N/A
    U8  MaxTiBdNum;       // ���Ѳ���¶Ȱ�����N/A
    U8  MaxIiBdNum;       // ��󿪹������������N/A
    U8  MaxOiBdNum;       // ��󿪹������������N/A

    U8  MaxEBiBdNum;      // �����չλ���������N/A
    U8  MaxEBoBdNum;      // �����չλ���������N/A
    U8  MaxEDiBdNum;      // �����չ�������������N/A
    U8  MaxEDoBdNum;      // �����չ�������������N/A

    U8  BoardViNum;       // �弶Ѳ���ѹ��
    U8  BoardTiNum;       // �弶Ѳ���¶���
    U8  BoardIiNum;       // �弶������������
    U8  BoardOiNum;       // �弶�����������

    U8  BoardExBiNum;     // �弶��չλ������
    U8  BoardExBoNum;     // �弶��չλ�����
    U8  BoardExDiNum;     // �弶��չ����������
    U8  BoardExDoNum;     // �弶��չ���������

    U16 CanOff_n;         // CAN��ʱ���������ղ����κ�ͨ���İ���
    U16 CtrlRetry_n;      //�����ƹ���ʱ�����Դ���
    U16 CanRetry_n;       //��ͨ�Ź���ʱ�����Դ���
    U16 CtrlRetry_t;      //�����ƹ���ʱ�����Լ����mS��

    U16 CanRetry_t;       //��ͨ�Ź���ʱ�����Լ����mS��
    U16 PDResume_t;       // ����ָ�����  0 ��ʾ���ָ����������ָ�������ʱ�� ��mS��
//    U16 CtrlDT;           // ��������ʱ��
    U16 Vdelta;           // ����ת����ѹ���Ʋ�ֵ��mV���������·����Ƶ�ѹ = ��ֵ+ת����ѹ 15-0113
    U16 StartDly_t;       // ������ʱ��Ԫ

    U16 XCurJdg_t;        // �����쳣�ж�����   (mS)
    U16 XVolJdg_t;        // ��ѹ�쳣�ж�����   (mS)
    U16 XTempJdg_t;       // �¶��쳣�ж�����   (mS)
    U16 XViJdg_t;         // �����ѹ�쳣�ж����� (mS)

    U16 XTiJdg_t;         // �����¶��쳣�ж����� (mS)
    U16 IOJdg_t;          // ��������Ӧ�ж�����  (mS)
    U16 VpotJdg_t;        // ƽ̨��ѹ��Ӧ�ж����� (mS)
    U16 CurJdg_t;         // ���� �ж���Ӧ����  (mS)

    U16 VolJdg_t;         // ��ѹ �ж���Ӧ����  (mS)
    U16 PowJdg_t;         // ���� �ж���Ӧ����  (mS)
    U16 EngyJdg_t;        // ��ʱ �ж���Ӧ����  (mS)
    U16 ViJdg_t;          // �����ѹ �ж���Ӧ���� (mS)

    U16 TiJdg_t;          // �����¶� �ж���Ӧ���� (mS)
    U16 Record_t;         // Ĭ�ϼ�¼��� (mS)

    U8  Vi_Dot;           // �����ѹС��λ
    U8  Ti_Dot;           // �����¶�С��λ
    U8  bAutoReset;       // �Զ���λ��� Ĭ�ϣ�0 - ��ֹ
    U8  bMainSubOrder;    // ����CAN���� Ĭ�ϣ�0 - CAN1����CAN2��

    U8  MainChanType;     // �����豸���� Ĭ�ϣ�0 - ͨ��
    U8  SubChanType;      // �����豸���� Ĭ�ϣ�0 - ͨ��

    U16 crc;              // CRCЧ���

}   strCtrlCfg;
//----------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// ����ͨ�����ñ�
typedef struct _strSubChCfg_    // MAX:256Bytes
{
    U32   Mark;         // ���

    U16   Rev;
    U16   crc;          // CRCЧ���

} strSubChCfg;
//------------------------------------------------------------------------------------------
// �����ѹӲ�����ñ�(��λ������)
typedef struct _strViHdCfg_     // 48bytes
{
    U32   Mark;                 // ���
    U8    HdTbl[MAX_VI_BOARD];  // ��������
    U16   UseNum;               // ���ð��� ����<MAX_OI_BOARD
    U16   crc;                  // CRCЧ���

} strViHdCfg;
//------------------------------
// �����ѹӲ��������(�������)
typedef struct _strViHdChk_
{
    U16   GblInx[MAX_VI_BOARD]; // ȫ������

} strViHdChk;
//------------------------------------------------------------------------------------------
// �����¶�Ӳ�����ñ�(��λ������)
typedef struct _strTiHdCfg_     // 48bytes
{
    U32   Mark;                 // ���
    U8    HdTbl[MAX_TI_BOARD];  // ��������
    U16   UseNum;               // ���ð��� ����<MAX_TI_BOARD
    U16   crc;                  // CRCЧ���

} strTiHdCfg;
//------------------------------
// �����¶�Ӳ��������(�������)
typedef struct _strTiHdChk_
{
    U16   GblInx[MAX_TI_BOARD]; // ȫ������

} strTiHdChk;
//------------------------------------------------------------------------------------------
// ��������Ӳ�����ñ�(��λ������)
typedef struct _strIiHdCfg_     // 28bytes
{
    U32   Mark;                 // ���
    U8    HdTbl[MAX_II_BOARD];  // ��������
    U16   UseNum;               // ���ð��� ����<MAX_II_BOARD
    U16   crc;                  // CRCЧ���

} strIiHdCfg;
//------------------------------
// ��������Ӳ��������(�������)
typedef struct _strIiHdChk_
{
    U16   GblInx[MAX_II_BOARD]; // ȫ������

} strIiHdChk;
//------------------------------------------------------------------------------------------
// ��������Ӳ�����ñ�(��λ������)
typedef struct _strOiHdCfg_     // 28bytes
{
    U32   Mark;                 // ���
    U8    HdTbl[MAX_OI_BOARD];  // ��������
    U16   UseNum;               // ���ð��� ����<MAX_OI_BOARD
    U16   crc;                  // CRCЧ���

} strOiHdCfg;
//------------------------------
// ��������Ӳ��������(�������)
typedef struct _strOiHdChk_
{
    U16   GblInx[MAX_OI_BOARD]; // ȫ������

} strOiHdChk;
//------------------------------------------------------------------------------------------
// ����ͨ��Ӳ�����ñ�(��λ������)
typedef struct _strXiHdCfg_     // 28bytes(֧�ֵ�20·ʱ)
{
    U32   Mark;                 // ���
    U8    HdTbl[MAX_XI_BOARD];  // ��������
    U16   UseNum;               // ���ð��� ����<MAX_OI_BOARD
    U16   crc;                  // CRCЧ���

} strXiHdCfg;
//------------------------------
// ����ͨ��Ӳ��������(�������)
typedef struct _strXiHdChk_
{
    U16   GblInx[MAX_XI_BOARD];   // ȫ������

} strXiHdChk;
//----------------------------------------------------------------------------------------------------
#define     DEVID_LEN       (32)        // �豸ID����   ���ֽڣ�
#define     DEVNAME_LEN     (32)        // �豸���Ƴ���   ���ֽڣ�
#define     CMDVERTBL_LEN   (256)       // ����汾����  ���ֽڣ�
#define     DEVSTR_LEN      (6)         // �豸�ṹ����  ���ֽڣ�
#define     SCFGTBL_LEN     (96)        // ����ͨ�������ñ���   ���ֽڣ�
#define     XITEMTBL_LEN    (32)        // ʵʩ��ϸ���������ñ��� ���ֽڣ�
//--------------------------------------------------------------------------
// �豸��Ϣ Ver 1.0 128�ֽ�
// 15-1112��������ʵʱ��ϸ���������ñ��汾Ver 1.1
// 16-0125������ͨ�����������ã��汾Ver 1.2 [176�ֽ�]
//--------------------------------------------------------------------------
// V1.2
typedef struct _strDevInfo_ //MAX:512Bytes
{
            U32         mark;
/*   0*/    U8          day;
/*   1*/    U8          mon;
/*   2*/    U16         year;                   // ��������
/*   4*/    U8          dev_id[DEVID_LEN];      // �豸��ID���ͻ����壩[32]
/*  36*/    U8          dev_name[DEVNAME_LEN];  // �豸���ƣ����Ҷ��壩[32]
/*  68*/    U16         soft_ver;               // ��λ������汾�ţ�ֻ����
/*  70*/    U16         hard_ver;               // ��λ��Ӳ���汾�ţ���д��
    
/*  72*/    U8          max_node_num;           // ���ڵ��� ����λ����Դ����ֻ�� == MAX_NODE_NUM��
/*  73*/    U8          max_chan_num;           // ���ͨ���� ��ͨ�����Ա����ֻ�� == MAX_CHAN_NUM��
/*  74*/    U8          chan_num;               // ����ͨ���� ���û����棨ʵʱ���ݣ����գ�У׼����أ���
/*  75*/    U8          par_node_num;           // �����ڵ��� �����Բ���������С�����ƣ�������ֹʱ == 0��
    
/*  76*/    U8          ext_chan_num;           // ��չͨ���� 16-0129
/*  77*/    U8          dev_str_type;           // �豸�ṹ����
/*  78*/    U8          dev_str_tbl[DEVSTR_LEN];// �豸�ṹ���ñ�[6]

/*  84*/    float       max_chr_cur;            // ��������
/*  88*/    float       max_chr_vol;            // ������ѹ
/*  92*/    float       max_chr_pow;            // ����繦��
/*  96*/    float       max_dch_cur;            // ���ŵ����
/* 100*/    float       min_dch_vol;            // ��С�ŵ��ѹ
/* 104*/    float       max_dch_pow;            // ���ŵ繦��
/* 108*/    Union32     run_fan_temp;           // ���з�������¶ȣ�0xffffffff��ʾ������
/* 112*/    Union32     prt_fan_temp;           // ������������¶ȣ�0xffffffff��ʾ���գ�

/* 116*/    unWmFunc    wmfunc_cfg;             // ����ģʽ���ܿ��� 16-0505
/* 120*/    unExFunc    exfunc_cfg;             // �豸��չ���ܿ��� 16-0505
/* 124*/    U32         min_rec_time;           // ��С��¼ʱ��(uS)

/* 128*/    U16         vi_num;                 // ͨ����ѹ��
/* 130*/    U16         ti_num;                 // ͨ���¶���
/* 132*/    U8          ii_num;                 // ͨ������������
/* 133*/    U8          oi_num;                 // ͨ������������
/* 134*/    U16         xitem_num;              // ������������ʱ֧�����16�
/* 136*/    U8          xitem_tbl[XITEMTBL_LEN];// ���������ñ�[32]
                    
/* 168*/    U8          rev[2];                 // ��������
/* 170*/    U16         crc;                    // crc16 MODBUS-RTU У��
        
} strDevInfo;

// V1.1
//typedef struct _strDevInfo_ //MAX:512Bytes
//{
//            U32        mark;
///*   0*/    U8         day;
///*   1*/    U8         mon;
///*   2*/    U16        year;                    // ��������

///*   4*/    U8         dev_id[DEVID_LEN];       // �豸��ID�����Ҷ��壩
///*  36*/    U8         dev_name[DEVNAME_LEN];   // �豸���ƣ��ͻ����壩

///*  68*/    U16        soft_ver;                // ��λ������汾�ţ�ֻ����
///*  70*/    U16        hard_ver;                // ��λ��Ӳ���汾�ţ���д��

/////*  72*/      U8         cmd_ver[CMDVERTBL_LEN];  // ͨ������汾��

///*  72*/    U8         chan_num;                // ͨ����
///*  73*/    U8         dev_str_type;            // �豸�ṹ����
///*  74*/    U8         dev_str_tbl[DEVSTR_LEN]; // �豸�ṹ���ñ�

///*  80*/    float      max_chr_cur;             // ��������
///*  84*/    float      max_chr_vol;             // ������ѹ
///*  88*/    float      max_chr_pow;             // ����繦��
///*  92*/    float      max_dch_cur;             // ���ŵ����
///*  96*/    float      min_dch_vol;             // ��С�ŵ��ѹ
///* 100*/    float      max_dch_pow;             // ���ŵ繦��
///* 104*/    Union32    run_fan_temp;            // ���з�������¶ȣ�0xffffffff��ʾ������
///* 108*/    Union32    prt_fan_temp;            // ������������¶ȣ�0xffffffff��ʾ���գ�

///* 112*/    U32        wmfunc_cfg;              // ����ģʽ���ܿ���
///* 116*/    U32        exfunc_cfg;              // �豸��չ���ܿ���
///* 120*/    U32        min_rec_time;            // ��С��¼ʱ��(uS)

/////* 372*/      U8         host_ip[4];              // ����IP 0x84����޸�
/////* 376*/      U16        host_port;               // �����˿� 0x84����޸�
/////* 378*/      U8         dev_mac[6];              // �豸MAC 0x84����޸�

/////* 384*/      U16        vi_soft_tbl[SCFGTBL_LEN];    // ͨ����ѹ     ������ñ�
/////* 576*/      U16        ti_soft_tbl[SCFGTBL_LEN];    // ͨ���¶�     ������ñ�
/////* 768*/      U8         ii_soft_tbl[SCFGTBL_LEN];    // ͨ����������   ������ñ�
/////* 864*/      U8         oi_soft_tbl[SCFGTBL_LEN];    // ͨ����������   ������ñ�
/////* 960*/      U8         xi_soft_tbl[SCFGTBL_LEN];    // ͨ����չ��    ������ñ�

//            U16        vi_num;                  // ͨ����ѹ��
//            U16        ti_num;                  // ͨ���¶���
//            U8         ii_num;                  // ͨ������������
//            U8         oi_num;                  // ͨ������������
//            U16        xitem_num;               // ������������ʱ֧�����16�
//            U8         xitem_tbl[XITEMTBL_LEN]; // ���������ñ�
//                    
//            U8         rev[2];                  // ��������
//            U16        crc;                     // crc16 MODBUS-RTU У��
//        
//} strDevInfo;

//----------------------------------------------------------------------------------------------------
//  Ĭ����ϸ����������� 
//----------------------------------------------------------------------------------------------------
#define     DEF_XITEM_NUM       (0)         // Ĭ������������
//#define       MAX_XITEM_NUM       (16)        // �������������

#ifdef      _IN_DEV_CPP
    const unsigned char DEF_XITEM_TBL[XITEMTBL_LEN] =
    {
/*      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,         */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x0X */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  /* 0x1X */
    };
#else

#endif

//----------------------------------------------------------------------------------------------------
//�¶ȷ����(�°�)
typedef struct _strTempTbl_
{
    U32     Mark;
    U16     tbl[MAX_CHAN_NUM][MAX_CHANTI_NUM];
    U16     rev1;
    U16     crc;

} strTempTbl;
//----------------------------------------------------------------------------------------------------
//��ͨ����Ӧ�ĸ�������ͨ�����ñ�(����������)
/*
    ͬһ�����а��������ӳ�䵽��������,Ĭ�ϸ����������������
*/
typedef struct  _strSampTbl_
{
    U16     ViOffset;   //�����ѹ��ƫ��
    U16     ViNum;      //�����ѹ��

    U16     TiOffset;   //�����¶Ⱥ�ƫ��
    U16     TiNum;      //�����¶���

    U16     IiOffset;   //����������ƫ��
    U16     IiNum;      //������������

    U16     OiOffset;   //���������ƫ��
    U16     OiNum;      //�����������
        
    U16     XiOffset;   //������չ����ƫ��
    U16     XiNum;      //������չ������

}   strSampTbl;
//------------------------------------------------------------
typedef struct  _strSampCfg_    // maxsize: 8 + 128*20 = 2568(0xA08) bytes
{
    U32         Mark;
    strSampTbl  ch[MAX_CHAN_NUM];   // num * 20bytes: 96ch->0x780 bytes
    U16         rev;
    U16         crc;

}   strSampCfg;
//----------------------------------------------------------------------------------------------------
//------------------------------------------------------------
//��������
typedef struct  _strHostCtrl_   // 12bytes
{
    U16     flag;       // ���
    U16     port;       // �����˿�
    U8      ip[4];      // ������ַ
    U16     rev;
    U16     crc;

}   strHostCtrl;
//------------------------------------------------------------
//PC�ӻ�����
typedef struct  _strSlaveCtrl_
{
    U8      flag;       // ע����
    U8      ch;         // ʵʱ�ϴ�����ͨ������
    U8      ip[4];      // ע���ַ
    U16     tout;       // ͨ�ų�ʱ����

}   strPCSlaCtrl;
//------------------------------------------------------------
//����������Ϣ
typedef struct _strLocalM_  // 36 bytes
{
    U32     Mark;       // ���
    U8      Ip[4];      // ����IP��ַ
    U8      GW[4];      // Ĭ������IP��ַ
    U8      Mask[4];    // ��������
    U8      DNS1[4];    // ��ѡDNS������IP��ַ
    U8      DNS2[4];    // ����DNS������IP��ַ
    U8      MAC[8];     // MAC��ַ��8����֤�ֶ��롿
    U16     Port;       // �˿�
    U16     crc;

} strLocalM;
//------------------------------------------------------------
//change with 'NETINFO_MARK'
static const strLocalM  DEFAULT_NETINFO =
{
    NETINFO_MARK,       // ���
    {192,168,0,195},    // ����IP��ַ
    {192,168,0,254},    // Ĭ������IP��ַ
    {255,255,255,0},    // ��������
    {0,0,0,0},          // ��ѡDNS������IP��ַ
    {0,0,0,0},          // ����DNS������IP��ַ
    {0x1e,0x1f,0x20,0x21,0x22,0x23,0x00,0x00},  // MAC��ַ
    1001,               // �˿ں�
    0x0000              // ����
};

//----------------------------------------------------------------------------------------------------
//  ʵʱ��ϸ���ݷ��ʹ���
//----------------------------------------------------------------------------------------------------
typedef struct _strDataTxCtrl_ 
{
    unsigned char   realCh;                 // ʵʱ������ʼѭ��ɨ��ͨ���ţ���ͨ���������UDP���޷�һ�δ���ʱ��
    unsigned char   recCh;                  // �ϴ���¼��ʼѭ��ɨ��ͨ����
    unsigned char   repeatNum;              // �����������ظ�����
    unsigned short  packLen[MAX_CHAN_NUM];  // ʵʱ����ϸ������(��ǰ����)
    unsigned short  recNum[MAX_CHAN_NUM];   // �����м�¼(δ��ȡ)����
    
}   strDataTxCtrl;    

//---------------------------------------------------------------------------------------------------- 
//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
//----------------------------------------------------------------------------------------------------
//��ṹ����
//----------------------------------------------------------------------------------------------------
//������Ի�ͨ����
//----------------------------------------------------------------------------------------------------
class Dev
{
private:
    U8              mNo;          // �豸������ 
    strDevInfo      mInfoBak;     // �豸��Ϣ����

public:
    strDevInfo      mInfo;        // �豸��Ϣ
    strCtrlCfg      mCtrlCfg;     // ���Ʋ�������
    strSubChCfg     mSubChCfg;    // ����ͨ��Ӳ��������
    strViHdCfg      mViHdCfg;     // �����ѹӲ�����ã����豸�ṹ��ϣ�δ��ʼ��16-0514
    strViHdChk      mViHdChk;     // �����ѹӲ������
    strTiHdCfg      mTiHdCfg;     // �����¶�Ӳ�����ã����豸�ṹ��ϣ�16-0514
    strTiHdChk      mTiHdChk;     // �����¶�Ӳ������
    strIiHdCfg      mIiHdCfg;     // ��������Ӳ�����ã����豸�ṹ��ϣ�16-0514
    strIiHdChk      mIiHdChk;     // ��������Ӳ������
    strOiHdCfg      mOiHdCfg;     // ��������Ӳ�����ã����豸�ṹ��ϣ�16-0514
    strOiHdChk      mOiHdChk;     // ��������Ӳ������
    strXiHdCfg      mXiHdCfg;     // ��������ͨ��Ӳ������
    strXiHdChk      mXiHdChk;     // ��������ͨ��Ӳ������

    strSampCfg      mSampCfg;       // ����ͨ��������ñ�
//      strTempTbl      mTempTbl;     // �¶����ñ� 150925
    strHostCtrl     mHostCtrl;      // ��������
    strPCSlaCtrl    mPCSlaCtrl[MAX_SLAVE_PC]; // PC�ӻ�����
    strLocalM       mNetInfo;       // ����������Ϣ
    U32             mCUI_Mark;      // ��ѹ����ͨ����־λ(���32��)
    U32             mGblFault;      // ȫ�ֹ���
    strDataTxCtrl   mTxCtrl;
    
    // ʹ�������̶��ĸ���ʵʱ��ϸ���� 15-1122
    U16         SavePackLen;        // ��ǰ��¼������¼���棩����
    U16         MaxSavePack;        // ���屣�������¼��
    U16         LoadPackLen;        // �ϴ����ݣ�ʵʱ����ϸ������
    U16         MaxLoadPack;        // ����ϴ�������

public:
    Dev();

    inline  U32   Get_CUI_Mark(void) {return (mCUI_Mark);};
    inline  void  Set_CUI_Mark(U32 val) {mCUI_Mark = val;};

//    U8      LoadCfg(U8 type);                       // ��������
//    U16     CopyCfg(U8 type, U8 *);                 // ��������
//    U8      SaveCfg(U8 type, U8 *);                 // ��������
//    U8      CheckCfg(U8 type, const strCtrlCfg *);  // �������

    U16     Rd_DevInfo(U8 *, U8);                   // ��ȡ�豸��Ϣ
    U8      Wr_DevInfo(U8 *);                       // �޸��豸��Ϣ
    U8      LoadDevInfo(void);                      // ��MRAM�����豸��Ϣ
    U8      SaveDevInfo(void);                      // �����豸��Ϣ��MRAM
    U8      CheckDevInfo(const strDevInfo *);       // ����豸��Ϣ�Ϸ���
    void    PackLenSetProc(void);                   // ʵʱ��ϸ���ݰ���������

    void    InitHostCtrl(void);                     // ��ʼ��������Ϣ
    U8      LoadHostCtrl(void);                     // ����������Ϣ
    U8      SaveHostCtrl(void);                     // ����������Ϣ
    U8      CheckHostCtrl(const strHostCtrl *);     // ������Ϣ���

    U8      LoadCtrlCfg(void);                      // ���ؿ�������
    U16     CopyCtrlCfg(U8 *);                      // ��ȡ��������
    U8      SaveCtrlCfg(U8 *);                      // �����������
    U8      CheckCtrlCfg(const strCtrlCfg *);       // ����������

    U8      LoadSubChCfg(void);                     // ���ظ���ͨ��Ӳ������
    U16     CopySubHdCfg(U8 *, U8);                 // ��ȡ����ͨ��Ӳ������
    U8      SaveSubHdCfg(U8 *, U8);                 // ���渨��ͨ������
    U8      CheckSubChCfg(const strSubChCfg *);     // ��鸨��ͨ������

    U8      LoadSchHdCfg(U8 type);                  // ����ָ������ͨ������
    U16     CopySchHdCfg(U8 type, U8 *);            // ����ָ������ͨ������
    U8      SaveSchHdCfg(U8 type, U8 *);            // ����ָ������ͨ������
    U8      CheckSchHdCfg(U8 type, const U8 *);     // ���ָ������ͨ������

    U8      LoadViHdCfg(void);                      // ���ص����ѹӲ����
    U16     CopyViHdCfg(U8 *);                      // ��ȡ�����ѹӲ����
    U8      SaveViHdCfg(U8 *);                      // ���浥���ѹӲ����
    U8      CheckViHdCfg(const strViHdCfg *);       // ��鵥���ѹӲ����

    U8      LoadTiHdCfg(void);                      // ���ص����¶�Ӳ����
    U16     CopyTiHdCfg(U8 *);                      // ��ȡ�����¶�Ӳ����
    U8      SaveTiHdCfg(U8 *);                      // ���浥���¶�Ӳ����
    U8      CheckTiHdCfg(const strTiHdCfg *);       // ��鵥���¶�Ӳ����

    U8      LoadIiHdCfg(void);                      // ���ؿ�������Ӳ����
    U16     CopyIiHdCfg(U8 *);                      // ��ȡ��������Ӳ����
    U8      SaveIiHdCfg(U8 *);                      // ���濪������Ӳ����
    U8      CheckIiHdCfg(const strIiHdCfg *);       // ��鿪������Ӳ����

    U8      LoadOiHdCfg(void);                      // ���ؿ�������Ӳ����
    U16     CopyOiHdCfg(U8 *);                      // ��ȡ��������Ӳ����
    U8      SaveOiHdCfg(U8 *);                      // ���濪������Ӳ����
    U8      CheckOiHdCfg(const strOiHdCfg *);       // ��鿪������Ӳ����

    U8      LoadSampCfg(void);                      // ���ظ���ͨ��������
    U16     CopySampCfg(U8 *);                      // ��ȡ����ͨ��������
    U8      SaveSampCfg(U8 *);                      // ���渨��ͨ��������
    U8      CheckSampTbl(const strSampTbl *, U8);   // ��鸨��ͨ�������� 15 -0119

//      U8    LoadTempTbl(void);                        // �����¶�����
//      U16   CopyTempTbl(U8 *);                        // ��ȡ�¶�����
//      U8    SaveTempTbl(U8 *);                        // �����¶�����
//      U8    CheckTempTbl(const strTempTbl *);         // ����¶�����

    U8    LoadNetInfo(void);                        // �����������Ϣ
    void  ReflashNetCfg(void);                      // ˢ����������
    U8    CheckNetInfo(const strLocalM *);          // ����������Ϣ�Ϸ���

    U8    Test_AnyChRunning(void);                  // �������ͨ�����Ƿ������е�ͨ��
    U8    SetMACIP(U8 *ptr);                        // ����IP���豸MAC(0x89)
    U16   GetMACIP(U8 *ptr);                        // ��ȡIP���豸MAC(0x89)

    void  Init_RealRecCtrl(void);                   // ʵʱ����¼�����շ����Ƴ�ʼ��

    U16   SetUIMark(U8 *ptr);                       // ��ȡ��ѹ�����ź�
    U16   RD_HIP_SMAC_Proc(U8 *, U8);               // ������IP���豸MAC(0x8B)

    void  GetRTC(void);                             // ��ȡʵʱʱ��
    U16   Test_RemotePC(const U8 *);                // Զ��PC - ���ӻ��ж�
    U8    EmptyPCSlaCtrl(U8);                       // ���PC�ӻ����ƻ���
    void  PCSlaTout(void);                          // PC�ӻ�ͨ�ų�ʱ����

    void  SetGblFault(U32);                         // ȫ�ֹ�����λ
    void  RstGblFault(U32);                         // ȫ�ֹ��ϸ�λ
    U32   GetGblFault(void) const;                  // ��ȡȫ�ֹ�����Ϣ

};
//----------------------------------------------------------------------------------------------------
dev_ext     Dev     cDev;       // ���� -> �豸

//----------------------------------------------------------------------------------------------------
#endif  // #ifdef   __cplusplus
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
    extern "C" 
    {
#endif
//--------------------------------------------------
dev_ext     U8      TstDevExfun(U32);
dev_ext     void    SRAM_Test(void);
dev_ext     void    IP_Reset_Test(void);    // 16-0602
dev_ext     void    Init_Dev(void);


//--------------------------------------------------
#ifdef      __cplusplus
    }
#endif
//----------------------------------------------------------------------------------------------------
#endif
