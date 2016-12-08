
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>      /* basic system data types */
#include <error.h>

#include "vtk_udp_stack_class.h"

/****************************************************************************************************************************
 * @fn:		push_vdp_common_queue
 *
 * @brief:	��udpͨ�ö����м�����Ϣ����
 *
 * @param:  	pobj 	- ���ж���
 * @param:  	data 	- ����ָ��
 * @length:  	length 	- ���ݳ���
 *
 * @return: 	0/full, !0/current data pointer
****************************************************************************************************************************/
int push_udp_common_queue( p_loop_udp_common_buffer pobj, p_udp_common_buffer data, int length)
{
	return OS_Q_Put (&pobj->embosQ, data, length);	//0: ok, 1: full
}

/****************************************************************************************************************************
 * @fn:		pop_udp_common_queue
 *
 * @brief:	�Ӷ����еõ�����ָ��
 *
 * @param:  	pobj 		- ���ж���
 * @param:  	pdb 		- ����ָ��
 * @param:  	timeout 	- ��ʱʱ�䣬msΪ��λ
 *
 * @return: 	size of data
****************************************************************************************************************************/
int pop_udp_common_queue(p_loop_udp_common_buffer pobj, p_udp_common_buffer* pdb, int timeout)
{
	if( !timeout )
		return OS_Q_GetPtr( &pobj->embosQ,  (void*)pdb );
	else
		return OS_Q_GetPtrTimed( &pobj->embosQ, (void*)pdb, timeout );
}

/****************************************************************************************************************************
 * @fn:		purge_udp_common_queue
 *
 * @brief:	�������Ķ���
 *
 * @param:  	pobj 		- ���ж���
 *
 * @return: 	size of data
****************************************************************************************************************************/
int purge_udp_common_queue(p_loop_udp_common_buffer pobj)
{
	OS_Q_Purge( &pobj->embosQ );
	return 0;
}

/****************************************************************************************************************************
 * @fn:		init_udp_common_queue
 *
 * @brief:	��ʼ��udpͨ�����ݶ���
 *
 * @param:  	pobj 			- ���ж���
 * @param:  	qsize 			- ���д�С
 * @param:  	powner 			- ���е�ӵ����
 * @param:	process		   	- ���е����ݴ�����
 				
 *
 * @return: 	0/ok, -1/err
****************************************************************************************************************************/
int init_udp_common_queue(p_loop_udp_common_buffer pobj, int qsize, udp_msg_process process )
{
	// create embOS queue
	pobj->QSize		= qsize;
	pobj->pQBuf		= (unsigned char*)malloc(qsize);
	OS_Q_Create( &pobj->embosQ, pobj->pQBuf, qsize );

	pobj->process	= process;

	if( pobj->pQBuf == NULL ) udp_printf("malloc err!\n"); //else printf("malloc addr = 0x%08x\n",pobj->pQBuf);
		
	return 0;
}

/****************************************************************************************************************************
 * @fn:		exit_udp_common_queue
 *
 * @brief:	�ͷ�һ������
 *
 * @param:  	pobj 			- ���ж��� 
 *
 * @return: 	0/ok, -1/err
****************************************************************************************************************************/
int exit_udp_common_queue( p_loop_udp_common_buffer pobj )
{
	free( pobj->pQBuf );
	return 0;
}

/****************************************************************************************************************************
udp �շ���������̷߳������
****************************************************************************************************************************/
int send_one_udp_data( int sock_fd, struct sockaddr_in sock_target_addr, char *data, int length)
{
	int ret;

	ret = sendto(sock_fd, data, length, 0, (struct sockaddr*)&sock_target_addr,sizeof(struct sockaddr_in));
	
	if( ret == -1 )
	{
		udp_printf("can not send data from socket! errno:%d,means:%s\n",errno,strerror(errno));
		return -1;
	}
	return ret;
}

// lzh_20160811_s
// ֱ�ӷ���һ�����ݰ�
int one_udp_comm_trs_direct( udp_comm_rt* pins, int target_ip, char* pbuf, int len )
{
	struct sockaddr_in trs_tar_addr;
	
	trs_tar_addr.sin_family 		= AF_INET;
	trs_tar_addr.sin_port			= pins->trs_tar_addr.sin_port;
	trs_tar_addr.sin_addr.s_addr	= target_ip;	// �õ�Ŀ��ip��ַ	
	
	send_one_udp_data( pins->sock_trs_fd, trs_tar_addr, pbuf, len);

	return 0;
}

