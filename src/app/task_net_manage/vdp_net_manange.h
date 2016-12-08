
#ifndef _VDP_NET_MANANGE_H
#define  _VDP_NET_MANANGE_H

#include "vdp_net_manange_process.h"
#include "../vtk_udp_stack/vtk_udp_stack_c5_ipc_cmd.h"

#define VDP_NET_MANANGE_MSG_DIP_SET_IP						0		//dip��������
#define VDP_NET_MANANGE_MSG_NET_TEST						1		//�����������
#define VDP_NET_MANANGE_MSG_NET_RESET						2

#define VDP_NET_MANANGE_MSG_IPG_RESET_OK					80		// n329��λokͨ��

#define VDP_NET_MANANGE_MSG_COMMON_LINK						100		// uart�����豸link
#define VDP_NET_MANANGE_MSG_IPG_LIST_UPDATE_REQ				101		// uart�������list����
#define VDP_NET_MANANGE_MSG_IPG_LIST_READ_REQ				102		// uart�����ȡlist����
#define VDP_NET_MANANGE_MSG_IPG_ONLINE_CHECK				103		// uart�����������߼��
#define VDP_NET_MANANGE_MSG_IPG_REPEAT_CHECK				104		// uart���뱾��ip�ظ����

#define VDP_NET_MANANGE_REMOTE_COMMON_LINK					110		// udp�����豸link
#define VDP_NET_MANANGE_REMOTE_IPG_LIST_UPDATE_REQ			111		// udp�������list����
#define VDP_NET_MANANGE_REMOTE_IPG_ONLINE_CHECK				113		// udp�����������߼��
#define VDP_NET_MANANGE_REMOTE_IPG_REPEAT_CHECK				114		// udp���뱾��ip�ظ����


#pragma pack(1)

/*------------------dip �Ȳ�������-----------------------*/
typedef struct 
{
	VDP_MSG_HEAD	head;
	union 
	{
		unsigned char data;				// DIP��������ʱ�Ĳ���
		unsigned char macadd[18];			// MAC���ò���
		unsigned char ipadd[18];			// IP��ַ���ò���
		unsigned char maskadd[18];		// �������ò���
		unsigned char gwadd[18];			// �������ò���
	}msg_data;
} NetManange_Para_type;

/*-------------------�����������------------------------*/
#define TIME_OUT				0
#define GOOD					1
#define GENERAL 					2
#define BAD 						3

typedef struct 
{
	VDP_MSG_HEAD	head;
	unsigned char 	net_state;			// 0/ 
} NetManange_net_test;

/*-------------------ipg �б����--------------------------*/
typedef struct 
{
	VDP_MSG_HEAD	head;
	int				ip;
	unsigned char		state;
} NetManange_ipg_list_update;

/*-------------------ipg Ŀ�걨��--------------------------*/
typedef struct 
{
	VDP_MSG_HEAD	head;
	int				asker;
	char 			ip[16];			// IP��ַ�ַ�����eg: [192.168.010.009\0]
	char 			mac[18];		// MAC���ò�����eg: [00:12:48:17:55:66\0]	
} NetManange_ipg_list_report;

/*-------------------ipg��λiok------------------------------*/
typedef struct 
{
	VDP_MSG_HEAD	head;
	unsigned char 	msg_data;			// reset ok msg data
} NetManange_reset_ok;

/*-------------------ipg�豸����-----------------------------*/
typedef struct 
{
	VDP_MSG_HEAD	head;
	
	int		  		targetIP;			//Ŀ��IP��ַ
	int		  		sourcetIP;			//ԴIP��ַ
	unsigned short	targetDevID;		//Ŀ���豸��ַ
	unsigned short	sourceDevID;		//Դ�豸��ַ

	unsigned char		call_type;			// ��������
	
	//link result
	unsigned char		link_state;
	
} NetManange_Link_type;

/*--------------------ipg list��¼��ȡ-----------------------*/
#define IPG_NUM_ONE_PACKAGE	5
#define IPG_IP_STR_BUF_LEN		16
#define IPG_MAC_STR_BUF_LEN	18

typedef	struct 
{
	unsigned char		attr;			// ipg list �ڵ������ֵ:   0/������1/ip ��ַ�ظ���2/mac��ַ�ظ�, 3/ both repeat
	char 			ip[16];			// IP��ַ�ַ�����eg: [192.168.010.009\0]
	char 			mac[18];		// MAC���ò�����eg: [00:12:48:17:55:66\0]
}ipg_list_node;

typedef struct 
{
	VDP_MSG_HEAD	head;	
	unsigned char		list_max;		//ipg list ���ܸ���
	unsigned char		list_off;			//ipg list ��ǰ�������ƫ����
	unsigned char		list_cnt;			//ipg list ��ǰ������ĸ���
	ipg_list_node		list_node[IPG_NUM_ONE_PACKAGE];		//ipg list ��ǰ�����������
} NetManange_ipg_list_read;

/*----------------------ipg ���߼��-------------------------*/
typedef struct 
{
	VDP_MSG_HEAD	head;	
	int		  		targetIP;			//Ŀ��IP��ַ
	unsigned char		online_state;		//����״̬
} NetManange_ipg_online_check;

