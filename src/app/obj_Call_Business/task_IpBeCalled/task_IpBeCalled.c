/**
  ******************************************************************************
  * @file    task_BeCalled.c
  * @author  czn
  * @version V00.01.00 (basic on vsip)
  * @date    2014.11.07
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  * <h2><center>&copy; COPYRIGHT 2014 V-Tec</center></h2>
  ******************************************************************************
  */ 
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>

#include "../../vtk_udp_stack/vtk_udp_stack_c5_ipc_cmd.h"

#include "../../task_survey/obj_CallServer_Virtual/obj_CallServer_Virtual.h"
#include "task_IpBeCalled.h"
//#include "task_Caller.h"
#include "obj_IpBeCalled_State.h"
#include "obj_IpBeCalled_Data.h"
#include "../../vdp_uart.h"
//��ʱ_Caller��ʱ����

// lzh_20160127_s
#include "../../video_service/video_object.h"
#include "../../video_service/ip_camera_control/ip_camera_control.h"
#include "../../video_service/ip_video_control/ip_video_control.h"
// lzh_20160127_e
#include "../../../os/OSTIME.h"

OS_TIMER timer_becalled;

BECALLED_RUN BeCalled_Run;
/*------------------------------------------------------------------------
					BeCalled�������ʼ��
------------------------------------------------------------------------*/
void vtk_TaskInit_BeCalled(void)	//R_
{
	//״̬(WAITING == ԭSTANDBY)
	BeCalled_Run.state = BECALLED_WAITING;
	
	//������ʱ(δ����)
	OS_CreateTimer(&timer_becalled, BeCalled_Timer_Callback, 1000/20);
}

/*------------------------------------------------------------------------
			BeCalled_Task_Process: ����״̬����Ϣ���д���
------------------------------------------------------------------------*/
void vtk_TaskProcessEvent_BeCalled(BECALLED_STRUCT  *msg_becalled)	//R_
{
	bprintf("-------------ipbecalled recv cmd %04x\n",msg_becalled->msg_head.msg_type);
	if(msg_becalled->msg_head.msg_type == BECALLED_MSG_GETSTATE)
	{
		Get_BeCalled_State_Rsp(msg_becalled);
	}
	else
	{
		/*uint16 cmd_type = (msg_becalled->msg_type<<8) |msg_becalled->msg_sub_type;
		API_Send_BusinessRsp_ByUart(msg_becalled->source_obj_id,MSG_ID_BECALLED,cmd_type|0x8000,NULL,0);
		if(msg_becalled->msg_type == 0x33)
		{
			
				//if(pb->msg_type == IP_CALL_STATE_1005 && pb->buf[0] == CALL_STATE_ACK)	
			//if( GetLocalIp()&0x01000000 )
			//set_g_video_object_src_attr( VD_LINPHONEC, msg_becalled->sdev.ipdev.ip, NULL, 1, 600, 1 );
			//else
			//set_g_video_object_src_attr( VD_MULTICAST, msg_becalled->sdev.ipdev.ip, NULL, 1, 600, 1 );
			//api_video_client_turn_on();
		}
		if(msg_becalled->msg_type == 0x53)
		{
			api_video_client_turn_off();
		}*/
		switch (BeCalled_Run.state)
		{
			//WAITING״̬
			case BECALLED_WAITING:
				BeCalled_Waiting_Process(msg_becalled);
				break;
			
			//RINGING״̬	
			case BECALLED_RINGING:
				BeCalled_Ringing_Process(msg_becalled);
				break;

			//ACK״̬
			case BECALLED_ACK:
				BeCalled_Ack_Process(msg_becalled);
				break;

			//BYE״̬	
			case BECALLED_BYE:
				BeCalled_Bye_Process(msg_becalled);
				break;

			//TRANSFER״̬	
			case BECALLED_TRANSFER:
				BeCalled_Transfer_Process(msg_becalled);
				break;
			
			//�쳣״̬	
			default:
				BeCalled_StateInvalid_Process(msg_becalled);
				break;
		}
	}
}

