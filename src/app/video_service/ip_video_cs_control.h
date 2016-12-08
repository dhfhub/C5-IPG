
#ifndef _IP_VIDEO_CS_CONTORL_H_
#define _IP_VIDEO_CS_CONTORL_H_

#include "video_multicast_common.h"
#include "./ip_video_control/ip_video_control.h"
#include "./ip_camera_control/ip_camera_control.h"

typedef enum
{
	VIDEO_CS_IDLE,
	VIDEO_CS_CLIENT,
	VIDEO_CS_SERVER,
	VIDEO_CS_BOTH,
} VIDEO_CS_STATE;

/*******************************************************************************************
 * @fn:		init_video_cs_service
 *
 * @brief:	��ʼ����ƵCS����
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int init_video_cs_service(void);


/*******************************************************************************************
 * @fn:		api_get_video_cs_service_state
 *
 * @brief:	�õ���ǰ��ƵCS��״̬ �C 0-����/1-�ͻ���/2-��������/3-C&S
 *
 * @param:	none
 *
 * @return: 	VIDEO_CS_STATE
*******************************************************************************************/
VIDEO_CS_STATE api_get_video_cs_service_state(void);


/*******************************************************************************************
 * @fn:		api_video_s_service_turn_on
 *
 * @brief:	��������������Ƶ������� �C ��Ϊ�鲥��Ϊ���Է���ǿ�ƴ���Ƶ�鲥���
 *
 * @param:	trans_type	- ��������
 * @param:  	client_ip		- �ͻ���IP��ַ
 * @param:  	period		- ����ʱ��
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_s_service_turn_on( ip_camera_type trans_type, int client_ip, int period );

/*******************************************************************************************
 * @fn:		api_video_s_service_turn_off
 *
 * @brief:	�رշ���������Ƶ�������
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_s_service_turn_off( void );

/*******************************************************************************************
 * @fn:		api_video_s_service_start_apply
 *
 * @brief:	�������˽��յ���Ƶ������� -���ɺ�˵��ã�
 *
 * @param:  	trans_type	- ��������
 * @param:  	client_ip		- �ͻ���IP��ַ
 * @param:  	dev_id		- �ͻ������������ͷid
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_s_service_start_apply( ip_camera_type trans_type, int client_ip, int dev_id );

// �õ���ǰ�������ƵԴ�豸id
int api_get_video_s_service_dev_id( void );

int api_video_s_service_close( void );

/*******************************************************************************************
 * @fn:		api_video_s_service_close_notify
 *
 * @brief:	�������˽��յ���Ƶ�ر�ͨ�� -���ɺ�˵��ã�
 *
 * @param:  	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_s_service_close_notify( void );

/*******************************************************************************************
 * @fn:		api_video_c_service_turn_on
 *
 * @brief:	�������������Ƶ�������
 *
 * @param:	trans_type	- ��������
 * @param:  	server_ip	- ��������IP��ַ
 * @param:  	dev_id	- ���������豸id
 * @param:  	period		- ����ʱ��
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_c_service_turn_on( ip_video_type trans_type, int server_ip, int dev_id, int period );


/*******************************************************************************************
 * @fn:		api_video_c_service_turn_off
 *
 * @brief:	�رտͻ�������Ƶ�������
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_c_service_turn_off( void );


/*******************************************************************************************
 * @fn:		api_video_c_service_start_apply
 *
 * @brief:	�ͻ��˽��յ���Ƶ�������� -���ɺ�˵��ã�
 *
 * @param:  	trans_type - ��������
 * @param:  	server_ip - ��������IP��ַ
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_c_service_start_apply( ip_video_type trans_type, int server_ip );

/*******************************************************************************************
 * @fn:		api_video_c_service_close_notify
 *
 * @brief:	�ͻ��˽��յ���Ƶ�ر�ͨ�� -���ɺ�˵��ã�
 *
 * @param:  	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_c_service_close_notify( void );


#define API_VIDEO_S_SERVICE_TURN_ON_TEST(t)	\
	api_video_s_service_turn_on( ip_camera_multicast, 0, t )

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int api_video_c_service_adjust_apply( UDP_Image_t* presult );
int api_video_c_service_adjust_bright_apply( int inc  );
int api_video_c_service_adjust_color_apply( int inc );
int api_video_c_service_adjust_contrast_apply( int inc  );

int api_video_s_service_adjust( UDP_Image_t* presource );

int api_video_s_service_adjust_bright( int inc  );
int api_video_s_service_adjust_color( int inc  );
int api_video_s_service_adjust_contrast( int inc  );
int api_video_s_service_adjust_reply( int client_ip, UDP_Image_t* presource );
//int api_video_s_service_initial( int inc  );

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif

