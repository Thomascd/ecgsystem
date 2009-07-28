#include  "Globol.h"
#include  "System/mmc.h"
#include  "System/Fat16.h"


//u16 Next=0;  //���}�Ϊ�

FAT_Struct *FAT16;
UINT File_Error;

void InitFAT16(){
   u32 address=0;
   read_from_sdcard(address,32,sd_buffer);
   FAT16->Byte_Per_Sector=sd_buffer[0x0B]|(u16)sd_buffer[0x0C]<<8;        //0x0B=X
   FAT16->Sector_Per_Cluster=sd_buffer[0x0D];                                  //0x0D=Y
   FAT16->Reserve_Sector=sd_buffer[0x0E]|(u16)(sd_buffer[0x0F]<<8);       //0x0E=A
   //FAT16->Num_of_Fat=sd_buffer[0x10];                                          //0x10=B
   //FAT16->Root_files=sd_buffer[0x11]|(u16)(sd_buffer[0x12]<<8);           //0x11=E
   FAT16->Sector_Per_Fat=sd_buffer[0x16]|(u16)(sd_buffer[0x17]<<8);       //0x16=C
   //FAT16->Hidden_Sector=sd_buffer[0x11]|
   //                        (u32)(sd_buffer[0x12]<<8)|
   //                        (u32)(sd_buffer[0x12]<<16)|
   //                        (u32)(sd_buffer[0x12]<<24);                           //0x1C=D
    //--------------------------------------------------------------------
   FAT16->Fat1_Addr=(u32)(FAT16->Byte_Per_Sector*(FAT16->Reserve_Sector));
   FAT16->Fat2_Addr=(u32)FAT16->Fat1_Addr+((u32)FAT16->Sector_Per_Fat*(u32)FAT16->Byte_Per_Sector);
   FAT16->DirTable_Addr=(u32)FAT16->Fat2_Addr+((u32)FAT16->Sector_Per_Fat*(u32)FAT16->Byte_Per_Sector);
   FAT16->Data_Addr=(u32)FAT16->DirTable_Addr+(512*32);
   //FAT16->DBR_Addr=0;
}

void InitFATwithFormat(u8 *Label){

  InitFAT16();
  //u32 Count=(0xFFFFFFFF&((FAT16->Sector_Per_Fat*FAT16->Num_of_Fat)+32))<<9;
  u32 Count=(0xFFFFFFFF&((FAT16->Sector_Per_Fat*2)+32))<<9;
  //u16 Count=(FAT16->Sector_Per_Fat*FAT16->Num_of_Fat)+32;
  //u32 Start=(u32)(FAT16->Fat1_Addr&0xFFFFFFFF);
  //erase(Start,Count);
  for (int j=0;j<512;j++)sd_buffer[j]=0;
  for (u32 i=FAT16->Fat1_Addr;i<Count;i+=512){
    write_to_sdcard(i,512,sd_buffer);
  }
  //-------------------------------------------------------------------�H�W�R���Ҧ�fat&Dir Table
   u8 FileTable[32]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
                     0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                     0x00,0x00,  //StartCluster
                     0x00,0x00,0x00,0x00}; //File_Size
   for(int i=0;i<11;i++){FileTable[i]=Label[i];}
   write_to_sdcard(FAT16->DirTable_Addr,32,FileTable);
   //-------------------------------------------------------------------�H�W�g�J�Ϻм���
   u8 Fatdata[4]={0xF8,0xFF,0xFF,0xFF};
   //-------------------------------------------------------------------�H�W�g�Jfat1,2�}�Y
   write_to_sdcard(FAT16->Fat1_Addr,4,Fatdata);
   write_to_sdcard(FAT16->Fat2_Addr,4,Fatdata);
}


BOOL InitFileSystem()
{
    File_Error = 0;
    FAT16 = (FAT_Struct *)malloc(sizeof(FAT_Struct));
    if (FAT16 == 0) return 0;
    else{
        char *temp;
        temp = (char *)FAT16;
        int i = sizeof(FAT_Struct);
        while(i--) temp[i]=0;
    }
    spi_set_divisor(2);   //�]�w��clock /2
    BYTE i=1;
    while(initMMC() == MMC_INIT_ERROR && ++i > 0);
    if (i == 0){
        File_Error |= ERROR_SD_CARD;
        return 0;
    }
    FAT16->cardSize =  MMC_ReadCardSize();
    InitFATwithFormat("GENIUSTOMSD");  //�R���Ҧ��ɫ��í��s�R�W����
    return 1;
}


