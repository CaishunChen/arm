//------------------------------------------------------------------------------
#ifdef SD_GLOBAL
#define SD_EXT
#else
#define SD_EXT extern
#endif
//------------------------------------------------------------------------------用户定义
#define SD_TEST
#define SPI_USED          SPI1
#define SPI_Select()    (GPIOA->ODR &= ~GPIO_Pin_4)
#define SPI_Deselect()  (GPIOA->ODR |= GPIO_Pin_4)
//------------------------------------------------------------------------------用户变量
//extern INT8U SDData[514];			//512字节数据缓存，2字节CRC16校验码
SD_EXT INT8U SPIBuffer[514];
SD_EXT INT8U CSD[18];	      //CSD寄存器，包含CRC16校验码
SD_EXT INT8U SDValid;         //SD卡是否有效
SD_EXT INT32U MaxBlockNum;
//用户定义(不用修改)
//-----命令定义
#define CMD_GO_IDLE_STATE 	     0
#define CMD_SEND_OP_COND 	       1
#define CMD_READ_CSD 		         9
#define CMD_SEND_CID 		         10
#define CMD_STOP_TRANSMISSION 	 12
#define CMD_SEND_STATUS 	       13
#define CMD_SET_BLOCKLEN 	       16
#define CMD_READ_SINGLE_BLOCK 	 17
#define CMD_READ_MULTIPLE_BLOCK  18
#define CMD_CMD_WRITEBLOCK 	     20
#define CMD_WRITE BLOCK 	       24
#define CMD_WRITE_MULTIPLE_BLOCK 25
#define CMD_WRITE_CSD 		       27
#define CMD_SET_WRITE_PROT 	     28
#define CMD_CLR_WRITE_PROT 	     29
#define CMD_SEND_WRITE_PROT 	   30
#define CMD_TAG_SECTOR_START 	   32
#define CMD_TAG_SECTOR_END 	     33
#define CMD_UNTAG_SECTOR 	       34
#define CMD_TAG_EREASE_GROUP_START 35
#define CMD_TAG_EREASE_GROUP_END   36
#define CMD_UNTAG_EREASE_GROUP 	   37
#define CMD_EREASE 		           38
#define CMD_READ_OCR 		         39
#define CMD_CRC_ON_OFF 		       40
#define CMD_APP_CMD 		         55  //下一条指令为APP特殊类类指令
//-----扩展指令
#define ACMD_ACTIVATE_SD         41  //使SD卡进入准备状态
//-----回应定义
#define R1_OK										 0x00
#define R1_ERROR_IDLE            0x01//空闲状态
#define R1_ERROR_ERASE_RESET		 0x02//擦除重置
#define R1_ERROR_ILLEGAL_CMD		 0x04//非法指令
#define R1_ERROR_CRC             0x08//校验错误
#define R1_ERROR_ERASE_SEQ       0x10//删除错误
#define R1_ERROR_ADDR_ERROR      0x20//地址错误
#define R1_ERROR_PARAMETER_ERR   0x40//参数错误
//用户函数(不用修改)
SD_EXT void SPI_Config(void);
SD_EXT void DataErrExplain(INT8U rsp);
SD_EXT void R1Explain(INT8U rsp);
SD_EXT INT8U SB(INT8U s);  
SD_EXT INT8U CRC7(INT8U *inf, int);
SD_EXT INT8U SDcmd(INT8U,INT32U);
SD_EXT INT8U GoIdle(void);
SD_EXT INT8U GoReady(void);
SD_EXT INT8U ReadBlock(INT32U BlockNum,INT8U *addr);
SD_EXT INT8U WriteBlock(INT32U BlockNum,const INT8U *addr);
SD_EXT INT8U FillBlock(INT32U BlockNum,INT8U chr);
SD_EXT INT8U GetCSD(void);
SD_EXT void GetStatus(void);
SD_EXT INT8U GetCID(void);
SD_EXT void Display_CID(void);
SD_EXT void Display_CSD(void);
SD_EXT void SD_TestSize(void);
SD_EXT INT8U SetBlockLen(void);
SD_EXT INT8U CheckAlive(void);
SD_EXT void delay(void);
SD_EXT void SD_Write_Delay(void);
SD_EXT void SD_CMD_Delay(void);
SD_EXT void SD_READ_CMD_Delay(void);
SD_EXT void SD_REG_CMD_Delay(void);
SD_EXT void SD_Init_Test(void);
//-----
SD_EXT INT8U SD_Init(void);
