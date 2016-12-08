
#include "obj_ip_mon_link.h"
#include "../vtk_udp_stack/vtk_udp_stack_c5_ipc_cmd.h"
#include "../video_service/ip_video_cs_control.h"

// 0/ok, -1/err
int ip_mon_link_req( vdp_task_t *ptask, int target_ip, unsigned char dev_id, char* prcvbuf, unsigned int* plen )
{
	mon_link_request 	MonLinkCmdReq;
	
	MonLinkCmdReq.mon_type		= 0x34;		//  ���Ӻͺ��е�link��Ҫ����
	MonLinkCmdReq.device_id		= dev_id;
	MonLinkCmdReq.apply_type	= 0;

	if( api_udp_c5_ipc_send_req( target_ip,CMD_CALL_MON_LINK_REQ,(char*)&MonLinkCmdReq,sizeof(mon_link_request), prcvbuf, plen) == -1 )
	{
		bprintf("API_ip_mon_link_req fail\n");
		return -1;
	}
	else
	{
		bprintf("API_ip_mon_link_req ok\n");
		return 0;
	}
}

int ip_mon_link_rsp(UDP_MSG_TYPE* psource, mon_link_rsp_state state, mon_link_rsp_ng_reason reason, int gateway, int ipaddr )
{
	mon_link_response	MonLinkCmdRsp;
	mon_link_request*	pMonLinkCmdReq = (mon_link_request*)psource->pbuf;

	MonLinkCmdRsp.mon_type			= pMonLinkCmdReq->mon_type;
	MonLinkCmdRsp.device_id			= pMonLinkCmdReq->device_id;
	MonLinkCmdRsp.result.state		= state;
	MonLinkCmdRsp.result.reason		= reason;
	MonLinkCmdRsp.result.gateway	= gateway;
	MonLinkCmdRsp.result.ipaddr		= ipaddr;

	if( api_udp_c5_ipc_send_rsp(psource->target_ip,CMD_CALL_MON_LINK_RSP,psource->id,(char*)&MonLinkCmdRsp,sizeof(mon_link_response) ) == -1)
	{
		bprintf("API_ip_mon_link_rsp fail\n");
		return -1;
	}
	else
	{
		bprintf("API_ip_mon_link_rsp ok\n");
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//return: 0/ok, -1/�豸�쳣��1/proxy ok, 2/�豸������
int send_ip_mon_link_req( int target_ip, unsigned char dev_id, int* premote_ip, unsigned char* pdev_id  )
{
	mon_link_response* presponse;
	char rev_buf[200];
	int  rec_len = 200;	//czn_20160516

	if( ip_mon_link_req( &task_control, target_ip, dev_id, rev_buf, &rec_len ) == 0 )
	{
		presponse = (mon_link_response*)rev_buf;
	
		// �ж�״̬
		if( presponse->result.state == MON_SERVICE_ON )
		{
			dprintf("mon link 0x%08x.%d rsp state ON!\n",target_ip,dev_id);
			if( premote_ip != NULL )
				*premote_ip 	= target_ip;
			if( pdev_id != NULL )
				*pdev_id		= dev_id;
			return 0;
		}
		else if( presponse->result.state == MON_SERVICE_PROXY )
		{
			dprintf("mon link 0x%08x.%d rsp state PROXY!\n",target_ip,dev_id);
			
			int proxy_ip = 0;
			
			if( presponse->result.ipaddr )
			{
				proxy_ip = presponse->result.ipaddr;
			}
			else if( presponse->result.gateway && !presponse->result.ipaddr )
			{
				proxy_ip = inet_addr("192.168.1.0");
				proxy_ip |= (presponse->result.gateway<<24);					
			}			
			if( proxy_ip )
			{
				if( ip_mon_link_req( &task_control, proxy_ip, dev_id, rev_buf, &rec_len) != -1 )
				{
					presponse = (mon_link_response*)rev_buf;
					if( presponse->result.state == MON_SERVICE_ON )
					{
						dprintf("mon link 0x%08x.%d rsp state ON!\n",proxy_ip,dev_id);	
						if( premote_ip != NULL )
							*premote_ip	= proxy_ip;
						if( pdev_id != NULL )
							*pdev_id 	= dev_id;						
						return 0;
					}
					else if( presponse->result.state == MON_SERVICE_NG )
					{
						dprintf("mon link 0x%08x.%d rsp state NG!\n",proxy_ip,dev_id);		
						return 2;
					}
					else
						return -1;
				}
				else						
					return -1;
			}
			else
				return -1;
		}
		else if( presponse->result.state == MON_SERVICE_NG )
		{
			dprintf("mon link 0x%08x.%d rsp state NG!\n",target_ip,dev_id);		
			return 2;
		}
		else
		{
			return -1;
		}
	}
	else
		return -1;
}

int recv_ip_mon_link_req( UDP_MSG_TYPE* psource )
{
	mon_link_response 		MonLinkCmdRsp;
	mon_link_request*		pMonLinkCmdReq;
	mon_link_rsp_state		state;
	mon_link_rsp_ng_reason	reason;
	int						gateway;
	int						ipaddr;
	
	pMonLinkCmdReq = (mon_link_request*)psource->pbuf;
	
	// ��ѯ�豸״̬
	MonLinkCmdRsp.device_id	= pMonLinkCmdReq->device_id;

	// �������д���״̬
	if( is_video_server_proxy_enable() )
	{
		state		= MON_SERVICE_PROXY;
		reason		= MON_LINK_NG_NONE;
		gateway 	= 0;
		ipaddr		= get_video_server_proxy_ip();
	}
	// ����������״̬
	else if( is_video_server_available() )
	{
		if( api_get_video_cs_service_state() == VIDEO_CS_IDLE )
		{
			state		= MON_SERVICE_ON;
			reason		= MON_LINK_NG_NONE;
			gateway 	= 0;
			ipaddr		= 0;
		}
		// �ж��Ƿ�Ϊ����Ϊͬһ���豸id(����)�������֪�Է�ϵͳæ
		else
		{
			if( api_get_video_s_service_dev_id() == pMonLinkCmdReq->device_id )
			{
				state		= MON_SERVICE_ON;
				reason		= MON_LINK_NG_NONE;
				gateway 	= 0;
				ipaddr		= 0;
			}
			else
			{
				state		= MON_SERVICE_NG;
				reason		= MON_LINK_NG_TARGET_BUSY;
				gateway 	= 0;
				ipaddr		= 0;				
			}
		}
	}
	// �������ر�״̬
	else
	{
		state		= MON_SERVICE_NG;
		reason		= MON_LINK_NG_NO_TARGET;
		gateway 	= 0;
		ipaddr		= 0;		
	}

	// Ӧ�����ݰ�����
	if( ip_mon_link_rsp( psource, state, reason, gateway, ipaddr ) == 0 )
		return 0;
	else
		return -1;
}


