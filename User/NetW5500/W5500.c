//-------------------------------------------------------------------------------//
//                    ��������: RealView MDK-ARM Version 5.10                    //
//                    �� �� ��: RealView MDK-ARM Version 5.10                    //
//                    оƬ�ͺ�: STM32F103ZET6                                    //
//                    ��Ŀ����: HH-SPS ������Ŀ                                  //
//                    �ļ�����: W5500.c                                          //
//                    ��    ��: ������                                           //
//-------------------------------------------------------------------------------//
/**********************************************************************************
 * һ,����5�����֣�
 *    1. W5500��ʼ��
 *    2. W5500��Socket��ʼ��
 *    3. Socket����
 *         ���Socket����ΪTCP������ģʽ�������Socket_Listen()����,W5500��������״̬��ֱ��Զ�̿ͻ����������ӡ�
 *         ���Socket����ΪTCP�ͻ���ģʽ�������Socket_Connect()������
 *                                        ÿ����һ��Socket_Connect(s)����������һ�����ӣ�
 *                                        ������Ӳ��ɹ����������ʱ�жϣ�Ȼ������ٵ��øú����������ӡ�
 *         ���Socket����ΪUDPģʽ,�����Socket_UDP����
 *    4. Socket���ݽ��պͷ���
 *    5. W5500�жϴ���
 * ��,3��ģʽ������
 *    ��W5500Ϊ������ģʽ�ĵ��ù��̣�
 *       W5500_Init()-->Socket_Init(s)-->Socket_Listen(s)�����ù��̼���ɣ��ȴ��ͻ��˵����ӡ�
 *    ��W5500Ϊ�ͻ���ģʽ�ĵ��ù��̣�
 *       W5500_Init()-->Socket_Init(s)-->Socket_Connect(s)�����ù��̼���ɣ�����Զ�̷��������ӡ�
 *    ��W5500ΪUDPģʽ�ĵ��ù��̣�
 *       W5500_Init()-->Socket_Init(s)-->Socket_UDP(s)�����ù��̼���ɣ�������Զ������UDPͨ�š�
 *    W5500���������ӳɹ�����ֹ���ӡ��������ݡ��������ݡ���ʱ���¼��������Դ��ж�״̬�л�á�
**********************************************************************************/
#include <includes.h>
#ifndef STM32
    #include "user.h"
#else
    #include "stm32f10x.h"
    #include "stm32f10x_spi.h"
    #include "W5500.h"
#endif

