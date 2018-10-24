/******************************* NEU ROBOT *************************************
* File Name          : FS.c
* Author             : Shi Enyi
* Version            : V2.0
* Date               : 10/4/2008
* Description        : FAT12/FAT16 file system Driver
Ƕ��ʽӦ��˵��:
  1,�Լ���дReadBlock()��ȡSD����������
  2,�Լ���дWriteBlock()д��SD����������
  3,�����MaxBlockNum(SD����������)
XSL-FAT16�ļ�ϵͳ��ѧϰ:
  1,SD���ĵ�һ������:
    1.1 MBR��������¼:0����--->0~445Byte(446B)
	  1.2 DPT��������  :0����--->446~509Byte(64B)
	                    ��¼���̻���������Ϣ(4��������,ÿ��16B)
	  1.3 ���������   :0����--->510-511(2B),0xAA55Ϊ�Ϸ�.
  2,��ʼ������:
    2.1 ��ȡDPT(��һ����������):SD��������0
	  2.2 ��ȡ��������(��ʼ�߼�����)����DPT�ṹ�����.

*******************************************************************************/
//------------------------------------------------------------------------------
#define FS_GLOBAL
#include "includes.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------Macro defines
#define FILE_TYPE                 1
#define DIR_TYPE                  2
#define LongNameSuport            1
//�������
#define SD_READ_ERR               1 //��SD������
#define SD_WRITE_ERR              2 //дSD������
#define FS_INVALID_ERR            3 //�ļ�ϵͳ��Ч����
#define SCFC_ERR                  4
#define CC_ERR                    5
#define FN_INVALID_ERR            6
#define FN_OCCUPIED_ERR           7
#define MEMORY_FULL_ERR           8
#define DIRECTER_FULL_ERR         9
#define OPERATION_INVALID_ERR     10
#define OF_PARAMETER_ERR          11
#define NOT_FOUND_ERR             12
#define READ_CLUSTER_ERR          13
//------------------------------------------------------------------------------Private variable declaration
struct T_DPT DPT;					 //DPT��Ϣ
struct T_BPB BPB;					 //BPB��Ϣ
struct T_EBPB EBPB;
struct T_FILE CurrentFile;
struct T_DIR CurrentDir;	 //��ǰ·����Ϣ
struct T_FAT FAT;					 //FAT״̬
struct T_DIRCache DirCache;

INT8U FSLastErr=0;				//����µĴ������
INT8U FATSector[514];			//FAT������
INT8U FileSector[514];    //�ļ��������ݻ�����
INT8U DirSector[514];     //·���������ݻ�����

INT8U DiskValid;          //DPT��ȡ�ɹ�(�����Ƿ���Ч)
INT8U BPBValid;           //BPB��ȡ�ɹ�
INT8U FSValid;            //FAT��ȡ�ɹ�

