/**
  ******************************************************************************
  * @file    task_Called.c
  * @author  czn
  * @version V00.02.00 (basic on vsip)
  * @date    2014.11.06
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


#include "../../vdp_uart.h"
#include "../../task_survey/obj_CallServer_Virtual/obj_CallServer_Virtual.h"
#include "task_IpCaller.h"
#include "obj_IpCaller_State.h"
#include "obj_IpCaller_Data.h"

#include "../../vtk_udp_stack/vtk_udp_stack_c5_ipc_cmd.h"
#include "../../../os/OSTIME.h"
OS_TIMER timer_caller;

CALLER_RUN Caller_Run;
/*------------------------------------------------------------------------
			Caller_Task_Init
------------------------------------------------------------------------*/
void vtk_TaskInit_Caller(void)	//R_
{
	//�����ʼ��
	Caller_Run.state = CALLER_WAITING;
	
	//������ʱ(δ����)
	OS_CreateTimer(&timer_caller, Caller_Timer_Callback, 1000/20);
}

/*------------------------------------------------------------------------
						Caller_Task_Process
------------------------------------------------------------------------*/
void vtk_TaskProcessEvent_Caller(CALLER_STRUCT	*msg_caller)	//R_
{
	bprintf("-------------ipfcaller recv cmd %04x,%04x\n",msg_caller->msg_head.msg_type,CALLER_MSG_GETSTATE);
	if(msg_caller->msg_head.msg_type == CALLER_MSG_GETSTATE)
	{
		Get_Caller_State_Rsp(msg_caller);
	}
	else
	{
		switch (Caller_Run.state)
		{
			//WAITING״̬
			case CALLER_WAITING:
				Caller_Waiting_Process(msg_caller);
				break;
			
			//INVITE״̬
			case CALLER_INVITE:
				Caller_Invite_Process(msg_caller);
				break;
			
			//RINGING״̬
			case CALLER_RINGING:
				Caller_Ringing_Process(msg_caller);
				break;
			
			//ACK״̬
			case CALLER_ACK:
				Caller_Ack_Process(msg_caller);
				break;
			
			//BYE״̬
			case CALLER_BYE:
				Caller_Bye_Process(msg_caller);
				break;
			
			//�쳣״̬
			default:
				Caller_StateInvalid_Process(msg_caller);
				break;
		}
	}
}

/*------------------------------------------------------------------------
			Caller_Timer_Callback
------------------------------------------------------------------------*/
void Caller_Timer_Callback(void)	//R_
{
//will_add_czn
	CALLER_STRUCT	send_msg_to_caller;
	uint8			timeout_flag;
	uint8			checklink_flag;
	
	timeout_flag = 0;
	checklink_flag = 0;
	
	Caller_Run.timer++;	//��ʱ�ۼ�
		
	switch (Caller_Run.state)
	{
		case CALLER_INVITE:		//�ȴ�����Ӧ��
			if (Caller_Run.timer >= Caller_Config.limit_invite_time)
			{
				timeout_flag = 1;
			}
			break;

		case CALLER_RINGING:	//�ȴ�����ժ��	
			if (Caller_Run.timer >= Caller_Config.limit_ringing_time)
			{
				timeout_flag = 1;
			}
			
			if(Caller_Run.timer > 0 && Caller_Run.timer % ((ACK_RESPONSE_TIMEOUT+999)/1000) == 0)
			{
				checklink_flag = 1;
			}
			break;
			
		case CALLER_ACK:		//�ȴ����йһ�
			if (Caller_Run.timer >= Caller_Config.limit_ack_time)
			{
				timeout_flag = 1;
			}

			if(Caller_Run.timer > 0 && Caller_Run.timer % ((ACK_RESPONSE_TIMEOUT+999)/1000) == 0)
			{
				checklink_flag = 1;
			}
			break;

		case CALLER_BYE:		//�ȴ�����Ӧ��
			if (Caller_Run.timer >= Caller_Config.limit_bye_time)
			{
				timeout_flag = 1;
			}
			break;
			
		default:				//״̬�쳣����
			timeout_flag = 1;
		  	break;
	}

	
		
	if (timeout_flag)
	{
		send_msg_to_caller.msg_head.msg_type 		= CALLER_MSG_TIMEOUT;
		send_msg_to_caller.msg_head.msg_sub_type	= CALLER_TOUT_TIMEOVER;
		
		//if (OS_Q_Put(&q_phone, &send_msg_to_caller, sizeof(CALLER_STRUCT)))
		if(push_vdp_common_queue(task_caller.p_msg_buf, (char *)&send_msg_to_caller, CALLER_STRUCT_BASIC_LENGTH) !=0 )
		{
			Caller_Run.timer--;
			OS_RetriggerTimer(&timer_caller);	//�������, �ٴδ�����ʱ, �Ա��´η�����Ϣ
		}
		else 
		{
			OS_StopTimer(&timer_caller);		//��Ϣѹ����гɹ�, �رն�ʱ	
		}			
	}
	else
	{
		if (checklink_flag == 1)
		{
			send_msg_to_caller.msg_head.msg_type 		= CALLER_MSG_TIMEOUT;
			send_msg_to_caller.msg_head.msg_sub_type	= CALLER_TOUT_CHECKLINK;
			
			push_vdp_common_queue(task_caller.p_msg_buf, (char *)&send_msg_to_caller, CALLER_STRUCT_BASIC_LENGTH);
					
		}
		
	  	OS_RetriggerTimer(&timer_caller);
	}

	
	
}

