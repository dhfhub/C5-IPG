
#ifndef _VIDEO_OBJECT_H
#define _VIDEO_OBJECT_H

typedef enum
{
	VD_MULTICAST,
	VD_LINPHONEC,
} VD_OBJECT_TRANS_TYPE;

// ��Ƶ��Դ���ö���
typedef struct
{
	VD_OBJECT_TRANS_TYPE		vd_trans_type;			// 	�������ͣ�0/ multicast��1/ linphonec
	int							ip;
	char						vd_src_name[250];		//  	��Ƶ��Դ�豸���ƣ���: "CDS1-1"
	int							vd_trans_time;			//	��Ƶ����ʱ��	
	int							auto_talk;				//	�Զ�����ͨ��״̬��linphonec����ʹ��
} video_object;

int set_g_video_object_src_attr( VD_OBJECT_TRANS_TYPE trans_type, int ip, char* src_name, int trans_time, int auto_talk );
int get_g_video_object_src_ip( void );
char* get_g_video_object_src_name( void );
int get_g_video_object_trans_type( void );
int get_g_video_object_auto_talk( void );
int get_g_video_object_trans_time( void );



#endif