#ifdef __W5500_H
#ifdef __W5500APP_H
//-------------------------------------------------�궨��
//���ͨ����
#define W5500_CH_MAX 8
//����ַ
#define COMMON_BASE           0x0000
//�����Ĵ���
#define W5500_MODE            COMMON_BASE          //Mode register,R/W,Default=0x00(ģʽ�Ĵ���,�ɶ���д,ȱʡֵ0x00,�üĴ������������λ��Ping�ر�ģʽ��PPPoEģʽ�Լ�������߽ӿ�)
#define W5500_MODE_RST        0x80  //�����λ,��1��Ч,��λ���Զ���0,оƬ�ڲ��Ĵ���������ʼ��Ϊȱʡֵ
#define W5500_MODE_WOL        0x20  //���绽��ʹ��
#define W5500_MODE_PB         0x10  //Ping blockʹ��(Ping��ֹģʽ,0�ر�Ping��ֹ,1����Ping��ֹ)
#define W5500_MODE_PPPOE      0x08  //PPPOE ʹ��(0:�ر� 1:�����������·����ֱ�����ӵ�ADSL,��λ�����á�1���Ա���ADSL���������ӡ���֪����,����ա�How to connect ADSL��Ӧ�ñʼ�)
#define W5500_MODE_FARP       0x02  //ǿ��ARPģʽ
#define W5500_GAR             COMMON_BASE+0x01     //����IP  ,    R/W, ȱʡֵ0x00, ��:192.168.1.1
#define W5500_SUBR            COMMON_BASE+0x05     //��������,    R/W, ȱʡֵ0x00, ��:255.255.255.0
#define W5500_SHAR            COMMON_BASE+0x09     //ԴMAC��ַ,   R/W, ȱʡֵ0x00, ��:00.08.DC.01.02.03->0x00,0x08,0xdc,0x01,0x02,0x03
#define W5500_SIPR            COMMON_BASE+0x0f     //ԴIP    ,    R/W, ȱʡֵ0x00, ��:192.168.0.3->0xc0,0xa8,0x00,0x03
#define W5500_INTLEVEL        COMMON_BASE+0x13     //�͵�ƽ�ж϶�ʱ���Ĵ���
#define W5500_IR              COMMON_BASE+0x15     //�жϼĴ���,R/W, ȱʡֵ0x00, 
#define W5500_IR_CONFLICT     0x80  //IP��ַ��ͻ:��һ���뱾��IP��ַ��ͬIP��ַ��ARP����ʱ,��λ���á�1��,�Ը�λд��1������0)
#define W5500_IR_UNREACH      0x40  //Ŀ�겻�ɵִ�:(��UDP���ݰ����͹�����,���Ŀ��IP��ַ������ʱ,W5500�����յ�һICMP(Ŀ���޷�����)���ݰ�,(����5.2.2.UDP ),�ڸ�״̬��,IP��ַ���˿ںŽ����浽UIPR��UPORT �Ĵ���,ͬʱ��λ�á�1��,�Ը�λд��1������0 )
#define W5500_IR_PPPOE        0x20  //PPPOE�ر� :(��PPPoEģʽ,���PPPoE���ӱ��ر�,��λ�á�1��,�Ը�λд��1������0)
#define W5500_IR_MP           0x10  //�յ����绽�Ѱ�(UDP)
#define W5500_IMR             COMMON_BASE+0x16     //�ж�����,    R/W, ȱʡֵ0x00
#define W5500_IMR_CONFLICT    0x80  //IP��ַ��ͻ
#define W5500_IMR_UNREACH     0x40  //Ŀ�겻�ɵִ�
#define W5500_IMR_PPPOE       0x20  //PPPOE�ر�
#define W5500_IMR_MP          0x10  //���绽�Ѱ�(UDP)
#define W5500_SIR             COMMON_BASE+0x17     //Socket�жϼĴ���
#define W5500_SIR_S7          (1<<7)
#define W5500_SIR_S6          (1<<6)
#define W5500_SIR_S5          (1<<5)
#define W5500_SIR_S4          (1<<4)
#define W5500_SIR_S3          (1<<3)
#define W5500_SIR_S2          (1<<2)
#define W5500_SIR_S1          (1<<1)
#define W5500_SIR_S0          (1<<0)
#define W5500_SIMR            COMMON_BASE+0x18     //Socket�ж����μĴ���
#define W5500_SIMR_S7         (1<<7)
#define W5500_SIMR_S6         (1<<6)
#define W5500_SIMR_S5         (1<<5)
#define W5500_SIMR_S4         (1<<4)
#define W5500_SIMR_S3         (1<<3)
#define W5500_SIMR_S2         (1<<2)
#define W5500_SIMR_S1         (1<<1)
#define W5500_SIMR_S0         (1<<0)
#define W5500_RTR             COMMON_BASE+0X19     //����ʱ��Ĵ���
#define W5500_RCR             COMMON_BASE+0X1b     //���Դ����Ĵ���
#define W5500_PTIMER          COMMON_BASE+0X1c     //PPP���ӿ���Э������ʱ�Ĵ���
#define W5500_PMAGIC          COMMON_BASE+0X1d     //PPP���ӿ���Э������Ĵ���
#define W5500_PHAR            COMMON_BASE+0x1E     //PPPoEģʽ��Ŀ��MAC�Ĵ���
#define W5500_PSID            COMMON_BASE+0x24     //PPPoEģʽ�»ỰID�Ĵ���
#define W5500_PMRU            COMMON_BASE+0x26     //PPPoEģʽ�������յ�Ԫ
#define W5500_UIPR            COMMON_BASE+0x28     //�޷��ִ�IP��ַ�Ĵ���
#define W5500_UPORTR          COMMON_BASE+0x2C     //�޷��ִ�PORT�Ĵ���
#define W5500_PHYCFGR         COMMON_BASE+0x2E     //W5500 PHY ���üĴ���
#define W5500_PHYCFGR_RST_PHY (1<<7)   //�����ڲ�PHY
#define W5500_PHYCFGR_OPMODE  (1<<6)   //����PHY����ģʽ
#define W5500_PHYCFGR_DPX     (1<<2)   //˫������״̬ (ֻ��)
#define W5500_PHYCFGR_SPD     (1<<1)   //�ٶ�״̬     (ֻ��)
#define W5500_PHYCFGR_LNK     (1<<0)   //����״̬     (ֻ��)
#define W5500_VER             COMMON_BASE+0x39     //W5500 оƬ�汾�Ĵ���
//SOCKET�Ĵ���
#define W5500_Sn_MR           COMMON_BASE+0x0000   //Socket ģʽ�Ĵ���
#define W5500_Sn_MR_MULTI_MFEN   (1<<7)
#define W5500_Sn_MR_BCASTB       (1<<6)
#define W5500_Sn_MR_ND_MC_MMB    (1<<5)
#define W5500_Sn_MR_UCASTB_MIP6B (1<<4)
#define W5500_Sn_MR_CLOSE     0x00
#define W5500_Sn_MR_TCP       0x01
#define W5500_Sn_MR_UDP       0x02
#define W5500_Sn_MR_MACRAW    0x04
#define W5500_Sn_CR           COMMON_BASE+0x0001   //Socket ���üĴ���
#define W5500_Sn_CR_OPEN      0x01
#define W5500_Sn_CR_LISTEN    0x02
#define W5500_Sn_CR_CONNECT   0x04
#define W5500_Sn_CR_DISCON    0x08
#define W5500_Sn_CR_CLOSE     0x10
#define W5500_Sn_CR_SEND      0x20
#define W5500_Sn_CR_SEND_MAC  0x21
#define W5500_Sn_CR_SEND_KEEP 0x22
#define W5500_Sn_CR_RECV      0x40
#define W5500_Sn_IR           COMMON_BASE+0x0002   //Socket �жϼĴ���(ֻ��)
#define W5500_Sn_IR_SENDOK    0x10
#define W5500_Sn_IR_TIMEOUT   0x08
#define W5500_Sn_IR_RECV      0x04
#define W5500_Sn_IR_DISCON    0x02
#define W5500_Sn_IR_CON       0x01
#define W5500_Sn_SR           COMMON_BASE+0x0003   //Socket ״̬�Ĵ���(ֻ��)
#define W5500_Sn_SR_CLOSED    0x00
#define W5500_Sn_SR_INIT      0x13
#define W5500_Sn_SR_LISTEN    0x14
#define W5500_Sn_SR_ESTABLISHED  0x17
#define W5500_Sn_SR_CLOSE_WAIT   0x1c
#define W5500_Sn_SR_UDP       0x22
#define W5500_Sn_SR_MACRAW    0x02
#define W5500_Sn_SR_SYNSEND   0x15
#define W5500_Sn_SR_SYNRECV   0x16
#define W5500_Sn_SR_FIN_WAI   0x18
#define W5500_Sn_SR_CLOSING   0x1a
#define W5500_Sn_SR_TIME_WAIT 0x1b
#define W5500_Sn_SR_LAST_ACK  0x1d
#define W5500_Sn_PORT         COMMON_BASE+0x0004   //Socket Դ�˿ڼĴ���
#define W5500_Sn_DHAR         COMMON_BASE+0x0006   //Ŀ��MAC
#define W5500_Sn_DIPR         COMMON_BASE+0x000c   //Ŀ��IP
#define W5500_Sn_DPORTR       COMMON_BASE+0x0010   //Ŀ��PORT
#define W5500_Sn_MSSR         COMMON_BASE+0x0012   //Socket n-th ���ֶμĴ���
#define W5500_Sn_TOS          COMMON_BASE+0x0015   //Socket IP �������ͼĴ���
#define W5500_Sn_TTL          COMMON_BASE+0x0016   //Socket IP ����ʱ��Ĵ���
#define W5500_Sn_RXBUF_SIZE   COMMON_BASE+0x001e   //���ջ����С�Ĵ���
#define W5500_Sn_TXBUF_SIZE   COMMON_BASE+0x001f   //���ͻ����С�Ĵ���
#define W5500_Sn_TX_FSR       COMMON_BASE+0x0020   //���з��ͻ���Ĵ���
#define W5500_Sn_TX_RD        COMMON_BASE+0x0022   //���Ͷ�ָ��Ĵ���
#define W5500_Sn_TX_WR        COMMON_BASE+0x0024   //����дָ��Ĵ���
#define W5500_Sn_RX_RSR       COMMON_BASE+0x0026   //���н��ջ���Ĵ���
#define W5500_Sn_RX_RD        COMMON_BASE+0x0028   //���ն�ָ��Ĵ���
#define W5500_Sn_RX_WR        COMMON_BASE+0x002a   //����дָ��Ĵ���
#define W5500_Sn_IMR          COMMON_BASE+0x002c   //�ж����μĴ���
#define W5500_Sn_IMR_SENDOK   0x10
#define W5500_Sn_IMR_TIMEOUT  0x08
#define W5500_Sn_IMR_RECV     0x04
#define W5500_Sn_IMR_DISCON   0x02
#define W5500_Sn_IMR_CON      0x01
#define W5500_Sn_FRAG         COMMON_BASE+0x002d   //Socket n �ֶμĴ���
#define W5500_Sn_KPALVTR      COMMON_BASE+0x002f   //Socket ����ʱ��Ĵ���
//����ģʽλ
#define W5500_VDM       0x00
#define W5500_FDM1  0x01
#define W5500_FDM2  0x02
#define W5500_FDM4  0x03
//��д����λ
#define W5500_RWB_READ  0x00
#define W5500_RWB_WRITE 0x04
//��ѡ��λ
#define W5500_COMMON_R  0x00
//
#define W5500_S_RX_SIZE 2048    //����Socket���ջ������Ĵ�С�����Ը���W5500_RMSR�������޸�
#define W5500_S_TX_SIZE 2048    //����Socket���ͻ������Ĵ�С�����Ը���W5500_TMSR�������޸�
//-------------------------------------------------����
static uint8_t W5500_InterruptSign;
W5500_S_CONFIG W5500_S_Config;
W5500_S_CONNECT W5500_S_Connect[W5500_CH_MAX];
//-------------------------------------------------����ָ��
void (*W5500APP_ProtocolAnalysis_ChX[8])(uint8_t *pbuf,uint16_t len,uint8_t *ipbuf,uint16_t port)= {W5500_ProtocolAnalysis_Ch0,\
                                                                                            W5500_ProtocolAnalysis_Ch1,\
                                                                                            W5500_ProtocolAnalysis_Ch2,\
                                                                                            W5500_ProtocolAnalysis_Ch3,\
                                                                                            W5500_ProtocolAnalysis_Ch4,\
                                                                                            W5500_ProtocolAnalysis_Ch5,\
                                                                                            W5500_ProtocolAnalysis_Ch6,\
                                                                                            W5500_ProtocolAnalysis_Ch7,\
                                                                                           };
