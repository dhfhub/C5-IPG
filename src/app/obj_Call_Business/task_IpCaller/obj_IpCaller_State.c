/**
  ******************************************************************************
  * @file    obj_Caller_State.c
  * @author  czn
  * @version V00.02.00 (basic on vsip
  * @date    2014.11.06
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  * <h2><center>&copy; COPYRIGHT 2014 V-Tec</center></h2>
  ******************************************************************************
  */ 
#include "../../task_survey/obj_CallServer_Virtual/obj_CallServer_Virtual.h"
#include "task_IpCaller.h"
#include "obj_IpCaller_State.h"
#include "obj_IpCaller_Data.h"
//#include "define_Command.h"



uint8 Caller_UnlockId;
uint8 Caller_ErrorCode;
/*------------------------------------------------------------------------
						Caller_To_Invite
���:  
	�绰��������ָ��, ��������

����:


����: 
	1 = ����ʧ�� 
	0 = ����ɹ�
------------------------------------------------------------------------*/
uint8 Caller_To_Invite(CALLER_STRUCT *msg)	//R_
{
	uint8 callback_id;
	
	
	//װ��Config
	Caller_Data_Init();
	

	//���в�����ʼ��
	Caller_Run.state 			= CALLER_INVITE;					//״̬��
	Caller_Run.call_type 		= msg->call_type;						//��������
	Caller_Run.s_addr		= msg->s_addr;						//Ŀ���ַ
	Caller_Run.t_addr			= msg->t_addr;
	//����INVITE��ʱ	
	Caller_Run.timer = 0;
	OS_RetriggerTimer(&timer_caller);
	
	callback_id = Get_Caller_ToInvite_CallbackID(msg->call_type);
	
	if (callback_id)
	{
		(*TABLE_CALLTYPE_CALLER_TOINVITE_CALLBACK[callback_id-1].callback)(msg);
	}
	
	return 0;


}

/*------------------------------------------------------------------------
						Caller_To_Redial
���:  
	�绰��������ָ��, ��������

����:
	ִ���ز��Ų���

����: 
	1 =	����ʧ�� 
	0 = ����ɹ�
------------------------------------------------------------------------*/
uint8 Caller_To_Redial(CALLER_STRUCT *msg)	//R	//ʵ����ʹ��
{
	//will_add_czn
	/*uint8 callback_id;
	
	
	//״̬��
	Caller_Run.state 		= CALLER_INVITE;
	Caller_Run.call_type 	= call_type;						//��������
	Caller_Run.target_addr	= target_addr;						//Ŀ���ַ
	
	Caller_Run.timer = 0;
	OS_RetriggerTimer(&timer_caller);
	
	callback_id = Get_Caller_ToRedial_CallbackID(call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_CALLER_TOREDIAL_CALLBACK[callback_id-1].callback)();
	}*/
	return 0;
}


/*------------------------------------------------------------------------
					Caller_To_Ringing
���:  
	�绰��������ָ��, ��������

����:
	����RINGING״̬

����: 
	��
------------------------------------------------------------------------*/
void Caller_To_Ringing(CALLER_STRUCT *msg)	//R_
{
	uint8 callback_id;


	//״̬��
	Caller_Run.state = CALLER_RINGING;

	//RINGING��ʱ
	Caller_Run.timer = 0;
	OS_RetriggerTimer(&timer_caller);
	
	callback_id = Get_Caller_ToRinging_CallbackID(msg->call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_CALLER_TORINGING_CALLBACK[callback_id-1].callback)(msg);
	}
}

/*------------------------------------------------------------------------
					Caller_To_Ack
���:  
	�绰��������ָ��, control����

����:
	״̬ת�ƽ���ACK״̬

����: 
	��
------------------------------------------------------------------------*/
void Caller_To_Ack(CALLER_STRUCT *msg)	//R_
{
	uint8 callback_id;


	//״̬��
	Caller_Run.state = CALLER_ACK;
	
	//ACK��ʱ
	Caller_Run.timer = 0;
	OS_RetriggerTimer(&timer_caller);
	printf("Caller_To_Ack===============\n");
	callback_id = Get_Caller_ToAck_CallbackID(msg->call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_CALLER_TOACK_CALLBACK[callback_id-1].callback)(msg);
	}
	
}