void GetFileInfo(SDFILE *Dfile){
   Dfile->Start_Cluster=sd_buffer[0x1A]|(u16)sd_buffer[0x1B]<<8;          //0x1A~0x1B
   Dfile->Data_Addr=(u32)(Dfile->Start_Cluster-2)*         //((start_cluster-2)*sector per cluster*512)+Data_Addr
                    (u32)FAT16->Byte_Per_Sector*
                    (u32)FAT16->Sector_Per_Cluster+
                    (u32)FAT16->Data_Addr;
   Dfile->Fat1_Addr=(u32)FAT16->Fat1_Addr+(u32)(Dfile->Start_Cluster*2);
   //Start_Cluster�q2�}�l..�ҥH��2..�@��CLUSTER��2BYTE..�ҥH*2..���O�����@�}�l��4BYTE..�ҥH��P�F
   Dfile->Fat2_Addr=(u32)FAT16->Fat2_Addr+(u32)(Dfile->Start_Cluster*2);
   //Dfile->File_Attrib=sd_buffer[0x0B];             //0x0B..�]�w��0x10=��Ƨ�)..0x00=�ɮ�
   Dfile->File_Size=sd_buffer[0x1C]|
                    (u32)(sd_buffer[0x1D]<<8)|
                    (u32)(sd_buffer[0x1E]<<16)|
                    (u32)(sd_buffer[0x1F]<<24);              //0x1C~0x1F
}


void SetFileInfo(u16 StartCluster,SDFILE *Dfile,ULONG *DateTime)  //�إ߷s�ɪ��B�J�A��sDIR TABLE�üg�JSD�d
{
   Dfile->Start_Cluster=StartCluster;          //0x1A~0x1B
   Dfile->Data_Addr=(u32)(Dfile->Start_Cluster-2)*         //((start_cluster-2)*sector per cluster*512)+Data_Addr
                    (u32)FAT16->Byte_Per_Sector*
                    (u32)FAT16->Sector_Per_Cluster+
                    (u32)FAT16->Data_Addr;
   Dfile->Fat1_Addr=(u32)FAT16->Fat1_Addr+(u32)(Dfile->Start_Cluster*2);
   //Start_Cluster�q2�}�l..�ҥH��2..�@��CLUSTER��2BYTE..�ҥH*2..���O�����@�}�l��4BYTE..�ҥH��P�F
   Dfile->Fat2_Addr=(u32)FAT16->Fat2_Addr+(u32)(Dfile->Start_Cluster*2);
   //Dfile->File_Attrib=0x00;             //0x0B..�]�w��0x10=��Ƨ�)..0x00=�ɮ�
   Dfile->File_Size=0x00000000;              //0x1C~0x1F
   u16 *temp;
   temp = &StartCluster;
   u8 FileTable[32]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
                     0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                     0x00,0x00,
                     0x00,0x00,0x00,0x00}; //File_Size
   FileTable[26] = (StartCluster & 0x00FF);
   FileTable[27] = StartCluster >> 8;
   ULONG *temp2;
   temp2 = (ULONG *)(FileTable+0x16);
   *temp2 = *DateTime;
   for(int i=0;i<11;i++){FileTable[i]=Dfile->File_Name[i];}
   write_to_sdcard(Dfile->DirTable_Addr,32,FileTable);
}