//-------------------------------------------------����
//Ӳ����غ���-->�����޸�
static void W5500_GPIO_Configuration(void);
static uint8_t W5500_SPI_SendByte(uint8_t dt);
//�����غ���-->�����޸�-->��������
static uint8_t W5500_LoadVar(void);
static void W5500_ProtocolAnalysis(uint8_t ch,uint8_t *pbuf,uint16_t len);
//Ӳ���޹غ���-->�����޸�-->�м亯��
static uint8_t W5500_ReadReg_1Byte(uint16_t addr);
static void W5500_WriteReg_1Byte(uint16_t addr,uint8_t dat);
#ifdef STM32
static void W5500_Initialization(void);
#else
void W5500_Initialization(void);
#endif

static uint8_t W5500_TCPClient_SocketConnect(uint8_t ch);
static uint8_t W5500_TCPServer_SocketListen(uint8_t ch);
static uint8_t W5500_UDP_Socket(uint8_t ch);
static uint16_t W5500_App_Rx(uint8_t ch,uint8_t *pbuf,uint16_t maxlen);
static void W5500_Interrupt_Process(void);
static uint8_t W5500_Socket_Monitor(void);
//����ӿں���-->�����޸�
void W5500_Main_100ms(uint8_t sign_init);
void W5500_IRQHandler(void);
/*******************************************************************************
* ����    : ��ʱ����(ms)
* ����    : d:��ʱϵ������λΪ����
* ���    : ��
* ����    : ��
*******************************************************************************/
#ifndef STM32
    void W5500_DelayMs(uint16_t d)
    {
        Delay_xmS(d);
    }
#else
    void W5500_DelayMs(uint16_t d)
    {
        OSTimeDlyHMSM(0, 0, d/1000, d%1000);
    }
#endif

/*******************************************************************************
* ����    : W5500ͳ��ʼ����
* ����    : ��
* ���    : ��
* ����    : ��
*******************************************************************************/
static void W5500_GPIO_Configuration(void)
{
#ifndef STM32                     //PIC ��ʼ��
    W5500_SCS = 0;                //Ƭѡʹ��
    Nop();Nop();Nop();
    W5500_SCS = 1;                //ȡ��Ƭѡ
    Nop();Nop();Nop();

    SPI2CON1  = 0;                //����
    SPI2CON2  = 0;
    SPI2STAT  = 0;

    SPI2CON1bits.DISSDO= 0;       //ʹ�� SDO
    SPI2CON1bits.DISSCK= 0;       //ʹ�� SCK
    SPI2CON1bits.SSEN  = 0;       //��ֹ SS

    //������Ƶ(��ѡһ)
    //SPI2CON1bits.SPRE  = 7;     //1:1��Ƶ   SCK = 16MHZ
    SPI2CON1bits.SPRE  = 6;       //2:1��Ƶ   SCK = 8MHZ
    //SPI2CON1bits.SPRE  = 5;     //3:1��Ƶ
    //SPI2CON1bits.SPRE  = 4;     //4:1��Ƶ
    //SPI2CON1bits.SPRE  = 3;     //5:1��Ƶ
    //SPI2CON1bits.SPRE  = 2;     //6:1��Ƶ
    //SPI2CON1bits.SPRE  = 1;     //7:1��Ƶ
    //SPI2CON1bits.SPRE  = 0;     //8:1��Ƶ

    //����Ƶ(��ѡһ)
    //SPI2CON1bits.PPRE  = 3;     //1:1��Ƶ
    SPI2CON1bits.PPRE  = 2;       //4:1��Ƶ
    //SPI2CON1bits.PPRE  = 1;     //16:1��Ƶ
    //SPI2CON1bits.PPRE  = 0;     //64:1��Ƶ

    SPI2CON1bits.MSTEN = 1;       //����ģʽ
    SPI2CON1bits.SMP   = 0;       //0 = �м����; 1 = ĩ�˲���
    SPI2CON1bits.CKE   = 1;       //�����ط�������
    SPI2CON1bits.CKP   = 0;       //ģʽ0 SCKʱ�� ���е͵�ƽ
    SPI2CON1bits.MODE16= 0;       //8λ�ֽڿ�

    SPI2STATbits.SPIEN = 1;       //SPIʹ��
    Nop();Nop();Nop();Nop();
    SPI2STATbits.SPIEN = 0;       //SPIʹ��
    Nop();Nop();Nop();Nop();
    SPI2STATbits.SPIEN = 1;       //SPIʹ��
    Nop();Nop();Nop();Nop();
    SPI2STATbits.SPIEN = 0;       //SPIʹ��
    Nop();Nop();Nop();Nop();
    SPI2STATbits.SPIEN = 1;       //SPIʹ��
    Nop();Nop();Nop();Nop();

    //-----------W5500 ��ƽ�ж�---------------
    CNENCbits.CNIEC9 = 1;        // RC9�ж�
    _CNIP = 5;                   // �ж����ȼ�
    IFS1bits.CNIF    = 0;        // �����־λ
    IEC1bits.CNIE    = 1;        // ��ƽ�仯�ж�

#else
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    //----------SPI��ʼ������
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    //
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    //��ʼ��SCK��MISO��MOSI����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5);
    //��ʼ������STM32 SPI3
    SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;    //SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode=SPI_Mode_Master;                         //����Ϊ��SPI
    SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;                     //SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;                            //ʱ�����յ�
    SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;                          //���ݲ����ڵ�1��ʱ����
    SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;                             //NSS���ⲿ�ܽŹ���
    SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8;    //������Ԥ��ƵֵΪ8
    SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;                    //���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial=7;                              //CRC����ʽΪ7
    SPI_Init(SPI3, &SPI_InitStructure);                                 //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPI1�Ĵ���
    SPI_Cmd(SPI3, ENABLE);
    //----------GPIO��ʼ������
    //
    GPIO_InitStructure.GPIO_Pin  = W5500_RST;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(W5500_RST_PORT, &GPIO_InitStructure);
    W5500_PIN_RST_H;
    //
    GPIO_InitStructure.GPIO_Pin = W5500_INT;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(W5500_INT_PORT, &GPIO_InitStructure);

    //
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOF, GPIO_PinSource7);
    EXTI_InitStructure.EXTI_Line = EXTI_Line7;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    //
    GPIO_InitStructure.GPIO_Pin = W5500_SCS;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Init(W5500_SCS_PORT, &GPIO_InitStructure);
    W5500_PIN_SCS_H;
    //----------�ж����ȼ�����
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
}
/*******************************************************************************
* ����    : STM32 SPI1����һ���ֽ����ݣ������ؽ��յ�������
* ����    : dt:�����͵�����
* ���    : ��
* ����ֵ  : STM32 SPI1���յ�������
* ˵��    : ��ֲ��Ҫ�޸�
*******************************************************************************/
static uint8_t W5500_SPI_SendByte(uint8_t dt)
{
#ifndef STM32                     //PIC���� 1 Byte

    SPI2BUF = dt;                 //��������
    while(!IFS2bits.SPI2IF)       //�ȴ�����
    {
        Nop();
        ClrWdt();
    }
    IFS2bits.SPI2IF = 0;          //�����־
    return (SPI2BUF);             //��ȡ����

#else
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI3, dt);
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI3);
#endif

}
static void W5500_SPI_SendShort(uint16_t dt)
{
    W5500_SPI_SendByte(dt/256);
    W5500_SPI_SendByte(dt);
}
/*******************************************************************************
* ����    : ��W5500ָ����ַ�Ĵ�����д����
* ����    : addr:(��ַ) dat:(��д������)
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��ֲ�����޸�
*******************************************************************************/
static void W5500_WriteReg_1Byte(uint16_t addr, uint8_t dat)
{
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    W5500_SPI_SendByte(W5500_FDM1|W5500_RWB_WRITE|W5500_COMMON_R);
    W5500_SPI_SendByte(dat);
    W5500_PIN_SCS_H;
}
static void W5500_WriteReg_2Byte(uint16_t addr, uint16_t dat)
{
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    W5500_SPI_SendByte(W5500_FDM2|W5500_RWB_WRITE|W5500_COMMON_R);
    W5500_SPI_SendShort(dat);
    W5500_PIN_SCS_H;
}
static void W5500_WriteReg_nByte(uint16_t addr, uint8_t* pBuf ,uint16_t len)
{
    uint16_t i16;
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    W5500_SPI_SendByte(W5500_VDM|W5500_RWB_WRITE|W5500_COMMON_R);
    for(i16=0; i16<len; i16++)
    {
        W5500_SPI_SendByte(pBuf[i16]);
    }
    W5500_PIN_SCS_H;
}
static uint8_t W5500_ReadReg_1Byte(uint16_t addr)
{
    uint8_t i;
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    W5500_SPI_SendByte(W5500_FDM1|W5500_RWB_READ|W5500_COMMON_R);
    i=W5500_SPI_SendByte(0);
    W5500_PIN_SCS_H;
    return i;
}
/*******************************************************************************
* ����    : ��W5500ָ����ַSOCKETд����
* ����    : addr:(��ַ) dat:(��д������)
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��ֲ�����޸�
*******************************************************************************/
static void W5500_WriteSocket_1Byte(uint8_t num,uint16_t addr, uint8_t dat)
{
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    //1���ֽ����ݳ���,д����,ѡ��ͨ�üĴ���
    W5500_SPI_SendByte(W5500_FDM1|W5500_RWB_WRITE|(num*0x20+0x08));
    //д������
    W5500_SPI_SendByte(dat);
    W5500_PIN_SCS_H;
}
static void W5500_WriteSocket_2Byte(uint8_t num,uint16_t addr, uint16_t dat)
{
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    //2���ֽ����ݳ���,д����,ѡ��ͨ�üĴ���
    W5500_SPI_SendByte(W5500_FDM2|W5500_RWB_WRITE|(num*0x20+0x08));
    W5500_SPI_SendShort(dat);
    W5500_PIN_SCS_H;
}
static void W5500_WriteSocket_4Byte(uint8_t num,uint16_t addr, uint8_t* pBuf)
{
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    //2���ֽ����ݳ���,д����,ѡ��ͨ�üĴ���
    W5500_SPI_SendByte(W5500_FDM4|W5500_RWB_WRITE|(num*0x20+0x08));
    W5500_SPI_SendByte(pBuf[0]);
    W5500_SPI_SendByte(pBuf[1]);
    W5500_SPI_SendByte(pBuf[2]);
    W5500_SPI_SendByte(pBuf[3]);
    W5500_PIN_SCS_H;
}
static uint8_t W5500_ReadSocket_1Byte(uint8_t num,uint16_t addr)
{
    uint8_t i;
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    W5500_SPI_SendByte(W5500_FDM1|W5500_RWB_READ|(num*0x20+0x08));
    i=W5500_SPI_SendByte(0);
    W5500_PIN_SCS_H;
    return i;
}
static uint16_t W5500_ReadSocket_2Byte(uint8_t num,uint16_t addr)
{
    uint16_t i16;
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(addr);
    W5500_SPI_SendByte(W5500_FDM2|W5500_RWB_READ|(num*0x20+0x08));
    i16=W5500_SPI_SendByte(0);
    i16*=256;
    i16+=W5500_SPI_SendByte(0);
    W5500_PIN_SCS_H;
    return i16;
}
/*******************************************************************************
* ����    : װ���������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���ء����롢�����ַ������IP��ַ���˿ںš�Ŀ��IP��ַ��Ŀ�Ķ˿ںš��˿ڹ���ģʽ
*******************************************************************************/
static uint8_t W5500_LoadVar(void)
{
    if(W5500_S_Config.LOCK==0x55)
    {
        W5500_S_Config.Sign_IpConflict=0;
        return W5500_TRUE;
    }
    else
    {
        return W5500_FALSE;
    }
}
/*******************************************************************************
* ����    : W5500��ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : OK ---   ��ʼ���ɹ�     ERR   ---   ��ʼ��ʧ��
* ˵��    : ��
*******************************************************************************/

