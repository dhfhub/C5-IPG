/**
  ******************************************************************************
  * @file    obj_BeCalled_Data.c
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
#include "obj_BeCalled_Data.h"
#include "obj_BeCalled_State.h"
#include "task_BeCalled.h"
//#include "obj_BeCalled_CdsCallIPG_Callback.h"
#include "./obj_BeCalled_ByType_Callback/obj_BeCalled_IPGVtkCall_Callback.h"
	
*/
#include "../../task_survey/obj_CallServer_Virtual/obj_CallServer_Virtual.h"
#include "task_IpBeCalled.h"
#include "obj_IpBeCalled_Data.h"
#include "obj_IpBeCalled_State.h"

//#include "obj_BeCalled_CdsCallIPG_Callback.h"
#include "./obj_IpBeCalled_ByType_Callback/obj_IpBeCalled_VtkUnicastCall_Callback.h"



BECALLED_CONFIG BeCalled_Config;

const STRUCT_IPBECALLED_CALLTYPE_CALLBACK TABLE_CALLTYPE_BECALLED_TOACK_CALLBACK[]={

	
	{IpCallType_VtkUnicast, 			Callback_BeCalled_ToAck_VtkUnicastCall,},	//С���������зֻ�

};
const uint8 TABLE_CALLTYPE_BECALLED_TOACK_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_BECALLED_TOACK_CALLBACK )\
	/ sizeof( TABLE_CALLTYPE_BECALLED_TOACK_CALLBACK[0] );

const STRUCT_IPBECALLED_CALLTYPE_CALLBACK TABLE_CALLTYPE_BECALLED_TOBYE_CALLBACK[]={
	{IpCallType_VtkUnicast, 		Callback_BeCalled_ToBye_VtkUnicastCall,},	//С���������зֻ�
};
const uint8 TABLE_CALLTYPE_BECALLED_TOBYE_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_BECALLED_TOBYE_CALLBACK )\
	/ sizeof( TABLE_CALLTYPE_BECALLED_TOBYE_CALLBACK[0] );

const STRUCT_IPBECALLED_CALLTYPE_CALLBACK TABLE_CALLTYPE_BECALLED_TOREDIAL_CALLBACK[]={
	//calltype					//callback
	
	{IpCallType_VtkUnicast, 		Callback_BeCalled_ToRedial_VtkUnicastCall,},	//С���������зֻ�
	
	
};
const uint8 TABLE_CALLTYPE_BECALLED_TOREDIAL_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_BECALLED_TOREDIAL_CALLBACK )\
	/ sizeof( TABLE_CALLTYPE_BECALLED_TOREDIAL_CALLBACK[0] );

const STRUCT_IPBECALLED_CALLTYPE_CALLBACK TABLE_CALLTYPE_BECALLED_TORINGING_CALLBACK[]={
	//calltype					//callback
	
	{IpCallType_VtkUnicast, 		Callback_BeCalled_ToRinging_VtkUnicastCall,},	//С���������зֻ�
	
};
const uint8 TABLE_CALLTYPE_BECALLED_TORINGING_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_BECALLED_TORINGING_CALLBACK )\
	/ sizeof( TABLE_CALLTYPE_BECALLED_TORINGING_CALLBACK[0] );

const STRUCT_IPBECALLED_CALLTYPE_CALLBACK TABLE_CALLTYPE_BECALLED_TOTRANSFER_CALLBACK[]={
	//calltype					//callback
	
	{IpCallType_VtkUnicast, 		Callback_BeCalled_ToTransfer_VtkUnicastCall,},	//С���������зֻ�
	
};
const uint8 TABLE_CALLTYPE_BECALLED_TOTRANSFER_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_BECALLED_TOTRANSFER_CALLBACK )\
	/ sizeof( TABLE_CALLTYPE_BECALLED_TOTRANSFER_CALLBACK[0] );	

const STRUCT_IPBECALLED_CALLTYPE_CALLBACK TABLE_CALLTYPE_BECALLED_TOUNLOCK_CALLBACK[]={
	//calltype					//callback
	
	{IpCallType_VtkUnicast, 		Callback_BeCalled_ToUnlock_VtkUnicastCall,},	//С���������зֻ�
	
	
};
const uint8 TABLE_CALLTYPE_BECALLED_TOUNLOCK_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_BECALLED_TOUNLOCK_CALLBACK )\
	/ sizeof( TABLE_CALLTYPE_BECALLED_TOUNLOCK_CALLBACK[0] );

