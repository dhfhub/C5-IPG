
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

#include "utility.h"
#include "slipframe.h"

void slip_push_fifo(p_Slip_Frame_Info pSlipFrame, char* pDat, int len)
{	
	int i;

	pthread_mutex_lock(&(pSlipFrame->lock));
	
	for( i = 0; i < len; i++ )
	{
		pSlipFrame->rx_fifo[pSlipFrame->rx_fifo_w++] = *pDat++;
		
		if ( pSlipFrame->rx_fifo_w >= SLIP_FRAME_MAX_FIFO) 
		{
		    	pSlipFrame->rx_fifo_w = 0;
		}
        }

	pthread_mutex_unlock(&(pSlipFrame->lock));
	
	sem_post(&(pSlipFrame->rx_in_sem));
}

int slip_pop_fifo(p_Slip_Frame_Info pSlipFrame, char* pCh)
{
	int ret;
	pthread_mutex_lock(&(pSlipFrame->lock));

	if (pSlipFrame->rx_fifo_r == pSlipFrame->rx_fifo_w) 
	{
		ret = 0;
	}
	else
	{
		*pCh = pSlipFrame->rx_fifo[pSlipFrame->rx_fifo_r++];
		
		if ( pSlipFrame->rx_fifo_r >= SLIP_FRAME_MAX_FIFO ) 
		{
			pSlipFrame->rx_fifo_r = 0;
		}
		ret = 1;
	}
	pthread_mutex_unlock(&(pSlipFrame->lock));	
	return ret;
}

/*******************************************************************************************
 * @fn:		SetOneSlipFrame
 *
 * @brief:	ת��һ�����ݰ���SLIP�����ݸ�ʽ
 *
 * @param:  	Dst - Ŀ��������ָ�룬Src - Դ������ָ�룬Size - ת�������ݴ�С
 *
 * @return: 	Dst����������Ч���ݳ���
 *******************************************************************************************/
short SetOneSlipFrame(unsigned char *Dst, unsigned char *Src, short Size)
{
	short i,size_frame;
	
	size_frame = 0;

	if( Size <= 0 )
		return 0;
	
	Dst[size_frame++] = SLIP_FRAME_END;
	
	for( i = 0; i < Size; i++ )
	{
		if( Src[i] == SLIP_FRAME_END )
		{
			Dst[size_frame++] = SLIP_FRAME_ESC;
			Dst[size_frame++] = SLIP_FRAME_ESC+1;
		}
		else if( Src[i] == SLIP_FRAME_ESC )
		{
			Dst[size_frame++] = SLIP_FRAME_ESC;
			Dst[size_frame++] = SLIP_FRAME_ESC+2;
		}
		else
		{
			Dst[size_frame++] = Src[i];
		}
	}
	Dst[size_frame++] = SLIP_FRAME_END;
	return size_frame;
}

/*******************************************************************************************
 * @fn:		GetOneSlipFrame
 *
 * @brief:	��һ��SLIP���ݰ���ԭ����Ч���ݰ�
 *
 * @param:  Dst - Ŀ��������ָ�룬Src - Դ������ָ�룬size - ת�������ݴ�С
 *
 * @return: Dst����������Ч���ݳ���
 *******************************************************************************************/
short GetOneSlipFrame(unsigned char *Dst, unsigned char *Src, short Size)
{
	short i,size_frame;
	
	size_frame = 0;
	for( i = 0; i < Size; i++ )
	{
		if( Src[i] == SLIP_FRAME_END )
		{
			break;
		}
		else if( Src[i] == SLIP_FRAME_ESC )
		{
			i++;
			if( Src[i] == (SLIP_FRAME_ESC+1) )
			{
				Dst[size_frame++] = SLIP_FRAME_END;
			}
			else if( Src[i] == (SLIP_FRAME_ESC+2) )
			{
				Dst[size_frame++] = SLIP_FRAME_ESC;
			}
			else
			{
				continue;	//break;
			}
		}
		else
		{
			Dst[size_frame++] = Src[i];
		}
	}
	return size_frame;
}

/*******************************************************************************************
 * @fn:		SlipTrsFrameProcess
 *
 * @brief:	slip��������֡����
 *
 * @param:  	pBuffer - ����������֡��ָ��, len - ���͵����ݰ�����
 *
 * @return: 	��Ч����֡�ĳ���
 *******************************************************************************************/
