/**
  ******************************************************************************
  * @file    obj_BeCalled_State.c
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



/*
#include "../task_Phone.h"
#include "task_BeCalled.h"
//#include "task_Caller.h"
#include "obj_BeCalled_State.h"
#include "obj_BeCalled_Data.h"
//#include "define_Command.h"
*/
#include "../../task_survey/obj_CallServer_Virtual/obj_CallServer_Virtual.h"
#include "task_IpBeCalled.h"
//#include "task_Caller.h"
#include "obj_IpBeCalled_State.h"
#include "obj_IpBeCalled_Data.h"
#include "../../../os/OSTIME.h"



uint8 BeCalled_UnlockId;
uint8 BeCalled_ErrorCode;
/*------------------------------------------------------------------------
					BeCalled_To_Ringing
���:  
	Դ��ַ����������

����:
	״̬ת�ƽ���RINGING״̬

����: 
	1 = ����ʧ�� 
	0 = ����ɹ�
------------------------------------------------------------------------*/
void BeCalled_To_Ringing(BECALLED_STRUCT *msg)	//R_
{
	uint8 callback_id; 

	printf("-------------BeCalled_To_Ringing-------------------\n");
		 
	//װ��Config 
	BeCalled_Data_Init();

	//��ʼ�����в���
	BeCalled_Run.call_type		= msg->call_type;
	BeCalled_Run.s_addr 			= msg->s_addr;
	BeCalled_Run.t_addr			= msg->t_addr;
	BeCalled_Run.state 			= BECALLED_RINGING;
	
	//RINGING��ʱ
	BeCalled_Run.timer = 0;
	OS_RetriggerTimer(&timer_becalled);

	//���ݺ������ͽ��в˵�,ָʾ��,��ʾ���ȵ���ش���
	callback_id = Get_BeCalled_ToRinging_CallbackID(msg->call_type);
	printf("-------------BeCalled_To_Ringing-CallbackID--%d   %d\n",callback_id,msg->call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_BECALLED_TORINGING_CALLBACK[callback_id-1].callback)(msg);
	}
}

/*------------------------------------------------------------------------
					BeCalled_To_Redial
���:  
	call���ͣ��绰��������ָ��

����:
	ִ���ز��Ų���

����: 
	1 =	����ʧ�� 
	0 = ����ɹ�
------------------------------------------------------------------------*/
uint8 BeCalled_To_Redial(BECALLED_STRUCT *msg)
{
//will_add_czn
	/*uint8 callback_id; 
		 

	//��ʼ�����в���
	BeCalled_Run.call_type		= call_type;
	BeCalled_Run.partner_source = source_addr;
	BeCalled_Run.state 			= BECALLED_RINGING;
	
	//RINGING��ʱ
	BeCalled_Run.timer = 0;
	OS_RetriggerTimer(&timer_becalled);

	//���ݺ������ͽ��в˵�,ָʾ��,��ʾ���ȵ���ش���
	callback_id = Get_BeCalled_ToRedial_CallbackID(call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_BECALLED_TOREDIAL_CALLBACK[callback_id-1].callback)();
	}*/
	return 0;
}

/*------------------------------------------------------------------------
				BeCalled_To_Transfer
���:  
	Դ��ַ,Ŀ���ַ��call����

����:
	״̬ת�ƽ���TRANSFER״̬

����: 
	��
------------------------------------------------------------------------*/
void BeCalled_To_Transfer(BECALLED_STRUCT *msg)
{
	/*uint8 callback_id;
	

	BeCalled_Run.partner_target = target_addr;
	BeCalled_Run.state 			= BECALLED_TRANSFER;
	
	//��ʱ
	BeCalled_Run.timer = 0;
	//will_add_czn OS_RetriggerTimer(&timer_becalled);

	
	callback_id = Get_BeCalled_ToTransfer_CallbackID(call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_BECALLED_TOTRANSFER_CALLBACK[callback_id-1].callback)();
	}
	*/
}

/*------------------------------------------------------------------------
					BeCalled_To_Ack
���:  
	Դ��ַ��Ŀ���ַ��control_type(bit4-7������ACK���ͣ�bit0-3����Ϣ������)

����:
	״̬ת�ƽ���ACK״̬

����: 
	��
------------------------------------------------------------------------*/
void BeCalled_To_Ack(BECALLED_STRUCT *msg)	//R_
{
	uint8 callback_id;
	
	
	BeCalled_Run.state = BECALLED_ACK;
	
	//����ʱ
	BeCalled_Run.timer = 0;
	OS_RetriggerTimer(&timer_becalled);

	callback_id = Get_BeCalled_ToAck_CallbackID(msg->call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_BECALLED_TOACK_CALLBACK[callback_id-1].callback)(msg);
	}
}