/*------------------------------------------------------------------------
					Caller_Waiting_Process
���:    msg_caller

����:	���ո�����Ϣ�ֱ���CALLER_WAITING״̬

����:   ��
------------------------------------------------------------------------*/
void Caller_Waiting_Process(CALLER_STRUCT *msg_caller)	//R_
{
 	switch (msg_caller->msg_head.msg_type)
 	{
 		case CALLER_MSG_ERROR:
			Caller_To_Error(msg_caller);
			break;	
			
 		case CALLER_MSG_FORCECLOSE:
			Caller_To_ForceClose(msg_caller);
			break;	
		//INVITE��Ӧ, ����INVITE״̬
		case CALLER_MSG_INVITE:
			Caller_To_Invite(msg_caller);
        		break;

		case CALLER_MSG_CANCEL:
			IpCaller_Business_Rps(msg_caller,0);
			API_CallServer_IpStateNoticeAckOk();
			break;
			
		//�쳣��Ϣ	
		default:
			Caller_MsgInvalid_Process(msg_caller);
			break;          
	}
}

/*------------------------------------------------------------------------
					Caller_Invite_Process
���:    msg_caller

����:	���ո�����Ϣ�ֱ���CALLER_INVIT״̬

����:   ��
		
------------------------------------------------------------------------*/
void Caller_Invite_Process(CALLER_STRUCT *msg_caller)	//R_
{
	switch (msg_caller->msg_head.msg_type)
	{
		//REDIAL������Ӧ	
		case CALLER_MSG_REDIAL:
			//Caller_To_Redial(Caller_Run.call_type, Caller_Run.target_addr);
			break;
			
		//�յ�ERROR��Ϣ
		case CALLER_MSG_ERROR:
			Caller_To_Error(msg_caller);
			break;

		case CALLER_MSG_FORCECLOSE:
			Caller_To_ForceClose(msg_caller);
			break;		
		
		//�յ�ringing��Ϣ: ���гɹ�
		case CALLER_MSG_RINGING:
			Caller_To_Ringing(msg_caller);
			break;
			
		//�յ�Ack��Ϣ
		case CALLER_MSG_ACK:
			Caller_To_Ack(msg_caller);
			break;
			
		//�յ�bye����Ϣ: ���н���
		case CALLER_MSG_BYE:
			Caller_To_Waiting(msg_caller);
			break;

		//�յ�cancel��Ϣ: ȡ������
		case CALLER_MSG_CANCEL:
			Caller_To_Bye(msg_caller);
			break;

		//INVITE��ʱ, ����BYE״̬	
		case CALLER_MSG_TIMEOUT:
			Caller_To_Timeout(msg_caller);
			break;
			
		//�쳣��Ϣ	
		default:
			Caller_MsgInvalid_Process(msg_caller);
			break;          
	}
}

