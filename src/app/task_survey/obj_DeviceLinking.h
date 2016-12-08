/**
  ******************************************************************************
  * @file    obj_DeviceLinking.h
  * @author  czb
  * @version V00.01.00
  * @date    2016.5.31
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  * <h2><center>&copy; COPYRIGHT 2016 V-Tec</center></h2>
  ******************************************************************************
  */ 

#ifndef _obj_DeviceLinking_H
#define _obj_DeviceLinking_H

#include "task_survey.h"

// Define Object Property-------------------------------------------------------
#define CHECK_STATE_IDLE	0
#define CHECK_STATE_BUSY	1

#define UATR_MSG_TYPE_LINKING		104
#define UATR_MSG_TYPE_SYNCLINKING	105		//czn_20160629

typedef struct
{	
	uint16	device;			//�豸
	uint8	result;			//�����
}LINK_RESULT;

typedef struct
{	
	uint8		CheckState;			//���״̬
	uint8		CheckStopFlag;		//���ֹͣ���
	uint16		deviceNum;			//��Ҫ����豸����
	uint16		currentDevice;		//��ǰ���ڼ����豸���
	uint16		*deviceAddr;			//����豸�߼���ַ���豸����
	OS_TIMER 	checkNextTimer;		//�����һ���豸��ʱ
}LINK_PROPERTY;

// Define Object Function - Public----------------------------------------------
void DeviceLinkingInit(void);
void LinkingMultipleDevices(uint16 deviceNum, uint16 *deviceAddr);
LINK_RESULT LinkingDeviceResponse(uint16 logicAddr, uint8 result);
void StopLinking(void);


// Define Object Function - Private---------------------------------------------
void CheckNextDeviceTimerCallBack(void);
void DeviceLinking_Standard_Processing(uint16 deviceAddr);
int Device_SyncLinking(uint16 logicAddr);			//czn_20160629

#endif
