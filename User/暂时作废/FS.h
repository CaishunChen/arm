//------------------------------------------------------------------------------
#ifdef FS_GLOBAL
#define FS_EXT
#else
#define FS_EXT extern
#endif
//------------------------------------------------------------------------------
#define INVALID  0	 //��Ч
#define READ     1	 //��
#define WRITE    3	 //д
#define MODIFY	 7	 //�޸�
//-----������ṹ��(ʵ����DPTռ��16B)
struct T_DPT
{				
  INT8U  BootIndicator;	    //��������:00-�ǻ���� 80-�����(������) ����-��Ч����
  INT8U  StartingHead;      //��������ʼ��
  INT8U  StartingSector;    //��������ʼ����
  INT16U  StartingCylinder; //��������ʼ�ŵ�
  INT8U  SystemID;          //������ϵͳ����(4-DOS 5-EXTEND 6-DOS 7-NTFS 83-LINUX)
  INT8U  EndingHead;        //�����Ľ�����
  INT8U  EndingSector;      //�����Ľ�������
  INT16U  EndingCylinder;   //�����Ľ����ŵ�
  INT32U RelativeSectors;   //��������ʼ�߼�����(��������)
  INT32U TotalSectors;      //��������������
};
//BPB��BIOS Parameter Block����,�����߼��̽ṹ���
struct T_BPB
{				
  INT16U  BytesPerSector;   //ÿ���������ֽ���(�ڴ�Ӧ����512)
  INT8U  SectorsPerCluster; //ÿ�����ж�������
  INT16U  ReservedSector;   //�ж��ٸ���������(FAT12��FAT16һ����1,FAT32��32)
  INT8U  NumberOfFAT;       //�ж��ٸ�FAT��
  INT16U  RootEntries;      //FDT(��Ŀ¼��)�ж��ٸ��Ǽ���(FAT16Ӧ����512)
  INT16U  SmallSector;	    //���������ľ�ʽ16λ???
  INT8U  MediaDescriptor;   //�洢��������(0xF8-�̶��洢����,0xF0-���ƶ��洢����)
  INT16U  SectorsPerFAT;	  //ÿ��FAT�ļ��������ռ������
  INT16U  SectorsPerTrark;  //ÿ��������(����SD��,Ӧ��Ϊ0)
  INT16U  NumberOfhead;     //��ͷ��(����SD��,Ӧ��Ϊ0)
  INT32U HiddenSector;      //����������
  INT32U LargeSector;       //��������
};

struct T_EBPB
{				
  INT8U FileSystemType[12]; 
};
//-----
struct T_FILE
{
  INT32U EntrySector;				//�������(Ŀ¼������)
  INT16U EntryAdds;					//��ڵ�ַ(Ŀ¼��32�ֽ���ռ������λ��)
  INT8U Name[9];						//�ļ���
  INT8U Type[4];						//�ļ���չ��
  INT8U LongName[256];			//���ļ���
  INT8U LongNameLen;				//���ļ�������
  INT8U Attr;
  INT32U 	Len;
  INT16U StartCluster;			//��ʼ��
  INT16U CurrentCluster;		//��ǰ��
  INT8U IsCont; 
  INT32U 	StartSector;			//��ʼ����
  INT32U 	CurrentSector;		//��ǰ����
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