// lzh_20160811_e

char get_pack_checksum( char* pbuf, int len )
{
	int 	i;
	char	checksum = 0;
	for( i = 0; i < len; i++ )
	{
		checksum += pbuf[i];
	}
	return checksum;
}

// checksum�ķ�Χ: ������ head��target.ip, ��target.cmd��ʼ���ۼӺͣ�checksumΪ���һ���ֽڣ�����Ϊ����checksum�ڵ������ֽ�
int create_one_send_pack( udp_comm_rt* pins, int target_ip, int cmd_type, int cmd, int id, char* pbuf, int len )
{
	pack 	send_pack;
	char	checksum;
			
	send_pack.head.start	= IP8210_CMD_START;
	send_pack.head.flag		= IP8210_CMD_FLAG;
	send_pack.head.type		= cmd_type;
	send_pack.head.len		= sizeof(baony_head) + sizeof(target_head) + len + 1; // include checksum
	
	send_pack.target.ip		= target_ip;
	send_pack.target.cmd	= cmd;
	send_pack.target.id		= id;
	memcpy( send_pack.dat, pbuf, len );

	// lzh_20160303_s	Ϊ����C5-IPC������ʱ��Ҫ��cmd��idת��Ϊ�����ֽ���
	send_pack.target.cmd	= htons(send_pack.target.cmd);
	//send_pack.target.id		= htons(send_pack.target.id);
	// lzh_20160303_e
	
	checksum = get_pack_checksum( (char*)&send_pack + CHECKSUM_OFFSET_LEN, send_pack.head.len - CHECKSUM_OFFSET_LEN - 1 );
	
	send_pack.dat[len]		= checksum;

	struct sockaddr_in trs_tar_addr;
	trs_tar_addr.sin_family 		= AF_INET;
	trs_tar_addr.sin_port			= pins->trs_tar_addr.sin_port;
	trs_tar_addr.sin_addr.s_addr	= send_pack.target.ip;	// �õ�Ŀ��ip��ַ	
	send_one_udp_data( pins->sock_trs_fd, trs_tar_addr, (char*)&send_pack,send_pack.head.len);

	udp_printf("send_one_udp_data: socket = %d,ip = 0x%08x, cmd = 0x%04x, id = %d, len = %d\n", pins->sock_trs_fd,send_pack.target.ip, send_pack.target.cmd, send_pack.target.id, send_pack.head.len );
	
	return send_pack.head.len;
}

sem_t* one_udp_comm_trs_api( udp_comm_rt* pins, int target_ip, int cmd, int id, int ack, char* ptrs_buf,  int ptrs_len )
{	
	sem_t *presend_sem = NULL;	
	if( pins->tmsg_buf.pQBuf == NULL )
	{
		udp_printf("send fail:tmsg_buf.pQBuf is NULL\n");
	}
	else
	{		
		if( ptrs_len > IP8210_BUFFER_MAX ) ptrs_len = IP8210_BUFFER_MAX;

		pack_buf	send_pack;
				
		send_pack.target.ip 	= target_ip;
		send_pack.target.cmd	= cmd;
		send_pack.target.id 	= id;
		send_pack.len			= ptrs_len;
		memcpy( send_pack.dat, ptrs_buf, ptrs_len );
		
		if( ack )
		{
			// �����ط�����
			presend_sem = join_one_send_array(&pins->resend_array,send_pack.target.id,ACK,ACK_RESPONSE_TIMEOUT,ack,(char*)&send_pack, sizeof(pack_buf)-IP8210_BUFFER_MAX + ptrs_len);
			if( presend_sem <= 0 )
			{
				udp_printf("join_one_send_array err...\n");		
			}
		}

		// ���뷢�Ͷ���
		push_udp_common_queue( &pins->tmsg_buf, (char*)&send_pack, sizeof(pack_buf)-IP8210_BUFFER_MAX + ptrs_len );
		
	}
	return presend_sem;
}
	

void *udp_trs_data_task_thread(void *arg)
{
	int size;
	udp_comm_rt* pins = (udp_comm_rt*)arg;

	p_udp_common_buffer pdb;
	
	pins->tmsg_run_flag = 1;
			
	udp_printf("udp_trs_data_task_thread = %s\n",pins->pname );
		
	do
	{
		size = pop_udp_common_queue( &pins->tmsg_buf, &pdb, 0);
		if( size > 0 )
		{
			// send process
			pack_buf* ppack_buf = (pack_buf*)pdb;
			create_one_send_pack( pins, ppack_buf->target.ip, IP8210_CMD_TYPE,ppack_buf->target.cmd, ppack_buf->target.id, ppack_buf->dat, ppack_buf->len );
			// clear 
			purge_udp_common_queue( &pins->tmsg_buf );
		}	
	}
	while(pins->tmsg_run_flag);

	udp_printf("udp_trs_data_task_thread exit...!\n"); 

	return (void *)0;
}

