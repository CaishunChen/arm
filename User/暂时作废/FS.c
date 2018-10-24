/******************************* NEU ROBOT *************************************
* File Name          : FS.c
* Author             : Shi Enyi
* Version            : V2.0
* Date               : 10/4/2008
* Description        : FAT12/FAT16 file system Driver
嵌入式应用说明:
  1,自己编写ReadBlock()读取SD卡扇区函数
  2,自己编写WriteBlock()写入SD卡扇区函数
  3,计算出MaxBlockNum(SD卡扇区数量)
XSL-FAT16文件系统的学习:
  1,SD卡的第一个扇区:
    1.1 MBR主引导记录:0扇区--->0~445Byte(446B)
	  1.2 DPT主分区表  :0扇区--->446~509Byte(64B)
	                    记录磁盘基本分区信息(4个分区项,每项16B)
	  1.3 引导区标记   :0扇区--->510-511(2B),0xAA55为合法.
  2,初始化流程:
    2.1 读取DPT(第一个物理扇区):SD卡的扇区0
	  2.2 读取引到扇区(起始逻辑扇区)根据DPT结构体参数.

*******************************************************************************/
//------------------------------------------------------------------------------
#define FS_GLOBAL
#include "includes.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------Macro defines
#define FILE_TYPE                 1
#define DIR_TYPE                  2
#define LongNameSuport            1
//错误代码
#define SD_READ_ERR               1 //读SD卡错误
#define SD_WRITE_ERR              2 //写SD卡错误
#define FS_INVALID_ERR            3 //文件系统无效错误
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
struct T_DPT DPT;					 //DPT信息
struct T_BPB BPB;					 //BPB信息
struct T_EBPB EBPB;
struct T_FILE CurrentFile;
struct T_DIR CurrentDir;	 //当前路径信息
struct T_FAT FAT;					 //FAT状态
struct T_DIRCache DirCache;

INT8U FSLastErr=0;				//最后导致的错误代码
INT8U FATSector[514];			//FAT表扇区
INT8U FileSector[514];    //文件扇区数据缓冲区
INT8U DirSector[514];     //路径扇区数据缓冲区

INT8U DiskValid;          //DPT读取成功(磁盘是否有效)
INT8U BPBValid;           //BPB读取成功
INT8U FSValid;            //FAT读取成功

