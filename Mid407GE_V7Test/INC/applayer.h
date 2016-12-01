//----------------------------------------------------------------------------------------------------
#ifndef __applayer_H
	#define __applayer_H

#include "app_config.h"
//----------------------------------------------------------------------------------------------------
#ifdef 	IN_APP_LAYER
	#define	 	ex_layer
#else  
	#define  	ex_layer  extern
#endif
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#define		L1_START_LB		0x00		// 插入层(L1)起始字段低字节
#define		L1_START_HB		0x01		// 插入层(L1)起始字段高字节
#define		L1_LEN_LB		0x02		// 插入层(L1)长度字段低字节
#define		L1_LEN_HB		0x03		// 插入层(L1)长度字段高字节 
#define		L1_DATA			0x04		// 插入层(L1)数据字段 
	
#define		L1_CRC_LB		0xFB		// 插入层(L1)校验字段低字节
#define		L1_CRC_HB		0xFC		// 插入层(L1)校验字段高字节 
#define		L1_STOP_LB		0xFD		// 插入层(L1)结束字段低字节
#define		L1_STOP_HB		0xFE		// 插入层(L1)结束字段高字节 
#define		L1_OK			0xFF		// 插入层(L1)数据接收解析成功
//--------------------
#define		L1_MAX_LEN		0x08		// 插入层(L1)数据包最大长度（2字节起始，4字节长度（考虑两字节扩展），2字节结束）
//----------------------------------------------------------------------------------------------------
#define		L2_FRAME_LEN	0x07		// 数据帧层(L2)有效帧长度
#define		L2_MAX_LEN		0x09		// 数据帧层(L2)数据包最大长度（含2字节CRC）
//--------------------	

//----------------------------------------------------------------------------------------------------
ex_layer	void 	App_Er_Process(strDataBuf *buf, U8 err_code);
ex_layer	U8		App_L1_Process(strDataBuf *buf, U8 *ptr, const U8 *pip);
ex_layer	U8 		App_L2_Process(strDataBuf *txbuf, const strDataBuf *rxbuf, U16 cutlen, const U8 *pip);
ex_layer	U16		App_L3_Process(strDataBuf *buf, U16 cutlen, const U8 *pip);
//ex_layer	U8		App_L3_Process(U8 *ptr, U16 *ret_len, U8 *pip);
ex_layer	U16		Crc16(U8 *ptr, U16 len);
//----------------------------------------------------------------------------------------------------
#endif	/* __applayer_H */
