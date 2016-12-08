
#include "call_trace.h"

#include "task_survey/uart_udp_msg_process.h"

// �����������ݰ���������Ҫ�����ͣ�ÿ�����͵Ĺ������Ϳ��ܲ�ͬ
int  call_trace_type_input_filer(unsigned char* pdat, int len, call_trace_data_type *pfiler_type)
{
	/*
	C5_IPG_PACK* 	pPack 	= (C5_IPG_PACK*)pdat;
	unsigned short* pcmd 	= (unsigned short*)pPack->dat;
	switch( pPack->target.type )
	{
		case DATA_SINGLE:			// ����ת����udp
			api_udp_transfer_send_data(pPack->target.ip,pbuf,len);
			break;
		
		case AUDIO_RUN: 			// ������Ƶ
			break;
			
		case DATA_AUDIO_RUN:		// ������Ƶ+����ת��
			api_udp_transfer_send_data(pPack->target.ip,pbuf,len);
			api_udp_transfer2_send_data(pPack->target.ip,pbuf,len);
			API_talk_on_by_unicast(pPack->target.ip,AUDIO_CLIENT_UNICAST_PORT, AUDIO_SERVER_UNICAST_PORT);
			break;
			
		case AUDIO_STOP:			// ֹͣ��Ƶ
			API_talk_off(); 		
			break;
			
		case DATA_AUDIO_STOP:		// ֹͣ��Ƶ+����ת��
			api_udp_transfer_send_data(pPack->target.ip,pbuf,len);
			api_udp_transfer2_send_data(pPack->target.ip,pbuf,len);
			API_talk_off(); 		
			break;
			
		case VIDEO_REC_RUN: 		// ������Ƶ����(����)
			API_VIDEO_C_SERVICE_TURN_ON_MULTICAST(pPack->target.ip,1000);
			break;
			
		case DATA_VIDEO_REC_RUN:	// ������Ƶ����(����)+����ת��
			api_udp_transfer_send_data(pPack->target.ip,pbuf,len);
			api_udp_transfer2_send_data(pPack->target.ip,pbuf,len);
			API_VIDEO_C_SERVICE_TURN_ON_MULTICAST(pPack->target.ip,1000);				
			break;
			
		case VIDEO_REC_STOP:		// ֹͣ��Ƶ����(����)
			API_VIDEO_C_SERVICE_TURN_OFF_MULTICAST();
			break;
			
		case DATA_VIDEO_REC_STOP:	// ֹͣ��Ƶ����(����)+����ת��
			api_udp_transfer_send_data(pPack->target.ip,pbuf,len);
			api_udp_transfer2_send_data(pPack->target.ip,pbuf,len);
			API_VIDEO_C_SERVICE_TURN_OFF_MULTICAST();
			break;
			
		case VIDEO_TRA_RUN: 		// ������Ƶ����(����)
			break;
			
		case DATA_VIDEO_TRA_RUN:	// ������Ƶ����(����)+����ת��
			api_udp_transfer_send_data(pPack->target.ip,pbuf,len);
			api_udp_transfer2_send_data(pPack->target.ip,pbuf,len);
			break;
			
		case VIDEO_TRA_STOP:		// ֹͣ��Ƶ����(����)
			break;
			
		case DATA_VIDEO_TRA_STOP:	// ֹͣ��Ƶ����(����)+����ת��
			api_udp_transfer_send_data(pPack->target.ip,pbuf,len);
			api_udp_transfer2_send_data(pPack->target.ip,pbuf,len);
			break;
			
		case ARUN_VREC_RUN: 		// ������Ƶ+������Ƶ����(����)
			API_VIDEO_C_SERVICE_TURN_ON_MULTICAST(pPack->target.ip,1000);
			API_talk_on_by_unicast(pPack->target.ip,AUDIO_SERVER_UNICAST_PORT, AUDIO_CLIENT_UNICAST_PORT);							
			break;
			
		case DATA_ARUN_VREC_RUN:	// ������Ƶ+������Ƶ����(����)+����ת��
			api_udp_transfer_send_data(pPack->target.ip,pbuf,len);
			api_udp_transfer2_send_data(pPack->target.ip,pbuf,len);
			API_talk_on_by_unicast(pPack->target.ip,AUDIO_CLIENT_UNICAST_PORT, AUDIO_SERVER_UNICAST_PORT);
			API_VIDEO_C_SERVICE_TURN_ON_MULTICAST(pPack->target.ip,1000);
			break;
			
		case ASTP_VREC_STP: 		// ֹͣ��Ƶ+ֹͣ��Ƶ����(����)
			API_talk_off();
			API_VIDEO_C_SERVICE_TURN_OFF_MULTICAST();
			break;
			
		case DATA_ASTP_VREC_STP:	// ֹͣ��Ƶ+ֹͣ��Ƶ����(����)+����ת��
			api_udp_transfer_send_data(pPack->target.ip,pbuf,len);
			api_udp_transfer2_send_data(pPack->target.ip,pbuf,len);
			API_talk_off();
			API_VIDEO_C_SERVICE_TURN_OFF_MULTICAST();
			break;
			
		case ARUN_VTRA_RUN: 		// ������Ƶ+������Ƶ����(����)
			API_talk_on_by_unicast(pPack->target.ip,AUDIO_CLIENT_UNICAST_PORT, AUDIO_SERVER_UNICAST_PORT);
			break;
			
		case DATA_ARUN_VTRA_RUN:	// ������Ƶ+������Ƶ����(����)+����ת��
			api_udp_transfer_send_data(pPack->target.ip,pbuf,len);
			api_udp_transfer2_send_data(pPack->target.ip,pbuf,len);
			API_talk_on_by_unicast(pPack->target.ip,AUDIO_CLIENT_UNICAST_PORT, AUDIO_SERVER_UNICAST_PORT);
			break;
			
		case ASTP_VTRA_STP: 		// ֹͣ��Ƶ+ֹͣ��Ƶ����(����)
			API_talk_off();
			break;
			
		case DATA_ASTP_VTRA_STP:	// ֹͣ��Ƶ+ֹͣ��Ƶ����(����)+����ת��
			api_udp_transfer_send_data(pPack->target.ip,pbuf,len);
			api_udp_transfer2_send_data(pPack->target.ip,pbuf,len);
			API_talk_off();
			break;
														
		case DATA_ASTP_VTRA_RUN:	// ����ת����ͬʱ��Ƶֹͣ����Ƶ����
			api_udp_transfer_send_data(pPack->target.ip,pbuf,len);
			api_udp_transfer2_send_data(pPack->target.ip,pbuf,len);
			API_talk_off();
			break;
			
		case DATA_ASTP_VALL_STP:	// ����ת����ͬʱ��Ƶֹͣ����Ƶ���շ���ֹͣ
			api_udp_transfer_send_data(pPack->target.ip,pbuf,len);
			api_udp_transfer2_send_data(pPack->target.ip,pbuf,len);
			API_talk_off();
			API_VIDEO_C_SERVICE_TURN_OFF_MULTICAST();												
			break;	
	}	
	*/
	return 0;
}

