

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>      /* basic system data types */
#include <error.h>

#include "video_source_map_tab.h"
#include "../video_multicast_common.h"

// �õ��鲥����ĵ�ַ�Ͷ˿ڣ����Ǵ�����鲥�ͷ���(��Ҫ����) - ����������������Բ���IPʵ�д���
// mcg_addr 	- Ϊ������Ƶ���������鲥��ַ ������������Ƶ����������õ�ַΪ�����������ip��ַ
// pport		- Ϊ������Ƶ�������Ķ˿ں�
// -1: 	err
// 0:	multicast service
int get_server_multicast_addr_port( int* mcg_addr, unsigned short *pport )
{
	char addr[200];
	char str[16];

	// �õ������ֽ����ip��ַ ( ���ֽڶ���, �ȷ��͸��ֽ� ) 
	int my_ip = GetLocalIp();

	// ����ip��ַ�õ��鲥��ַ
	sprintf( addr,"224.0.2.%d", (my_ip>>24)&0xff );
	memset( str, 0, 16 );
	strcpy( str, addr );
	*mcg_addr	= inet_addr(str);

	// ����ip��ַ�õ��鲥�˿ں�	(���ǵ�PC���Ӷ��IPG������£��˿ںŲ��ܸ��ظ����ʲ��ÿɱ�Ķ˿ں�)
	//*pport 		= VIDEO_SERVER_MULTICAST_PORT;	
	unsigned short my_port = (htonl(my_ip))&0x1ff;		// ����512����ַ
	*pport = (VIDEO_SERVER_MULTICAST_PORT + my_port);	
	
	printf("get server multicast addr = %s, port = %d\n",str,*pport);	
	
	return 0;
}


int get_server_multicast_addr_port2( int service_ip, int* mcg_addr, unsigned short *pport )
{
	char addr[200];
	char str[16];

	int my_ip = service_ip;

	// ����ip��ַ�õ��鲥��ַ
	sprintf( addr,"224.0.2.%d", (my_ip>>24)&0xff );
	memset( str, 0, 16 );
	strcpy( str, addr );
	*mcg_addr	= inet_addr(str);

	// ����ip��ַ�õ��鲥�˿ں�	(���ǵ�PC���Ӷ��IPG������£��˿ںŲ��ܸ��ظ����ʲ��ÿɱ�Ķ˿ں�)
	//*pport 		= VIDEO_SERVER_MULTICAST_PORT;	
	unsigned short my_port = (htonl(my_ip))&0x1ff;		// ����512����ַ
	*pport = (VIDEO_SERVER_MULTICAST_PORT + my_port);	
	
	printf("get server multicast addr = %s, port = %d\n",str,*pport);	
	
	return 0;	
}

	
