
#include <unistd.h>  
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h> 

#include "udp_fragment_opt.h"
#include "../../../utility.h"

// ������ϵͳ���������ĺ�������tick��
unsigned long GetTickCount(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

///////////////////////////////////////////////////////////////////////////////////////////
// �ְ�����
///////////////////////////////////////////////////////////////////////////////////////////

// ����: ׼������һ�����ݰ�ǰ��Ҫ��ʼ��һ����ͷ
// ����: p_udp_fragment - �ַ����ݰ��ṹ��udp_total_size - udp���ְ������ݳ���
// ����: 0/ok�� 1/err
static int				m_usFragOffset; 
static int				m_bLastFragment;
static unsigned int 	m_send_tick;

int init_send_fragment_head( udp_fragment_t* p_udp_fragment)
{
	// ��ʼ����־
	memcpy( p_udp_fragment->flag, PACK_MARK, PACK_MARK_LEN );
	// ��ʼ��tick
	m_send_tick	= GetTickCount();
}

int start_send_fragment_head( udp_fragment_t* p_udp_fragment, int udp_total_size)
{
	static unsigned int send_sn 	= 1;	
	// ����֡����
	p_udp_fragment->m_head.DataType			= VIDEO_DATA_TYPE;
	// ֡�ı��
	p_udp_fragment->m_head.FrameNo			= send_sn++;
	// ʱ��� - δ��ǰϵͳtick - ����tick = ����video�ĳ���
	p_udp_fragment->m_head.Timestamp		= abs(GetTickCount()-m_send_tick);
	// ����֡���ܳ���
	p_udp_fragment->m_head.Framelen			= udp_total_size;
	// ����֡�ܵİ���
	p_udp_fragment->m_head.TotalPackage		= (udp_total_size/FRAGMENT_SIZE);
	p_udp_fragment->m_head.TotalPackage		+=((udp_total_size%FRAGMENT_SIZE)?1:0);
	// ����֡��ǰ����
	p_udp_fragment->m_head.CurrPackage		= 1;	// ��1��ʼ
	// ���ݰ�����󳤶�
	p_udp_fragment->m_head.PackLen			= FRAGMENT_SIZE;
	
	// ���ݰ��ĵ�ǰ����
	if( udp_total_size > p_udp_fragment->m_head.PackLen )
	{
		p_udp_fragment->m_head.Datalen		= p_udp_fragment->m_head.PackLen;
		m_bLastFragment						= 0;	// �����һ����
	}
	else
	{
		p_udp_fragment->m_head.Datalen		= udp_total_size;
		m_bLastFragment						= 1;	// �����һ����
	}
	// ÿ���ְ���������������ݵĵ�ƫ����
	m_usFragOffset 							= 0;		
	return 0;
}

// ����: �����Ĵ�һ����������ȡ������װ�ַ���
// ����: p_udp_fragment - �ַ����ݰ��ṹ��p_udp_buf - udp���ְ���������
// ����: 0/ok(�õ���Ч�ְ�)�� 1/(�õ����һ����Ч�ְ�)
int pull_send_fragment_data( udp_fragment_t* p_udp_fragment, unsigned char* p_udp_buf)
{
	memcpy( p_udp_fragment->m_data, p_udp_buf+m_usFragOffset, p_udp_fragment->m_head.Datalen );
	//p_udp_fragment->m_head.Timestamp = abs(GetTickCount()-m_send_tick); // ���Ͱ�ʹ��ͬһ��ʱ���
	return p_udp_fragment->m_head.Datalen;
}

// ����: ׼����һ������
// ����: p_udp_fragment - �ַ����ݰ��ṹ
// ����: 0/ok�� 1/err
int prepare_for_next_fragment( udp_fragment_t* p_udp_fragment)
{
	if( m_bLastFragment == 1 )
		return 1;
	
	// ָ����һ��������
	m_usFragOffset	+= p_udp_fragment->m_head.Datalen;
	
	// ��������
	p_udp_fragment->m_head.CurrPackage++;

	// ȷ���Ƿ�Ϊ���һ����
	if( p_udp_fragment->m_head.CurrPackage >= p_udp_fragment->m_head.TotalPackage )
	{
		p_udp_fragment->m_head.Datalen	 	= p_udp_fragment->m_head.Framelen - m_usFragOffset;
		m_bLastFragment						= 1;
	}
	else
	{
		p_udp_fragment->m_head.Datalen 		= p_udp_fragment->m_head.PackLen;
		m_bLastFragment						= 0;
	}
	return 0;	
}

/*
// ����: ���յ�һ�����ݰ����뵽udp������
// ����: p_udp_fragment - �������ݰ��ṹ��p_udp_buf - udp�����յ�������
// ����: 0/����ok�� 1/�õ�һ����Ч���ݰ���2/err
int push_recv_fragment_data( udp_fragment_t* p_udp_fragment, unsigned char* p_udp_buf )
{
	static unsigned int recv_sn = 0;
	static unsigned short recv_frag_cnt = 0;
	static unsigned short recv_total_cnt = 0;
	
	if( (p_udp_fragment->flag[0] != 0) || (p_udp_fragment->flag[1] != 0) )
	{
		return 2;
	}
	else
	{
		if( p_udp_fragment->m_nFragmentIndex == 0 )
		{
			recv_sn = p_udp_fragment->m_nSeqNumber;
			// test
			recv_frag_cnt = 0;
			recv_total_cnt = 0;
		}
		else if( recv_sn != p_udp_fragment->m_nSeqNumber )
		{
			return 2;
		}
		else
		{
			// test �ְ��������к�
			recv_frag_cnt++;
			if( recv_frag_cnt != p_udp_fragment->m_nFragmentIndex )
				return 2;
		}
		
		// �õ��ְ�����
		memcpy( p_udp_buf+p_udp_fragment->m_usFragOffset, p_udp_fragment->m_nFragmentDat, p_udp_fragment->m_usPayloadSize );

		recv_total_cnt += p_udp_fragment->m_usPayloadSize;
		
		// �ж��Ƿ����
		if( p_udp_fragment->m_bLastFragment == 1 )
		{
			p_udp_fragment->m_nTotalSize = recv_total_cnt;
			return 1;
		}
		else
			return 0;
	}
}
*/
///////////////////////////////////////////////////////////////////////////////////////////
// �������
///////////////////////////////////////////////////////////////////////////////////////////
AVPackNode 		Root;
AVPackNode 		*UnUsedRoot;
int 			FullNodeCount;
int 			NodeLength;

static pthread_mutex_t 	m_node_lock;
static sem_t			m_sem_node;

int				m_var_available = 0;

void CVideoPackProcessor_InitNode()
{
	int i;
	AVPackNode *p;
	Root.llink = NULL;
	Root.rlink = NULL;
	FullNodeCount = 0;
	NodeLength = 0;
	UnUsedRoot = NULL;

	for( i=0; i<VIDEO_BUF_COUNT; i++ )
	{
		p = (AVPackNode *)malloc(sizeof(AVPackNode));
		if (p == NULL)
		{
			printf("malloc fail 0");
		}
		//p->Content.Buffer = NULL;
		p->Content.Buffer = (unsigned char *)malloc(VIDEO_BUF_LEN);
		if (p->Content.Buffer == NULL)
		{
			printf("malloc fail 1");
		}
		p->rlink = UnUsedRoot;
		if (UnUsedRoot != NULL)
		{
		  UnUsedRoot->llink = p;
		}
		p->llink = NULL;
		UnUsedRoot = p;
	}
	//

	m_var_available = 0;
	
	pthread_mutex_init( &m_node_lock, 0);	
	sem_init(&m_sem_node,0, 0);

	
	printf("CVideoPackProcessor_InitNode ok\n");
	
	
}

void CVideoPackProcessor_ClearNode()
{
	CVideoPackProcessor_LockNode();

	AVPackNode *p,*q;
	p=Root.rlink;
	while(p != NULL)
	{
		q = p;
		p = p->rlink;
		if (q->Content.Buffer != NULL)
		{
		  free(q->Content.Buffer);
		}
		free(q);
	}

	p=UnUsedRoot;
	while(p != NULL)
	{
		q = p;
		p = p->rlink;
		if (q->Content.Buffer != NULL)
		{
			free(q->Content.Buffer);
		}
		free(q);
	}

	m_var_available = 1;
	
	CVideoPackProcessor_UnLockNode();

	NodeLength = 0;
	FullNodeCount = 0;
	UnUsedRoot = NULL;

	pthread_mutex_destroy( &m_node_lock );	
	sem_destroy(&m_sem_node);

	printf("CVideoPackProcessor_ClearNode ok\n");
	
}


void CVideoPackProcessor_LockNode()
{
	pthread_mutex_lock(&m_node_lock);
	
}

void CVideoPackProcessor_UnLockNode()
{
	pthread_mutex_unlock(&m_node_lock);
}

void ReleaseSemaphore(void)
{
	sem_wait_ex2(&m_sem_node, 0);	
}

void TriggerSemaphore(void)
{
	sem_post(&m_sem_node);
}

void CVideoPackProcessor_ReturnNode(AVPackNode * pNode)
{
	pNode->rlink = UnUsedRoot;
	if (UnUsedRoot != NULL)
	{
		UnUsedRoot->llink = pNode;
	}
	pNode->llink = NULL;
	UnUsedRoot = pNode;
	//free(pNode->Content.Buffer);
	//pNode->Content.Buffer = NULL;
}

bool CVideoPackProcessor_CheckBuf(unsigned char * buf,int len)
{
	if (len > PACK_MARK_LEN+(int)sizeof(AVPackHead))
	{
		if(strncmp((char *)buf,PACK_MARK,PACK_MARK_LEN) == 0)
			return true;
		else
			return false;
	}
	else
		return false;
}

bool CVideoPackProcessor_CheckHead(AVPackHead * pHead,int len)
{
	if(pHead->DataType != VIDEO_DATA_TYPE)
		return false;
	else if(pHead->Framelen > VIDEO_BUF_LEN)
		return false;
	else if(pHead->CurrPackage > pHead->TotalPackage)
		return false;
	else if(pHead->CurrPackage <= 0)
		return false;
	else if(pHead->TotalPackage <= 0)
		return false;
	else if(pHead->TotalPackage > VIDEO_MAX_PACK_NUM)
		return false;
	else
		return true;
}

// ����һ��֡
AVPackNode * CVideoPackProcessor_AddNode(AVPackHead * pHead,unsigned char * buf,int len)
{
	AVPackNode * RetNode;
	AVPackNode * LastNode;
	int i;

	LastNode = &Root;
	while (LastNode->rlink != NULL)
	{
		LastNode = LastNode->rlink;
	}

	RetNode = UnUsedRoot;
	if(RetNode == NULL)
	{
		return RetNode;
	}

	UnUsedRoot = RetNode->rlink;
	if (UnUsedRoot != NULL)
	{
		UnUsedRoot->llink = NULL;
	}

	RetNode->Content.AllReceived = false;
	for(i=0; i<VIDEO_MAX_PACK_NUM; i++)
	{
		RetNode->Content.PackageReceived[i] = false;
	}

	RetNode->Content.FrameType 		= pHead->DataType;
	RetNode->Content.FrameNo 		= pHead->FrameNo;
	RetNode->Content.TotalPackage 	= pHead->TotalPackage;
	RetNode->Content.Timestamp 		= pHead->Timestamp;
	RetNode->Content.PackageReceived[pHead->CurrPackage - 1] = true;
	RetNode->Content.Len 			= len - PACK_MARK_LEN-sizeof(AVPackHead);
	//RetNode->Content.Buffer = (unsigned char *)malloc(pHead->TotalPackage*RetNode->Content.Len);
	memcpy(RetNode->Content.Buffer,buf + PACK_MARK_LEN+sizeof(AVPackHead), RetNode->Content.Len);

	if (pHead->TotalPackage == 1)
	{
		RetNode->Content.AllReceived = true;
		FullNodeCount++;
	}
	else
	{
		RetNode->Content.AllReceived = false;
	}
	NodeLength++;
	RetNode->rlink=LastNode->rlink;
	RetNode->llink=LastNode;
	LastNode->rlink=RetNode;
	return RetNode;
}


// ���һ��֡
void CVideoPackProcessor_AppendToNode(AVPackNode * pNode,AVPackHead * pHead,unsigned char * buf,int len)
{
	int i;
	bool allrec;
	// �Ƿ��ظ���
	if (pNode->Content.PackageReceived[pHead->CurrPackage - 1]) {return;}
	// �Ǽǽ��ձ�־
	pNode->Content.PackageReceived[pHead->CurrPackage - 1] = true;
	// ��ӳ���
	pNode->Content.Len = pNode->Content.Len + len - PACK_MARK_LEN-sizeof(AVPackHead);
	// copy����
	memcpy(pNode->Content.Buffer + (pHead->CurrPackage - 1) * pHead->PackLen,
	     buf + PACK_MARK_LEN+sizeof(AVPackHead), len - PACK_MARK_LEN-sizeof(AVPackHead));
	// ȷ���Ƿ�����
	allrec = true;
	for(i=0; i<pNode->Content.TotalPackage; i++)
	{
		if(!pNode->Content.PackageReceived[i])
		{
			allrec = false;
			break;
		}
	}
	pNode->Content.AllReceived = allrec;
	if ((!allrec) && (pHead->CurrPackage == pHead->TotalPackage))
	{
	  //���Ӷ�ʧ������
	}
	if (allrec)
	{
		FullNodeCount++;		
		//printf("-----------------FullNodeCount=%d\n",FullNodeCount); 
		//PrintCurrentTime(0);
		
	}
}


AVPackNode * CVideoPackProcessor_FindNode(AVPackHead * pHead)
{
	AVPackNode * RetNode;
	RetNode=Root.rlink;
	while(RetNode!=NULL)
	{
		if(RetNode->Content.FrameNo == pHead->FrameNo)
		{
			break;
		}
		RetNode=RetNode->rlink;
	}
	return RetNode;
}

// ��ȡһ����Ч���ݰ�(һ֡����)
AVPackNode * CVideoPackProcessor_PickPack()
{
	AVPackNode * StartNode,*LostNode;
	int i;
	//if (NodeLength  <= VIDEO_BUF_FRAME)  
	{
		//return NULL;
	}
	StartNode = Root.rlink;
	i = 1;
	while(StartNode != NULL)                                   //������Ч��
	{                                                          
		//if ((NodeLength - i) <= VIDEO_BUF_FRAME)                 //��֤��������3����
		{
			//break;
		}
		if(StartNode ->Content.AllReceived)
		{
			break;
		}
		else
		{                                                       //���ղ������İ�
		   LostNode = StartNode;
		   StartNode = StartNode->rlink;
		   Root.rlink = StartNode;
		   StartNode->llink = &Root;
		   LostNode->rlink = UnUsedRoot;
		   if (UnUsedRoot != NULL)
		   {
				UnUsedRoot->llink = LostNode;
		   }
		   LostNode->llink = NULL;
		   UnUsedRoot = LostNode;
		   NodeLength--;
		}
		i++;
	}

	if ((StartNode == NULL) || (!StartNode->Content.AllReceived))    //���û�ҵ���Ч�������ؿ�
	{
		return NULL;
	}

	StartNode->llink = NULL;                                         //��ȡ��Ч��һ��
	Root.rlink = StartNode->rlink;
	if (StartNode->rlink != NULL)
	{
		StartNode->rlink->llink = &Root;
	}
	StartNode->rlink = NULL;
	FullNodeCount -= 1;                                              //���½ڵ���
	NodeLength -= 1;
	
	return StartNode;
}

// ����һ��udp���յ������ݰ�
static int test_cnt;
int CVideoPackProcessor_ProcPack(unsigned char *buf,int len)
{
	AVPackHead 	head;
	AVPackNode 	*tmpnode;

	CVideoPackProcessor_LockNode();

	if( !CVideoPackProcessor_CheckBuf(buf,len) ) 
	{
		CVideoPackProcessor_UnLockNode();	
		return -1;
	}
	
	memcpy(&head, buf + 6, sizeof(AVPackHead));
	
	if( !CVideoPackProcessor_CheckHead(&head,len) ) 
	{ 
		CVideoPackProcessor_UnLockNode();	
		return -1;
	}
	if( FullNodeCount > VIDEO_MAX_PACK_NUM )
	{ 
		CVideoPackProcessor_UnLockNode();	
		return -1;
	}	

	
	tmpnode = CVideoPackProcessor_FindNode(&head);
	if(tmpnode == NULL)
	{
		//printf("r:first_frame=%d\n",head.FrameNo);		
		test_cnt = 0;
		tmpnode = CVideoPackProcessor_AddNode(&head,buf,len);
	}
	else
	{
		test_cnt++;
		//printf("r:append=%d\n",test_cnt);
		CVideoPackProcessor_AppendToNode(tmpnode,&head,buf,len);
	}
	if(tmpnode != NULL && tmpnode->Content.AllReceived == 1)
	{

		//printf("r:debug2\n");
	
		//if( FullNodeCount >= VIDEO_BUF_FRAME )
		{
			TriggerSemaphore();
		}
	}

	CVideoPackProcessor_UnLockNode();
	
	return 0;
}