BOOL Ffind(u32 StartAddr,SDFILE *Dfile){ //�M���ɮ�..���G�Ǧ^��}�A�䤣��G�Ǧ^�i�Φ�}
  //u32 NewAddr=0;
  BOOL find=0;
  int i,j,k;
  //-----------------------------------------���ɮ�-------------------------------------
  for(i=0;i<32;i++){
    read_from_sdcard(StartAddr+(512*i),512,sd_buffer);  //�C��Ū��512btye
    for(j=0;j<512;j+=32){  //�C32byte���@����
      for(k=0;k<11;k++){    //�M����w�ɦW�P���ɦW
        if (Dfile->File_Name[k]!=sd_buffer[j+k]){break;}
        if (k==10&&(Dfile->File_Name[k]==sd_buffer[j+k])){
          find=1;
        } //����ɮפF
      }
      if(find==1){ break; }  //����ɮפF�A�N���X
      if(sd_buffer[j]==0){break;}  //�p�G�ݨ��1��BYTE�O0�A��ܤw�g�j�M���ثe�Ҧ����ɮפF�A�]��ܥثe����}�O�i�ΪŶ�
      StartAddr+=32;
    }
    if(find==1){ break; }  //����ɮפF�A�N���X
    if(sd_buffer[j]==0){break;}  //�p�G�ݨ��1��BYTE�O0�A��ܤw�g�j�M���ثe�Ҧ����ɮפF�A�]��ܥثe����}�O�i�ΪŶ�

  }

  //--------------------------------�䧹�ɮסA�^�ǥi�Ϊ��_�l��}----------------------------
  if (find==1){  //����ɮסA�ç��T��i�h
    Dfile->DirTable_Addr=StartAddr;
    GetFileInfo(Dfile);
  } //������ɮסG���ɮת���}
  else{
    //if(NewAddr==0){NewAddr=StartAddr;}  //�p�G���e�䪺�S��"�w�R���i�ΪŶ�"�A�N�^��"���ϥΪ��Ŷ�"
    Dfile->DirTable_Addr=StartAddr;  //�S���G�H�Ĥ@��00�Τw�R������}���_�l��}
  }
  return find;
}


u16 FindNewCluster(){  //�qFAT���M��i�Ϊ�"Cluster��}"�A�]�w���w�ΡA�ç�StartCluster�Ǧ^
  u32 StartAddr=0;   //FAT2���_�l��}
  u16 ClusterValue;
  u16 Offset=0;  //�����h�֤~���0x0000
  u16 StartCluster=0;  //
  int i,j;
  u8 FindFlag=0;
  for(i=0;i<FAT16->Sector_Per_Fat;i++){ //FAT��̦h��(DBR.Sector_Per_Fat)��SECTORS
    read_from_sdcard(FAT16->Fat2_Addr+StartAddr,512,sd_buffer);
    for(j=0;j<511;j+=2){
      ClusterValue=(u16)(sd_buffer[j+1]<<8)|sd_buffer[j];  //���oFAT��BUFFER����
      if (ClusterValue==0x0000||(ClusterValue>>8==0xE5)){    //�p�G�O�i�ΪŶ�
        u8 EndBuffer[2]={0xFF,0xFF};
        write_to_sdcard(FAT16->Fat1_Addr+StartAddr,2,EndBuffer);  //�g�JFFFF...��ܭn�Φ��Ŷ��F
        write_to_sdcard(FAT16->Fat2_Addr+StartAddr,2,EndBuffer);
        FindFlag=1;
        return Offset;  //�^��OFFSET
        break;
      }
      Offset++;
    }
    if(FindFlag==1) break;
    StartAddr+=512;
  }
}


//------------------------------------------------------------------------------------------------------------------------------------------
BOOL FOpen(u8 *fName,SDFILE *Dfile){
  //for(int i=0;i<11;i++) Dfile->File_Name[i]=fName[i];
  Dfile->File_Name=fName;
  BOOL find=Ffind(FAT16->DirTable_Addr,Dfile); //�qDATA_TABLE�}�l��

  if (find==0){  //��줣���ɮסAAPPEND�P�_�L�ҿסA���إߤ@�ӷs��
    ULONG DateTime;
    SynthethizeDT(&DateTime);
    SetFileInfo(FindNewCluster(),Dfile,&DateTime); //�b�Ĥ@��BYTE��00��E5����줤�л\�A�üg�J�s��  <--OK
    Dfile->LastCluster=Dfile->Start_Cluster;  //�طs�ɪ��ɭ�..�u���@��
    Dfile->Point_Offset=0;
    return 1;
  }else return 0;
  
  //��find=1��..�u��Ū����g..�_�h�|����..���g�b�o��O�]���bFfind���N���ɮ׸�T���J�F
}



void FWrite(u16 Length,unsigned char *buf,SDFILE *File){
   File->File_Size+=Length;     //increase Size
   u32 ClusterOffset=File->Point_Offset+Length;
   //u32 AddrOffset,AddrFat1,AddrFat2;
   File->Point_Offset=File->File_Size%(FAT16->Sector_Per_Cluster*512);
   if (ClusterOffset>=(FAT16->Sector_Per_Cluster*512)){File->LastCluster++;}
   //---------------------------------------------------------------------�g�J��ƨ�data��
   write_to_sdcard(File->Data_Addr,Length,buf);
   File->Data_Addr+=Length;   //���ثe���ɮ׫���
}



