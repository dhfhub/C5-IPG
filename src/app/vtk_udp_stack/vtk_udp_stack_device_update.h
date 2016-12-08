
#ifndef _VTK_UDP_STACK_DEVICE_UPDATE_H
#define _VTK_UDP_STACK_DEVICE_UPDATE_H

#include "vtk_udp_stack_class_ext.h"

#define	VDP_PRINTF_DEVICE_UPDATE

#ifdef	VDP_PRINTF_DEVICE_UPDATE
#define	dev_update_printf(fmt,...)	printf("[DECVICE_UPDATE]-["__FILE__"]-[%04d]-"fmt"",__LINE__,##__VA_ARGS__)
#else
#define	dev_update_printf(fmt,...)
#endif

#define DEVICE_SEARCH_MULTICAST_ADDR			"236.6.6.1"		// �鲥��ַ
#define DEVICE_SEARCH_UDP_BOARDCAST_PORT		25007			// �鲥�����������豸

#define DEVICE_SEARCH_CMD_READ_REQ				0x9A01			// �豸������������
#define DEVICE_SEARCH_CMD_READ_RSP				0x9A02			// �豸��������Ӧ��
#define DEVICE_SEARCH_CMD_WRITE_REQ				0x9B01			// �豸������������
#define DEVICE_SEARCH_CMD_WRITE_RSP				0x9B02			// �豸��������Ӧ��


#pragma pack(1)

//ip head struct define
typedef struct
{
	char 	s;
	char 	w;
	char 	i;
	char 	t;
	char 	c;
	char 	h;
} device_search_head;

typedef struct
{
	unsigned short	cmd;
} device_search_cmd;

// �豸�����������ݰ�
typedef struct
{
	char		req_source_zero[6];		// ����6���ֽ�0
	int			req_source_ip;			// ����PC�˵�ip��ַ
	char		req_target_zero[6];		// ����6���ֽ�0
	int			req_target_ip_seg;		// ����PC�˵�ip��ַ���ڵ�����
} device_search_read_req;

// �豸����Ӧ�����ݰ�
typedef struct
{
	char		req_source_zero[6];		// ����6���ֽ�0
	int			req_source_ip;			// ����PC�˵�ip��ַ
	char		req_target_zero[6];		// ����6���ֽ�0
	int			rsp_target_ip;			// Ӧ���豸��ip��ַ
	int			rsp_target_mask;		// Ӧ���豸��mask����
	int			rsp_target_gateway;		// Ӧ���豸������
	char		rsp_target_mac[6];		// Ӧ���豸��mac��ַ
	char		rsp_target_sn[10];		// Ӧ���豸�����кţ���ȷ������
} device_search_read_rsp;

// �豸�����������ݰ�
typedef struct
{
	int			req_target_old_ip;		// ָ���豸��ԭip��ַ
	int			req_target_new_ip;		// ָ���豸����ip��ַ
	int			req_target_new_mask;	// ָ���豸����mask����
	int			req_target_new_gateway;	// ָ���豸��������
	char		req_target_new_mac[6];	// ָ���豸����mac
} device_search_write_req;

// �豸����Ӧ�����ݰ�
typedef struct
{
	int			rsp_target_old_ip;		// Ӧ���豸��ԭip��ַ
	int			rsp_target_new_ip;		// Ӧ���豸����ip��ַ
	int			rsp_target_new_mask;	// Ӧ���豸����mask����
	int			rsp_target_new_gateway;	// Ӧ���豸��������
	char		rsp_target_new_mac[6];	// Ӧ���豸����mac
} device_search_write_rsp;

typedef struct
{
	device_search_head	head;
	device_search_cmd	cmd;
	union
	{
		char						dat[1];
		device_search_read_req		read_req;
		device_search_read_rsp		read_rsp;
		device_search_write_req		write_req;
		device_search_write_rsp		write_rsp;		
	}buf;
} device_search_package;


#pragma pack()

typedef struct
{
	udp_comm_rt 		udp;				//	udp����ģ��ʵ��
	send_sem_id_array	waitrsp_array;		// 	ҵ��Ӧ��ͬ������	
}device_update_instance_t;

// ��ʼ��udp:25000�ķ���ʵ��
int init_device_update_instance( void );

// ͨ��udp:25007�˿ڷ�������
int api_udp_device_update_send_data( int target_ip, unsigned short cmd, const char* pbuf, unsigned int len );

// ͨ��udp:25007�˿ڷ������ݰ��󣬲��ȴ�ҵ��Ӧ�𣬵õ�ҵ��Ӧ�������
int api_udp_device_update_send_req(int target_ip, unsigned short cmd, char* psbuf, int slen , char *prbuf, unsigned int *prlen);

// ���յ�udp:25007�˿����ݰ��������ҵ��Ӧ��
int api_udp_device_update_send_rsp( int target_ip, unsigned short cmd, int id, const char* pbuf, unsigned int len );

// ���յ�udp:25007�˿����ݰ��Ļص�����
int api_udp_device_update_recv_callback( int target_ip, unsigned short cmd, int sn, char* pbuf, unsigned int len );

// ���յ�udp:25007�˿����ݰ��Ļص����� - ���͵�survey����
int api_uddevice_update_recv_callback( int target_ip, unsigned short cmd, char* pbuf, unsigned int len );

#endif

