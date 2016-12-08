
#ifndef _VIDEO_LINPHONE_LINKER_H
#define _VIDEO_LINPHONE_LINKER_H

#include "../vtk_udp_stack/vtk_udp_stack_class.h"

// linphone call���ն˿ں�
#define LINPHONE_STARTER_RECV_PORT		28002

typedef enum
{
	VD_LINPHONE_IDLE,
	VD_LINPHONE_CLIENT,
	VD_LINPHONE_SERVER,
} vd_linphone_state;

typedef enum
{
	//request
	VD_LINPHONE_MSG_REQ_NONE,
	VD_LINPHONE_MSG_REQ_BUSY,
	VD_LINPHONE_MSG_REQ_NO_RSP,
	VD_LINPHONE_MSG_REQ_OK,
	VD_LINPHONE_MSG_REQ_UNALLOW,
	VD_LINPHONE_MSG_REQ_TIMEOUT,
	//close
	VD_LINPHONE_MSG_CLOSE_NONE,
	VD_LINPHONE_MSG_CLOSE_ERR,
	VD_LINPHONE_MSG_CLOSE_NO_RSP,
	VD_LINPHONE_MSG_CLOSE_OK,
	VD_LINPHONE_MSG_CLOSE_TIMEOUT,
} vd_linphone_msg_type;

typedef struct _video_linphone_linker_t_
{
	udp_comm_rt 			udp;				//	udp����ģ��ʵ��
	vd_linphone_state 		state;				//	״̬��
	vd_linphone_msg_type	msg;				//	���·�����Ϣ
	int 					send_cmd_sn;		//	������������к�
	send_sem_id_array		waitrsp_array;		// 	ҵ��Ӧ��ͬ������
}video_linphone_linker;

// ���н��������
typedef enum
{
	LINPHONEC_ACK,					// 0 just communication ack, must be reserved
	// linphone link req couple
	LINPHONE_LINK_REQ,				//  client 	-> server
	LINPHONE_LINK_RSP,				//  server 	-> client
	
	// linphone quit req couple
	LINPHONE_QUIT_REQ,				//	server	-> client	
	LINPHONE_QUIT_RSP,				//	client	-> server
	
} vd_linphone_cmd;

// linphone��������response�ظ���result����
typedef enum
{
	VD_LINPHONE_RSP_RESULT_NONE,		// ��Ч
	VD_LINPHONE_RSP_RESULT_ENABLE,		// ����
	VD_LINPHONE_RSP_RESULT_DISABLE,		// ������
} vd_linphone_rsp_result;

// �ͻ�����������ݰ���ʽ
typedef struct
{
	target_head				target;
	int						vd_direct;	// ��Ƶ�����������
	int						auto_talk;	// �����Զ�Ӧ��
} vd_linphonec_linker_req_pack;

// ����������Ӧ�ͻ��˵��������ݰ���ʽ
typedef struct
{
	target_head				target;
	int						result;		// ����Ļظ����
} vd_linphonec_linker_rsp_pack;


int init_one_linphone_linker( void );

vd_linphone_msg_type api_one_linphone_linker_req( int server_ip, int vd_dir, int auto_talk );
vd_linphone_msg_type api_one_linphone_linker_rsp( int client_ip, vd_linphonec_linker_req_pack* plinkreq, vd_linphone_rsp_result result);

vd_linphone_msg_type api_one_linphone_unlink_req( int target_ip, int vd_dir);
vd_linphone_msg_type api_one_linphone_unlink_rsp( int target_ip, vd_linphonec_linker_req_pack* punlinkreq, vd_linphone_rsp_result result);

#endif

