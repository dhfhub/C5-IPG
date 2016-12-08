/**
  ******************************************************************************
  * @file    obj_Caller_Data.c
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
/*
#include "../task_Phone.h"
#include "task_Caller.h"
#include "obj_Caller_State.h"
#include "obj_Caller_Data.h"
#include "./obj_Caller_ByType_Callback/obj_Caller_IPGVtkCall_Callback.h"
//#include "obj_Caller_IPGCallIPG_Callback.h"
*/
#include "../../task_survey/obj_CallServer_Virtual/obj_CallServer_Virtual.h"
#include "task_IpCaller.h"
#include "obj_IpCaller_State.h"
#include "obj_IpCaller_Data.h"
#include "./obj_IpCaller_ByType_Callback/obj_IpCaller_VtkUnicastCall_Callback.h"


const STRUCT_IPCALLER_CALLTYPE_CALLBACK TABLE_CALLTYPE_CALLER_TOREDIAL_CALLBACK[]={
	//calltype					//callback


	{IpCallType_VtkUnicast, 			Callback_Caller_ToRedial_VtkUnicastCall,},		//С���������зֻ�

};
const uint8 TABLE_CALLTYPE_CALLER_TOREDIAL_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_CALLER_TOREDIAL_CALLBACK ) \
													/ sizeof( TABLE_CALLTYPE_CALLER_TOREDIAL_CALLBACK[0] );

const STRUCT_IPCALLER_CALLTYPE_CALLBACK TABLE_CALLTYPE_CALLER_TOINVITE_CALLBACK[]={
	//calltype					//callback

	{IpCallType_VtkUnicast, 		Callback_Caller_ToInvite_VtkUnicastCall,},		//С���������зֻ�

	
};
const uint8 TABLE_CALLTYPE_CALLER_TOINVITE_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_CALLER_TOINVITE_CALLBACK ) \
													/ sizeof( TABLE_CALLTYPE_CALLER_TOINVITE_CALLBACK[0] );

const STRUCT_IPCALLER_CALLTYPE_CALLBACK TABLE_CALLTYPE_CALLER_TORINGING_CALLBACK[]={
	//calltype					//callback
	
	{IpCallType_VtkUnicast, 			Callback_Caller_ToRinging_VtkUnicastCall,},		//С���������зֻ�
	
	
};
const uint8 TABLE_CALLTYPE_CALLER_TORINGING_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_CALLER_TORINGING_CALLBACK ) \
													/ sizeof( TABLE_CALLTYPE_CALLER_TORINGING_CALLBACK[0] );

const STRUCT_IPCALLER_CALLTYPE_CALLBACK TABLE_CALLTYPE_CALLER_TOACK_CALLBACK[]={
	//calltype					//callback

	{IpCallType_VtkUnicast, 		Callback_Caller_ToAck_VtkUnicastCall,},			//С���������зֻ�
	
	
};

const uint8 TABLE_CALLTYPE_CALLER_TOACK_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_CALLER_TOACK_CALLBACK ) \
													/ sizeof( TABLE_CALLTYPE_CALLER_TOACK_CALLBACK[0] );

const STRUCT_IPCALLER_CALLTYPE_CALLBACK TABLE_CALLTYPE_CALLER_TOBYE_CALLBACK[]={
	//calltype					//callback
	
	{IpCallType_VtkUnicast, 			Callback_Caller_ToBye_VtkUnicastCall,},			//С���������зֻ�
	
	
};
const uint8 TABLE_CALLTYPE_CALLER_TOBYE_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_CALLER_TOBYE_CALLBACK ) \
													/ sizeof( TABLE_CALLTYPE_CALLER_TOBYE_CALLBACK[0] );

const STRUCT_IPCALLER_CALLTYPE_CALLBACK TABLE_CALLTYPE_CALLER_TOWAITING_CALLBACK[]={
	//calltype					//callback

	{IpCallType_VtkUnicast, 		Callback_Caller_ToWaiting_VtkUnicastCall,},		//С���������зֻ�
	
	
};
const uint8 TABLE_CALLTYPE_CALLER_TOWAITING_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_CALLER_TOWAITING_CALLBACK ) \
													/ sizeof( TABLE_CALLTYPE_CALLER_TOWAITING_CALLBACK[0] );