#define RCV_BUFFER_MAX	1000
#define RCV_TIMEOUT_GAP	100		//ms
void *udp_rcv_data_task_thread(void *arg)
{
	int recvcnt;
	udp_comm_rt* pins = (udp_comm_rt*)arg;

	unsigned int addr_len = sizeof(struct sockaddr_in);
	char buffer[RCV_BUFFER_MAX];

	fd_set fds;
	struct timeval tv={1,0};
	int ret;
	
	pins->rcv_run_flag = 1;
	
	udp_printf("udp_rcv_data_task_thread = %s, socket = %d\n",pins->pname, pins->sock_rcv_fd );

	do
	{
		FD_ZERO(&fds);
		FD_SET(pins->sock_rcv_fd,&fds);
		
		tv.tv_sec = 0;
		tv.tv_usec = RCV_TIMEOUT_GAP*1000;	// 100ms
		ret = select( pins->sock_rcv_fd + 1, &fds, NULL, NULL, &tv );

		switch( ret )
		{
			case -1:
				udp_printf("socket err!\n");
				goto udp_rcv_data_task_thread_error;
			case 0:
				//timeout
				break;
			default:
				if( FD_ISSET( pins->sock_rcv_fd, &fds ) )
				{
					recvcnt = recvfrom(pins->sock_rcv_fd, buffer, RCV_BUFFER_MAX,0, (struct sockaddr*)&pins->rcv_tar_addr,&addr_len);
					
					if( recvcnt == -1 )
					{
						udp_printf("can not rec data from socket! errno:%d,means:%s\n",errno,strerror(errno));
					}
					else
					{
						pack*	precv_pack = (pack*)buffer;
						
						udp_printf("recvfrom remote ip = 0x%08x,port = %d, cmd=0x%04x,id=%d,len = %d!\n",htonl(pins->rcv_tar_addr.sin_addr.s_addr),htons(pins->rcv_tar_addr.sin_port),precv_pack->target.cmd,precv_pack->target.id, recvcnt);

						// �ж��Ƿ��Լ����͵�����
						if( (pins->rcv_tar_addr.sin_addr.s_addr != pins->local_addr) && (precv_pack->head.start == IP8210_CMD_START) )
						{
							//�����������
							if( precv_pack->head.type == IP8210_CMD_TYPE )
							{
								// ��Ч���ݰ����� = �������ݰ����� - ��ͷ�ͣɣе�ַ������У��ͣ��ֽ�							
								int dat_len;
								int dat_cks;
								dat_len = recvcnt-CHECKSUM_OFFSET_LEN-1;
								dat_cks = get_pack_checksum(buffer+CHECKSUM_OFFSET_LEN, dat_len);
								// ���ݰ�����չ�����򲻼���checksum
								if( dat_len && (dat_cks == buffer[recvcnt-1]) )
								{							
									udp_printf("recvfrom remote ip = 0x%08x,port = %d, len = %d,checksum ok!\n",htonl(pins->rcv_tar_addr.sin_addr.s_addr),htons(pins->rcv_tar_addr.sin_port),recvcnt);

															
									// lzh_20160303_s	Ϊ����C5-IPC������ʱ��Ҫ��cmd��id�����ֽ���ת��ΪС����ʾ��ʽ
									precv_pack->target.cmd	= htons(precv_pack->target.cmd);
									//precv_pack->target.id = htons(precv_pack->target.id);
									// lzh_20160303_e
								
									//udp_printf("recv data process...!\n");
									
									// Ӧ��������� 
									if( precv_pack->target.cmd == ACK )
									{
										if( trig_one_send_array( &pins->resend_array, precv_pack->target.id, precv_pack->target.cmd ) < 0 )
										{
											udp_printf("recv one vd_ack command err,cmd=0x%04x,id=%d\n",precv_pack->target.cmd,precv_pack->target.id);
										}
										else
										{
											udp_printf("recv one vd_ack command ok,cmd=0x%04x,id=%d\n",precv_pack->target.cmd,precv_pack->target.id);
										}
									}
									else
									{
										udp_printf("recv one vd_req command ok,cmd=0x%04x,id=%d\n",precv_pack->target.cmd,precv_pack->target.id);

										// send process
										create_one_send_pack( pins, pins->rcv_tar_addr.sin_addr.s_addr, IP8210_CMD_TYPE, ACK, precv_pack->target.id, NULL, 0 );
										
										udp_printf("send one vd_ack command,id=%d\n",precv_pack->target.id);
										
										// ҵ���� - ȫ�����ݰ��ϴ�(�����ٶ�����ͷ)
										if( pins->rmsg_buf.pQBuf != NULL )
										{
											udp_printf("push_udp_common_queue ok,len=%d\n",recvcnt);
											precv_pack->target.ip = pins->rcv_tar_addr.sin_addr.s_addr;											
											push_udp_common_queue( &pins->rmsg_buf, buffer,recvcnt);
										}
										else
										{
											udp_printf("push_udp_common_queue er,len=%d\n",recvcnt);									
										}
									}
								}
							}
							//����ת������ - �ϴ���ҵ����(��Ҫ�������ݰ��İ�ͷ������)
							else
							{
								// ҵ����
								if( pins->rmsg_buf.pQBuf != NULL )
								{
									udp_printf("push_udp_common_queue ok,len=%d\n",recvcnt);
							
									precv_pack->target.ip = pins->rcv_tar_addr.sin_addr.s_addr; 										
									
									push_udp_common_queue( &pins->rmsg_buf, buffer,recvcnt);
								}
								else
								{
									udp_printf("push_udp_common_queue er,len=%d\n",recvcnt);								
								}
							}							
						}
					}					
				}
				break;
		}
		// polling resend array
		poll_all_send_array( pins, RCV_TIMEOUT_GAP );
	}
	while(pins->rcv_run_flag);

	udp_printf("udp_rcv_data_task_thread exit...!\n");	

	return (void *)0;
	
	udp_rcv_data_task_thread_error:

	return (void *)-1;
}