INT8U FatType;            //�ļ�ϵͳ����(12��16)
INT32U  RootStartSector;  //��Ŀ¼����ʼ����(��������+��������+FATռ������)
INT32U  DataStartSector;	//������ʼ����(��Ŀ¼����ʼ���� + FDT��Ŀ¼ȡ��ռ�õ�����)
INT16U FirstFreeCluster;	//��һ�����ɴ�
INT8U LN_flag;
INT16U FreeClusterCounter;//δʹ�õĴ�����
//------------------------------------------------------------------------------Private function prototypes
INT8U GetDPT(void);
INT8U GetBPB(void);
INT8U GetFAT(void);
INT8U LoadFileSector(INT32U SectorNum);
INT8U LoadFAT(INT16U index);
INT8U SaveFAT(void);
INT8U SaveFileSector(void);
INT8U TurnToNextFileSector(void);
INT16U ReadFAT(INT16U addr);
void WriteFAT(INT16U addr,INT16U value);
INT16U ReadCluster(INT16U sCluster);
INT16U GetFreeCluster(void);
void WriteCluster(INT16U sCluster,INT16U value);
void DelClusterLink(INT16U sCluster);
INT32U ClusterToSector(INT16U sCluster);
INT16U CountFreeClusters(void);
void CreatEntry(INT8U *name, INT8U *type, INT8U attr, INT16U Entry, INT16U FirstCluster, INT32U Size);
INT8U CheckCont(INT16U start);
INT16U CreatContChain(INT16U num);
INT8U GetLongName(INT32U EntrySector,INT16U EntryStart,INT8U *LongName);
INT16U GetFreeEntry(INT16U num);
//------------------------------------------------------------------------------����ʵ��
//------------------------------------------------------------------------------����-BEGIN
/*******************************************************************************
��������: ��ȡ"��",��λ��ǰ
*******************************************************************************/
INT16U DrawWord(INT8U *str)
{
  return (INT16U)(*str)+((INT16U)(*(str+1))<<8);
}
/*******************************************************************************
��������: ��ȡ"˫��",��λ��ǰ
*******************************************************************************/
INT32U DrawDword(INT8U *str)
{
  return (INT32U)DrawWord(str)+((INT32U)DrawWord(str+2)<<16);
}
/*******************************************************************************
��������: ת����һ���ļ�����
��    ��: ��
�� �� ֵ: 1 --- OK
          0 --- ERR
�����Ա: ������
*******************************************************************************/
INT8U TurnToNextFileSector(void)
{
  FSLastErr=0;
  if(CurrentFile.State==INVALID)
	  {
		  return 1;
	  }
  if((CurrentFile.SectorIndex+1)%BPB.SectorsPerCluster!=0)
    { //���ڵ�������ַ��1
      CurrentFile.CurrentSector++;
      CurrentFile.SectorIndex++;
      return 1;
    }
  else if(ReadCluster(CurrentFile.CurrentCluster)!=0xFFFF)
    {	//�ظ���,����������
      CurrentFile.CurrentCluster=ReadCluster(CurrentFile.CurrentCluster);
      CurrentFile.CurrentSector=ClusterToSector(CurrentFile.CurrentCluster);
      CurrentFile.SectorIndex++; 
      return 1;
    }
  FSLastErr=READ_CLUSTER_ERR;
  return 0;
}
/*******************************************************************************
��������: ��֪�غ�ת��Ϊ������
��    ��: �غ�
�� �� ֵ: ������
�����Ա: ������
*******************************************************************************/
INT32U ClusterToSector(INT16U sCluster)
{
  if(sCluster<=1)
		{	//�غ�0�ʹغ�1:��Ŀ¼����ʼ����(��������+��������+FATռ������)
      return RootStartSector;
		}
  else
    {	//������ʼ����(��Ŀ¼����ʼ���� + FDT��Ŀ¼ȡ��ռ�õ�����) + (sCluster-2)*BPB.SectorsPerCluster
		  return DataStartSector+sCluster*BPB.SectorsPerCluster-2*BPB.SectorsPerCluster;
	  }
}
/*******************************************************************************
��������: �������ɴ�
��    ��: ��
�� �� ֵ: ���ɴظ���
�����Ա: ������
*******************************************************************************/
INT16U CountFreeClusters(void)
{	
  INT16U i,temp,ans=0;
  FirstFreeCluster=0;
  Display_Str("ͳ�ƿ��ÿռ�:\n");
  for(i=2;i<MaxBlockNum/BPB.SectorsPerCluster;i++)
    {	//���ݴغŶ�ȡFAT��������
      temp=ReadCluster(i);
      if(temp==0)
        {  
          ans++;
          if(FirstFreeCluster==0)
			      {
              FirstFreeCluster=i;
			      }
        }
      if(temp==0xFFF0)break;
    }
  Display_Str("    ���� ");
  Display_Num(i);
  Display_Str(" ���洢��Ԫ\n");
  Display_Str("    ���� ");
  Display_Num(i-ans);
  Display_Str(" ���洢��Ԫ\n");
  Display_Str("    ʣ�� ");
  Display_Num(ans);
  Display_Str(" ���洢��Ԫ\n");
  return ans;
}
//------------------------------------------------------------------------------����-END
//------------------------------------------------------------------------------װ���Ӻ���-BEGIN
/*******************************************************************************
��������: ��ȡ�������ݵ�FileSector[]
��    ��: SectorNum --- ������
�� �� ֵ: 1 --- OK
          0 --- ERR
�����Ա: ������
*******************************************************************************/
INT8U LoadFileSector(INT32U SectorNum)
{
  //Ҫװ�ص��ļ�����Ϊ��Ч�ĵ�ǰ���ļ�����,���ٴ�װ��
  if((SectorNum==CurrentFile.CurrentSector)&&(CurrentFile.State!=INVALID))
    {
      return 1;
    }
  //���������ݵ�������
  if(ReadBlock(SectorNum,FileSector))
    {
      CurrentFile.CurrentSector=SectorNum;
      return 1;
    }
  Display_Str("�޷���ȡ������");
  Display_Num(SectorNum);
  Display_Char('\n');
  SDValid=0;
  FSLastErr=SD_READ_ERR;
  return 0;
}
/*******************************************************************************
��������: ��ȡ�������ݵ�DirSector[]
��    ��: SectorNum --- ������
�� �� ֵ: 1 --- OK
          0 --- ERR
�����Ա: ������
*******************************************************************************/
INT8U LoadDirSector(INT32U SectorNum)
{
  //Ҫװ�ص�·������Ϊ��ǰ��·������,����װ��
  if(SectorNum==CurrentDir.CurrentSector)
    {
      return 1;
    }
  if(ReadBlock(SectorNum,DirSector))
    {
      CurrentDir.CurrentSector=SectorNum;
      return 1;
    }
  Display_Str("�޷���ȡ������");
  Display_Num(SectorNum);
  Display_Char('\n');
  SDValid=0;
  FSLastErr=SD_READ_ERR;
  return 0;
}
/*******************************************************************************
��������: װ��һ��FAT��������FATSector[]
��    ��: index --- ��������(FAT��ƫ��)
�� �� ֵ: 1 --- OK
          0 --- ERR
�����Ա: ������
*******************************************************************************/
INT8U LoadFAT(INT16U index)
{
  //��ʼ����FAT.SectorIndex = 0,��ֵ���������
  if(index==FAT.SectorIndex)
    {
      return 1;
    }
  //��������޸Ĺ�,����Ҫ����
  if(FAT.State==WRITE)
    {
      SaveFAT();
    }
  //ת��Ϊ����������,����ȡ������
  if(ReadBlock(BPB.HiddenSector+BPB.ReservedSector+index,FATSector))
    {
      FAT.SectorIndex=index;
      FAT.State=0;
      return 1;
    }
  Display_Str("�޷���ȡ������");
  Display_Num(BPB.HiddenSector+BPB.ReservedSector+index);
  Display_Char('\n');
  SDValid=0;
  FSLastErr=SD_READ_ERR;
  return 0;
}
/*******************************************************************************
��������: ���ݴغŶ�ȡFAT��������(����Ӧ�ļ�����һ���غ�)
��    ��: �غ�
�� �� ֵ: FAT���ж�Ӧ�����غŵ�����(����Ӧ�ļ�����һ���غ�)
�����Ա: ������
*******************************************************************************/
INT16U ReadCluster(INT16U sCluster)
{
  INT16U res;
  INT16U addr;
  if(sCluster<=1)
    {	//ϵͳ�غŴ�2��ʼ,�Զ���0xFFF0��ʾ��ѯԽ��
      return 0xFFF0;
    }
  if(FatType==12)
    {	 //FAT12
      if(sCluster>=0xFF0) 
        {
          return 0xFFF0;
        }
      addr=sCluster*3/2;
      res=ReadFAT(addr);
      if(sCluster%2==0)
        {
          res=res&0x0FFF;  
        }
      else 
        {
          res=(res&0xFFF0)>>4;
        }
      if(res>=0xFF0&&res<=0xFF6)//����FAT12�еı���ֵ
        {
          res=0xFFF;
        }
    }
  else
    {	//FAT16
      if(sCluster>=0xFFF0) 
        { //0xFFF0���ϱ�ʾ���Խ��;
          return 0xFFF0;			
        }
      addr=sCluster*2;
      res=ReadFAT(addr);
      if(res>=0xFFF0&&res<=0xFFF6) //����FAT16�еı���ֵ
        {
          res=0xFFFF;
        }
    }
  if(res==0xFFF)
    {
      res=0xFFFF;          //ͳһFAT12��FAT16�Ľ�����־
    }
  return res;
}
/*******************************************************************************
��������: ��ȡFAT����һ��ָ����ַ�������ֽڵ�����.
��    ��: addr   --- FAT��ƫ���ֽڵ�ַ
�� �� ֵ: 0xFFF0 --- ���Խ��
          0xFFFF --- ��ȡʧ��
					����   --- ��ֵ
�����Ա: ������
*******************************************************************************/
INT16U ReadFAT(INT16U addr)
{
  INT16U res;
  if(addr>BPB.BytesPerSector*BPB.SectorsPerFAT-1)
	  { //��ַ������FAT���ڵĵ�ַ�ռ�
      return 0xFFF0;	//0xFFF0��ʾ���Խ��;
		}
  if(!LoadFAT(addr/BPB.BytesPerSector))
    {	//
      Display_Str("��ȡFAT��ʧ�ܣ�");
			return 0xFFFF;
    }
  res=(INT16U)FATSector[addr%BPB.BytesPerSector];
  addr++;
  if(!LoadFAT(addr/BPB.BytesPerSector))
    {
      Display_Str("��ȡFAT��ʧ�ܣ�");
      return 0xFFFF;
    }
  return res+(((INT16U)FATSector[addr%BPB.BytesPerSector])<<8);
}
/*******************************************************************************
��������: ��ȡ���ɴ�
��    ��: ��
�� �� ֵ: 0xFFFF --- ����
          ����	 --- ���ɴ�λ��
�����Ա: ������
*******************************************************************************/
INT16U GetFreeCluster(void)
{
  INT16U i,temp;
  //����ǵ�һ��,��ӵڶ��ؿ�ʼ,��������ϴεĴغſ�ʼ
  if(!LN_flag)
    {
      i=2;
    }
  else
    {
      i=FirstFreeCluster;
    }
  for(;i<MaxBlockNum/BPB.SectorsPerCluster;i++) 
    {
      temp=ReadCluster(i);
      if(temp==0) 
        {//0��ʾ�ô�û�б�ռ��
          FirstFreeCluster=i;
          LN_flag=1;
          return i;
        }
      if(temp==0xFFF0)
        {//���Խ��
          break;
        }
    }
  return 0xFFFF;			//�������
}
//------------------------------------------------------------------------------װ���Ӻ���-END
//------------------------------------------------------------------------------�����Ӻ���-BEGIN
/*******************************************************************************
��������: ɾ��������
��    ��: ��ID
�� �� ֵ: ��
�����Ա: ������
*******************************************************************************/
void DelClusterLink(INT16U sCluster)
{
  INT16U next;
  if(FatType==12)
    {
      do
        {
          next=ReadCluster(sCluster);
          if(next)
            {
              FreeClusterCounter++;
              if(sCluster<FirstFreeCluster)
              FirstFreeCluster=sCluster;
            }
          WriteCluster(sCluster,0);
          sCluster=next;
        }
      while(next>=2&&next<0xFF0);
    } 
  else 
    {
      do
        {
          next=ReadCluster(sCluster);
          if(next)
            {
              FreeClusterCounter++;
              if(sCluster<FirstFreeCluster)
              FirstFreeCluster=sCluster;
            }
          WriteFAT(sCluster*2,0x0000);
          sCluster=next;
        }
      while(next>=2&&next<0xFFF0);
    } 
}
/*******************************************************************************
��������: ��FAT��д�����ֽڵ�����
��    ��: addr  --- FAT��ƫ���ֽڵ�ַ
          value --- Ҫд���16λ����
�� �� ֵ: ��
�����Ա: ������
*******************************************************************************/
void WriteFAT(INT16U addr,INT16U value)
{
  //��ַԽ���ж�,FAT���е���Ե�ַ��0��ʼ
  if(addr>BPB.BytesPerSector*BPB.SectorsPerFAT-2)
  {
    Display_Str("WriteFAT���غ�");
    Display_Num(addr);
    Display_Str("Խ�磡\n");
    return;
  }
  //����FAT��,��Ի��ƴ�0��ʼ
  if(!LoadFAT(addr/BPB.BytesPerSector))
  {
    Display_Str("WriteFAT���޷�����FAT��\n");
    SDValid=0;
    return;
  }
  FATSector[addr%BPB.BytesPerSector]=(INT8U)(value&0xFF);
  FAT.State=WRITE;
  addr++;
  if(!LoadFAT(addr/BPB.BytesPerSector))
  {
    Display_Str("WriteFAT���޷�����FAT��\n");
    SDValid=0;
    return;
  }
  FATSector[addr%BPB.BytesPerSector]=(INT8U)((value&0xFF00)>>8);
  FAT.State=WRITE;
}
/*******************************************************************************
��������: ���ݴغ�д��FAT��������(����Ӧ�ļ�����һ���غ�)
��    ��: sCluster --- �غ�
					value		 --- д��ֵ
�� �� ֵ: ��
�����Ա: ������
*******************************************************************************/
void WriteCluster(INT16U sCluster,INT16U value)
{
  if(sCluster<=1)
  {
    Display_Str("WriteCluster���غŴ���\n");
    return;
  }
  if(FatType==12)
  {
    if(sCluster>0xFF0)
    {
      Display_Str("WriteCluster����ַԽ�磡\n");
      return;
    }
    if(value==0xFFFF)
      value=0xFFF;			 
    else if(value>0xFFF||(value>=0xFF0&&value<=0xFF6))
    {
      Display_Str("WriteCluster���Ƿ����ݣ�\n");
      return;
    }
    if(sCluster%2==0) 
      WriteFAT(sCluster*3/2,(ReadFAT(sCluster*3/2)&0xF000)|value);
    else
      WriteFAT(sCluster*3/2,(ReadFAT(sCluster*3/2)&0x000F)|(value<<4));
  } 
  else 
  {
    if(sCluster>0xFFF0)
    {
      Display_Str("WriteCluster����ַԽ�磡\n");
      return;
    }
    if(value>=0xFFF0&&value<=0xFFF6)
    {
      Display_Str("WriteCluster���Ƿ����ݣ�\n");
      return;
    }
    WriteFAT(sCluster*2,value);
  } 
}
/*******************************************************************************
��������: ����FAT
��    ��: index --- ����
�� �� ֵ: 1 --- OK
          0 --- ERR
�����Ա: ������
*******************************************************************************/   
INT8U SaveFAT(void)
{
  //д��һ��FAT��
  if(!WriteBlock(BPB.HiddenSector+BPB.ReservedSector+FAT.SectorIndex,FATSector))
    {
      Display_Str("SaveFAT��дFAT�����\n");
      FSValid=0;
      FSLastErr=SD_WRITE_ERR;
      return 0;
    }
  //д�ڶ���FAT��
  if(!WriteBlock(BPB.HiddenSector+BPB.ReservedSector+BPB.SectorsPerFAT+FAT.SectorIndex,FATSector))
    {
      Display_Str("SaveFAT��дFAT�����\n");
      FSValid=0;
      FSLastErr=SD_WRITE_ERR;
      return 0;
    }
  FAT.State=0;
  return 1;
}
/*******************************************************************************
��������: �����ļ�����
��    ��: ��
�� �� ֵ: 1 --- OK
          0 --- ERR
�����Ա: ������
*******************************************************************************/
INT8U SaveFileSector(void)
{
  return WriteBlock(CurrentFile.CurrentSector,FileSector);		
}
/*******************************************************************************
��������: ����·������
��    ��: ��
�� �� ֵ: 1 --- OK
          0 --- ERR
�����Ա: ������
*******************************************************************************/
void SaveDirSector(void)
{
  if(!WriteBlock(CurrentDir.CurrentSector,DirSector))
		{
      Display_Str("FileDelete��дĿ¼���ִ���\n");
	  }
}
//------------------------------------------------------------------------------�����Ӻ���-END
/*******************************************************************************
��������: ��ȡ������
��    ��: ��
�� �� ֵ: 1 --- OK
          0 --- ERR
�����Ա: ������
*******************************************************************************/
INT8U GetDPT(void)
{				
  Display_Str("��ȡӲ�̷�����...\n");
  //��ȡ����0������
  if(!LoadFileSector(0))
  {
    Display_Str("��ȡBPBʧ�ܣ�\n");
    FSLastErr=SD_READ_ERR;
    return 0;
  }
  //����0����������ֽ���0x55AA,˵��SD���Ѿ�����Ч��ʽ��
  if(FileSector[510]==0x55&&FileSector[511]==0xAA)
  {	//���µ���˵��Ϊ2G-SD��
    //��������:      00-�ǻ���� (80-�����) ����-��Ч����
    DPT.BootIndicator=FileSector[446];
    //��������ʼ��:  0x02
    DPT.StartingHead=FileSector[447];
    //��������ʼ����:0x03
    DPT.StartingSector=FileSector[448]>>2;
    //��������ʼ�ŵ�:0x0000
    DPT.StartingCylinder=((INT16U)(FileSector[448]&0x02)<<8)+(INT16U)FileSector[449];
    //������ϵͳ����:0x06(4-DOS 5-EXTEND 6-DOS 7-NTFS 83-LINUX)
    DPT.SystemID=FileSector[450];
    //�����Ľ�����	:0x38
    DPT.EndingHead=FileSector[451];
    //�����Ľ�������:0x3E
    DPT.EndingSector=FileSector[452]>>2;
    //�����Ľ����ŵ�:0x00B8
    DPT.EndingCylinder=((INT16U)(FileSector[452]&0x02)<<8)+(INT16U)FileSector[453];
    //��������ʼ�߼�����(��������):0x00000089
    DPT.RelativeSectors=DrawDword(FileSector+454);
    //��������������:0x003A9F77
    DPT.TotalSectors=DrawDword(FileSector+458);
    //
    Display_Str("��Ч������С��");
    Display_Num(DPT.TotalSectors*512);
    Display_Str(" �ֽ�\n");
    //������Ч
    DiskValid=1;
    return 1;
  }
  Display_Str("��ǰ��Ӳ�̷�������Ч��\n");
  FSLastErr=FS_INVALID_ERR;
  return 0;
}
/*******************************************************************************
��������: ��ȡBPB(BIOS ����)����
��    ��: ��
�� �� ֵ: 1 --- OK
          0 --- ERR
�����Ա: ������
˵    ��: 
    1,BPB(BIOS������)����:SD������,FAT(�ļ������)��FDT(�ļ�Ŀ¼��)��λ�úʹ�С
*******************************************************************************/
INT8U GetBPB(void)
{
  int i;
  FatType=0;
  //���������Ч,���Ȼ�ȡ��������Ϣ
  if(!DiskValid) 
    {
      if(!GetDPT())
        {
          Display_Str("û�з�����Ϣ��\n");
          return 0;
        }
    }
  //װ��������������
  Display_Str("��ȡ�ļ�ϵͳ...\n");
  if(!LoadFileSector(DPT.RelativeSectors))
    {
      Display_Str("��ȡBPBʧ�ܣ�\n");
      return 0;
    }
  //�������������ĩ���ֽ�Ϊ0x55AA,����Ч
  if((FileSector[510]==0x55)&&(FileSector[511]==0xAA))
    {
      //��ȡ�ļ�ϵͳ����FATXX.ֻʶ��"FAT12"��"FAT16"
      for(i=0;i<12;i++)
        {
          EBPB.FileSystemType[i]=FileSector[i+54];
        }
      Display_Str("�ļ�ϵͳ��FAT");
      if((FileSector[54]=='F')&&(FileSector[55]=='A')&&
         (FileSector[56]=='T')&&(FileSector[57]=='1'))
        {
          if(FileSector[58]=='2')
            {
              FatType=12;
            }
          else if(FileSector[58]=='6')
            {
              FatType=16;
            }
          Display_Num(FatType);
          Display_Char('\n');
        }
      if(FatType==0)
        {
          Display_Str("��Ч��\n");
          return 0;
        }
      else
        {
          //ÿ���������ֽ���-0x0200(512�ֽ�/����)
          BPB.BytesPerSector=(INT16U)FileSector[11]+((INT16U)FileSector[12]<<8);
          //ÿ�����ж�������-0x40(64������/��)
          BPB.SectorsPerCluster=FileSector[13];
          //�ж��ٸ���������-0x0002
          BPB.ReservedSector=(INT16U)FileSector[14]+((INT16U)FileSector[15]<<8);
          //�ж��ٸ�FAT��----0x02
          BPB.NumberOfFAT=FileSector[16];
          //FDT(��Ŀ¼��)�ж��ٸ��Ǽ���(FAT16Ӧ����512)
          BPB.RootEntries=(INT16U)FileSector[17]+((INT16U)FileSector[18]<<8);
          //���������ľ�ʽ16λ???	---0x0000
          BPB.SmallSector=(INT16U)FileSector[19]+((INT16U)FileSector[20]<<8);
          //�洢��������(0xF8-�̶��洢����,0xF0-���ƶ��洢����)
          BPB.MediaDescriptor=FileSector[21];
          //ÿ��FAT�ļ��������ռ������---0x00EB(235)
          BPB.SectorsPerFAT=(INT16U)FileSector[22]+((INT16U)FileSector[23]<<8);
          //ÿ��������----0x003F(����SD��������)
          BPB.SectorsPerTrark=(INT16U)FileSector[24]+((INT16U)FileSector[25]<<8);
          //��ͷ��---0x00FF(����SD��������)
          BPB.NumberOfhead=(INT16U)FileSector[26]+((INT16U)FileSector[27]<<8);
          //����������---0x00000089(FAT�����ڷ���ǰ�����ص�����)
          BPB.HiddenSector=DrawWord(FileSector+28);
          //��������-----0x00009F77
          BPB.LargeSector=DrawWord(FileSector+32);
          //��Ŀ¼����ʼ����(�������� + �������� + FATռ�õ�����)
          RootStartSector = BPB.HiddenSector + BPB.ReservedSector +
                            BPB.NumberOfFAT * BPB.SectorsPerFAT;
          //������ʼ����(��Ŀ¼����ʼ���� + FDT��Ŀ¼ȡ��ռ�õ�����)
          DataStartSector = RootStartSector + 
                            BPB.RootEntries * 32 / BPB.BytesPerSector;
          //
          BPBValid=1; 
          return 1;
        }
    } 
  else
    {
      Display_Str("\n�ļ�ϵͳ��Ч��\n");
      FSLastErr=FS_INVALID_ERR;
      return 0;
    }
}
/*******************************************************************************
��������: ��ȡ FAT ��Ϣ
��    ��: ��
�� �� ֵ: 1 --- OK
          0 --- ERR
�����Ա: ������
*******************************************************************************/
INT8U GetFAT(void)
{
  //���BPB��Ч,�����¶�ȡBPB(BIOS ������)
  if(!BPBValid) 
    {
      if(!GetBPB())
        {
          Display_Str("��ǰ�ļ�ϵͳ��Ч��\n");
          SDValid=0;
          return 0;
        }
    }
  //����FAT��
  Display_Str("����FAT��...\n");
  if(!ReadBlock(BPB.HiddenSector+BPB.ReservedSector,FATSector))
    {
      Display_Str("��ȡFAT��ʧ�ܣ�");
      SDValid=0;
      FSLastErr=SD_READ_ERR;
      return 0;
    }
  FAT.SectorIndex = 0;
  FAT.State       = 0;
  FSValid         = 1;
  return 1;
}
/*******************************************************************************
��������: �ļ�ϵͳ��ʼ��
*******************************************************************************/
void FS_Init(void)
{
  //�ļ�ϵͳ��Ч
  FSValid=0;
  //������Ч
  DiskValid=0;
  //BPB(Bios Paramater Block)��Ч
  BPBValid=0;
  //Fat���ͱ�����ʼ��
  FatType=0;
  //FAT����״̬Ϊ��
  FAT.State=INVALID;
  LN_flag=0;
  //......
  Display_Str("��ʼ���ļ�ϵͳ��\n");
  //���SD��û�б���ʼ��,���SD�����г�ʼ��
  if(!SDValid)
    {
      SD_Init();
      if(!SDValid)
        {
          Display_Str("û�ҵ�SD����\n");
          return;
        }
    }
  //��ȡDPT(������)����
  if(!GetDPT())
    {
      Display_Str("GetDPT����ȡʧ�ܣ�\n");
      return;
    }
  //��ȡBPB(BIOS ����)����
  if(!GetBPB())
    {
      Display_Str("GetBPB����ȡʧ�ܣ�\n");
      return;
    }
  //��֤������С
  if(BPB.BytesPerSector != 512)
    {
      Display_Str("��ʼ��ʧ�ܣ�������С����֧�֣�\n");
      return;
    }
  //��ȡTAT
  if(!GetFAT())
    {
      Display_Str("GetFAT����ȡʧ�ܣ�\n");
      return;
    }
  //��ǰĿ¼Ϊ��Ŀ¼
  CurrentDir.StartCluster=0;              //��Ŀ¼����ʼ�ع涨Ϊ0
  CurrentDir.StartSector=RootStartSector; //��Ŀ¼����ʼ����
  CurrentDir.DirPath[0]=':';              //��Ŀ¼��·���涨Ϊ":\"
  CurrentDir.DirPath[1]='\\';
  CurrentDir.DirPath[2]='\0';
  CurrentDir.PathLen=2;                   //
  //��ǰ�ļ���Ч
  CurrentFile.State=INVALID;
  FreeClusterCounter=CountFreeClusters();
  FSValid=1;
  Display_Str("��ʼ���ļ�ϵͳ��ϣ�\n");
}
/*******************************************************************************
��������: �鿴�Ƿ�������num�����ɴ�,�����׸���ID��
��    ��: Ҫ��ѯ�صĸ���
�� �� ֵ: �׸���ID��
�����Ա: ������
*******************************************************************************/
INT16U SearchContFreeClusters(INT16U num)
{		
  INT16U start,counter,temp,i;
  INT8U flag=0;
  //���ص�һ����¼��Ϊ0�Ĵغ�
  start=GetFreeCluster();
  counter=0;
  for(i=start;;i++)
  {
    temp=ReadCluster(i);
    //δ����Ĵ�
    if(temp==0)
      { 
        if(flag==0)
          {
            start=i;
            flag=1;
          }
        counter++;
        if(counter>=num)
          { //���������ؿռ���״غ�
            return start;
          }
        continue;
      }
    else if(temp==0xFFF0)
      { 
        Display_Str("  ���������ռ�ʧ�ܣ�\n");
        FSLastErr=SCFC_ERR;
        return 0;
      }
    else 
      { //��ǰ�ز��ǿ��д� 
        flag=0;
        counter=0;
      }
  }
}
/*******************************************************************************
��������: ���������Ĵ���
��    ��: ���������Ĵظ���
�� �� ֵ: �����״�ID
�����Ա: ������
*******************************************************************************/
INT16U CreatContChain(INT16U num)
{
  INT16U start,i;
  start=SearchContFreeClusters(num);
  if(start!=0)
  { //�ֱ������سɹ�
    for(i=0;i<num-1;i++)
    { //���������ر��Ϊ�ļ���¼
      WriteCluster(start+i,start+i+1);
    }
    WriteCluster(start+num-1,0xFFFF);
    SaveFAT();
  }
  return start;
}
/*******************************************************************************
��������: ��������(���Բ�����)
��    ��: ���������Ĵظ���
�� �� ֵ: 0    --- ʧ��
          ���� --- �����״�ID
�����Ա: ������
*******************************************************************************/
INT16U CreatChain(INT16U num)
{
  INT16U start,i,next,last;
	//���ص�һ����¼��Ϊ0�Ĵغ�
  start=GetFreeCluster();
  if(start==0)
    {
      FSLastErr=CC_ERR;
      return 0;
    }
  last=start;
  WriteCluster(last,0xFFFF);
  FreeClusterCounter--;
  for(i=1;i<num;i++)
    {
      next=GetFreeCluster();
      if(next==0)
        {
          Display_Str("CreatChain���ռ䲻�㣬����ʧ�ܣ�");
          DelClusterLink(start);
          FSLastErr=CC_ERR;
          return 0;
        }
      WriteCluster(last,next);
      WriteCluster(next,0xFFFF);
      FreeClusterCounter--;
      last=next;
    }
  SaveFAT();
  return start;
}
/*******************************************************************************
��������: ����������
��    ��: ���Ĵ�
�� �� ֵ: 1 --- ������
          0 --- ��������
�����Ա: ������
*******************************************************************************/
INT8U CheckCont(INT16U start)
{
  INT16U temp;
  for(;;)
  {
    temp=ReadCluster(start);
    if(temp==0xFFFF)
		  {	//���FAT��ָʾ��Ϊ0xFFFF
			  return 1;
			}
    if(temp!=start+1)
		  {
			  return 0;
			}
    start=temp;
  }
}
/*******************************************************************************
��������: �ļ�·����Ѱ(����CurrentDir�ṹ��)
��		��: *path		-	Ҫ��Ѱ��·��
					Cluster	-	
�� �� ֵ:	0        --- ����
          DIR_TYPE --- �ļ���
�����Ա: ������ 
*******************************************************************************/
INT8U PathSearch(INT8U *path, INT16U Cluster)
{
  INT8U i,j,k,len,len2;
  //����ļ�ϵͳ��Ч��
	if(!FSValid)
	  {
			Display_Str("PathSearch:��ǰ�ļ�ϵͳ��Ч��\n");
			return 0;
		}
	//���*path�ĵ�һ���ֽھ���':',�����Ŀ¼
  if(*path==':')
	  {
			if(*(path+1)=='\\')
				{//·���е�":\"���ص���Ŀ¼
					Cluster=1;
					path+=2;
				}
			else
				{//·��������
					Display_Str("PathSearch:�ļ����в��ܰ����ַ�':'��\n");
					FSLastErr=FN_INVALID_ERR;
					return 0;
				}
		}
  //��ȡ�ļ���(����'\'˵�����ļ��У�����'.'˵�����ļ�������������������Ч·��)
  len=0;
  while(*(path+len)!='\0' && *(path+len)!='\\' && *(path+len)!='.')
		{
			len++;
		}
  //�ļ������Ʋ���Ϊ��
  if(len==0)
	  {
			Display_Str("PathSearch:·��������\n");
			FSLastErr=FN_INVALID_ERR;
			return 0;
		}
  //�ļ�(��)�����ܶ���8���ֽ�
  if(len>8)
	  {
			Display_Str("PathSearch:�ļ�(��)�����ܶ���8���ֽڣ�\n");
			FSLastErr=FN_INVALID_ERR;
			return 0;
		}
  //�ļ�û����չ��
  if(*(path+len)=='\0')
	  {
			Display_Str("PathSearch:�ļ�û����չ����\n");
			FSLastErr=FN_INVALID_ERR;
			return 0;
		}
  //�������չ��
  if(*(path+len)=='.')
  	{
			len2=0;
			//��չ�����ܶ���3���ֽ�
			while(*(path+len+len2+1)!='\0')
				{
					len2++;
					if(len2>3)
						{				 
							Display_Str("PathSearch:��չ�����ܶ���3���ֽڣ�\n");
							FSLastErr=FN_INVALID_ERR;
							return 0;
						}
				}
			//��չ������Ϊ��
			if(len2==0)
				{
					Display_Str("PathSearch:��չ������Ϊ�գ�\n");
					FSLastErr=FN_INVALID_ERR;
					return 0;
				}
		}
  //�ļ����Ϸ�
	//��Ѱÿ������
  for(i=0;i<BPB.SectorsPerCluster;i++)
	  {	//��������
			if(!LoadDirSector(ClusterToSector(Cluster)+i))  
				{
					return 0;
				}
			for(j=0;j<16;j++)
				{
					//�Ƚ��ļ���
					for(k=0;k<len;k++)
						{
							if(*(path+k)!=DirSector[j*32+k]) break;
						}
					if(k<len) continue;
					//�ļ�������8�ֽڵĲ���Ӧ�Կհ׷����
					for(k=len;k<8;k++)
						{
							if(DirSector[j*32+k]>' ') break;
						}
					if(k<8) continue;
					//�ļ�����ͬ
					if(*(path+len)=='\\')
						{
							//��ǰĿ¼�����Ŀ¼�����أ���������
							if(!(DirSector[j*32+11]&0x10)) continue;
							//���ҵ�Ŀ¼�����浱ǰĿ¼������
							CurrentDir.StartCluster=DrawWord(DirSector+j*32+26);
							CurrentDir.StartSector=ClusterToSector(CurrentDir.StartCluster);
							CurrentDir.CurrentSector=0;
							for(k=0;k<=len;k++)
								{
									CurrentDir.DirPath[CurrentDir.PathLen+k]=*(path+k);
								}
							CurrentDir.DirPath[CurrentDir.PathLen+len+1]='\0';
							CurrentDir.PathLen+=len+1;
							//·���л����¼�Ŀ¼����ݹ���øú���
							if(*(path+len+1)!='\0')
								{
									return PathSearch(path+len+1,DrawWord(DirSector+j*32+26));							 
								}
							//���	-	����2��ʾ��һ���ļ���
							return DIR_TYPE;
						}
					else
						{
							for(k=0;k<len2;k++)
								{      //�Ƚ���չ��
									if(*(path+len+1+k)!=DirSector[j*32+8+k])break;
								}
							if(k<len2) continue;
							for(k=len2;k<3;k++)
								{      //��չ������3�ֽڵĲ���Ӧ�Կհ׷����
									if(DirSector[j*32+k]>' ')	break;
								}
							if(k<3) continue;
							//Ŀ���ļ��ҵ�������Ϊ��ǰ�ļ�
							for(k=0;k<8;k++)
								{					//�����ļ���
									CurrentFile.Name[k]=DirSector[j*32+k];
								}
							for(k=0;k<3;k++)
								{					//������չ��
									CurrentFile.Type[k]=DirSector[j*32+8+k];
								} 
							CurrentFile.EntrySector=ClusterToSector(Cluster)+i;
							CurrentFile.EntryAdds=j*32;
							CurrentFile.Attr=DirSector[j*32+11];
							CurrentFile.Len=DrawDword(DirSector+j*32+28);	 
							CurrentFile.StartCluster=DrawWord(DirSector+j*32+26);
							CurrentFile.CurrentCluster=CurrentFile.StartCluster;
							CurrentFile.StartSector=ClusterToSector(CurrentFile.CurrentCluster);
							CurrentFile.CurrentSector=0;
							CurrentFile.SectorIndex=0;
							CurrentFile.Pointer=0;
							CurrentFile.IsCont=CheckCont(CurrentFile.StartCluster);
							CurrentFile.State=READ;		//Ĭ����ֻ����ʽ��
							//�����һ������
							return LoadFileSector(ClusterToSector(CurrentFile.StartCluster));
						}
				}
		}
  Display_Str("  ����·��ʧ�ܣ�\n");
  return 0;
}
/*******************************************************************************
��������: �����ϼ�Ŀ¼(����CurrentDir�ṹ��)
��		��: ��
�� �� ֵ:	0 --- ����
          1 --- �ɹ�
�����Ա: ������ 
*******************************************************************************/
INT8U GoToParentDir(void)
{
  int i,j,k;
  //��ʾ
  Display_Str("�����ϼ�Ŀ¼\n");
  if(CurrentDir.StartCluster<2)
  {
    FSLastErr=OPERATION_INVALID_ERR;
    return 0;
  }
  for(i=0;i<BPB.SectorsPerCluster;i++)
  {
    if(!LoadDirSector(CurrentDir.StartSector+i))//��������
      return 0;
    for(j=0;j<16;j++)
    {
      //�Ƚ��ļ���
      if(DirSector[j*32]!='.'||DirSector[j*32+1]!='.')
        continue;
      for(k=2;k<11;k++)
      {
        if(DirSector[j*32+k]>' ')
          break;
      }
      if(k<11)
        continue;                             //���Ʋ�ƥ��
      if(!(DirSector[j*32+11]&0x10)) 
        continue;      //���Բ�ƥ��
      //���ҵ�Ŀ¼�����浱ǰĿ¼������
      CurrentDir.StartCluster=DrawWord(DirSector+j*32+26);
      CurrentDir.StartSector=ClusterToSector(CurrentDir.StartCluster);
      //ɾ��·���е��ļ�����
      do
      {
        CurrentDir.DirPath[CurrentDir.PathLen-1]=0; 
        CurrentDir.PathLen--;	 
      }
      while(CurrentDir.DirPath[CurrentDir.PathLen-1]!='\\');
      Display_Str("--�����ϼ�Ŀ¼���--\n");
      return 1;
    }
  }
  Display_Str("�ļ�ϵͳ����δ�ҵ���Ŀ¼�\n");
  return 0;
}
/*******************************************************************************
��������: ת�Ƶ�ǰ�ļ�(��)·����path·��(����CurrentDir�ṹ��)
��		��: Ŀ��·��
�� �� ֵ:	0 --- ����
          1 --- �ɹ�
�����Ա: ������ 
*******************************************************************************/
INT8U ChangeDir(INT8U *path)
{
  //��ʾ
  Display_Str("���ĵ�ǰ�ļ��У�");
  Display_Str(path);
  Display_Char('\n');
  if(PathSearch(path,CurrentDir.StartCluster)!=DIR_TYPE)
  {
    Display_Str("�Ҳ���ָ���ļ��У�\n");
    FSLastErr=NOT_FOUND_ERR;
    return 0;	 
  }
  Display_Str("--���ĵ�ǰ�ļ������--\n");
  return 1;
}
/*******************************************************************************
��������: �ļ�����
��		��: ��
�� �� ֵ:	0 --- ����
          1 --- �ɹ�
�����Ա: ������ 
*******************************************************************************/
INT8U FileSave(void){
  int k;
  INT8U ErrFlag=0;
  if(CurrentFile.State&WRITE)
  {
    //�����ļ�����
    if(!WriteBlock(CurrentFile.CurrentSector,FileSector))
    {
      ErrFlag=1;
      Display_Str("���浱ǰ�ļ������ݳ���\n");	
    }
    //���ļ���Ŀ¼��
    if(!LoadDirSector(CurrentFile.EntrySector))
    {
      ErrFlag=2;
      Display_Str("�򿪵�ǰ�ļ���Ŀ¼�����\n");
    }
    else
    {   
      //�����ļ���
      for(k=0;k<8;k++)			
        DirSector[CurrentFile.EntryAdds+k]=CurrentFile.Name[k];
      //������չ��
      for(k=0;k<3;k++)					
        DirSector[CurrentFile.EntryAdds+8+k]=CurrentFile.Type[k];
      //��������
      DirSector[CurrentFile.EntryAdds+8+11]=CurrentFile.Attr;
      //������ʼ��
      DirSector[CurrentFile.EntryAdds+26]=(INT8U)(CurrentFile.StartCluster&0x000000FF); 
      DirSector[CurrentFile.EntryAdds+27]=(INT8U)((CurrentFile.StartCluster&0x0000FF00)>>8); 
      //�����ļ���С
      DirSector[CurrentFile.EntryAdds+28]=(INT8U)(CurrentFile.Len&0x000000FF);
      DirSector[CurrentFile.EntryAdds+29]=(INT8U)((CurrentFile.Len&0x0000FF00)>>8);
      DirSector[CurrentFile.EntryAdds+30]=(INT8U)((CurrentFile.Len&0x00FF0000)>>16);
      DirSector[CurrentFile.EntryAdds+31]=(INT8U)((CurrentFile.Len&0xFF000000)>>24);
      //���浽��
      if(!WriteBlock(CurrentDir.CurrentSector,DirSector))
      {
        ErrFlag=3;
        Display_Str("���浱ǰ�ļ���Ŀ¼�����\n");
      }
      if(!SaveFAT())
      {
        ErrFlag=4;
        Display_Str("���浱ǰ�ļ���Ŀ¼�����\n");
      }
    }
  }
  if(ErrFlag==0)
    return 1;
  else
    return 0;
}
/*******************************************************************************
��������: �رղ����浱ǰ�ļ�
��    ��: ��
�� �� ֵ:	0 --- ����
          1 --- �ɹ�
�����Ա: ������ 
*******************************************************************************/
INT8U FileClose(void)
{
  CurrentFile.State=INVALID;
  return FileSave();
}
/*******************************************************************************
��������: ���ļ�
��    ��: *path   - ·��
          method  - ��ʽ('A'/'R'/'W'/'M')
�� �� ֵ: 0 --- ����
          1 --- �ɹ�
�����Ա: ������ 
*******************************************************************************/
INT8U FileOpen(INT8U *path, INT8U method)
{
  INT16U i;
  //��ʾ
  Display_Str("���ļ���");
  Display_Str(path);
  Display_Char('\n');
  //����δ�رյ��ļ�,ǿ�йر�
  if(CurrentFile.State & WRITE)
    {
      Display_Str("����δ�رյ��ļ��������ļ���ǿ�йر������ļ���\n");
      FileClose();
    }
  //��֤"��ʽ"�����Ƿ���Ч
  method=Capital(method);//Сд���д
  if(method!='A' && method!='R' && method!='W' && method!='M')
    {
      Display_Str("��������\n");
      FSLastErr=OF_PARAMETER_ERR;
      return 0;
    }
  //��Ѱ�ļ�·��
  if(PathSearch(path,CurrentDir.StartCluster)!=FILE_TYPE)
    {
      Display_Str("�Ҳ���ָ���ļ���\n");
      return 0;	 
    }
  //
  if(method=='A')
    {
      //�ҵ����һ����
      for(i=0;i<(INT16U)(CurrentFile.Len/BPB.SectorsPerCluster/BPB.BytesPerSector);i++)
        {
          CurrentFile.CurrentCluster=ReadCluster(CurrentFile.CurrentCluster);
        }
      //ת�Ƶ����һ������
      CurrentFile.SectorIndex=(CurrentFile.Len%(BPB.SectorsPerCluster*BPB.BytesPerSector))/BPB.BytesPerSector;
      CurrentFile.CurrentSector=ClusterToSector(CurrentFile.CurrentCluster)+CurrentFile.SectorIndex%BPB.SectorsPerCluster;
      //ָ���ļ���ĩβ��
      CurrentFile.Pointer=(INT16U)(CurrentFile.Len%BPB.BytesPerSector); //Ϊʲô���棿
      return LoadFileSector(CurrentFile.CurrentSector);
    } 
  else if(method=='W')
    { //"д"ģʽ
      CurrentFile.State=WRITE;
    }
  else if(method=='M')
    { //"�޸�"ģʽ
      CurrentFile.State=MODIFY;
    }
	else if(method=='R')
		{
			//"��"ģʽ
			CurrentFile.State=READ;
		}
  Display_Str("--���ļ����--\n");
  return 1;
}
//�������ʿԴ������ȫһ��!!!!!!!!!!!!!!!!!
/*******************************************************************************
��������: ɾ���ļ�(ֻ����'W'��ʽ�򿪲���ɾ���ļ�)
��    ��: ��
�� �� ֵ: 0 --- ����
          1 --- �ɹ�
�����Ա: ������ 
*******************************************************************************/
INT8U FileDelete(void)
  {
    //INT8U DelIndex=0;
    Display_Str("FileDelete:ɾ����ǰ�ļ� ");
    Display_Char('\n');
    if(CurrentFile.State==INVALID)
      {
        Display_Str("FileDelete:��ǰû���ļ���\n");
        FSLastErr=OPERATION_INVALID_ERR;
        return 0;
      }
    if(!(CurrentFile.State&WRITE))
      {
        Display_Str("FileDelete:��ǰ�ļ�������ɾ����\n");
        FSLastErr=OPERATION_INVALID_ERR;
        return 0;
      }
    //�ͷ��ļ���ռ�õ����д�
    DelClusterLink(CurrentFile.StartCluster);
    SaveFAT();
    CurrentFile.State=INVALID;
		//������ʿ-BEGIN
		//��ȡ�ļ�����Ŀ¼��ʧ��
	  if(!LoadDirSector(CurrentFile.EntrySector))
		  {
			  return 0;
			}
		//����ļ���Ŀ¼��Ϊ"��ɾ��"	
	  DirSector[CurrentFile.EntryAdds]=0xE5;
	  SaveDirSector();
		Display_Str("ɾ���ļ��ɹ�!\n");
		return 1;
		//������ʿ-END
		//����Ϊԭ�ļ�����,������ʿ��û��
		/*	
    while(1)
    {
      if(!LoadDirSector(CurrentFile.EntrySector))
			  {
          Display_Str("FileDelete���޷���ȡĿ¼�\n");
				}
      //ɾ��Ŀ¼��
      while(1)
        {
          if(DelIndex!=0)
            {
              if((DirSector[CurrentFile.EntryAdds]&0xC0)==0x40)
                {
                  DirSector[CurrentFile.EntryAdds]=0xE5;
                  SaveDirSector();
                  Display_Str("ɾ���ļ��ɹ�!\n");
                  return 1;
                }
              else if(DirSector[CurrentFile.EntryAdds]!=DelIndex)
                {
                  SaveDirSector();
                  Display_Str("ɾ���ļ��ɹ�,��Ŀ¼���д���!\n");
                  return 1;
                }
            }
          //����ļ���Ŀ¼��Ϊ"��ɾ��"	
          DirSector[CurrentFile.EntryAdds]=0xE5;
          DelIndex++;
          if(CurrentFile.EntryAdds<32)
            {
              CurrentFile.EntryAdds=480;
              SaveDirSector();
              CurrentFile.EntrySector-=1;
              if(CurrentFile.EntrySector<CurrentDir.StartSector)
                {
                  Display_Str("����ɾ��Ŀ¼��Խ�磡\n");
                  return 0;
                }
              break;
            }
          CurrentFile.EntryAdds-=32;
        }
    }
	*/		
  }