const STRUCT_IPCALLER_CALLTYPE_CALLBACK TABLE_CALLTYPE_CALLER_TOUNLOCK_CALLBACK[]={
	//calltype					//callback
	
	{IpCallType_VtkUnicast, 		Callback_Caller_ToUnlock_VtkUnicastCall,},		//С���������зֻ�
	
};
const uint8 TABLE_CALLTYPE_CALLER_TOUNLOCK_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_CALLER_TOUNLOCK_CALLBACK ) \
													/ sizeof( TABLE_CALLTYPE_CALLER_TOUNLOCK_CALLBACK[0] );

const STRUCT_IPCALLER_CALLTYPE_CALLBACK TABLE_CALLTYPE_CALLER_TOTIMEOUT_CALLBACK[]={
	//calltype					//callback

	{IpCallType_VtkUnicast, 		Callback_Caller_ToTimeout_VtkUnicastCall,},		//С���������зֻ�
	
	
};
const uint8 TABLE_CALLTYPE_CALLER_TOTIMEOUT_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_CALLER_TOTIMEOUT_CALLBACK ) \
													/ sizeof( TABLE_CALLTYPE_CALLER_TOTIMEOUT_CALLBACK[0] );

const STRUCT_IPCALLER_CALLTYPE_CALLBACK TABLE_CALLTYPE_CALLER_TOERROR_CALLBACK[]={
	//calltype					//callback

	{IpCallType_VtkUnicast, 		Callback_Caller_ToError_VtkUnicastCall,},		//С���������зֻ�
	
	
};
const uint8 TABLE_CALLTYPE_CALLER_TOERROR_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_CALLER_TOERROR_CALLBACK ) \
													/ sizeof( TABLE_CALLTYPE_CALLER_TOERROR_CALLBACK[0] );

const STRUCT_IPCALLER_CALLTYPE_CALLBACK TABLE_CALLTYPE_CALLER_TOFORCECLOSE_CALLBACK[]={
	//calltype					//callback

	{IpCallType_VtkUnicast, 		Callback_Caller_ToForceClose_VtkUnicastCall,},		//С���������зֻ�
	
	
};
const uint8 TABLE_CALLTYPE_CALLER_TOFORCECLOSE_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_CALLER_TOFORCECLOSE_CALLBACK ) \
													/ sizeof( TABLE_CALLTYPE_CALLER_TOFORCECLOSE_CALLBACK[0] );

CALLER_CONFIG	Caller_Config;
/*------------------------------------------------------------------------
				��ȡ�������Ͷ�Ӧ��Callback�����ڱ��е�������
���:  
	��������

����: 
	1~0xff = ���� , ��ֵΪ��TABLE_CALLTYPE_TORINGING_CALLBACK��������+1
	0x00 = ����
------------------------------------------------------------------------*/
uint8 Get_Caller_ByType_CallbackID_Common(const STRUCT_IPCALLER_CALLTYPE_CALLBACK *pcallback,
										  const uint8 callback_length,
										  uint8 call_type)
{
	uint8 i;
	
	//��������ƥ���call_type, ȡ��������
	for (i=0; i<callback_length; i++)
	{
		if (pcallback[i].call_type == call_type)
		{
			return (i+1);
		}
	}
	
	return (0);
}
/*------------------------------------------------------------------------
						Caller_Data_init
���:  
	��

����:
	��ʼ��
------------------------------------------------------------------------*/
void Caller_Data_Init(void)		//R_
{
	//czn_20150609
	Caller_Config.limit_invite_time = 8;
	Caller_Config.limit_ringing_time = 1000;
	Caller_Config.limit_ack_time = 1000;
	Caller_Config.limit_bye_time = 5;
//	API_Event_IoServer_InnerRead_All(CALLER_INVITE_TIME_LIMIT, (uint8*)&Caller_Config.limit_invite_time);			//��ȴ�Ӧ��ʱ��				//bdu_zxj	
//	API_Event_IoServer_InnerRead_All_Aligning(CALLER_RINGING_TIME_LIMIT, (uint8*)&Caller_Config.limit_ringing_time);//��ȴ�ժ��ʱ��	
//	API_Event_IoServer_InnerRead_All_Aligning(CALLER_ACK_TIME_LIMIT, (uint8*)&Caller_Config.limit_ack_time);		//�ͨ��ʱ��	
//	API_Event_IoServer_InnerRead_All(CALLER_BYE_TIME_LIMIT, (uint8*)&Caller_Config.limit_bye_time);					//����йһ��ȴ�����Ӧ��ʱ��	//bdu_zxj
}



/*********************************************************************************************************
**  End Of File
*********************************************************************************************************/
