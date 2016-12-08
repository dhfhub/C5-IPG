
#ifndef _VTK_UDP_STACK_C5_IPC_CMD_H
#define _VTK_UDP_STACK_C5_IPC_CMD_H

#include "vtk_udp_stack_class.h"

#define	C5_IPC_CMD_RCV_PORT	25000
#define C5_IPC_CMD_TRS_PORT	25000

#define	VDP_PRINTF_C5_IPC

#ifdef	VDP_PRINTF_C5_IPC
#define	c5_ipc_printf(fmt,...)	printf("[C5-IPC]-["__FILE__"]-[%04d]-"fmt"",__LINE__,##__VA_ARGS__)
#else
#define	c5_ipc_printf(fmt,...)
#endif

#define CMD_CALL_MON_LINK_REQ			0x1001
#define CMD_CALL_MON_LINK_RSP			0x1081

#define CMD_CALL_MON_UNLINK_REQ			0x1002
#define CMD_CALL_MON_UNLINK_RSP			0x1082

#define CMD_NET_MANAGE_REQ				0x8001		// ���������������
#define CMD_NET_MANAGE_RSP				0x8081		// �����������Ӧ��

// ���������ָ��
#define CMD_CAM_REMOTE_ADJUST_REQ		0x0010		// Զ�̵�������ͷ����
#define CMD_CAM_REMOTE_ADJUST_RSP		0x0090		// Զ�̵�������ͷӦ��

// lzh_20160622_s
#define CMD_DEVICE_TYPE_CODE_VER_REQ		0x9001		// Զ�̲�ѯ�豸���ͣ���code�汾��Ϣ
#define CMD_DEVICE_TYPE_CODE_VER_RSP		0x9081		// Զ�̲�ظ��豸���ͣ���code�汾��Ϣ

#define CMD_DEVICE_TYPE_DOWNLOAD_START_REQ	0x9002		// 
#define CMD_DEVICE_TYPE_DOWNLOAD_START_RSP	0x9082		// 

#define CMD_DEVICE_TYPE_DOWNLOAD_STOP_REQ	0x9003		// 
#define CMD_DEVICE_TYPE_DOWNLOAD_STOP_RSP	0x9083		// 

#define CMD_DEVICE_TYPE_DOWNLOAD_VERIFY_REQ	0x9004		// 
#define CMD_DEVICE_TYPE_DOWNLOAD_VERIFY_RSP	0x9084		// 

#define CMD_DEVICE_TYPE_UPDATE_START_REQ	0x9005		// 
#define CMD_DEVICE_TYPE_UPDATE_START_RSP	0x9085		// 

#define CMD_DEVICE_TYPE_UPDATE_OVER_REQ		0x9006		// 
#define CMD_DEVICE_TYPE_UPDATE_OVER_RSP		0x9086		// 
// lzh_20160622_e

typedef struct
{
	udp_comm_rt 		udp;				//	udp����ģ��ʵ��
	int 				send_cmd_sn;		//	������������к�
	send_sem_id_array	waitrsp_array;		// 	ҵ��Ӧ��ͬ������	
}c5_ipc_instance_t;

// ��ʼ��udp:25000�ķ���ʵ��
int init_c5_ipc_instance( void );

// ͨ��udp:25000�˿ڷ������ݣ����ȴ�ҵ��Ӧ��
int api_udp_c5_ipc_send_data( int target_ip, int cmd, const char* pbuf, unsigned int len );

// ͨ��udp:25000�˿ڷ������ݰ��󣬲��ȴ�ҵ��Ӧ�𣬵õ�ҵ��Ӧ�������
int api_udp_c5_ipc_send_req(int target_ip, int cmd, char* psbuf, int slen , char *prbuf, unsigned int *prlen);

// ���յ�udp:25000�˿����ݰ��������ҵ��Ӧ��
int api_udp_c5_ipc_send_rsp( int target_ip, int cmd, int id, const char* pbuf, unsigned int len );

// ���յ�udp:25000�˿����ݰ��Ļص����� - ���͵�survey����
int api_udp_c5_ipc_recv_callback( int target_ip, int cmd, int sn, char* pbuf, unsigned int len );

// ���յ�udp:25000�˿����ݰ��Ļص����� - ���͵�survey����
int api_udp_transfer_recv_callback( char* pbuf, unsigned int len );
int api_udp_transfer_send_data( int target_ip, const char* pbuf, unsigned int len );

#endif

