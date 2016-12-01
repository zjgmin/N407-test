//----------------------------------------------------------------------------------------------------
#include <string.h>
#include "RTL.h"
#include "stm32f4xx.h"
#include "main.h"
#include "Net_Config.h"
#include "app_config.h"
#include "applayer.h"
#include "LED.h"
//----------------------------------------------------------------------------------------------------
// typedef struct localm {   /* << Local Machine info >>                */
//   U8  IpAdr[4];           /* Local IP Address                        */
//   U8  DefGW[4];           /* Default GateWay                         */
//   U8  NetMask[4];         /* Net Mask                                */
//   U8  PriDNS[4];          /* Primary DNS Server                      */
//   U8  SecDNS[4];          /* Secondary DNS Server                    */
// } LOCALM;
//----------------------------------------------------------------------------------------------------
//����MAC��ַ
U8  own_hw_adr[6] = {0x1e,0x1f,0x20,0x21,0x22,0x23};
//����IP��ַ
LOCALM  nlocalm[3] = 
{
    /* Ethernet: Local IP address */
    {
        {192,168,0,195},    //����IP
        {192,168,0,254},    /* Default Gateway IP address */
        {255,255,255,0},    /* Subnet IP Mask */
        {0,0,0,0},          /* Primary DNS Server IP */
        {0,0,0,0}           /* Secondary DNS Server IP */
    },
    {{192,168,0,196},{0,0,0,0},{255,255,255,0},{0,0,0,0},{0,0,0,0}},
    {{192,168,0,197},{0,0,0,0},{255,255,255,0},{0,0,0,0},{0,0,0,0}},
};
//----------------------------------------------------------------------------------------------------
U8  Rem_IP[4] = {192,168,0,111};//Զ��IP
U16 Rem_PORT;                   //Զ�̶˿�
U16 Loc_PORT;                   //���ض˿�
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//Udp ����
extern  U8  socket_udp;
        U8  RemIP_Test(U8 *pip);
/*--------------------------- UDP socket ------------------------------------*/
void    Udpproc_data (U16 len, U8 *ptr, const U8 *pip);
//----------------------------------------------------------------------------------------------------
U16 udp_callback (U8 soc, U8 *rip, U16 rport, U8 *buf, U16 len) 
{
//  U8  rem_attrib; //Զ�̻�����

    Rem_IP[0]   = rip[0];
    Rem_IP[1]   = rip[1];
    Rem_IP[2]   = rip[2];
    Rem_IP[3]   = rip[3];
    Rem_PORT    = rport;

    if (RemIP_Test(Rem_IP)) {   //IP��ַ����
        return (0);
    }

    if (soc != socket_udp) {
        /* Check if this is the socket we are connected to */
        return (0);
    }

    LED_Flash (LD9);    //�������ָʾ LD9

    Udpproc_data (len, buf, Rem_IP);

    return (0);
}
//----------------------------------------------------------------------------------------------------
strDataBuf BackBuf; // UDPӦ�����ݻ���
//----------------------------------------------------------------------------------------------------
//UDP���ݷ���
//----------------------------------------------------------------------------------------------------
void Udpproc_data (U16 len, u8 *ptr, const U8 *pip) 
{
    U8  *sendbuf;
    U8  ret = 0;
    U32 i;
//    strDataBuf  *buf_ptr = &BackBuf;

    if ((len>MAX_UDP_PACK_LEN)|(len==0)) {
        return;
    }

    // UDP  Recv
    BackBuf.dat_len = len;
    // ���������������г���
    BackBuf.max_len = MAX_UDP_PACK_LEN;
//    memcpy(&BackBuf.data[0], ptr, len);
    ret = App_L1_Process(&BackBuf, ptr, pip);
    gEtherTout = 0;     //�峬ʱ
    // UDP  Send
    if (ret == 0) {     // ��������ɹ�
        /* Start Connection */
        sendbuf = udp_get_buf (BackBuf.dat_len);
        for (i=0;i<BackBuf.dat_len;i++) {
            sendbuf[i]=BackBuf.data[i];
        }
        udp_send (socket_udp, Rem_IP, Rem_PORT, sendbuf, BackBuf.dat_len);
        LED_Flash (LD8);    //���緢��ָʾ LD8
    } else {
        // �����������
        App_Er_Process(&BackBuf, ret);
//        sendbuf = udp_get_buf (BackBuf.len);
//        for (i=0;i<BackBuf.len;i++) {
//            sendbuf[i]=BackBuf.data[i];
//        }
//        udp_send (socket_udp, Rem_IP, Rem_PORT, sendbuf, BackBuf.len);

    }
}
//----------------------------------------------------------------------------------------------------
//  IP��ַ����(����·�ɵ�������ô�죿��)
/*
    ����  ret :   1��������   0������ͨ��
*/
//----------------------------------------------------------------------------------------------------
U8  RemIP_Test(U8 *pip)
{
    U8  ret = 0;

    pip = pip;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
////Tcp ����
//extern U8 socket_tcp;
/*--------------------------- TCP socket ------------------------------------*/
//void Tcpsend_data (U16 len,U8 *ptr);
// U16 tcp_callback (U8 soc, U8 evt, U8 *ptr, U16 par) {
//   /* This function is called by the TCP module on TCP event */
//   /* Check the 'Net_Config.h' for possible events.          */
//   par = par;

//   if (soc != socket_tcp) {
//     return (0);
//   }

//   switch (evt) {
//     case TCP_EVT_DATA:
//       /* TCP data frame has arrived, data is located at *ptr, */
//       /* data length is par. Allocate buffer to send reply.   */
//       procrec(ptr);
//       Tcpsend_data (par,ptr);
//       break;

//     case TCP_EVT_CONREQ:
//       /* Remote peer requested connect, accept it */
//       /* 'ptr' points to Remote IP, 'par' holds the remote port. */
//       Rem_IP[1]  = ptr[1];Rem_IP[0]  = ptr[0];
//       Rem_IP[3]  = ptr[3];Rem_IP[2]  = ptr[2];
//       Rem_PORT = par;
//       /* Return 1 to accept connection, or 0 to reject connection */
//       return (1);

//     case TCP_EVT_CONNECT:
//       /* The TCP socket is connected */
//       return (1);
//   }
//   return (0);
// }
// /*--------------------------- TCP send --------------------------------------*/
// void Tcpsend_data (U16 len,u8 *ptr) {
//   U8 *sendbuf;
//   U32 i;
//   if(len>1000)len=1000;
//   /* TCP */
//   if (socket_tcp != 0) {
//     /* Start Connection */
//     switch (tcp_get_state(socket_tcp)) {
//       case TCP_STATE_FREE:
//       case TCP_STATE_CLOSED:
//         tcp_connect (socket_tcp, Rem_IP, Rem_PORT, Loc_PORT);
//         break;
//       case TCP_STATE_CONNECT:
//         if (tcp_check_send (socket_tcp) == __TRUE) {
//           sendbuf = tcp_get_buf (2);
//           for(i=0;i<2;i++)sendbuf[i]=ptr[i];
//           tcp_send (socket_tcp,sendbuf, 2);
//         }
//         break;
//     }
//   }
// }
//----------------------------------------------------------------------------------------------------