/*------------------------------------------------------------------------
			BeCalled_Timer_Callback
------------------------------------------------------------------------*/
void BeCalled_Timer_Callback(void)	//R_
{
//will_add_czn
	BECALLED_STRUCT	send_msg_to_becalled;
	uint8			timeout_flag;
	uint8			checklink_flag;
	
	timeout_flag=0;
	checklink_flag = 0;
	
	BeCalled_Run.timer++;	//��ʱ�ۼ�

	switch (BeCalled_Run.state)
	{
		case BECALLED_RINGING:	//�ȴ�ժ��
			if (BeCalled_Run.timer >= BeCalled_Config.limit_ringing_time)
			{
				timeout_flag=1;
			}
			if(BeCalled_Run.timer > 0 && BeCalled_Run.timer % ((ACK_RESPONSE_TIMEOUT+999)/1000) == 0)
			{
				checklink_flag = 1;
			}
			break;
		
		case BECALLED_ACK:		//�ȴ��һ�
			if (BeCalled_Run.timer >= BeCalled_Config.limit_ack_time)
			{
				timeout_flag=1;
			}
			if(BeCalled_Run.timer > 0 && BeCalled_Run.timer % ((ACK_RESPONSE_TIMEOUT+999)/1000) == 0)
			{
				checklink_flag = 1;
			}
			break;
		  
		case BECALLED_TRANSFER:	//�ȴ�����ת�Ƴɹ�
			if (BeCalled_Run.timer >= BeCalled_Config.limit_transfer_time)	
			{
				timeout_flag = 1;
			}
			break;
		  
		case BECALLED_BYE:		//�ȴ�����Ӧ��
			if (BeCalled_Run.timer >= BeCalled_Config.limit_bye_time)	
			{
				timeout_flag = 1;
			}
			break;
			
		default:
			timeout_flag = 1;
		  	break;
	}
	
	if (timeout_flag)
	{
		send_msg_to_becalled.msg_head.msg_type 		= BECALLED_MSG_TIMEOUT;
		send_msg_to_becalled.msg_head.msg_sub_type	= BECALLED_TOUT_TIMEOVER;
		
		//if (OS_Q_Put(&q_be, &send_msg_to_becalled, sizeof(BECALLED_STRUCT)))
		if(push_vdp_common_queue(task_becalled.p_msg_buf, (char *)&send_msg_to_becalled, BECALLED_STRUCT_BASIC_LENGTH) !=0 )
		{
			BeCalled_Run.timer--;
			OS_RetriggerTimer(&timer_becalled);	//�������, �ٴδ�����ʱ, �Ա��´η�����Ϣ
		}
		else 
		{
			OS_StopTimer(&timer_becalled);		//��Ϣѹ����гɹ�, �رն�ʱ	
		}			
	}
	else
	{
		if(checklink_flag == 1)
		{
			send_msg_to_becalled.msg_head.msg_type 		= BECALLED_MSG_TIMEOUT;
			send_msg_to_becalled.msg_head.msg_sub_type	= BECALLED_TOUT_CHECKLINK;
		
			push_vdp_common_queue(task_becalled.p_msg_buf, (char *)&send_msg_to_becalled, BECALLED_STRUCT_BASIC_LENGTH);
		}
		OS_RetriggerTimer(&timer_becalled);
	}
}

/*------------------------------------------------------------------------
					BeCalled_Waiting_Process
���:	msg_becalled

����:	����BECALLED_MSG_INVITE��Ϣ���д���: ����RINGING״̬

����:	��
------------------------------------------------------------------------*/
void BeCalled_Waiting_Process(BECALLED_STRUCT  *msg_becalled)	//R_
{
	switch (msg_becalled->msg_head.msg_type)
	{
		//MSG_ERROR
		case BECALLED_MSG_ERROR:
			BeCalled_To_Error(msg_becalled);
			break;

		case BECALLED_MSG_FORCECLOSE:
			BeCalled_To_ForceClose(msg_becalled);
			break;
			
		//�յ�INVIT, ����RINIGING
		case BECALLED_MSG_INVITE:
			BeCalled_To_Ringing(msg_becalled);
			break;

		//�쳣��Ϣ
		default:
			BeCalled_MsgInvalid_Process(msg_becalled);
			break;
	}
}

