
#ifndef _VTK_UDP_STACK_CLASS_H
#define _VTK_UDP_STACK_CLASS_H

#include "../task_survey/task_survey.h"

#define NET_DEVICE_NAME		"eth0"

// video�ͻ���������ն˿ں�
#define VIDEO_CLIENT_CMD_RECV_PORT		28000
// video��������������ն˿�
#define VIDEO_SERVER_CMD_RECV_PORT		28001

// linphone call���ն˿ں�
#define LINPHONE_STARTER_RECV_PORT		28002

// video�鲥����˿ں�
#define VIDEO_SERVER_MULTICAST_PORT		28100	// 28003 ����Ϊ 28100-29100 , ��1000���˿ڣ�ÿ��ip 2���˿�

// audio�ͻ��˴���˿ں�
#define AUDIO_CLIENT_UNICAST_PORT		25003
// audio�������˴���˿�
#define AUDIO_SERVER_UNICAST_PORT		25003

//#define	VDP_PRINTF_UDP

#ifdef	VDP_PRINTF_UDP
#define	udp_printf(fmt,...)	printf("[UDP]-["__FILE__"]-[%04d]-"fmt"",__LINE__,##__VA_ARGS__)
#else
#define	udp_printf(fmt,...)
#endif

#define IP8210_CMD_START 		0xA1	// ��ʼͷ
#define IP8210_CMD_FLAG			0x5A	// ��־

// c5-ipg����������������
#define IP8210_CMD_TYPE			0xF1	// ��Ҫ�������ݰ�

// ip8210 ��������
#define DATA_SINGLE				0x01	/*����ת��*/
#define AUDIO_RUN				0x02	/*������Ƶ*/
#define DATA_AUDIO_RUN			0x03	/*������Ƶ+����ת��*/									 
#define AUDIO_STOP				0x04	/*ֹͣ��Ƶ*/
#define DATA_AUDIO_STOP			0x05	/*ֹͣ��Ƶ+����ת��*/
#define VIDEO_REC_RUN			0x08	/*������Ƶ����(����)*/
#define DATA_VIDEO_REC_RUN		0x09	/*������Ƶ����(����)+����ת��*/
#define VIDEO_REC_STOP			0x10	/*ֹͣ��Ƶ����(����)*/
#define DATA_VIDEO_REC_STOP		0x11	/*ֹͣ��Ƶ����(����)+����ת��*/
#define VIDEO_TRA_RUN			0x20	/*������Ƶ����(����)*/
#define DATA_VIDEO_TRA_RUN		0x21	/*������Ƶ����(����)+����ת��*/
#define VIDEO_TRA_STOP			0x40	/*ֹͣ��Ƶ����(����)*/
#define DATA_VIDEO_TRA_STOP		0x41	/*ֹͣ��Ƶ����(����)+����ת��*/
#define ARUN_VREC_RUN			0x0a 	/*������Ƶ+������Ƶ����(����)*/
#define DATA_ARUN_VREC_RUN		0x0b 	/*������Ƶ+������Ƶ����(����)+����ת��*/
#define ASTP_VREC_STP			0x14	/*ֹͣ��Ƶ+ֹͣ��Ƶ����(����)*/
#define DATA_ASTP_VREC_STP		0x15	/*ֹͣ��Ƶ+ֹͣ��Ƶ����(����)+����ת��*/
#define ARUN_VTRA_RUN			0x22	/*������Ƶ+������Ƶ����(����)*/
#define DATA_ARUN_VTRA_RUN		0x23	/*������Ƶ+������Ƶ����(����)+����ת��*/
#define ASTP_VTRA_STP			0x44	/*ֹͣ��Ƶ+ֹͣ��Ƶ����(����)*/
#define DATA_ASTP_VTRA_STP		0x45	/*ֹͣ��Ƶ+ֹͣ��Ƶ����(����)+����ת��*/
#define IP_LINK					0x96	/*IP���߲���*/
#define IP_LINK_RECEIPT			0x97	/*IP���߲���֮Ӧ��*/

#define IP_ADDR_R				0x98	/*��ȡIP��ַ*/
#define IP_ADDR_R_RECEIPT		0x99	/*��ȡIP��ַӦ��*/
#define IP_ADDR_W				0x9A	/*д��IP��ַ*/
#define IP_ADDR_W_RECEIPT		0x9B	/*д��IP��ַӦ��*/

