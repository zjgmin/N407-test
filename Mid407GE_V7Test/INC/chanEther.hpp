//----------------------------------------------------------------------------------------------------
#ifndef __CHANETHER_HPP
    #define __CHANETHER_HPP
//----------------------------------------------------------------------------------------------------
//C++类定义
//-------------------------------------------------------
#ifdef      _IN_CHANETHER_CPP
    #define     chanether_ext
#else
    #define     chanether_ext   extern
#endif
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
////  校准点数据结构
//typedef struct  _strCalibPoint_
//{
//    unsigned char   mark;   // 校准标记  0
//    unsigned char   type;   //
//    
//    unsigned char   year;   // 年 0
//    unsigned char   month;  // 月 2
//    unsigned char   day;    // 日 3
//    unsigned char   hour;   // 时 4
//    unsigned char   min;    // 分 5
//    unsigned char   sec;    // 秒 6

//    int             sample; // 测量值  8
//    int             actual; // 真实值  12

//    unsigned short  Crc16;
//    
//} strCalibPoint;
////---------------------------------------
////  数据校准表
//typedef struct  _strCalibTbl_
//{
//    unsigned char   num;        // 校准点数(最后一次校准设定的点数)
//    strCalibPoint   point[16];  // 校准数据表
//    unsigned short  Crc16;
//    
//} strCalibTbl;

////---------------------------------------
////  中位机保存的单点校准信息结构
//typedef struct  _strPointSave_
//{
//    int             sample;     // 测量值  8
//    int             actual;     // 真实值  12
//    unsigned char   year;       // 年    0
//    unsigned char   month;      // 月    1
//    unsigned char   day;        // 日    2
//    unsigned char   hour;       // 时    3
//    unsigned char   min;        // 分    4
//    unsigned char   sec;        // 秒    5
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
    