/*------------------------------------------------------------------------
						BeCalled_Ringing_Process
���:    msg_becalled

����:	���ո�����Ϣ�ֱ���BECALLED_RINGING״̬

����:   ��
		
------------------------------------------------------------------------*/
void BeCalled_Ringing_Process(BECALLED_STRUCT  *msg_becalled)	//R_
{
	switch (msg_becalled->msg_head.msg_type)
	{

		//�յ�INVIT, ����RINIGING
		case BECALLED_MSG_INVITE:
			BeCalled_To_Ringing(msg_becalled);
			break;
			
		//MSG_ERROR
		case BECALLED_MSG_ERROR:
			BeCalled_To_Error(msg_becalled);
			break;

		case BECALLED_MSG_FORCECLOSE:
			BeCalled_To_ForceClose(msg_becalled);
			break;	
	
		//MAG_ACK
		case BECALLED_MSG_ACK:
			BeCalled_To_Ack(msg_becalled);
			break;
		
		//REDIAL		
		case BECALLED_MSG_REDIAL:
			//BeCalled_To_Redial(msg_becalled->call_type, msg_becalled->address_s);
			break;
		
		//��1����	
		case BECALLED_MSG_UNLOCK1:
			BeCalled_To_Unlock(msg_becalled);
			break;
		
		//��2����	
		case BECALLED_MSG_UNLOCK2:
			BeCalled_To_Unlock(msg_becalled);
			break;
		
		//BYE	
		case BECALLED_MSG_BYE:
			BeCalled_To_Bye(msg_becalled);
			break;
		
		//CANCEL
		case BECALLED_MSG_CANCEL:
			BeCalled_To_Waiting(msg_becalled);
			break;
		
		//����(����)��ʱ
		case BECALLED_MSG_TIMEOUT:	
			BeCalled_To_Timeout(msg_becalled);
			break;
		
		//�쳣��Ϣ	
		default:
			BeCalled_MsgInvalid_Process(msg_becalled);
			break;
	}
	
}

/*------------------------------------------------------------------------
						BeCalled_Ack_Process
���:    msg_becalled

����:	���ո�����Ϣ�ֱ���BECALLED_ACK״̬

����:   ��
		
------------------------------------------------------------------------*/
void BeCalled_Ack_Process(BECALLED_STRUCT  *msg_becalled)	//R_
{
	switch (msg_becalled->msg_head.msg_type)
	{
		//MSG_ERROR
		case BECALLED_MSG_ERROR:
			BeCalled_To_Error(msg_becalled);
			break;

		case BECALLED_MSG_FORCECLOSE:
			BeCalled_To_ForceClose(msg_becalled);
			break;	
			
		//MAG_ACK
		case BECALLED_MSG_ACK:
			BeCalled_To_Ack(msg_becalled);
			break;
			
		//����ת��
		case BECALLED_MSG_TRANSFER:
			//BeCalled_To_Transfer(msg_becalled->call_type,msg_becalled->address_t);
			break;
			
		//��1����	
		case BECALLED_MSG_UNLOCK1:
			BeCalled_To_Unlock(msg_becalled);
			break;
		
		//��2����	
		case BECALLED_MSG_UNLOCK2:
			BeCalled_To_Unlock(msg_becalled);
			break;

		//MSG_BYE
		case BECALLED_MSG_BYE:
			BeCalled_To_Bye(msg_becalled);
			break;
		
		//���յ�ǿ��ȡ������(��·ȡ�� / ����ȡ��), ���������
		case BECALLED_MSG_CANCEL:
			BeCalled_To_Waiting(msg_becalled);
			break;

		//����(ͨ��)��ʱ
		case BECALLED_MSG_TIMEOUT:
			BeCalled_To_Timeout(msg_becalled);
			break;
		
		//�쳣��Ϣ		
		default:
			BeCalled_MsgInvalid_Process(msg_becalled);
			break;
	}
}

/*------------------------------------------------------------------------
					BeCalled_Bye_Process
���:    msg_becalled

����:	���ո�����Ϣ�ֱ���BECALLED_BYE״̬

����:   ��
		
------------------------------------------------------------------------*/
void BeCalled_Bye_Process(BECALLED_STRUCT  *msg_becalled)	//R_
{
	switch(msg_becalled->msg_head.msg_type)
	{
		//MSG_ERROR
		case BECALLED_MSG_ERROR:
			BeCalled_To_Error(msg_becalled);
			break;

		case BECALLED_MSG_FORCECLOSE:
			BeCalled_To_ForceClose(msg_becalled);
			break;	
			
		//�յ�200/OK, ����WAITING״̬
		case BECALLED_MSG_ACK:
			BeCalled_To_Waiting(msg_becalled);
			break;
			
		case BECALLED_MSG_CANCEL:
			BeCalled_To_Waiting(msg_becalled);
			break;
			
		//BYE��ʱ��Ӧ, ����WAITING״̬		
		case BECALLED_MSG_TIMEOUT:
			BeCalled_To_Timeout(msg_becalled);
			break;
	
		//�쳣��Ϣ
		default:
			BeCalled_MsgInvalid_Process(msg_becalled);
			break;
	}
}