/*------------------------------------------------------------------------
					BeCalled_To_Bye
���:  
	control����

����:
	״̬ת�ƽ���BYE״̬

����: 
	��
------------------------------------------------------------------------*/
void BeCalled_To_Bye(BECALLED_STRUCT *msg)	//R_
{
	uint8 callback_id;
	
	//����BYE״̬
	BeCalled_Run.state = BECALLED_BYE;
	
	BeCalled_Run.timer = 0;
	OS_RetriggerTimer(&timer_becalled);
	
	callback_id = Get_BeCalled_ToBye_CallbackID(msg->call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_BECALLED_TOBYE_CALLBACK[callback_id-1].callback)(msg);
	}
}

/*------------------------------------------------------------------------
					BeCalled_To_Timeout
���:  
	control����

����:
	��ʱ����

����: 
	��
------------------------------------------------------------------------*/
void BeCalled_To_Timeout(BECALLED_STRUCT *msg)	//R_
{
	uint8 callback_id;
	
	callback_id = Get_BeCalled_ToTimeout_CallbackID(BeCalled_Run.call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_BECALLED_TOTIMEOUT_CALLBACK[callback_id-1].callback)(msg);
	}
}

/*------------------------------------------------------------------------
					BeCalled_To_Waiting
���:  
	control����

����:
	״̬ת�ƽ���WAITING״̬

����: 
	��
------------------------------------------------------------------------*/
void BeCalled_To_Waiting(BECALLED_STRUCT *msg)	//R_
{
	uint8 callback_id;


	BeCalled_Run.state = BECALLED_WAITING;
	
	//�رն�ʱ��
	BeCalled_Run.timer = 0;
	OS_StopTimer(&timer_becalled);
	
	callback_id = Get_BeCalled_ToWaiting_CallbackID(msg->call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_BECALLED_TOWAITING_CALLBACK[callback_id-1].callback)(msg);
	}
	
}

/*-----------------------------------------------
			BeCalled_To_Unlock
���:
       call_type,	    lock_id:��1 / ��2

����:
	��
-----------------------------------------------*/
void BeCalled_To_Unlock(BECALLED_STRUCT *msg)	//R_
{
	uint8 callback_id;


	//��¼����id���Թ��ص�ʹ��
	//BeCalled_UnlockId = msg->ext_buf[0];

	callback_id = Get_BeCalled_ToUnlock_CallbackID(msg->call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_BECALLED_TOUNLOCK_CALLBACK[callback_id-1].callback)(msg);
	}
}
/*-----------------------------------------------
			BeCalled_To_Error
���:
       call_type,	    error_code

����:
	��
-----------------------------------------------*/
void BeCalled_To_Error(BECALLED_STRUCT *msg)	//R_
{
	uint8 callback_id;


	//��¼����id���Թ��ص�ʹ��
	BeCalled_ErrorCode = msg->ext_buf[0];
	
	callback_id = Get_BeCalled_ToError_CallbackID(msg->call_type);
	if (callback_id)
	{
		(*TABLE_CALLTYPE_BECALLED_TOERROR_CALLBACK[callback_id-1].callback)(msg);
	}
}

void BeCalled_To_ForceClose(BECALLED_STRUCT *msg)
{
	uint8 callback_id;

	if(BeCalled_Run.state != BECALLED_WAITING)
	{
		//״̬��
		BeCalled_Run.state = BECALLED_WAITING;
		
		//�رն�ʱ
		BeCalled_Run.timer = 0;
		OS_StopTimer(&timer_becalled);
		
		callback_id = Get_BeCalled_ToForceClose_CallbackID(BeCalled_Run.call_type);
		if (callback_id)
		{
			(*TABLE_CALLTYPE_BECALLED_TOFORCECLOSE_CALLBACK[callback_id-1].callback)(msg);
		}
	}

	IpBeCalled_Business_Rps(msg,0);
}

/*********************************************************************************************************
**  End Of File
*********************************************************************************************************/
