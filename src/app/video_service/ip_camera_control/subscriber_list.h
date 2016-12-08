
#ifndef _SUBSCRIBER_LIST_H
#define _SUBSCRIBER_LIST_H

#include <pthread.h>

// ����״̬��
typedef enum
{
	IDLE,
	ACTIVATE_SREVICE,
	IN_SERVICE,
	DEACTIVATE_SERVICE,
} subscriber_state;

typedef struct	_subscriber_data_t
{
	int				reg_ip;			// �ǼǵĿͻ���IP
	struct timeval	reg_tv;			// �ǼǵĿͻ�������ʱ�� 	
	int				reg_period;		// �ǼǵĿͻ������벥��ʱ�� (s)
	int				reg_timer;		// �ǼǵĿͻ��˶�ʱ��
} subscriber_data;

#define MAX_SUBUSCRIBER_LIST_COUNT		50

typedef struct	_subscriber_list_t
{	
	subscriber_data		dat[MAX_SUBUSCRIBER_LIST_COUNT];
	int					counter;
    pthread_mutex_t 	lock;	
} subscriber_list;

int init_subscriber_list( void );
int activate_subscriber_list( int reg_ip, int reg_period );
int deactivate_subscriber_list( int reg_ip );
int get_total_subscriber_list(void);


#endif

