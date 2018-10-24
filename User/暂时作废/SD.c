/*******************************************************************************
使用说明:
1,更改SD.h宏定义. 
2,更改SD_GPIO()
3,更改SPI_Config()
4,更改INT8U SB(INT8U s);

*******************************************************************************/
//------------------------------------------------------------------------------
#define SD_GLOBAL
#include "includes.h"
//------------------------------------------------------------------------------用户变量
INT8U SPIBuffer[514];	//SPI缓冲区
INT8U CSD[18];				//SD-CSD
INT8U CID[18];				//SD-CID
INT8U SDValid;				//标志-SD卡有效
INT32U  MaxBlockNum=0;
//------------------------------------------------------------------------------用户函数

//------------------------------------------------------------------------------Private Functions
void Delayus(int t)
{
  int i,j;
  for(i=0;i<t;i++)
    for(j=0;j<5;)
      j++;
}
void SD_TEST_StrOut(INT8U *str)
{
#ifdef SD_TEST
	Display_Str(str);
#endif
}
/*******************************************************************************
函数功能: SD_GPIO初始化
参    数: 无
返 回 值: 无
编程人员: 徐松亮
*******************************************************************************/
void SD_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); 
  // SD_CS 推挽
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // SPI1_CLK ,SPI_MOSI 复用推挽
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // SPI1_MISO (PA.6) 上拉输入
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*******************************************************************************
函数功能: SPI初始化
参    数: 无
返 回 值: 无
编程人员: 徐松亮
*******************************************************************************/
void SPI_Config(void)
{
  SPI_InitTypeDef SPI_InitStructure;
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  
	//初始化 SPI1
  SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode=SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL=SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;
  SPI_Init(SPI1,&SPI_InitStructure);
  //CRC 校验禁用
  SPI_CalculateCRC(SPI1,DISABLE);
  //使能 SPI1 
  SPI_Cmd(SPI1,ENABLE);
}
/*******************************************************************************
函数功能: SPI发送一个字节
参    数: 要写入的字节
返 回 值: 返回接收到的字符
编程人员: 徐松亮
*******************************************************************************/
INT8U SB(INT8U s)
{
  INT16U timeout=0;
  while((SPI1->SR & SPI_I2S_FLAG_TXE)==0){}     //等待之前的发送结束                  
  SPI1->DR = (u16)s;                              //发送该字符       
  while ((SPI1->SR & SPI_I2S_FLAG_RXNE)==0)
  {
    timeout++;
    if(timeout>10000)
    {
      SD_TEST_StrOut("SB：没有接受到回复\n");
      return 0x00;
    }
  }
  return (INT8U)SPI1->DR;                   // 返回接收到的字符
}
/*******************************************************************************
函数功能: SPI繁忙检测
参    数: 无
返 回 值: 1 - 不繁忙
          0 - 繁忙
编程人员: 徐松亮
*******************************************************************************/
INT8U BusyWait(void)
{
  int i;
  for(i=0;i<1000;i++)
  {
    if(SB(0xFF)==0xFF)
      return 1;
    else
      Delayus(100);
  }
  SDValid=0;
  return 0;
}
/*******************************************************************************
函数功能: 向SD卡发送指令
参    数: CMD      - 指令
          blocknum - 块号
返 回 值: 
编程人员: 徐松亮
说    明: SD卡指令由6个字节组成:
          Byte1  : 指令(cmd|0x40),
					Byte2-5: 指令参数
					Byte6  : 前七位CRC,最后一位为停止位0
*******************************************************************************/
INT8U SDcmd(INT8U cmd, INT32U blocknum) 
{
  INT8U i;
  INT8U b;
  INT32U adr;
  INT8U M[5];
  adr = blocknum * 512;
  M[0]=cmd | 0x40;
  M[1]=(INT8U)(adr >> 24);
  M[2]=(INT8U)((adr >> 16) & 0x000000ff);
  M[3]=(INT8U)((adr >> 8) & 0x000000ff);
  M[4]=0;
  SPI_Select(); 
  BusyWait();
  b = SB(M[0]);
  b = SB(M[1]);
  b = SB(M[2]);
  b = SB(M[3]);
  b = SB(0);  
  b = SB(0x95);
  for(i=0;i<100;i++)
  {
    if(b!=0xFF)break;
    b=SB(0xFF);
  }
  return b;
}
/*******************************************************************************
函数功能: 复位SD卡
参    数: 无
返 回 值: 1 - 处于空闲状态
          0 - 没有进入空闲状态
编程人员: 徐松亮
*******************************************************************************/
INT8U GoIdle(void)
{
  INT8U b;
  int i,j;
  if(1==SDValid)return 1;
  for(j=0;j<10;j++)
  {
    SPI_Deselect();
    for(i=0;i<10;i++)
    {
      b=SB(0xFF);
    } 
    SPI_Select();
    b=SDcmd(CMD_GO_IDLE_STATE,0);
    if(b==R1_ERROR_IDLE) 
    {
      return 1;
    }
    Delayus(100);
  }
  return 0; 
}
/*******************************************************************************
函数功能: SD卡准备
参    数: 无
返 回 值: 1 - SD卡准备成功
          0 - SD卡准备失败
编程人员: 徐松亮
*******************************************************************************/
INT8U GoReady(void)
{
  INT8U b;
  int i,j;
  if(SDValid)
		{	//SD卡已经有效状态,返回
    	return 1;
		}
  for(j=0;j<10;j++)
    {	
      //指明要发送扩展指令
			b=SDcmd(CMD_APP_CMD,0);
      if(b==R1_OK)
        {	//SD卡已经准备完毕
          b=SB(0xFF);
          return 1;
        }
      if(b!=R1_ERROR_IDLE)
        {	//指令返回错误码
          if(b!=0xFF)
						{	//错误码报告
              R1Explain(b);
					  }
          continue;
        }
			//扩展指令
      b=SDcmd(ACMD_ACTIVATE_SD,0);   
      if(b==R1_OK) 
        { //SD卡已经准备完毕
          SDValid=1;
          (void)SB(0xFF);
          return 1;
        }        
      if(b!=R1_ERROR_IDLE)
        {
          if(b!=0xFF)
					  {	//错误码报告
              R1Explain(b);
						}
          continue;
        }  
      for(i=0;i<1000;i++)
        {  
          b=SB(0xFF);
          b=SDcmd(CMD_APP_CMD,0);	    //循环发送cmd55和cmd41，
          b=SB(0xFF);
          b=SDcmd(ACMD_ACTIVATE_SD,0);//直到接受到的回复是0  
          if(b == R1_OK) 
            { //成功
              SDValid=1;
              (void)SB(0xFF); 
              return 1;
            }
					else if(b != R1_ERROR_IDLE)
            {
              if(b!=0xFF)
							  {	//错误码报告
                  R1Explain(b);
							  }
              while(b!=0&&b!=0xFF)
                {
                  Display_Hex(b);
                  Display_Char(' ');
                  b=SB(0xFF);
                }
              Display_Char('\n');
              continue;
            }
        }
    }
  return 0;         
}
/*******************************************************************************
函数功能: 数据类错误码报告
参    数: rsp --- 错误码
返 回 值: 无
编程人员: 徐松亮
*******************************************************************************/
void DataErrExplain(INT8U rsp)
{
  if(!rsp)return;
  SD_TEST_StrOut("DataErrExplain:");
  Display_Hex(rsp);
  Display_Char('\n');
  if(rsp&0x01)SD_TEST_StrOut("错误！");
  if(rsp&0x02)SD_TEST_StrOut("SD卡内部错误！");
  if(rsp&0x04)SD_TEST_StrOut("ECC校验错误！");
  if(rsp&0x08)SD_TEST_StrOut("地址越界！");
  Display_Char('\n');
}
/*******************************************************************************
函数功能: R1类错误码报告
参    数: rsp --- 错误码
返 回 值: 无
编程人员: 徐松亮
*******************************************************************************/
void R1Explain(INT8U rsp)
{
  if(!rsp)return;
  SD_TEST_StrOut("R1Explain:");
  Display_Hex(rsp);
  Display_Char('\n');
  if(rsp & R1_ERROR_IDLE)          SD_TEST_StrOut("空闲状态！");
  if(rsp & R1_ERROR_ERASE_RESET)   SD_TEST_StrOut("擦除重置！");
  if(rsp & R1_ERROR_ILLEGAL_CMD)   SD_TEST_StrOut("非法指令！");
  if(rsp & R1_ERROR_CRC)           SD_TEST_StrOut("校验错误！");
  if(rsp & R1_ERROR_ERASE_SEQ)     SD_TEST_StrOut("删除错误！");
  if(rsp & R1_ERROR_ADDR_ERROR)    SD_TEST_StrOut("地址错误！");
  if(rsp & R1_ERROR_PARAMETER_ERR) SD_TEST_StrOut("参数错误！");
  Display_Char('\n');
}
/*******************************************************************************
函数功能: 检测是否有SD卡
参    数: 无
返 回 值: 1 - 有SD卡
          0 - 无SD卡
编程人员: 徐松亮
*******************************************************************************/
INT8U CheckAlive(void)
{
  int i;
  if(!SDValid)
  {	//SD卡目前无效
    for(i=0;i<3;i++)
    {
      SPI_Deselect();
      SD_TEST_StrOut("\n进入CheckAlive函数:检测并初始化SD卡...\n");
      //复位SD卡
			if(!GoIdle())
      {
        SD_TEST_StrOut("\nSD卡无法进入 Idle 状态.");
        continue;
      }
			//启动SD卡
      if(!GoReady())
      {
        SD_TEST_StrOut("\nSD无法进入 Ready 状态.");
        continue;
      }
			//SD卡有效
      SDValid=1;
      SD_TEST_StrOut("\nSD卡初始化完成.");
      return 1;	
    }
    return 0;	
  }
	//SD卡有效
  return 1;
}
/*******************************************************************************
函数功能: 读一个扇区的数据
参    数: BlockNum  ----- 扇区号
          *addr     ----- 数据缓存区 
返 回 值: 1 - OK
          0 - ERR
*******************************************************************************/
INT8U ReadBlock(INT32U BlockNum, INT8U *addr)
{
  INT8U b;
  INT16U i=0,j;
  for(j=0;j<3;j++)
    {
      if(!BusyWait())
        {
          if(!CheckAlive())
					  {
              return 0;		
						}
        }
      b=SB(0xFF);
      b=SDcmd(CMD_READ_SINGLE_BLOCK,BlockNum);
      if(b!=0)
        {
          SDValid=0;
          continue;
        }
      b=SB(0xFF);
      while(b==0xFF)//等待传送开始
        {
          b=SB(0xFF);
          i++;
          if(i>30000)
            {
              break;
            }
        }    
      if(b!=0xfe)
        { //强制停止传输
          b=SDcmd(12,0);
          continue;
        }
      for(i=0;i<512;i++)
        { 
          *(addr+i)=SB(0xFF);
        } 
			SB(0xFF);
			SB(0xFF);	 
      b=SB(0xFF);
      return 1;
    }
  SD_TEST_StrOut("读数据块：");
  Display_Num(BlockNum);
  SD_TEST_StrOut(" 发生错误！\n");   
  DataErrExplain(b);
  return 0;
}