void *udp_rcv_data_process_task_thread(void *arg)
{
	int size;
	udp_comm_rt* pins = (udp_comm_rt*)arg;

	p_udp_common_buffer pdb;
	
	pins->rmsg_run_flag = 1;
			
	udp_printf("udp_rcv_data_process_task_thread = %s\n",pins->pname );
		
	do
	{
		size = pop_udp_common_queue( &pins->rmsg_buf, &pdb, RCV_TIMEOUT_GAP);
		if( size > 0 )
		{
			(*pins->rmsg_buf.process)(pdb,size);
			purge_udp_common_queue( &pins->rmsg_buf );
		}	
	}
	while(pins->rmsg_run_flag);

	udp_printf("udp_rcv_data_process_task_thread exit...!\n"); 

	return (void *)0;
}
	
/*
 ���ܣ�
	��ʼ��һ��udpʵ���Ļ�����д�С����Ϣ����ص�����������СΪ0�����޻���
 ������
	pins				- udp �շ�ʵ��ָ��
	rmsg_qsize		- udp �������ݻ����С
	rmsg_process			- udp �������ݴ�����ָ��
	tmsg_qsize		- udp �������ݻ����С
	tsyn_qsize		- udp ����ͬ�������С
	tmsg_process			- udp �������ݴ�����ָ��
*/
int init_one_udp_comm_rt_buff( udp_comm_rt* pins, int rmsg_qsize, udp_msg_process rmsg_process,int tmsg_qsize, udp_msg_process tmsg_process )
{
	init_udp_common_queue(&(pins->rmsg_buf), rmsg_qsize, rmsg_process);
	init_udp_common_queue(&(pins->tmsg_buf), tmsg_qsize, tmsg_process);

	return 0;	
}

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
int init_one_udp_comm_rt_type( udp_comm_rt* pins,  char* pname, int type, unsigned short rport, unsigned short tport, char* target_pstr)
{
	// �õ�������ַ
	pins->local_addr 	= GetLocalIp();
	// global para initial
	pins->pname			= pname;
	pins->rport			= rport;
	pins->tport			= tport;
	pins->type			= type;
	pins->target_pstr	= target_pstr;
	
	// trs initial
	pins->sock_trs_fd = create_trs_udp_socket();
	if( pins->sock_trs_fd == -1 )
	{
		udp_printf("create_trs_udp_socket Failure,%s\n", strerror(errno));
		return -1;		
	}
	
	if( pins->type == UDP_RT_TYPE_BROADCAST	)	// �����׽ӿڴ��͹㲥��Ϣ
	{
    	int so_broadcast = 1;	
	    if( setsockopt( pins->sock_trs_fd, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast) ) == -1 )
		{
			udp_printf("setsockopt failure!msg:%s\n", strerror(errno));
			return -1;		
		}
	}
	
	pins->tmsg_run_flag	= 0;
	if( pins->tmsg_buf.pQBuf != NULL )
	{
		if( pthread_create(&pins->tmsg_pid, 0, udp_trs_data_task_thread, pins) )
		{
			udp_printf("Create udp_trs_data_task_thread Failure,%s\n", strerror(errno));
			return -1;
		}
	}	
	// rcv initial
	if( (pins->sock_rcv_fd = create_rcv_udp_socket(NET_DEVICE_NAME,pins->rport,0))  == -1 )
	{
		udp_printf("create_rcv_udp_socket failure!msg:%s\n", strerror(errno));
		return -1;
	}

	if( pins->type == UDP_RT_TYPE_MULTICAST && pins->target_pstr != NULL )
	{
		if( join_multicast_group(NET_DEVICE_NAME, pins->sock_rcv_fd, inet_addr(pins->target_pstr) ) == -1 )
		{
			udp_printf("add multicast group failure!msg:%s\n", strerror(errno));
			return -1;
		}		
	}
	pins->trs_tar_addr.sin_family 		= AF_INET;  
	pins->trs_tar_addr.sin_addr.s_addr 	= inet_addr(pins->target_pstr);
	pins->trs_tar_addr.sin_port 		= htons( pins->tport );		

	pins->rcv_run_flag 	= 0;
	if( pthread_create(&pins->rcv_pid, 0, udp_rcv_data_task_thread, pins) )
	{
		udp_printf("Create udp_rcv_data_task_thread Failure,%s\n", strerror(errno));
		return -1;
	}
	
	pins->rmsg_run_flag	= 0;
	if( pins->rmsg_buf.pQBuf != NULL )
	{
		if( pthread_create(&pins->rmsg_pid, 0, udp_rcv_data_process_task_thread, pins) )
		{
			udp_printf("Create udp_rcv_data_process_task_thread Failure,%s\n", strerror(errno));
			return -1;
		}		
	}	

	// ��ʼ���ط�����
	init_one_send_array(&pins->resend_array);	
	return 0;
}

