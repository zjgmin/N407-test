//----------------------------------------------------------------------------------------------------
#ifndef __SUBCHAN_HPP
    #define __SUBCHAN_HPP

#include    "rtx_can.h"
#include    "Dev.hpp"
#include    "chan.hpp"
//----------------------------------------------------------------------------------------------------
//C++�ඨ��
//-------------------------------------------------------
#ifdef      _IN_SUBCHAN_CPP
    #define     subchan_ext
#else
    #define     subchan_ext extern
#endif
//----------------------------------------------------------------------------------------------------
//���ָ�������ͨ��CAN�̶���ʼ��ַ,��������
#define     VIADDR_OFFSET       (0x00)                              // ��ѹ���帨��ͨ����ʼCANƫ�Ƶ�ַ 1~40
#define     TIADDR_OFFSET       (VIADDR_OFFSET + MAX_VI_BOARD)      // �¶ȵ��帨��ͨ����ʼCANƫ�Ƶ�ַ 41~80
#define     IIADDR_OFFSET       (TIADDR_OFFSET + MAX_TI_BOARD)      // �������븨��ͨ����ʼCANƫ�Ƶ�ַ 81~100
#define     OIADDR_OFFSET       (IIADDR_OFFSET + MAX_II_BOARD)      // ������������ͨ����ʼCANƫ�Ƶ�ַ 101~120
#define     XIADDR_OFFSET       (OIADDR_OFFSET + MAX_OI_BOARD)      // ������չ����ͨ����ʼCANƫ�Ƶ�ַ 121~128

#define     EBIADDR_OFFSET      (0x80)                              // ��չλ���븨��ͨ����ʼCANƫ�Ƶ�ַ 129~
#define     EBOADDR_OFFSET      (EBIADDR_OFFSET + MAX_EBI_BOARD)    // ��չλ�������ͨ����ʼCANƫ�Ƶ�ַ
#define     EDIADDR_OFFSET      (EBOADDR_OFFSET + MAX_EBO_BOARD)    // ��չ�������븨��ͨ����ʼCANƫ�Ƶ�ַ
#define     EDOADDR_OFFSET      (EDIADDR_OFFSET + MAX_EDI_BOARD)    // ��չ�����������ͨ����ʼCANƫ�Ƶ�ַ
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
//----------------------------------------------------------------------------------------------------
//��ṹ����
//----------------------------------------------------------------------------------------------------
//���帨��ͨ���� 15-1119 ����ͨ��һһӳ��
//----------------------------------------------------------------------------------------------------
class SubChan //: public Chan
{
private:
    U8      mAddr;              // ����ͨ���� 

public:

    S16     Vi[MAX_VI_BOARD * MAX_BOARDVI_NUM];         // ӳ���ĵ����ѹ
    S16     Ti[MAX_TI_BOARD * MAX_BOARDTI_NUM];         // ӳ���ĵ����¶�
    U32     Ii[MAX_II_BOARD * MAX_BOARDII_NUM / 32];    // ӳ���Ŀ���������
    U32     Oi[MAX_OI_BOARD * MAX_BOARDOI_NUM / 32];    // ӳ���Ŀ��������
    U32     Xi[MAX_XI_BOARD * MAX_BOARDXI_NUM / 32];    // ӳ���������� ����32����չIO 15-1229

    S16     mMinVi[MAX_CHAN_NUM],
            mMaxVi[MAX_CHAN_NUM];                       // �����ѹ��ֵ��ͬ��ͨ����أ�  
    S16     mMinTi[MAX_CHAN_NUM], 
            mMaxTi[MAX_CHAN_NUM];                       // �����ѹ��ֵ��ͬ��ͨ����أ�  
public:

    SubChan();
    void    CanSendProc(CAN_msg *msg);                  // ������λ������ͨ������
    U16     SetIOData(U8 *ptr, U8 hver);                // ������������������
};
//--------------------------------------------------
subchan_ext     SubChan     cSubChan;   // ���� -> ����ͨ��

//--------------------------------------------------
#endif  // #ifdef   __cplusplus
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
#ifdef      __cplusplus
    extern "C" 
    {
#endif
//--------------------------------------------------
//subchan_ext 
const U32 B32IO_MASK[32] = 
{
    0x00000001, 0x00000003, 0x00000007, 0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff, 
    0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff,  
    0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff, 
    0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff
};


//--------------------------------------------------
#ifdef      __cplusplus
    }
#endif
//----------------------------------------------------------------------------------------------------
#endif
