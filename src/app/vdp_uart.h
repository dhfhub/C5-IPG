
#ifndef VDP_UART_H
#define  VDP_UART_H

#define 	PACKAGE_TYPE_STRING		0		// �ַ������ݰ�
#define	PACKAGE_TYPE_SLIP			1		// slip���ݰ���ʽ

#define 	PACKAGE_TYPE				PACKAGE_TYPE_STRING
//#define 	PACKAGE_TYPE				PACKAGE_TYPE_SLIP


/*******************************************************************************************
 * @fn:		Init_vdp_uart
 *
 * @brief:	��ʼ��uart�˿�
 *
 * @param:  	none
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int Init_vdp_uart(void);

/*******************************************************************************************
 * @fn:		vdp_uart_send_data
 *
 * @brief:	���ڷ������ݰ�����
 *
 * @param:  	*data	- ������ָ��
 * @param:  	len		- ���ݰ�����
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int vdp_uart_send_data(char *data, int len);

/*******************************************************************************************
 * @fn:		vdp_uart_recv_data
 *
 * @brief:	���ڽ������ݰ�����
 *
 * @param:  	buf - ����ָ��
 * @param:  	len - ���ݳ���
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int vdp_uart_recv_data( char* buf, int len);

/*******************************************************************************************
 * @fn:		close_vdp_uart
 *
 * @brief:	�رմ��ڼ������Դ
 *
 * @param:  	none
 *
 * @return: 	0/ok, -1/err
*******************************************************************************************/
int close_vdp_uart(void);

int api_uart_recv_callback( char* pbuf, unsigned int len );

int API_Send_BusinessRsp_ByUart(unsigned char  target_id,unsigned char  source_id,unsigned short cmd_type,unsigned char *buf,int len);	//czn_20160601
#endif


