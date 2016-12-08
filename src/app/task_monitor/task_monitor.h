

#ifndef _TASK_MONITOR_
#define _TASK_MONITOR_

#include "../task_survey/task_survey.h"

typedef enum
{
	MONITOR_IDLE,
	MONITOR_LOCAL,
	MONITOR_REMOTE,
} MONITOR_STATE;


typedef struct
{
	MONITOR_STATE	state;				// ״̬
	int				target_ip;			// Զ�̼���ip
	int				device_id;			// Զ���豸��
	int				period;				// ����ʱ��	
} monitor_sbu;

int open_monitor_local( void );
int open_monitor_remote( int ip );
int close_monitor( void );

#endif