INT8U FatType;            //文件系统类型(12或16)
INT32U  RootStartSector;  //根目录的起始扇区(隐藏扇区+保留扇区+FAT占用扇区)
INT32U  DataStartSector;	//数据起始扇区(根目录的起始扇区 + FDT根目录取所占用的扇区)
INT16U FirstFreeCluster;	//第一个自由簇
INT8U LN_flag;
INT16U FreeClusterCounter;//未使用的簇数量
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
//------------------------------------------------------------------------------函数实体
//------------------------------------------------------------------------------计算-BEGIN
/*******************************************************************************
函数功能: 提取"字",低位在前
*******************************************************************************/
INT16U DrawWord(INT8U *str)
{
  return (INT16U)(*str)+((INT16U)(*(str+1))<<8);
}
/*******************************************************************************
函数功能: 提取"双字",低位在前
*******************************************************************************/
INT32U DrawDword(INT8U *str)
{
  return (INT32U)DrawWord(str)+((INT32U)DrawWord(str+2)<<16);
}
/*******************************************************************************
函数功能: 转到下一个文件扇区
参    数: 无
返 回 值: 1 --- OK
          0 --- ERR
编程人员: 徐松亮
*******************************************************************************/
INT8U TurnToNextFileSector(void)
{
  FSLastErr=0;
  if(CurrentFile.State==INVALID)
	  {
		  return 1;
	  }
  if((CurrentFile.SectorIndex+1)%BPB.SectorsPerCluster!=0)
    { //簇内的扇区地址加1
      CurrentFile.CurrentSector++;
      CurrentFile.SectorIndex++;
      return 1;
    }
  else if(ReadCluster(CurrentFile.CurrentCluster)!=0xFFFF)
    {	//簇更新,簇扇区更新
      CurrentFile.CurrentCluster=ReadCluster(CurrentFile.CurrentCluster);
      CurrentFile.CurrentSector=ClusterToSector(CurrentFile.CurrentCluster);
      CurrentFile.SectorIndex++; 
      return 1;
    }
  FSLastErr=READ_CLUSTER_ERR;
  return 0;
}
/*******************************************************************************
函数功能: 已知簇号转换为扇区号
参    数: 簇号
返 回 值: 扇区号
编程人员: 徐松亮
*******************************************************************************/
INT32U ClusterToSector(INT16U sCluster)
{
  if(sCluster<=1)
		{	//簇号0和簇号1:根目录的起始扇区(隐藏扇区+保留扇区+FAT占用扇区)
      return RootStartSector;
		}
  else
    {	//数据起始扇区(根目录的起始扇区 + FDT根目录取所占用的扇区) + (sCluster-2)*BPB.SectorsPerCluster
		  return DataStartSector+sCluster*BPB.SectorsPerCluster-2*BPB.SectorsPerCluster;
	  }
}
/*******************************************************************************
函数功能: 计算自由簇
参    数: 无
返 回 值: 自由簇个数
编程人员: 徐松亮
*******************************************************************************/
INT16U CountFreeClusters(void)
{	
  INT16U i,temp,ans=0;
  FirstFreeCluster=0;
  Display_Str("统计可用空间:\n");
  for(i=2;i<MaxBlockNum/BPB.SectorsPerCluster;i++)
    {	//根据簇号读取FAT表中数据
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
  Display_Str("    共有 ");
  Display_Num(i);
  Display_Str(" 个存储单元\n");
  Display_Str("    已用 ");
  Display_Num(i-ans);
  Display_Str(" 个存储单元\n");
  Display_Str("    剩余 ");
  Display_Num(ans);
  Display_Str(" 个存储单元\n");
  return ans;
}
//------------------------------------------------------------------------------计算-END
//------------------------------------------------------------------------------装载子函数-BEGIN
/*******************************************************************************
函数功能: 读取扇区数据到FileSector[]
参    数: SectorNum --- 扇区号
返 回 值: 1 --- OK
          0 --- ERR
编程人员: 徐松亮
*******************************************************************************/
INT8U LoadFileSector(INT32U SectorNum)
{
  //要装载的文件扇区为有效的当前的文件扇区,则不再次装载
  if((SectorNum==CurrentFile.CurrentSector)&&(CurrentFile.State!=INVALID))
    {
      return 1;
    }
  //读扇区数据到缓冲区
  if(ReadBlock(SectorNum,FileSector))
    {
      CurrentFile.CurrentSector=SectorNum;
      return 1;
    }
  Display_Str("无法读取扇区：");
  Display_Num(SectorNum);
  Display_Char('\n');
  SDValid=0;
  FSLastErr=SD_READ_ERR;
  return 0;
}
/*******************************************************************************
函数功能: 读取扇区数据到DirSector[]
参    数: SectorNum --- 扇区号
返 回 值: 1 --- OK
          0 --- ERR
编程人员: 徐松亮
*******************************************************************************/
INT8U LoadDirSector(INT32U SectorNum)
{
  //要装载的路径扇区为当前的路径扇区,则不再装载
  if(SectorNum==CurrentDir.CurrentSector)
    {
      return 1;
    }
  if(ReadBlock(SectorNum,DirSector))
    {
      CurrentDir.CurrentSector=SectorNum;
      return 1;
    }
  Display_Str("无法读取扇区：");
  Display_Num(SectorNum);
  Display_Char('\n');
  SDValid=0;
  FSLastErr=SD_READ_ERR;
  return 0;
}
/*******************************************************************************
函数功能: 装载一个FAT表扇区到FATSector[]
参    数: index --- 扇区索引(FAT表偏移)
返 回 值: 1 --- OK
          0 --- ERR
编程人员: 徐松亮
*******************************************************************************/
INT8U LoadFAT(INT16U index)
{
  //初始化后FAT.SectorIndex = 0,该值相对扇区号
  if(index==FAT.SectorIndex)
    {
      return 1;
    }
  //如果扇区修改过,则需要保存
  if(FAT.State==WRITE)
    {
      SaveFAT();
    }
  //转换为绝对扇区号,并读取该扇区
  if(ReadBlock(BPB.HiddenSector+BPB.ReservedSector+index,FATSector))
    {
      FAT.SectorIndex=index;
      FAT.State=0;
      return 1;
    }
  Display_Str("无法读取扇区：");
  Display_Num(BPB.HiddenSector+BPB.ReservedSector+index);
  Display_Char('\n');
  SDValid=0;
  FSLastErr=SD_READ_ERR;
  return 0;
}
/*******************************************************************************
函数功能: 根据簇号读取FAT表中数据(即对应文件的下一个簇号)
参    数: 簇号
返 回 值: FAT表中对应参数簇号的数据(即对应文件的下一个簇号)
编程人员: 徐松亮
*******************************************************************************/
INT16U ReadCluster(INT16U sCluster)
{
  INT16U res;
  INT16U addr;
  if(sCluster<=1)
    {	//系统簇号从2开始,自定义0xFFF0表示查询越界
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
      if(res>=0xFF0&&res<=0xFF6)//这是FAT12中的保留值
        {
          res=0xFFF;
        }
    }
  else
    {	//FAT16
      if(sCluster>=0xFFF0) 
        { //0xFFF0以上表示查表越界;
          return 0xFFF0;			
        }
      addr=sCluster*2;
      res=ReadFAT(addr);
      if(res>=0xFFF0&&res<=0xFFF6) //这是FAT16中的保留值
        {
          res=0xFFFF;
        }
    }
  if(res==0xFFF)
    {
      res=0xFFFF;          //统一FAT12和FAT16的结束标志
    }
  return res;
}
/*******************************************************************************
函数功能: 读取FAT表中一个指定地址的两个字节的内容.
参    数: addr   --- FAT表偏移字节地址
返 回 值: 0xFFF0 --- 查表越界
          0xFFFF --- 读取失败
					其他   --- 数值
编程人员: 徐松亮
*******************************************************************************/
INT16U ReadFAT(INT16U addr)
{
  INT16U res;
  if(addr>BPB.BytesPerSector*BPB.SectorsPerFAT-1)
	  { //地址超出了FAT所在的地址空间
      return 0xFFF0;	//0xFFF0表示查表越界;
		}
  if(!LoadFAT(addr/BPB.BytesPerSector))
    {	//
      Display_Str("读取FAT表失败！");
			return 0xFFFF;
    }
  res=(INT16U)FATSector[addr%BPB.BytesPerSector];
  addr++;
  if(!LoadFAT(addr/BPB.BytesPerSector))
    {
      Display_Str("读取FAT表失败！");
      return 0xFFFF;
    }
  return res+(((INT16U)FATSector[addr%BPB.BytesPerSector])<<8);
}
/*******************************************************************************
函数功能: 获取自由簇
参    数: 无
返 回 值: 0xFFFF --- 错误
          其他	 --- 自由簇位置
编程人员: 徐松亮
*******************************************************************************/
INT16U GetFreeCluster(void)
{
  INT16U i,temp;
  //如果是第一次,则从第二簇开始,否则接着上次的簇号开始
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
        {//0表示该簇没有被占用
          FirstFreeCluster=i;
          LN_flag=1;
          return i;
        }
      if(temp==0xFFF0)
        {//如果越界
          break;
        }
    }
  return 0xFFFF;			//分配错误
}
//------------------------------------------------------------------------------装载子函数-END
//------------------------------------------------------------------------------保存子函数-BEGIN
/*******************************************************************************
函数功能: 删除簇连接
参    数: 簇ID
返 回 值: 无
编程人员: 徐松亮
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
函数功能: 向FAT表写两个字节的数据
参    数: addr  --- FAT表偏移字节地址
          value --- 要写入的16位数据
返 回 值: 无
编程人员: 徐松亮
*******************************************************************************/
void WriteFAT(INT16U addr,INT16U value)
{
  //地址越界判断,FAT表中的相对地址从0开始
  if(addr>BPB.BytesPerSector*BPB.SectorsPerFAT-2)
  {
    Display_Str("WriteFAT：簇号");
    Display_Num(addr);
    Display_Str("越界！\n");
    return;
  }
  //载入FAT表,相对机制从0开始
  if(!LoadFAT(addr/BPB.BytesPerSector))
  {
    Display_Str("WriteFAT：无法载入FAT表！\n");
    SDValid=0;
    return;
  }
  FATSector[addr%BPB.BytesPerSector]=(INT8U)(value&0xFF);
  FAT.State=WRITE;
  addr++;
  if(!LoadFAT(addr/BPB.BytesPerSector))
  {
    Display_Str("WriteFAT：无法载入FAT表！\n");
    SDValid=0;
    return;
  }
  FATSector[addr%BPB.BytesPerSector]=(INT8U)((value&0xFF00)>>8);
  FAT.State=WRITE;
}
/*******************************************************************************
函数功能: 根据簇号写入FAT表中数据(即对应文件的下一个簇号)
参    数: sCluster --- 簇号
					value		 --- 写入值
返 回 值: 无
编程人员: 徐松亮
*******************************************************************************/
void WriteCluster(INT16U sCluster,INT16U value)
{
  if(sCluster<=1)
  {
    Display_Str("WriteCluster：簇号错误！\n");
    return;
  }
  if(FatType==12)
  {
    if(sCluster>0xFF0)
    {
      Display_Str("WriteCluster：地址越界！\n");
      return;
    }
    if(value==0xFFFF)
      value=0xFFF;			 
    else if(value>0xFFF||(value>=0xFF0&&value<=0xFF6))
    {
      Display_Str("WriteCluster：非法内容！\n");
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
      Display_Str("WriteCluster：地址越界！\n");
      return;
    }
    if(value>=0xFFF0&&value<=0xFFF6)
    {
      Display_Str("WriteCluster：非法内容！\n");
      return;
    }
    WriteFAT(sCluster*2,value);
  } 
}
/*******************************************************************************
函数功能: 保存FAT
参    数: index --- 索引
返 回 值: 1 --- OK
          0 --- ERR
编程人员: 徐松亮
*******************************************************************************/   
INT8U SaveFAT(void)
{
  //写第一个FAT表
  if(!WriteBlock(BPB.HiddenSector+BPB.ReservedSector+FAT.SectorIndex,FATSector))
    {
      Display_Str("SaveFAT：写FAT表错误！\n");
      FSValid=0;
      FSLastErr=SD_WRITE_ERR;
      return 0;
    }
  //写第二个FAT表
  if(!WriteBlock(BPB.HiddenSector+BPB.ReservedSector+BPB.SectorsPerFAT+FAT.SectorIndex,FATSector))
    {
      Display_Str("SaveFAT：写FAT表错误！\n");
      FSValid=0;
      FSLastErr=SD_WRITE_ERR;
      return 0;
    }
  FAT.State=0;
  return 1;
}
/*******************************************************************************
函数功能: 保存文件扇区
参    数: 无
返 回 值: 1 --- OK
          0 --- ERR
编程人员: 徐松亮
*******************************************************************************/
INT8U SaveFileSector(void)
{
  return WriteBlock(CurrentFile.CurrentSector,FileSector);		
}
/*******************************************************************************
函数功能: 保存路径扇区
参    数: 无
返 回 值: 1 --- OK
          0 --- ERR
编程人员: 徐松亮
*******************************************************************************/
void SaveDirSector(void)
{
  if(!WriteBlock(CurrentDir.CurrentSector,DirSector))
		{
      Display_Str("FileDelete：写目录出现错误！\n");
	  }
}
//------------------------------------------------------------------------------保存子函数-END
/*******************************************************************************
函数功能: 获取分区表
参    数: 无
返 回 值: 1 --- OK
          0 --- ERR
编程人员: 徐松亮
*******************************************************************************/
INT8U GetDPT(void)
{				
  Display_Str("读取硬盘分区表...\n");
  //读取扇区0的数据
  if(!LoadFileSector(0))
  {
    Display_Str("读取BPB失败！\n");
    FSLastErr=SD_READ_ERR;
    return 0;
  }
  //扇区0的最后两个字节是0x55AA,说明SD卡已经被有效格式化
  if(FileSector[510]==0x55&&FileSector[511]==0xAA)
  {	//以下调试说明为2G-SD卡
    //分区类型:      00-非活动分区 (80-活动分区) 其他-无效分区
    DPT.BootIndicator=FileSector[446];
    //分区的起始面:  0x02
    DPT.StartingHead=FileSector[447];
    //分区的起始扇区:0x03
    DPT.StartingSector=FileSector[448]>>2;
    //分区的起始磁道:0x0000
    DPT.StartingCylinder=((INT16U)(FileSector[448]&0x02)<<8)+(INT16U)FileSector[449];
    //分区的系统类型:0x06(4-DOS 5-EXTEND 6-DOS 7-NTFS 83-LINUX)
    DPT.SystemID=FileSector[450];
    //分区的结束面	:0x38
    DPT.EndingHead=FileSector[451];
    //分区的结束扇区:0x3E
    DPT.EndingSector=FileSector[452]>>2;
    //分区的结束磁道:0x00B8
    DPT.EndingCylinder=((INT16U)(FileSector[452]&0x02)<<8)+(INT16U)FileSector[453];
    //分区的起始逻辑扇区(引导扇区):0x00000089
    DPT.RelativeSectors=DrawDword(FileSector+454);
    //分区的总扇区数:0x003A9F77
    DPT.TotalSectors=DrawDword(FileSector+458);
    //
    Display_Str("有效分区大小：");
    Display_Num(DPT.TotalSectors*512);
    Display_Str(" 字节\n");
    //磁盘有效
    DiskValid=1;
    return 1;
  }
  Display_Str("当前的硬盘分区表无效！\n");
  FSLastErr=FS_INVALID_ERR;
  return 0;
}
/*******************************************************************************
函数功能: 获取BPB(BIOS 参数)数据
参    数: 无
返 回 值: 1 --- OK
          0 --- ERR
编程人员: 徐松亮
说    明: 
    1,BPB(BIOS参数块)作用:SD卡容量,FAT(文件分配表)和FDT(文件目录表)的位置和大小
*******************************************************************************/
INT8U GetBPB(void)
{
  int i;
  FatType=0;
  //如果磁盘无效,则先获取分区表信息
  if(!DiskValid) 
    {
      if(!GetDPT())
        {
          Display_Str("没有分区信息！\n");
          return 0;
        }
    }
  //装载引导扇区数据
  Display_Str("获取文件系统...\n");
  if(!LoadFileSector(DPT.RelativeSectors))
    {
      Display_Str("读取BPB失败！\n");
      return 0;
    }
  //如果引导扇区的末两字节为0x55AA,则有效
  if((FileSector[510]==0x55)&&(FileSector[511]==0xAA))
    {
      //读取文件系统属于FATXX.只识别"FAT12"和"FAT16"
      for(i=0;i<12;i++)
        {
          EBPB.FileSystemType[i]=FileSector[i+54];
        }
      Display_Str("文件系统：FAT");
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
          Display_Str("无效！\n");
          return 0;
        }
      else
        {
          //每个扇区的字节数-0x0200(512字节/扇区)
          BPB.BytesPerSector=(INT16U)FileSector[11]+((INT16U)FileSector[12]<<8);
          //每个簇有多少扇区-0x40(64个扇区/簇)
          BPB.SectorsPerCluster=FileSector[13];
          //有多少个保留扇区-0x0002
          BPB.ReservedSector=(INT16U)FileSector[14]+((INT16U)FileSector[15]<<8);
          //有多少个FAT表----0x02
          BPB.NumberOfFAT=FileSector[16];
          //FDT(根目录区)有多少个登记项(FAT16应该是512)
          BPB.RootEntries=(INT16U)FileSector[17]+((INT16U)FileSector[18]<<8);
          //扇区总数的旧式16位???	---0x0000
          BPB.SmallSector=(INT16U)FileSector[19]+((INT16U)FileSector[20]<<8);
          //存储介质描述(0xF8-固定存储介质,0xF0-可移动存储介质)
          BPB.MediaDescriptor=FileSector[21];
          //每个FAT文件分配表所占扇区数---0x00EB(235)
          BPB.SectorsPerFAT=(INT16U)FileSector[22]+((INT16U)FileSector[23]<<8);
          //每道扇区数----0x003F(对于SD卡无意义)
          BPB.SectorsPerTrark=(INT16U)FileSector[24]+((INT16U)FileSector[25]<<8);
          //磁头数---0x00FF(对于SD卡无意义)
          BPB.NumberOfhead=(INT16U)FileSector[26]+((INT16U)FileSector[27]<<8);
          //隐藏扇区数---0x00000089(FAT表所在分区前面隐藏的扇区)
          BPB.HiddenSector=DrawWord(FileSector+28);
          //扇区总数-----0x00009F77
          BPB.LargeSector=DrawWord(FileSector+32);
          //根目录的起始扇区(隐藏扇区 + 保留扇区 + FAT占用的扇区)
          RootStartSector = BPB.HiddenSector + BPB.ReservedSector +
                            BPB.NumberOfFAT * BPB.SectorsPerFAT;
          //数据起始扇区(根目录的起始扇区 + FDT根目录取所占用的扇区)
          DataStartSector = RootStartSector + 
                            BPB.RootEntries * 32 / BPB.BytesPerSector;
          //
          BPBValid=1; 
          return 1;
        }
    } 
  else
    {
      Display_Str("\n文件系统无效！\n");
      FSLastErr=FS_INVALID_ERR;
      return 0;
    }
}
/*******************************************************************************
函数功能: 获取 FAT 信息
参    数: 无
返 回 值: 1 --- OK
          0 --- ERR
编程人员: 徐松亮
*******************************************************************************/
INT8U GetFAT(void)
{
  //如果BPB无效,则重新读取BPB(BIOS 参数块)
  if(!BPBValid) 
    {
      if(!GetBPB())
        {
          Display_Str("当前文件系统无效！\n");
          SDValid=0;
          return 0;
        }
    }
  //建立FAT表
  Display_Str("建立FAT表...\n");
  if(!ReadBlock(BPB.HiddenSector+BPB.ReservedSector,FATSector))
    {
      Display_Str("读取FAT表失败！");
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
函数功能: 文件系统初始化
*******************************************************************************/
void FS_Init(void)
{
  //文件系统无效
  FSValid=0;
  //磁盘无效
  DiskValid=0;
  //BPB(Bios Paramater Block)无效
  BPBValid=0;
  //Fat类型变量初始化
  FatType=0;
  //FAT操作状态为空
  FAT.State=INVALID;
  LN_flag=0;
  //......
  Display_Str("初始化文件系统：\n");
  //如果SD卡没有被初始化,则对SD卡进行初始化
  if(!SDValid)
    {
      SD_Init();
      if(!SDValid)
        {
          Display_Str("没找到SD卡！\n");
          return;
        }
    }
  //获取DPT(分区表)数据
  if(!GetDPT())
    {
      Display_Str("GetDPT：读取失败！\n");
      return;
    }
  //获取BPB(BIOS 参数)数据
  if(!GetBPB())
    {
      Display_Str("GetBPB：读取失败！\n");
      return;
    }
  //验证扇区大小
  if(BPB.BytesPerSector != 512)
    {
      Display_Str("初始化失败，扇区大小不被支持！\n");
      return;
    }
  //获取TAT
  if(!GetFAT())
    {
      Display_Str("GetFAT：读取失败！\n");
      return;
    }
  //当前目录为根目录
  CurrentDir.StartCluster=0;              //根目录的起始簇规定为0
  CurrentDir.StartSector=RootStartSector; //根目录的起始扇区
  CurrentDir.DirPath[0]=':';              //根目录的路径规定为":\"
  CurrentDir.DirPath[1]='\\';
  CurrentDir.DirPath[2]='\0';
  CurrentDir.PathLen=2;                   //
  //当前文件无效
  CurrentFile.State=INVALID;
  FreeClusterCounter=CountFreeClusters();
  FSValid=1;
  Display_Str("初始化文件系统完毕！\n");
}
/*******************************************************************************
函数功能: 查看是否有连续num个自由簇,返回首个簇ID号
参    数: 要查询簇的个数
返 回 值: 首个簇ID号
编程人员: 徐松亮
*******************************************************************************/
INT16U SearchContFreeClusters(INT16U num)
{		
  INT16U start,counter,temp,i;
  INT8U flag=0;
  //返回第一个记录项为0的簇号
  start=GetFreeCluster();
  counter=0;
  for(i=start;;i++)
  {
    temp=ReadCluster(i);
    //未分配的簇
    if(temp==0)
      { 
        if(flag==0)
          {
            start=i;
            flag=1;
          }
        counter++;
        if(counter>=num)
          { //返回连续簇空间的首簇号
            return start;
          }
        continue;
      }
    else if(temp==0xFFF0)
      { 
        Display_Str("  分配连续空间失败！\n");
        FSLastErr=SCFC_ERR;
        return 0;
      }
    else 
      { //当前簇不是空闲簇 
        flag=0;
        counter=0;
      }
  }
}
/*******************************************************************************
函数功能: 创建连续的簇链
参    数: 创建簇链的簇个数
返 回 值: 簇链首簇ID
编程人员: 徐松亮
*******************************************************************************/
INT16U CreatContChain(INT16U num)
{
  INT16U start,i;
  start=SearchContFreeClusters(num);
  if(start!=0)
  { //分别连续簇成功
    for(i=0;i<num-1;i++)
    { //将该连续簇标记为文件记录
      WriteCluster(start+i,start+i+1);
    }
    WriteCluster(start+num-1,0xFFFF);
    SaveFAT();
  }
  return start;
}
/*******************************************************************************
函数功能: 创建簇链(可以不连续)
参    数: 创建簇链的簇个数
返 回 值: 0    --- 失败
          其他 --- 簇链首簇ID
编程人员: 徐松亮
*******************************************************************************/
INT16U CreatChain(INT16U num)
{
  INT16U start,i,next,last;
	//返回第一个记录项为0的簇号
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
          Display_Str("CreatChain：空间不足，分配失败！");
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
函数功能: 检测簇连续性
参    数: 检测的簇
返 回 值: 1 --- 连续簇
          0 --- 非连续簇
编程人员: 徐松亮
*******************************************************************************/
INT8U CheckCont(INT16U start)
{
  INT16U temp;
  for(;;)
  {
    temp=ReadCluster(start);
    if(temp==0xFFFF)
		  {	//如果FAT表指示簇为0xFFFF
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
函数功能: 文件路径搜寻(载入CurrentDir结构体)
参		数: *path		-	要搜寻的路径
					Cluster	-	
返 回 值:	0        --- 出错
          DIR_TYPE --- 文件夹
编程人员: 徐松亮 
*******************************************************************************/
INT8U PathSearch(INT8U *path, INT16U Cluster)
{
  INT8U i,j,k,len,len2;
  //检查文件系统有效性
	if(!FSValid)
	  {
			Display_Str("PathSearch:当前文件系统无效！\n");
			return 0;
		}
	//如果*path的第一个字节就是':',代表根目录
  if(*path==':')
	  {
			if(*(path+1)=='\\')
				{//路径中的":\"将回到根目录
					Cluster=1;
					path+=2;
				}
			else
				{//路径表达出错
					Display_Str("PathSearch:文件名中不能包含字符':'。\n");
					FSLastErr=FN_INVALID_ERR;
					return 0;
				}
		}
  //提取文件名(遇到'\'说明是文件夹，遇到'.'说明是文件，遇到结束符号是无效路径)
  len=0;
  while(*(path+len)!='\0' && *(path+len)!='\\' && *(path+len)!='.')
		{
			len++;
		}
  //文件夹名称不能为空
  if(len==0)
	  {
			Display_Str("PathSearch:路径名错误！\n");
			FSLastErr=FN_INVALID_ERR;
			return 0;
		}
  //文件(夹)名不能多于8个字节
  if(len>8)
	  {
			Display_Str("PathSearch:文件(夹)名不能多于8个字节！\n");
			FSLastErr=FN_INVALID_ERR;
			return 0;
		}
  //文件没有扩展名
  if(*(path+len)=='\0')
	  {
			Display_Str("PathSearch:文件没有扩展名！\n");
			FSLastErr=FN_INVALID_ERR;
			return 0;
		}
  //如果有扩展名
  if(*(path+len)=='.')
  	{
			len2=0;
			//扩展名不能多于3个字节
			while(*(path+len+len2+1)!='\0')
				{
					len2++;
					if(len2>3)
						{				 
							Display_Str("PathSearch:扩展名不能多于3个字节！\n");
							FSLastErr=FN_INVALID_ERR;
							return 0;
						}
				}
			//扩展名不能为空
			if(len2==0)
				{
					Display_Str("PathSearch:扩展名不能为空！\n");
					FSLastErr=FN_INVALID_ERR;
					return 0;
				}
		}
  //文件名合法
	//搜寻每个扇区
  for(i=0;i<BPB.SectorsPerCluster;i++)
	  {	//读入扇区
			if(!LoadDirSector(ClusterToSector(Cluster)+i))  
				{
					return 0;
				}
			for(j=0;j<16;j++)
				{
					//比较文件名
					for(k=0;k<len;k++)
						{
							if(*(path+k)!=DirSector[j*32+k]) break;
						}
					if(k<len) continue;
					//文件名不足8字节的部分应以空白符填充
					for(k=len;k<8;k++)
						{
							if(DirSector[j*32+k]>' ') break;
						}
					if(k<8) continue;
					//文件名相同
					if(*(path+len)=='\\')
						{
							//当前目录项不是子目录，返回，继续查找
							if(!(DirSector[j*32+11]&0x10)) continue;
							//已找到目录，保存当前目录项的入口
							CurrentDir.StartCluster=DrawWord(DirSector+j*32+26);
							CurrentDir.StartSector=ClusterToSector(CurrentDir.StartCluster);
							CurrentDir.CurrentSector=0;
							for(k=0;k<=len;k++)
								{
									CurrentDir.DirPath[CurrentDir.PathLen+k]=*(path+k);
								}
							CurrentDir.DirPath[CurrentDir.PathLen+len+1]='\0';
							CurrentDir.PathLen+=len+1;
							//路径中还有下级目录，则递归调用该函数
							if(*(path+len+1)!='\0')
								{
									return PathSearch(path+len+1,DrawWord(DirSector+j*32+26));							 
								}
							//完成	-	返回2表示是一个文件夹
							return DIR_TYPE;
						}
					else
						{
							for(k=0;k<len2;k++)
								{      //比较扩展名
									if(*(path+len+1+k)!=DirSector[j*32+8+k])break;
								}
							if(k<len2) continue;
							for(k=len2;k<3;k++)
								{      //扩展名不足3字节的部分应以空白符填充
									if(DirSector[j*32+k]>' ')	break;
								}
							if(k<3) continue;
							//目标文件找到，保存为当前文件
							for(k=0;k<8;k++)
								{					//拷贝文件名
									CurrentFile.Name[k]=DirSector[j*32+k];
								}
							for(k=0;k<3;k++)
								{					//拷贝扩展名
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
							CurrentFile.State=READ;		//默认以只读方式打开
							//读入第一个扇区
							return LoadFileSector(ClusterToSector(CurrentFile.StartCluster));
						}
				}
		}
  Display_Str("  搜索路径失败！\n");
  return 0;
}
/*******************************************************************************
函数功能: 返回上级目录(更新CurrentDir结构体)
参		数: 无
返 回 值:	0 --- 出错
          1 --- 成功
编程人员: 徐松亮 
*******************************************************************************/
INT8U GoToParentDir(void)
{
  int i,j,k;
  //提示
  Display_Str("返回上级目录\n");
  if(CurrentDir.StartCluster<2)
  {
    FSLastErr=OPERATION_INVALID_ERR;
    return 0;
  }
  for(i=0;i<BPB.SectorsPerCluster;i++)
  {
    if(!LoadDirSector(CurrentDir.StartSector+i))//读入扇区
      return 0;
    for(j=0;j<16;j++)
    {
      //比较文件名
      if(DirSector[j*32]!='.'||DirSector[j*32+1]!='.')
        continue;
      for(k=2;k<11;k++)
      {
        if(DirSector[j*32+k]>' ')
          break;
      }
      if(k<11)
        continue;                             //名称不匹配
      if(!(DirSector[j*32+11]&0x10)) 
        continue;      //属性不匹配
      //已找到目录，保存当前目录项的入口
      CurrentDir.StartCluster=DrawWord(DirSector+j*32+26);
      CurrentDir.StartSector=ClusterToSector(CurrentDir.StartCluster);
      //删除路径中的文件夹名
      do
      {
        CurrentDir.DirPath[CurrentDir.PathLen-1]=0; 
        CurrentDir.PathLen--;	 
      }
      while(CurrentDir.DirPath[CurrentDir.PathLen-1]!='\\');
      Display_Str("--返回上级目录完成--\n");
      return 1;
    }
  }
  Display_Str("文件系统错误：未找到父目录项！\n");
  return 0;
}
/*******************************************************************************
函数功能: 转移当前文件(夹)路径到path路径(更新CurrentDir结构体)
参		数: 目标路径
返 回 值:	0 --- 出错
          1 --- 成功
编程人员: 徐松亮 
*******************************************************************************/
INT8U ChangeDir(INT8U *path)
{
  //提示
  Display_Str("更改当前文件夹：");
  Display_Str(path);
  Display_Char('\n');
  if(PathSearch(path,CurrentDir.StartCluster)!=DIR_TYPE)
  {
    Display_Str("找不到指定文件夹！\n");
    FSLastErr=NOT_FOUND_ERR;
    return 0;	 
  }
  Display_Str("--更改当前文件夹完成--\n");
  return 1;
}
/*******************************************************************************
函数功能: 文件保存
参		数: 无
返 回 值:	0 --- 出错
          1 --- 成功
编程人员: 徐松亮 
*******************************************************************************/
INT8U FileSave(void){
  int k;
  INT8U ErrFlag=0;
  if(CurrentFile.State&WRITE)
  {
    //保存文件内容
    if(!WriteBlock(CurrentFile.CurrentSector,FileSector))
    {
      ErrFlag=1;
      Display_Str("保存当前文件的内容出错！\n");	
    }
    //打开文件的目录项
    if(!LoadDirSector(CurrentFile.EntrySector))
    {
      ErrFlag=2;
      Display_Str("打开当前文件的目录项出错！\n");
    }
    else
    {   
      //保存文件名
      for(k=0;k<8;k++)			
        DirSector[CurrentFile.EntryAdds+k]=CurrentFile.Name[k];
      //保存扩展名
      for(k=0;k<3;k++)					
        DirSector[CurrentFile.EntryAdds+8+k]=CurrentFile.Type[k];
      //保存属性
      DirSector[CurrentFile.EntryAdds+8+11]=CurrentFile.Attr;
      //保存起始簇
      DirSector[CurrentFile.EntryAdds+26]=(INT8U)(CurrentFile.StartCluster&0x000000FF); 
      DirSector[CurrentFile.EntryAdds+27]=(INT8U)((CurrentFile.StartCluster&0x0000FF00)>>8); 
      //保存文件大小
      DirSector[CurrentFile.EntryAdds+28]=(INT8U)(CurrentFile.Len&0x000000FF);
      DirSector[CurrentFile.EntryAdds+29]=(INT8U)((CurrentFile.Len&0x0000FF00)>>8);
      DirSector[CurrentFile.EntryAdds+30]=(INT8U)((CurrentFile.Len&0x00FF0000)>>16);
      DirSector[CurrentFile.EntryAdds+31]=(INT8U)((CurrentFile.Len&0xFF000000)>>24);
      //保存到卡
      if(!WriteBlock(CurrentDir.CurrentSector,DirSector))
      {
        ErrFlag=3;
        Display_Str("保存当前文件的目录项出错！\n");
      }
      if(!SaveFAT())
      {
        ErrFlag=4;
        Display_Str("保存当前文件的目录项出错！\n");
      }
    }
  }
  if(ErrFlag==0)
    return 1;
  else
    return 0;
}
/*******************************************************************************
函数功能: 关闭并保存当前文件
参    数: 无
返 回 值:	0 --- 出错
          1 --- 成功
编程人员: 徐松亮 
*******************************************************************************/
INT8U FileClose(void)
{
  CurrentFile.State=INVALID;
  return FileSave();
}
/*******************************************************************************
函数功能: 打开文件
参    数: *path   - 路径
          method  - 方式('A'/'R'/'W'/'M')
返 回 值: 0 --- 出错
          1 --- 成功
编程人员: 徐松亮 
*******************************************************************************/
INT8U FileOpen(INT8U *path, INT8U method)
{
  INT16U i;
  //提示
  Display_Str("打开文件：");
  Display_Str(path);
  Display_Char('\n');
  //有尚未关闭的文件,强行关闭
  if(CurrentFile.State & WRITE)
    {
      Display_Str("有尚未关闭的文件，打开新文件将强行关闭其它文件！\n");
      FileClose();
    }
  //验证"方式"参数是否有效
  method=Capital(method);//小写变大写
  if(method!='A' && method!='R' && method!='W' && method!='M')
    {
      Display_Str("参数错误！\n");
      FSLastErr=OF_PARAMETER_ERR;
      return 0;
    }
  //搜寻文件路径
  if(PathSearch(path,CurrentDir.StartCluster)!=FILE_TYPE)
    {
      Display_Str("找不到指定文件！\n");
      return 0;	 
    }
  //
  if(method=='A')
    {
      //找到最后一个簇
      for(i=0;i<(INT16U)(CurrentFile.Len/BPB.SectorsPerCluster/BPB.BytesPerSector);i++)
        {
          CurrentFile.CurrentCluster=ReadCluster(CurrentFile.CurrentCluster);
        }
      //转移到最后一个扇区
      CurrentFile.SectorIndex=(CurrentFile.Len%(BPB.SectorsPerCluster*BPB.BytesPerSector))/BPB.BytesPerSector;
      CurrentFile.CurrentSector=ClusterToSector(CurrentFile.CurrentCluster)+CurrentFile.SectorIndex%BPB.SectorsPerCluster;
      //指向文件的末尾。
      CurrentFile.Pointer=(INT16U)(CurrentFile.Len%BPB.BytesPerSector); //为什么警告？
      return LoadFileSector(CurrentFile.CurrentSector);
    } 
  else if(method=='W')
    { //"写"模式
      CurrentFile.State=WRITE;
    }
  else if(method=='M')
    { //"修改"模式
      CurrentFile.State=MODIFY;
    }
	else if(method=='R')
		{
			//"读"模式
			CurrentFile.State=READ;
		}
  Display_Str("--打开文件完成--\n");
  return 1;
}
//与电梯卫士源程序不完全一样!!!!!!!!!!!!!!!!!
/*******************************************************************************
函数功能: 删除文件(只有以'W'方式打开才能删除文件)
参    数: 无
返 回 值: 0 --- 出错
          1 --- 成功
编程人员: 徐松亮 
*******************************************************************************/
INT8U FileDelete(void)
  {
    //INT8U DelIndex=0;
    Display_Str("FileDelete:删除当前文件 ");
    Display_Char('\n');
    if(CurrentFile.State==INVALID)
      {
        Display_Str("FileDelete:当前没有文件！\n");
        FSLastErr=OPERATION_INVALID_ERR;
        return 0;
      }
    if(!(CurrentFile.State&WRITE))
      {
        Display_Str("FileDelete:当前文件不允许被删除！\n");
        FSLastErr=OPERATION_INVALID_ERR;
        return 0;
      }
    //释放文件所占用的所有簇
    DelClusterLink(CurrentFile.StartCluster);
    SaveFAT();
    CurrentFile.State=INVALID;
		//电梯卫士-BEGIN
		//读取文件所在目录项失败
	  if(!LoadDirSector(CurrentFile.EntrySector))
		  {
			  return 0;
			}
		//标记文件的目录项为"已删除"	
	  DirSector[CurrentFile.EntryAdds]=0xE5;
	  SaveDirSector();
		Display_Str("删除文件成功!\n");
		return 1;
		//电梯卫士-END
		//以下为原文件程序,电梯卫士中没有
		/*	
    while(1)
    {
      if(!LoadDirSector(CurrentFile.EntrySector))
			  {
          Display_Str("FileDelete：无法读取目录项！\n");
				}
      //删除目录项
      while(1)
        {
          if(DelIndex!=0)
            {
              if((DirSector[CurrentFile.EntryAdds]&0xC0)==0x40)
                {
                  DirSector[CurrentFile.EntryAdds]=0xE5;
                  SaveDirSector();
                  Display_Str("删除文件成功!\n");
                  return 1;
                }
              else if(DirSector[CurrentFile.EntryAdds]!=DelIndex)
                {
                  SaveDirSector();
                  Display_Str("删除文件成功,但目录中有错误!\n");
                  return 1;
                }
            }
          //标记文件的目录项为"已删除"	
          DirSector[CurrentFile.EntryAdds]=0xE5;
          DelIndex++;
          if(CurrentFile.EntryAdds<32)
            {
              CurrentFile.EntryAdds=480;
              SaveDirSector();
              CurrentFile.EntrySector-=1;
              if(CurrentFile.EntrySector<CurrentDir.StartSector)
                {
                  Display_Str("错误：删除目录项越界！\n");
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
函数功能: 创建文件
参    数: *path - 路径
          size  - 文件大小
返 回 值: 1: success
				  11: file name error
				  12: file name length exceed 8 bytes
				  13: extend name length exceed 3 bytes
				  14: no extend name
				  15: read entry sector error
				  16: file already exist
				  17: no enough entry
				  18: no enough cluster
				  19：no enough space
				  20：can not read dir sector
				  21：write error
				  22：creat file fail 
说    明: 
          1,电梯卫士中对应: u8 CreatFileInfo(u8 *path, u32 size)
          2,且代码不完全相同,电梯卫士中有改进(体现在返回值上)
*******************************************************************************/
INT8U FileCreat(INT8U *path, INT32U size)
{
  INT16U i,j,k,len,len2;
  INT16U Entry,FirstCluster;
  INT32U Sector;
  //提示
  Display_Str("创建文件：");
  Display_Str(path);
  Display_Str("\n  检查合法性...\n");
  //验证文件名
  len=0;
  while(*(path+len)!='\0' && *(path+len)!='\\' && *(path+len)!='.')
    {
      len++;
    }
  if(*(path+len)!='.')
    {
      Display_Str("  文件名错误！\n");
      FSLastErr=FN_INVALID_ERR;
      return 0;
    }
  if(len>8)
    {
      Display_Str("  文件名不能大于8个字节！\n");
      FSLastErr=FN_INVALID_ERR;
      return 0;
    }
  //验证扩展名
  len2=0;
  while(*(path+len+len2+1)>' ')
    {
      len2++;
    }
  if(len2>3)
    {
      Display_Str("  扩展名不能多于3个字节！\n");
      FSLastErr=FN_INVALID_ERR;
      return 0;
    }
  if(len2==0)
    {
      Display_Str("  扩展名不能为空！\n");
      FSLastErr=FN_INVALID_ERR;
      return 0;
    }
  //文件名合法
  Sector=0;
  for(i=0;i<BPB.SectorsPerCluster;i++)
    {//搜寻每个扇区
      if(!LoadDirSector(CurrentDir.StartSector+i))
        {//读入扇区
          Display_Str("读入目录扇区失败！\n");
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
          //比较文件名
          for(k=0;k<len;k++)
            {
              if(*(path+k)!=DirSector[j*32+k])
                break;
            }
          if(k<len) continue;
          //文件名不足8字节的部分应以空白符填充
          for(k=len;k<8;k++)
            {
              if(DirSector[j*32+k]>' ') 
                break;
            }
          if(k<8)   continue;
          //文件名相同，比较扩展名
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
          //创建的文件名与现有文件名重名
          Display_Str("创建的文件与现有文件重名！\n");
          FSLastErr=FN_OCCUPIED_ERR;
          return 0;
        }
    }
  if(Sector==0)
    {
      Display_Str("  当前文件夹的目录项已满！\n");
      FSLastErr=DIRECTER_FULL_ERR;
      return 0;
    }
  //检查是否有足够的空间
  Display_Str("  检查磁盘空间...\n");
  i=(INT16U)((size+BPB.BytesPerSector*BPB.SectorsPerCluster-1)/BPB.BytesPerSector/BPB.SectorsPerCluster);
  if(i>FreeClusterCounter)
  {
    Display_Str("  没有足够的空闲簇！\n");
    FSLastErr=MEMORY_FULL_ERR;
    return 0;
  }
  Display_Str("  尝试分配连续空间...\n");
  FirstCluster=CreatContChain(i);
  if(FirstCluster==0)
  {
    Display_Str("  分配连续空间失败\n");
    FirstCluster=CreatChain(i);
  }
  if(FirstCluster==0)
  { 
    Display_Str("  存储空间不够！\n");
    FSLastErr=MEMORY_FULL_ERR;
    return 0;
  }
  //分配大小成功
  if(!LoadDirSector(Sector))
  {              //读入目录项所在扇区
    Display_Str("不能读取目录所在扇区！\n");
    return 0;
  }
  //创建目录项
  CreatEntry(path, path+len+1, 0x20, Entry, FirstCluster, size);
  if(!WriteBlock(Sector,DirSector))
  {
    Display_Str("写目录发生错误！\n");
    return 0;
  }
  Display_Str("--创建文件完成--\n");
  return 1;
}

void CreatEntry(INT8U *name, INT8U *type, INT8U attr, INT16U Entry, INT16U FirstCluster, INT32U size){
  INT8U k;
  for(k=0;k<8;k++)		//拷贝文件名
  {	
    if(*(name+k)=='.'||*(name+k)=='\0')
      break;
    DirSector[Entry+k]=*(name+k);
  }
  for(;k<8;k++)
  {
    DirSector[Entry+k]=' ';
  }
  for(k=0;k<3;k++)		//拷贝扩展文件名
  {	
    if(*(type+k)=='.'||*(type+k)=='\0')
      break;
    DirSector[Entry+k+8]=*(type+k);
  }
  for(;k<3;k++)
  {
    DirSector[Entry+k+8]=' ';
  }
  DirSector[Entry+11]=attr;		//默认属性为"存档"
  //时间
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
  //入口地址
  DirSector[Entry+26]=(INT8U)(FirstCluster&0xFF);
  DirSector[Entry+27]=(INT8U)(FirstCluster>>8);
  //长度
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
      //OutChar(0x00); //回复：出错
      return;
    }
  }
  for(i=0;i<BPB.SectorsPerCluster;i++)	      //遍历每个扇区
  {
    if(!LoadDirSector(CurrentDir.StartSector+i))	//读入扇区
    {	
     //OutChar(0x00); //回复：出错
     return;
    }
    for(j=0;j<16;j++){
      if(DirSector[j*32]==0xE5)       //文件已删除
        continue;	
      if(DirSector[j*32]==0x00)       //目录项为空
        continue;
      if(DirSector[j*32+11]==0x0F)     //目录项无效,它可能用来记录一个长文件名
        continue;	
      //找到文件或文件夹
      //OutChar(0xFE);                   //回复：开始传送文件名，共12个字节
      //OutChar(12);
      for(k=0;k<8;k++)               //发送8个字节的文件名，不足用空格填充
      { 
        if(DirSector[j*32+k]<' ')
          DirSector[j*32+k]=' ';
        //OutChar(DirSector[j*32+k]);
      }
      //OutChar('.');                   //显示文件的扩展名
      for(k=0;k<3;k++)
      {			 
        if(DirSector[j*32+8+k]<' ')
          DirSector[j*32+8+k]=' ';
        //OutChar(DirSector[j*32+8+k]);
      }
      //OutChar(DirSector[j*32+11]);		//文件属性
    }
  }
  //OutChar(0xFF);	 //回复：完毕
}
/******************************* END OF FILE **********************************/


/*******************************************************************************
函数功能: 创建文件"XSL.TXT",内容为"1982".
编程人员: 徐松亮
*******************************************************************************/
INT8U CreatFileTest(void)
  {
    INT8U str[50]   = "XSL.TXT";
    INT32U  len = 4; 
    INT16U i;
    //创建文件
    if(!FileCreat(str,len))
      {
        return 0;
      }
    //打开文件(修改模式)
    if(!FileOpen(str,'M'))
      {
        return 0;
      }
    //装载数据到FileSector缓冲区
    FileSector[0]='1';
    FileSector[1]='9';
    FileSector[2]='8';
    FileSector[3]='2';
    //写到SD卡响应扇区
    for(i=0;i<len/BPB.BytesPerSector;i++)
      {
        if(SaveFileSector())                           //保存数据
          {
            if(TurnToNextFileSector())		       //转到下一个扇区
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
    //关闭文件
    return FileClose();
  }
/*******************************************************************************
函数功能: 读取文件"XSL.TXT",内容为"1982".
编程人员: 徐松亮
*******************************************************************************/
INT8U ReadFileTest(void)
  {
    INT8U str[50]   = "XSL.TXT";
    INT32U  len = 4; 
    INT16U i;
    //打开文件(修改模式)
    if(!FileOpen(str,'R'))
      {
        return 0;
      }
    //装载数据到FileSector缓冲区
    //写到SD卡响应扇区
    for(i=0;i<len/BPB.BytesPerSector;i++)
      {
        /*
				if(!LoadFileSector(0))
          {
            Display_Str("读取BPB失败！\n");
            FSLastErr=SD_READ_ERR;
            return 0;
          }
				*/
				if(SaveFileSector())                           //保存数据
          {
            if(TurnToNextFileSector())		       //转到下一个扇区
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
    //关闭文件
    return FileClose();
  }
/*******************************************************************************
函数功能: 追加数据测试.
编程人员: 徐松亮
*******************************************************************************/

/*******************************************************************************
函数功能: 删除文件测试.
编程人员: 徐松亮
*******************************************************************************/
INT8U FileDeleteTest(void)
  {
	  //打开文件(修改模式)
    if(!FileOpen("XSL.TXT",'W'))
      {
        return 0;
      }
		FileDelete();
		return 1;
  }
