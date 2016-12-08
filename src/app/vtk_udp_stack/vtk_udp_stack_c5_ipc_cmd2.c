
#include "vtk_udp_stack_c5_ipc_cmd.h"
#include "vtk_udp_stack_c5_ipc_cmd2.h"

c5_ipc2_instance_t		one_c5_ipc2_ins;

Loop_vdp_common_buffer	c5_ipc2_msg_queue;
pthread_t				c5_ipc2_process_pid;
void* 					c5_ipc2_msg_process( void* arg );
int 					c5_ipc2_inner_recv_anaylasis( char* buf, int len );

int c5_ipc2_udp_recv_anaylasis( char* buf, int len );

// udpͨ�ų�ʼ��
int init_c5_ipc2_instance( void )
{
	// init state
	one_c5_ipc2_ins.send_cmd_sn 	= 0;	
	// init udp
	init_one_udp_comm_rt_buff( &one_c5_ipc2_ins.udp, 500,	c5_ipc2_udp_recv_anaylasis, 500, NULL );
	init_one_udp_comm_rt_type( &one_c5_ipc2_ins.udp, "c5_ipc2", UDP_RT_TYPE_UNICAST,C5_IPC2_CMD_RCV_PORT, C5_IPC2_CMD_TRS_PORT,NULL );	

	// init business rsp wait array
	init_one_send_array(&one_c5_ipc2_ins.waitrsp_array);
 	init_vdp_common_queue(&c5_ipc2_msg_queue,100,(msg_process)c5_ipc2_inner_recv_anaylasis,NULL);	
 	if( pthread_create(&c5_ipc2_process_pid, 0, c5_ipc2_msg_process, &c5_ipc2_msg_queue) )
	{
		c5_ipc2_printf("Create task thread Failure,%s\n", strerror(errno));
	}
	
	c5_ipc2_printf("init_one_c5_ipc ok!\n");	

	return 0;
}

#define C5_IPC2_POLLING_MS		200		// lzh_20161022  // 100
void* c5_ipc2_msg_process(void* arg )
{
	p_Loop_vdp_common_buffer	pmsgqueue 	= (p_Loop_vdp_common_buffer)arg;
	p_vdp_common_buffer pdb 	= 0;

	while( 1 )
	{	
		int size;
		size = pop_vdp_common_queue( pmsgqueue,&pdb,C5_IPC2_POLLING_MS );
		if( size > 0 )
		{
			(*pmsgqueue->process)(pdb,size);
			purge_vdp_common_queue( pmsgqueue );
		}
		// 100ms��ʱ��ѯ
		poll_all_business_recv_array( &one_c5_ipc2_ins.waitrsp_array, C5_IPC2_POLLING_MS );
	}	
	return 0;	
}
// inner��������ҵ�����
int c5_ipc2_inner_recv_anaylasis( char* buf, int len )
{
	return 0;
}

// udp��������
//czn_20160422
int c5_ipc2_udp_recv_anaylasis( char* buf, int len )
{	
	// lzh_20160811_s
	// ��ͷ��Ҫȥ��
	baony_head* pbaony_head = (baony_head*)buf;
	if( pbaony_head->type != IP8210_CMD_TYPE )
	{
		//lzh_20160901_s
		pack*pbuf = (pack*)buf;
		//�ⲿ�豸��ѯ���ӱ��� - A1 + 96 + LEN(8) + 5A + IP(4)		
		if( pbaony_head->type == IP_LINK )
		{
			pack	rsp_pack;			
			rsp_pack.head 		= pbuf->head;
			rsp_pack.head.type	= IP_LINK_RECEIPT;
			rsp_pack.target.ip	= pbuf->target.ip;			
			one_udp_comm_trs_direct(&one_c5_ipc2_ins.udp,pbuf->target.ip,(char*)&rsp_pack,sizeof(baony_head)+sizeof(int));		
		}
		//���������ѯ�ⲿ�豸���ⲿ������Ӧ��
		else if( pbaony_head->type == IP_LINK_RECEIPT )
		{
		}														
		//��������͸����ҵ��
		else
		{
			api_udp_transfer2_recv_callback( buf, len );
		}
		//lzh_20160901_e
		return 0;
	}
	len -= sizeof(baony_head);
	buf += sizeof(baony_head);
	// lzh_20160811_e

	target_head	*ptarget = (target_head*)buf;
	
	// 1��ƥ�䵽ҵ��Ӧ������ݰ�ͨ�������ź�ͬ��������ĵȴ�������
	// 2��δƥ�䵽ҵ��Ӧ������ݰ����͸�ϵͳsurvey����
	if( trig_one_business_recv_array( &one_c5_ipc2_ins.waitrsp_array, ptarget->id, ptarget->cmd,buf+sizeof(target_head), len-sizeof(target_head) ) < 0)
	{
		api_udp_c5_ipc2_recv_callback( ptarget->ip, ptarget->cmd, ptarget->id, buf+sizeof(target_head), len-sizeof(target_head) );
	}
	return 0;
}

