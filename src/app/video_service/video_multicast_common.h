
#ifndef _VIDEO_MULTICAST_COMMON_H
#define _VIDEO_MULTICAST_COMMON_H

#include "video_object.h"
#include "../vtk_udp_stack/vtk_udp_stack_class.h"
#include "../vtk_udp_stack/vtk_udp_stack_c5_ipc_cmd.h"

#include "../task_survey/task_survey.h"
#include "../task_survey/sys_msg_process.h"
#include "../task_debug_sbu/task_debug_sbu.h"

#define	VDP_PRINTF_VIDEO

#ifdef	VDP_PRINTF_VIDEO
#define	vd_printf(fmt,...)	printf("[V]-["__FILE__"]-[%04d]-"fmt"",__LINE__,##__VA_ARGS__)
#else
#define	vd_printf(fmt,...)
#endif

// C/S���������
typedef enum
{
	// communication ack
	COM_ACK=0,				//  client 	<-> server
	
	// subscribe couple
	SUBSCRIBE_REQ=0x0001,			//  client 	-> server
	SUBSCRIBE_RSP=0x0081,			//  server 	-> client
	
	// desubscribe couple
	DESUBSCRIBE_REQ=0x0002,		//  client 	-> server
	DESUBSCRIBE_RSP=0x0082,		//  server 	-> client
	
	// cancel couple
	CANCEL_REQ=0x0003,				//	server	-> client	
	CANCEL_RSP=0x0083,				//	client	-> server
} vd_subscribe_cmd;

// ��������response�ظ���result����
typedef enum
{
	VD_RESPONSE_RESULT_NONE,		// ��Ч
	VD_RESPONSE_RESULT_ENABLE,		// ����
	VD_RESPONSE_RESULT_DISABLE,		// ������
	//ת�����������
	VD_RESPONSE_RESULT_TO_PROXY,	// ��������
} vd_response_result;

// ��Ƶ��Դ״̬��������
typedef enum
{
	VD_NOTIFY_NORMAL,			// ��Ƶ�ź�����
	VD_NOTIFY_NO_SIGNAL,		// ����Ƶ�ź�	
	VD_NOTIFY_SIGNAL_RESET,		// ��Ƶ�ź�����
} vd_notify_State;

// subscribe couple

// �ͻ�����������ݰ���ʽ
typedef struct
{
	target_head			target;
	vd_response_result	result;				// ����Ļظ����
	int					vd_multicast_time;	// ��Ƶ�鲥��ʱ��(s)
	int					dev_id;				// ��������ͷ�豸id
} vd_subscribe_req_pack;

// ����������Ӧ�ͻ��˵��������ݰ���ʽ
typedef struct
{
	target_head			target;
	vd_response_result	result;				// ����Ļظ����
	int					vd_multicast_time;	// ��Ƶ�鲥��ʱ��(s)
	short				vd_multicast_port;	// ��Ƶ�鲥�ض˿ں�
	int					vd_multicast_ip;	// ��Ƶ�鲥��ַ
} vd_subscribe_rsp_pack;

// desubscribe couple
typedef struct _vd_desubscribe_req_pack_t
{
	target_head			target;
	vd_response_result	result;
} vd_desubscribe_req_pack;

typedef struct _vd_desubscribe_rsp_pack_t
{
	target_head			target;
	vd_response_result	result;
} vd_desubscribe_rsp_pack;

// cancel couple
typedef struct _vd_cancel_req_pack_t
{
	target_head			target;
	vd_response_result	result;				// ����Ļظ����
} vd_cancel_req_pack;

typedef struct _vd_cancel_rsq_pack_t
{
	target_head			target;
	vd_response_result	result;				// ����Ļظ����
} vd_cancel_rsq_pack;


/////////////////////////////////////////////////////
#include "./ip_camera_control/encoder_vin/obj_adjust.h"

typedef enum
{
    ADJ_GET,
    ADJ_SET,
    ADJ_DEC,
    ADJ_INC,
}Dir_ype_t;

#if 0

typedef enum
{
    ADJ_CONTRAST,
    ADJ_BRIGHT,
    ADJ_COLOR,
    ADJ_ALL,
}AdjustType_t;

typedef struct
{
	unsigned char   	logContrastCnt;
	unsigned char 	logBrightCnt;
	unsigned char 	logColorCnt;
}ImagePara_t;
#endif

typedef struct
{
	Dir_ype_t		dir;
	AdjustType_t	type;
    ImagePara_s		data;
	int				state;  // lzh_20160530 state: 0/ok, 1/server close
} UDP_Image_t;

#endif


