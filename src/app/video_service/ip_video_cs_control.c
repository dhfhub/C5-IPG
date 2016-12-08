

#include "ip_video_cs_control.h"

VIDEO_CS_STATE 	global_cs_state;
ip_video_type	video_c_type;
ip_camera_type	video_s_type;

// ��ǰ��ƵԴ�豸id
int				video_source_id = 0;
/*******************************************************************************************
 * @fn:		init_video_cs_service
 *
 * @brief:	��ʼ����ƵCS����
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int init_video_cs_service(void)
{
	ip_video_control_init();
	ip_camera_control_init();
	//wgc
	vtk_VideoCaptureController();
	vtk_VideoDisplayerController();
	audio_to_process_state_init();
	audio_fm_process_state_init();

	global_cs_state	= VIDEO_CS_IDLE;
	video_c_type	= ip_video_none;
	video_s_type	= ip_camera_none;

	// ���÷������������� for test
	VIDEO_PROXY proxy;
	proxy.proxy_type	= PROXY_MULTICAST;
	proxy.proxy_ip		= inet_addr("192.168.1.23");
	set_video_server_proxy(&proxy);
	// ���÷�����ʹ�ܴ���
	//enable_video_server_proxy();
	
	// �رշ���������
	//disable_video_server();
	// ʹ�ܷ���������
	enable_video_server();
	
	return 0;
}


/*******************************************************************************************
 * @fn:		api_get_video_cs_service_state
 *
 * @brief:	�õ���ǰ��ƵCS��״̬ �C 0-����/1-�ͻ���/2-��������/3-C&S
 *
 * @param:	none
 *
 * @return: 	VIDEO_CS_STATE
*******************************************************************************************/
VIDEO_CS_STATE api_get_video_cs_service_state(void)
{
	return global_cs_state;
}

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
int api_video_s_service_turn_on( ip_camera_type trans_type, int client_ip, int period )
{
	api_camera_server_turn_on( trans_type, client_ip, period );	
	global_cs_state	= VIDEO_CS_SERVER;	
	video_s_type	= trans_type;
	return 0;
}

/*******************************************************************************************
 * @fn:		api_video_s_service_turn_off
 *
 * @brief:	�رշ���������Ƶ�������
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_s_service_turn_off( void )
{
	api_camera_server_turn_off();
	global_cs_state	= VIDEO_CS_IDLE;	
	video_s_type	= ip_camera_none;
	return 0;
}


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
int api_video_s_service_start_apply( ip_camera_type trans_type, int client_ip, int dev_id )
{
	// ��������������δ������������ͻ���������Ƶ������
	if( is_video_server_available() )
	{
		if( global_cs_state == VIDEO_CS_IDLE )
		{
			video_s_type 	= trans_type;
			global_cs_state = VIDEO_CS_SERVER;
			video_source_id = dev_id;
			
			VIDEO_SEVICE_NOTIFY_REMOTE_ON();
			
			return 0;
		}
		else if( global_cs_state == VIDEO_CS_SERVER )
		{
			// ƥ���Ƿ�Ϊͬһ������ͷ
			if( video_source_id == dev_id )
				return 0;
			else
				return -1;
		}			
	}
	else
	{
		return -1;
	}
}

// �õ���ǰ�������ƵԴ�豸id
int api_get_video_s_service_dev_id( void )
{
	return video_source_id;
}

int api_video_s_service_close( void )
{
	global_cs_state	= VIDEO_CS_IDLE;
	video_s_type	= ip_camera_none;
	return 0;
}

/*******************************************************************************************
 * @fn:		api_video_s_service_close_notify
 *
 * @brief:	�������˽��յ���Ƶ�ر�ͨ�� -���ɺ�˵��ã�
 *
 * @param:  	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_s_service_close_notify( void )
{
	global_cs_state	= VIDEO_CS_IDLE;
	video_s_type	= ip_camera_none;

	VIDEO_SEVICE_NOTIFY_REMOTE_OFF();
	
	return 0;
}


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
int api_video_c_service_turn_on( ip_video_type trans_type, int server_ip, int dev_id, int period )
{
	if( api_video_client_turn_on(trans_type,server_ip,dev_id,period) == 0 )
	{
		global_cs_state	= VIDEO_CS_CLIENT;
		video_c_type	= trans_type;	
		return 0;
	}
	else
		return -1;	
}

/*******************************************************************************************
 * @fn:		api_video_c_service_turn_off
 *
 * @brief:	�رտͻ�������Ƶ�������
 *
 * @param:	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_c_service_turn_off( void )
{
	if( global_cs_state	!= VIDEO_CS_IDLE )
	{
		api_video_client_turn_off();
	}
	global_cs_state	= VIDEO_CS_IDLE;	
	video_c_type	= ip_video_none;	
	return 0;
}

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
int api_video_c_service_start_apply( ip_video_type trans_type, int server_ip )
{
	global_cs_state	= VIDEO_CS_CLIENT;
	video_c_type	= trans_type;
	return 0;
}

/*******************************************************************************************
 * @fn:		api_video_c_service_close_notify
 *
 * @brief:	�ͻ��˽��յ���Ƶ�ر�ͨ�� -���ɺ�˵��ã�
 *
 * @param:  	none
 *
 * @return: 	-1/err, 0/ok
*******************************************************************************************/
int api_video_c_service_close_notify( void )
{
	global_cs_state	= VIDEO_CS_IDLE;
	video_c_type	= ip_video_none;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
//��Ƶ����api
/////////////////////////////////////////////////////////////////////////////////////
int api_video_c_service_adjust_apply( UDP_Image_t* presult  )
{
	return api_video_client_adjust_apply(presult);
}

int api_video_c_service_adjust_bright_apply( int inc  )
{
	return api_video_client_adjust_bright_apply(inc);
}

int api_video_c_service_adjust_color_apply( int inc  )
{
	return api_video_client_adjust_color_apply(inc);
}

int api_video_c_service_adjust_contrast_apply( int inc  )
{
	return api_video_client_adjust_contrast_apply(inc);
}

int api_video_s_service_adjust( UDP_Image_t* presource  )
{
	if( global_cs_state == VIDEO_CS_IDLE || global_cs_state == VIDEO_CS_SERVER )
		return -1;
	else
		return api_camera_server_adjust(presource);
}

int api_video_s_service_adjust_bright( int inc  )
{
	//if( global_cs_state == VIDEO_CS_IDLE || global_cs_state == VIDEO_CS_SERVER )
	//	return -1;
	//else
		return api_video_server_adjust_bright(inc);
}

int api_video_s_service_adjust_color( int inc  )
{
	//if( global_cs_state == VIDEO_CS_IDLE || global_cs_state == VIDEO_CS_SERVER )
	//	return -1;
	//else
		return api_video_server_adjust_color(inc);
}

int api_video_s_service_adjust_contrast( int inc  )
{
	if( global_cs_state == VIDEO_CS_IDLE || global_cs_state == VIDEO_CS_SERVER )
		return -1;
	else
		return api_video_server_adjust_contrast(inc);
}

int api_video_s_service_adjust_reply( int client_ip, UDP_Image_t* presource )
{
	if( presource == NULL )
		return -1;
	else
		return api_camera_server_adjust_reply(client_ip, presource);
}



