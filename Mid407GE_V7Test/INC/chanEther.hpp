//----------------------------------------------------------------------------------------------------
#ifndef __CHANETHER_HPP
    #define __CHANETHER_HPP
//----------------------------------------------------------------------------------------------------
//C++�ඨ��
//-------------------------------------------------------
#ifdef      _IN_CHANETHER_CPP
    #define     chanether_ext
#else
    #define     chanether_ext   extern
#endif
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
////  У׼�����ݽṹ
//typedef struct  _strCalibPoint_
//{
//    unsigned char   mark;   // У׼���  0
//    unsigned char   type;   //
//    
//    unsigned char   year;   // �� 0
//    unsigned char   month;  // �� 2
//    unsigned char   day;    // �� 3
//    unsigned char   hour;   // ʱ 4
//    unsigned char   min;    // �� 5
//    unsigned char   sec;    // �� 6

//    int             sample; // ����ֵ  8
//    int             actual; // ��ʵֵ  12

//    unsigned short  Crc16;
//    
//} strCalibPoint;
////---------------------------------------
////  ����У׼��
//typedef struct  _strCalibTbl_
//{
//    unsigned char   num;        // У׼����(���һ��У׼�趨�ĵ���)
//    strCalibPoint   point[16];  // У׼���ݱ�
//    unsigned short  Crc16;
//    
//} strCalibTbl;

////---------------------------------------
////  ��λ������ĵ���У׼��Ϣ�ṹ
//typedef struct  _strPointSave_
//{
//    int             sample;     // ����ֵ  8
//    int             actual;     // ��ʵֵ  12
//    unsigned char   year;       // ��    0
//    unsigned char   month;      // ��    1
//    unsigned char   day;        // ��    2
//    unsigned char   hour;       // ʱ    3
//    unsigned char   min;        // ��    4
//    unsigned char   sec;        // ��    5
//    unsigned short  Crc16;
//    
//} strPointSave;
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
    extern "C" 
    {
#endif

// chanether_ext    ;   //

//--------------------------------------------------
#ifdef  __cplusplus
    }
#endif
//----------------------------------------------------------------------------------------------------
#endif  //eof
    
