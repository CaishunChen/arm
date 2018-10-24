//-------------------------------------------------------------------------------//
//                           ������ͨ������޹�˾                                //
//                                ��Ȩ����                                       //
//                    ��ҵ��ַ��http://www.htong.com                             //
//                    ��������: RealView MDK-ARM Version 4.14                    //
//                    �� �� ��: RealView MDK-ARM Version 4.14                    //
//                    оƬ�ͺ�: STM32F103ZET6                                    //
//                    ��Ŀ����: HH-SPS ������Ŀ                                  //
//                    �ļ�����: Memory.h                                         //
//                    ��    ��: ������                                           //
//                    ʱ    ��: 2014-01-17    �汾:  0.1                         //
//-------------------------------------------------------------------------------//
// �ļ���������д����.���ļ�ΪFlash.c��EEPROM.c���ϲ��ļ�,Ӧ��Ӳ���޹�����
// ע�����PICΪ16λ��Ƭ��,ע���ڽṹ���ж���Ҫ����Ϊ��λ��������,���߿ռ��˷��������ṹ�峤������
// ��ǰ״̬: ֻ�Ǵ�˳�����,��û������������
//-------------------------------------------------------------------------------
#ifndef __Memory_H
#define __Memory_H
//-------------------------------------------------------------------------------�궨��
#define MEMORY_PRODUCT_HEAD         "HH_FRAME"
#define MEMORY_PRODUCTION_DATE      0x0D0C
#ifdef STM32
#define MEMORY_HARDWARE_VER         STM32_HARD_BSP_VER
#else
#define MEMORY_HARDWARE_VER         1
#endif
#define MEMORY_SOFTWARE_VER         49
#define MEMORY_MAIN_VER             1
#define MEMORY_SUB_VER              14
#define MEMORY_PRODUCT_SERIAL_NUMBER 12345
#define MEMORY_PRODUCT_CODE         12345678
#define MEMORY_PRODUCT_TYPE         2
//-------------------------------------------------------------------------------EEPROM��ַ�ռ����
#define MEMORY_EEPROM_BEGIN_ADDR    256L
//EEPROMͷ
#define MEMORY_EEPROM_HEAD_SIZE     32
#define MEMORY_EEPROM_HEAD_ADDR     MEMORY_EEPROM_BEGIN_ADDR
#define MEMORY_EEPROM_HEAD_END      (MEMORY_EEPROM_HEAD_ADDR+MEMORY_EEPROM_HEAD_SIZE-1)
//�汾
#define MEMORY_VER_SIZE             32
#define MEMORY_VER_LEN              20
#define MEMORY_VER_ADDR             (MEMORY_EEPROM_HEAD_END+1)
#define MEMORY_VER_ADDR_END         (MEMORY_VER_ADDR+MEMORY_VER_SIZE-1)
//У��
#define MEMORY_CHECK_SIZE           512
#define MEMORY_CHECK_LEN            (4+32*8)
#define MEMORY_CHECK_ADDR           (MEMORY_VER_ADDR_END+1)
#define MEMORY_CHECK_ADDR_END       (MEMORY_CHECK_ADDR+MEMORY_CHECK_SIZE-1)
//EEPROM
#define MEMORY_KEEPPARA_SIZE        32
#define MEMORY_KEEPPARA_LEN         22
#define MEMORY_KEEPPARA_ADDR        (MEMORY_CHECK_ADDR_END+1)
#define MEMORY_KEEPPARA_ADDR_END    (MEMORY_KEEPPARA_ADDR+MEMORY_KEEPPARA_SIZE-1)
//����
#define MEMORY_CONTROL_SIZE         32
#define MEMORY_CONTROL_LEN          6
#define MEMORY_CONTROL_ADDR         (MEMORY_KEEPPARA_ADDR_END+1)
#define MEMORY_CONTROL_ADDR_END     (MEMORY_CONTROL_ADDR+MEMORY_CONTROL_SIZE-1)
//��
#define MEMORY_DOOR_MAX_NUM         4
#define MEMORY_DOOR_SIZE            16
#define MEMORY_DOOR_LEN             9
#define MEMORY_DOOR_ADDR            (MEMORY_CONTROL_ADDR_END+1)
#define MEMORY_DOOR_ADDR_END        (MEMORY_DOOR_ADDR+(MEMORY_DOOR_MAX_NUM*MEMORY_DOOR_SIZE)-1)
//����λͼ
#define MEMORY_DOORGBITMAP_MAX_NUM  4
#define MEMORY_DOORGBITMAP_SIZE     256
#define MEMORY_DOORGBITMAP_LEN      129
#define MEMORY_DOORGBITMAP_ADDR     (MEMORY_DOOR_ADDR_END+1)
#define MEMORY_DOORGBITMAP_ADDR_END (MEMORY_DOORGBITMAP_ADDR+(MEMORY_DOORGBITMAP_MAX_NUM*MEMORY_DOORGBITMAP_SIZE)-1)
//����λͼ
#define MEMORY_CARDGBITMAP_MAX_NUM  4
#define MEMORY_CARDGBITMAP_SIZE     256
#define MEMORY_CARDGBITMAP_LEN      129
#define MEMORY_CARDGBITMAP_ADDR     (MEMORY_DOORGBITMAP_ADDR_END+1)
#define MEMORY_CARDGBITMAP_ADDR_END (MEMORY_CARDGBITMAP_ADDR+(MEMORY_CARDGBITMAP_MAX_NUM*MEMORY_CARDGBITMAP_SIZE)-1)
//������
#define MEMORY_READER_MAX_NUM       8
#define MEMORY_READER_SIZE          32
#define MEMORY_READER_LEN           23
#define MEMORY_READER_ADDR          (MEMORY_CARDGBITMAP_ADDR_END+1)
#define MEMORY_READER_ADDR_END      (MEMORY_READER_ADDR+(MEMORY_READER_MAX_NUM*MEMORY_READER_SIZE)-1)
//�̵���
#define MEMORY_RELAY_MAX_NUM        8
#define MEMORY_RELAY_SIZE           16
#define MEMORY_RELAY_LEN            6
#define MEMORY_RELAY_ADDR           (MEMORY_READER_ADDR_END+1)
#define MEMORY_RELAY_ADDR_END       (MEMORY_RELAY_ADDR+(MEMORY_RELAY_MAX_NUM*MEMORY_RELAY_SIZE)-1)
//����
#define MEMORY_INPUT_MAX_NUM        8
#define MEMORY_INPUT_SIZE           16
#define MEMORY_INPUT_LEN            8
#define MEMORY_INPUT_ADDR           (MEMORY_RELAY_ADDR_END+1)
#define MEMORY_INPUT_ADDR_END       (MEMORY_INPUT_ADDR+(MEMORY_INPUT_MAX_NUM*MEMORY_INPUT_SIZE)-1)
//����
#define MEMORY_ALARM_SIZE           16
#define MEMORY_ALARM_LEN            13
#define MEMORY_ALARM_ADDR           (MEMORY_INPUT_ADDR_END+1)
#define MEMORY_ALARM_ADDR_END       (MEMORY_ALARM_ADDR+MEMORY_ALARM_SIZE-1)
//����ָ���
typedef struct S_MEMORY_EEPROM_FLASH
{
    INT8U Head[8];
    INT32U FalshAppAddr;  //Ӧ�õ�ַ
    INT8U SaveBlockNum;   //�������(0��ʼ,����ƽ���������)
    //----------
    INT8U Reserve[1];     //����
    INT16U Sum;
} S_MEMORY_EEPROM_FLASH;
#define MEMORY_EEPROM_FLASH_HEAD_STR         "FLASH"
#define MEMORY_EEPROM_FLASH_ADDR             (MEMORY_ALARM_ADDR_END+1)
#define MEMORY_EEPROM_FLASH_ADDR_END         (MEMORY_EEPROM_FLASH_ADDR+16-1)
//�߼�ģ��-->���������
#define MEMORY_LOGICBLOCK_INOUTAND_MAX_NUM      16
#define MEMORY_LOGICBLOCK_INOUTAND_SIZE         32
#define MEMORY_LOGICBLOCK_INOUTAND_LEN          23
#define MEMORY_LOGICBLOCK_INOUTAND_ADDR         (MEMORY_EEPROM_FLASH_ADDR_END+1)
#define MEMORY_LOGICBLOCK_INOUTAND_ADDR_END     (MEMORY_LOGICBLOCK_INOUTAND_ADDR+(MEMORY_LOGICBLOCK_INOUTAND_MAX_NUM*MEMORY_LOGICBLOCK_INOUTAND_SIZE)-1)
/*
//�߼�ģ��-->����ģ������
#define MEMORY_LOGICBLOCK_INPUT_MAX_NUM      64
#define MEMORY_LOGICBLOCK_INPUT_SIZE         8
#define MEMORY_LOGICBLOCK_INPUT_LEN          5
#define MEMORY_LOGICBLOCK_INPUT_ADDR         (MEMORY_EEPROM_FLASH_ADDR_END+1)
#define MEMORY_LOGICBLOCK_INPUT_ADDR_END     (MEMORY_LOGICBLOCK_INPUT_ADDR+(MEMORY_LOGICBLOCK_INPUT_MAX_NUM*MEMORY_LOGICBLOCK_INPUT_SIZE)-1)
//�߼�ģ��-->�߼�ģ������
#define MEMORY_LOGICBLOCK_LOGIC_MAX_NUM      128
#define MEMORY_LOGICBLOCK_LOGIC_SIZE         16
#define MEMORY_LOGICBLOCK_LOGIC_LEN          7
#define MEMORY_LOGICBLOCK_LOGIC_ADDR         (MEMORY_LOGICBLOCK_INPUT_ADDR_END+1)
#define MEMORY_LOGICBLOCK_LOGIC_ADDR_END     (MEMORY_LOGICBLOCK_LOGIC_ADDR+(MEMORY_LOGICBLOCK_LOGIC_MAX_NUM*MEMORY_LOGICBLOCK_LOGIC_SIZE)-1)
//�߼�ģ��-->���ģ������
#define MEMORY_LOGICBLOCK_OUTPUT_MAX_NUM      64
#define MEMORY_LOGICBLOCK_OUTPUT_SIZE         16
#define MEMORY_LOGICBLOCK_OUTPUT_LEN          9
#define MEMORY_LOGICBLOCK_OUTPUT_ADDR         (MEMORY_LOGICBLOCK_LOGIC_ADDR_END+1)
#define MEMORY_LOGICBLOCK_OUTPUT_ADDR_END     (MEMORY_LOGICBLOCK_OUTPUT_ADDR+(MEMORY_LOGICBLOCK_OUTPUT_MAX_NUM*MEMORY_LOGICBLOCK_OUTPUT_SIZE)-1)
*/
//������
#define MEMORY_REVERSE_MAX_NUM               (256L)
#define MEMORY_REVERSE_SIZE                  (4L)
#define MEMORY_REVERSEG_NUM                  (4L)
#define MEMORY_REVERSE_ADDR                  (MEMORY_LOGICBLOCK_INOUTAND_ADDR_END+1)
#define MEMORY_REVERSE_ADDR_END              (MEMORY_REVERSE_ADDR+(MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE*MEMORY_REVERSEG_NUM)-1)
//������ַ
#define MEMORY_EERPOM_END_ADDR               (MEMORY_REVERSE_ADDR_END+1)
//оƬ����ַ
#define MEMORY_EEPROM_MAX_ADDR               (32*1024L)
//-------------------------------------------------------------------------------FLASH��ַ�ռ����
//��С������λ
#define MEMORY_FLASH_MIN_ERASE_SIZE          (4*1024L)
//���д�뵥λ
#define MEMORY_FLASH_MAX_WRITE_SIZE          (256L)
//Ӧ�ò���ʼ��ַ
#define MEMORY_FLASH_BEGIN_ADDR              0L
//ͷ��ַ(4K)
#define MEMORY_FLASH_HEAD_ADDR               MEMORY_FLASH_BEGIN_ADDR
#define MEMORY_FLASH_HEAD_ADDR_END           (MEMORY_FLASH_HEAD_ADDR+MEMORY_FLASH_MIN_ERASE_SIZE-1)
//Ȩ�ޱ��ַ(320K)(�ȴ���ͨ�û�,���ÿ�)
#define MEMORY_FLASH_USER_PERMISSION_MAX_NUM (2048L)
#define MEMORY_FLASH_USER_PERMISSION_SIZE    (32L)
#define MEMORY_FLASH_USER_PERMISSION_LEN     (26)
#define MEMORY_FLASH_USER_PERMISSION_GROUP_NUM 17
#define MEMORY_FLASH_MAX_USER                ((MEMORY_FLASH_USER_PERMISSION_GROUP_NUM-1)*MEMORY_FLASH_USER_PERMISSION_MAX_NUM)
#define MEMORY_FLASH_USER_PERMISSION_ADDR    (MEMORY_FLASH_HEAD_ADDR_END+1)
#define MEMORY_FLASH_USER_PERMISSION_ADDR_END  (MEMORY_FLASH_USER_PERMISSION_ADDR+(MEMORY_FLASH_USER_PERMISSION_SIZE*MEMORY_FLASH_USER_PERMISSION_MAX_NUM*MEMORY_FLASH_USER_PERMISSION_GROUP_NUM)-1)
//ˢ����¼��ַ(160K)
#define MEMORY_FLASH_MAX_CARD_RECORD         (10240L)
#define MEMORY_FLASH_CARD_RECORD_SIZE        (32L)
#define MAX_CARD_RECORD_STRUCT_BYTE_LEN      (19)
#define MEMORY_FLASH_CARD_RECORD_ADDR        (MEMORY_FLASH_USER_PERMISSION_ADDR_END+1)
#define MEMORY_FLASH_CARD_RECORD_ADDR_END    (MEMORY_FLASH_CARD_RECORD_ADDR+(MEMORY_FLASH_MAX_CARD_RECORD*MEMORY_FLASH_CARD_RECORD_SIZE)-1)
//������¼��ַ(160K)
#define MEMORY_FLASH_MAX_ALARM_RECORD        (10240L)
#define MEMORY_FLASH_ALARM_RECORD_SIZE       (32L)
#define MAX_ALARM_RECORD_STRUCT_BYTE_LEN     (19)
#define MEMORY_FLASH_ALARM_RECORD_ADDR       (MEMORY_FLASH_CARD_RECORD_ADDR_END+1)
#define MEMORY_FLASH_ALARM_RECORD_ADDR_END   (MEMORY_FLASH_ALARM_RECORD_ADDR+(MEMORY_FLASH_MAX_ALARM_RECORD*MEMORY_FLASH_ALARM_RECORD_SIZE)-1)
//ʱ������(16K)
#define MEMORY_FLASH_MAX_TIMEGROUP           (256L)
#define MEMORY_FLASH_TIMEGROUP_SIZE          (128L)
#define MEMORY_FLASH_TIMEGROUP_LEN           (96)
#define MEMORY_FLASH_TIMEGROUP_NUMBER_LEN    (2) /*ֻ����ɾ��ָ��ĳ���*/
#define MEMORY_FLASH_TIMEGROUP_ADDR          (MEMORY_FLASH_ALARM_RECORD_ADDR_END+1)
#define MEMORY_FLASH_TIMEGROUP_ADDR_END      (MEMORY_FLASH_TIMEGROUP_ADDR+MEMORY_FLASH_TIMEGROUP_SIZE*MEMORY_FLASH_MAX_TIMEGROUP-1)
//�ڼ�����(16K)
#define MEMORY_FLASH_MAX_HOLIDAYGROUP        (256L)
#define MEMORY_FLASH_HOLIDAYGROUP_SIZE       (64L)
#define MEMORY_FLASH_HOLIDAYGROUP_LEN        (49)
#define MEMORY_FLASH_HOLIDAYGROUP_NUMBER_LEN (2) /*ֻ����ɾ��ָ��ĳ���*/
#define MEMORY_FLASH_HOLIDAYGROUP_ADDR       (MEMORY_FLASH_TIMEGROUP_ADDR_END+1)
#define MEMORY_FLASH_HOLIDAYGROUP_ADDR_END   (MEMORY_FLASH_HOLIDAYGROUP_ADDR+MEMORY_FLASH_HOLIDAYGROUP_SIZE*MEMORY_FLASH_MAX_HOLIDAYGROUP-1)
//����
#define MEMORY_DOORGROUP_MAX_NUM             (1024L)
#define MEMORY_DOORGROUP_SIZE                (8L)
#define MEMORY_DOORGROUP_LEN                 (3)
#define MEMORY_DOORGROUP_NUMBER_LEN          (2) /*ֻ����ɾ��ָ��ĳ���*/
#define MEMORY_DOORGROUP_ADDR                (MEMORY_FLASH_HOLIDAYGROUP_ADDR_END+1)
#define MEMORY_DOORGROUP_ADDR_END            (MEMORY_DOORGROUP_ADDR+MEMORY_DOORGROUP_MAX_NUM*MEMORY_DOORGROUP_SIZE-1)
//����Ȩ��
#define MEMORY_CARDGROUPLIMIT_MAX_NUM        (1024L)
#define MEMORY_CARDGROUPLIMIT_SIZE           (16L)
#define MEMORY_CARDGROUPLIMIT_LEN            (10)
#define MEMORY_CARDGROUPLIMIT_NUMBER_LEN     (2) /*ֻ����ɾ��ָ��ĳ���*/
#define MEMORY_CARDGROUPLIMIT_ADDR           (MEMORY_DOORGROUP_ADDR_END+1)
#define MEMORY_CARDGROUPLIMIT_ADDR_END       (MEMORY_CARDGROUPLIMIT_ADDR+(MEMORY_CARDGROUPLIMIT_MAX_NUM*MEMORY_CARDGROUPLIMIT_SIZE)-1)
//����
#define MEMORY_AREA_MAX_NUM                  (1024L)
#define MEMORY_AREA_SIZE                     (16L)
#define MEMORY_AREA_LEN                      (12)
#define MEMORY_AREA_NUMBER_LEN               (2) /*ֻ����ɾ��ָ��ĳ���*/
#define MEMORY_AREA_ADDR                     (MEMORY_CARDGROUPLIMIT_ADDR_END+1)
#define MEMORY_AREA_ADDR_END                 (MEMORY_AREA_ADDR+(MEMORY_AREA_MAX_NUM*MEMORY_AREA_SIZE)-1)
//������
/*
#define MEMORY_REVERSE_MAX_NUM               (1024L)
#define MEMORY_REVERSE_SIZE                  (4L)
#define MEMORY_REVERSEG_NUM                  (4L)
#define MEMORY_REVERSE_ADDR                  (MEMORY_AREA_ADDR_END+1)
#define MEMORY_REVERSE_ADDR_END              (MEMORY_REVERSE_ADDR+(MEMORY_REVERSE_MAX_NUM*MEMORY_REVERSE_SIZE*MEMORY_REVERSEG_NUM)-1)
*/
//�޸ı��õ�ַ(400K)
#define MEMORY_FLASH_MAX_MODIFICATION        100
#define MEMORY_FLASH_MODIFICATION_SIZE       MEMORY_FLASH_MIN_ERASE_SIZE
#define MEMORY_FLASH_MODIFICATION_ADDR       (MEMORY_AREA_ADDR_END+1)
#define MEMORY_FLASH_MODIFICATION_ADDR_END   (MEMORY_FLASH_MODIFICATION_ADDR+MEMORY_FLASH_MODIFICATION_SIZE*MEMORY_FLASH_MAX_MODIFICATION-1)
//������ַ
#define MEMORY_FLASH_END_ADDR                (MEMORY_FLASH_MODIFICATION_ADDR_END+1)
#define MEMORY_FLASH_MAX_ADDR                (8*1024*1024L)
//-------------------------------------------------------------------------------����ö�ٶ���
enum MEMORY_APP_CMD
{
    //������д����
    MEMORY_APP_CMD_VER_R=0,            //���汾
    MEMORY_APP_CMD_VER_W,              //д�汾(���ڲ�ʹ��)
    MEMORY_APP_CMD_CHECK_R,            //�����
    MEMORY_APP_CMD_CHECK_W,            //д���
    MEMORY_APP_CMD_KEEPPARA_R,         //��Ҫ����Ĳ���(���ڲ�ʹ��)
    MEMORY_APP_CMD_KEEPPARA_W,         //дҪ����Ĳ���(���ڲ�ʹ��)
    MEMORY_APP_CMD_CONTROL_R,          //��������Ϣ
    MEMORY_APP_CMD_CONTROL_W,          //д������Ϣ
    MEMORY_APP_CMD_DOOR_R,             //����
    MEMORY_APP_CMD_DOOR_W,             //д��
    //MEMORY_APP_CMD_DOORGBITMAP_R,      //������λͼ
    //MEMORY_APP_CMD_DOORGBITMAP_W,      //д����λͼ
    MEMORY_APP_CMD_CARDGBITMAP_R,      //������λͼ
    MEMORY_APP_CMD_CARDGBITMAP_W,      //д����λͼ
    MEMORY_APP_CMD_READER_R,           //��������
    MEMORY_APP_CMD_READER_W,           //д������
    MEMORY_APP_CMD_RELAY_R,            //���̵���
    MEMORY_APP_CMD_RELAY_W,            //д�̵���
    MEMORY_APP_CMD_INPUT_R,            //������
    MEMORY_APP_CMD_INPUT_W,            //д����
    MEMORY_APP_CMD_ALARM_R,            //������
    MEMORY_APP_CMD_ALARM_W,            //д����
    MEMORY_APP_CMD_DOORGROUP_R,        //������
    MEMORY_APP_CMD_DOORGROUP_W,        //д����
    MEMORY_APP_CMD_CARDGROUP_LIMIT_R,  //������Ȩ��
    MEMORY_APP_CMD_CARDGROUP_LIMIT_W,  //д����Ȩ��
    MEMORY_APP_CMD_AREA_R,             //������
    MEMORY_APP_CMD_AREA_W,             //д����
    MEMORY_APP_CMD_HOLIDAYGROUP_R,     //���ڼ�����
    MEMORY_APP_CMD_HOLIDAYGROUP_W,     //д�ڼ�����
    MEMORY_APP_CMD_TIMEGROUP_R,        //��ʱ������
    MEMORY_APP_CMD_TIMEGROUP_W,        //дʱ������
    MEMORY_APP_CMD_USER_PERMISSION_R,  //���û�
    MEMORY_APP_CMD_NEW_USER_W,         //�½���ͨ�û�
    MEMORY_APP_CMD_MODY_USER_W,        //�޸���ͨ�û�
    MEMORY_APP_CMD_NEW_VISITOR_W,      //�½��ÿ��û�
    MEMORY_APP_CMD_MODY_VISITOR_W,     //�޸ķÿ��û�
    MEMORY_APP_CMD_CARD_RECORD_R,      //���¼���¼
    MEMORY_APP_CMD_CARD_RECORD_W,      //д�¼���¼
    MEMORY_APP_CMD_ALARM_RECORD_R,     //��������¼
    MEMORY_APP_CMD_ALARM_RECORD_W,     //д������¼
    //
    MEMORY_APP_CMD_REVERSE_R,          //������
    MEMORY_APP_CMD_REVERSE_W,          //д����
    MEMORY_APP_CMD_REVERSE_C,          //�巴��
    //
    MEMORY_APP_CMD_LOGICBLOCK_INOUTAND_R, //�����������
    MEMORY_APP_CMD_LOGICBLOCK_INOUTAND_W, //д���������
    /*
    MEMORY_APP_CMD_LOGICBLOCK_INPUT_R, //���߼�ģ��֮����ģ��
    MEMORY_APP_CMD_LOGICBLOCK_INPUT_W, //д�߼�ģ��֮����ģ��
    MEMORY_APP_CMD_LOGICBLOCK_LOGIC_R, //���߼�ģ��֮�߼�ģ��
    MEMORY_APP_CMD_LOGICBLOCK_LOGIC_W, //д�߼�ģ��֮�߼�ģ��
    MEMORY_APP_CMD_LOGICBLOCK_OUTPUT_R,//���߼�ģ��֮���ģ��
    MEMORY_APP_CMD_LOGICBLOCK_OUTPUT_W,//д�߼�ģ��֮���ģ��
    */
    //����ָ��
    MEMORY_APP_CMD_FORMAT,             //��ʽ��EEPROM��FLASH,��д�����ͷ��Ϣ
    MEMORY_APP_CMD_INIT,               //��ʼ��IIC��SPI�����DMA
    //EEPROM��ʼ������
    MEMORY_APP_CMD_EEPROM_INIT_TEST,   //�����ʼ������
    //FLASH��ʼ������
    MEMORY_APP_CMD_FLASH_INIT_TEST,    //FLASH��ʼ������
};
enum MEMORY_APP_ERR
{
    MEMORY_APP_ERR_NO=OK,               //�޴���
    MEMORY_APP_ERR_OPERATION=ERR,       //��д��������
    MEMORY_APP_ERR_SUM,                 //��д��ȷ,���ǽṹ��У�����
    MEMORY_APP_ERR_PARA,                //��������
    MEMORY_APP_ERR_VOID_RETURN,         //����Ч����
    MEMORY_APP_ERR_FIRST,               //ִ�����״γ�ʼ��
};
//---------------------------------------------------
//enum MEMORY_TEST_CMD
//{
#define Memory_TEST_READ_BASEINFO       0x00000001  //ֻ��-�ſ���������Ϣ
#define Memory_TEST_READ_CHECK          0x00000002  //ֻ��-У����Ϣ
#define Memory_TEST_READ_DOORG          0x00000004  //ֻ��-У����Ϣ
#define Memory_TEST_READ_CARDG          0x00000008  //ֻ��-У����Ϣ
#define Memory_TEST_READ_AREA           0x00000010  //ֻ��-������Ϣ
#define Memory_TEST_READ_HOLIDAYGROUP   0x00000020  //ֻ��-�ڼ�������Ϣ
#define Memory_TEST_READ_TIMEGROUP      0x00000040  //ֻ��-ʱ������Ϣ
#define Memory_TEST_READ_USER           0x00000080  //ֻ��-������Ϣ
#define Memory_TEST_WR_TYPE             0x00000100  //д��-�汾��д����
#define Memory_TEST_WR_TIMEGROUP        0x00000200  //д��-ʱ�����д����
#define Memory_TEST_WR_HOLIDAYGROUP     0x00000400  //д��-�ڼ������д����
#define Memory_TEST_WR_CARDRECORD       0x00000800  //д��-ˢ����¼��Ϣ���޶�д����
#define Memory_TEST_WR_ALARMRECORD      0x00001000  //д��-������¼��Ϣ���޶�д����
#define Memory_TEST_WR_USER_NEW         0x00002000  //д��-�û���Ϣ���޶�д����(��ʱ����---����OK)
#define Memory_TEST_WR_VISITOR_NEW      0x00004000  //д��-�ÿͼ��޶�д����(��ʱ����)
#define Memory_TEST_WR_REVERSE          0x00100000  //д��-��������(������,���ǹ���������)
#define Memory_TEST_READ_EVENT          0x00200000  //ֻ��-�¼���¼
#define Memory_TEST_READ_ALARM          0x00400000  //ֻ��-������¼
#define Memory_TEST_READ_DOOR           0x00800000  //ֻ��-ȡ��(������Ϣ+����λͼ+����λͼ+������+�̵���+����)
#define Memory_TEST_WR_EEPROM           0x10000000  //д��-EEPROM����оƬ����
#define Memory_TEST_WR_FLASH            0x20000000  //д��-FLASH����оƬ����(��ʱ����)
#define Memory_TEST_PRE_CONF            0x40000000  //Ԥ������
#define Memory_TEST_FLASH_FORMAT        0x80000000  //д  -FLASH������ʽ������
//};
//---------------------------------------------------
//enum MEMORY_FORMAT_CODE
//{
#define MEMORY_FORMAT_ALARM_RECORD     0x00000001
#define MEMORY_FORMAT_CARD_RECORD      0x00000002
#define MEMORY_FORMAT_VISITOR          0x00000004
#define MEMORY_FORMAT_USER1            0x00000008
#define MEMORY_FORMAT_USER2            0x00000010
#define MEMORY_FORMAT_USER3            0x00000020
#define MEMORY_FORMAT_USER4            0x00000040
#define MEMORY_FORMAT_USER5            0x00000080
#define MEMORY_FORMAT_USER6            0x00000100
#define MEMORY_FORMAT_USER7            0x00000200
#define MEMORY_FORMAT_USER8            0x00000400
#define MEMORY_FORMAT_USER9            0x00000800
#define MEMORY_FORMAT_USER10           0x00001000
#define MEMORY_FORMAT_USER11           0x00002000
#define MEMORY_FORMAT_USER12           0x00004000
#define MEMORY_FORMAT_USER13           0x00008000
#define MEMORY_FORMAT_USER14           0x00010000
#define MEMORY_FORMAT_USER15           0x00020000
#define MEMORY_FORMAT_USER16           0x00040000
#define MEMORY_FORMAT_TIMEG            0x00080000
#define MEMORY_FORMAT_HOLIDAYG         0x00100000
#define MEMORY_FORMAT_DOOR             0x00200000
#define MEMORY_FORMAT_DOORG            0x00400000
#define MEMORY_FORMAT_CARDG            0x00800000
#define MEMORY_FORMAT_AREA             0x01000000
//#define MEMORY_FORMAT_DOORGB           0x02000000
#define MEMORY_FORMAT_CARDGB           0x04000000
#define MEMORY_FORMAT_READER           0x08000000
#define MEMORY_FORMAT_RELAY            0x10000000
#define MEMORY_FORMAT_INPUT            0x20000000
#define MEMORY_FORMAT_INOUTAND         0x40000000
#define MEMORY_FORMAT_ALL              0xFFFFFFFF
//};
//---------------------------------------------------
//-------------------------------------------------------------------------------�������Ͷ���
//�汾
typedef struct S_MEMORY_VER
{
    INT16U ProductionDate;       //��������
    INT16U SerialNumber;         //���
    INT32U ProductCode;          //��Ʒ����(����)
    INT16U HardwareVer;          //Ӳ���汾��
    INT16U SoftwareVer;          //����汾��
    INT16U MemoryMainVer;        //�洢�ṹ���汾��
    INT16U MemorySubVer;         //�洢�ṹ�Ӱ汾��
    INT8U Type;                  //�ſ�������(1,2,4)
    INT8U addr;                  //��ַ(1-8)
    INT16U BaudRate;             //
    //----------
    INT8U Reserve[MEMORY_VER_SIZE-MEMORY_VER_LEN-2]; //����
    INT16U Sum;                  //У���
} S_MEMORY_VER;
//У��ṹ��(ֻ����У��)
typedef struct S_CHECK
{
    INT32U SerialNumber;
    INT16U Number;
    INT16U CheckSum;
} S_CHECK;
//У��
enum MEMORY_APP_CHECK_ENUM
{
    MEMORY_APP_CHECK_ALL=0,
    MEMORY_APP_CHECK_CARD_RECORD,
    MEMORY_APP_CHECK_ALARM_RECORD,
    MEMORY_APP_CHECK_DOOR,
    MEMORY_APP_CHECK_DOORGROUP,
    MEMORY_APP_CHECK_CARDGROUP,
    MEMORY_APP_CHECK_ALARM,
    MEMORY_APP_CHECK_AREA,
    MEMORY_APP_CHECK_HOLIDAYGROUP,
    MEMORY_APP_CHECK_TIMEGROUP,
    MEMORY_APP_CHECK_CARD_VISITOR,
    MEMORY_APP_CHECK_CARD_USER1,
    MEMORY_APP_CHECK_CARD_USER2,
    MEMORY_APP_CHECK_CARD_USER3,
    MEMORY_APP_CHECK_CARD_USER4,
    MEMORY_APP_CHECK_CARD_USER5,
    MEMORY_APP_CHECK_CARD_USER6,
    MEMORY_APP_CHECK_CARD_USER7,
    MEMORY_APP_CHECK_CARD_USER8,
    MEMORY_APP_CHECK_CARD_USER9,
    MEMORY_APP_CHECK_CARD_USER10,
    MEMORY_APP_CHECK_CARD_USER11,
    MEMORY_APP_CHECK_CARD_USER12,
    MEMORY_APP_CHECK_CARD_USER13,
    MEMORY_APP_CHECK_CARD_USER14,
    MEMORY_APP_CHECK_CARD_USER15,
    MEMORY_APP_CHECK_CARD_USER16,
    MEMORY_APP_CHECK_READER,
    MEMORY_APP_CHECK_RELAY,
    MEMORY_APP_CHECK_INPUT,
    //MEMORY_APP_CHECK_DOORG_BITMAP,
    MEMORY_APP_CHECK_LOGICBLOCK_INOUTAND,
    MEMORY_APP_CHECK_CARDG_BITMAP,
    //
    /*
    MEMORY_APP_CHECK_LOGICBLOCK_INPUT,
    MEMORY_APP_CHECK_LOGICBLOCK_LOGIC,
    MEMORY_APP_CHECK_LOGICBLOCK_OUTPUT,
    */
};
typedef struct S_MEMORY_CHECK
{
    INT16U CardRecordNumber;           //ˢ����¼����
    INT16U AlarmRecordNumber;          //������¼����
    S_CHECK Door;
    S_CHECK DoorGroup;
    S_CHECK CardGroup;
    S_CHECK Alarm;
    S_CHECK Area;
    S_CHECK HolidayGroup;
    S_CHECK TimeGroup;
    S_CHECK Card_Visitor;              //��0(��ʱ)
    S_CHECK Card_User[16];             //��1-��16
    S_CHECK Reader;
    S_CHECK Relay;
    S_CHECK Input;
    S_CHECK DoorGBitmap;
    S_CHECK CardGBitmap;
    S_CHECK LogicBlockInput;
    S_CHECK LogicBlockLogic;
    S_CHECK LogicBlockOutput;
    //----------
    INT8U Reserve[MEMORY_CHECK_SIZE-MEMORY_CHECK_LEN-2];
    INT16U Sum;
} S_MEMORY_CHECK;
//������Ϣ
typedef struct S_MEMORY_KEEPPARA
{
    INT16U TransferAlarmTxNum;         //���ͼ���
    INT16U TransferAlarmTxLastNum;     //��һ�η��ͼ���
    INT16U TransferEventTxNum;         //���ͼ���
    INT16U TransferEventTxLastNum;     //��һ�η��ͼ���
    INT8U MonitorSign[4];    
    INT32U DoorFirstTime[2];           //�׿�ˢ��Unixʱ��(0xFFFFFFFF��Ч)
    INT8U ReaderLockBitmap;            //����������λͼ(0-δ����,1-����)
    INT8U PowerDownCount;
    //----------
    INT8U Reserve[MEMORY_KEEPPARA_SIZE-MEMORY_KEEPPARA_LEN-2];
    INT16U Sum;
} S_MEMORY_KEEPPARA;
//������Ϣ
typedef struct S_MEMORY_CONTROL
{
    INT8U DeviceEnable;                 //�豸ʹ��
    INT8U ForceCountMode;               //в��ģʽ
    INT8U ReaderEnableBitmap;           //Τ���ź�ʹ��λͼ
    INT8U ReaderPasswordEnableBitmap;   //��������������ʹ��λͼ
    INT8U ReaderTimeGroupEnableBitmap;  //������׼��ʱ��ʹ��λͼ
    INT8U ReaderManyCardOpenDoorEnableBitmap; //�������࿨����ʹ��λͼ
    //----------
    INT8U Reserve[MEMORY_CONTROL_SIZE-MEMORY_CONTROL_LEN-2];
    INT16U Sum;
} S_MEMORY_CONTROL;
//��
enum E_MEMORY_DOOR_FUNCTION
{
   E_MEMORY_DOOR_FUNCTION_DIR=0,       //����(0����,1˫��)
   E_MEMORY_DOOR_FUNCTION_ENABLE,      //��ʹ��
   E_MEMORY_DOOR_FUNCTION_SELFLOCK,    //����ʹ��
   E_MEMORY_DOOR_FUNCTION_LONG_OPENDOOR_ALARM, //��ʱ�䲻���ű���ʹ��
   E_MEMORY_DOOR_FUNCTION_NO_CONTINUOUS,       //��������������ʹ��
   E_MEMORY_DOOR_FUNCTION_MA,          //�Ŵ�ʹ��
   E_MEMORY_DOOR_FUNCTION_OPENDOOR_BUTTON,      //���Ű�ťʹ��
   };