/*******************************************************************************
��������: �����ļ�
��    ��: *path - ·��
          size  - �ļ���С
�� �� ֵ: 1: success
				  11: file name error
				  12: file name length exceed 8 bytes
				  13: extend name length exceed 3 bytes
				  14: no extend name
				  15: read entry sector error
				  16: file already exist
				  17: no enough entry
				  18: no enough cluster
				  19��no enough space
				  20��can not read dir sector
				  21��write error
				  22��creat file fail 
˵    ��: 
          1,������ʿ�ж�Ӧ: u8 CreatFileInfo(u8 *path, u32 size)
          2,�Ҵ��벻��ȫ��ͬ,������ʿ���иĽ�(�����ڷ���ֵ��)
*******************************************************************************/
INT8U FileCreat(INT8U *path, INT32U size)
{
  INT16U i,j,k,len,len2;
  INT16U Entry,FirstCluster;
  INT32U Sector;
  //��ʾ
  Display_Str("�����ļ���");
  Display_Str(path);
  Display_Str("\n  ���Ϸ���...\n");
  //��֤�ļ���
  len=0;
  while(*(path+len)!='\0' && *(path+len)!='\\' && *(path+len)!='.')
    {
      len++;
    }
  if(*(path+len)!='.')
    {
      Display_Str("  �ļ�������\n");
      FSLastErr=FN_INVALID_ERR;
      return 0;
    }
  if(len>8)
    {
      Display_Str("  �ļ������ܴ���8���ֽڣ�\n");
      FSLastErr=FN_INVALID_ERR;
      return 0;
    }
  //��֤��չ��
  len2=0;
  while(*(path+len+len2+1)>' ')
    {
      len2++;
    }
  if(len2>3)
    {
      Display_Str("  ��չ�����ܶ���3���ֽڣ�\n");
      FSLastErr=FN_INVALID_ERR;
      return 0;
    }
  if(len2==0)
    {
      Display_Str("  ��չ������Ϊ�գ�\n");
      FSLastErr=FN_INVALID_ERR;
      return 0;
    }
  //�ļ����Ϸ�
  Sector=0;
  for(i=0;i<BPB.SectorsPerCluster;i++)
    {//��Ѱÿ������
      if(!LoadDirSector(CurrentDir.StartSector+i))
        {//��������
          Display_Str("����Ŀ¼����ʧ�ܣ�\n");
          return 0;
        }
      for(j=0;j<16;j++)
        {
          if(!Sector)
            {
              if((DirSector[j*32]==0xE5)||(DirSector[j*32]==0))
                {
                  Sector=CurrentDir.CurrentSector;
                  Entry=j*32;
                }	 
            }
          //�Ƚ��ļ���
          for(k=0;k<len;k++)
            {
              if(*(path+k)!=DirSector[j*32+k])
                break;
            }
          if(k<len) continue;
          //�ļ�������8�ֽڵĲ���Ӧ�Կհ׷����
          for(k=len;k<8;k++)
            {
              if(DirSector[j*32+k]>' ') 
                break;
            }
          if(k<8)   continue;
          //�ļ�����ͬ���Ƚ���չ��
          if(DirSector[j*32+8]<=' ')  continue;
          for(k=0;k<len2;k++)
            {      
              if(*(path+len+1+k)!=DirSector[j*32+8+k]) break;
            }
          if(k<len2)  continue;
          for(k=len2;k<3;k++)
            { 
              if(DirSector[j*32+8+k]>' ') break;
            }
          if(k<3)   continue;
          //�������ļ����������ļ�������
          Display_Str("�������ļ��������ļ�������\n");
          FSLastErr=FN_OCCUPIED_ERR;
          return 0;
        }
    }
  if(Sector==0)
    {
      Display_Str("  ��ǰ�ļ��е�Ŀ¼��������\n");
      FSLastErr=DIRECTER_FULL_ERR;
      return 0;
    }
  //����Ƿ����㹻�Ŀռ�
  Display_Str("  �����̿ռ�...\n");
  i=(INT16U)((size+BPB.BytesPerSector*BPB.SectorsPerCluster-1)/BPB.BytesPerSector/BPB.SectorsPerCluster);
  if(i>FreeClusterCounter)
  {
    Display_Str("  û���㹻�Ŀ��дأ�\n");
    FSLastErr=MEMORY_FULL_ERR;
    return 0;
  }
  Display_Str("  ���Է��������ռ�...\n");
  FirstCluster=CreatContChain(i);
  if(FirstCluster==0)
  {
    Display_Str("  ���������ռ�ʧ��\n");
    FirstCluster=CreatChain(i);
  }
  if(FirstCluster==0)
  { 
    Display_Str("  �洢�ռ䲻����\n");
    FSLastErr=MEMORY_FULL_ERR;
    return 0;
  }
  //�����С�ɹ�
  if(!LoadDirSector(Sector))
  {              //����Ŀ¼����������
    Display_Str("���ܶ�ȡĿ¼����������\n");
    return 0;
  }
  //����Ŀ¼��
  CreatEntry(path, path+len+1, 0x20, Entry, FirstCluster, size);
  if(!WriteBlock(Sector,DirSector))
  {
    Display_Str("дĿ¼��������\n");
    return 0;
  }
  Display_Str("--�����ļ����--\n");
  return 1;
}