/*------------------------------------------------------------------------
					Caller_To_Bye
���:  
	control����

����:
	״̬ת�ƽ���BYE״̬

����: 
	��
------------------------------------------------------------------------*/
void Caller_To_Bye(CALLER_STRUCT *msg)	//R_
{
	uint8 callback_id;


	//״̬��
	Caller_Run.state = CALLER_BYE;

	//BYE��ʱ
	Caller_Run.timer = 0;
	OS_RetriggerTimer(&timer_caller);

	callback_id = Get_Caller_ToBye_CallbackID(msg->call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_CALLER_TOBYE_CALLBACK[callback_id-1].callback)(msg);
	}
}

/*------------------------------------------------------------------------
					Caller_To_Timeout
���:  
	control����

����:
	��ʱ����

����: 
	��
------------------------------------------------------------------------*/
void Caller_To_Timeout(CALLER_STRUCT *msg)	//R_
{
	uint8 callback_id;


	callback_id = Get_Caller_ToTimeout_CallbackID(Caller_Run.call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_CALLER_TOTIMEOUT_CALLBACK[callback_id-1].callback)(msg);
	}
}
/*------------------------------------------------------------------------
					Caller_To_Waiting
���:  
       control����

����:
	״̬ת�ƽ���WAITING״̬

����: 
	��
------------------------------------------------------------------------*/
void Caller_To_Waiting(CALLER_STRUCT *msg)	//R_
{
	uint8 callback_id;


	//״̬��
	Caller_Run.state = CALLER_WAITING;
	
	//�رն�ʱ
	Caller_Run.timer = 0;
	OS_StopTimer(&timer_caller);
	
	callback_id = Get_Caller_ToWaiting_CallbackID(msg->call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_CALLER_TOWAITING_CALLBACK[callback_id-1].callback)(msg);
	}
}

/*-----------------------------------------------
			Caller_Unlock
���:
       call_type,	    lock_id:��1 / ��2

����:
	��
-----------------------------------------------*/
void Caller_To_Unlock(CALLER_STRUCT *msg)	//R_
{
	uint8 callback_id;


	//��¼����ID
	Caller_UnlockId = msg->ext_buf[0];
	
	callback_id = Get_Caller_ToUnlock_CallbackID(msg->call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_CALLER_TOUNLOCK_CALLBACK[callback_id-1].callback)(msg);
	}
	
}

/*-----------------------------------------------
			BeCalled_To_Error
���:
       call_type,	    error_code

����:
	��
-----------------------------------------------*/
void Caller_To_Error(CALLER_STRUCT *msg)	
{
	uint8 callback_id;


	//��¼����id���Թ��ص�ʹ��
	Caller_ErrorCode = msg->ext_buf[0];
	
	callback_id = Get_Caller_ToError_CallbackID(msg->call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_CALLER_TOERROR_CALLBACK[callback_id-1].callback)(msg);
	}
}

void Caller_To_ForceClose(CALLER_STRUCT *msg)
{
	uint8 callback_id;

	if(Caller_Run.state != CALLER_WAITING)
	{
		//״̬��
		Caller_Run.state = CALLER_WAITING;
		
		//�رն�ʱ
		Caller_Run.timer = 0;
		OS_StopTimer(&timer_caller);
		
		callback_id = Get_Caller_ToForceClose_CallbackID(Caller_Run.call_type);
		if (callback_id)
		{
			(*TABLE_CALLTYPE_CALLER_TOFORCECLOSE_CALLBACK[callback_id-1].callback)(msg);
		}
	}
	IpCaller_Business_Rps(msg,0);
}
/*********************************************************************************************************
**  End Of File
*********************************************************************************************************/
