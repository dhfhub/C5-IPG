

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>      /* basic system data types */
#include <error.h>

#include "ip_camera_control.h"
#include "../video_object.h"

ip_camera_server_state	one_ip_camera_server;

/*******************************************************************************************
 * @fn:		ip_camera_control_init
 *
 * @brief:	��Ƶ�������˳�ʼ��
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int ip_camera_control_init(void)
{
	one_ip_camera_server.type 	= ip_camera_none;
	one_ip_camera_server.active	= ip_camera_idle;
	one_ip_camera_server.ip		= 0;
	one_ip_camera_server.dev_id	= 0;
	init_one_multicast_server();
}

/*******************************************************************************************
 * @fn:		api_camera_server_turn_on
 *
 * @brief:	���������Ƶ����
 *
 * @param:	trans_type	- ��������
 * @param:  	client_ip		- �ͻ���IP��ַ
 * @param:  	period		- ����ʱ��
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_camera_server_turn_on( ip_camera_type trans_type, int client_ip, int period )
{
	int				mcg_addr;
	short			port;
	
	vd_printf("send server request command to ip = 0x%08x\n",client_ip);

	switch( trans_type )
	{
		case ip_camera_local:
			// ���������鲥�����������������Ϊ����ģʽ�������Ĵ��鲥����
			get_server_multicast_addr_port( &mcg_addr, &port );
			// ���ÿ���ָ���ͻ�����Ƶ�鲥�������
			api_enable_video_server_multicast( mcg_addr, port );			
			break;
		case ip_camera_multicast:
			// lzh_20161119_s
			one_ip_camera_server.ip 	= client_ip;			
			// lzh_20161119_e
			// ���������鲥�����������������Ϊ����ģʽ�������Ĵ��鲥����
			get_server_multicast_addr_port( &mcg_addr, &port );
			// ���ÿ���ָ���ͻ�����Ƶ�鲥�������
			api_enable_video_server_multicast( mcg_addr, port );
			break;
		case ip_camera_linphone:
			// check if multicast or not
			if( one_ip_camera_server.type == ip_camera_multicast )
			{
				one_ip_camera_server.type 	= ip_camera_none;
				api_one_video_server_cancel_req( one_ip_camera_server.ip );
			}
			// check if linphone or not
			if( one_ip_camera_server.type == ip_camera_linphone )
			{
				one_ip_camera_server.type 	= ip_camera_none;
				api_linphone_server_to_close( one_ip_camera_server.ip );
			}			
			// apply for multicast			
			if( api_linphone_server_to_call(client_ip,get_g_video_object_auto_talk() ) >= 0 )
			{
				one_ip_camera_server.ip 	= client_ip;
				one_ip_camera_server.type 	= ip_camera_linphone;
				one_ip_camera_server.active	= ip_camera_caller;				
			}
			break;
	}	
	return 0;	
}


/*******************************************************************************************
 * @fn:		api_camera_server_turn_off
 *
 * @brief:	�˳���Ƶ���� - (��Ϊ�鲥��ȫ���˳�)
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_camera_server_turn_off( void )
{
	vd_printf("send server close command to ip = 0x%08x\n",one_ip_camera_server.ip );

	switch( one_ip_camera_server.type )
	{
		case ip_camera_multicast:
			api_one_video_server_cancel_req( one_ip_camera_server.ip );
			break;
		case ip_camera_linphone:
			api_linphone_server_to_close( one_ip_camera_server.ip );
			break;
		default:
			break;
	}
	one_ip_camera_server.type 	= ip_camera_none;
	one_ip_camera_server.active	= ip_camera_idle;
	return 0;		
}

/*******************************************************************************************
 * @fn: 	api_camera_server_multicast_apply_on
 *
 * @brief:	�鲥�������� - ������һ�ο����鲥����ʱ����Ҫ��ǰ������
 *
 * @param:	apply_ip 	- ���󷽵�IP
 * @param:	dev_id 	- ������豸id
 *
 * @return: 	-1/err, 0/ok, 1/ת����
*******************************************************************************************/
int api_camera_server_multicast_apply_on( int apply_ip, int dev_id )
{
	if( api_video_s_service_start_apply(ip_camera_multicast, apply_ip, dev_id ) == 0 )
	{
		one_ip_camera_server.type	= ip_camera_multicast;
		one_ip_camera_server.active = ip_camera_becalled; 
		one_ip_camera_server.dev_id	= dev_id;
		return 0;
	}
	else
		return -1;
}