int SlipTrsFrameProcess( p_Slip_Frame_Info pSlipFrame )
{
	pSlipFrame->tx_out_cnt = SetOneSlipFrame(pSlipFrame->tx_out_buf, pSlipFrame->tx_in_buf, pSlipFrame->tx_in_cnt );
	return 1;
}

/*******************************************************************************************
 * @fn:		SlipRecFrameProcess
 *
 * @brief:	slip��������֡����
 *
 * @param:  	pSlipFrame - �������
 *
 * @return: 	-1/�����ݣ�0/�����ݣ�1/��Ч���ݰ�
 *******************************************************************************************/
int SlipRecFrameProcess( p_Slip_Frame_Info pSlipFrame )
{
	char ch;
	
	if( !slip_pop_fifo(pSlipFrame,&ch) )
	{
		return -1;
	}
	do
	{
		//
		if( pSlipFrame->rx_in_cnt == 0 )
		{
			if( ch == 0xA1 )
			{
				pSlipFrame->rx_in_buf[pSlipFrame->rx_in_cnt++] = ch;
			}
		}
		else if( pSlipFrame->rx_in_cnt == 1 )
		{
			pSlipFrame->rx_in_buf[pSlipFrame->rx_in_cnt++] = ch;		
		}
		else if( pSlipFrame->rx_in_cnt == 2 )
		{
			// �õ�����
			pSlipFrame->rx_in_buf[pSlipFrame->rx_in_cnt++] = ch;
		}
		else if( pSlipFrame->rx_in_cnt >= 3 )
		{
			pSlipFrame->rx_in_buf[pSlipFrame->rx_in_cnt++] = ch;
			if( pSlipFrame->rx_in_cnt >= pSlipFrame->rx_in_buf[2] )
			{
				memcpy( pSlipFrame->rx_out_buf, pSlipFrame->rx_in_buf, pSlipFrame->rx_in_cnt );				
				pSlipFrame->rx_out_cnt 	= pSlipFrame->rx_in_cnt;
				pSlipFrame->rx_in_cnt	= 0;				
				//printf("----------------get one uart pack,len=%d---------------\n",pSlipFrame->rx_out_cnt);
				return 1;
			}
		}
		/*
		// ʶ���н�����־����
		if( ch == SLIP_FRAME_END )
		{		
			// ����������������ת����ʽ������������ջ����е�����
			if( pSlipFrame->rx_in_cnt )				
			{
				pSlipFrame->rx_out_cnt = GetOneSlipFrame(pSlipFrame->rx_out_buf, pSlipFrame->rx_in_buf,pSlipFrame->rx_in_cnt);
				pSlipFrame->rx_in_cnt = 0;
				return 1;
			}
			// ����������������Ϊ�ս��������������
			else
			{
				return 0;
			}
		}
		// �޽�����־��������ӵ���������
		else
		{
			pSlipFrame->rx_in_buf[pSlipFrame->rx_in_cnt++] = ch;
			if( pSlipFrame->rx_in_cnt >= SLIP_FRAME_MAX_R_IN )
			{
				pSlipFrame->rx_in_cnt = 0;
			}			
		}
		*/
	}while( slip_pop_fifo(pSlipFrame,&ch) );
	
	return 0;
}

/*******************************************************************************************
 *******************************************************************************************/
int slip_frame_process_init( p_Slip_Frame_Info pSlipFrame )
{
	pSlipFrame->rx_fifo_r 	= 0;
	pSlipFrame->rx_fifo_w 	= 0;
	pSlipFrame->rx_in_cnt 	= 0;
	pSlipFrame->rx_out_cnt 	= 0;
	pSlipFrame->tx_in_cnt 	= 0;
	pSlipFrame->tx_out_cnt 	= 0;
		
	if( sem_init(&(pSlipFrame->rx_in_sem), 0, 0) == -1 )
	{
		printf("slip sem init failure!msg:%s\n", strerror(errno));
		return -1;
	}	

	if(pthread_mutex_init(&(pSlipFrame->lock), 0) == -1)
	{
		printf("slip pthread_mutex_init failure!msg:%s\n", strerror(errno));
		sem_destroy(&(pSlipFrame->rx_in_sem));
		return -1;
	}
	return 0;
}

int slip_frame_process_exit( p_Slip_Frame_Info pSlipFrame )
{
	pthread_mutex_destroy(&(pSlipFrame->lock));	
	sem_destroy(&(pSlipFrame->rx_in_sem));
	return 0;
}

