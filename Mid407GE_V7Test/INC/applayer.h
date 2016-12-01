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
#define		L1_START_LB		0x00		// �����(L1)��ʼ�ֶε��ֽ�
#define		L1_START_HB		0x01		// �����(L1)��ʼ�ֶθ��ֽ�
#define		L1_LEN_LB		0x02		// �����(L1)�����ֶε��ֽ�
#define		L1_LEN_HB		0x03		// �����(L1)�����ֶθ��ֽ� 
#define		L1_DATA			0x04		// �����(L1)�����ֶ� 
	
#define		L1_CRC_LB		0xFB		// �����(L1)У���ֶε��ֽ�
#define		L1_CRC_HB		0xFC		// �����(L1)У���ֶθ��ֽ� 
#define		L1_STOP_LB		0xFD		// �����(L1)�����ֶε��ֽ�
#define		L1_STOP_HB		0xFE		// �����(L1)�����ֶθ��ֽ� 
#define		L1_OK			0xFF		// �����(L1)���ݽ��ս����ɹ�
//--------------------
#define		L1_MAX_LEN		0x08		// �����(L1)���ݰ���󳤶ȣ�2�ֽ���ʼ��4�ֽڳ��ȣ��������ֽ���չ����2�ֽڽ�����
//----------------------------------------------------------------------------------------------------
#define		L2_FRAME_LEN	0x07		// ����֡��(L2)��Ч֡����
#define		L2_MAX_LEN		0x09		// ����֡��(L2)���ݰ���󳤶ȣ���2�ֽ�CRC��
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
