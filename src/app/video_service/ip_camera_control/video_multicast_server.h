
#ifndef _VIDEO_MULTICAST_SERVR_H
#define _VIDEO_MULTICAST_SERVR_H

#include "../video_multicast_common.h"

#include "subscriber_list.h"
#include "video_source_map_tab.h"
#include "video_source_proxy.h"

#include "./encoder_vin/video_capture_controller.h"
#include "./encoder_vin/obj_adjust.h"

typedef enum
{
	VD_SERVER_IDLE,
	VD_SERVER_ACTIVE,
} vd_server_state;


typedef enum
{
	// response
	VD_SERVER_MSG_RSP_OK,
	VD_SERVER_MSG_RSP_ERR,
	VD_SERVER_MSG_RSP_NO_PERFORMAT,	
	VD_SERVER_MSG_RSP_TIMEOUT,	
	// cancel
	VD_SERVER_MSG_CANCEL_OK,
	VD_SERVER_MSG_CANCEL_ERR,
	VD_SERVER_MSG_CANCEL_NO_RSP,	
	VD_SERVER_MSG_CANCEL_TIMEOUT,		
} vd_server_msg_type;

typedef struct _video_multicast_server_t_
{
	udp_comm_rt 			udp;					//  	udp����ģ��ʵ��
	vd_server_state 		state;					//	״̬��
	vd_server_msg_type		msg;					//	���·�����Ϣ
	send_sem_id_array		waitrsp_array;			//	ҵ��Ӧ��ͬ������
	int 					send_cmd_sn;			//	������������к�
}video_multicast_server;



int init_one_multicast_server( void );


/*******************************************************************************************
 * @fn:		api_one_video_server_cancel_req
 *
 * @brief:	ָ֪ͨ���ͻ����˳���Ƶ�鲥��
 *
 * @param:  	client_ip			- �ͻ���IP��ַ
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
vd_server_msg_type api_one_video_server_cancel_req( int client_ip );

#endif


