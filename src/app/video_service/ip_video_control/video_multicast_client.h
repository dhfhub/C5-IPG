

#ifndef _VIDEO_MULTICAST_CLIENT_H
#define _VIDEO_MULTICAST_CLIENT_H

#include "../video_multicast_common.h"

#include "./decoder/video_displaye_controller.h"

typedef enum
{
	VD_CLIENT_IDLE,				// �ͻ��˿���״̬
	VD_CLIENT_SUBSCRIBE,		// �ͻ�������״̬
	VD_CLIENT_ACTIVE,			// �ͻ��˽����鲥״̬
} vd_client_state;

typedef enum
{
	//request
	VD_CLIENT_MSG_REQ_NONE,
	VD_CLIENT_MSG_REQ_BUSY,
	VD_CLIENT_MSG_REQ_NO_RSP,
	VD_CLIENT_MSG_REQ_OK,
	VD_CLIENT_MSG_REQ_UNALLOW,
	VD_CLIENT_MSG_REQ_TIMEOUT,
	VD_CLIENT_MSG_REQ_TO_PROXY,
	//close
	VD_CLIENT_MSG_CLOSE_NONE,
	VD_CLIENT_MSG_CLOSE_ERR,
	VD_CLIENT_MSG_CLOSE_NO_RSP,
	VD_CLIENT_MSG_CLOSE_OK,
	VD_CLIENT_MSG_CLOSE_TIMEOUT,
} vd_client_msg_type;

typedef struct _video_multicast_client_t_
{
	udp_comm_rt 			udp;				//	udp����ģ��ʵ��
	vd_client_state 		state;				//	״̬��
	vd_client_msg_type		msg;				//	���·�����Ϣ
	int 					send_cmd_sn;		//	������������к�
	send_sem_id_array		waitrsp_array;		// ҵ��Ӧ��ͬ������	
	// registor data
	int						vd_multicast_time;	// ��Ƶ�鲥��ʱ��(s)
	unsigned short			vd_multicast_port;	// ��Ƶ�鲥���Ͷ˿ں�
	int						vd_multicast_ip;	// ��Ƶ�鲥���͵�ַ	
	int						vd_ask_server_ip;	// ��Ƶ�鲥�����������ַ	
	int						vd_proxy_server_ip;	// ��Ƶ�鲥�����������ַ	
}video_multicast_client;


int init_one_multicast_client( void );

/*******************************************************************************************
 * @fn:		api_one_video_client_subscribe_req
 *
 * @brief:	��������Ӧip����Ƶ�鲥����
 *
 * @param:  	server_ip		- ������IP��ַ
 * @param:  	dev_id		- ���������豸id 
 * @param:  	second	 	- ��Ƶ����ʱ��
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
vd_client_msg_type api_one_video_client_subscribe_req( int server_ip, int dev_id, int second );

/*******************************************************************************************
 * @fn:		api_one_video_client_desubscribe_req
 *
 * @brief:	���������Ƶ�������Ƶ��
 *
 * @param:  	server_ip		- ������IP��ַ
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
vd_client_msg_type api_one_video_client_desubscribe_req( int server_ip );

/*******************************************************************************************
 * @fn:		api_cur_video_server_proxy_ip
 *
 * @brief:	�õ���ǰ����Ĵ��������IP - ����api_one_video_client_subscribe_req�Ľ��ΪVD_CLIENT_MSG_REQ_TO_PROXYʱ��Ч
 *
 * @param:	none
 *
 * @return: 	0 - none�� other - proxy ip
*******************************************************************************************/
int	api_cur_video_server_proxy_ip(void);

#endif