/*------------------------------------------------------------------------
					Caller_Ringing_Process
���:    msg_caller

����:	���ո�����Ϣ�ֱ���CALLER_RINGING״̬

����:   ��
		
------------------------------------------------------------------------*/
void Caller_Ringing_Process(CALLER_STRUCT *msg_caller)	//R_
{
	
	switch (msg_caller->msg_head.msg_type)
	{
		//�յ�ERROR��Ϣ
		case CALLER_MSG_ERROR:
			Caller_To_Error(msg_caller);
			break;

		case CALLER_MSG_FORCECLOSE:
			Caller_To_ForceClose(msg_caller);
			break;		
			
		//REDIAL������Ӧ	
		case CALLER_MSG_REDIAL:
			//Caller_To_Redial(Caller_Run.call_type, Caller_Run.target_addr);
			break;
			
		case CALLER_MSG_ACK:
			Caller_To_Ack(msg_caller);
			break;
		
		//��1��Ӧ	
		case CALLER_MSG_UNLOCK1:
			Caller_To_Unlock(msg_caller);
			break;

		//��2��Ӧ	
		case CALLER_MSG_UNLOCK2:
			Caller_To_Unlock(msg_caller);
			break;
		
		//�յ�BYE��Ϣ: ���н���
		case CALLER_MSG_BYE:
			Caller_To_Waiting(msg_caller);
			break;
			
		//�յ�CANCELָ��: ȡ������
		case CALLER_MSG_CANCEL:
			Caller_To_Bye(msg_caller);
			break;
			
		//RINGING��ʱ, ����BYE״̬
		case CALLER_MSG_TIMEOUT:
			Caller_To_Timeout(msg_caller);
			break;
			
		//�쳣��Ϣ	
		default:
			Caller_MsgInvalid_Process(msg_caller);
			break;          
	}
}

/*------------------------------------------------------------------------
						Caller_Ack_Process
���:    msg_caller

����:	���ո�����Ϣ�ֱ���CALLER_ACK״̬

����:   ��
------------------------------------------------------------------------*/
void Caller_Ack_Process(CALLER_STRUCT *msg_caller)	//R_
{
	switch (msg_caller->msg_head.msg_type)
	{
		//�յ�ERROR��Ϣ
		case CALLER_MSG_ERROR:
			Caller_To_Error(msg_caller);
			break;

		case CALLER_MSG_FORCECLOSE:
			Caller_To_ForceClose(msg_caller);
			break;		
		//��1��Ӧ
		case CALLER_MSG_UNLOCK1:
			Caller_To_Unlock(msg_caller);
			break;
		
		//��2��Ӧ
		case CALLER_MSG_UNLOCK2:
			Caller_To_Unlock(msg_caller);
			break;
			
		//�յ�RINGING��Ϣ: ����RINGING
		case CALLER_MSG_RINGING:
			Caller_To_Ringing(msg_caller);
			break;	
			
		//�յ�BYE��Ϣ: ���н���
		case CALLER_MSG_BYE:
			Caller_To_Waiting(msg_caller);
			break;
			
		//�յ�CANCELָ��: ȡ������
		case CALLER_MSG_CANCEL:
			Caller_To_Bye(msg_caller);
			break;
			
		//ACK��ʱ, ����BYE״̬
		case CALLER_MSG_TIMEOUT:
			Caller_To_Timeout(msg_caller);
			break;
		
			
		//�쳣��Ϣ
		default:
			Caller_MsgInvalid_Process(msg_caller);
			break;
	}
}

/*------------------------------------------------------------------------
					Caller_Bye_Process
���:    msg_caller

����:	���ո�����Ϣ�ֱ���CALLER_BYE״̬

����:   ��
		
------------------------------------------------------------------------*/
void Caller_Bye_Process(CALLER_STRUCT *msg_caller)	//R_
{
	switch (msg_caller->msg_head.msg_type)
	{
		//�յ�ERROR��Ϣ
		case CALLER_MSG_ERROR:
			Caller_To_Error(msg_caller);
			break;

		case CALLER_MSG_FORCECLOSE:
			Caller_To_ForceClose(msg_caller);
			break;		
			
		//�յ�ACK, ����WAITING״̬
		case CALLER_MSG_ACK:
			Caller_To_Waiting(msg_caller);
			break;
		//�յ�BYE,����WAITING״̬
		case CALLER_MSG_BYE:
			Caller_To_Waiting(msg_caller);
			break;
		//BYE��ʱ, ����WAITING״̬
		case CALLER_MSG_TIMEOUT:
			Caller_To_Timeout(msg_caller);
			break;
		
		//�쳣��Ϣ
		default:
			Caller_MsgInvalid_Process(msg_caller);
			break;          
	}
}
/*------------------------------------------------------------------------
					Caller_StateInvalid_Process
���:    msg_caller

����:	��ȷ��״̬�������
����:   ��
		
------------------------------------------------------------------------*/
void Caller_StateInvalid_Process(CALLER_STRUCT *msg_caller)	//R
{
	//......
}

