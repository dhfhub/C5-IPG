
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>      /* basic system data types */
#include <error.h>

#include "video_multicast_server.h"

video_multicast_server	one_multicast_server;

int 					server_inner_msg_process_pid; 				// �ڲ���Ϣ����pid
void* 					server_inner_msg_process( void* arg );

int server_recv_busines_anaylasis( char* buf, int len );

int init_one_multicast_server( void )
{
	// init state
	one_multicast_server.state = VD_SERVER_IDLE;
	// init udp
	init_one_udp_comm_rt_buff( &one_multicast_server.udp, 500,server_recv_busines_anaylasis, 500, NULL );
	init_one_udp_comm_rt_type( &one_multicast_server.udp, "multicast server", UDP_RT_TYPE_UNICAST, VIDEO_SERVER_CMD_RECV_PORT, VIDEO_CLIENT_CMD_RECV_PORT, NULL );
	// init business rsp wait array	
	init_one_send_array(&one_multicast_server.waitrsp_array);
	if( pthread_create(&server_inner_msg_process_pid, 0, server_inner_msg_process, 0) )
	{
		return -1;
	}	
	one_multicast_server.send_cmd_sn = 0;

	init_subscriber_list();

	vd_printf("init_one_multicast_server ok!\n");

	return 0;
}

#define SERVER_POLLING_MS		100
void* server_inner_msg_process(void* arg )
{
	// 100ms��ʱ��ѯ
	while( 1 )
	{
		usleep(SERVER_POLLING_MS*1000);
		poll_all_business_recv_array( &one_multicast_server.waitrsp_array, SERVER_POLLING_MS );
	}
	return 0;	
}


vd_server_msg_type api_one_video_server_subscribe_rsp( int client_ip,vd_subscribe_req_pack *pclient_request, vd_response_result result, int mcg_addr, short port);
vd_server_msg_type api_one_video_server_desubscribe_rsp( int client_ip,vd_subscribe_req_pack *pclient_request, vd_response_result result);

