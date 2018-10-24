//-------------------------------------------------------------------------------//
//                           ������ͨ������޹�˾                                //
//                                ��Ȩ����                                       //
//                    ��ҵ��ַ��http://www.htong.com                             //
//                    ��������: MPLAB X IDE V1.95                                //
//                    �� �� ��: XC16  V1.20                                      //
//                    оƬ�ͺ�: dsPIC33EP256GM710                                //
//                    ��Ŀ����: HH-SPS ������Ŀ                                  //
//                    �ļ�����: Count.h                                          //
//                    ��    ��: ������                                           //
//                    ʱ    ��: 2013-10-21    �汾:  0.1                         //
//-------------------------------------------------------------------------------//
//
// �ļ�������
//
//
// ע�����
//
//
// �޸ļ�¼��
//
//
// �޸�ʱ��:
//
//
//-------------------------------------------------------------------------------
//--------------------------------
#ifdef COUNT_GLOBAL
#define COUNT_EXT
#else
#define COUNT_EXT extern
#endif
//--------------------------------
//COUNT_EXT void delay_ms(INT16U t);
COUNT_EXT INT16U Count_Sum(INT8U *Buf,INT16U len);
COUNT_EXT INT8U Count_Xor(INT8U *p,INT32U len);

COUNT_EXT INT32U sum32_pingjun(INT32U *p,INT8U len);
COUNT_EXT INT16U sum16_pingjun(INT16U *p,INT8U len);

COUNT_EXT INT16U GenCRC16(INT8U *pchMsg, INT16U DataLen);
COUNT_EXT INT32U CreateCRC32(INT8U *databuf,INT32U len);
COUNT_EXT INT32S ascii_2_hex(INT8U *O_data,INT8U *r_data,INT32U len);
COUNT_EXT INT32U hex_2_ascii(INT8U *data, INT8U *buffer, INT32U len);
COUNT_EXT void HextoAscii(INT8U *dest, INT8U *sour, INT8U hex_len);
COUNT_EXT INT8U AsciitoHex(INT8U *dest, INT8U *source, INT8U str_len);
COUNT_EXT INT32U hex_2_3x(INT8U *buffer, INT32U len);

COUNT_EXT void uintToStr(INT32U ui,INT8S *pp);
COUNT_EXT long StrToLong(INT8S *str,INT8U len);

COUNT_EXT void IP_Format(INT8U * ip);
COUNT_EXT void PORT_Format(INT8U *port);

COUNT_EXT void Delete_Char_0(INT8U *buf,INT8U num);

COUNT_EXT INT8U Capital(INT8U chr);

COUNT_EXT INT8U time_compare(struct tm tm1,struct tm tm2);

