

#ifndef __VDP_DEBUG_SBU_H
#define __VDP_DEBUG_SBU_H

#include "../task_survey/task_survey.h"

#define SUB_ID_DEBUG_PH_KEY		        1
#define SUB_ID_DEBUG_LOCAL_VIDEO_O	    2
#define SUB_ID_DEBUG_LOCAL_VIDEO_I		3

#define	SUB_ID_DEBUG_UPD_LINK_REQ		4
#define	SUB_ID_DEBUG_UPD_LINK_RSP		5
#define	SUB_ID_DEBUG_UPD_CALLER			6
#define	SUB_ID_DEBUG_UPD_BECALLED		7

#define SUB_ID_DEBUG_LOG_DOC			8

#pragma pack(1)

typedef struct
{
    VDP_MSG_HEAD    head;
    unsigned char   cmd;
    unsigned char   state;
}vdp_debug_type;

typedef struct
{
    VDP_MSG_HEAD    head;
    unsigned char   key_dat;
    unsigned char   key_state;
}vdp_debug_key_type;

typedef struct
{
    VDP_MSG_HEAD    head;
    unsigned char   onoff;
    unsigned char   state;
}vdp_debug_video_type;

typedef struct
{
    VDP_MSG_HEAD    head;
	int				target_ip;
    unsigned char   result;
    unsigned char   state;
}vdp_debug_link_type;

typedef struct
{
    VDP_MSG_HEAD    head;
	int				target_ip;
    unsigned char   result;
    unsigned char   state;
}vdp_debug_caller_type;

typedef struct
{
    VDP_MSG_HEAD    head;
	int				target_ip;
    unsigned char   result;
    unsigned char   state;
}vdp_debug_becalled_type;

#pragma pack()

// ������Ϣ - ��������
#define KEY1_PRESS      	1
#define KEY1_RELEASE    	2
#define KEY1_3SECOND    	3

#define KEY2_PRESS    		4
#define KEY2_RELEASE  		5
#define KEY2_3SECOND  		6

#define KEY3_PRESS      	7
#define KEY3_RELEASE    	8
#define KEY3_3SECOND    	9

#define KEY4_PRESS    		10
#define KEY4_RELEASE  		11
#define KEY4_3SECOND  		12


#define LOG_TIME_LEN	15
#define LOG_TYPE_LEN	6
#define LOG_DESC_LEN	62 //(MAX)
#define LOG_LEVEL_LEN	1
#define LOG_DETAIL_LEN	100

#define LOG_VIEW_FIXED_LEN	(6+1+LOG_TYPE_LEN+1)

// debug sbu�����ʼ��
void vtk_TaskInit_debug_sbu(void);

// debug sbu���񷴳�ʼ��
void exit_vdp_debug_sbu_task(void);

/****************************************************************************************************************************************
 * @fn      API_add_log_item()
 *
 * @brief   ����һ��log��Ϣ
 *
 * @param   level 		- ���浽�ĵ���view�ĵȼ���0-9������Խ��Ǽ�Խ��
 * @param   pbusiness 	- ҵ�����ͣ��̶�Ϊ6���ַ����ȣ���: ��ʽΪS_XXXX
 * @param   pdes 		- ҵ�������Ϊ64���ֽڣ������пո���'_'��������ո���Ϊexcel�ķָ��� (��Ϊͨ�Ÿ�ʽ�����ʽΪ:[IP];UDP/TCP;T/R)
 * @param   pdetail	- ҵ��ϸ�ڣ�һ����Ϊͨ�����ݰ���ʾ
 *
 * @return  0 - ok, other - err
 ***************************************************************************************************************************************/
int API_add_log_item( int level, char* pbusiness, char* pdes, char* pdetail );

//lzh_20160704_s

#pragma pack(1)

#define DEVICE_TYPE_MAX_LEN		20
#define FW_VERSION_MAX_LEN		20
#define SERVER_FILE_NAME		40

typedef struct
{
	unsigned char	device_type_req;		// �豸��������
	unsigned char	fireware_ver_req;		// �̼��汾��������
} fw_link_request;


typedef struct
{
	unsigned char	device_type_rsp;		// �豸Ӧ������
	unsigned char	fireware_ver_rsp;		// �̼��汾��������
	//Ӧ����������
	unsigned char	device_type[DEVICE_TYPE_MAX_LEN];		// �豸��������
	unsigned char	fireware_ver[DEVICE_TYPE_MAX_LEN];		// �̼��汾��������
}fw_link_response;

typedef struct
{
	int 		server_ip_addr;							// ������ip��ַ
	// lzh_20160715_s
	int			file_type;								// �������ļ�����	0/fireware��1/config files��2/others	
	// lzh_20160715_e
	char 		filename[SERVER_FILE_NAME];				// �������ļ�����
}fw_download_start_request;

typedef struct
{
	int 		result;									// �������
}fw_download_start_response;

typedef struct
{
	int 		file_len;								// download �ļ�����
	int 		checksum;								// download У����
}fw_download_verify_request;

typedef struct
{
	int 		result;									// У����
}fw_download_verify_response;

typedef struct
{
	int 		update;									// ����״̬
}fw_download_update_request;

typedef struct
{
	int 		result;									// У����
}fw_download_update_response;

typedef struct
{
	int		cmd;
	int		len;
	union
	{
		fw_link_request				link_req;
		fw_link_response			link_rsp;
		fw_download_start_request	download_start_req;
		fw_download_start_response	download_start_rsp;
		fw_download_verify_request	download_verify_req;
		fw_download_verify_response	download_verify_rsp;
		fw_download_update_request	download_update_req;
		fw_download_update_response	download_update_rsp;
	}dat;
}common_send_pack;

#pragma pack()

//czn_20160827_s
#define MAX_UPDATE_CONFIGFILE		60		
typedef struct
{
	int			server_ip_addr;
	int			configfile_cnt;
	struct
	{
		int 		rid;
		char		filename[SERVER_FILE_NAME];
	}resource_record[MAX_UPDATE_CONFIGFILE];
}Updater_Run_Stru;
//czn_20160827_e

int api_fireware_upgrade_cmd_send( int target_ip, int cmd, char* pdat  );
int api_fireware_upgrade_apply( int target_ip, fw_link_request* presult  );

//lzh_20160704_e




#endif

