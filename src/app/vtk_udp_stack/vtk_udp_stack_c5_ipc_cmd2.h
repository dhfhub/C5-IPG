
#ifndef _VTK_UDP_STACK_C5_IPC2_CMD_H
#define _VTK_UDP_STACK_C5_IPC2_CMD_H

#include "vtk_udp_stack_class.h"

#define	C5_IPC2_CMD_RCV_PORT	25001
#define C5_IPC2_CMD_TRS_PORT	25001

#define	VDP_PRINTF_C5_IPC2

#ifdef	VDP_PRINTF_C5_IPC2
#define	c5_ipc2_printf(fmt,...)	printf("[C5-IPC2]-["__FILE__"]-[%04d]-"fmt"",__LINE__,##__VA_ARGS__)
#else
#define	c5_ipc2_printf(fmt,...)
#endif

typedef struct
{
	udp_comm_rt 		udp;				//	udp����ģ��ʵ��
	int 				send_cmd_sn;		//	������������к�
	send_sem_id_array	waitrsp_array;		// 	ҵ��Ӧ��ͬ������	
}c5_ipc2_instance_t;

// ��ʼ��udp:25001�ķ���ʵ��
int init_c5_ipc2_instance( void );

// ͨ��udp:25001�˿ڷ������ݣ����ȴ�ҵ��Ӧ��
int api_udp_c5_ipc2_send_data( int target_ip, int cmd, const char* pbuf, unsigned int len );

// ͨ��udp:25001�˿ڷ������ݰ��󣬲��ȴ�ҵ��Ӧ�𣬵õ�ҵ��Ӧ�������
int api_udp_c5_ipc2_send_req(int target_ip, int cmd, char* psbuf, int slen , char *prbuf, unsigned int *prlen);

// ���յ�udp:25001�˿����ݰ��������ҵ��Ӧ��
int api_udp_c5_ipc2_send_rsp( int target_ip, int cmd, int id, const char* pbuf, unsigned int len );

// ���յ�udp:25001�˿����ݰ��Ļص����� - ���͵�survey����
int api_udp_c5_ipc2_recv_callback( int target_ip, int cmd, int sn, char* pbuf, unsigned int len );

// ���յ�udp:25001�˿����ݰ��Ļص����� - ���͵�survey����
int api_udp_transfer2_recv_callback( char* pbuf, unsigned int len );
int api_udp_transfer2_send_data2( int target_ip, const char* pbuf, unsigned int len );

#endif