/*
 ���ܣ�
	����ʼ��һ��udpʵ��
 ������
 	pins 		- udp �շ�ʵ��ָ��
*/
int deinit_one_udp_comm_rt( udp_comm_rt* pins)
{
	pins->rcv_run_flag 	= 0;
	pins->rmsg_run_flag	= 0;
	pins->tmsg_run_flag	= 0;
		
	if( pins->tmsg_buf.pQBuf )
		exit_udp_common_queue(&pins->tmsg_buf);
	if( pins->rmsg_buf.pQBuf )
		exit_udp_common_queue(&(pins->rmsg_buf));
	return 0;
}

/*
 ���ܣ�
	udpʵ�������ݽӷ�������API����
 ������
 	pins 		- udp �շ�ʵ��ָ��
 	ptrs_buf		- udp ��������ָ�� : target_ip + cmd + id
 	ptrs_len		- udp �������ݳ���
 ���أ�
	-1/err��0/ok
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��ʼ�����øýӿ�
int init_one_send_array( send_sem_id_array* psendarray )
{
	int i;
	
	usleep(100*1000);
	
	pthread_mutex_init( &psendarray->lock, 0);
		
	for( i = 0; i < MAXf_SEND_SEM_ID_CNT; i++ )
	{
		psendarray->dat[i].enable			= 0;
		psendarray->dat[i].send_id			= 0;		
		psendarray->dat[i].send_cmd			= 0;		
		psendarray->dat[i].send_timeout		= 0;		
		psendarray->dat[i].send_timeout_cnt	= 0;		
		psendarray->dat[i].resend_times		= 0;
		psendarray->dat[i].len				= 0;
		psendarray->dat[i].pbuffer 			= NULL;
		if( sem_init(&psendarray->dat[i].trig_sem,0,0) == -1 )
		{
			udp_printf("init_one_send_array fail...\n");
		}
		//udp_printf("i=%d: enable=%d,cmd=%d,id=%d,sem_post = %d\n",i,psendarray->dat[i].enable,psendarray->dat[i].send_cmd,psendarray->dat[i].send_id,(int)&psendarray->dat[i].trig_sem);
	}	

	usleep(100*1000);

	return 0;
}

// ���͵��øýӿ�
sem_t* join_one_send_array( send_sem_id_array* psendarray, unsigned short send_id, unsigned short send_cmd, int send_timeout, int resend_times, char *pbuffer, int len )
{
	int i;
	sem_t *psem;

	pthread_mutex_lock( &psendarray->lock );

	// �鿴�Ƿ�������е�send_id��send_cmd
	for( i = 0; i < MAXf_SEND_SEM_ID_CNT; i++ )
	{
		if( psendarray->dat[i].enable )
		{
			if( (psendarray->dat[i].send_id == send_id) && (psendarray->dat[i].send_cmd == send_cmd) )
			{
				udp_printf("join one cmd=%d,id=%d,sem=%d fail = redo\n",send_cmd,send_id,(int)psem);					
				pthread_mutex_unlock( &psendarray->lock );
				return (sem_t*)-1;
			}
		}
	}		
	// ���ҿ���λ��
	for( i = 0; i < MAXf_SEND_SEM_ID_CNT; i++ )
	{
		if( psendarray->dat[i].enable == 0 )
		{
			psendarray->dat[i].enable			= 1;
			psendarray->dat[i].send_id			= send_id;
			psendarray->dat[i].send_cmd 		= send_cmd;		
			psendarray->dat[i].send_timeout 	= send_timeout;	
			psendarray->dat[i].resend_times		= resend_times;
			psendarray->dat[i].send_timeout_cnt = 0;
			
			sem_init(&psendarray->dat[i].trig_sem,0,0);
			
			psendarray->dat[i].len			= len;
			psendarray->dat[i].pbuffer		= (char*)malloc(len);
			memcpy( psendarray->dat[i].pbuffer, pbuffer, len );
			
			psem = &psendarray->dat[i].trig_sem;

			udp_printf("join one cmd=%d,id=%d,sem=%d...\n",send_cmd,send_id,(int)psem);					
			
			pthread_mutex_unlock( &psendarray->lock );
			return psem;
		}
	}
	pthread_mutex_unlock( &psendarray->lock );
	return (sem_t*)0;	
}


// ���յ��øýӿ�
int trig_one_send_array( send_sem_id_array* psendarray, unsigned short recv_id, unsigned short recv_cmd )
{
	int i;
	pthread_mutex_lock( &psendarray->lock );

	//udp_printf("for match one cmd=%d,id=%d...\n",recv_cmd, recv_id);					
	
	for( i = 0; i < MAXf_SEND_SEM_ID_CNT; i++ )
	{
		//udp_printf("i=%d: enable=%d,cmd=%d,id=%d,sem_post = %d\n",i,psendarray->dat[i].enable,psendarray->dat[i].send_cmd,psendarray->dat[i].send_id,(int)&psendarray->dat[i].trig_sem);
		
		if( psendarray->dat[i].enable && (psendarray->dat[i].send_id == recv_id) && (psendarray->dat[i].send_cmd == recv_cmd) )
		{			
			psendarray->dat[i].enable		= 0;
			psendarray->dat[i].send_id		= 0;		
			psendarray->dat[i].send_cmd 	= 0;		
			psendarray->dat[i].send_timeout = 0;		
			psendarray->dat[i].resend_times	= 0;
			psendarray->dat[i].len			= 0;
			if( psendarray->dat[i].pbuffer != NULL )
			{
				free(psendarray->dat[i].pbuffer);
				psendarray->dat[i].pbuffer = NULL;
			}
			
			sem_post(&psendarray->dat[i].trig_sem);

			//udp_printf("match one cmd=%d,id=%d,sem_post = %d ...\n",recv_cmd,recv_id,(int)&psendarray->dat[i].trig_sem); 					

			pthread_mutex_unlock( &psendarray->lock );	
			return i;
		}
	}
	pthread_mutex_unlock( &psendarray->lock );
	return -1;	
}

int poll_all_send_array( udp_comm_rt* pins, int time_gap )
{
	int i;
	pthread_mutex_lock( &pins->resend_array.lock );
	for( i = 0; i < MAXf_SEND_SEM_ID_CNT; i++ )
	{
		if( pins->resend_array.dat[i].enable )
		{			
			// ʱ�����
			pins->resend_array.dat[i].send_timeout_cnt += time_gap;
			// ��ʱ�Ƴ��ȴ����Ͷ���
			if( pins->resend_array.dat[i].send_timeout_cnt >= pins->resend_array.dat[i].send_timeout )
			{
				pins->resend_array.dat[i].send_timeout_cnt = 0;
				
				if( pins->resend_array.dat[i].resend_times == 0 )
				{
					udp_printf("sem_post = %d,cmd=0x%04x,id= %d, timeout...\n",(int)&pins->resend_array.dat[i].trig_sem,pins->resend_array.dat[i].send_cmd, pins->resend_array.dat[i].send_id);
					
					pins->resend_array.dat[i].enable		= 0;
					pins->resend_array.dat[i].send_id		= 0;		
					pins->resend_array.dat[i].send_cmd 		= 0;		
					pins->resend_array.dat[i].send_timeout	= 0;	
					pins->resend_array.dat[i].len			= 0;					
					if( pins->resend_array.dat[i].pbuffer != NULL )
					{
						free(pins->resend_array.dat[i].pbuffer);
						pins->resend_array.dat[i].pbuffer = NULL;
					}
					//sem_post(&pins->resend_array.dat[i].trig_sem);			// ���败�����������ѹ������ź����ϣ���ʱ�˳�
				}
				else
				{
					pins->resend_array.dat[i].resend_times--;
					// resend process
					pack_buf*	ppack_buf = (pack_buf*)pins->resend_array.dat[i].pbuffer;
					create_one_send_pack( pins, ppack_buf->target.ip, IP8210_CMD_TYPE, ppack_buf->target.cmd, ppack_buf->target.id, ppack_buf->dat, ppack_buf->len );
				}
			}
		}
	}
	pthread_mutex_unlock( &pins->resend_array.lock );
	return -1;	
}

// �ȴ�ҵ����ն��г�ʱ����
int poll_all_business_recv_array( send_sem_id_array* psendarray, int time_gap )
{
	int i;
	pthread_mutex_lock( &psendarray->lock );
	for( i = 0; i < MAXf_SEND_SEM_ID_CNT; i++ )
	{
		if( psendarray->dat[i].enable )
		{			
			// ʱ�����
			psendarray->dat[i].send_timeout_cnt += time_gap;
			// ��ʱ�Ƴ��ȴ����ն���
			if( psendarray->dat[i].send_timeout_cnt >= psendarray->dat[i].send_timeout )
			{
				psendarray->dat[i].send_timeout_cnt = 0;
				
				if( psendarray->dat[i].resend_times == 0 )
				{
					udp_printf("sem_post = %d,cmd=0x%04x,id= %d, timeout...\n",(int)&psendarray->dat[i].trig_sem,psendarray->dat[i].send_cmd, psendarray->dat[i].send_id);
					
					psendarray->dat[i].enable		= 0;
					psendarray->dat[i].send_id		= 0;		
					psendarray->dat[i].send_cmd 	= 0;		
					psendarray->dat[i].send_timeout = 0;	
					psendarray->dat[i].len			= 0;					
					if( psendarray->dat[i].pbuffer != NULL )
					{
						free(psendarray->dat[i].pbuffer);
						psendarray->dat[i].pbuffer = NULL;
					}
					//sem_post(&psendarray->dat[i].trig_sem);		// ���败�����������ѹ������ź����ϣ���ʱ�˳�
				}
				else
				{
					psendarray->dat[i].resend_times--;
				}
			}
		}
	}
	pthread_mutex_unlock( &psendarray->lock );
	return -1;	
}


//czn_20160422_s
sem_t* join_one_business_recv_array( send_sem_id_array* psendarray, unsigned short send_id, unsigned short send_cmd, int send_timeout, char *prcvbuffer, unsigned int *prcvlen )
{
	int i;
	sem_t *psem;

	pthread_mutex_lock( &psendarray->lock );

	// �鿴�Ƿ�������е�send_id��send_cmd
	for( i = 0; i < MAXf_SEND_SEM_ID_CNT; i++ )
	{
		if( psendarray->dat[i].enable )
		{
			if( (psendarray->dat[i].send_id == send_id) && (psendarray->dat[i].send_cmd == send_cmd) )
			{
				udp_printf("join one cmd=%d,id=%d,sem=%d fail = redo\n",send_cmd,send_id,(int)psem);					
				pthread_mutex_unlock( &psendarray->lock );
				return (sem_t*)-1;
			}
		}
	}		
	// ���ҿ���λ��
	for( i = 0; i < MAXf_SEND_SEM_ID_CNT; i++ )
	{
		if( psendarray->dat[i].enable == 0 )
		{
			psendarray->dat[i].enable			= 1;
			psendarray->dat[i].send_id			= send_id;
			psendarray->dat[i].send_cmd 		= send_cmd;		
			psendarray->dat[i].send_timeout 	= send_timeout;	
			psendarray->dat[i].resend_times		= 0;
			psendarray->dat[i].send_timeout_cnt = 0;
			
			sem_init(&psendarray->dat[i].trig_sem,0,0);
			
			psendarray->dat[i].len				= 0;
			psendarray->dat[i].pbuffer			= NULL;

			psendarray->dat[i].prcvbuffer		= prcvbuffer;
			psendarray->dat[i].prcvlen			= prcvlen;
			
			psem = &psendarray->dat[i].trig_sem;

			udp_printf("join one business ack cmd=%d,id=%d,sem=%d...\n",send_cmd,send_id,(int)psem);	
						
			pthread_mutex_unlock( &psendarray->lock );
			return psem;
		}
	}
	pthread_mutex_unlock( &psendarray->lock );
	return (sem_t*)0;	
}

int trig_one_business_recv_array( send_sem_id_array* psendarray, unsigned short recv_id, unsigned short recv_cmd, char* buf, unsigned int len )
{
	int i;
	pthread_mutex_lock( &psendarray->lock );

	//udp_printf("for match one cmd=%d,id=%d...\n",recv_cmd, recv_id);					
	
	for( i = 0; i < MAXf_SEND_SEM_ID_CNT; i++ )
	{
		//udp_printf("i=%d: enable=%d,cmd=%d,id=%d,sem_post = %d\n",i,psendarray->dat[i].enable,psendarray->dat[i].send_cmd,psendarray->dat[i].send_id,(int)&psendarray->dat[i].trig_sem);
		
		if( psendarray->dat[i].enable && (psendarray->dat[i].send_id == recv_id) && (psendarray->dat[i].send_cmd == recv_cmd) )
		{			
			psendarray->dat[i].enable		= 0;
			psendarray->dat[i].send_id		= 0;		
			psendarray->dat[i].send_cmd 	= 0;		
			psendarray->dat[i].send_timeout = 0;		
			psendarray->dat[i].resend_times	= 0;
			psendarray->dat[i].len			= 0;

			if( psendarray->dat[i].prcvbuffer != NULL && psendarray->dat[i].prcvlen != NULL )
			{				
				// psendarray->dat[i].prcvlen��ʾ�������ݵ���󳤶ȣ�copy���ݲ�ҪԽ��
				unsigned int real_len = *psendarray->dat[i].prcvlen;
				if( real_len > len )
					real_len = len;				
				*psendarray->dat[i].prcvlen = real_len;
				memcpy( psendarray->dat[i].prcvbuffer, buf, real_len );
			}
			
			sem_post(&psendarray->dat[i].trig_sem);
			
			//udp_printf("match one cmd=%d,id=%d,sem_post = %d ...\n",recv_cmd,recv_id,(int)&psendarray->dat[i].trig_sem); 					

			pthread_mutex_unlock( &psendarray->lock );	
			return i;
		}
	}
	pthread_mutex_unlock( &psendarray->lock );
	return -1;	
}

// ��ͨ��Ӧ��ʱɾ��ҵ��Ӧ�������
int dele_one_business_recv_array( send_sem_id_array* psendarray, unsigned short recv_id, unsigned short recv_cmd )
{
	int i;
	pthread_mutex_lock( &psendarray->lock );

	for( i = 0; i < MAXf_SEND_SEM_ID_CNT; i++ )
	{
		if( psendarray->dat[i].enable && (psendarray->dat[i].send_id == recv_id) && (psendarray->dat[i].send_cmd == recv_cmd) )
		{			
			psendarray->dat[i].enable		= 0;
			psendarray->dat[i].send_id		= 0;		
			psendarray->dat[i].send_cmd 	= 0;		
			psendarray->dat[i].send_timeout = 0;		
			psendarray->dat[i].resend_times	= 0;
			psendarray->dat[i].len			= 0;
			pthread_mutex_unlock( &psendarray->lock );	
			return i;
		}
	}
	pthread_mutex_unlock( &psendarray->lock );
	return -1;	
}

//czn_20160422_e
