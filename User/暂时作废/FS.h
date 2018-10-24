//------------------------------------------------------------------------------
#ifdef FS_GLOBAL
#define FS_EXT
#else
#define FS_EXT extern
#endif
//------------------------------------------------------------------------------
#define INVALID  0	 //无效
#define READ     1	 //读
#define WRITE    3	 //写
#define MODIFY	 7	 //修改
//-----分区表结构体(实际上DPT占用16B)
struct T_DPT
{				
  INT8U  BootIndicator;	    //分区类型:00-非活动分区 80-活动分区(启动盘) 其他-无效分区
  INT8U  StartingHead;      //分区的起始面
  INT8U  StartingSector;    //分区的起始扇区
  INT16U  StartingCylinder; //分区的起始磁道
  INT8U  SystemID;          //分区的系统类型(4-DOS 5-EXTEND 6-DOS 7-NTFS 83-LINUX)
  INT8U  EndingHead;        //分区的结束面
  INT8U  EndingSector;      //分区的结束扇区
  INT16U  EndingCylinder;   //分区的结束磁道
  INT32U RelativeSectors;   //分区的起始逻辑扇区(引导扇区)
  INT32U TotalSectors;      //分区的总扇区数
};
//BPB（BIOS Parameter Block）表,描述逻辑盘结构组成
struct T_BPB
{				
  INT16U  BytesPerSector;   //每个扇区的字节数(在此应该是512)
  INT8U  SectorsPerCluster; //每个簇有多少扇区
  INT16U  ReservedSector;   //有多少个保留扇区(FAT12和FAT16一定是1,FAT32是32)
  INT8U  NumberOfFAT;       //有多少个FAT表
  INT16U  RootEntries;      //FDT(根目录区)有多少个登记项(FAT16应该是512)
  INT16U  SmallSector;	    //扇区总数的旧式16位???
  INT8U  MediaDescriptor;   //存储介质描述(0xF8-固定存储介质,0xF0-可移动存储介质)
  INT16U  SectorsPerFAT;	  //每个FAT文件分配表所占扇区数
  INT16U  SectorsPerTrark;  //每道扇区数(对于SD卡,应该为0)
  INT16U  NumberOfhead;     //磁头数(对于SD卡,应该为0)
  INT32U HiddenSector;      //隐藏扇区数
  INT32U LargeSector;       //扇区总数
};

struct T_EBPB
{				
  INT8U FileSystemType[12]; 
};
//-----
struct T_FILE
{
  INT32U EntrySector;				//入口扇区(目录项扇区)
  INT16U EntryAdds;					//入口地址(目录项32字节所占扇区的位置)
  INT8U Name[9];						//文件名
  INT8U Type[4];						//文件扩展名
  INT8U LongName[256];			//长文件名
  INT8U LongNameLen;				//长文件名长度
  INT8U Attr;
  INT32U 	Len;
  INT16U StartCluster;			//起始簇
  INT16U CurrentCluster;		//当前簇
  INT8U IsCont; 
  INT32U 	StartSector;			//起始扇区
  INT32U 	CurrentSector;		//当前扇区
  INT32U 	SectorIndex;
  INT16U Pointer;	
  INT8U State;
};
//-----
struct T_DIR
{
  INT8U DirPath[88];	
  INT8U PathLen;	
  INT16U StartCluster;
  INT32U StartSector;
  INT16U CurrentCluster;
  INT32U CurrentSector;	
};

struct T_Entry
{
  INT8U Name[15];
  INT16U Pos;
};

struct T_DIRCache
{
  INT16U CacheLenth;
  struct T_Entry Entry[32];
};

struct T_FAT
{
  INT8U State;		
  INT16U SectorIndex;	
};

extern struct T_DPT DPT;
extern struct T_BPB BPB;
extern struct T_EBPB EBPB;
extern struct T_FILE CurrentFile;
extern struct T_DIR CurrentDir;
extern struct T_DIRCache DirCache;

extern INT8U FileSector[514];
extern INT8U FATSector[514];
extern INT8U DirSector[514];
extern INT8U FSValid;
extern INT8U DiskValid;
extern INT8U BPBValid;
extern INT8U FatType;
extern INT32U RootStartSector;
extern INT32U DataStartSector;
extern INT8U FSLastErr;

void FS_Init(void);
void SendDirList(void);
INT8U PathSearch(INT8U *path, INT16U Cluster);
INT8U ChangeDir(INT8U *path);
INT8U GoToParentDir(void);
INT8U FileOpen(INT8U *path, INT8U method);
INT8U FileSave(void);
INT8U FileClose(void);
INT8U FileCreat(INT8U *path, INT32U size);
INT8U FileDelete(void);
//INT8U CreatFileFromUsart(void);

INT8U CreatFileTest(void);
INT8U ReadFileTest(void);
INT8U FileDeleteTest(void);
