

#ifndef _SLIPFRAME_H
#define _SLIPFRAME_H

#define SLIP_FRAME_MAX_FIFO    	2048
#define SLIP_FRAME_MAX_R_IN		500
#define SLIP_FRAME_MAX_R_OUT	500
#define SLIP_FRAME_MAX_T_IN		500
#define SLIP_FRAME_MAX_T_OUT	500

#define SLIP_FRAME_END			0xc0
#define SLIP_FRAME_ESC			0xdb

typedef struct Slip_Frame_Info_tag
{
	//rx fifo
	int 			rx_fifo_w;
	int	 		rx_fifo_r;
	char 		rx_fifo[SLIP_FRAME_MAX_FIFO];
	sem_t 		rx_in_sem;						// �����ź���
	pthread_mutex_t 	lock;	
	//rx frame process
	int 			rx_in_cnt;							// ���������ֽڼ���
	char 		rx_in_buf[SLIP_FRAME_MAX_R_IN];		//��������������
	int 			rx_out_cnt;							// ��������ֽڼ���
	char 		rx_out_buf[SLIP_FRAME_MAX_R_OUT];	//�������������
	//tx frame process
	int	 		tx_in_cnt;							// ���������ֽڼ���
	char 		tx_in_buf[SLIP_FRAME_MAX_T_IN];		//��������������
	int	 		tx_out_cnt;							// ��������ֽڼ���
	char 		tx_out_buf[SLIP_FRAME_MAX_T_OUT];	//�������������
} Slip_Frame_Info, *p_Slip_Frame_Info;

void slip_push_fifo(p_Slip_Frame_Info pSlipFrame, char* pDat, int len);
int slip_pop_fifo(p_Slip_Frame_Info pSlipFrame, char* pCh);

int slip_frame_process_init( p_Slip_Frame_Info pSlipFrame );
int slip_frame_process_exit( p_Slip_Frame_Info pSlipFrame );


/*******************************************************************************************
 * @fn:		SlipTrsFrameProcess
 *
 * @brief:	slip��������֡����
 *
 * @param:  	pBuffer - ����������֡��ָ��, len - ���͵����ݰ�����
 *
 * @return: 	��Ч����֡�ĳ���
 *******************************************************************************************/
int SlipTrsFrameProcess( p_Slip_Frame_Info pSlipFrame );

/*******************************************************************************************
 * @fn:		SlipRecFrameProcess
 *
 * @brief:	slip��������֡����
 *
 * @param:  	pSlipFrame - �������
 *
 * @return: 	0/�����ݣ�1/������
 *******************************************************************************************/
int SlipRecFrameProcess( p_Slip_Frame_Info pSlipFrame );

#endif

