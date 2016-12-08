
#ifndef _VIDEO_SOURCE_PROXY_H
#define _VIDEO_SOURCE_PROXY_H

typedef enum
{
	PROXY_NONE,
	PROXY_UNICAST,
	PROXY_MULTICAST,	
	PROXY_LINPHONE,
	PROXY_ONVIF,
}VIDEO_PROXY_TYPE;

typedef struct
{
	int					proxy_en;		// ��������Ч��־ 0/ disable�� 1/eanble �� 2/ proxy
	VIDEO_PROXY_TYPE	proxy_type;		// ����ķ�������
	int 				proxy_ip;		// �����������ip
}VIDEO_PROXY;

int disable_video_server( void );
int enable_video_server( void );
int enable_video_server_proxy( void );

int is_video_server_invailable( void );
int is_video_server_available( void );
int is_video_server_proxy_enable( void );

/*******************************************************************************************
 * @fn:		set_video_server_proxy
 *
 * @brief:	���ô��������
 *
 * @param:  	pproxy_server  - ����������ķ�������ָ��
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int set_video_server_proxy( const VIDEO_PROXY* pproxy_server );


/*******************************************************************************************
 * @fn:		get_video_server_proxy
 *
 * @brief:	�õ����������
 *
 * @param:  	pproxy_server  - ����������ķ�������ָ��
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int get_video_server_proxy( VIDEO_PROXY* pproxy_server );

/*******************************************************************************************
 * @fn:		get_video_server_proxy_type
 *
 * @brief:	�õ������������IP��ַ�ͷ�������
 *
 * @param:  	pproxy_server  - ����������ķ�������ָ��
 *
 * @return: 	VIDEO_PROXY_TYPE
*******************************************************************************************/
VIDEO_PROXY_TYPE get_video_server_proxy_type( void );

/*******************************************************************************************
 * @fn:		get_video_server_proxy_type
 *
 * @brief:	�õ������������IP��ַ
 *
 * @param:  	pproxy_server  - ����������ķ�������ָ��
 *
 * @return: 	jip
*******************************************************************************************/
int get_video_server_proxy_ip( void );

#endif