typedef struct S_MEMORY_DOOR
{
    //-----��������-----
    INT8U Number;                   //���
    INT8U InDoorNumber[2];          //����������������
    INT8U OutDoorNumber[2];         //����������������
    INT8U FunctionSelect;           //����ѡ��
    INT8U DoorGroup[2];             //����������(2�ֽڱ�ʾһ������)
    INT8U LongNoCloseDoorTimeS;     //��ʱ�䲻���ű���ʱ��
    //------------------
    INT8U Reserve[MEMORY_DOOR_SIZE-MEMORY_DOOR_LEN-2];              //����
    INT16U Sum;                     //У���
} S_MEMORY_DOOR;
//����λͼ
/*
typedef struct S_MEMORY_DOORGBITMAP
{
    INT8U Number;                   //���
    INT8U Bitmap[128];              //λͼ
    //------------------
    INT8U Reserve[MEMORY_DOORGBITMAP_SIZE-MEMORY_DOORGBITMAP_LEN-2];       //����
    INT16U Sum;                     //У���
} S_MEMORY_DOORGBITMAP;
*/
//����λͼ
typedef struct S_MEMORY_CARDG_BITMAP
{
    INT8U Number;        //���
    INT8U Bitmap[128];   //λͼ
    //------------------
    INT8U Reserve[MEMORY_CARDGBITMAP_SIZE-MEMORY_CARDGBITMAP_LEN-2];              //����
    INT16U Sum;                     //У���
} S_MEMORY_CARDG_BITMAP;
//������
typedef struct S_MEMORY_READER
{
    INT8U Number;                    //���
    INT8U InterfaceType;             //0-Τ��26;1-Τ��34
    INT8U AuthenticationType;        //0-��,1-��+��,2-��/��,3-��,4-�࿨,5-�࿨+��
    INT8U PasswordRetryCount;        //�������Դ���
    INT8U PasswordMaxInputTime_s;    //�����������ʱ��(��)
    INT8U Password[4];               //����
    INT8U PasswordErrLockTime_s;     //�����������ʱ��(��)
    INT8U EffectTimeGroupNumber;     //��Чʱ����
    INT8U functionOption;            //����ѡ��
    INT8U FirstCardEffectTime_m[2];  //�׿���Чʱ��(����)
    INT8U CardG1Num[2];              //����1���
    INT8U CardG1Sum;                 //����1����
    INT8U CardG2Num[2];              //����1���
    INT8U CardG2Sum;                 //����1����
    INT8U CardG3Num[2];              //����1���
    INT8U CardG3Sum;                 //����1����
    //------------------
    INT8U Reserve[MEMORY_READER_SIZE-MEMORY_READER_LEN-2];          //����
    INT16U Sum;                      //У���
} S_MEMORY_READER;
//�̵���
typedef struct S_MEMORY_RELAY
{
    INT8U Number;                    //���
    //-----�̵�������-----
    INT8U Relay_Mode;               //ģʽ:0����/1����
    INT8U Relay_OpenDoorKeepTime;   //���ű���ʱ��
    INT8U Relay_ExtendOpenDoorTime; //�ӳ�����ʱ��
    //INT8U Relay_EffectTimeGroupNumber;           //�̵�����Чʱ��������
    INT8U Relay_LinkageRelayGroupNumber_Global;  //�����̵�����_ȫ��
    INT8U Relay_LinkageRelayGroupNumber_Local;   //�����̵�����-����
    //------------------
    INT8U Reserve[MEMORY_RELAY_SIZE-MEMORY_RELAY_LEN-2];              //����
    INT16U Sum;                     //У���
} S_MEMORY_RELAY;
//����
typedef struct S_MEMORY_INPUT
{
    INT8U Number;                    //���
    //-----��������-----
    INT8U Input_LineModeType;                    //��ģ������
    INT8U Input_FunctionSelect;                  //b0��Ч״̬+b1ʹ��
    INT8U Input_DelayTime_s;                     //�����ӳ�ʱ��
    INT8U Input_KeepTime_s;                      //���뱣��ʱ��
    INT8U Input_DisableTimeGroupNumber;          //�������ʱ����
    INT8U Input_LinkageRelayGroupNumber_Global;  //�����̵�����_ȫ��
    INT8U Input_LinkageRelayGroupNumber_Local;   //�����̵�����-����
    //
    INT8U Reserve[MEMORY_INPUT_SIZE-MEMORY_INPUT_LEN-2];              //����
    INT16U Sum;                     //У���
} S_MEMORY_INPUT;
//����
typedef struct S_MEMORY_ALARM
{
    INT8U Common_RelayNumber;                     //���汨��-�̵������
    INT8U Common_LinkageRelayGroupNumber_Global;  //���汨��-����ȫ�̵ּ�������
    INT8U Common_LinkageRelayGroupNumber_Local;   //���汨��-�������ؼ̵�������
    INT8U Force_RelayNumber;                      //в�ȱ���-�̵������
    INT8U Force_LinkageRelayGroupNumber_Global;   //в�ȱ���-����ȫ�̵ּ�������
    INT8U Force_LinkageRelayGroupNumber_Local;    //в�ȱ���-�������ؼ̵�������
    INT8U Invade_RelayNumber;                     //���ֱ���-�̵������
    INT8U Invade_LinkageRelayGroupNumber_Global;  //���ֱ���-����ȫ�̵ּ�������
    INT8U Invade_LinkageRelayGroupNumber_Local;   //���ֱ���-�������ؼ̵�������
    INT8U Fault_RelayNumber;                      //���ϱ���-�̵������
    INT8U Fault_LinkageRelayGroupNumber_Global;   //���ϱ���-����ȫ�̵ּ�������
    INT8U Fault_LinkageRelayGroupNumber_Local;    //���ϱ���-�������ؼ̵�������
    INT8U Reverse_FunctionSelect;                 //b0����-�¼�����
    //b1����-����
    //b2��������-�������ؼ̵�������
    //----------
    INT8U Reserve[MEMORY_ALARM_SIZE-MEMORY_ALARM_LEN-2];
    INT16U Sum;
} S_MEMORY_ALARM;
//����
typedef struct S_MEMORY_DOOR_GROUP
{
    INT8U Number[2];                //���
    INT8U FunctionSelect;        //b0:�����ڻ���,b1-b7:����
    //----------
    INT8U Reserve[MEMORY_DOORGROUP_SIZE-MEMORY_DOORGROUP_LEN-2];
    INT16U Sum;                  //У���
} S_MEMORY_DOOR_GROUP;
//����Ȩ��
typedef struct S_MEMORY_CARDGROUP_LIMIT
{
    INT8U Number[2];                   //���
    INT8U AreaGroupNumber[4];       //��������
    INT8U WayId[2];                 //��·ID
    INT8U RedioSAB;                 //��Ǳ�ع��ܵ�ѡ
    INT8U TimeGroupNumber;          //ʱ��������
    //----------
    INT8U Reserve[MEMORY_CARDGROUPLIMIT_SIZE-MEMORY_CARDGROUPLIMIT_LEN-2];          //����
    INT16U Sum;                     //У���
} S_MEMORY_CARDGROUP_LIMIT;
//����
typedef struct S_MEMORY_AREA
{
    INT8U Number[2];                 //���
    INT8U AreaGroupNumber;        //��������
    INT8U TimeGroupNumber;        //ʱ������ID
    INT8U SafetyLevel;            //��ȫ�ȼ�
    INT8U PeopleMax[2];             //��������
    INT8U PeopleMin[2];             //��������
    INT8U FunctionSelect;         //����ѡ��:
    //b0-�������ż以��ʹ��
    //b1-�������ڻ���ʹ��
    //----------
    INT8U InterlockArea[2];       //��������
    INT8U Reserve[MEMORY_AREA_SIZE-MEMORY_AREA_LEN-2];
    INT16U Sum;                   //У���
} S_MEMORY_AREA;
//�ڼ�����
typedef struct S_MEMORY_HOLIDAYGROUP
{
    INT8U Number;                //���
    INT8U DayBitMap[48];         //����λͼ
    //----------
    INT8U Reserve[MEMORY_FLASH_HOLIDAYGROUP_SIZE-MEMORY_FLASH_HOLIDAYGROUP_LEN-2];
    INT16U Sum;
} S_MEMORY_HOLIDAYGROUP;
//ʱ������
typedef struct S_MEMORY_TIMEGROUP
{
    INT8U Number;                //���    
    INT8U HolidayGroupNumber;    //�ڼ�������
    INT8U EffectBeginTime[5];    //ʱ��
    INT8U EffectEndTime[5];      //ʱ��
    INT8U TimeBitmap[7][12];     //��n����ʱ��λͼ(96bit,15min/bit)
    //----------
    INT8U Reserve[MEMORY_FLASH_TIMEGROUP_SIZE-MEMORY_FLASH_TIMEGROUP_LEN-2];
    INT16U Sum;                  //У���
} S_MEMORY_TIMEGROUP;