#ifdef STM32
static void W5500_Initialization(void)
#else
void W5500_Initialization(void)
#endif
{
    uint8_t i;
    uint8_t res;
    res=res;
    //----------Ӳ��λ
    W5500_PIN_RST_L;
    W5500_DelayMs(100);
    W5500_PIN_RST_H;
    W5500_DelayMs(100);
    //�ȴ���̫���������
    while((W5500_ReadReg_1Byte(W5500_PHYCFGR)&W5500_PHYCFGR_LNK)==0)
    {
        W5500_DelayMs(100);
    }
    //----------��ʼ��W5500�Ĵ�������
    //�����λ
    W5500_WriteReg_1Byte(W5500_MODE,W5500_MODE_RST);
    W5500_DelayMs(1000);
    W5500_DelayMs(1000);
    //��ȡоƬ�汾
    i=W5500_ReadReg_1Byte(W5500_VER);
    //��������
    W5500_WriteReg_nByte(W5500_GAR, W5500_S_Config.Gateway_IP, 4);
    //������������
    W5500_WriteReg_nByte(W5500_SUBR, W5500_S_Config.Sub_Mask, 4);
    //���������ַ
    //�õ�ֵַ��Ҫ��IEEE���룬����OUI�Ĺ涨��ǰ3���ֽ�Ϊ���̴��룬�������ֽ�Ϊ��Ʒ���
    //����Լ����������ַ��ע���һ���ֽڱ���Ϊż��
    W5500_WriteReg_nByte(W5500_SHAR, W5500_S_Config.Phy_Addr, 6);
    //���ñ���IP
    W5500_WriteReg_nByte(W5500_SIPR, W5500_S_Config.IP_Addr, 4);
    //���÷��ͻ������ͽ��ջ������Ĵ�С���ο�W5500�����ֲ�
    //����2K�Ĵ洢�ռ�
    for(i=0; i<W5500_CH_MAX; i++)
    {
        W5500_WriteSocket_1Byte(i,W5500_Sn_RXBUF_SIZE,0x02);
        W5500_WriteSocket_1Byte(i,W5500_Sn_TXBUF_SIZE,0x02);
    }
    //��������ʱ�䣬Ĭ��Ϊ2000(200ms)
    //ÿһ��λ��ֵΪ100΢��,��ʼ��ʱֵ��Ϊ2000(0x07D0),����200����
    W5500_WriteReg_2Byte(W5500_RTR,0x07d0);
    //�������Դ�����Ĭ��Ϊ8��
    //����ط��Ĵ��������趨ֵ,�������ʱ�ж�(��صĶ˿��жϼĴ����е�Sn_IR ��ʱλ(TIMEOUT)�á�1��)
    W5500_WriteReg_1Byte(W5500_RCR,8);
    //�ж�����->Socket�¼�:      IP��ͻ�ж�  |  UDPĿ�ĵ�ַ���ִܵ��ж�
    W5500_WriteReg_1Byte(W5500_IMR,(W5500_IMR_CONFLICT|W5500_IMR_UNREACH));
    //�ж�����->ͨ���ܿ���:      8��ͨ�����ж�ȫ��ʹ��
    W5500_WriteReg_1Byte(W5500_SIMR,W5500_SIMR_S0|W5500_SIMR_S1|W5500_SIMR_S2|W5500_SIMR_S3|W5500_SIMR_S4|W5500_SIMR_S5|W5500_SIMR_S6|W5500_SIMR_S7);
    //�ж�����->ͨ�����ܿ���:    �������    |  ��ʱ  |  ������� |  �Ͽ����� |  �������
    W5500_WriteSocket_1Byte(0, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    W5500_WriteSocket_1Byte(1, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    W5500_WriteSocket_1Byte(2, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    W5500_WriteSocket_1Byte(3, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    W5500_WriteSocket_1Byte(4, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    W5500_WriteSocket_1Byte(5, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    W5500_WriteSocket_1Byte(6, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    W5500_WriteSocket_1Byte(7, W5500_Sn_IMR, W5500_Sn_IMR_SENDOK | W5500_Sn_IMR_TIMEOUT | W5500_Sn_IMR_RECV | W5500_Sn_IMR_DISCON | W5500_Sn_IMR_CON);
    //----------������ط�����
    /*
    //������ؼ���ȡ���ص������ַ
    {
        INT8U buf[4];
        buf[0]=W5500_S_Config.IP_Addr[0];
        buf[1]=W5500_S_Config.IP_Addr[1];
        buf[2]=W5500_S_Config.IP_Addr[2];
        buf[3]=W5500_S_Config.IP_Addr[3]+1;
        W5500_WriteSocket_4Byte(0,W5500_Sn_DIPR,buf);
    }
    //����socket0ΪTCPģʽ
    W5500_WriteSocket_1Byte(0,W5500_Sn_MR,W5500_Sn_MR_TCP);
    //��socket0
    W5500_WriteSocket_1Byte(0,W5500_Sn_CR,W5500_Sn_CR_OPEN);
    //��ʱ5ms
    W5500_DelayMs(5);
    //���socket0��ʧ��
    if(W5500_ReadSocket_1Byte(0,W5500_Sn_SR)!=W5500_Sn_SR_INIT)
    {
        //�򿪲��ɹ�,�ر�Socket
        W5500_WriteSocket_1Byte(0,W5500_Sn_CR,W5500_Sn_CR_CLOSE);
        res = FALSE;
        goto W5500_Initialization_Goto;
    }

    //��socket0��TCP����
    W5500_WriteSocket_1Byte(0,W5500_Sn_CR,W5500_Sn_CR_CONNECT);
    do
    {
        INT8U j=0;
        //��ȡSocket0�жϱ�־�Ĵ���
        j=W5500_ReadSocket_1Byte(0,W5500_Sn_IR);
        if(j!=0)
            W5500_WriteSocket_1Byte(0,W5500_Sn_IR,j);
        W5500_DelayMs(5);//��ʱ5ms
        if((j&W5500_Sn_IR_TIMEOUT) == W5500_Sn_IR_TIMEOUT)
        {
            res = FALSE;
            goto W5500_Initialization_Goto;
        }
        else if(W5500_ReadSocket_1Byte(0,W5500_Sn_DHAR) != 0xff)
        {
            //�ر�Socket
            W5500_WriteSocket_1Byte(0,W5500_Sn_CR,W5500_Sn_CR_CLOSE);
            break;
        }
    }
    while(1);
    */
    //----------ָ��Socket(0~3)��ʼ��,��ʼ���˿�0
    for(i=0; i<W5500_CH_MAX; i++)
    {
        if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_NULL)
        {
            continue;
        }
        //���÷�Ƭ���ȣ��ο�W5500�����ֲᣬ��ֵ���Բ��޸�
        //����Ƭ�ֽ���=1460(0x5b4)
        //W5500_WriteReg_1Byte((W5500_S0_MSS+i*0x100),0x05);
        //W5500_WriteReg_1Byte((W5500_S0_MSS+i*0x100+1),0xb4);
        //���ñ��ض˿ڶ˿ں�
        W5500_WriteSocket_2Byte(i,W5500_Sn_PORT,W5500_S_Connect[i].LocalPort);
        //����Ŀ��IP�Ͷ˿�
        switch(W5500_S_Connect[i].order)
        {
            case W5500_E_CONNECT_ORDER_TCP_SERVER:
                //Ŀǰδʵ��
                break;
            case W5500_E_CONNECT_ORDER_TCP_IP_CLIENT:
                //����Ŀ�Ķ˿ں�
                W5500_WriteSocket_2Byte(i,W5500_Sn_DPORTR,W5500_S_Connect[i].DestPort);
                //����Ŀ��IP��ַ
                W5500_WriteSocket_4Byte(i,W5500_Sn_DIPR,W5500_S_Connect[i].DestIp);
                break;
            case W5500_E_CONNECT_ORDER_UDP_IP:
                //�˴���������,����ʱ���ü���
                break;
            case W5500_E_CONNECT_ORDER_TCP_DOMAIN_CLIENT:
            case W5500_E_CONNECT_ORDER_UDP_DOMAIN:
                //��Ҫ����DNS��������ȡIP
                //����DNS������IP
                //W5500_WriteSocket_2Byte(i,W5500_Sn_DIPR,W5500_S_Config.DNS_IP);
                //����DNS������PORT
                //W5500_WriteSocket_4Byte(i,W5500_Sn_DPORTR,W5500_S_Config.DNS_Port);
                break;
        }
    }
    //----------
//W5500_Initialization_Goto:
    return;
}
/*******************************************************************************
* ����    : ����ָ��Socket(0~3)Ϊ�ͻ�����Զ�̷���������
* ����    : s:���趨�Ķ˿�
* ���    : ��
* ����ֵ  : �ɹ�����TRUE(0xFF),ʧ�ܷ���FALSE(0x00)
* ˵��    : ������Socket�����ڿͻ���ģʽʱ,���øó���,��Զ�̷�������������
*           ����������Ӻ���ֳ�ʱ�жϣ��������������ʧ��,��Ҫ���µ��øó�������
*           �ó���ÿ����һ��,�������������һ������
*******************************************************************************/
static uint8_t W5500_TCPClient_SocketConnect(uint8_t ch)
{
    //����socketΪTCPģʽ
    W5500_WriteSocket_1Byte(ch,W5500_Sn_MR, W5500_Sn_MR_TCP);
    //��Socket
    W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_OPEN);
    //
    W5500_DelayMs(5);
    //���socket��ʧ��
    if(W5500_ReadSocket_1Byte(ch,W5500_Sn_SR)!=W5500_Sn_SR_INIT)
    {
        //�򿪲��ɹ�,�ر�Socket
        W5500_WriteSocket_1Byte(ch,W5500_Sn_CR,W5500_Sn_CR_CLOSE);
        return FALSE;
    }
    //����SocketΪConnectģʽ
    W5500_WriteSocket_1Byte(ch,W5500_Sn_CR,W5500_Sn_CR_CONNECT);
    return TRUE;

    //���������Socket�Ĵ����ӹ���,�������Ƿ���Զ�̷�������������,����Ҫ�ȴ�Socket�жϣ�
    //���ж�Socket�������Ƿ�ɹ����ο�W5500�����ֲ��Socket�ж�״̬
}

/*******************************************************************************
* ����    : ����ָ��Socket(0~3)��Ϊ�������ȴ�Զ������������
* ����    : s:���趨�Ķ˿�
* ���    : ��
* ����ֵ  : �ɹ�����TRUE(0xFF),ʧ�ܷ���FALSE(0x00)
* ˵��    : ������Socket�����ڷ�����ģʽʱ,���øó���,�ȵ�Զ������������
*           �ó���ֻ����һ��,��ʹW5500����Ϊ������ģʽ
*******************************************************************************/
static uint8_t W5500_TCPServer_SocketListen(uint8_t ch)
{
    //����socketΪTCPģʽ
    W5500_WriteSocket_1Byte(ch,W5500_Sn_MR, W5500_Sn_MR_TCP);
    //��Socket
    W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_OPEN);
    //
    W5500_DelayMs(5);
    //���socket��ʧ��
    if(W5500_ReadSocket_1Byte(ch,W5500_Sn_SR)!=W5500_Sn_SR_INIT)
    {
        //�򿪲��ɹ�,�ر�Socket
        W5500_WriteSocket_1Byte(ch,W5500_Sn_CR,W5500_Sn_CR_CLOSE);
        return FALSE;
    }
    //����SocketΪ����ģʽ
    W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_LISTEN);
    //
    W5500_DelayMs(5);
    //���socket����ʧ��
    if(W5500_ReadSocket_1Byte(ch,W5500_Sn_SR)!=W5500_Sn_SR_LISTEN)
    {
        //���ò��ɹ�,�ر�Socket
        W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_CLOSE);
        return FALSE;
    }
    return TRUE;

    //���������Socket�Ĵ򿪺�������������,����Զ�̿ͻ����Ƿ�������������,����Ҫ�ȴ�Socket�жϣ�
    //���ж�Socket�������Ƿ�ɹ����ο�W5500�����ֲ��Socket�ж�״̬
    //�ڷ���������ģʽ����Ҫ����Ŀ��IP��Ŀ�Ķ˿ں�
}

/*******************************************************************************
* ����    : ����ָ��Socket(0~3)ΪUDPģʽ
* ����    : s:���趨�Ķ˿�
* ���    : ��
* ����ֵ  : �ɹ�����TRUE(0xFF),ʧ�ܷ���FALSE(0x00)
* ˵��    : ���Socket������UDPģʽ,���øó���,��UDPģʽ��,Socketͨ�Ų���Ҫ��������
*           �ó���ֻ����һ�Σ���ʹW5500����ΪUDPģʽ
*******************************************************************************/
static uint8_t W5500_UDP_Socket(uint8_t ch)
{
    //����SocketΪUDPģʽ
    W5500_WriteSocket_1Byte(ch,W5500_Sn_MR, W5500_Sn_MR_UDP);
    //��Socket
    W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_OPEN);
    //
    W5500_DelayMs(5);
    //���Socket��ʧ��
    if(W5500_ReadSocket_1Byte(ch,W5500_Sn_SR)!=W5500_Sn_SR_UDP)
    {
        //�򿪲��ɹ�,�ر�Socket
        W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_CLOSE);
        return FALSE;
    }
    return TRUE;

    //���������Socket�Ĵ򿪺�UDPģʽ����,������ģʽ��������Ҫ��Զ��������������
    //��ΪSocket����Ҫ��������,�����ڷ�������ǰ����������Ŀ������IP��Ŀ��Socket�Ķ˿ں�
    //���Ŀ������IP��Ŀ��Socket�Ķ˿ں��ǹ̶���,�����й�����û�иı�,��ôҲ��������������
}

