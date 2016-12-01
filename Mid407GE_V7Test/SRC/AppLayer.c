//----------------------------------------------------------------------------------------------------
// Header:      协议分层解析 --（1）插入协议
//                          --（2）数据帧协议
//                          --（3）电池测试数据协议 剥去设备号，解析具体命令
// File Name:   AppLayer.c
// Author:      David
// Date:        2015-02-13
//----------------------------------------------------------------------------------------------------
#define     IN_APP_LAYER
#include    "RTL.h"
#include    <string.h>
#include    "main.h"
#include    "app_config.h"
#include    "Led.h"
#include    "applayer.h"
#include    "EtherCmd.hpp"
//----------------------------------------------------------------------------------------------------
//  (0) -- 底层协议错误处理
//----------------------------------------------------------------------------------------------------
void App_Er_Process(strDataBuf *buf, U8 err_code)
{
    U16 len         = 0,    // 整包数据长度
        data_len    = 0;    // 命令数据部分长度

    switch(err_code) 
    {
    default:            // 未知错误
    case STA_ERR:       // 起始字错
    case STP_ERR:       // 结束字错
    case LEN_ERR:       // 长度错误
    case CRC_ERR:       // 校验错误
    case FORMAT_ERR:    // 数据格式错误
    case PACK_LEN_ERR:  // 封装后长度错误
    case CMD_ERR:       // 无效命令
        buf->data[0]    = 0x7d; // 数据头
        buf->data[1]    = 0x7b;

//        buf->data[2]    =     // 长度
//        buf->data[3]    =
        buf->data[4]    =       //
        data_len = data_len;

        buf->dat_len = len;     // 数据长度
        break;
    }

    LED_Flash(LD2);
    LED_Flash(LD3);
}
//----------------------------------------------------------------------------------------------------
//strDataBuf    L1_Buf;
strDataBuf  L1_RxBuf;   // 插入层接收数据缓冲,送上层（应用层方向）
strDataBuf  L1_TxBuf;   // 插入层发送数据缓冲,送下层（网络层方向）
//----------------------------------------------------------------------------------------------------
//  (1) -- 插入协议层处理  //  buf == BackBuf
/*
        15-0408 调整,数据部分出现'7d'需要校验
*/
//----------------------------------------------------------------------------------------------------
U8 App_L1_Process(strDataBuf *buf, U8 *ptr, const U8 *pip)
{
    U8  ret     = ANALYSIS_OK;
    U8  par_inx = L1_START_LB;          // 插入层解析状态
    U8  try_cnt = 0;                    // 应用层数据返回长度超出后重新解析次数
    U16 len_udp = buf->dat_len;         // udp包数据长度暂存
    U16 len_max = buf->max_len - 16;    // 最大数据部分长度（去掉头2、尾2、长度2+2、校验2+2、保留4 字节）暂存
    U16 len_pack;                       // 长度缓存，数据部分封装长度
    U16 crc_len = 0;    // crc数据部分长度（含0x7d）
    U16 crc_inx = 4;    // crc校验起始数据索引（ptr[crc_i]）
    U16 crc_buf = 0;    // crc备份
    U16 cut_len = 0;    // 应用层截去长度，用于封装后长度越界时，重新发应用层解析，应答长度调整用
    U32 i,j;

    //==========剥去第一层==========
    for (i=0; i<len_udp; i++) {
        if (par_inx == L1_OK) { // 接收结束
            break;
        }
        switch (par_inx)
        {
        default:
        case L1_START_LB:               // 寻找起始字段低，有则跳下一步
            if (ptr[i] == 0x7d) {
                par_inx = L1_START_HB;  // 指向起始字段高字节
            }
            break;
        case L1_START_HB:               // 寻找起始字段高，有则跳下一步
            switch (ptr[i])
            {
            default:
                par_inx = L1_START_LB;  // 回到起始字段低字节
                break;
            case 0x7b:                  // 收到完整起始段
                par_inx = L1_LEN_LB;    // 指向长度字段低字节
                break;
            case 0x7d:                  // 收到停止段，报错
                return  (STA_ERR);      // 起始字段错
            }
            break;
            
        case L1_LEN_LB:                 // 长度字段不计入crc
            if (ptr[i] == 0x7d) {       // 是0x7d，长度字段低需要调整
                if (ptr[i+1] == 0x82) {
                    i += 1;             // 数据索引调整+1
                    L1_TxBuf.dat_len = 0x7d;// 长度低字节暂存
                    par_inx = L1_LEN_HB;    // 指向长度字段高字节
                } else {
                    return  (LEN_ERR);      // 长度错
                }
            } else {
                L1_TxBuf.dat_len = ptr[i];  // 长度低字节暂存
                par_inx = L1_LEN_HB;        // 指向长度字段高字节
            }
            break;
        case L1_LEN_HB:
            if (ptr[i] == 0x7d) {           // 是0x7d，长度字段高溢出 0x7d00 = 32000 > 1500
                return  (LEN_ERR);          // 长度错！
            } else {
                L1_TxBuf.dat_len |= ptr[i]<<8;  // 加入长度高字节
                len_pack = L1_TxBuf.dat_len;    // 备份封装长度
                if (L1_TxBuf.dat_len > len_max) {
                    return  (LEN_ERR);          // 长度错！
                }
                j = 0;
                crc_len = 0;
                crc_inx = i + 1;            // crc校验起始索引
                par_inx = L1_DATA;          // 指向数据字段
            }
            break;
            
        case L1_DATA:
            if (ptr[i] == 0x7d) {           // 是0x7d，数据字段需要调整
                switch (ptr[i+1])           // 判断0x7d后字节的情况
                {
                default:
                case 0x7b:                  // 重复起始错
                case 0x7d:                  // 提前结束错
                    return  (DAT_ERR);      // 数据段解析错！
                
                case 0x82:
                    i++;                    // 数据索引调整+1
                    crc_len++;              // 数据段索引调整+1
                    L1_TxBuf.dat_len--;     // 送应用层实际数据长度-1
                    L1_TxBuf.data[j++] = 0x7d;  // 拷贝数据
                    break;
                }
            } else {
                L1_TxBuf.data[j++] = ptr[i];// 拷贝数据
            }
            crc_len++;
            if (crc_len >= len_pack) {
                par_inx = L1_CRC_LB;        // 转校验低字节
            }
            break;
            
        case L1_CRC_LB:
            if (ptr[i] == 0x7d) {           // 是0x7d，校验字段需要调整
                switch (ptr[i+1])           // 判断0x7d后字节的情况
                {
                default:
                case 0x7b:                  // 重复起始错
                case 0x7d:                  // 提前结束错
                    return  (CRC_ERR);      // 数据段解析错！
                
                case 0x82:
                    i++;                    // 数据索引调整+1
                    crc_buf = 0x7d;         // 校验低
                    break;
                }
            } else {
                crc_buf = ptr[i];           // 校验低
            }
            par_inx = L1_CRC_HB;            // 转校验高字节
            break;
        case L1_CRC_HB:
            if (ptr[i] == 0x7d) {           // 是0x7d，校验字段需要调整
                switch (ptr[i+1])           // 判断0x7d后字节的情况
                {
                default:
                case 0x7b:                  // 重复起始错
                case 0x7d:                  // 提前结束错
                    return  (CRC_ERR);      // 校验段解析错！
                
                case 0x82:
                    i++;                    // 数据索引调整+1
                    crc_buf |= 0x7d00;      // 校验高
                    break;
                }
            } else {
                crc_buf |= ptr[i]<<8;       // 校验高
            }
            par_inx = L1_STOP_LB;           // 转停止低字节
            break;
            
        case L1_STOP_LB:
            if (ptr[i] != 0x7d) {           // 停止低不是0x7d，报错
                return  (STP_ERR);          // 停止段解析错！
            } else {
                par_inx = L1_STOP_HB;       // 转停止高字节
            }
            break;
        case L1_STOP_HB:
            if (ptr[i] != 0x7d) {           // 停止高不是0x7d，报错
                return  (STP_ERR);          // 停止段解析错！
            } else {
                par_inx = L1_OK;            // 插入层解析成功
            }
            break;
        }
    }
    // 数据解析成功，送上层处理
    if (par_inx == L1_OK) {
        // crc校验处理
        if (crc_buf != Crc16 (&ptr[crc_inx], crc_len)) {
            return  (CRC_ERR);          // 校验和错！
        }
        L1_TxBuf.max_len = len_max;     // 调整数据限制长度，去插入层最大长度、校验、保留
        
LAYER2_LOOP:
        ret = App_L2_Process(&L1_RxBuf, &L1_TxBuf, cut_len, pip);
        
        if (ret == ANALYSIS_OK) {
            // 封装第一层
            buf->data[0] = 0x7d;    // 0 - 起始低
            buf->data[1] = 0x7b;    // 1 - 起始高
            j = 4;                  // 空出起始、长度段，指向实际数据
            len_pack = 0;
            for (i=0; i<L1_RxBuf.dat_len; i++) {
                len_pack++;
                buf->data[j++] = L1_RxBuf.data[i];
                if (L1_RxBuf.data[i] == 0x7d) {
                    buf->data[j++] = 0x82;
                    len_pack++;
                }
            } 
            if (len_pack > len_max) {   // 封装插入层后，长度超出处理
                try_cnt++;
                if (try_cnt <= 3) {
                    cut_len += len_pack - buf->max_len + 50;
                    if (cut_len >= (buf->max_len>>1)) {
                        return  (PACK_LEN_ERR);     // 长度小于最大长度一半，报错
                    } else {
                        goto    LAYER2_LOOP;        // 长度调整后重新解析
                    }
                } else {
                    return  (PACK_LEN_ERR);     // 重试次数超过，长度错误返回
                }
            } else {                            // 长度打包
                buf->data[2] = len_pack & 0xff; // 长度低字节
                buf->data[3] = len_pack >> 8;   // 长度高字节 不可能为0x7d
                if (buf->data[2] == 0x7d) {     // 调整
                    for (i = j; i>3; i--) {
                        buf->data[i] = buf->data[i-1];
                    }
                    buf->data[i] = 0x82;        // [3]
                    j++;
                    crc_buf = Crc16(&(buf->data[5]), len_pack);
                } else {
                    crc_buf = Crc16(&(buf->data[4]), len_pack);
                }
                buf->data[j++] = crc_buf & 0xff;
                if ((crc_buf & 0xff) == 0x7d) {
                    buf->data[j++] = 0x82;
                }
                buf->data[j++] = crc_buf >> 8;
                if ((crc_buf & 0xff00) == 0x7d00) {
                    buf->data[j++] = 0x82;
                }
                buf->data[j++]  = 0x7d; //结束
                buf->data[j++]  = 0x7d;
                buf->dat_len    = j;
            }
        } else {    //命令解析错
            ret = CMD_ERR;
        }
    } 
    // 数据解析失败
    else {
        switch (par_inx)
        {
        default:
            ret = FORMAT_ERR;   // 解析格式不完整错误
            break;
        case L1_START_LB:
        case L1_START_HB:
            ret = STA_ERR;      // 解析到起始段不完整错误
            break;
        case L1_LEN_LB:
        case L1_LEN_HB:
            ret = LEN_ERR;      // 解析到长度段不完整错误
            break;
        case L1_DATA:
            ret = DAT_ERR;      // 解析到数据段不完整错误
            break;
        }
    }

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
strDFHead   L2_Head;
strDataBuf  L2_Buf;
//----------------------------------------------------------------------------------------------------
//  (2) -- 数据帧协议层处理 //  buf == Backbuf  ptr == L1_Buf
//----------------------------------------------------------------------------------------------------
U8 App_L2_Process(strDataBuf *txbuf, const strDataBuf *rxbuf, U16 cutlen, const U8 *pip)
{
    U8  ret = ANALYSIS_OK;
    U16 i;

    // 数据帧层校验
    if (rxbuf->dat_len <= L2_MAX_LEN) {
        return  (LEN_ERR);
    }
    // 送上一层处理
//    L2_Head.stamp       = rxbuf->data[0];
//    L2_Head.page_num    = rxbuf->data[1];
//    L2_Head.page_inx    = rxbuf->data[2];
//    L2_Head.frame_id    = rxbuf->data[3]
//                        +(rxbuf->data[4]<<8)
//                        +(rxbuf->data[5]<<16)
//                        +(rxbuf->data[6]<<24);
    // 拷贝数据帧头部内容
    for (i=0; i<L2_FRAME_LEN; i++) {
        txbuf->data[i] = rxbuf->data[i];
    }

    L2_Buf.max_len  = rxbuf->max_len - L2_MAX_LEN;      // 调整数据限制长度，去数据帧层最大长度
//    L2_Buf.dat_len  = rxbuf->dat_len - L2_MAX_LEN;      // 调整数据长度
    for (i=0; i<rxbuf->dat_len; i++) {
        L2_Buf.data[i] = rxbuf->data[i+L2_FRAME_LEN];   // 拷贝数据送应用层
    }
    App_L3_Process(&L2_Buf, cutlen, pip);
    // 拷贝返回数据
    for (i=0; i<L2_Buf.dat_len; i++) {
        txbuf->data[i+L2_FRAME_LEN] = L2_Buf.data[i];
    }
    txbuf->dat_len = L2_Buf.dat_len + 7;

    return  (ret);
}
//----------------------------------------------------------------------------------------------------
//  (3) -- 数据帧协议层处理
//          2015-0323: 调整返回，错误代码改为命令长度，错误代码放由数据部分返回
//----------------------------------------------------------------------------------------------------
U16 App_L3_Process(strDataBuf *buf, U16 cutlen, const U8 *pip)
{
    U8  ret = 0;
    U8  cmd = buf->data[0];
    U8  ver = buf->data[1];
    U16 len;

    // 版本判断(上位机联机命令不判断)
    if (cmd != CMD_CONNECTION) {
        if ((ver != CMD_VER_TBL[cmd])
        ||  (ver == NVER)) {
            buf->data[1] = CMD_VER_TBL[cmd];    // 版本不匹配或支持版本,返回本机版本
        }
    } else {
        buf->data[1] = CMD_VER_TBL[cmd];    // 联机版本返回
    }
    if ((cutlen != 0) 
    &&  (cmd != CMD_RD_REALDATA)) {
        buf->data[2] = PACK_LEN_ERR;        // 封装长度错误
        buf->dat_len = 5;
        return  (buf->dat_len);
    }
    // 版本匹配后解析
    switch (cmd)
    {
    case CMD_CONNECTION:            //0x80-联机
        ret = CMD_CONNECTION_Proc(&buf->data[2], &buf->dat_len, pip);
        buf->dat_len += 3;
        break;

    case CMD_RD_REALDATA:           //0x81-读取实时数据
        ret = CMD_RD_REALDATA_Proc(&buf->data[2], &buf->dat_len, cutlen, pip);
        buf->dat_len += 3;
        break;
        
    case CMD_CIRCUIT_CTRL:          //0x82-通道控制
        ret = CMD_CIRCUIT_CTRL_Proc(&buf->data[2], pip);
        buf->data[2] = ret;     //返回状态 0,1
        buf->dat_len = 5;
        break;
        
    case CMD_CIRCUIT_START:         //0x83-通道启动
        ret = CMD_CIRCUIT_START_Proc(&buf->data[2],pip);
        buf->data[2] = ret;     //返回状态 0,1,2
        buf->dat_len = 5;
        break;
        
    case CMD_MODIFY_DEVINFO:        //0x84-修改设备信息
        ret = CMD_MODIFY_DEVINFO_Proc(&buf->data[2], pip);
        buf->data[2] = ret;     //返回状态 0,1
        buf->dat_len = 5;
        break;
        
    case CMD_CFG_MAC_IP:            //0x89-MAC及IP配置
        ret = CMD_CFG_MAC_IP_Proc(&buf->data[2], pip);
        buf->data[2] = ret;     //返回状态 0,1
        buf->dat_len = 5;
        break;
        
    case CMD_RD_DEV_MAC_IP:         //0x8a-读取指定设备号的MAC及IP
        ret = CMD_RD_DEV_MAC_IP_Proc(&buf->data[2], &buf->dat_len);
        if (ret) {
            buf->data[2] = ret;
            buf->dat_len = 5;
        } else {
            buf->dat_len += 3;
        }
        break;
        
    case CMD_RD_HIP_SMAC:           //0x8b-读取主机IP地址以及设备MAC
        ret = CMD_RD_HIP_SMAC_Proc(&buf->data[2], &buf->dat_len, pip);
        if (ret) {
            buf->data[2] = ret;
            buf->dat_len = 5;
        } else {
            buf->dat_len += 3;
        }
        break;
        
    case CMD_RQ_HOSTCTRL:           //0x8c-申请主控发送协议
        ret = CMD_RQ_HOSTCTRL_Proc(&buf->data[2], pip);
        buf->data[2] = ret;
        buf->dat_len = 5;
        break;

//----------------------------------------
//  基于金帆测试机版本的命令
//----------------------------------------
//--主通道控制相关的命令处理--

    case CMD_RD_SYSBIT:             //0x90-读系统功能位（报警屏蔽位及功能控制开关）
    case CMD_RD_CALIB:              //0x92-读校准数据
    case CMD_RD_MASKREG:            //0x94-读取报警屏蔽及调节器
    case CMD_RD_IRATE:              //0x96-读取额定电流
    case CMD_RD_URATE:              //0x98-读取额定电压
    case CMD_RD_IPID:               //0x9A-读取电流PID
    case CMD_RD_UPID:               //0x9C-读取电压PID
    case CMD_RD_TVALUE:             //0x9E-读取延时时间
    case CMD_RD_SVALUE:             //0xA0-读取报警阈值
    case CMD_RD_TVALUE_EX:          //0xBA-读取扩展延时时间
    case CMD_RD_SVALUE_EX:          //0xBC-读取扩展报警阈值
    case CMD_RD_VERSION:            //0xA2-读取软件版本
    case CMD_RD_BUSDATA:            //0xC0-读取母线参数 15-1223
    case CMD_RD_FREQDATA:           //0xC2-读取频率等参数 15-1223
    case CMD_RD_DUTYDATA:           //0xC4-读取占空比参数 15-1223
        ret = CMD_RD_ChanInfo_Proc(cmd, &buf->data[2], &buf->dat_len);
        if (ret) {
            buf->data[2] = ret;
            buf->dat_len = 5;
        } else {
            buf->dat_len += 3;
        }
        break;

    case CMD_WR_SYSBIT:             //0x91-写系统功能位（报警屏蔽位及功能控制开关）
    case CMD_WR_CALIB:              //0x93-写校准数据
    case CMD_WR_MASKREG:            //0x95-写入报警屏蔽及调节器
    case CMD_WR_IRATE:              //0x97-写入额定电流
    case CMD_WR_URATE:              //0x99-写入额定电压
    case CMD_WR_IPID:               //0x9B-写入电流PID
    case CMD_WR_UPID:               //0x9D-写入电压PID
    case CMD_WR_TVALUE:             //0x9F-写入延时时间
    case CMD_WR_SVALUE:             //0xA1-写入报警阈值
    case CMD_WR_TVALUE_EX:          //0xBB-写入扩展延时时间
    case CMD_WR_SVALUE_EX:          //0xBD-写入扩展报警阈值
    case CMD_WR_CANADDR:            //0xA3-写入CAN地址
    case CMD_CALIBSET:              //0xA5-保存或恢复校准数据
    case CMD_WR_BUSDATA:            //0xC1-写入母线参数 15-1223
    case CMD_WR_FREQDATA:           //0xC3-写入频率等参数 15-1223
    case CMD_WR_DUTYDATA:           //0xC5-写入占空比参数 15-1223
        ret = CMD_WR_ChanInfo_Proc(cmd, &buf->data[2], pip);
        buf->data[3] = ret; //返回状态
        buf->dat_len = 6;
        break;

//--中位机控制相关的命令处理--
    case CMD_RD_CTRLCFG:            //0xA6-读取中位机控制配置参数
        ret = CMD_RD_CtrlCfg_Proc(&buf->data[2], &buf->dat_len);
        if (ret) {
            buf->data[2] = ret;
            buf->dat_len = 5;
        } else {
            buf->dat_len += 3;
        }
        break;

    case CMD_WR_CTRLCFG:            //0xA7-写入中位机控制配置参数
        ret = CMD_WR_CtrlCfg_Proc(&buf->data[2], pip);
        buf->data[3] = ret; //返回状态
        buf->dat_len = 6;
        break;

    case CMD_RD_SCH_HCFG:           //0xA8-读取辅助通道硬件配置参数
        ret = CMD_RD_SubChHCfg_Proc(&buf->data[2], &buf->dat_len);
        if (ret) {
            buf->data[2] = ret;
            buf->dat_len = 5;
        } else {
            buf->dat_len += 3;
        }
        break;

    case CMD_WR_SCH_HCFG:           //0xA9-写入辅助通道硬件配置参数
        ret = CMD_WR_SubChHCfg_Proc(&buf->data[2], pip);
        buf->data[3] = ret; //返回状态
        buf->dat_len = 6;
        break;

    case CMD_RD_SCH_SCFG:             //0xAA-读取辅助通道软件配置参数
        ret = CMD_RD_SubChSCfg_Proc(&buf->data[2], &buf->dat_len);
        if (ret) {
            buf->data[2] = ret;
            buf->dat_len = 5;
        } else {
            buf->dat_len += 3;
        }
        break;
        
    case CMD_WR_SCH_SCFG:             //0xAB-写入辅助通道软件配置参数
        ret = CMD_WR_SubChSCfg_Proc(&buf->data[2], pip);
        buf->data[3] = ret; //返回状态
        buf->dat_len = 6;
        break;

    case CMD_RESET_DEV:             //0xAA-复位中位机
        ret = CMD_RESET_Dev_Proc(&buf->data[2], pip);
        buf->data[3] = ret; //返回状态
        buf->dat_len = 6;
        break;

//--辅助通道控制相关的命令处理--
    case CMD_RD_SUBREAL:            //0xB0-读取辅助通道实时数据
        ret = CMD_RD_SubChReal_Proc(&buf->data[2], &buf->dat_len);
        if (ret) {
            buf->data[2] = ret;
            buf->dat_len = 5;
        } else {
            buf->dat_len += 3;
        }
        break;

    case CMD_RD_SUBRATE:            //0xB2-读取辅助通道额定数据
    case CMD_RD_SUBCALIB:           //0xB4-读取辅助通道校准数据
    case CMD_RD_SUBEDATA:           //0xB6-读取辅助通道扩展数据（CAN地址及AD控制参数）
        ret = CMD_RD_SubChInfo_Proc(cmd, &buf->data[2], &buf->dat_len);
        if (ret) {
            buf->data[2] = ret;
            buf->dat_len = 5;
        } else {
            buf->dat_len += 3;
        }
        break;

    case CMD_WR_SUBREAL:            //0xB1-写入辅助通道实时数据
    case CMD_WR_SUBRATE:            //0xB3-写入辅助通道额定数据
    case CMD_WR_SUBCALIB:           //0xB5-写入辅助通道校准数据
    case CMD_WR_SUBEDATA:           //0xB7-写入辅助通道扩展数据（CAN地址及AD控制参数）
        ret = CMD_WR_SubChInfo_Proc(cmd, &buf->data[2], pip);
        buf->data[3]    = ret;  //返回状态
        buf->data[4]    = 0;
        buf->data[5]    = 0;
        buf->dat_len    = 6;
        break;

    case CMD_PROTL_TRANS:           //0xBE-通信中转
        ret = CMD_CommTransfer_Proc(&buf->data[2], pip);
        buf->data[3]    = ret;  //返回状态
        buf->data[4]    = 0;
        buf->data[5]    = 0;
        buf->dat_len    = 6;
        break;
    
    case CMD_TRANSPARENT:           //0xF1-透明传输 16-0801
        ret = CMD_Transparent_Proc(&buf->data[2], pip);
        buf->data[2]    = ret;  //返回状态
        buf->data[3]    = 0;
        buf->data[4]    = 0;
        buf->data[5]    = 0;
        buf->dat_len    = 6;
        break;

    default:
        buf->data[3]    = CMD_ERR;  //无效命令
        buf->data[4]    = 0;
        buf->data[5]    = 0;
        buf->dat_len    = 6;
        break;
    }

    len = buf->dat_len;

    return  (len);//(ret);
}
//----------------------------------------------------------------------------------------------------
/*  CRC16校验 
    CRC16监督式（美国标准）：G(X)=X16+X15+X2+1
*/
//----------------------------------------------------------------------------------------------------
const U16 crc_tab[256] =
{
/*00-0F*/0x0000,0xC0C1,0xC181,0x0140,0xC301,0x03C0,0x0280,0xC241,
         0xC601,0x06C0,0x0780,0xC741,0x0500,0xC5C1,0xC481,0x0440,
/*10-1F*/0xCC01,0x0CC0,0x0D80,0xCD41,0x0F00,0xCFC1,0xCE81,0x0E40,
         0x0A00,0xCAC1,0xCB81,0x0B40,0xC901,0x09C0,0x0880,0xC841,
/*20-2F*/0xD801,0x18C0,0x1980,0xD941,0x1B00,0xDBC1,0xDA81,0x1A40,
         0x1E00,0xDEC1,0xDF81,0x1F40,0xDD01,0x1DC0,0x1C80,0xDC41,
/*30-3F*/0x1400,0xD4C1,0xD581,0x1540,0xD701,0x17C0,0x1680,0xD641,
         0xD201,0x12C0,0x1380,0xD341,0x1100,0xD1C1,0xD081,0x1040,
/*40-4F*/0xF001,0x30C0,0x3180,0xF141,0x3300,0xF3C1,0xF281,0x3240,
         0x3600,0xF6C1,0xF781,0x3740,0xF501,0x35C0,0x3480,0xF441,
/*50-5F*/0x3C00,0xFCC1,0xFD81,0x3D40,0xFF01,0x3FC0,0x3E80,0xFE41,
         0xFA01,0x3AC0,0x3B80,0xFB41,0x3900,0xF9C1,0xF881,0x3840,
/*60-6F*/0x2800,0xE8C1,0xE981,0x2940,0xEB01,0x2BC0,0x2A80,0xEA41,
         0xEE01,0x2EC0,0x2F80,0xEF41,0x2D00,0xEDC1,0xEC81,0x2C40,
/*70-7F*/0xE401,0x24C0,0x2580,0xE541,0x2700,0xE7C1,0xE681,0x2640,
         0x2200,0xE2C1,0xE381,0x2340,0xE101,0x21C0,0x2080,0xE041,
/*80-8F*/0xA001,0x60C0,0x6180,0xA141,0x6300,0xA3C1,0xA281,0x6240,
         0x6600,0xA6C1,0xA781,0x6740,0xA501,0x65C0,0x6480,0xA441,
/*90-9F*/0x6C00,0xACC1,0xAD81,0x6D40,0xAF01,0x6FC0,0x6E80,0xAE41,
         0xAA01,0x6AC0,0x6B80,0xAB41,0x6900,0xA9C1,0xA881,0x6840,
/*A0-AF*/0x7800,0xB8C1,0xB981,0x7940,0xBB01,0x7BC0,0x7A80,0xBA41,
         0xBE01,0x7EC0,0x7F80,0xBF41,0x7D00,0xBDC1,0xBC81,0x7C40,
/*B0-BF*/0xB401,0x74C0,0x7580,0xB541,0x7700,0xB7C1,0xB681,0x7640,
         0x7200,0xB2C1,0xB381,0x7340,0xB101,0x71C0,0x7080,0xB041,
/*C0-CF*/0x5000,0x90C1,0x9181,0x5140,0x9301,0x53C0,0x5280,0x9241,
         0x9601,0x56C0,0x5780,0x9741,0x5500,0x95C1,0x9481,0x5440,
/*D0-DF*/0x9C01,0x5CC0,0x5D80,0x9D41,0x5F00,0x9FC1,0x9E81,0x5E40,
         0x5A00,0x9AC1,0x9B81,0x5B40,0x9901,0x59C0,0x5880,0x9841,
/*E0-EF*/0x8801,0x48C0,0x4980,0x8941,0x4B00,0x8BC1,0x8A81,0x4A40,
         0x4E00,0x8EC1,0x8F81,0x4F40,0x8D01,0x4DC0,0x4C80,0x8C41,
/*F0-FF*/0x4400,0x84C1,0x8581,0x4540,0x8701,0x47C0,0x4680,0x8641,
         0x8201,0x42C0,0x4380,0x8341,0x4100,0x81C1,0x8081,0x4040
};
//----------------------------------------------------------------------------------------------------
U16 Crc16(U8 *ptr, U16 len)
{
    U16 CRC_val;

    CRC_val = 0xFFFF;
    while(len--)
    {
        CRC_val ^= *ptr++;
        CRC_val = (CRC_val>>8)^crc_tab[CRC_val&0xff];
    }

    return CRC_val;
}
//----------------------------------------------------------------------------------------------------
/*  CRC16校验 
    CRC16监督式（欧洲标准）：G(X)=X16+X12+X5+1
*/
//----------------------------------------------------------------------------------------------------
/* CRC余式表 */
/*
const U16 crc_tbl[256]  =
{    
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};
//-------------------------------------------------- 
U16 Crc16(U8 *ptr, U16 len)
{
    U16 crc = 0;
    U8  data;

    crc = 0;
    while (len-- != 0) 
    { 
        data = (U8) (crc/256);  //存储CRC的高8位
        crc <<= 8;              //左移8位，相当于CRC的低8位乘以2^8
        crc ^= crc_tbl[data ^ *ptr];
        ptr++;
        //高8位和当前字节相加后再查表求的CRC，再加上以前的CRC
    }
    return  (crc);
}
*/
//----------------------------------------------------------------------------------------------------
//eof