/*------------------------------------------------------------------------
		BeCalled_Transfer_Process
���:    msg_becalled

����:	���ո�����Ϣ�ֱ���BECALLED_TRANSFER״̬

����:   ��
		
------------------------------------------------------------------------*/
void BeCalled_Transfer_Process(BECALLED_STRUCT  *msg_becalled)
{
	switch (msg_becalled->msg_head.msg_type)
	{	
		//��1��Ӧ
		case BECALLED_MSG_UNLOCK1:
			BeCalled_To_Unlock(msg_becalled);
			break;
		
		//��2��Ӧ
		case BECALLED_MSG_UNLOCK2:
			BeCalled_To_Unlock(msg_becalled);
			break;

		//BYE������Ӧ, ����BYE״̬
		case BECALLED_MSG_BYE:
			BeCalled_To_Bye(msg_becalled);
			break;
		
		//�յ�CANCEL, ����WAITING״̬	
		case BECALLED_MSG_CANCEL:
			BeCalled_To_Waiting(msg_becalled);
			break;

		//TRANSFER��ʱ��Ӧ, ����BYE״̬	
		case BECALLED_MSG_TIMEOUT:
			BeCalled_To_Timeout(msg_becalled);
			break;
			
		//����ת��ʧ��
		case BECALLED_MSG_ERROR:
			BeCalled_To_Ack(msg_becalled);
			break;

		case BECALLED_MSG_FORCECLOSE:
			BeCalled_To_ForceClose(msg_becalled);
			break;	
		
		//����ת�Ƴɹ�
		case BECALLED_MSG_RINGING:
			BeCalled_To_Waiting(msg_becalled);
			break;
			
		//�쳣��Ϣ		
		default:
			BeCalled_MsgInvalid_Process(msg_becalled);
			break;
	}
	
}

/*------------------------------------------------------------------------
			 BeCalled_StateInvalid_Process
���:    msg_becalled

����:	��ȷ��״̬�������
����:   ��
		
------------------------------------------------------------------------*/
void BeCalled_StateInvalid_Process(BECALLED_STRUCT  *msg_becalled)	//R
{
	//......
}

/*------------------------------------------------------------------------
			BeCalled_MsgInvalid_Process
���:    msg_becalled

����:	�Բ�ȷ��msg���д����򲻲���

		
------------------------------------------------------------------------*/
void BeCalled_MsgInvalid_Process(BECALLED_STRUCT *msg_becalled)	//R
{
	//will_add
	/*
	if (msg_becalled->msg_type==BECALLED_MSG_INVITE)
	{
		if (BeCalled_Run.state==BECALLED_RINGING)
		{
			OS_SignalEvent(RESPONSE_OK, &tcb_survey);
		}
		else
		{
			OS_SignalEvent(RESPONSE_ERR, &tcb_survey);
		}
	}
	if (msg_becalled->msg_type==BECALLED_MSG_CANCEL)
	{
		if (BeCalled_Run.state==BECALLED_WAITING)
		{
			OS_SignalEvent(RESPONSE_OK, &tcb_survey);
		}
		else
		{
			OS_SignalEvent(RESPONSE_ERR, &tcb_survey);
		}
	}
	*/
}

/*==============================================================================
					BeCalled_API
==============================================================================*/
/*uint8 API_BeCalled_Common(uint8 msg_type_temp, uint8 call_type_temp, 
						  uint16 address_s_temp,uint16 address_t_temp)	//R
{
	BECALLED_STRUCT	send_msg_to_BeCalled;	
	//OS_TASK_EVENT MyEvents;	

	//��֯������Ϣ��BeCalled
	send_msg_to_BeCalled.phone_type = PHONE_TYPE_BECALLED;
	send_msg_to_BeCalled.msg_type   = msg_type_temp;
	send_msg_to_BeCalled.address_s 	= address_s_temp;
	send_msg_to_BeCalled.address_t 	= address_t_temp;
	send_msg_to_BeCalled.call_type 	= call_type_temp;
//	if(OS_GetTaskID() == &tcb_phone)
//	{
//		vtk_TaskProcessEvent_BeCalled(&send_msg_to_BeCalled);
//		return 0;							  
//	}
	if (OS_Q_Put(&q_phone, &send_msg_to_BeCalled, sizeof(BECALLED_STRUCT)))
	{
		return 1;
	}
	
	return 0;
}
*/
/*------------------------------------------------------------------------
			Get_BeCalled_State
���أ�
	0-WAITING
	1-BUSY
------------------------------------------------------------------------*/
uint8 Get_BeCalled_State(void)
{
	if (BeCalled_Run.state == BECALLED_WAITING)
	{
		return (0);
	}
	return (1);
}