void CreatEntry(INT8U *name, INT8U *type, INT8U attr, INT16U Entry, INT16U FirstCluster, INT32U size){
  INT8U k;
  for(k=0;k<8;k++)		//�����ļ���
  {	
    if(*(name+k)=='.'||*(name+k)=='\0')
      break;
    DirSector[Entry+k]=*(name+k);
  }
  for(;k<8;k++)
  {
    DirSector[Entry+k]=' ';
  }
  for(k=0;k<3;k++)		//������չ�ļ���
  {	
    if(*(type+k)=='.'||*(type+k)=='\0')
      break;
    DirSector[Entry+k+8]=*(type+k);
  }
  for(;k<3;k++)
  {
    DirSector[Entry+k+8]=' ';
  }
  DirSector[Entry+11]=attr;		//Ĭ������Ϊ"�浵"
  //ʱ��
  DirSector[Entry+14]=0x5D;
  DirSector[Entry+16]=0x5B; 
  DirSector[Entry+16]=0x7D;
  DirSector[Entry+17]=0x38;
  DirSector[Entry+18]=0x7D;
  DirSector[Entry+19]=0x38;
  DirSector[Entry+22]=0x35;
  DirSector[Entry+23]=0xB0;
  DirSector[Entry+24]=0x8A;
  DirSector[Entry+25]=0x37;
  //��ڵ�ַ
  DirSector[Entry+26]=(INT8U)(FirstCluster&0xFF);
  DirSector[Entry+27]=(INT8U)(FirstCluster>>8);
  //����
  DirSector[Entry+28]=(INT8U)(size&0xFF);
  DirSector[Entry+29]=(INT8U)((size>>8)&0xFF);
  DirSector[Entry+30]=(INT8U)((size>>16)&0xFF);
  DirSector[Entry+31]=(INT8U)((size>>24)&0xFF);
}


