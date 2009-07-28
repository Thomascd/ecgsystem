
#include "globol.h"

/*
typedef struct{  //DBR���e62byte...�H�U���Ѧ�**���N��i��
    BYTE  JumpCode[3];                  //�T�wEB 3C 90
    BYTE  OemName[8];                   //MSDOS5.0
    UINT  BytesPerSector;               //�T�w512
    BYTE  SectorsPer;
    //�C��Cluster���X��Sector 1:<32M,2:<64M,4:<128M,8:<256M,16:<512M,32:<1G,64:<2G
    UINT  ReservedSectors;              //�T�w��1
    BYTE  FATNumber;                    //�T�w��2
    UINT  MaxRoot;                      //�ڥؿ����̤j�ɮ�+��Ƨ���
    UINT  SectorsSmaller32MB;           //++<32MB��sd�d�~�ϥ�
    BYTE  MediaDescriptor;              //�T�w��F8(�w��)
    WORD  SectorsPerFAT;
    //++�q���Q��FATNumber+SectorsPerFAT+HiddenSectors..�ӨM�w�ڥؿ��ѭ��̶}�l
    WORD  SectorsPerTrack;              //�T�w��63
    WORD  NumberofHeads;                //�T�w��255
    DWORD HiddenSectors;                //SD�d�@�볣�O0
    DWORD SectorsLarger32MB;            //++>32MB��sd�d�~�ϥ�
    WORD  PartitionNumber;
    BYTE  ExtendedSignature;
    DWORD PartitionSerialNumber;
    BYTE  VolumeName[11];
    BYTE  FATName[8];
} DBR_Table;*/
/*
typedef struct   //Ū��dbr�u�n�o�X�Ӹ�T
{
   u16  Byte_Per_Sector;        //0x0B
   u8   Sector_Per_Cluster;     //0x0D
   u16  Reserve_Sector;         //0x0E
   //u8   Num_of_Fat;             //0x10
   //u16  Root_files;             //0x11
   u16  Sector_Per_Fat;         //0x16
   //u32  Hidden_Sector;          //0x1C
} FAT16_DBR;
*/
/*
typedef struct    //��짡��Address
{
   //u16  DBR_Addr;
   u32  Fat1_Addr;
   u32  Fat2_Addr;
   u32  DirTable_Addr;
   u32  Data_Addr;
} FAT_Offset;
*/
typedef struct
{
   u8  *File_Name;   //0x00~0x0A
   u16 Start_Cluster;          //0x1A~0x1B
   u32 Data_Addr;             //(start_cluster*sector per cluster*512)+Data_Addr
   u32 DirTable_Addr;          //�b�ɮת�̭�����}
   u32 Fat1_Addr;
   u32 Fat2_Addr;
   //u8 File_Attrib;             //0x0B..�]�w��0x10=��Ƨ�)..0x00=�ɮ�
   u32 File_Size;              //0x1C~0x1F
   u16 LastCluster;            //�ɧ���cluster
   u16 Point_Offset;           //�ɮפj�p mod (�C��cluster���j�p)
} SDFILE;

typedef struct    //��짡��sectors
{
   u16  Byte_Per_Sector;        //0x0B
   u8   Sector_Per_Cluster;     //0x0D
   u16  Reserve_Sector;         //0x0E
   u16  Sector_Per_Fat;         //0x16
   //FAT16_DBR    DBR;
   //FAT_Offset   Offset;
   u32  Fat1_Addr;
   u32  Fat2_Addr;
   u32  DirTable_Addr;
   u32  Data_Addr;
   u32  cardSize;
} FAT_Struct;

#define ERROR_SD_CARD 1
