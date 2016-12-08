

#include "../task_survey/task_survey.h"
#include "../task_debug_sbu/task_debug_sbu.h"
#include "../task_io_server/task_IoServer.h"
#include "../task_io_server/vdp_IoServer_Data.h"
#include "../task_io_server/vdp_IoServer_State.h"

#include "vdp_net_manange.h"
#include "vdp_net_manange_ip_list.h"

IPG_LIST_ARRAY	ipg_list;

// ����ipg list�Ĳ���
// 1��������е��б�
// 2�����뱾����ip��mac���б�ĵ�1��λ��
// 3�����͹㲥�����������������������ipg�豸����ip��mac������

void ipg_list_update_initial(void)
{
	ipg_list.node[0].attr = IP_LIST_ATTR_BOTH_OK;
	GetLocalIpStr(ipg_list.node[0].ip);
	GetLocalMacStr(ipg_list.node[0].mac);	
	ipg_list.number = 1;
}

// ���һ��ipg�ڵ��ip��mac�Ƿ��ظ�
// return: 0/ok, 1/ip�ظ���2/mac�ظ���3/ip&mac���ظ�
int ipg_list_check_one_node(ipg_list_node* pnode)
{	
	int i,ret_ip, ret_mac,result;

	result = IP_LIST_ATTR_BOTH_OK;
		
	for( i = 0; i < ipg_list.number; i++ )
	{
		//�Ƚ�ip string
		ret_ip 	= strcmp(pnode->ip, ipg_list.node[i].ip );
		ret_mac 	= strcmp(pnode->mac, ipg_list.node[i].mac );
		
		if( (ret_ip == 0) && (ret_mac == 0) )
		{
			result = IP_LIST_ATTR_BOTH_REPEAT;
			pnode->attr	= result;
			ipg_list.node[i].attr = result;
			break;
		}
		else if( ret_ip == 0 )
		{
			result = IP_LIST_ATTR_IP_REPEAT;
			pnode->attr	= result;
			ipg_list.node[i].attr = result;
			break;
		}
		else if( ret_mac == 0 )
		{
			result = IP_LIST_ATTR_MAC_REPEAT;
			pnode->attr	= result;
			ipg_list.node[i].attr = result;
			break;
		}
		else			
			continue;
	}
	return result;
}

// ����һ��ipg�ڵ�
void ipg_list_add_one_node(ipg_list_node* pnode)
{
	int node_attr; 
	
	node_attr = ipg_list_check_one_node(pnode);
	
	if( node_attr != IP_LIST_ATTR_BOTH_REPEAT )
	{
		memcpy( (char*)&ipg_list.node[ipg_list.number], (char*)pnode, sizeof(ipg_list_node) );

		dprintf("--one new node ok, num=%d,attr=%d,ip=%s,mac=%s \n",ipg_list.number, ipg_list.node[ipg_list.number].attr,ipg_list.node[ipg_list.number].ip,ipg_list.node[ipg_list.number].mac);
		// ��Ҫ���ǵ�������ip��mac
		if( ++ipg_list.number >= IP_LIST_NUMBER_MAX ) 	ipg_list.number = 1;

	}
	else
	{
		dprintf("--one new node er, num=%d,attr=%d,ip=%s,mac=%s \n",ipg_list.number, node_attr, pnode->ip, pnode->mac);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//����ipg�б����
int API_IPG_List_Update_Request( unsigned char msg_target_id, unsigned char  msg_source_id, unsigned char msg_type, unsigned char sub_type )
{
	if( ipg_list.asker.state != IP_LIST_UPDATE_STATE_BUSY )
	{
		dprintf("--ipg list update start------- \n");
		
		// ��ʼ������
		ipg_list_update_initial();
	
		ipg_list.asker.state = IP_LIST_UPDATE_STATE_BUSY;
		
		//���������ߵ���Ϣ
		ipg_list.asker.head.msg_target_id 	= msg_target_id;
		ipg_list.asker.head.msg_source_id	= msg_source_id;
		ipg_list.asker.head.msg_type		= msg_type;
		ipg_list.asker.head.msg_sub_type	= sub_type;
		ipg_list.asker.ip					= GetLocalIp();
		//���͵�udp�㲥����
		api_udp_c5_ipc_send_data( htonl(INADDR_BROADCAST), CMD_NET_MANAGE_REQ, (char*)&ipg_list.asker, sizeof(NetManange_ipg_list_update) );

		//������ʱ��ʱ��
		sleep(2);
		API_IPG_List_Update_Over();
		
		return 0;
	}
	else
		return -1;
}


//�ظ�ipg�б����
int API_IPG_List_Update_Reply( int ask_ip, unsigned char msg_target_id, unsigned char  msg_source_id, unsigned char msg_type, unsigned char sub_type )
{
	int ip;
	NetManange_ipg_list_report reporter;

	memcpy( (char*)&reporter.head, (char*)&ipg_list.asker.head, sizeof(VDP_MSG_HEAD) );
	
	reporter.head.msg_target_id		= msg_target_id;
	reporter.head.msg_source_id	= msg_source_id;
	reporter.head.msg_type			= (msg_type|COMMON_RESPONSE_BIT);
	reporter.head.msg_sub_type		= sub_type;
	reporter.asker					= ask_ip;

	ip = GetLocalIpStr(reporter.ip);
	GetLocalMacStr(reporter.mac);

	//ip >>= 24;
	usleep( (ip&0xff)*1000);	//delay a while
		
	//���͵�udp��ָ��ip
	api_udp_c5_ipc_send_data( reporter.asker, CMD_NET_MANAGE_RSP, (char*)&reporter, sizeof(NetManange_ipg_list_report) );	
	
	return 0;
	
}

//�õ�һ���ڵ�
int API_IPG_List_Update_Get_Node( NetManange_ipg_list_report* pOneNode )
{
	ipg_list_node node;
	
	memcpy( node.ip, 	pOneNode->ip, 	IPG_IP_STR_BUF_LEN );
	memcpy( node.mac, 	pOneNode->mac,	IPG_MAC_STR_BUF_LEN );
	node.attr	= 0;
	
	ipg_list_add_one_node( &node );
		
	return 0;
}

//ipg�б�������
void API_IPG_List_Update_Over( void )
{
	//callback for net manange object	
	dprintf("--ipg list update over------- \n");	
}

// �õ�ipglist���ܹ�����
int API_IPG_List_Report_Max_Number( void )
{
	return ipg_list.number;
}

//�õ�ipg list �Ľڵ�����
int API_IPG_List_Report_Data( int start, int number, ipg_list_node* pnodes )
{
	int num_temp,i;	
	if( start >= ipg_list.number )
		return -1;

	num_temp = ipg_list.number - start;
	if( number > num_temp ) number = num_temp;

	for( i = 0; i < number; i++ )
	{
		memcpy( (char*)&pnodes[i], (char*)&ipg_list.node[start+i], sizeof(ipg_list_node) );
		dprintf("copy node: attr=%d,ip=%s,mac=%d \n", pnodes[i].attr, pnodes[i].ip, pnodes[i].mac);
	}
	return number;
}