/*----------------------ipg �ظ����-------------------------*/
typedef struct 
{
	VDP_MSG_HEAD	head;	
	int		  		targetIP;			//Ŀ��IP��ַ
	unsigned char		repeat_state;		//����״̬
} NetManange_ipg_repeat_check;

/*-----------------------------------------------------------*/

#pragma pack()

void vtk_TaskInit_net_manang(void);
void exit_vdp_net_manange_task(void);


unsigned char ResetNetWork( void );


/*******************************************************************************************
 * @fn:		API_net_manange_Udp_Link_Request
 *
 * @brief:	����ͨ��linkָ�����
 *
 * @param:  	target_ip 	- Ŀ��ip��ַ
 * @param:  	tdev_id 		- Ŀ���豸��ַ
 * @param:  	sdev_id 		- Դ�豸��ַ
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int API_net_manange_Udp_Link_Request( unsigned char call_type, int target_ip, unsigned char tdev_id, unsigned char sdev_id );

/*******************************************************************************************
 * @fn:		API_net_manange_Udp_Link_Reply
 *
 * @brief:	����ͨ��link�ظ�ָ�����
 *
 * @param:  	msg_id 		- ���󷽵���Ϣid
 * @param:  	target_ip 		- Ŀ��ip��ַ
 * @param:  	tdev_id 		- Ŀ���豸��ַ
 * @param:  	sdev_id 		- Դ�豸��ַ
 * @param:  	state 		- ����״̬
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int API_net_manange_Udp_Link_Reply( unsigned char msg_id, unsigned char call_type,  int target_ip, unsigned char tdev_id, unsigned char sdev_id, unsigned char state );

/*******************************************************************************************
 * @fn:		API_net_manange_Uart_IPG_List_Read_Request
 *
 * @brief:	�ظ���������ipg list������
 *
 * @param:  	pNetManage - ���󷽵���Ϣ
 *
 * @return: 	-1/err, other/state
*******************************************************************************************/
int API_net_manange_Uart_IPG_List_Read_Reply( NetManange_ipg_list_read* pNetManage );


/*******************************************************************************************
 * @fn:		API_net_manange_Udp_OnLine_Check_Reply
 *
 * @brief:	uart �ظ��豸�������
 *
 * @param:  	msg_id 		- ���󷽵���Ϣid
 * @param:  	target_ip 		- Ŀ��ip��ַ
 * @param:  	state 		- ����״̬
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int API_net_manange_Uart_OnLine_Check_Reply( unsigned char msg_id, int target_ip, unsigned char state );

/*******************************************************************************************
 * @fn:		API_net_manange_Udp_OnLine_Check_Request
 *
 * @brief:	����ͨ�����߼������
 *
 * @param:  	msg_id 		- ���󷽵���Ϣid
 * @param:  	target_ip 		- Ŀ��ip��ַ
 *
 * @return: 	-1/err, other/state
*******************************************************************************************/
int API_net_manange_Udp_OnLine_Check_Request( unsigned char msg_id,   int target_ip );

/*******************************************************************************************
 * @fn:		API_net_manange_Udp_OnLine_Check_Reply
 *
 * @brief:	����ͨ��link�ظ�ָ�����
 *
 * @param:  	msg_id 		- ���󷽵���Ϣid
 * @param:  	target_ip 		- Ŀ��ip��ַ
 * @param:  	state 		- ����״̬
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int API_net_manange_Udp_OnLine_Check_Reply( unsigned char msg_id, int target_ip, unsigned char state );

/*******************************************************************************************
 * @fn:		API_net_manange_Udp_OnLine_Check_Reply
 *
 * @brief:	uart �ظ��豸�������
 *
 * @param:  	msg_id 		- ���󷽵���Ϣid
 * @param:  	target_ip 		- Ŀ��ip��ַ
 * @param:  	state 		- ����״̬
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int API_net_manange_Uart_IPG_Repeat_Reply( unsigned char msg_id, int target_ip, unsigned char state );

/*******************************************************************************************
 * @fn:		API_net_manange_Udp_IPG_Repeat_Request
 *
 * @brief:	����ipg �ظ����߼��
 *
 * @param:  	msg_id 		- ���󷽵���Ϣid
 *
 * @return: 	-1/err, other/state
*******************************************************************************************/
int API_net_manange_Udp_IPG_Repeat_Request( unsigned char msg_id );

/*******************************************************************************************
 * @fn:		API_net_manange_Udp_IPG_Repeat_Reply
 *
 * @brief:	�ظ�ipg repeat
 *
 * @param:  	msg_id 		- ���󷽵���Ϣid
 * @param:  	target_ip 		- Ŀ��ip��ַ
 * @param:  	state 		- ����״̬
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int API_net_manange_Udp_IPG_Repeat_Reply( unsigned char msg_id, int target_ip, unsigned char state );

/*******************************************************************************************
 * @fn:		API_net_manange_Uart_Net_Test_Reply
 *
 * @brief:	uart �ظ��豸�������
 *
 * @param:  	msg_id 		- ���󷽵���Ϣid
 * @param:  	state 		- ��������
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int API_net_manange_Uart_Net_Test_Reply( unsigned char msg_id, unsigned char state );

#endif