void Get_BeCalled_State_Rsp(BECALLED_STRUCT *msg_becalled)
{
	BECALLED_STRUCT msg;
	
	msg.msg_head.msg_source_id	= msg_becalled->msg_head.msg_target_id;
	msg.msg_head.msg_target_id	= msg_becalled->msg_head.msg_source_id;
	msg.msg_head.msg_type 		= BECALLED_MSG_GETSTATE | 0x80;
	msg.msg_head.msg_sub_type	= 0;
	msg.ext_buf[0]				= BeCalled_Run.state;
	msg.ext_len					= 1;

	vdp_uart_send_data((char*)&msg,BECALLED_STRUCT_BASIC_LENGTH + 1);
}
/*------------------------------------------------------------------------
			Get_BeCalled_PartnerAddr
------------------------------------------------------------------------*/





Loop_vdp_common_buffer	vdp_ipbecalled_mesg_queue;
Loop_vdp_common_buffer	vdp_ipbecalled_sync_queue;


vdp_task_t	task_becalled;
void* vdp_becalled_task( void* arg );

/*------------------------------------------------------------------------
						OutCall&CallIn Task Process
------------------------------------------------------------------------*/
void ipbecalled_mesg_data_process(void *Msg,int len )	//R_
{
	vtk_TaskProcessEvent_BeCalled((BECALLED_STRUCT *)Msg);
}
void BeCalled_RevData_Process_Udp(void *Msg_Phone,int len )
{
	;
}

void BeCalled_RevData_Process_Inner(void *Msg_Phone,int len )
{
	;
}
//OneCallType	OneMyCallObject;

void init_vdp_becalled_task(void)
{
	// ��ʼ��һ�����ж���	
	//memset( (char*)&OneMyCallObject, 0, sizeof(OneMyCallObject) );
	vtk_TaskInit_BeCalled();
	

	init_vdp_common_queue(&vdp_ipbecalled_mesg_queue, 1000, (msg_process)ipbecalled_mesg_data_process, &task_becalled);
	init_vdp_common_queue(&vdp_ipbecalled_sync_queue, 100, NULL, 								  &task_becalled);
	init_vdp_common_task(&task_becalled, MSG_ID_IpBeCalled, vdp_becalled_task, &vdp_ipbecalled_mesg_queue, &vdp_ipbecalled_sync_queue);
}

void exit_beccalled_task(void)
{
	
}

void* vdp_becalled_task( void* arg )
{
	vdp_task_t*	 ptask 			= (vdp_task_t*)arg;
	p_vdp_common_buffer pdb 	= 0;
	int	size;

	while( ptask->task_run_flag )
	{
		size = pop_vdp_common_queue(ptask->p_msg_buf, &pdb, VDP_QUEUE_POLLING_TIME);
		if( size > 0 )
		{
			(*ptask->p_msg_buf->process)(pdb,size);
			purge_vdp_common_queue( ptask->p_msg_buf );
		}
	}
	return 0;
}


void IpBeCalled_Business_Rps(BECALLED_STRUCT *msg,unsigned char result)
{
	BECALLED_STRUCT rspmsg;
	rspmsg.msg_head.msg_target_id 		= msg->msg_head.msg_source_id;
	rspmsg.msg_head.msg_source_id		= msg->msg_head.msg_target_id;
	rspmsg.msg_head.msg_type			= msg->msg_head.msg_type|0x80;
	memcpy(&rspmsg.msg_head.msg_sub_type,&msg->msg_head.msg_sub_type,BECALLED_STRUCT_BASIC_LENGTH-3);
	rspmsg.ext_len = 1;
	rspmsg.ext_buf[0] = result;
	vdp_uart_send_data((char*)&rspmsg,BECALLED_STRUCT_BASIC_LENGTH + 1);
}
/*********************************************************************************************************
**  End Of File
*********************************************************************************************************/