/*******************************************************************************************
 * @fn:		api_camera_server_multicast_notify_off
 *
 * @brief:	�޿ͻ��˺�ر��鲥����ͨ�� 
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_camera_server_multicast_notify_off( void )
{
	one_ip_camera_server.type 	= ip_camera_none;			
	one_ip_camera_server.active	= ip_camera_idle;	
	// ����ͨ������
	api_video_s_service_close_notify();
	return 0;
}

/*******************************************************************************************
 * @fn:		api_camera_server_linphone_apply_on
 *
 * @brief:	�ͻ��˺���linphonec - �ͻ���linphonec���гɹ������
 *
 * @param:	client_ip - �ͻ���ip
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_camera_server_linphone_apply_on( int client_ip )
{
	vd_printf("one linphone apply on, ip = 0x%08x\n",client_ip );

	if( api_video_s_service_start_apply(ip_camera_linphone, client_ip) )
	{
		// �Ǽ�״̬
		one_ip_camera_server.type	= ip_camera_linphone;
		one_ip_camera_server.active = ip_camera_becalled; 	
		one_ip_camera_server.ip		= client_ip;
		return 0;
	}	
	else
		return -1;
}

/*******************************************************************************************
 * @fn:		api_camera_server_linphone_apply_off
 *
 * @brief:	�ͻ���linphonec�˳� - �ͻ���linphonec�����˳������
 *
 * @param:	client_ip - �ͻ���ip
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_camera_server_linphone_apply_off( int client_ip )
{
	vd_printf("one linphone apply off, ip = 0x%08x\n",client_ip );

	// �Ǽ�״̬
	one_ip_camera_server.type	= ip_camera_none;
	one_ip_camera_server.active = ip_camera_idle;
	
	// ����ͨ�����
	api_video_s_service_close_notify();
	return 0;
}

int api_camera_server_adjust( UDP_Image_t* presource )
{
	if( presource == NULL )
		return -1;
	
	switch( presource->type )
	{
		case ADJ_CONTRAST:
			if( presource->dir == ADJ_DEC )
				presource->data = SetImageContrastDec();
			else if( presource->dir == ADJ_INC )
				presource->data = SetImageContrastInc();
			break;
	
		case ADJ_BRIGHT:
			if( presource->dir == ADJ_DEC )
				presource->data = SetImageBrightDec();
			else if( presource->dir == ADJ_INC )
				presource->data = SetImageBrightInc();		
			break;
	
		case ADJ_COLOR:
			if( presource->dir == ADJ_DEC )
				presource->data = SetImageColorDec();
			else if( presource->dir == ADJ_INC )
				presource->data = SetImageColorInc();			
			break;
	
		case ADJ_ALL:
			vd_printf(" ADJ_ALL  %d %d %d \n",presource->data.logContrastCnt, presource->data.logBrightCnt, presource->data.logColorCnt);
			if( presource->dir == ADJ_GET )
				ReadImagePara( &presource->data );
			else if( presource->dir == ADJ_SET )
				presource->data = SetImageAllValue( presource->data.logContrastCnt, presource->data.logBrightCnt, presource->data.logColorCnt );
			break;
	}
	return 0;
}

int api_video_server_adjust_bright( int inc  )
{
	UDP_Image_t apply;

	if( inc )
		apply.dir 	= ADJ_INC;
	else
		apply.dir 	= ADJ_DEC;
	apply.type		= ADJ_BRIGHT;
	
	return api_camera_server_adjust(&apply);
}

int api_video_server_adjust_color( int inc  )
{
	UDP_Image_t apply;

	if( inc )
		apply.dir 	= ADJ_INC;
	else
		apply.dir 	= ADJ_DEC;
	apply.type		= ADJ_COLOR;
	
	return api_camera_server_adjust(&apply);
}

int api_video_server_adjust_contrast( int inc )
{
	UDP_Image_t apply;

	if( inc )
		apply.dir 	= ADJ_INC;
	else
		apply.dir 	= ADJ_DEC;
	apply.type		= ADJ_CONTRAST;
	
	return api_camera_server_adjust(&apply);
}

int api_camera_server_adjust_reply( int client_ip, UDP_Image_t* presource )
{
	if( api_udp_c5_ipc_send_data( client_ip,CMD_CAM_REMOTE_ADJUST_RSP,(char*)&presource,sizeof(UDP_Image_t)) == -1 )
	{
		vd_printf("api_video_s_service_adjust_reply fail\n");
		return -1;
	}
	else
	{
		vd_printf("api_video_s_service_adjust_reply ok\n");
		return 0;
	}		
}