// ͨ��udp:25000�˿ڷ������ݣ����ȴ�ҵ��Ӧ��
int api_udp_c5_ipc2_send_data( int target_ip, int cmd, const char* pbuf, unsigned int len )
{

	// ��������
	sem_t *presend_sem = one_udp_comm_trs_api( &one_c5_ipc2_ins.udp, target_ip, cmd, ++one_c5_ipc2_ins.send_cmd_sn, 1, (char*)pbuf, len);

	// �ȴ�ͨ��Ӧ��
	if(presend_sem > 0)
	{
		if( sem_wait_ex2(presend_sem, ACK_RESPONSE_TIMEOUT) != 0 )
		{
			return -1;
		}
	}
	return 0;
}

// ͨ��udp:25000�˿ڷ������ݰ��󣬲��ȴ�ҵ��Ӧ�𣬵õ�ҵ��Ӧ�������
int api_udp_c5_ipc2_send_req(int target_ip, int cmd, char* psbuf, int slen , char *prbuf, unsigned int *prlen)
{	
	int send_cmd_sn = ++one_c5_ipc2_ins.send_cmd_sn;
	
	// ����ȴ�ҵ��Ӧ�����	 - �ȴ����յ���������к���Ҫ�ͷ��͵���������к�����ϵ
	sem_t *pwaitrsp_sem = join_one_business_recv_array( &one_c5_ipc2_ins.waitrsp_array,send_cmd_sn, cmd+0x80,BUSINESS_RESEND_TIME, prbuf, prlen );
	
	// �������ݷ��Ͷ��У��ȴ�ackӦ��
	sem_t *presend_sem = one_udp_comm_trs_api( &one_c5_ipc2_ins.udp, target_ip, cmd, send_cmd_sn, 1, psbuf, slen);

	if( pwaitrsp_sem > 0 && presend_sem > 0 )
	{
		// �ȴ�������ͨ��Ӧ��
		if( sem_wait_ex2(presend_sem, ACK_RESPONSE_TIMEOUT) != 0 )
		{
			dele_one_business_recv_array( &one_c5_ipc2_ins.waitrsp_array, send_cmd_sn, cmd+0x80 );
			bprintf("S_UUDP:NO_COMMU_ACK\n");
			return -1;
		}
		// �ȴ�binuessӦ��	
		if( sem_wait_ex2(pwaitrsp_sem, BUSINESS_WAIT_TIMEPUT+100) != 0 )
		{
			bprintf("S_UUDP:NO_BUSINESS_ACK\n");
			return -1;
		}
	}
	else
	{
		bprintf("S_UUDP:SEM_ERR\n");
		return -1;
	}
	if( prlen == NULL )
		bprintf("S_UUDP:HAVE_BUSINESS_ACK_PKT size %d\n" ,0);
	else
		bprintf("S_UUDP:HAVE_BUSINESS_ACK_PKT size %d\n" ,*prlen);	
	return 0;
}

// ���յ�udp:25000�˿����ݰ��������ҵ��Ӧ��
int api_udp_c5_ipc2_send_rsp( int target_ip, int cmd, int id, const char* pbuf, unsigned int len )
{	
	// ��������
	sem_t *presend_sem = one_udp_comm_trs_api( &one_c5_ipc2_ins.udp, target_ip, cmd, id, 1, (char*)pbuf, len);

	// �ȴ�ͨ��Ӧ��
	if(presend_sem > 0)
	{
		if( sem_wait_ex2(presend_sem, ACK_RESPONSE_TIMEOUT) != 0 )
		{
			return -1;
		}
	}
	return 0;
}

// ���յ�udp:25000�˿����ݰ��Ļص����� - ���͵�survey����
int api_udp_c5_ipc2_recv_callback( int target_ip, int cmd, int sn, char* pbuf, unsigned int len )
{
	UDP_MSG_TYPE				sys_msg;
	sys_msg.head.msg_source_id	= MSG_ID_udp_stack;
	sys_msg.head.msg_target_id	= MSG_ID_survey;
	sys_msg.head.msg_type		= 0;
	sys_msg.head.msg_sub_type	= 0;	

	sys_msg.target_ip			= target_ip;
	sys_msg.cmd					= cmd;
	sys_msg.id					= sn;
	sys_msg.len					= len;
	memcpy( sys_msg.pbuf, pbuf, len );
	API_add_message_to_suvey_queue(	(char*)&sys_msg, sizeof(UDP_MSG_TYPE) - VDP_DAT_BUF_LEN + len ); 	

	return 0;
}

// lzh_20160811_s
// ���յ�udp:25000�˿����ݰ��Ļص����� - ���͵�survey����
int api_udp_transfer2_recv_callback( char* pbuf, unsigned int len )
{
	UDP_MSG_TYPE				sys_msg;
	sys_msg.head.msg_source_id	= MSG_ID_udp_stack;
	sys_msg.head.msg_target_id	= MSG_ID_survey;
	sys_msg.head.msg_type		= 1;
	sys_msg.head.msg_sub_type	= 1;
	
	sys_msg.target_ip			= 0;	
	sys_msg.cmd					= 0;
	sys_msg.id					= 0;
	sys_msg.len					= len;
	memcpy( sys_msg.pbuf, pbuf, len );
	API_add_message_to_suvey_queue(	(char*)&sys_msg, sizeof(UDP_MSG_TYPE) - VDP_DAT_BUF_LEN + len ); 	

	return 0;
}

int api_udp_transfer2_send_data( int target_ip, const char* pbuf, unsigned int len )
{
	one_udp_comm_trs_direct(&one_c5_ipc2_ins.udp,target_ip,pbuf,len);
	return 0;
}

// lzh_20160811_e