/*******************************************************************************
* ����    : ָ��Socket(0~3)�������ݴ���
* ����    : s:�˿�
* ���    : ��
* ����ֵ  : ���ؽ��յ����ݵĳ���
* ˵��    : ���Socket�����������ݵ��ж�,�����øó�����д���
*           �ó���Socket�Ľ��յ������ݻ��浽Rx_buffer������,�����ؽ��յ������ֽ���
*******************************************************************************/
static uint16_t W5500_App_Rx(uint8_t ch,uint8_t *pbuf,uint16_t maxlen)
{
    uint8_t j;
    uint16_t i16;
    uint16_t rx_size,rx_offset,rx_offset1;
    //
    if(0==(W5500_S_Connect[ch].SignBitmap & S_W5500_SOCKET_SIGNBITMAP_RX_OK))
    {
        return 0;
    }
    W5500_S_Connect[ch].SignBitmap&=~S_W5500_SOCKET_SIGNBITMAP_RX_OK;
    //��ȡ�������ݵ��ֽ���
    rx_size=W5500_ReadSocket_2Byte(ch,W5500_Sn_RX_RSR);
    if(rx_size==0)
    {
        return 0;
    }
    else if(rx_size>1460)
    {
        rx_size=1460;
    }
    //��ֹ�ڴ����
    if(rx_size>maxlen)
    {
        rx_size=maxlen;
    }
    //��ȡ���ջ�������ƫ����
    rx_offset=W5500_ReadSocket_2Byte(ch,W5500_Sn_RX_RD);
    rx_offset1=rx_offset;
    rx_offset&=(W5500_S_RX_SIZE-1);
    //
    W5500_PIN_SCS_L;
    W5500_SPI_SendShort(rx_offset);
    W5500_SPI_SendByte(W5500_VDM|W5500_RWB_READ|(ch*0x20+0x18));
    //����һ�������ݲ���ȡ����
    //j=W5500_SPI_SendByte(0);
    //�������ַδ����W5500���ջ������Ĵ���������ַ
    if((rx_offset+rx_size)<W5500_S_RX_SIZE)
    {
        //ѭ����ȡrx_size���ֽ�����
        for(i16=0; i16<rx_size; i16++)
        {
            j=W5500_SPI_SendByte(0);
            *pbuf=j;
            pbuf++;
        }
    }
    //�������ַ����W5500���ջ������Ĵ���������ַ
    else
    {
        rx_offset=W5500_S_RX_SIZE-rx_offset;
        for(i16=0; i16<rx_offset; i16++) //ѭ����ȡ��ǰoffset���ֽ�����
        {
            j=W5500_SPI_SendByte(0);
            *pbuf=j;
            pbuf++;
        }
        W5500_PIN_SCS_H;
        W5500_PIN_SCS_L;
        //д16λ��ַ
        W5500_SPI_SendShort(0x00);
        //д�����ֽ�,N���ֽ����ݳ���,������,ѡ��˿�s�ļĴ���
        W5500_SPI_SendByte(W5500_VDM|W5500_RWB_READ|(ch*0x20+0x18));
        j=W5500_SPI_SendByte(0);
        //ѭ����ȡ��rx_size-offset���ֽ�����
        for(; i16<rx_size; i16++)
        {
            j=W5500_SPI_SendByte(0);
            *pbuf=j;
            pbuf++;
        }
    }
    W5500_PIN_SCS_H;
    //����ʵ�������ַ,���´ζ�ȡ���յ������ݵ���ʼ��ַ
    rx_offset1+=rx_size;
    W5500_WriteSocket_2Byte(ch, W5500_Sn_RX_RD, rx_offset1);
    //����������������
    W5500_WriteSocket_1Byte(ch, W5500_Sn_CR, W5500_Sn_CR_RECV);
    //���ؽ��յ����ݵĳ���
    return rx_size;
}

