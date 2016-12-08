
#ifndef _VTK_UDP_STACK_IO_SERVER_H
#define _VTK_UDP_STACK_IO_SERVER_H

#include "vtk_udp_stack_class_ext.h"

#define	VDP_PRINTF_IO_SERVER

#ifdef	VDP_PRINTF_IO_SERVER
#define	udp_io_server_printf(fmt,...)	printf("[udp_io_server]-["__FILE__"]-[%04d]-"fmt"",__LINE__,##__VA_ARGS__)
#else
#define	udp_io_server_printf(fmt,...)
#endif

#define UDP_IO_SERVER_UDP_PORT					25008			// IO server ͨ�Ŷ˿�

#define UDP_IO_SERVER_CMD_RW				0				// IO server ͨ��ָ��
#define UDP_IO_SERVER_CMD_RW_RSP		1				// IO server ͨ��ָ��

typedef struct
{
	udp_comm_rt 		udp;				//	udp����ģ��ʵ��
	int 				send_cmd_sn;		//	������������к�
	send_sem_id_array	waitrsp_array;		// 	ҵ��Ӧ��ͬ������	
}udp_io_server_instance_t;


// ͨ��udp:25008�˿ڷ������ݣ����ȴ�ҵ��Ӧ��
int api_udp_io_server_send_data( int target_ip, int cmd, const char* pbuf, unsigned int len );

// ͨ��udp:25008�˿ڷ������ݰ��󣬲��ȴ�ҵ��Ӧ�𣬵õ�ҵ��Ӧ�������
int api_udp_io_server_send_req(int target_ip, int cmd, char* psbuf, int slen , char *prbuf, unsigned int *prlen);

// ���յ�udp:25008�˿����ݰ��������ҵ��Ӧ��
int api_udp_io_server_send_rsp( int target_ip, int cmd, int id, const char* pbuf, unsigned int len );

// ���յ�udp:25008�˿����ݰ��Ļص����� - ���͵�survey����
int api_udp_io_serverc_recv_callback( int target_ip, int cmd, int sn, char* pbuf, unsigned int len );

#endif

