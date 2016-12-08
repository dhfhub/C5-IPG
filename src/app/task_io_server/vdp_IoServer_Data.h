/**
  ******************************************************************************
  * @file    obj_IoServer_Data.h
  * @author  zxj
  * @version V00.01.00
  * @date    2012.11.01
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  * <h2><center>&copy; COPYRIGHT 2012 V-Tec</center></h2>
  ******************************************************************************
  */ 

#ifndef _obj_IoServer_Data_H
#define _obj_IoServer_Data_H

#define INIT_DISABLE				0x00
#define INSTALLER_INIT_ENABLE		0x01
#define FACTORY_INIT_ENABLE		0x02
#define USER_INIT_ENABLE			0x04


// Define Object Property-------------------------------------------------------
#define KNX_000	0
#define KNX_001	1
#define KNX_002	2
#define KNX_003	3
#define KNX_004	4
#define KNX_005	5
#define KNX_006	6
#define KNX_007	7
#define KNX_008	8
#define KNX_009	9
#define KNX_010	10
#define KNX_011	11
#define KNX_012	12
#define KNX_013	13
#define KNX_014	14
#define KNX_015	15
#define KNX_016	16
#define KNX_017	17
#define KNX_018	18
#define KNX_019	19
#define KNX_020	20
#define KNX_021	21
#define KNX_022	22
#define KNX_023	23



//KNX�������Ͷ���
typedef unsigned char	KNX_000_B1_BOOL;	//0 Boolean
typedef unsigned char	KNX_001_B1_BIT0;	//1 1-Bit Controled
typedef unsigned char	KNX_002_B1_BIT012;	//2 3-Bit Controled
typedef unsigned char	KNX_003_B1_CHARSET;	//3 Char Set
typedef unsigned char	KNX_004_B1_UCHAR;	//4 8-Bit Unsigned
typedef signed char		KNX_005_B1_CHAR;	//5 8-Bit Signed

typedef unsigned short	KNX_006_B2_USHORT;	//6 Unsigned Value
typedef signed short	KNX_007_B2_SHORT;	//7 Signed Value
typedef unsigned short	KNX_008_B2_FLOAT;	//8 Floated Value
typedef unsigned short	KNX_009_B2_VERSION;	//9 Version

typedef struct _KNX_010_B3_TIME_			//10 Time
{
	unsigned char TIME[3];
} KNX_010_B3_TIME;

typedef struct _KNX_011_B3_DATE_			//11 Date
{
	unsigned char DATE[3];
} KNX_011_B3_DATE;

typedef unsigned long	KNX_012_B4_UINT;	//12 Unsigned Value
typedef long			KNX_013_B4_INT;		//13 Signed Value
typedef long			KNX_014_B4_FLOAT;	//14 Float Value
typedef long			KNX_015_B4_ACCESS;	//15 Access_6-digits_option

typedef struct _KNX_016_B8_DATETIME_		//16 DateTime
{
	unsigned char actual_byte;	//�洢�ռ�(������byte)
	unsigned char date_time[8];
} _KNX_016_B8_DATETIME_;
typedef struct _KNX_017_B8_STR8_			//17 ShortString
{
	unsigned char actual_byte;	//�洢�ռ�(������byte)
	unsigned char STR8[8];
} KNX_017_B8_STR8;

typedef struct _KNX_018_B14_STR14_			//18 String
{
	unsigned char actual_byte;	//�洢�ռ�(������byte)
	unsigned char STR14[14];
} KNX_018_B14_STR14;

typedef struct _KNX_019_B24_NAMELIST_		//19 data
{
	unsigned char actual_byte;		//�洢�ռ�(������byte)
	
	unsigned char valid_data;		//�ַ���
	unsigned char flag_valid;		//��Ч��־: 0=��Ч, 0x20=��Ч
	unsigned char dip_address;		//DIP���õĵ�ַ: 0~31
	unsigned char data[21];			//ʵ�����洢20byte�ַ�,��1byte����
} KNX_019_B24_NAMELIST;