/*******************************************************************************************
 * @fn:		api_enable_video_server_multicast
 *
 * @brief:	������Ƶ�������Ƶ�鲥���
 *
 * @param:  	mcg_addr	 	- �鲥��ַ�ĵ��ֽ� (���ֽڹ̶�Ϊ224.0.2.)
 * @param:  	port 		- �鲥����Ķ˿ں�
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_enable_video_server_multicast( int mcg_addr, short port )
{
	API_ToMulticastJoin( port, mcg_addr );
	usleep(1000*1000);

	one_multicast_server.state = VD_SERVER_ACTIVE;
	
	return 0;
}

/*******************************************************************************************
 * @fn:		api_disable_video_server_multicast
 *
 * @brief:	ֹͣ��Ƶ�������Ƶ�鲥���
 *
 * @param:  	mcg_addr	 	- �鲥��ַ�ĵ��ֽ� (���ֽڹ̶�Ϊ224.0.2.)
 * @param:  	port 		- �鲥����Ķ˿ں�
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_disable_video_server_multicast( int mcg_addr, short port )
{
	API_ToMulticastLeave( port, mcg_addr ); 
	usleep(1000*1000);

	one_multicast_server.state = VD_SERVER_IDLE;

	// ����ͨ��
	api_camera_server_multicast_notify_off();				

	return 0;
}

// �������˽�������ҵ�����
int server_recv_busines_anaylasis( char* buf, int len )
{	
	int				mcg_addr;
	short			port;
	int				user_cnt;
	int				get_server_ip_result;

	// lzh_20160811_s
	// ��ͷ��Ҫȥ��
	baony_head* pbaony_head = (baony_head*)buf;
	if( pbaony_head->type != IP8210_CMD_TYPE )
		return -1;
	len -= sizeof(baony_head);
	buf += sizeof(baony_head);
	// lzh_20160811_e

	// �������˽�������ҵ�����, ��Ҫ���˵�target ip ��4���ֽ�
	vd_subscribe_req_pack* 	prcvbuf = (vd_subscribe_req_pack*)(buf);
	vd_subscribe_rsp_pack*	prspbuf	= (vd_subscribe_rsp_pack*)(buf);

	vd_printf("get_one_client_request,ip=0x%08x,cmd=%d\n",prcvbuf->target.ip,prcvbuf->target.cmd);

	switch( prcvbuf->target.cmd )
	{
		case SUBSCRIBE_REQ:
			// ��������
			if( api_camera_server_multicast_apply_on(prcvbuf->target.ip,prcvbuf->dev_id) < 0 )
			{
				api_one_video_server_subscribe_rsp( prcvbuf->target.ip, prcvbuf, VD_RESPONSE_RESULT_DISABLE, 0, 0 );
				break;
			}
			// �ж��Ƿ��д�������� - ���ⲿ��mon_link��ȡ���������������
			if( 1 ) // !is_video_server_proxy_enable() )
			{
				// �õ��鲥��ַ
				get_server_multicast_addr_port( &mcg_addr, &port );
				// �ظ��鲥���� - enable
				if( api_one_video_server_subscribe_rsp( prcvbuf->target.ip, prcvbuf, VD_RESPONSE_RESULT_ENABLE, mcg_addr, port ) == VD_SERVER_MSG_RSP_OK )
				{
					// ���ÿ���ָ���ͻ�����Ƶ�鲥�������
					api_enable_video_server_multicast( mcg_addr, port );
					// �Ǽǵ�user list
					vd_printf("activate_subscriber_list,ip=0x%08x,time=%d\n",prcvbuf->target.ip,prcvbuf->vd_multicast_time);
					user_cnt = activate_subscriber_list(prcvbuf->target.ip,prcvbuf->vd_multicast_time); 			
					vd_printf("activate_subscriber_list,user total = %d\n",user_cnt);
				}
			}
			else
			{	
				// �ظ��鲥���� - ���ӵ�����
				if( get_video_server_proxy_type() == PROXY_MULTICAST )
				{
					mcg_addr = get_video_server_proxy_ip();
					vd_printf("server proxy ip = 0x%08x\n",mcg_addr );				
					api_one_video_server_subscribe_rsp( prcvbuf->target.ip, prcvbuf, VD_RESPONSE_RESULT_TO_PROXY, mcg_addr, 0 );
				}
				// �ظ��鲥���� - ��Ч
				else
				{
					api_one_video_server_subscribe_rsp( prcvbuf->target.ip, prcvbuf, VD_RESPONSE_RESULT_DISABLE, 0, 0 );	
				}
			}
			break;
		case DESUBSCRIBE_REQ:
			api_one_video_server_desubscribe_rsp( prcvbuf->target.ip, prcvbuf, VD_RESPONSE_RESULT_ENABLE );
			// ע����user list
			vd_printf("deactivate_subscriber_list,ip=0x%08x\n",prcvbuf->target.ip);			
			deactivate_subscriber_list(prcvbuf->target.ip);
			if( get_total_subscriber_list() == 0 )
			{				
				if( get_server_multicast_addr_port( &mcg_addr, &port ) == 0 )
				{
					// ���ùر�ָ���ͻ�����Ƶ�鲥�������				
					api_disable_video_server_multicast( mcg_addr, port );
				}
			}			
			break;

		case CANCEL_RSP:
			// ����ҵ��Ӧ��
			if( trig_one_send_array( &one_multicast_server.waitrsp_array, prspbuf->target.id, prspbuf->target.cmd ) < 0 )
				vd_printf("recv one vd_rsp none-match command,id=%d\n",	prspbuf->target.id);		
			else
				vd_printf("recv one vd_rsp match command ok,id=%d\n",prspbuf->target.id);
			deactivate_subscriber_list(prspbuf->target.ip);
			if( get_total_subscriber_list() == 0 )
			{				
				if( get_server_multicast_addr_port( &mcg_addr, &port ) == 0 )
				{			
					// ���ùر�ָ���ͻ�����Ƶ�鲥�������				
					api_disable_video_server_multicast( mcg_addr, port );
				}
			}			
			break;
		default:
			break;
	}
	return 0;
}

/*******************************************************************************************
 * @fn:		api_one_video_server_subscribe_rsp
 *
 * @brief:	��Ӧ������Ƶ�������Ƶ��
 *
 * @param:  	client_ip			- �ͻ���IP��ַ
 * @param:  	pclient_request 	- ��Ӧ��Ƶ�鲥����
 * @param:  	vd_response_result	- ��Ӧ���
 * @param:	mcg_addr  		- �鲥��ַ
 * @param:	port  			- �鲥�˿ں�
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
vd_server_msg_type api_one_video_server_subscribe_rsp( int client_ip,vd_subscribe_req_pack *pclient_request, vd_response_result result, int mcg_addr, short port)
{	
	vd_subscribe_rsp_pack	server_response; 

	// �������
	server_response.target.ip			= client_ip;
	server_response.target.cmd			= SUBSCRIBE_RSP;
	server_response.target.id			= pclient_request->target.id;
	server_response.result				= result;
	server_response.vd_multicast_time	= pclient_request->vd_multicast_time;
	server_response.vd_multicast_port	= port;	
	server_response.vd_multicast_ip		= mcg_addr;

	// ��������
	sem_t *presend_sem = one_udp_comm_trs_api( &one_multicast_server.udp, server_response.target.ip, server_response.target.cmd, server_response.target.id,
				1, (char*)&server_response + sizeof(target_head), sizeof(vd_subscribe_rsp_pack)-sizeof(target_head));
	
	vd_printf("api_one_video_server_subscribe_rsp...\n");
	
	// �ȴ�������ͨ��Ӧ��
	if( sem_wait_ex2(presend_sem, 5000) != 0 )
	{
		vd_printf("api_one_video_server_subscribe_rsp,wait ack --- timeout ---\n");		
		one_multicast_server.msg	= VD_SERVER_MSG_RSP_TIMEOUT;	
		return one_multicast_server.msg;
	}
	one_multicast_server.msg	= VD_SERVER_MSG_RSP_OK;	
	vd_printf("api_one_video_server_subscribe_rsp ok!\n");
	return one_multicast_server.msg;
}

/*******************************************************************************************
 * @fn:		api_one_video_server_desubscribe_rsp
 *
 * @brief:	��Ӧ�˳���Ƶ�������Ƶ��
 *
 * @param:  	client_ip			- �ͻ���IP��ַ
 * @param:  	pclient_request 	- ��Ӧ��Ƶ�鲥����
 * @param:  	vd_response_result	- ��Ӧ���
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
vd_server_msg_type api_one_video_server_desubscribe_rsp( int client_ip,vd_subscribe_req_pack *pclient_request, vd_response_result result)
{	
	vd_desubscribe_rsp_pack	server_response; 

	// �������
	server_response.target.ip			= client_ip;
	server_response.target.cmd			= DESUBSCRIBE_RSP;
	server_response.target.id			= pclient_request->target.id;
	server_response.result				= result;

	// ��������
	sem_t *presend_sem = one_udp_comm_trs_api( &one_multicast_server.udp, server_response.target.ip, server_response.target.cmd, server_response.target.id, 
						1, (char*)&server_response + sizeof(target_head), sizeof(vd_desubscribe_rsp_pack)-sizeof(target_head));
	
	vd_printf("api_one_video_server_desubscribe_rsp...\n");
	
	// �ȴ�������ͨ��Ӧ��
	if( sem_wait_ex2(presend_sem, 5000) != 0 )
	{
		vd_printf("api_one_video_server_desubscribe_rsp,wait ack --- timeout ---\n");		
		one_multicast_server.msg	= VD_SERVER_MSG_RSP_TIMEOUT;	
		return one_multicast_server.msg;
	}
	one_multicast_server.msg	= VD_SERVER_MSG_RSP_OK;	
	vd_printf("api_one_video_server_desubscribe_rsp ok!\n");
	return one_multicast_server.msg;
}

/*******************************************************************************************
 * @fn:		api_one_video_server_cancel_req
 *
 * @brief:	ָ֪ͨ���ͻ����˳���Ƶ�鲥��
 *
 * @param:  	client_ip			- �ͻ���IP��ַ
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
vd_server_msg_type api_one_video_server_cancel_req( int client_ip )
{	
	vd_cancel_req_pack	server_cancel; 

	// �������
	server_cancel.target.ip				= client_ip;
	server_cancel.target.cmd			= CANCEL_REQ;
	server_cancel.target.id				= ++one_multicast_server.send_cmd_sn;
	server_cancel.result				= VD_RESPONSE_RESULT_NONE;

	// ����ȴ�ҵ��Ӧ�����
	//sem_t *pwaitrsp_sem = join_one_send_array(&one_multicast_server.waitrsp_array,server_cancel.target.id, server_cancel.target.cmd+1,BUSINESS_RESEND_TIME, 1, NULL, 0 );
	sem_t *pwaitrsp_sem = join_one_send_array(&one_multicast_server.waitrsp_array,server_cancel.target.id, server_cancel.target.cmd+0x80,BUSINESS_RESEND_TIME, 1, NULL, 0 );

	// ��������
	sem_t *presend_sem = one_udp_comm_trs_api( &one_multicast_server.udp, server_cancel.target.ip, server_cancel.target.cmd, server_cancel.target.id,
					1, (char*)&server_cancel + sizeof(target_head), sizeof(vd_cancel_req_pack)-sizeof(target_head));
	vd_printf("api_one_video_server_cancel_req...\n");
	
	// �ȴ��ͻ�����Ӧ��
	if( sem_wait_ex2(presend_sem, ACK_RESPONSE_TIMEOUT) != 0 )
	{
		vd_printf("api_one_video_server_cancel_req,wait ack --- timeout ---\n");		
		one_multicast_server.msg	= VD_SERVER_MSG_RSP_TIMEOUT;	
		return one_multicast_server.msg;
	}

	// �ȴ��ͻ���ҵ��ظ�2s
	if( sem_wait_ex2(pwaitrsp_sem, BUSINESS_WAIT_TIMEPUT) != 0 )
	{
		vd_printf("api_one_video_server_cancel_req,wait rsp ---have no rsp---\n");		
		one_multicast_server.msg	= VD_SERVER_MSG_CANCEL_NO_RSP;		
		return one_multicast_server.msg;
	}
	
	vd_printf("api_one_video_server_cancel_req ok!\n" );
	
	return one_multicast_server.msg;
}


