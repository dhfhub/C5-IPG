
#include "../video_multicast_common.h"
#include "video_source_proxy.h"

VIDEO_PROXY	one_video_proxy={0};

/*******************************************************************************************
 * @fn:		disable_video_server
 *
 * @brief:	������Ƶ��������Ч
 *
 * @param:  	none
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int disable_video_server( void )
{
	one_video_proxy.proxy_en = 0;
	return 0;
}

/*******************************************************************************************
 * @fn:		enable_video_server
 *
 * @brief:	������Ƶ��������Ч
 *
 * @param:  	none
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int enable_video_server( void )
{
	one_video_proxy.proxy_en = 1;
	return 0;
}

/*******************************************************************************************
 * @fn:		enable_video_server_proxy
 *
 * @brief:	���ô����������Ч
 *
 * @param:  	none
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int enable_video_server_proxy( void )
{
	one_video_proxy.proxy_en = 2;
	return 0;
}

/*******************************************************************************************
 * @fn:		is_video_server_invailable
 *
 * @brief:	�жϷ�������Ч���޴���
 *
 * @param:  	none
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int is_video_server_invailable( void )
{
	return !one_video_proxy.proxy_en;
}

/*******************************************************************************************
 * @fn:		is_video_server_available
 *
 * @brief:	�жϷ�������Ч���޴���
 *
 * @param:  	none
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int is_video_server_available( void )
{
	return (one_video_proxy.proxy_en==1)?1:0;
}

/*******************************************************************************************
 * @fn:		is_video_server_proxy_enable
 *
 * @brief:	�жϷ�����������Ч
 *
 * @param:  	none
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int is_video_server_proxy_enable( void )
{
	return (one_video_proxy.proxy_en==2)?1:0;
}

/*******************************************************************************************
 * @fn:		set_video_server_proxy
 *
 * @brief:	���ô��������
 *
 * @param:  	pproxy_server  - ����������ķ�������ָ��
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int set_video_server_proxy( const VIDEO_PROXY* pproxy_server )
{
	if( pproxy_server )
	{
		one_video_proxy = *pproxy_server;
		return 0;
	}
	else
		return -1;
}

/*******************************************************************************************
 * @fn:		get_video_server_proxy
 *
 * @brief:	�õ������������IP��ַ�ͷ�������
 *
 * @param:  	pproxy_server  - ����������ķ�������ָ��
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int get_video_server_proxy( VIDEO_PROXY* pproxy_server )
{	
	if( pproxy_server )
	{
		*pproxy_server = one_video_proxy;
		return 0;
	}
	else
		return -1;
}

/*******************************************************************************************
 * @fn:		get_video_server_proxy_type
 *
 * @brief:	�õ������������IP��ַ�ͷ�������
 *
 * @param:  	pproxy_server  - ����������ķ�������ָ��
 *
 * @return: 	VIDEO_PROXY_TYPE
*******************************************************************************************/
VIDEO_PROXY_TYPE get_video_server_proxy_type( void )
{	
	return one_video_proxy.proxy_type;
}

/*******************************************************************************************
 * @fn:		get_video_server_proxy_type
 *
 * @brief:	�õ������������IP��ַ
 *
 * @param:  	pproxy_server  - ����������ķ�������ָ��
 *
 * @return: 	jip
*******************************************************************************************/
int get_video_server_proxy_ip( void )
{	
	return one_video_proxy.proxy_ip;
}

