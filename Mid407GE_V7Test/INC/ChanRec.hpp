//----------------------------------------------------------------------------------------------------
#ifndef __CHANREC_HPP
    #define __CHANREC_HPP

#include "rtl.h"
//----------------------------------------------------------------------------------------------------
//C++�ඨ��
//-------------------------------------------------------
#ifdef      _IN_CHANREC_CPP
    #define     chanrec_ext
#else
    #define     chanrec_ext extern
#endif
//----------------------------------------------------------------------------------------------------
//  ������¼�ṹ 15-1123 ���V1.1�汾��ʵʱ��ϸЭ�����������볤���ֶ�
typedef struct  _strVarRec_
{
    U32     Mark;       // ������¼��־��������0xffffffff��
    U16     Len;        // ����  15-1123   
    U32     V_Cyc1;     // ѭ��1
    U32     V_Cyc2;     // ѭ��2
    U16     V_Day;      // ����
    U32     V_Ms;       // ������
    float   V_Cap;      // ����
    U32     V_Cyc3;     // ѭ��3
    U32     V_Cyc4;     // ѭ��4
    U32     V_Cyc5;     // ѭ��5

} strVarRec;
//----------------------------------------------------------------------------------------------------
//  ���̼�¼�ṹ
typedef struct  _strProcRec_
{
    U32     P_CycNum;   //ѭ����
    U8      P_Mode;     //����ģʽ
    U8      P_AbnCode;  //������ͣ����
    U16     P_Stp;      //����
    U32     P_TotStp;   //�ۼƹ���
    U16     P_StpDay;   //����ʱ������
    U16     P_TotDay;   //��ʱ������
    U32     P_StpMs;    //����ʱ�����
    U32     P_TotMs;    //��ʱ�����

    float   P_Engy;     //����
    float   P_Cap;      //����
    float   P_Res;      //��̬����

//  U32     U32NULL;    //����

} strProcRec;
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#ifdef      __cplusplus
    extern "C" 
    {
#endif
//--------------------------------------------------
chanrec_ext void    Memset_16b(U16 *dst, U16 val, U32 len);
chanrec_ext void    Memcpy_16b(U16 *dst, U16 *src, U32 len);

//--------------------------------------------------
#ifdef      __cplusplus
    }
#endif
//----------------------------------------------------------------------------------------------------
#endif  //eof