void FRead(u16 Length,unsigned char *buf,SDFILE *File,u32 Index){
    u32 Fileoffset=File->Data_Addr-File->File_Size+Index;
    read_from_sdcard(Fileoffset,Length,buf);
}

void FClose(SDFILE *File)
{
  u32 AddrFat1=(u32)(File->Fat1_Addr);  //FAT1��start_cluster�Ҧb��}
  u32 AddrFat2=File->Fat2_Addr; //FAT2��start_cluster�Ҧb��}
  u16 ClusterOffset=(File->Start_Cluster*2)%512; //�_�lcluster�۹��512���Ʀ�}�������q
  long ClusterSize=(File->LastCluster-File->Start_Cluster)*2;  //�̫᪺cluster�P�_�l��cluster�bfat�����j
//---------------------------------------------------------------------�g�J�ɮפj�p��dir��
  u32 *SizeTable = &File->File_Size;
/*  u8 SizeTable[4]={(u8)(File->File_Size&0xFF),
                 (u8)(File->File_Size>>8&0xFF),
                 (u8)(File->File_Size>>16&0xFF),
                 (u8)(File->File_Size>>24&0xFF)}; //File_Size*/
  write_to_sdcard(File->DirTable_Addr+28,4,(u8 *)SizeTable);
//---------------------------------------------------------------------�g�J�쵲��fat��
  u16 Cluster=File->Start_Cluster;
  for(int i=0;i<=ClusterSize;i+=2){
     if(Cluster==File->LastCluster){
        sd_buffer[i]=0xff; sd_buffer[i+1]=0xff;
     }else{
        sd_buffer[i]=(u8)(Cluster+1)&0xFF;  //��cluster�g�J�U�@��cluster����}
        sd_buffer[i+1]=(u8)((Cluster+1)>>8&0xFF); //��cluster�g�J�U�@��cluster����}
     }
     if (i>=(512-ClusterOffset))break;
     Cluster++;
  }
  write_to_sdcard(AddrFat1,(512-ClusterOffset),sd_buffer);  //���N�@�}�l����512�������ɻ�
  write_to_sdcard(AddrFat2,(512-ClusterOffset),sd_buffer);  //���N�@�}�l����512�������ɻ�
  ClusterSize-=(512-ClusterOffset);
  AddrFat1+=(512-ClusterOffset);  AddrFat2+=(512-ClusterOffset);
  while(ClusterSize>0){
     u16 size=0;
     for(int i=0;i<512;i+=2){
        sd_buffer[i]=(u8)(Cluster+1)&0xFF;  //��cluster�g�J�U�@��cluster����}
        sd_buffer[i+1]=(u8)((Cluster+1)>>8&0xFF); //��cluster�g�J�U�@��cluster����}
        size+=2;
        if(Cluster==File->LastCluster){
           sd_buffer[i]=0xff; sd_buffer[i+1]=0xff;
           break;
        }
        Cluster++;
     }
     write_to_sdcard(AddrFat1,size,sd_buffer);  //���N�@�}�l����512�������ɻ�
     write_to_sdcard(AddrFat2,size,sd_buffer);  //���N�@�}�l����512�������ɻ�
     AddrFat1+=512;  AddrFat2+=512;
     ClusterSize-=size;
  }
}


