/*******************************************************************************
* ����    : ָ��Socket(0~3)�������ݴ���
* ����    : s:�˿�,size(�������ݵĳ���)
* ���    : ��
* ����ֵ  : �ɹ�����TRUE(0xFF),ʧ�ܷ���FALSE(0x00)
* ˵��    : Ҫ���͵����ݻ�����Tx_buffer��
*******************************************************************************/
uint8_t W5500_App_Tx(uint8_t ch,uint8_t* pbuf ,uint16_t size)
{
    uint16_t i16;
    uint16_t tx_offset,tx_offset1;
    //���û��������,��������
    if(W5500_S_Connect[ch].state!=W5500_E_CONNECT_STATE_IP_CONNECTED
       && W5500_S_Connect[ch].state!=W5500_E_CONNECT_STATE_DOMAIN_SERVER_CONNECTED)
    {
        return W5500_FALSE;
    }
    //�����UDPģʽ,�����ڴ�����Ŀ��������IP�Ͷ˿ں�
    //if((W5500_ReadSocket_1Byte(ch,W5500_Sn_MR)&0x0f)!=W5500_Sn_SR_UDP)
    if((W5500_ReadSocket_1Byte(ch,W5500_Sn_MR))!=W5500_Sn_SR_UDP)
    {
        if(W5500_S_Connect[ch].state==W5500_E_CONNECT_STATE_IP_CONNECTED)
        {
            //����Ŀ������IP
            W5500_WriteSocket_4Byte(ch,W5500_Sn_DIPR,W5500_S_Connect[ch].DestIp);
            //����Ŀ�������˿ں�
            W5500_WriteSocket_2Byte(ch,W5500_Sn_DPORTR,W5500_S_Connect[ch].DestPort);
        }
        else if(W5500_S_Connect[ch].state==W5500_E_CONNECT_STATE_DOMAIN_SERVER_CONNECTED)
        {
            //����DNS������IP
            W5500_WriteSocket_4Byte(ch,W5500_Sn_DIPR,W5500_S_Config.DNS_IP);
            //����DNS������PORT
            W5500_WriteSocket_2Byte(ch,W5500_Sn_DPORTR,W5500_S_Config.DNS_Port);
        }
    }
    //����ʵ�ʵ������ַ
    tx_offset=W5500_ReadSocket_2Byte(ch,W5500_Sn_TX_WR);
    tx_offset1=tx_offset;
    tx_offset&=(W5500_S_TX_SIZE-1);
    W5500_PIN_SCS_L;
    //д16λ��ַ
    W5500_SPI_SendShort(tx_offset);
    //д�����ֽ�,N���ֽ����ݳ���,д����,ѡ��˿�s�ļĴ���
    W5500_SPI_SendByte(W5500_VDM|W5500_RWB_WRITE|(ch*0x20+0x10));
    //�������ַδ����W5500���ͻ������Ĵ���������ַ
    if((tx_offset+size)<W5500_S_TX_SIZE)
    {
        //ѭ��д��size���ֽ�����
        for(i16=0; i16<size; i16++)
        {
            //д��һ���ֽڵ�����
            W5500_SPI_SendByte(*pbuf++);
        }
    }
    //�������ַ����W5500���ͻ������Ĵ���������ַ
    else
    {
        tx_offset=W5500_S_TX_SIZE-tx_offset;
        //ѭ��д��ǰoffset���ֽ�����
        for(i16=0; i16<tx_offset; i16++)
        {
            //д��һ���ֽڵ�����
            W5500_SPI_SendByte(*pbuf++);
        }
        W5500_PIN_SCS_H;
        W5500_PIN_SCS_L;
        //д16λ��ַ
        W5500_SPI_SendShort(0x00);
        //д�����ֽ�,N���ֽ����ݳ���,д����,ѡ��˿�s�ļĴ���
        W5500_SPI_SendByte(W5500_VDM|W5500_RWB_WRITE|(ch*0x20+0x10));
        //ѭ��д��size-offset���ֽ�����
        for(; i16<size; i16++)
        {
            //д��һ���ֽڵ�����
            W5500_SPI_SendByte(*pbuf++);
        }
    }
    //��W5500��SCSΪ�ߵ�ƽ
    W5500_PIN_SCS_H;
    //����ʵ�������ַ,���´�д���������ݵ��������ݻ���������ʼ��ַ
    tx_offset1+=size;
    W5500_WriteSocket_2Byte(ch, W5500_Sn_TX_WR, tx_offset1);
    //����������������
    W5500_WriteSocket_1Byte(ch, W5500_Sn_CR, W5500_Sn_CR_SEND);
    //���سɹ�
    return W5500_TRUE;
}
/*******************************************************************************
* ������  : W5500_Socket_Set
* ����    : W5500�˿ڳ�ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : �ֱ�����4���˿�,���ݶ˿ڹ���ģʽ,���˿�����TCP��������TCP�ͻ��˻�UDPģʽ.
*           �Ӷ˿�״̬�ֽ�Socket_State�����ж϶˿ڵĹ������
*******************************************************************************/
static uint8_t W5500_Socket_Monitor(void)
{
    uint8_t i,res,len;
    uint8_t *p_i8;
    static uint8_t timer[W5500_CH_MAX]= {0};
    static uint16_t timer_all=0;
    //��ͨ�����
    for(i=0; i<W5500_CH_MAX; i++)
    {
        //��������
        if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_NULL)
        {
            continue;
        }
        if(W5500_S_Connect[i].state==W5500_E_CONNECT_STATE_IP_CONNECTED)
        {
            timer_all=0;
        }
    }
    timer_all++;
    if(timer_all>=300)
    {
        timer_all=0;
        return TRUE;
    }
    //����ͨ�����
    for(i=0; i<W5500_CH_MAX; i++)
    {
        //��������
        if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_NULL)
        {
            continue;
        }
        //��ʼ����
        if(W5500_S_Connect[i].state==W5500_E_CONNECT_STATE_NULL)
        {
            //TCP������ģʽ
            if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_TCP_SERVER)
            {
                if(W5500_TCPServer_SocketListen(i)==TRUE)
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_IP_CONNECTING;
                else
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
            }
            //TCP-IP�ͻ���ģʽ
            else if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_TCP_IP_CLIENT)
            {
                if(W5500_TCPClient_SocketConnect(i)==TRUE)
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_IP_CONNECTING;
                else
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
            }
            //UDP-IPģʽ
            else if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_UDP_IP)
            {
                if(W5500_UDP_Socket(i)==TRUE)
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_IP_CONNECTED;
                else
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
            }
            //TCP-�����ͻ���ģʽ || UDP-����ģʽ
            else if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_TCP_DOMAIN_CLIENT
                    ||W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_UDP_DOMAIN)
            {
                if(W5500_UDP_Socket(i)==TRUE)
                {
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_DOMAIN_SERVER_CONNECTED;
                    //���Ͳ�ѯDNS����֡
                    //---���뻺��
                    p_i8=MemManager_Get(256,&res);
                    //---����ת��Ϊ��׼��ʽ֡
                    DNSXsl_MakeQuery(W5500_S_Connect[i].DestDnmain,p_i8,&len);
                    //---����
                    W5500_App_Tx(i,p_i8,len);
                    //---�ͷŻ���
                    MemManager_Free(p_i8,&res);
                }
                else
                {
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
                }
            }
        }
        //����Ѿ���ȡ����IP,��Ͽ�����,�����±�־
        else if(W5500_S_Connect[i].state==W5500_E_CONNECT_STATE_DOMAIN_GETIP_OK)
        {
            W5500_WriteSocket_1Byte(i,W5500_Sn_CR,W5500_Sn_CR_CLOSE);
            W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_DOMAIN_GETIP_CLOSED;
        }
        //����Ѿ��ر���DNS����������,��ʼ����Ŀ��
        else if(W5500_S_Connect[i].state==W5500_E_CONNECT_STATE_DOMAIN_GETIP_CLOSED)
        {
            //����Ŀ��IP��PORT
            W5500_WriteSocket_2Byte(i,W5500_Sn_DPORTR,W5500_S_Connect[i].DestPort);
            W5500_WriteSocket_4Byte(i,W5500_Sn_DIPR,W5500_S_Connect[i].DestIp);
            //TCP������ģʽ
            if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_TCP_SERVER)
            {
                if(W5500_TCPServer_SocketListen(i)==TRUE)
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_IP_CONNECTING;
                else
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
            }
            //TCP-IP�ͻ���ģʽ
            else if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_TCP_IP_CLIENT
                    ||W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_TCP_DOMAIN_CLIENT)
            {
                if(W5500_TCPClient_SocketConnect(i)==TRUE)
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_IP_CONNECTING;
                else
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
            }
            //UDP-IPģʽ
            else if(W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_UDP_IP
                    ||W5500_S_Connect[i].order==W5500_E_CONNECT_ORDER_UDP_DOMAIN)
            {
                if(W5500_UDP_Socket(i)==TRUE)
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_IP_CONNECTED;
                else
                    W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
            }
        }
        //�������ʱ��
        if(W5500_S_Connect[i].state!=W5500_E_CONNECT_STATE_IP_CONNECTED)
        {
            timer[i]++;
            if(timer[i]>=100)
            {
                timer[i]=0;
                W5500_S_Connect[i].state=W5500_E_CONNECT_STATE_NULL;
            }
        }
    }
    return FALSE;
}
/*******************************************************************************
* ����    : W5500�жϴ��������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
static void W5500_Interrupt_Process(void)
{
    uint8_t i,j,ch;
    uint8_t num=3;
    while(num--)
    {
        //��ȡ�жϱ�־�Ĵ���
        i=W5500_ReadReg_1Byte(W5500_IR);
        //��д����жϱ�־
        W5500_WriteReg_1Byte(W5500_IR, (i&0xf0));
        //IP��ַ��ͻ�쳣����
        if((i & W5500_IR_CONFLICT) == W5500_IR_CONFLICT)
        {
            //-----�Լ���Ӵ���
            W5500_S_Config.Sign_IpConflict=1;
            //-----
        }
        //UDPģʽ�µ�ַ�޷������쳣����
        if((i & W5500_IR_UNREACH) == W5500_IR_UNREACH)
        {
            //-----�Լ���Ӵ���
            //-----
        }
        //��ȡ�˿��жϱ�־�Ĵ���
        i=W5500_ReadReg_1Byte(W5500_SIR);
        if(i==0)
        {
            break;
        }
        for(ch=0; ch<W5500_CH_MAX; ch++)
        {
            //Socket0�¼�����
            if((i & (W5500_SIR_S0<<ch)) != 0)
            {
                //��ȡSocket�жϱ�־�Ĵ���
                j=W5500_ReadSocket_1Byte(ch,W5500_Sn_IR);
                //��д���жϱ�־
                W5500_WriteSocket_1Byte(ch,W5500_Sn_IR, j);
                //��TCPģʽ��,Socket�ɹ�����
                if(j&W5500_Sn_IR_CON)
                {
                    W5500_S_Connect[ch].state=W5500_E_CONNECT_STATE_IP_CONNECTED;
                }
                //��TCPģʽ��Socket�Ͽ����Ӵ���
                if(j&W5500_Sn_IR_DISCON)
                {
                    // �رն˿�,�ȴ����´�����
                    W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_CLOSE);
                    W5500_S_Connect[ch].state=W5500_E_CONNECT_STATE_NULL;
                }
                //Socket���ݷ������
                if(j&W5500_Sn_IR_SENDOK)
                {
                    W5500_S_Connect[ch].SignBitmap|=S_W5500_SOCKET_SIGNBITMAP_TX_OK;
                }
                //Socket���յ�����
                if(j&W5500_Sn_IR_RECV)
                {
                    W5500_S_Connect[ch].SignBitmap|=S_W5500_SOCKET_SIGNBITMAP_RX_OK;
                }
                //Socket���ӻ����ݴ��䳬ʱ����
                if(j&W5500_Sn_IR_TIMEOUT)
                {
                    //�رն˿�,�ȴ����´�����
                    W5500_WriteSocket_1Byte(ch,W5500_Sn_CR, W5500_Sn_CR_CLOSE);
                    //��������״̬0x00,�˿�����ʧ��
                    W5500_S_Connect[ch].state=W5500_E_CONNECT_STATE_NULL;
                }
            }
        }
    }
}

/*******************************************************************************
* ����    : �жϷ�����
* ����    : ��
* ���    : ��
* ����ֵ  : ��
*******************************************************************************/
void W5500_IRQHandler(void)
{
    W5500_InterruptSign=1;
}
/*******************************************************************************
* ����    : ����ӿں���,����100mS��ʱ
* ����    : ��
* ���    : ��
* ����ֵ  : ��
*******************************************************************************/
void W5500_Main_100ms(uint8_t sign_init)
{
    static uint8_t first=1;
    uint8_t ch=0;
    uint16_t i16;
    uint8_t res;
    uint8_t *pbuf;
    //
    if(sign_init==TRUE)
    {
        first=1;
    }
    //INT8U i;
    if(first!=0)
    {
        if(first==1)
        {
            //��ʼ��Ӳ������
            W5500_GPIO_Configuration();
            first=2;
        }
        if(first==2)
        {
            //װ���������
            if(W5500_TRUE==W5500_LoadVar())
            {
                first=3;
            }
            else
            {
                return;
            }
        }
        if(first==3)
        {
            //W5500��ʼ������
            W5500_Initialization();
            first=0;
        }
    }
    //W5500�˿����Ӽ��
    if(TRUE==W5500_Socket_Monitor())
    {
        first=1;
    }
    //����W5500�ж�
    if(W5500_InterruptSign)
    {
        W5500_InterruptSign=0;
        W5500_Interrupt_Process();
    }
    //Socket0�������ݽ���
    //���뻺��
    pbuf=Mem1Manager_Get(1024,&res);
    for(ch=0; ch<W5500_CH_MAX; ch++)
    {
        i16=W5500_App_Rx(ch,pbuf,1024);
        if(i16!=0)
        {
            W5500_ProtocolAnalysis(ch,pbuf,i16);
        }
    }
    //�ͷŻ���
    Mem1Manager_Free(pbuf,&res);
}
/*******************************************************************************
* ����    : ͨ��0-7�Ľ�������
* ����    : *pbuf ---   �����ַ
*           len   ---   ������Ч����
* ���    : ��
* ����ֵ  : ��
*******************************************************************************/
static void W5500_ProtocolAnalysis(uint8_t ch,uint8_t *pbuf,uint16_t len)
{
    if(W5500_S_Connect[ch].state==W5500_E_CONNECT_STATE_IP_CONNECTED)
    {
        if(W5500_S_Connect[ch].order==W5500_E_CONNECT_ORDER_UDP_IP
           ||W5500_S_Connect[ch].order==W5500_E_CONNECT_ORDER_UDP_DOMAIN)
        {
            (*W5500APP_ProtocolAnalysis_ChX[ch])(&pbuf[8],len-8,&pbuf[0],pbuf[4]*256+pbuf[5]);
        }
        else
        {
            (*W5500APP_ProtocolAnalysis_ChX[ch])(pbuf,len,W5500_S_Connect[ch].DestIp,W5500_S_Connect[ch].DestPort);
        }
    }
    else if(W5500_S_Connect[ch].state==W5500_E_CONNECT_STATE_DOMAIN_SERVER_CONNECTED)
    {
        //����DNS����������֡
        if(TRUE==DNSXsl_Response(&pbuf[8],len-8,W5500_S_Connect[ch].DestIp))
        {
            //����״̬
            W5500_S_Connect[ch].state=W5500_E_CONNECT_STATE_DOMAIN_GETIP_OK;
        }
    }
}
#endif
#endif
