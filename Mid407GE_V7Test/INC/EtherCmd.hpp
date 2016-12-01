//----------------------------------------------------------------------------------------------------
#ifndef __EtherCmd_Hpp
#define __EtherCmd_Hpp
//----------------------------------------------------------------------------------------------------
#ifdef  IN_ETHERCMD
    #define     ex_ethercmd extern "C"
#else  
    #define     ex_ethercmd extern
#endif
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//  ETHERNET����汾����

#define     DEFAULT_VER     (10)    // ͨ��Ĭ�ϰ汾

//----------------------------------------------------------------------------------------------------
//  ETHERNET����ķ���״̬���� 16-0129 ����

#define     RET_OK              (0x00)    // �ɹ�
#define     RET_FRAME_ERR       (0x01)    // �������ݴ���
#define     RET_VER_ERR         (0x02)    // �汾֧�ִ���
#define     RET_SEND_TOUT       (0x03)    // ��Ϣ���ͳ�ʱ
#define     RET_DATA_ERR        (0x04)    // �������ݴ���
#define     RET_RECV_TOUT       (0x05)    // Ӧ��ʱ
#define     RET_SYS_ERR         (0x06)    // ϵͳ����
#define     RET_ADDR_ABN        (0x07)    // ��ַ�쳣
#define     RET_OFFLINE         (0x08)    // ͨ���ѻ�
#define     RET_NOTMASTERPC     (0x09)    // ��PC����
#define     RET_SUBID_ERR       (0x0A)    // ����ͨ�����ʹ�
#define     RET_CHAN_RUNNING    (0x0B)    // ͨ�������� 16-0802

//----------------------------------------------------------------------------------------------------  
//----------------------------------------------------------------------------------------------------
ex_ethercmd     unsigned char   CMD_CONNECTION_Proc(unsigned char *, unsigned short *, const unsigned char *);
ex_ethercmd     unsigned char   CMD_RD_REALDATA_Proc(unsigned char *, unsigned short *, unsigned short, const unsigned char *);
ex_ethercmd     unsigned char   CMD_CIRCUIT_CTRL_Proc(unsigned char *, const unsigned char *);
ex_ethercmd     unsigned char   CMD_CIRCUIT_START_Proc(unsigned char *, const unsigned char *);
ex_ethercmd     unsigned char   CMD_MODIFY_DEVINFO_Proc(unsigned char *, const unsigned char *);
ex_ethercmd     unsigned char   CMD_RD_PRECISION_Proc(unsigned char *);
ex_ethercmd     unsigned char   CMD_CAL_PRECISION_Proc(unsigned char *);
ex_ethercmd     unsigned char   CMD_WR_PRECISION_Proc(unsigned char *);
ex_ethercmd     unsigned char   CMD_ALLOC_TEMP_Proc(unsigned char *, const unsigned char *);
ex_ethercmd     unsigned char   CMD_CFG_MAC_IP_Proc(unsigned char *, const unsigned char *);
ex_ethercmd     unsigned char   CMD_RD_DEV_MAC_IP_Proc(unsigned char *, unsigned short *);
ex_ethercmd     unsigned char   CMD_RD_HIP_SMAC_Proc(unsigned char *, unsigned short *, const unsigned char *);
ex_ethercmd     unsigned char   CMD_RQ_HOSTCTRL_Proc(unsigned char *, const unsigned char *);
ex_ethercmd     unsigned char   CMD_TEST_HARDWARE_Proc(unsigned char *, unsigned short *);
ex_ethercmd     unsigned char   CMD_RD_EEPROM_Proc(unsigned char *, unsigned short *);
ex_ethercmd     unsigned char   CMD_WR_EEPROM_Proc(unsigned char *, unsigned short *);
ex_ethercmd     unsigned char   CMD_Save_Calib_Proc(unsigned char *);
ex_ethercmd     unsigned char   CMD_RD_ChanInfo_Proc(unsigned char, unsigned char *, unsigned short *);         // �������,PC������Ϊ��������
ex_ethercmd     unsigned char   CMD_WR_ChanInfo_Proc(unsigned char, unsigned char *, const unsigned char *);    // �������,PC������Ϊ��������
ex_ethercmd     unsigned char   CMD_RD_CtrlCfg_Proc(unsigned char *, unsigned short *);
ex_ethercmd     unsigned char   CMD_WR_CtrlCfg_Proc(unsigned char *, const unsigned char *);
ex_ethercmd     unsigned char   CMD_RESET_Dev_Proc(unsigned char *, const unsigned char *);
ex_ethercmd     unsigned char   CMD_RD_SubChHCfg_Proc(unsigned char *, unsigned short *);
ex_ethercmd     unsigned char   CMD_WR_SubChHCfg_Proc(unsigned char *, const unsigned char *);
ex_ethercmd     unsigned char   CMD_RD_SubChSCfg_Proc(unsigned char *, unsigned short *);
ex_ethercmd     unsigned char   CMD_WR_SubChSCfg_Proc(unsigned char *, const unsigned char *);
ex_ethercmd     unsigned char   CMD_RD_SubChReal_Proc(unsigned char *, unsigned short *);
ex_ethercmd     unsigned char   CMD_WR_SubChReal_Proc(unsigned char *, const unsigned char *);
ex_ethercmd     unsigned char   CMD_RD_SubChInfo_Proc(unsigned char, unsigned char *, unsigned short *);        // �������,PC������Ϊ��������
ex_ethercmd     unsigned char   CMD_WR_SubChInfo_Proc(unsigned char, unsigned char *, const unsigned char *);   // �������,PC������Ϊ��������
ex_ethercmd     unsigned char   CMD_CommTransfer_Proc(unsigned char *, const unsigned char *);
ex_ethercmd     unsigned char   CMD_Transparent_Proc(unsigned char *, const unsigned char *);
//------------------------------
ex_ethercmd     unsigned char   Check_SubAddr(unsigned char, unsigned char);
//----------------------------------------------------------------------------------------------------
#endif /* __EtherCmd_Hpp */