/*
BOOL Ffind(u32 StartAddr,FAT_Struct *FAT,SDFILE *Dfile){ //�M���ɮ�..���G�Ǧ^��}�A�䤣��G�Ǧ^�i�Φ�}
  //u32 NewAddr=0;
  BOOL find=0;
  int i,j;
  //-----------------------------------------���ɮ�-------------------------------------
  for(i=0;i<512;i++){   //�̦h��512���ɮ�
    read_from_sdcard(StartAddr,32,sd_buffer);
    //if(sd_buffer[0]==0xE5){NewAddr=StartAddr;}  //��ܬO�H�R�����ɮסA�p�G�䤣���ɮ׮ɥi��
    for(j=0;j<11;j++){    //�M����w�ɦW�P���ɦW
      if (Dfile->File_Name[j]!=sd_buffer[j]){break;}
      if (j==10&&(Dfile->File_Name[j]==sd_buffer[j])){
        find=1;
      } //����ɮפF
    }
    if(find==1){ break; }  //����ɮפF�A�N���X
    if(sd_buffer[0]==0){break;}  //�p�G�ݨ��1��BYTE�O0�A��ܤw�g�j�M���ثe�Ҧ����ɮפF�A�]��ܥثe����}�O�i�ΪŶ�
    StartAddr+=32;
  }
  //--------------------------------�䧹�ɮסA�^�ǥi�Ϊ��_�l��}----------------------------
  if (find==1){  //����ɮסA�ç��T��i�h
    Dfile->DirTable_Addr=StartAddr;
    GetFileInfo(FAT,Dfile);
  } //������ɮסG���ɮת���}
  else{
    //if(NewAddr==0){NewAddr=StartAddr;}  //�p�G���e�䪺�S��"�w�R���i�ΪŶ�"�A�N�^��"���ϥΪ��Ŷ�"
    Dfile->DirTable_Addr=StartAddr;  //�S���G�H�Ĥ@��00�Τw�R������}���_�l��}
  }
  return find;
}*/

/*
void FindNextCluster(FAT_Struct *FAT,u16 StartCluster,u16 *Next){ //�ǤJ�ثe��CLUSTER..�Ǧ^�U�@��CLUSTER
  u32 StartAddr=FAT16->Fat2_Addr+(u32)(StartCluster*2); //�p��X����CLUSTER�ҦbFAT2������}
  read_from_sdcard(StartAddr,2,sd_buffer);
  *Next=sd_buffer[0]|(u16)(sd_buffer[1]&0xFFFF)<<8;
}
*/
/* �o�������D..�]���O������|����...�ҥH����λ��^��
void RecycleAllCluster(FAT_Struct *FAT,u16 StartCluster){
  FindNextCluster(FAT,StartCluster,&Next);
  sd_buffer[0x00]=0x00; sd_buffer[0x01]=0x00;
  write_to_sdcard((u32)FAT16->Fat2_Addr+(StartCluster*2),2,sd_buffer);
  write_to_sdcard((u32)FAT16->Fat1_Addr+(StartCluster*2),2,sd_buffer);  //�g�J0000...��ܭn�^���ثe���Ŷ��F
  if(Next!=0xFFFF){  //���j�ۤv�A����Next=0xFFFF����
    RecycleAllCluster(FAT,Next);
  }
}
*/
/*
void RecycleAllCluster(FAT_Struct *FAT,u16 StartCluster){  //�o���O�ΰj���..�����ɮצA�j..���M����
  u32 StartAddr;
  do{
    StartAddr=FAT16->Fat2_Addr+(StartCluster*2); //�p��X����CLUSTER�ҦbFAT2������}
    read_from_sdcard(StartAddr,2,sd_buffer);   //Ū���U�@��
    Next=sd_buffer[0]|(u16)(sd_buffer[1]&0xFFFF)<<8; //���}�s��next
    sd_buffer[0x00]=0x00; sd_buffer[0x01]=0x00;
    write_to_sdcard((u32)FAT16->Fat2_Addr+(StartCluster*2),2,sd_buffer);
    write_to_sdcard((u32)FAT16->Fat1_Addr+(StartCluster*2),2,sd_buffer);  //�g�J0000...��ܭn�^���ثe���Ŷ��F
    StartCluster=Next;
  }while (Next!=0xFFFF);
}
*/
/*
void FindLastCluster(FAT_Struct *FAT,SDFILE *Dfile){
  u32 StartAddr;
  do{
    StartAddr=FAT16->Fat2_Addr+(StartCluster*2); //�p��X����CLUSTER�ҦbFAT2������}
    read_from_sdcard(StartAddr,2,sd_buffer);   //Ū���U�@��
    Next=sd_buffer[0]|(u16)(sd_buffer[1]&0xFFFF)<<8; //���}�s��next
    StartCluster=Next;
  }while (Next!=0xFFFF);

   Dfile->LastCluster;   //�ɧ���cluster
   Dfile->Point_Offset=Dfile->File_Size % (FAT16->Byte_Per_Sector*FAT16->Sector_Per_Cluster);
   //�ɮפj�p mod (�C��cluster���j�p)
}
*/