#define DATA_ASTP_VTRA_RUN 		0x25	/*����ת����ͬʱ��Ƶֹͣ����Ƶ����*/
#define DATA_ASTP_VALL_STP		0x55	/*����ת����ͬʱ��Ƶֹͣ����Ƶ���շ���ֹͣ*/

#define CAMERA_BRIGHT_ADJUST 	0xA0	/*����֪ͨ��������ͷ����*/
#define CAMERA_COLOR_ADJUST		0xA1	/*����֪ͨ��������ͷɫ��*/


#define IP8210_BUFFER_MAX		240

// communication ack
#define	ACK						0xB5A1	// ����C5-IPC ([0]:B5, [1]:A1)


#define	ACK_RESPONSE_TIMEOUT	2000	// ʱ����Ҫ��һ�㣬�ϵ��һ�ν���ack����ʱ1.02s����??
#define BUSINESS_RESEND_TIME	5000
#define BUSINESS_WAIT_TIMEPUT	5000

//ip head struct define
typedef struct _baony_head_ 
{
	char 	start;
	char 	type;
	char 	len;
	char 	flag;
} baony_head;

//cmd head struct define
typedef struct _target_head_ 
{
	int				ip;
	unsigned short	cmd;
	unsigned short	id;
} target_head;

#define CHECKSUM_OFFSET_LEN	( sizeof(baony_head) + sizeof(int) )

//one normal package struct define
typedef struct _pack_
{
	//send head
	baony_head	head;
	//send target
	target_head	target;
	//send data
	char		dat[IP8210_BUFFER_MAX]; 
} pack;

typedef struct _pack_buf_
{
	//send target
	target_head	target;
	//len
	int			len;
	//send data
	char		dat[IP8210_BUFFER_MAX]; 
} pack_buf;

#define UDP_RT_TYPE_UNICAST		0
#define UDP_RT_TYPE_MULTICAST	1
#define UDP_RT_TYPE_BROADCAST	2

typedef int (*udp_msg_process)(char*,int);
typedef char* p_udp_common_buffer;

typedef struct loop_udp_common_buffer_tag
{
	OS_Q 					embosQ;
	int 					QSize;
	unsigned char*			pQBuf;
	udp_msg_process			process;			// ���ݴ�����
} loop_udp_common_buffer, *p_loop_udp_common_buffer;

typedef struct _send_sem_id_t
{
	int				enable;
	sem_t			trig_sem;
	unsigned short	send_id;
	unsigned short	send_cmd;
	int				send_timeout;
	int				send_timeout_cnt;
	int				resend_times;
	char			*pbuffer;			// ��̬�������ط�������ָ��
	int				len;				// ��̬�������ط�����������
	char			*prcvbuffer;		// ���յ����ݱ��浽�û���������ָ��
	unsigned int	*prcvlen;			// ���յ����ݱ��浽�û����ݳ���ָ��
} send_sem_id;

#define MAXf_SEND_SEM_ID_CNT		6
typedef struct _send_sem_id_array_t
{
	send_sem_id			dat[MAXf_SEND_SEM_ID_CNT];
    pthread_mutex_t 	lock;
} send_sem_id_array;

typedef struct _udp_comm_rt_t
{
	// comm
	char*						pname;				// �շ�����
	int							type;				// ���ͣ�0/�㲥���ͣ�1/�鲥���ͣ�2/�㲥����
	unsigned short				tport;				// ���Ͷ˿ں�
	unsigned short				rport;				// ���ն˿ں�
	int							local_addr;			// ������ַ  �����ֽ���
	char*						target_pstr;		// Ŀ���ַ
	// trs
	int							sock_trs_fd;		// ����socket���
	struct sockaddr_in 			trs_tar_addr;		// ���ͷ�Ŀ���ַ
	// trs buffer
	int 						tmsg_run_flag;		// ���ͻ�������߳����б�־
	pthread_t					tmsg_pid;			// ���ͻ�������߳�����id
	loop_udp_common_buffer		tmsg_buf;			// ���ͻ�����Ϣ����
	// resend array
	send_sem_id_array			resend_array;		// ͨ��Ӧ���ط�����
	// rcv
	int							sock_rcv_fd;		// ����socket���
	struct sockaddr_in 			rcv_tar_addr;		// ���շ�Դ��ַ
	int 						rcv_run_flag;		// �����߳����б�־
	pthread_t					rcv_pid;			// �����߳�����id	
	// rcv buffer
	int 						rmsg_run_flag;		// ���ջ�������߳����б�־
	pthread_t					rmsg_pid;			// ���ջ�����д����߳�����id	
	loop_udp_common_buffer		rmsg_buf;			// ���ջ������ݶ���
} udp_comm_rt;


