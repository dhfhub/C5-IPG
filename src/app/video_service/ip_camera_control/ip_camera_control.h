
#ifndef _IP_CAMERA_CONTROL_H
#define _IP_CAMERA_CONTROL_H

#include "video_multicast_server.h"
#include "video_linphone_server.h"

typedef enum
{
	ip_camera_none,				// ��
	ip_camera_local,			// ������Ƶ����
	ip_camera_multicast,		// �鲥����
	ip_camera_linphone,			// linhpone����
	ip_camera_unicast,			// �㲥����
} ip_camera_type;

typedef enum
{
	ip_camera_idle,				// �޺���
	ip_camera_caller,			// ������
	ip_camera_becalled,			// ������
} ip_camera_active;

typedef struct _ip_camera_server_state_
{
	ip_camera_type		type;
	ip_camera_active	active;
	int 				ip;
	int					dev_id;
} ip_camera_server_state;

/*******************************************************************************************
 * @fn:		ip_camera_control_init
 *
 * @brief:	��Ƶ�������˳�ʼ��
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int ip_camera_control_init(void);

/*******************************************************************************************
 * @fn:		api_camera_server_turn_on
 *
 * @brief:	���������Ƶ���� - ��Ϊ�鲥��Ϊ����ģʽ��ֱ�������鲥������linphone���������й���
 *
 * @param:	trans_type	- ��������
 * @param:  	client_ip		- �ͻ���IP��ַ
 * @param:  	period		- ����ʱ��
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_camera_server_turn_on( ip_camera_type trans_type, int client_ip, int period );

/*******************************************************************************************
 * @fn:		api_camera_server_turn_off
 *
 * @brief:	�˳���Ƶ���� - (��Ϊ�鲥��ȫ���˳�)
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_camera_server_turn_off(void);

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
int api_camera_server_multicast_apply_on( int apply_ip, int dev_id );

/*******************************************************************************************
 * @fn:		api_camera_server_multicast_notify_off
 *
 * @brief:	�޿ͻ��˺�ر��鲥����ͨ�� 
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_camera_server_multicast_notify_off( void );


/*******************************************************************************************
 * @fn:		api_camera_server_linphone_apply_on
 *
 * @brief:	�ͻ��˺���linphonec - �ͻ���linphonec���гɹ������
 *
 * @param:	client_ip - �ͻ���ip
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_camera_server_linphone_apply_on( int client_ip );

/*******************************************************************************************
 * @fn:		api_camera_server_linphone_apply_off
 *
 * @brief:	�ͻ���linphonec�˳� - �ͻ���linphonec�����˳������
 *
 * @param:	client_ip - �ͻ���ip
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_camera_server_linphone_apply_off( int client_ip );

int api_camera_server_adjust( UDP_Image_t* presource );
int api_video_server_adjust_bright( int inc  );
int api_video_server_adjust_color( int inc  );
int api_video_server_adjust_contrast( int inc  );
int api_camera_server_adjust_reply( int client_ip, UDP_Image_t* presource );

#endif