// ����������ݰ���������Ҫ�����ͣ�ÿ�����͵Ĺ������Ϳ��ܲ�ͬ
int  call_trace_type_output_filer(unsigned char* pdat, int len, call_trace_data_type *pfiler_type)
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
int m_CallTraceData[trace_type_max]={0};
int	m_CallTraceSuccStepCnt = 0;
int m_CallTraceSuccResult = 0;

const call_trace_data_type CALL_TRACE_BUSINESS_SUCC_STEP_LIST_TAB[]=
{
	trace_call_apply_number,
	trace_call_reply_number,
	
	trace_video_on_apply_number,
	trace_video_on_reply_number,
	
	trace_video_on_number,
	
	trace_talk_apply_number,
	trace_talk_reply_number,
	
	trace_audio_on_number,
	
	trace_unlock_apply_number,
	trace_unlock_reply_number,
	
	trace_close_apply_number,
	trace_close_reply_number,
	
	trace_video_off_apply_number,
	trace_video_off_reply_number,
	
	trace_video_off_number,
	trace_audio_off_number,
};

// ��Чҵ������ķ�������
int  call_trace_anaysis_data_type_stream(call_trace_data_type  type)
{
	// �Ǽ��������͵����ݰ�
	m_CallTraceData[type]++;

	// ������ȷ�ĺ���ҵ�����ݰ����Ǽ���ȷ�ĺ���ҵ�����
	if( type == CALL_TRACE_BUSINESS_SUCC_STEP_LIST_TAB[m_CallTraceSuccStepCnt] )
	{
		m_CallTraceSuccStepCnt++;
		if( m_CallTraceSuccStepCnt >= (sizeof(CALL_TRACE_BUSINESS_SUCC_STEP_LIST_TAB)/sizeof(call_trace_data_type)) )
		{
			m_CallTraceSuccResult++;
		}
	}
	else
	{
		m_CallTraceSuccStepCnt = 0;
	}	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ⲿAPI
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ͳ��������λ
int  call_trace_anaysis_data_reset(void)
{
	int i;
	for( i = 0; i < trace_type_max; i++ )
	{
		m_CallTraceData[i] = 0;
	}

	m_CallTraceSuccResult = 0;
	
	return 0;
}

// ��������������
int  call_trace_anaysis_data_input(unsigned char* pdat, int len)
{
	call_trace_data_type input_type;
	
	if( call_trace_type_input_filer(pdat,len,&input_type) == 0 )
	{
		call_trace_anaysis_data_type_stream(input_type);
	}
	
	return 0;
}

// �������������
int  call_trace_anaysis_data_output(unsigned char* pdat, int len)
{
	call_trace_data_type output_type;
	
	if( call_trace_type_output_filer(pdat,len,&output_type) == 0 )
	{
		call_trace_anaysis_data_type_stream(output_type);
	}
	
	return 0;
}