int init_one_send_array( send_sem_id_array* psendarray );

// lzh_20160811_s
int one_udp_comm_trs_direct( udp_comm_rt* pins, int target_ip, char* pbuf, int len );
// lzh_20160811_e

sem_t* join_one_send_array( send_sem_id_array* psendarray, unsigned short send_id, unsigned short send_cmd, int send_timeout, int resend_times, char *pbuffer, int len );
int trig_one_send_array( send_sem_id_array* psendarray, unsigned short recv_id, unsigned short recv_cmd );
int poll_all_send_array( udp_comm_rt* pins, int time_gap );

// ��ʱ��ѯҵ��Ӧ����еĳ�ʱ
int poll_all_business_recv_array( send_sem_id_array* psendarray, int time_gap );

/*
 ���ܣ�
	��ʼ��һ��udpʵ���Ļ�����д�С����Ϣ����ص�����������СΪ0�����޻���
 ������
	pins				- udp �շ�ʵ��ָ��
	rmsg_qsize		- udp �������ݻ����С
	rmsg_process			- udp �������ݴ�����ָ��
	tmsg_qsize		- udp �������ݻ����С
	tmsg_process			- udp �������ݴ�����ָ��
*/
int init_one_udp_comm_rt_buff( udp_comm_rt* pins, int rmsg_qsize, udp_msg_process rmsg_process,int tmsg_qsize, udp_msg_process tmsg_process );

/*
 ���ܣ�
	��ʼ��һ��udpʵ��: ID�ţ����ͣ��˿ںţ�Ŀ���ַ
 ������
	pins		- udp �շ�ʵ��ָ��
	id			- udp ʵ��id
	type			- udp ���ͣ�0/�㲥��1/�鲥��2/�㲥
	rport		- udp ���հ󶨵Ķ˿ں�
	tport		- udp ���͵Ķ˿ں�
	target_pstr	- udp ����Ŀ���ַ
*/
int init_one_udp_comm_rt_type( udp_comm_rt* pins,  char* pname, int type, unsigned short rport, unsigned short tport, char* target_pstr);

/*
 ���ܣ�
	����ʼ��һ��udpʵ��
 ������
 	pins 		- udp �շ�ʵ��ָ��
*/
int deinit_one_udp_comm_rt( udp_comm_rt* pins);

/*
 ���ܣ�
	udpʵ�������ݽӷ�������API����
 ������
 	pins 		- udp �շ�ʵ��ָ��
 	target_ip 	- Ŀ��ip
 	cmd	 		- ����
 	id	 		- ����id
 	ack	 		- Ӧ�����(0����ͨ��Ӧ��)
 	ptrs_buf		- udp ��������ָ��
 	ptrs_len		- udp �������ݳ���
 ���أ�
	-1/err��0/ok
*/
sem_t* one_udp_comm_trs_api( udp_comm_rt* pins, int target_ip, int cmd, int id, int ack, char* ptrs_buf,  int ptrs_len );

// ����һ���������������������кŵ�ҵ��Ӧ����У����ȴ���������
sem_t* join_one_business_recv_array( send_sem_id_array* psendarray, unsigned short send_id, unsigned short send_cmd, int send_timeout, char *prcvbuffer, unsigned int *prcvlen );

// һ�����յ���������к���ҵ���������Ч�ķ�����������к�ƥ�䣬ƥ��ok�򴥷��ȴ����ź���
int trig_one_business_recv_array( send_sem_id_array* psendarray, unsigned short recv_id, unsigned short recv_cmd, char* buf, unsigned int len );

// ��ͨ��Ӧ��ʱɾ��ҵ��Ӧ�������
int dele_one_business_recv_array( send_sem_id_array* psendarray, unsigned short recv_id, unsigned short recv_cmd );

#endif