void SendDirList(void)
{
  int i,j,k;
  if(!FSValid)
  {
    FS_Init();
    if(!FSValid)
    {
      //OutChar(0x00); //�ظ�������
      return;
    }
  }
  for(i=0;i<BPB.SectorsPerCluster;i++)	      //����ÿ������
  {
    if(!LoadDirSector(CurrentDir.StartSector+i))	//��������
    {	
     //OutChar(0x00); //�ظ�������
     return;
    }
    for(j=0;j<16;j++){
      if(DirSector[j*32]==0xE5)       //�ļ���ɾ��
        continue;	
      if(DirSector[j*32]==0x00)       //Ŀ¼��Ϊ��
        continue;
      if(DirSector[j*32+11]==0x0F)     //Ŀ¼����Ч,������������¼һ�����ļ���
        continue;	
      //�ҵ��ļ����ļ���
      //OutChar(0xFE);                   //�ظ�����ʼ�����ļ�������12���ֽ�
      //OutChar(12);
      for(k=0;k<8;k++)               //����8���ֽڵ��ļ����������ÿո����
      { 
        if(DirSector[j*32+k]<' ')
          DirSector[j*32+k]=' ';
        //OutChar(DirSector[j*32+k]);
      }
      //OutChar('.');                   //��ʾ�ļ�����չ��
      for(k=0;k<3;k++)
      {			 
        if(DirSector[j*32+8+k]<' ')
          DirSector[j*32+8+k]=' ';
        //OutChar(DirSector[j*32+8+k]);
      }
      //OutChar(DirSector[j*32+11]);		//�ļ�����
    }
  }
  //OutChar(0xFF);	 //�ظ������
}
/******************************* END OF FILE **********************************/


