
#ifndef _IP_VIDEO_CONTROL_H
#define _IP_VIDEO_CONTROL_H

#include "video_multicast_client.h"
#include "video_linphone_client.h"

typedef enum
{
	ip_video_none,				// ��
	ip_video_multicast,			// �鲥����
	ip_video_linphone,			// linhpone����
	ip_video_unicast,			// �㲥����
} ip_video_type;

typedef enum
{
	ip_video_idle,				// �޺���
	ip_video_caller,			// ������
	ip_video_becalled,			// ������
} ip_video_active;

typedef struct _ip_video_client_state_
{
	ip_video_type	type;
	ip_video_active	active;
	int 			server_ip;
	int 			server_dev_id;
} ip_video_client_state;

/*******************************************************************************************
 * @fn:		ip_video_control_init
 *
 * @brief:	��Ƶ�ͻ��˳�ʼ��
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int ip_video_control_init(void);

/*******************************************************************************************
 * @fn:		api_video_client_turn_on
 *
 * @brief:	���������Ƶ�������Ƶ��
 *
 * @param:	trans_type	- ��������
 * @param:  	server_ip		- ������IP��ַ
 * @param:  	dev_id		- ���������豸id
 * @param:  	period		- ����ʱ��
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_client_turn_on( ip_video_type trans_type, int server_ip, int dev_id, int period );

/*******************************************************************************************
 * @fn:		api_video_client_turn_off
 *
 * @brief:	�˳���Ƶ����
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_client_turn_off(void);

/*******************************************************************************************
 * @fn:		api_video_client_multicast_notify_off
 *
 * @brief:	������֪ͨ�رտͻ����鲥����
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_client_multicast_notify_off( int server_ip );


/*******************************************************************************************
 * @fn:		api_video_client_linphone_apply_on
 *
 * @brief:	�������˺���linphonec - ��������linphonec���гɹ������
 *
 * @param:	server_ip - ��������ip
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_client_linphone_apply_on( int server_ip );

/*******************************************************************************************
 * @fn:		api_video_client_linphone_apply_off
 *
 * @brief:	���������˳�linphonec - ��������linphonec���йرպ����
 *
 * @param:	server_ip - ��������ip
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_client_linphone_apply_off( int server_ip );


int api_video_client_adjust_apply( UDP_Image_t* presult  );
int api_video_client_adjust_bright_apply( int inc  );
int api_video_client_adjust_color_apply( int inc  );
int api_video_client_adjust_contrast_apply( int inc  );

#endif