INT8U WriteBlock(INT32U BlockNum, const INT8U *addr){
  INT8U b,r;
  INT16U i,j;
  for(j=0;j<3;j++)
  {
    if(!BusyWait())
      if(!CheckAlive())
        return 0;	

    b=SB(0xFF);
    b=SDcmd(24,BlockNum);		//CMD24 写数据块，默认长度为512字节，后跟2个字节的校验码
    if(b!=0)
    {
      SDValid=0;
      continue;
    }
    for(i=0;i<10;i++)
    {
      b=SB(0xFF);
    }		  
    b=SB(0xFE);		          //数据开始令牌
    for(i=0;i<512;i++)
    {            //发送数据
      b=SB(*(addr+i));
    }
		SB(0xFF);
		SB(0xFF);
    i=0;
    while(b==0xFF)
    {
      b=SB(0xFF); 
      if(++i>100)
      {
        SDValid=0;
        break;
      }
    }
    r=(b>>1)&0x07;
    if(r!=2)
    {
      Display_Char('\n'); 
      if(r==5)
        SD_TEST_StrOut("CRC校验错误，数据写入未成功，错误代码：");
      else if(r==6)
        SD_TEST_StrOut("数据写入发生错误，错误代买：");
      else
        SD_TEST_StrOut("未知错误：");
      Display_Hex(b);
      Display_Char('\n');
      SDValid=0;
      continue;
    }
    b=SB(0xFF);
    if(!BusyWait())
    {
        SD_TEST_StrOut("等待 BUSY 超时!\n");
    }
    b=SB(0xff);   
    GetStatus();
    return 1; 
  }
  SD_TEST_StrOut("SD卡发生错误！\n");
  return 0; 
}
INT8U FillBlock(INT32U BlockNum,INT8U chr)
{
  int i;
  for(i=0;i<514;i++)
  {
    SPIBuffer[i]=chr;
  }
  return WriteBlock(BlockNum, SPIBuffer);
}
/*******************************************************************************
函数功能: 获取SD的CSD信息
参    数: 无
返 回 值: 1 - 获取成功
          0 - 获取失败
编程人员: 徐松亮
*******************************************************************************/
INT8U GetCSD(void)
{
  INT8U i,b;
  SD_TEST_StrOut("读CSD寄存器:\n");
  if(!CheckAlive())
		{
		  return 0;	
		}
  (void)SB(0xFF);
  b=SDcmd(9,0);	
  if(b!=0)
    {
      return 0;
    }
  b=SB(0xFF);
  while(b==0xFF)
    {
      b=SB(0xFF);
    }	
  if(b!=0xFE)
    {
      DataErrExplain(b);
      return 0;
    }
  for(i=0;i<18;i++)
    {
      CSD[i]=SB(0xFF);
    }
  if(SB(0xFF)==0xFF)
    {
      Display_CSD();
      return 1;
    } 
  SD_TEST_StrOut("错误：CSD数据长度大于预期！");
  return 0;
}
/*******************************************************************************
函数功能: 显示CSD信息
参    数: 无
返 回 值: 1 - 获取成功
          0 - 获取失败
编程人员: 徐松亮
*******************************************************************************/
void Display_CSD(void)
{
  INT16U i,j;
  int C_SIZE;
  INT8U C_SIZE_MULT,READ_BL_LEN;
  INT32U SD_SIZE;
  SD_TEST_StrOut("CSD版本：1.");
  Display_Num(CSD[0]>>6);
  SD_TEST_StrOut("\n最大数据访问时间：");
  Display_Num(CSD[1]+CSD[2]*100);
  SD_TEST_StrOut("\n最高数据传输速率：");
  if((CSD[3]&0x03)==0)
    SD_TEST_StrOut("100Kbit/s");
  else if((CSD[3]&0x03)==1)
    SD_TEST_StrOut("1Mbit/s");
  else if((CSD[3]&0x03)==2)
    SD_TEST_StrOut("10Mbit/s");
  else if((CSD[3]&0x03)==3)
    SD_TEST_StrOut("100Mbit/s");
  SD_TEST_StrOut("\n读数据块长度：");
  j=1; 
  READ_BL_LEN=(CSD[5]&0x0f);
  for(i=0;i<READ_BL_LEN;i++)
    j*=2;
  Display_Num(j);
  SD_TEST_StrOut(" 字节\n");
  SD_TEST_StrOut("跨数据块写入：");
  if(!(CSD[6]&0x40))
    SD_TEST_StrOut("不");
  SD_TEST_StrOut("允许\n");
  SD_TEST_StrOut("跨数据块读取：");
  if(!(CSD[6]&0x20))
    SD_TEST_StrOut("不");
  SD_TEST_StrOut("允许\n");
  SD_TEST_StrOut("可用读写容量：");
  C_SIZE=(((INT16U)(CSD[6]&0x02))<<10)+(CSD[7]<<2)+(CSD[8]>>6);
  C_SIZE_MULT=((CSD[9]&0x02)<<1)+(CSD[11]>>7);
  SD_SIZE=C_SIZE+1;
  SD_SIZE=SD_SIZE<<(INT8U)(C_SIZE_MULT+READ_BL_LEN+2);
  Display_Num(SD_SIZE);
  SD_TEST_StrOut(" 字节\n");  
}
/*******************************************************************************
函数功能: 获取SD的CID信息
参    数: 无
返 回 值: 1 - 获取成功
          0 - 获取失败
编程人员: 徐松亮
*******************************************************************************/
INT8U GetCID(void)
{
  INT8U i,b;
	//检验SD卡是否已经被初始化   
  if(!CheckAlive())
		{
      return 0;	
		}
  SD_TEST_StrOut("\n读CID寄存器:");
  (void)SB(0xFF);//XSL认为无用
	//发送命令
  b=SDcmd(CMD_SEND_CID,0);
  if(b != R1_OK)
    {
      return 0;
    }
	//查找回应头0xFE
  b=SB(0xFF);
  while(b==0xFF)
    {
      b=SB(0xFF);
    }
  if(b!=0xFE)
	  {
      SD_TEST_StrOut("GetCID：没有数据传输！\n");
      DataErrExplain(b);
      return 0;
    }
	//读取CID
  for(i=0;i<18;i++)
	  {
      CID[i]=SB(0xFF);
    }
  if(SB(0xFF)==0xFF)
    {
      Display_CID();
      return 1;
    }
	else
		{	 
      SD_TEST_StrOut("错误：CID数据长度大于预期！");
      return 0;	 
		}
}
/*******************************************************************************
函数功能: 显示CID信息
参    数: 无
返 回 值: 1 - 获取成功
          0 - 获取失败
编程人员: 徐松亮
*******************************************************************************/
void Display_CID(void)
{
  INT16U i;
  SD_TEST_StrOut("制造商ID：");
  Display_Hex(CID[0]);
  Display_Char('\n');
  SD_TEST_StrOut("OEM ID：");
  Display_Hex(CID[1]);
  Display_Char(' ');
  Display_Hex(CID[2]);
  Display_Char('\n');
  SD_TEST_StrOut("产品名称：");
  for(i=0;i<5;i++){
    Display_Char(CID[3+i]);
  }
  Display_Char('\n');
  SD_TEST_StrOut("产品版本：V");
  Display_Num(CID[8]);
  Display_Char('\n');
  SD_TEST_StrOut("产品序列号：");
  for(i=0;i<4;i++){
    Display_Hex(CID[9+i]);
    Display_Char(' ');
  }
  Display_Char('\n');
}
/*******************************************************************************
函数功能: 设置块长度
参    数: 无
返 回 值: 1 - 获取成功
          0 - 获取失败
编程人员: 徐松亮
*******************************************************************************/
INT8U SetBlockLen(void)
{
  INT8U b;
  SD_TEST_StrOut("\n设置数据块长度为512");
  if(!CheckAlive())
    {
      SD_TEST_StrOut("\nSetBlockLen：当前SD卡无效.");
      return 0;
    }
  (void)SB(0xff);
  b=SDcmd(CMD_SET_BLOCKLEN,1);
  if(b)
    {
      R1Explain(b);
    }
  if(SB(0xff)==0xFF)
		{
      return 1;
		}
  GetStatus();
  return 0;
}
/*******************************************************************************
函数功能: 实际容量测试
参    数: 无
返 回 值: 无
编程人员: 徐松亮
*******************************************************************************/
void SD_TestSize(void){ 
  int r;
  INT32U high,low,mid;
  high=14000;
  SD_TEST_StrOut("测试SD卡的容量：\n");
  SD_TEST_StrOut("测试： ");
  Display_Num(high);
  SD_TEST_StrOut(" ...");
  r = ReadBlock(high, (INT8U *) & SPIBuffer);
  if(r==0)
    {
      SD_TEST_StrOut("SD卡容量小于8MB！\n");
      return;
    }
  while (r)
    {
      SD_TEST_StrOut("成功！\n测试： ");
      low = high;
      high = high <<1; 
      Display_Num(high);
      SD_TEST_StrOut(" ...");
      r = ReadBlock(high, (INT8U *) & SPIBuffer);
    }
    // 低地址可读，高地址不可读，尝试中间的地址
    mid = (low+high) >>1;
  do 
    {
      (void)GoReady(); 
      SD_TEST_StrOut("测试： ");
      Display_Num(mid);
      SD_TEST_StrOut(" ...");
      r = ReadBlock(mid, (INT8U *) & SPIBuffer);
      if (!r)
        {         // 中间地址不可读
          high = mid;	  	//降低高地址
        } 
      else
        {             // 中间地址可读
          low = mid;		  //则提高低地址
          SD_TEST_StrOut("成功！\n");
        } 
      mid = (low+high) >>1;
    }
  while(high>low+1);	//直到mid不再改变
  MaxBlockNum=mid;
  SD_TEST_StrOut("SD卡实际容量： ");
  Display_Num(mid*512);
  SD_TEST_StrOut(" 字节\n");
}
void GetStatus(void)
{
  INT8U r;
  INT16U state;
  r = SDcmd(13, 0); 
  state = SB(0xff);  
  state = ((INT16U) (r) << 8) | (INT16U) state;
  r=SB(0xFF);
}
/*******************************************************************************
函数功能: SD卡初始化
*******************************************************************************/
void SD_Init_Test(void)
{
  //GPIO初始化
	SD_GPIO();
	//SPI初始化
  SPI_Config();
  //SD卡有效标志清零
  SDValid=0;
  SD_TEST_StrOut("\n初始化SD卡...\n");
  //检测并初始化SD卡
  if(!CheckAlive())
	  {	//无卡返回
		  return;		
		}
  //获取CID(SD卡身份识别,只读)
  if(!GetCID())
  {
    SD_TEST_StrOut("CID无法获取！\n");
  }  
  //获取CSD(SD卡块定义,写保护区大小等)  
  if(!GetCSD()) 
  {
    SD_TEST_StrOut("CSD无法获取！\n");
  }     
  //设置数据块长度
  if(!SetBlockLen())
  {
    SD_TEST_StrOut("设置数据块长度失败！\n");
  }
  //测试实际容量
  SD_TestSize(); 
  //置SD卡有效标志
  SDValid=1;
  SD_TEST_StrOut("初始化SD卡完成！\n");
}
//--------------------------------------------------以下应用于FATFS
INT8U SD_Init(void)
{
	//GPIO初始化
	SD_GPIO();
	//SPI初始化
  SPI_Config();
  //SD卡有效标志清零
  SDValid=0;
  SD_TEST_StrOut("\n初始化SD卡...\n");
  //检测并初始化SD卡
  if(!CheckAlive())
	  {	//无卡返回
		  return 1;		
		}
  //获取CID(SD卡身份识别,只读)
  if(!GetCID())
    {
      SD_TEST_StrOut("CID无法获取！\n");
			return 1;
    }  
  //获取CSD(SD卡块定义,写保护区大小等)  
  if(!GetCSD()) 
    {
      SD_TEST_StrOut("CSD无法获取！\n");
			return 1;
    }     
  //设置数据块长度
  if(!SetBlockLen())
    {
      SD_TEST_StrOut("设置数据块长度失败！\n");
			return 1;
    }
  //测试实际容量
  SD_TestSize(); 
  //置SD卡有效标志
  SDValid=1;
  SD_TEST_StrOut("初始化SD卡完成！\n");
	return 0;
}
/******************************* END OF FILE **********************************/

