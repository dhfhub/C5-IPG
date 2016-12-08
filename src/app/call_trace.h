
#ifndef _CALL_TRACE_H
#define _CALL_TRACE_H


// ������������
typedef enum
{
	trace_call_apply_number = 0,	// �����������
	trace_call_reply_number,		// ���лظ�����
	trace_video_on_apply_number,	// ��Ƶ�����������
	trace_video_on_reply_number,	// ��Ƶ�����ظ�����
	trace_video_on_number,			// ��Ƶ��������
	trace_talk_apply_number,		// ͨ�������������
	trace_talk_reply_number,		// ͨ�������ظ�����
	trace_audio_on_number,			// ��Ƶ��������
	trace_unlock_apply_number,		// �����������
	trace_unlock_reply_number,		// �����ظ�����
	trace_close_apply_number,		// ���йر��������
	trace_close_reply_number,		// ���йرջظ�����
	trace_video_off_apply_number,	// ��Ƶ�ر��������
	trace_video_off_reply_number,	// ��Ƶ�رջظ�����
	trace_video_off_number,			// ��Ƶ�رմ���
	trace_audio_off_number, 		// ��Ƶ�رմ���
	trace_type_max					// �Զ�������������
} call_trace_data_type; 

// ͳ��������λ
int  call_trace_anaysis_data_reset(void);
// ��������������
int  call_trace_anaysis_data_input(unsigned char* pdat, int len);
// �������������
int  call_trace_anaysis_data_output(unsigned char* pdat, int len);


#endif