/*******************************************************************************
��������: �����ļ�"XSL.TXT",����Ϊ"1982".
�����Ա: ������
*******************************************************************************/
INT8U CreatFileTest(void)
  {
    INT8U str[50]   = "XSL.TXT";
    INT32U  len = 4; 
    INT16U i;
    //�����ļ�
    if(!FileCreat(str,len))
      {
        return 0;
      }
    //���ļ�(�޸�ģʽ)
    if(!FileOpen(str,'M'))
      {
        return 0;
      }
    //װ�����ݵ�FileSector������
    FileSector[0]='1';
    FileSector[1]='9';
    FileSector[2]='8';
    FileSector[3]='2';
    //д��SD����Ӧ����
    for(i=0;i<len/BPB.BytesPerSector;i++)
      {
        if(SaveFileSector())                           //��������
          {
            if(TurnToNextFileSector())		       //ת����һ������
              {
                continue;
              }
          }
        FileDelete();
        return 0;
      }
    //
    if(len%BPB.BytesPerSector!=0)
      {
        if(!SaveFileSector())
          {
            return 0;
          }
      }
    //�ر��ļ�
    return FileClose();
  }
/*******************************************************************************
��������: ��ȡ�ļ�"XSL.TXT",����Ϊ"1982".
�����Ա: ������
*******************************************************************************/
INT8U ReadFileTest(void)
  {
    INT8U str[50]   = "XSL.TXT";
    INT32U  len = 4; 
    INT16U i;
    //���ļ�(�޸�ģʽ)
    if(!FileOpen(str,'R'))
      {
        return 0;
      }
    //װ�����ݵ�FileSector������
    //д��SD����Ӧ����
    for(i=0;i<len/BPB.BytesPerSector;i++)
      {
        /*
				if(!LoadFileSector(0))
          {
            Display_Str("��ȡBPBʧ�ܣ�\n");
            FSLastErr=SD_READ_ERR;
            return 0;
          }
				*/
				if(SaveFileSector())                           //��������
          {
            if(TurnToNextFileSector())		       //ת����һ������
              {
                continue;
              }
          }
        FileDelete();
        return 0;
      }
    //
    if(len%BPB.BytesPerSector!=0)
      {
        if(!SaveFileSector())
          {
            return 0;
          }
      }
    //�ر��ļ�
    return FileClose();
  }
/*******************************************************************************
��������: ׷�����ݲ���.
�����Ա: ������
*******************************************************************************/

/*******************************************************************************
��������: ɾ���ļ�����.
�����Ա: ������
*******************************************************************************/
INT8U FileDeleteTest(void)
  {
	  //���ļ�(�޸�ģʽ)
    if(!FileOpen("XSL.TXT",'W'))
      {
        return 0;
      }
		FileDelete();
		return 1;
  }