//�û��洢
typedef struct S_MEMORY_USER_PERMISSION
{
    INT8U CardNumber[4];        //����
    INT8U CardGroupNumber[2];    //������
    INT8U EffectBeginTime[5]; //ʱ��
    INT8U EffectEndTime[5];   //ʱ��
    INT8U Password[4];        //
    INT8U Status;             //״̬(��/0-��Ч/1-��ʧ/2-��/3-����/4-����/5-������ 0xFF-�ѱ�ɾ��)
    INT8U Type;               //������
    INT8U Privilege[2];       //��Ȩ
    INT8U PassLevel;          //ͨ�еȼ�
    INT8U ForceLevel;         //в�ȵȼ�
    //----------
    INT8U Reserve[MEMORY_FLASH_USER_PERMISSION_SIZE-MEMORY_FLASH_USER_PERMISSION_LEN-2];   //����
    INT16U Sum;
} S_MEMORY_USER_PERMISSION;
//ˢ����¼�洢
typedef struct S_MEMORY_CARD_RECORD
{
    //INT8U Number[2];             //���
    INT8U AlarmArea[2];          //��������
    INT8U ReaderOrWireOrRelayNum;//������/��ģ��/�̵������
    INT8U CardNumber[4];         //����
    INT8U TargetArea[2];         //Ŀ������
    INT8U CurrentArea[2];        //��ǰ����(�ſ����̶���ֵΪ0)
    INT8U Event[2];              //�¼�(������Ƭ״̬)
    INT8U TimeGErr;              //Υ����ʱ������
    INT8U RecordTime[5];         //ʱ��
    //----------
    INT8U Reserve[32-MAX_CARD_RECORD_STRUCT_BYTE_LEN];
    INT16U Sum;
} S_MEMORY_CARD_RECORD;
//������¼�洢
typedef struct S_MEMORY_ALARM_RECORD
{
    //INT8U Number[2];             //���
    INT8U AlarmArea[2];          //��������
    INT8U ReaderOrWireOrRelayNum;//������/��ģ��/�̵������
    INT8U CardNumber[4];         //����
    INT8U TargetArea[2];         //Ŀ������
    INT8U CurrentArea[2];        //��ǰ����(�ſ����̶���ֵΪ0)
    INT8U Event[2];              //�¼�(������Ƭ״̬)
    INT8U TimeGErr;              //Υ����ʱ������
    INT8U RecordTime[5];         //ʱ��
    //----------
    INT8U Reserve[32-MAX_ALARM_RECORD_STRUCT_BYTE_LEN];
    INT16U Sum;
} S_MEMORY_ALARM_RECORD;
//�߼�ģ��-->���������
typedef struct S_MEMORY_LOGICBLOCK_INOUTAND
{
   INT8U Number;                 //���  
   INT8U Input1Type;             //����1�Ĵ���״̬(0-ʼ����Ч,1-�͵�ƽ,2-�ߵ�ƽ,3-�½���,4-������)
   INT8U Input1Time_100ms;       //����1�ı���/ȥ��ʱ��(100mS)
   INT8U Input2Type;             //����2�Ĵ���״̬(0-ʼ����Ч,1-�͵�ƽ,2-�ߵ�ƽ,3-�½���,4-������)
   INT8U Input2Time_100ms;       //����2�ı���/ȥ��ʱ��(100mS)
   INT8U Input3Type;             //����3�Ĵ���״̬(0-ʼ����Ч,1-�͵�ƽ,2-�ߵ�ƽ,3-�½���,4-������)
   INT8U Input3Time_100ms;       //����3�ı���/ȥ��ʱ��(100mS)
   INT8U Input4Type;             //����4�Ĵ���״̬(0-ʼ����Ч,1-�͵�ƽ,2-�ߵ�ƽ,3-�½���,4-������)
   INT8U Input4Time_100ms;       //����4�ı���/ȥ��ʱ��(100mS)
   INT8U Output1OnTimeS[2];      //���1�Ŀ���ʱ��(��)
   INT8U Output1_TimeG;          //���1����Чʱ������
   INT8U Output2OnTimeS[2];      //���2�Ŀ���ʱ��(��)
   INT8U Output2_TimeG;          //���2����Чʱ������
   INT8U Output3OnTimeS[2];      //���3�Ŀ���ʱ��(��)
   INT8U Output3_TimeG;          //���3����Чʱ������
   INT8U Output4OnTimeS[2];      //���4�Ŀ���ʱ��(��)
   INT8U Output4_TimeG;          //���4����Чʱ������
   INT8U AlarmID[2];             //����ID
   //----------
   INT8U Reserve[MEMORY_LOGICBLOCK_INOUTAND_SIZE-MEMORY_LOGICBLOCK_INOUTAND_LEN-2];
   INT16U Sum;
}S_MEMORY_LOGICBLOCK_INOUTAND;
//�߼�ģ��-->����ģ������
/*
typedef struct S_MEMORY_LOGICBLOCK_INPUT
{   
   INT8U Number[2];     //���
   INT8U InBitmap;      //����λͼ(bit7~2-����,bit1-�ߵ�ƽ/�͵�ƽ��Ч,bit0-ʹ��/����)
   INT8U Timer;         //���뱣��nSʱ���,�ٴ������.
   INT8U TimeG;         //��Чʱ���� 
   //----------
   INT8U Reserve[MEMORY_LOGICBLOCK_INPUT_SIZE-MEMORY_LOGICBLOCK_INPUT_LEN-2];
   INT16U Sum;
} S_MEMORY_LOGICBLOCK_INPUT;
//�߼�ģ��-->�߼�ģ������
typedef struct S_MEMORY_LOGICBLOCK_LOGIC
{
   INT8U Number[2];     //���
   INT8U ID_Input1[2];    //ID-����1
   INT8U ID_Input2[2];    //ID-����2
   INT8U InBitmap;      //����λͼ(bit7~4-����,bit3~1-��/��/��/ͬ��/���,bit0-ʹ��/����)
   //
   INT8U Reserve[MEMORY_LOGICBLOCK_LOGIC_SIZE-MEMORY_LOGICBLOCK_LOGIC_LEN-2];
   INT16U Sum;
} S_MEMORY_LOGICBLOCK_LOGIC;
//�߼�ģ��-->���ģ������
typedef struct S_MEMORY_LOGICBLOCK_OUTPUT
{   
   INT8U Number[2];     //���
   INT8U ID_Input[2];     //ID-����
   INT8U RelayBitmap[2];  //�̵���λͼ
   INT8U AlarmID[2];      //����ID
   INT8U InBitmap;      //����λͼ(bit7~1-����,bit0-ʹ��/����)
   //
   INT8U Reserve[MEMORY_LOGICBLOCK_OUTPUT_SIZE-MEMORY_LOGICBLOCK_OUTPUT_LEN-2];
   INT16U Sum;
} S_MEMORY_LOGICBLOCK_OUTPUT;
*/
//�������洢
typedef struct S_MEMORY_REVERSE
{
    INT8U CardNumber[4];    //����
    INT8U State;            //0-δ֪,1-����,2-������,3-����
} S_MEMORY_REVERSE;
//
extern S_MEMORY_VER *p_Ver;
extern S_CHECK *p_Check;
extern S_MEMORY_CHECK *p_MemoryCheck;
extern S_MEMORY_CONTROL *p_Control;
extern S_MEMORY_ALARM *p_Alarm;
extern S_MEMORY_DOOR *p_Door;
//extern S_MEMORY_DOORGBITMAP *p_DoorGBitmap;
extern S_MEMORY_CARDG_BITMAP *p_CardGBitmap;
extern S_MEMORY_READER *p_Reader;
extern S_MEMORY_RELAY *p_Relay;
extern S_MEMORY_INPUT *p_Input;
extern S_MEMORY_DOOR_GROUP *p_DoorG;
extern S_MEMORY_CARDGROUP_LIMIT *p_CardG;
extern S_MEMORY_AREA *p_Area;
extern S_MEMORY_HOLIDAYGROUP *p_HolidayG;
extern S_MEMORY_TIMEGROUP *p_TimeG;
extern S_MEMORY_USER_PERMISSION *p_User;
extern S_MEMORY_ALARM_RECORD *p_AlarmRecord;
extern S_MEMORY_CARD_RECORD *p_CardRecord;
extern S_MEMORY_REVERSE *p_Reverse;
extern S_MEMORY_LOGICBLOCK_INOUTAND *p_LogicBlockInOutAnd;
extern INT8U Memory_RecordNum_ms;
//-------------------------------------------------------------------------------��������
extern void Memory_1msPro(void);
extern INT8U Memory_AppFunction(INT16U Cmd,INT8U *Buf,INT32U Number,INT32U SerialNumber);
extern void Memory_TEST(INT32U cmd);
#endif
//-------------------------------------------------------------------------------

