
#ifndef _UDP_FRAGMENT_OPT_H
#define _UDP_FRAGMENT_OPT_H

#define PACK_MARK  			"SWITCH"
#define PACK_MARK_LEN 		6

#define VIDEO_BUF_COUNT 	10
#define VIDEO_BUF_LEN 		(200*1200)		//1244160  //720*576*3

#define VIDEO_MAX_PACK_NUM  200
#define VIDEO_BUF_FRAME 	1

#define VIDEO_DATA_TYPE		0x8001

typedef int			bool;
#define true		1
#define false		0

typedef struct
{
	unsigned int 	Timestamp;           				//ʱ���
	unsigned short 	FrameNo;           					//֡���
	short 			TotalPackage;               		//�ܰ���
	bool 			PackageReceived[VIDEO_MAX_PACK_NUM];//���յ�����
	unsigned int 	Len;                 				//���ݳ���
	bool 			AllReceived;                 		//���ݰ�ȫ�յ����
	unsigned char 	*Buffer;            				//����
	unsigned short 	FrameType;         					//֡����
} AVPack;

typedef struct _AVPackNode_t
{
	AVPack 					Content;
	struct _AVPackNode_t 	*llink, *rlink;
} AVPackNode;


#pragma pack(2)
typedef struct 
{
	unsigned short DataType;          //��������
	unsigned short FrameNo;           //֡���
	unsigned int   Timestamp;         //ʱ���
	unsigned int   Framelen;          //֡���ݳ���
	unsigned short TotalPackage;      //�ܰ���
	unsigned short CurrPackage;       //��ǰ����
	unsigned short Datalen;           //���ݳ���
	unsigned short PackLen;           //���ݰ���С
}AVPackHead;

#define FRAGMENT_SIZE		1200

typedef struct
 { 
	//ÿ֡���з�Ƭ������Ϣ 
	char 			flag[PACK_MARK_LEN]; 	//���������ַ������ֻ�����������������־���Ƿ��͵����� 
	AVPackHead		m_head;					// ���ݰ�ͷ
	unsigned char	m_data[FRAGMENT_SIZE];	// ������
}udp_fragment_t;

#pragma pack()


int init_send_fragment_head( udp_fragment_t* p_udp_fragment);

// ����: ׼��һ�����ݰ�ǰ��Ҫ��ʼ��һ���ְ���ͷ
// ����: p_udp_fragment - �ַ����ݰ��ṹ��udp_total_size - udp���ְ������ݳ���
// ����: 0/ok�� 1/err
int start_send_fragment_head( udp_fragment_t* p_udp_fragment, int udp_total_size);

// ����: ��һ�����ְ������ݻ�������ȡ������װΪ�ַ���
// ����: p_udp_fragment - �ַ����ݰ��ṹ��p_udp_buf - udp���ְ���������
// ����: 0/ok�� 1/err
int pull_send_fragment_data( udp_fragment_t* p_udp_fragment, unsigned char* p_udp_buf);

// ����: ׼����һ���ְ�����
// ����: p_udp_fragment - �ַ����ݰ��ṹ
// ����: 0/ok�� 1/err
int prepare_for_next_fragment( udp_fragment_t* p_udp_fragment);


void CVideoPackProcessor_InitNode();
void CVideoPackProcessor_ClearNode();
void CVideoPackProcessor_LockNode();
void CVideoPackProcessor_UnLockNode();
void ReleaseSemaphore(void);
void TriggerSemaphore(void);
int CVideoPackProcessor_ProcPack(unsigned char *buf,int len);
AVPackNode * CVideoPackProcessor_PickPack();
void CVideoPackProcessor_ReturnNode(AVPackNode * pNode);


#endif

