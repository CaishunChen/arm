/*******************************************************************************
ʹ��˵��:
1,����SD.h�궨��. 
2,����SD_GPIO()
3,����SPI_Config()
4,����INT8U SB(INT8U s);

*******************************************************************************/
//------------------------------------------------------------------------------
#define SD_GLOBAL
#include "includes.h"
//------------------------------------------------------------------------------�û�����
INT8U SPIBuffer[514];	//SPI������
INT8U CSD[18];				//SD-CSD
INT8U CID[18];				//SD-CID
INT8U SDValid;				//��־-SD����Ч
INT32U  MaxBlockNum=0;
//------------------------------------------------------------------------------�û�����

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
��������: SD_GPIO��ʼ��
��    ��: ��
�� �� ֵ: ��
�����Ա: ������
*******************************************************************************/
void SD_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); 
  // SD_CS ����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // SPI1_CLK ,SPI_MOSI ��������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // SPI1_MISO (PA.6) ��������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*******************************************************************************
��������: SPI��ʼ��
��    ��: ��
�� �� ֵ: ��
�����Ա: ������
*******************************************************************************/
void SPI_Config(void)
{
  SPI_InitTypeDef SPI_InitStructure;
	//����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  
	//��ʼ�� SPI1
  SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode=SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL=SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;
  SPI_Init(SPI1,&SPI_InitStructure);
  //CRC У�����
  SPI_CalculateCRC(SPI1,DISABLE);
  //ʹ�� SPI1 
  SPI_Cmd(SPI1,ENABLE);
}
/*******************************************************************************
��������: SPI����һ���ֽ�
��    ��: Ҫд����ֽ�
�� �� ֵ: ���ؽ��յ����ַ�
�����Ա: ������
*******************************************************************************/
INT8U SB(INT8U s)
{
  INT16U timeout=0;
  while((SPI1->SR & SPI_I2S_FLAG_TXE)==0){}     //�ȴ�֮ǰ�ķ��ͽ���                  
  SPI1->DR = (u16)s;                              //���͸��ַ�       
  while ((SPI1->SR & SPI_I2S_FLAG_RXNE)==0)
  {
    timeout++;
    if(timeout>10000)
    {
      SD_TEST_StrOut("SB��û�н��ܵ��ظ�\n");
      return 0x00;
    }
  }
  return (INT8U)SPI1->DR;                   // ���ؽ��յ����ַ�
}
/*******************************************************************************
��������: SPI��æ���
��    ��: ��
�� �� ֵ: 1 - ����æ
          0 - ��æ
�����Ա: ������
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
��������: ��SD������ָ��
��    ��: CMD      - ָ��
          blocknum - ���
�� �� ֵ: 
�����Ա: ������
˵    ��: SD��ָ����6���ֽ����:
          Byte1  : ָ��(cmd|0x40),
					Byte2-5: ָ�����
					Byte6  : ǰ��λCRC,���һλΪֹͣλ0
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
��������: ��λSD��
��    ��: ��
�� �� ֵ: 1 - ���ڿ���״̬
          0 - û�н������״̬
�����Ա: ������
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
��������: SD��׼��
��    ��: ��
�� �� ֵ: 1 - SD��׼���ɹ�
          0 - SD��׼��ʧ��
�����Ա: ������
*******************************************************************************/
INT8U GoReady(void)
{
  INT8U b;
  int i,j;
  if(SDValid)
		{	//SD���Ѿ���Ч״̬,����
    	return 1;
		}
  for(j=0;j<10;j++)
    {	
      //ָ��Ҫ������չָ��
			b=SDcmd(CMD_APP_CMD,0);
      if(b==R1_OK)
        {	//SD���Ѿ�׼�����
          b=SB(0xFF);
          return 1;
        }
      if(b!=R1_ERROR_IDLE)
        {	//ָ��ش�����
          if(b!=0xFF)
						{	//�����뱨��
              R1Explain(b);
					  }
          continue;
        }
			//��չָ��
      b=SDcmd(ACMD_ACTIVATE_SD,0);   
      if(b==R1_OK) 
        { //SD���Ѿ�׼�����
          SDValid=1;
          (void)SB(0xFF);
          return 1;
        }        
      if(b!=R1_ERROR_IDLE)
        {
          if(b!=0xFF)
					  {	//�����뱨��
              R1Explain(b);
						}
          continue;
        }  
      for(i=0;i<1000;i++)
        {  
          b=SB(0xFF);
          b=SDcmd(CMD_APP_CMD,0);	    //ѭ������cmd55��cmd41��
          b=SB(0xFF);
          b=SDcmd(ACMD_ACTIVATE_SD,0);//ֱ�����ܵ��Ļظ���0  
          if(b == R1_OK) 
            { //�ɹ�
              SDValid=1;
              (void)SB(0xFF); 
              return 1;
            }
					else if(b != R1_ERROR_IDLE)
            {
              if(b!=0xFF)
							  {	//�����뱨��
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
��������: ����������뱨��
��    ��: rsp --- ������
�� �� ֵ: ��
�����Ա: ������
*******************************************************************************/
void DataErrExplain(INT8U rsp)
{
  if(!rsp)return;
  SD_TEST_StrOut("DataErrExplain:");
  Display_Hex(rsp);
  Display_Char('\n');
  if(rsp&0x01)SD_TEST_StrOut("����");
  if(rsp&0x02)SD_TEST_StrOut("SD���ڲ�����");
  if(rsp&0x04)SD_TEST_StrOut("ECCУ�����");
  if(rsp&0x08)SD_TEST_StrOut("��ַԽ�磡");
  Display_Char('\n');
}
/*******************************************************************************
��������: R1������뱨��
��    ��: rsp --- ������
�� �� ֵ: ��
�����Ա: ������
*******************************************************************************/
void R1Explain(INT8U rsp)
{
  if(!rsp)return;
  SD_TEST_StrOut("R1Explain:");
  Display_Hex(rsp);
  Display_Char('\n');
  if(rsp & R1_ERROR_IDLE)          SD_TEST_StrOut("����״̬��");
  if(rsp & R1_ERROR_ERASE_RESET)   SD_TEST_StrOut("�������ã�");
  if(rsp & R1_ERROR_ILLEGAL_CMD)   SD_TEST_StrOut("�Ƿ�ָ�");
  if(rsp & R1_ERROR_CRC)           SD_TEST_StrOut("У�����");
  if(rsp & R1_ERROR_ERASE_SEQ)     SD_TEST_StrOut("ɾ������");
  if(rsp & R1_ERROR_ADDR_ERROR)    SD_TEST_StrOut("��ַ����");
  if(rsp & R1_ERROR_PARAMETER_ERR) SD_TEST_StrOut("��������");
  Display_Char('\n');
}
/*******************************************************************************
��������: ����Ƿ���SD��
��    ��: ��
�� �� ֵ: 1 - ��SD��
          0 - ��SD��
�����Ա: ������
*******************************************************************************/
INT8U CheckAlive(void)
{
  int i;
  if(!SDValid)
  {	//SD��Ŀǰ��Ч
    for(i=0;i<3;i++)
    {
      SPI_Deselect();
      SD_TEST_StrOut("\n����CheckAlive����:��Ⲣ��ʼ��SD��...\n");
      //��λSD��
			if(!GoIdle())
      {
        SD_TEST_StrOut("\nSD���޷����� Idle ״̬.");
        continue;
      }
			//����SD��
      if(!GoReady())
      {
        SD_TEST_StrOut("\nSD�޷����� Ready ״̬.");
        continue;
      }
			//SD����Ч
      SDValid=1;
      SD_TEST_StrOut("\nSD����ʼ�����.");
      return 1;	
    }
    return 0;	
  }
	//SD����Ч
  return 1;
}
/*******************************************************************************
��������: ��һ������������
��    ��: BlockNum  ----- ������
          *addr     ----- ���ݻ����� 
�� �� ֵ: 1 - OK
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
      while(b==0xFF)//�ȴ����Ϳ�ʼ
        {
          b=SB(0xFF);
          i++;
          if(i>30000)
            {
              break;
            }
        }    
      if(b!=0xfe)
        { //ǿ��ֹͣ����
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
  SD_TEST_StrOut("�����ݿ飺");
  Display_Num(BlockNum);
  SD_TEST_StrOut(" ��������\n");   
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
    b=SDcmd(24,BlockNum);		//CMD24 д���ݿ飬Ĭ�ϳ���Ϊ512�ֽڣ����2���ֽڵ�У����
    if(b!=0)
    {
      SDValid=0;
      continue;
    }
    for(i=0;i<10;i++)
    {
      b=SB(0xFF);
    }		  
    b=SB(0xFE);		          //���ݿ�ʼ����
    for(i=0;i<512;i++)
    {            //��������
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
        SD_TEST_StrOut("CRCУ���������д��δ�ɹ���������룺");
      else if(r==6)
        SD_TEST_StrOut("����д�뷢�����󣬴������");
      else
        SD_TEST_StrOut("δ֪����");
      Display_Hex(b);
      Display_Char('\n');
      SDValid=0;
      continue;
    }
    b=SB(0xFF);
    if(!BusyWait())
    {
        SD_TEST_StrOut("�ȴ� BUSY ��ʱ!\n");
    }
    b=SB(0xff);   
    GetStatus();
    return 1; 
  }
  SD_TEST_StrOut("SD����������\n");
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
��������: ��ȡSD��CSD��Ϣ
��    ��: ��
�� �� ֵ: 1 - ��ȡ�ɹ�
          0 - ��ȡʧ��
�����Ա: ������
*******************************************************************************/
INT8U GetCSD(void)
{
  INT8U i,b;
  SD_TEST_StrOut("��CSD�Ĵ���:\n");
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
  SD_TEST_StrOut("����CSD���ݳ��ȴ���Ԥ�ڣ�");
  return 0;
}
/*******************************************************************************
��������: ��ʾCSD��Ϣ
��    ��: ��
�� �� ֵ: 1 - ��ȡ�ɹ�
          0 - ��ȡʧ��
�����Ա: ������
*******************************************************************************/
void Display_CSD(void)
{
  INT16U i,j;
  int C_SIZE;
  INT8U C_SIZE_MULT,READ_BL_LEN;
  INT32U SD_SIZE;
  SD_TEST_StrOut("CSD�汾��1.");
  Display_Num(CSD[0]>>6);
  SD_TEST_StrOut("\n������ݷ���ʱ�䣺");
  Display_Num(CSD[1]+CSD[2]*100);
  SD_TEST_StrOut("\n������ݴ������ʣ�");
  if((CSD[3]&0x03)==0)
    SD_TEST_StrOut("100Kbit/s");
  else if((CSD[3]&0x03)==1)
    SD_TEST_StrOut("1Mbit/s");
  else if((CSD[3]&0x03)==2)
    SD_TEST_StrOut("10Mbit/s");
  else if((CSD[3]&0x03)==3)
    SD_TEST_StrOut("100Mbit/s");
  SD_TEST_StrOut("\n�����ݿ鳤�ȣ�");
  j=1; 
  READ_BL_LEN=(CSD[5]&0x0f);
  for(i=0;i<READ_BL_LEN;i++)
    j*=2;
  Display_Num(j);
  SD_TEST_StrOut(" �ֽ�\n");
  SD_TEST_StrOut("�����ݿ�д�룺");
  if(!(CSD[6]&0x40))
    SD_TEST_StrOut("��");
  SD_TEST_StrOut("����\n");
  SD_TEST_StrOut("�����ݿ��ȡ��");
  if(!(CSD[6]&0x20))
    SD_TEST_StrOut("��");
  SD_TEST_StrOut("����\n");
  SD_TEST_StrOut("���ö�д������");
  C_SIZE=(((INT16U)(CSD[6]&0x02))<<10)+(CSD[7]<<2)+(CSD[8]>>6);
  C_SIZE_MULT=((CSD[9]&0x02)<<1)+(CSD[11]>>7);
  SD_SIZE=C_SIZE+1;
  SD_SIZE=SD_SIZE<<(INT8U)(C_SIZE_MULT+READ_BL_LEN+2);
  Display_Num(SD_SIZE);
  SD_TEST_StrOut(" �ֽ�\n");  
}
/*******************************************************************************
��������: ��ȡSD��CID��Ϣ
��    ��: ��
�� �� ֵ: 1 - ��ȡ�ɹ�
          0 - ��ȡʧ��
�����Ա: ������
*******************************************************************************/
INT8U GetCID(void)
{
  INT8U i,b;
	//����SD���Ƿ��Ѿ�����ʼ��   
  if(!CheckAlive())
		{
      return 0;	
		}
  SD_TEST_StrOut("\n��CID�Ĵ���:");
  (void)SB(0xFF);//XSL��Ϊ����
	//��������
  b=SDcmd(CMD_SEND_CID,0);
  if(b != R1_OK)
    {
      return 0;
    }
	//���һ�Ӧͷ0xFE
  b=SB(0xFF);
  while(b==0xFF)
    {
      b=SB(0xFF);
    }
  if(b!=0xFE)
	  {
      SD_TEST_StrOut("GetCID��û�����ݴ��䣡\n");
      DataErrExplain(b);
      return 0;
    }
	//��ȡCID
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
      SD_TEST_StrOut("����CID���ݳ��ȴ���Ԥ�ڣ�");
      return 0;	 
		}
}
/*******************************************************************************
��������: ��ʾCID��Ϣ
��    ��: ��
�� �� ֵ: 1 - ��ȡ�ɹ�
          0 - ��ȡʧ��
�����Ա: ������
*******************************************************************************/
void Display_CID(void)
{
  INT16U i;
  SD_TEST_StrOut("������ID��");
  Display_Hex(CID[0]);
  Display_Char('\n');
  SD_TEST_StrOut("OEM ID��");
  Display_Hex(CID[1]);
  Display_Char(' ');
  Display_Hex(CID[2]);
  Display_Char('\n');
  SD_TEST_StrOut("��Ʒ���ƣ�");
  for(i=0;i<5;i++){
    Display_Char(CID[3+i]);
  }
  Display_Char('\n');
  SD_TEST_StrOut("��Ʒ�汾��V");
  Display_Num(CID[8]);
  Display_Char('\n');
  SD_TEST_StrOut("��Ʒ���кţ�");
  for(i=0;i<4;i++){
    Display_Hex(CID[9+i]);
    Display_Char(' ');
  }
  Display_Char('\n');
}
/*******************************************************************************
��������: ���ÿ鳤��
��    ��: ��
�� �� ֵ: 1 - ��ȡ�ɹ�
          0 - ��ȡʧ��
�����Ա: ������
*******************************************************************************/
INT8U SetBlockLen(void)
{
  INT8U b;
  SD_TEST_StrOut("\n�������ݿ鳤��Ϊ512");
  if(!CheckAlive())
    {
      SD_TEST_StrOut("\nSetBlockLen����ǰSD����Ч.");
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
��������: ʵ����������
��    ��: ��
�� �� ֵ: ��
�����Ա: ������
*******************************************************************************/
void SD_TestSize(void){ 
  int r;
  INT32U high,low,mid;
  high=14000;
  SD_TEST_StrOut("����SD����������\n");
  SD_TEST_StrOut("���ԣ� ");
  Display_Num(high);
  SD_TEST_StrOut(" ...");
  r = ReadBlock(high, (INT8U *) & SPIBuffer);
  if(r==0)
    {
      SD_TEST_StrOut("SD������С��8MB��\n");
      return;
    }
  while (r)
    {
      SD_TEST_StrOut("�ɹ���\n���ԣ� ");
      low = high;
      high = high <<1; 
      Display_Num(high);
      SD_TEST_StrOut(" ...");
      r = ReadBlock(high, (INT8U *) & SPIBuffer);
    }
    // �͵�ַ�ɶ����ߵ�ַ���ɶ��������м�ĵ�ַ
    mid = (low+high) >>1;
  do 
    {
      (void)GoReady(); 
      SD_TEST_StrOut("���ԣ� ");
      Display_Num(mid);
      SD_TEST_StrOut(" ...");
      r = ReadBlock(mid, (INT8U *) & SPIBuffer);
      if (!r)
        {         // �м��ַ���ɶ�
          high = mid;	  	//���͸ߵ�ַ
        } 
      else
        {             // �м��ַ�ɶ�
          low = mid;		  //����ߵ͵�ַ
          SD_TEST_StrOut("�ɹ���\n");
        } 
      mid = (low+high) >>1;
    }
  while(high>low+1);	//ֱ��mid���ٸı�
  MaxBlockNum=mid;
  SD_TEST_StrOut("SD��ʵ�������� ");
  Display_Num(mid*512);
  SD_TEST_StrOut(" �ֽ�\n");
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
��������: SD����ʼ��
*******************************************************************************/
void SD_Init_Test(void)
{
  //GPIO��ʼ��
	SD_GPIO();
	//SPI��ʼ��
  SPI_Config();
  //SD����Ч��־����
  SDValid=0;
  SD_TEST_StrOut("\n��ʼ��SD��...\n");
  //��Ⲣ��ʼ��SD��
  if(!CheckAlive())
	  {	//�޿�����
		  return;		
		}
  //��ȡCID(SD�����ʶ��,ֻ��)
  if(!GetCID())
  {
    SD_TEST_StrOut("CID�޷���ȡ��\n");
  }  
  //��ȡCSD(SD���鶨��,д��������С��)  
  if(!GetCSD()) 
  {
    SD_TEST_StrOut("CSD�޷���ȡ��\n");
  }     
  //�������ݿ鳤��
  if(!SetBlockLen())
  {
    SD_TEST_StrOut("�������ݿ鳤��ʧ�ܣ�\n");
  }
  //����ʵ������
  SD_TestSize(); 
  //��SD����Ч��־
  SDValid=1;
  SD_TEST_StrOut("��ʼ��SD����ɣ�\n");
}
//--------------------------------------------------����Ӧ����FATFS
INT8U SD_Init(void)
{
	//GPIO��ʼ��
	SD_GPIO();
	//SPI��ʼ��
  SPI_Config();
  //SD����Ч��־����
  SDValid=0;
  SD_TEST_StrOut("\n��ʼ��SD��...\n");
  //��Ⲣ��ʼ��SD��
  if(!CheckAlive())
	  {	//�޿�����
		  return 1;		
		}
  //��ȡCID(SD�����ʶ��,ֻ��)
  if(!GetCID())
    {
      SD_TEST_StrOut("CID�޷���ȡ��\n");
			return 1;
    }  
  //��ȡCSD(SD���鶨��,д��������С��)  
  if(!GetCSD()) 
    {
      SD_TEST_StrOut("CSD�޷���ȡ��\n");
			return 1;
    }     
  //�������ݿ鳤��
  if(!SetBlockLen())
    {
      SD_TEST_StrOut("�������ݿ鳤��ʧ�ܣ�\n");
			return 1;
    }
  //����ʵ������
  SD_TestSize(); 
  //��SD����Ч��־
  SDValid=1;
  SD_TEST_StrOut("��ʼ��SD����ɣ�\n");
	return 0;
}
/******************************* END OF FILE **********************************/

