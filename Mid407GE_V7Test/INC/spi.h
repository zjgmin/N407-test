//----------------------------------------------------------------------------------------------------
#ifndef __spi_H
#define __spi_H

#include "rtl.h"
//----------------------------------------------------------------------------------------------------
#ifdef  IN_SPI
    #define     ex_spi
#else  
    #define     ex_spi  extern 
#endif
//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
    extern "C" 
    {
#endif
//----------------------------------------------------------------------------------------------------
// MRAM_CHIP1   -> U4 in PCB
// MRAM_CHIP2   -> U5 in PCB
//----------------------------------------------------------------------------------------------------
/*  
    MRAM    ����    MR25H40     4Mbit     0x00000~0x80000
    MRAM    ����    MR25H10     1Mbit     0x00000~0x20000
    MRAM    ����    MR25H256    256Kbit   0x00000~0x08000
    EEPROM  ����    CAT25256    256Kbit   0x00000~0x08000
*/    
//--------------------
#ifdef  USE_256KBITS  // 256bits 32KBytes
//--------------------
    
  #define       DEVCFG_ADDR       0x0000    // �豸������ (mCtrlCfg)         0x0000~007f 256 bytes ʵ�ʣ�76
  #define       DEVINF_ADDR       0x0100    // �豸��Ϣ�� (mInfo)            0x0100~02ff 512 bytes ʵ�ʣ�128
      
  #define       SUBCHANCFG_ADDR   0x0300    // ����ͨ��Ӳ����Ϣ�� (mSubChCfg) 0x0300~03ff 256 bytes
  #define       SAMPTBLCFG_ADDR   0x0400    // ����ͨ��������Ϣ�� (mSampCfg)  0x0400~0eff 2816 bytes ʵ�ʣ�2568
      
  #define       NETINFO_ADDR      0x0F00    // ������Ϣ�� (mNetInfo)         0x0f00~0f7f 128 bytes ʵ�ʣ�36
  #define       HOSTINF_ADDR      0x0F80    // ������Ϣ�� (mHostCtrl)        0x0f80~0fff 128 bytes ʵ�ʣ�12
      
  #define       NODETBL_ADDR      0x1000    // ͨ��ӳ������� (mNodeTbl)    0x1000~19ff 2560 bytes ʵ�ʣ�2320

  #define       VIHDCFG_ADDR      0x1A00    // �����ѹӲ������(mViHdCfg)   0x1a00~1aff 256 bytes ʵ�ʣ�48
  #define       TIHDCFG_ADDR      0x1B00    // �����¶�Ӳ������(mTiHdCfg)   0x1b00~1bff 256 bytes ʵ�ʣ�48 
  #define       IIHDCFG_ADDR      0x1C00    // ��������Ӳ������(mIiHdCfg)   0x1c00~1cff 256 bytes ʵ�ʣ�28 
  #define       OIHDCFG_ADDR      0x1D00    // ��������Ӳ������(mOiHdCfg)   0x1d00~1dff 256 bytes ʵ�ʣ�28 
  #define       XIHDCFG_ADDR      0x1E00    // ��������Ӳ������(mXiHdCfg)   0x1e00~1eff 256 bytes ʵ�ʣ�28 

//--------------------
#else   // 1~4Mbits
//--------------------    

  #define       DEVCFG_ADDR       0x00000   // �豸������(mCtrlCfg) 0x0000~0x00ff
  #define       DEVINF_ADDR       0x00100   // �豸��Ϣ��(mInfo) 0x0100~0x02ff
      
  #define       SUBCHANCFG_ADDR   0x00300   // ����ͨ��Ӳ����Ϣ��(mSubChCfg) 0x0300~0x03ff
  #define       SAMPTBLCFG_ADDR   0x00400   // ����ͨ��������Ϣ��(mSampCfg) 0x0400~0x1808 ʵ��5128�ֽ�  -> 21pages  @256chans
      
  #define       NETINFO_ADDR      0x01900   // ������Ϣ��(mNetInfo) 0x1900~0x197f
  #define       HOSTINF_ADDR      0x01980   // ������Ϣ��(strHostCtrl) 0x1980~0x19ff 2015-0212
      
  #define       NODETBL_ADDR      0x02000   // ͨ��ӳ�������(mNodeTbl) 0x2000~0x4210 ʵ��8720�ֽ� -> 35pages @256chans

  #define       TEMPTBL_ADDR      0x04300   // ���ñ���Ϣ��(mTempTbl) 0x4300~0xbb08 ʵ��30728�ֽ� -> 121pages @256chans
      
  //--------------------
  #define       VIHDCFG_ADDR      0x0C000   // (mViHdCfg)
  #define       TIHDCFG_ADDR      0x10000   // (mTiHdCfg)
  #define       IIHDCFG_ADDR      0x14000   // (mIiHdCfg)
  #define       OIHDCFG_ADDR      0x18000   // (mOiHdCfg)
  #define       XIHDCFG_ADDR      0x1C000   // (mXiHdCfg)
  //--------------------

#endif
//----------------------------------------------------------------------------------------------------
ex_spi  void    SPI_Init (void);
ex_spi  void    SPI_CS(U8 chip_no, U8 stat);
ex_spi  void    SPI_HOLD(U8 chip_no, U8 stat);
ex_spi  U8      SPI_ByteRd(void);
ex_spi  U8      SPI_ByteWr(U8 data);
ex_spi  U8      SPI_RdSR(void);
ex_spi  void    SPI_WrSR(U8 sr_Val);
ex_spi  U8      Is_WrBusy(void);
ex_spi  U8      SPI_Rd(U8 *ptr, U32 addr, U16 len);
ex_spi  U8      SPI_Wr(U8 *ptr, U32 addr, U16 len);
//----------
ex_spi  void    MRAM_Init(void);
ex_spi  U8      MRAM_RdSR(U8 chip_no);
ex_spi  void    MRAM_WrSR(U8 chip_no, U8 data);
ex_spi  U8*     MRAM_Rd(U8 chip_no, U8 *ptr, U32 addr, U16 len);
ex_spi  U8      MRAM_Wr(U8 chip_no, U8 *ptr, U32 addr, U16 len);
//----------------------------------------------------------------------------------------------------
#ifdef  __cplusplus
    }
#endif
//----------------------------------------------------------------------------------------------------
#endif /* __spi_H */
