//-------------------------------------------------------------------------------//
//                    开发环境: RealView MDK-ARM Version 5.10                    //
//                    编 译 器: RealView MDK-ARM Version 5.10                    //
//                    芯片型号: STM32F103ZET6                                    //
//                    项目名称: HH-SPS 安防项目                                  //
//                    文件名称: DNS_Xsl.c                                        //
//                    作    者: 徐松亮                                           //
//-------------------------------------------------------------------------------//
/*************************************说明*****************************************
 * 一, DNS实验数据(服务器:8.8.8.8 端口:53 实验域名:www.baidu.com)
 *    1, 发送:
 *       12 34 01 00 00 01 00 00 00 00 00 00 03 77 77 77 05 62 61 69 64 75 03 63
 *       6f 6d 00 00 01 00 01
 *       解释:
 *       12 34 :  标识字段
 *       01 00 :  标志字段,该字段设置了TC表示该报文是可截断的
 *       00 00 00 00 00 00 :  表示回答,授权和额外信息都是0
 *       03 77 77 77 05 62 61 69 64 75 03 63 6f 6d 00 :  表示域名www.baidu.com
 *       00 01 :  类型,1表示A查询
 *       00 01 :  类,1表示Internet数据
 *    2. 接收:
 *       12 34 81 80 00 01 00 03 00 00 00 00 03 77 77 77 05 62 61 69 64 75 03 63
 *       6F 6D 00 00 01 00 01 C0 0C 00 05 00 01 00 00 02 17 00 0F 03 77 77 77 01
 *       61 06 73 68 69 66 65 6E C0 16 C0 2B 00 01 00 01 00 00 00 2E 00 04 DC B5
 *       70 F4 C0 2B 00 01 00 01 00 00 00 2E 00 04 DC B5 6F BC
 *       解释:
 *       12 34 :  标识字段
 *       81 80 :  为标志字段(QR  opcode   AA TC RD RA zero  rcode4),其中设置了QR = 1，RD = 1，RA = 1
 *                           X   XXXX     X  X  X  X  000   XXXX
 *                QR       :  0表示查询报文，1表示响应报文
 *                opcode   :  通常值为0（标准查询），其他值为1（反向查询）和2（服务器状态请求）
 *                AA       :  表示授权回答（authoritative answer）.
 *                TC       :  表示可截断的（truncated）
 *                RD       :  表示期望递归
 *                RA       :  表示可用递归
 *                zero     :  必须为0
 *                rcode    :  通常为0（没有差错）和3（名字差错）
 *       00 01 :  问题数
 *       00 03 :  回答数
 *       00 00 :  未知
 *       00 00 :  未知
 *       03 77 77 77 05 62 61 69 64 75 03 63 6F 6D 00 :  表示查询域名www.baidu.com
 *       00 01 :  为类型，1表示A查询
 *       00 01 :  为类，1表示Internet数据
 *       c0 0c :  为域名指针
 *       00 05 :  表示CNAME（规范名称）
 *       00 01 :  类，表示为Internet数据
 *       00 00 02 17 :  生存时间
 *       00 0F :  数据长度
 *       03 77 77 77 01 61 06 73 68 69 66 65 6E C0 16 C0 2B 00 : www.a.....
 *       ...   :  未知
 *       DC B5 6F BC :  IP地址
 * 二,参考网址:http://blog.csdn.net/wangyifei0822/article/details/2316857
**********************************************************************************/
//-------------------------------------------------------------------------------库函数
#include <includes.h>
#ifndef STM32
#include "user.h"
#endif

#include <DNS_Xsl.h>
//-------------------------------------------------------------------------------宏定义
#define DNSXSL_DNS_ID   0x1234
/********************************************************************************
* 函数功能: 将域名转换为标准待上传的DNS数据结构
* 入口参数: pUrlStr   ---   域名
* 返回参数: 无
********************************************************************************/
void DNSXsl_MakeQuery(uint8_t* pUrlStr,uint8_t* pOutBuf,uint8_t* out_len)
{
    uint8_t i=0,j=0;
    uint8_t str_len=0;
    uint8_t index_num;
    //DNS标识符
    pOutBuf[i++]=(DNSXSL_DNS_ID>>8)&0xFF;
    pOutBuf[i++]=DNSXSL_DNS_ID&0xFF;
    //DNS头
    pOutBuf[i++] = 0x01;
    pOutBuf[i++] = 0x00;
    pOutBuf[i++] = 0x00;
    pOutBuf[i++] = 0x01;
    pOutBuf[i++] = 0x00;
    pOutBuf[i++] = 0x00;
    pOutBuf[i++] = 0x00;
    pOutBuf[i++] = 0x00;
    pOutBuf[i++] = 0x00;
    pOutBuf[i++] = 0x00;
    //正文
    index_num = i++;
    while (1)
    {
        //判断是否到字符末尾
        if (pUrlStr[j] == '\0')
        {
            pOutBuf[index_num] = str_len;
            pOutBuf[i++] = 0x00;
            break;
        }
        if (pUrlStr[j] != '.')
        {
            pOutBuf[i++] = pUrlStr[j];
            str_len++;
        }
        else
        {
            //字符长度
            pOutBuf[index_num] = str_len;
            str_len = 0;
            index_num = i++;
        }
        j++;
    }
    //dns尾
    pOutBuf[i++] = 0x00;
    pOutBuf[i++] = 0x01;
    pOutBuf[i++] = 0x00;
    pOutBuf[i++] = 0x01;
    //
    *out_len = i;
}
/********************************************************************************
* 函数功能: DNS应答解析
* 入口参数: 无
* 返回参数: 无
********************************************************************************/
uint8_t DNSXsl_Response(uint8_t* pInBuf,uint8_t len,uint8_t* pIpBuf)
{
    uint8_t i;
    uint16_t i16;
    //解析标识符
    i16=pInBuf[0]*256+pInBuf[1];
    if(i16!=DNSXSL_DNS_ID)
    {
        return FALSE;
    }
    //验证是否找到域名
    i16=pInBuf[2]*256+pInBuf[3];
    i16=i16&0x000F;
    if(i16!=0)
    {
        return FALSE;
    }
    //获取IP
    for (i = 0; i < 4; i++)
    {
        pIpBuf[i] = pInBuf[len - 4 + i];
    }
    return TRUE;
}