/*------------------------------------------------------------------------
				Caller_MsgInvalid_Process
���:    msg_caller

����:	�Բ�ȷ��msg���д����򲻲���

		
------------------------------------------------------------------------*/
void Caller_MsgInvalid_Process(CALLER_STRUCT *msg_caller)	//R
{
  	//......
}



/*------------------------------------------------------------------------
			Get_Caller_State
����:
	0-WAITING
	1-BUSY
------------------------------------------------------------------------*/
uint8 Get_Caller_State(void)	//R
{
	if (Caller_Run.state==CALLER_WAITING)
	{
		return (0);
	}
	return (1);
}

void Get_Caller_State_Rsp(CALLER_STRUCT *msg_caller)
{
	CALLER_STRUCT msg;
	
	

	msg.msg_head.msg_source_id	= msg_caller->msg_head.msg_target_id;
	msg.msg_head.msg_target_id	= msg_caller->msg_head.msg_source_id;
	msg.msg_head.msg_type 		= CALLER_MSG_GETSTATE | 0x80;
	msg.msg_head.msg_sub_type	= 0;
	msg.ext_buf[0]				= Caller_Run.state;
	msg.ext_len					= 1;

	bprintf("send caller state len %d\n",CALLER_STRUCT_BASIC_LENGTH + 1);
	vdp_uart_send_data((char*)&msg,CALLER_STRUCT_BASIC_LENGTH + 1);
}
/*------------------------------------------------------------------------
			Get_Caller_PartnerAddr
------------------------------------------------------------------------*/
//uint16 Get_Caller_PartnerAddr(void)	//R
//{
//	return (Caller_Run.partner_IA);
//}



Loop_vdp_common_buffer	vdp_ipcaller_mesg_queue;
Loop_vdp_common_buffer	vdp_ipcaller_sync_queue;

//void vdp_call_survey_uart_data_process(char* msg_data, int len);
//void vdp_call_survey_udp_data_process(char* msg_data, int len);
//void vdp_call_survey_inner_data_process(char* msg_data, int len);

vdp_task_t	task_caller;
void* vdp_caller_task( void* arg );

/*------------------------------------------------------------------------
						OutCall&CallIn Task Process
------------------------------------------------------------------------*/
void ipcaller_mesg_data_process(void *Msg,int len )	//R_
{
	
	vtk_TaskProcessEvent_Caller((CALLER_STRUCT *)Msg);
	
}
void Caller_RevData_Process_Udp(void *Msg_Phone,int len )
{
	;
}
void Caller_RevData_Process_Inner(void *Msg_Phone,int len )
{
	printf("================Caller_RevData_Process_Inner\n");	
}
//OneCallType	OneMyCallObject;

void init_vdp_caller_task(void)
{
	
	vtk_TaskInit_Caller();
	init_vdp_common_queue(&vdp_ipcaller_mesg_queue, 1000, (msg_process)ipcaller_mesg_data_process, &task_caller);
	init_vdp_common_queue(&vdp_ipcaller_sync_queue, 100, NULL, 								  &task_caller);
	init_vdp_common_task(&task_caller, MSG_ID_IpBeCalled, vdp_caller_task, &vdp_ipcaller_mesg_queue, &vdp_ipcaller_sync_queue);
}

void exit_caller_task(void)
{
	
}

void* vdp_caller_task( void* arg )
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



void IpCaller_Business_Rps(CALLER_STRUCT *msg,unsigned char result)
{
	CALLER_STRUCT rspmsg;
	rspmsg.msg_head.msg_target_id 		= msg->msg_head.msg_source_id;
	rspmsg.msg_head.msg_source_id		= msg->msg_head.msg_target_id;
	rspmsg.msg_head.msg_type			= msg->msg_head.msg_type|0x80;
	memcpy(&rspmsg.msg_head.msg_sub_type,&msg->msg_head.msg_sub_type,CALLER_STRUCT_BASIC_LENGTH-3);
	rspmsg.ext_len 					= 1;
	rspmsg.ext_buf[0] 					= result;
	vdp_uart_send_data((char*)&rspmsg,CALLER_STRUCT_BASIC_LENGTH + 1);
}
/*********************************************************************************************************
**  End Of File
*********************************************************************************************************/

