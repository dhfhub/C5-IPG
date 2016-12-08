
#ifndef _OBJ_IP_MON_LINK_H
#define _OBJ_IP_MON_LINK_H

#include "task_survey.h"

typedef enum
{
	MON_SERVICE_ON,
	MON_SERVICE_NG,
	MON_SERVICE_PROXY,
} mon_link_rsp_state;

typedef enum
{
	MON_LINK_NG_NONE,
	MON_LINK_NG_GATEWAY_BUSY,
	MON_LINK_NG_TARGET_BUSY,
	MON_LINK_NG_NO_TARGET,
} mon_link_rsp_ng_reason;

#pragma pack(1)

typedef struct
{
	//ͨ��ͷ
	uint8 	mon_type;		// ��������
	uint8 	device_id;		// �豸id
	//������������
	uint16 	apply_type;		// ��������
}mon_link_request;

typedef struct
{
	mon_link_rsp_state 		state;			// 0-OK; 1-NG; 2-Proxy
	mon_link_rsp_ng_reason	reason;			// �������ԭ�� : 1-Gateway_BUSY; 2-Target_BUSY; 3-NO_TARGET
	uint32					ipaddr;	
	uint32					gateway;	
}mon_link_result;

typedef struct
{
	//ͨ��ͷ
	uint8 					mon_type;		// ��������
	uint8 					device_id;		// �豸id
	//Ӧ����������
	mon_link_result			result;			// ���ؽ��
}mon_link_response;

#pragma pack()

int send_ip_mon_link_req( int target_ip, unsigned char dev_id, int* premote_ip, unsigned char* pdev_id  );
int recv_ip_mon_link_req( UDP_MSG_TYPE* psource );

#endif