const STRUCT_IPBECALLED_CALLTYPE_CALLBACK TABLE_CALLTYPE_BECALLED_TOTIMEOUT_CALLBACK[]={
	//calltype					//callback
	
	{IpCallType_VtkUnicast, 		Callback_BeCalled_ToTimeout_VtkUnicastCall,},	//С���������зֻ�
	
	
};
const uint8 TABLE_CALLTYPE_BECALLED_TOTIMEOUT_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_BECALLED_TOTIMEOUT_CALLBACK )\
	/ sizeof( TABLE_CALLTYPE_BECALLED_TOTIMEOUT_CALLBACK[0] );

const STRUCT_IPBECALLED_CALLTYPE_CALLBACK TABLE_CALLTYPE_BECALLED_TOWAITING_CALLBACK[]={
	//calltype					//callback
	
	{IpCallType_VtkUnicast, 		Callback_BeCalled_ToWaiting_VtkUnicastCall,},	//С���������зֻ�
	
	
};
const uint8 TABLE_CALLTYPE_BECALLED_TOWAITING_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_BECALLED_TOWAITING_CALLBACK )\
	/ sizeof( TABLE_CALLTYPE_BECALLED_TOWAITING_CALLBACK[0] );

const STRUCT_IPBECALLED_CALLTYPE_CALLBACK TABLE_CALLTYPE_BECALLED_TOERROR_CALLBACK[]={
	//calltype					//callback
	
	{IpCallType_VtkUnicast, 		Callback_BeCalled_ToError_VtkUnicastCall,},	//С���������зֻ�
	
	
};
const uint8 TABLE_CALLTYPE_BECALLED_TOERROR_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_BECALLED_TOERROR_CALLBACK )\
	/ sizeof( TABLE_CALLTYPE_BECALLED_TOERROR_CALLBACK[0] );

const STRUCT_IPBECALLED_CALLTYPE_CALLBACK TABLE_CALLTYPE_BECALLED_TOFORCECLOSE_CALLBACK[]={
	//calltype					//callback
	
	{IpCallType_VtkUnicast, 		Callback_BeCalled_ToForceClose_VtkUnicastCall,},	//С���������зֻ�
	
	
};
const uint8 TABLE_CALLTYPE_BECALLED_TOFORCECLOSE_CALLBACK_NUMBERS = sizeof( TABLE_CALLTYPE_BECALLED_TOFORCECLOSE_CALLBACK )\
	/ sizeof( TABLE_CALLTYPE_BECALLED_TOFORCECLOSE_CALLBACK[0] );


/*------------------------------------------------------------------------
				��ȡ�������Ͷ�Ӧ��Callback�����ڱ��е�������
���:  
	��������

����: 
	1~0xff = ���� , ��ֵΪ��TABLE_CALLTYPE_TORINGING_CALLBACK��������+1
	0x00 = ����
------------------------------------------------------------------------*/
uint8 Get_BeCalled_ByType_CallbackID_Common(const STRUCT_IPBECALLED_CALLTYPE_CALLBACK *pcallback,
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
void BeCalled_Data_Init(void)	//R_	
{
	//czn_20150609
	BeCalled_Config.limit_ringing_time = 1000;
	BeCalled_Config.limit_ack_time = 1000;
	BeCalled_Config.limit_bye_time = 5;
//	API_Event_IoServer_InnerRead_All_Aligning(BECALLED_RINGING_TIME_LIMIT, (uint8*)&BeCalled_Config.limit_ringing_time);	//��ȴ�ʱ��	
//	API_Event_IoServer_InnerRead_All_Aligning(BECALLED_ACK_TIME_LIMIT, (uint8*)&BeCalled_Config.limit_ack_time);			//�ͨ��ʱ��	
//	API_Event_IoServer_InnerRead_All(BECALLED_BYE_TIME_LIMIT, (uint8*)&BeCalled_Config.limit_bye_time);						//����йһ��ȴ�����Ӧ��ʱ��	//bdu_zxj
}

/*********************************************************************************************************
**  End Of File
*********************************************************************************************************/
