
#ifndef _VDP_CONTROLLER_H
#define _VDP_CONTROLLER_H

#include "../utility.h"
#include "../unix_socket.h"
#include "../vtk_udp_stack/vtk_udp_stack_class.h"
#include "./obj_CallServer_Virtual/obj_IP_Call_Link.h"				//czn_20160427

#define		VDP_DAT_BUF_LEN					240
#define 	VDP_THREAD_CYCLE_TIME			600
#define 	VDP_QUEUE_POLLING_TIME			0  //1000		// ����polling�ĵȴ�ʱ��

//��ϢĿ������壺
#define MSG_ID_ACK				1

#define MSG_ID_UART				2

#define MSG_ID_survey			10		// ��Ϣ�ַ���������
#define MSG_ID_udp_stack		13		// udp stack

#define MSG_ID_NetManage		100		// ������������Ϣ
#define MSG_ID_IOServer			101		// IO���������Ϣ
#define MSG_ID_GOServer			102		// GO���������Ϣ
#define MSG_ID_DeviceManage		105		// �豸���������Ϣ
#define MSG_ID_CallSurvey		106		// ����ҵ����������Ϣ
#define MSG_ID_NamelistManage 	108

#define MSG_ID_DEBUG_SBU		110		// ���Է��������Ϣ

//czn_20160526_s
#define MSG_ID_CallServer		111		// 
#define MSG_ID_IpCalller			112		// 
#define MSG_ID_IpBeCalled		113
//czn_20160526_e

//czn_20160217_s
#define MSG_ID_CALL_LINK		114
#define MSG_ID_MON_LINK			115
#define MSG_ID_UNIT_LINK		116
#define MSG_ID_Phone_Rep		211
#define MSG_ID_CAMERA_CTRL		116
//czn_20160217_e

#define CALL_SURVEY_IDLE			0
#define CALL_SURVEY_ASCALLER		1
#define CALL_SURVEY_ASBECALLED		2
//lzh_20160503_s
#define CALL_SURVEY_MONITOR			3
//lzh_20160503_s


extern unsigned char Call_Survey_State;
//czn_20160418_e

typedef struct
{
	// ��Ϣ����ͷ
	unsigned char 	msg_target_id;		// ��Ϣ��Ŀ�����
	unsigned char 	msg_source_id;		// ��Ϣ��Դ����
	unsigned char 	msg_type;			// ��Ϣ������
	unsigned char  	msg_sub_type;		 //��Ϣ��������
} VDP_MSG_HEAD;

typedef struct 
{
	VDP_MSG_HEAD	head;
	int				target_ip;
	int 			cmd;
	int 			id;	
	int 			len;
	char 			pbuf[VDP_DAT_BUF_LEN];
} UDP_MSG_TYPE;

typedef struct 
{
	VDP_MSG_HEAD	head;
	int 			len;
	char 			pbuf[VDP_DAT_BUF_LEN];
} UART_MSG_TYPE;

void vtk_TaskInit_survey( void );

extern vdp_task_t	task_control;
extern vdp_task_t	task_debug_sbu;
extern vdp_task_t	task_io_server;
extern vdp_task_t	task_net_manange;
// czn_20160526_s
extern vdp_task_t	task_caller;
extern vdp_task_t	task_becalled;
// czn_20160526_e

vdp_task_t* GetTaskAccordingMsgID(unsigned char msg_id);
int GetMsgIDAccordingPid( pthread_t pid );

// ����: �ȴ�ҵ��ͬ��Ӧ��
// ����: pBusiness - ͬ���ȴ�ʱ�Ĺ���Ķ��У�business_type - �ȴ���ҵ�����ͣ� data - �õ������ݣ�plen - ������Ч����
// ����: 0 - ���س�ʱ��1 -  ��������
int WaitForBusinessACK( p_Loop_vdp_common_buffer pBusiness, unsigned char business_type,  char* data, int* plen, int timeout );


/****************************************************************************************************************************
 * @fn:		API_add_message_to_suvey_queue
 *
 * @brief:	������Ϣ���ַ��������
 *
 * @param:  pdata 			- ����ָ��
 * @param:  len 			- ���ݳ���
 *
 * @return: 0/ok, 1/full
****************************************************************************************************************************/
int	API_add_message_to_suvey_queue( char* pdata, unsigned int len );


int API_one_message_to_suvey_queue( int msg, int sub_msg );

#define VIDEO_SEVICE_NOTIFY_REMOTE_ON()		API_one_message_to_suvey_queue(SURVEY_MSG_VIDEO_SERVICE_ON,0)
#define VIDEO_SEVICE_NOTIFY_REMOTE_OFF()	API_one_message_to_suvey_queue(SURVEY_MSG_VIDEO_SERVICE_OFF,0)

#endif