typedef struct _KNX_020_B19_STR19_
{
	unsigned char actual_byte;	//�洢�ռ�(������byte)
	unsigned char STR19[19];
} KNX_020_B19_STR19;

typedef struct _KNX_021_B21_STR21_
{
	unsigned char actual_byte;	//�洢�ռ�(������byte)
	unsigned char STR21[21];	
} KNX_021_B21_STR21;

typedef struct _KNX_022_B5_DATA_
{
	unsigned char actual_byte;	//�洢�ռ�(������byte)
	unsigned char DATA[5];
} KNX_022_B5_DATA;

typedef struct _KNX_023_B220_STR220_
{
	unsigned char actual_byte;	//�洢�ռ�(������byte)
	unsigned char STR220[220];
} KNX_023_B220_STR220;


/*
 *�ļ�����˵����
 *  ���ļ�Ϊ����������ɵĲ������ͷ�ļ�
 *
 *�ļ��ֶν�����
 *	 ADDR_DPT:	
 *  DESCRIPTION:Ϊ���������������
 *  BaseAddress:Ϊ��������ʼ��ַ
 *  XXX_xxxxxxx:Ϊ�����Ĺ��ܱ��,��3�����ܣ�
 *    1.�õ�������ǰֵ����ΪEEPROM_ADDR���е��������õ�EEPROM��ַ���Ӷ���д�ò����ĵ�ǰֵ
 *    2.�õ�����������ֵ����ΪEEPROM_ADDR���е��������õ��ò����Ƿ��������Ȼ��������Description_TAB�е�����
 *    3.�õ����������ֵ����Сֵ��ȱʡֵ��������Сֵ��Բ�ͬ�����͹̶���
 */

//��չ���Ͷ���
typedef struct _INFO_
{
	unsigned int address;
	unsigned char level;
	unsigned char dpt;               //data length
	unsigned char init_type;
} INFO;

typedef struct _DESCRIPTION_
{
	unsigned short id;				
	unsigned char actual_byte;		
	unsigned char STR8[8];
} DESCRIPTION;



#define BaseAddress 		0x0808000	

#define DeviceAdd				0						//�豸��ַ						//������ʼ��
#define DeviceName				1						//�豸����						//code�� �����ʼ��						
#define DeviceModel				2						//�豸�ͺ�						//code�� �����ʼ��							
#define Description				3						//�豸����						//code�� �����ʼ��						
#define FWRevision				4						//Ӳ���汾						//code�� �����ʼ��						
#define SWRevision				5						//����汾						//code�� �����ʼ��						
#define ProtocolVersion			6						//Э��汾						//code�� �����ʼ��							
#define SerialNumber			7						//����Ψһ���к�				//������ʼ��
#define MFgDate					8						//��������						//������ʼ��
#define ControlCode				9						//��λ������					//������ʼ��
#define DeviceTypeID			10						//�豸����						//code�� �����ʼ��						
#define DisplayName				11						//����������					//������ʼ��
#define DisplayMode				12						//�������ͺ�					//������ʼ��
#define Installation			13						//��������						//������ʼ��
#define Location				14						//��װλ��						//������ʼ��
#define UpdateDate				15						//�������ʱ��					//������ʼ��
#define AccessPassword			16						//��������						//������ʼ��
#define EventUpload				17						//�¼��ϱ�						//������ʼ��
#define WaitVoltage				18						//������ѹ						//������ʼ��
#define WorkVoltage				19						//������ѹ						//������ʼ��

#define IO_CameraBright			20
#define IO_CameraColor			21
#define IO_CameraContrast		22

#define PROPERTY_NUMBS 			(IO_CameraContrast + 1)


// Define Object Function - Public----------------------------------------------
extern const INFO EEPROM_ADDR[];
extern const unsigned char* const ptrRomTab[];


// Define Object Function - Private---------------------------------------------
void IoServer_Data_Init(void);	


// Define Object Function - other-----------------------------------------------


#endif
