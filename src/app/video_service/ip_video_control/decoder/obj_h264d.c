/* 
    Assume the bitstream size is known in file "test.info", 720x480 resolution, 10 frames
    sample code for H264 for bitstream input and YUV output
    This sample code is to do decode 100 rounds,10 frames/round, named "/tmp/dev0.yuv"
    #./h264d test.264 test.info
 */

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/videodev.h>
#include <errno.h>
#include <linux/fb.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>
#include <signal.h>
#include <dirent.h>

#include "favc_avcodec.h"
#include "favc_version.h"
#include "obj_h264d.h"
#include "obj_vpost.h"
#include "obj_vpe.h"

//#include "../../os/RTOS.h"
//#include "../../os/OSQ.h"


#define DECODE_OUTPUT_PACKET_YUV422	0



#define DISPLAY_MODE_CBYCRY				4
#define DISPLAY_MODE_YCBYCR				5
#define DISPLAY_MODE_CRYCBY				6



#define IOCTL_LCD_ENABLE_INT				_IO('v', 28)
#define IOCTL_LCD_DISABLE_INT				_IO('v', 29)
#define IOCTL_WAIT_VSYNC		    			_IOW('v', 67, unsigned int)	

#define dout_name         					".//dev0.yuv"

#define	OUTPUT_FILE						0



/*------------------------------------------------------------------------
���:  
		�ڴ��С

����:
	   ��װ,�ڴ����	

����:
		ָ��
------------------------------------------------------------------------*/
void* H264DecMalloc0( size_t size )
{
	void* ptr = malloc( size );

	memset( ptr, 0, size );

	return ptr;
}

/*------------------------------------------------------------------------
								�ر�H264_Decoder	
���:  
		H264���нṹָ��

����:
	   �ͷ�ӳ��,�ر��ļ�,�ͷ��ڴ�

����:
		��
------------------------------------------------------------------------*/
int CloseH264Dec( DECODE_OBJ_t* f )
{   
	H264DecInfo *H264Dec = (H264DecInfo*)f->Data;
	
	if( H264Dec->OutPutVirBuf )
	{
		munmap( (char *)H264Dec->OutPutVirBuf, H264Dec->OutPutBuf.size );
		H264Dec->OutPutVirBuf = 0;
	}        

	if( H264Dec->FD  )
		close( H264Dec->FD  );
	H264Dec->FD	= -1;

	if( H264Dec )
		free( H264Dec );
	
	return 0;
}

/*------------------------------------------------------------------------
								��ʼ��H264_Decoder	
���:  
		H264���нṹָ��

����:
	   ���ļ�,����ӳ���ϵ,H264_Decoder��������,�ṩOutput�Ļ���ָ��(Y,U,V)	

����:
		��
------------------------------------------------------------------------*/
/*
�׸�I֡������I֡���𿪣����ԲŰѵ�һ���׸�I֡��IDR
����IDR֡��˵����IDR֮֡�������֡�����������κ�IDR֮֡ǰ��֡�����ݣ�����෴��������ͨ��I-֡��˵��λ����֮���B-��P-֡��������λ����ͨI-֮֡ǰ��I-֡���������ȡ����Ƶ���У���������Զ���Դ�һ��IDR֡���ţ���Ϊ����֮��û���κ�֡����֮ǰ��֡

// ��h264�������NALU���ɡ�
// ��һ֡ SPS��0 0 0 1 0x67�� PPS��0 0 0 1 0x68�� SEI��0 0 0 1 0x66�� IDR��0 0 0 1 0x65��
// p֡      P��0 0 0 1 0x61��
// I֡    SPS��0 0 0 1 0x67�� PPS��0 0 0 1 0x68�� IDR��0 0 0 1 0x65��
*/

int InitH264Dec( DECODE_OBJ_t* f, int Srcwidth, int Srcheight )
{
	H264DecInfo *H264Dec  = DecodeNew0( H264DecInfo, 1 );
	int32_t ret;
	
	H264Dec->FD = open( FAVC_DECODER_DEV, O_RDWR );

	if( H264Dec->FD <= 0 )
	{
		printf( "open H264 dec device fail \n" );
		goto fail;
	}

       ret = ioctl( H264Dec->FD, FAVC_IOCTL_DECODE_GET_BSSIZE, &H264Dec->Bitstreamsize ) ;
	if( ret < 0 )		
	{
		printf("Get vde bitstream size fail\n");
		goto fail;
	}	

	ret = ioctl( H264Dec->FD, FAVC_IOCTL_DECODE_GET_OUTPUT_INFO, &H264Dec->OutPutBuf );		// �õ�decoder��ݔ������ָᘺʹ�С��Ϣ
	if( ret < 0 || H264Dec->OutPutBuf.size == 0 )
	{
		printf("Get output buffer size fail\n");
        	goto fail;
	}		 
	
	printf( "output buf addr = 0x%x, size=0x%x\n", H264Dec->OutPutBuf.addr, H264Dec->OutPutBuf.size );
	
	H264Dec->OutPutVirBuf =  mmap( NULL, H264Dec->OutPutBuf.size, PROT_READ|PROT_WRITE, MAP_SHARED, H264Dec->FD, 0 );
	
	//printf( "mmap output_vir_buf = 0x%x\n", H264Dec.OutPutVirBuf  );
	
	if( H264Dec->OutPutVirBuf == MAP_FAILED )
	{
		printf("Map Output Buffer Failed!\n");
		goto fail;
	}
	
	memset( &H264Dec->Param, 0, sizeof(FAVC_DEC_PARAM) );

	H264Dec->Param.u32API_version 		= H264VER;
	H264Dec->Param.u32MaxWidth 			= Srcwidth;//H264Dec->Profile.width;//MAX_IMG_WIDTH;//video_setting->width;
	H264Dec->Param.u32MaxHeight 		= Srcheight;//H264Dec->Profile.height;//MAX_IMG_HEIGHT;//video_setting->height;

	// For file output or VPE convert planr to packet format
	H264Dec->Param.u32FrameBufferWidth 	= Srcwidth;//H264Dec->Profile.width;//MAX_IMG_WIDTH;//video_setting->width;
	H264Dec->Param.u32FrameBufferHeight 	= Srcheight;//H264Dec->Profile.height;//MAX_IMG_HEIGHT;//video_setting->height;
	H264Dec->Param.u32OutputFmt			= 0; // 1->Packet YUV422 format, 0-> planar YUV420 foramt


	// Output : Packet YUV422 or Planar YUV420
	ret = ioctl( H264Dec->FD, FAVC_IOCTL_DECODE_INIT, &H264Dec->Param );
	if( ret < 0 )
	{
		printf("FAVC_IOCTL_DECODE_INIT: memory allocation failed\n");
		return -1;
	}

	// ����decoder��ݔ������ָ�
	H264Dec->Profile.coded.data[0] = (unsigned char *)H264Dec->OutPutBuf.addr;
	H264Dec->Profile.coded.data[1] = (unsigned char *)(H264Dec->OutPutBuf.addr+ (Srcwidth*Srcheight));
	H264Dec->Profile.coded.data[2] = (unsigned char *)(H264Dec->OutPutBuf.addr+ (Srcwidth*Srcheight*5/4));

	f->Data = H264Dec;
	
	return 0;

	fail:
	{
		CloseH264Dec( f );
		
		return -1;
	}	
}

/*------------------------------------------------------------------------
								H264_Decoder����
���:  
		H264���нṹָ��

����:
	   Decoder�Ĳ����������ü�Decoder��������ִ��

����:
		����������(Y,U,V��ָ��)
------------------------------------------------------------------------*/
int DecodeH264( DECODE_OBJ_t* f , unsigned char *frame,  int size )
{
	FAVC_DEC_PARAM      tDecParam;
	H264DecInfo *H264Dec = (H264DecInfo*)f->Data;
	int  ret_value;
    
	memset( &tDecParam, 0, sizeof(FAVC_DEC_PARAM) );

	// 	�õ�decoder�Ľ�aݔ������ָ�
	tDecParam.pu8Display_addr[0] = (unsigned int)H264Dec->Profile.coded.data[0];
	tDecParam.pu8Display_addr[1] = (unsigned int)H264Dec->Profile.coded.data[1];
	tDecParam.pu8Display_addr[2] = (unsigned int)H264Dec->Profile.coded.data[2];
	
	tDecParam.u32Pkt_size =	(unsigned int)size;
	tDecParam.pu8Pkt_buf = frame;

	tDecParam.crop_x = 0;
	tDecParam.crop_y = 0;
    
	tDecParam.u32OutputFmt = 0; // 1->Packet YUV422 format, 0-> planar YUV420 foramt
	
	//printf("decode Y = 0x%x\n",tDecParam.pu8Display_addr[0]);

	if( (ret_value = ioctl( H264Dec->FD, FAVC_IOCTL_DECODE_FRAME, &tDecParam)) != 0 )    
	{
		printf("FAVC_IOCTL_DECODE_FRAME: Failed.ret=%x\n",ret_value);
		return -1;
	}

#if 0
	if (tDecParam.tResult.isDisplayOut != 0)
	{
		fwrite( (void *)pict->data[0], video_setting->width * video_setting->height, 1, dout );
		if( tDecParam.u32OutputFmt == 0 )
		{
			fwrite((void *)pict->data[1],video_setting->width*video_setting->height/4,1,dout);
			fwrite((void *)pict->data[2],video_setting->width*video_setting->height/4,1,dout);
		}
	}
#endif

//	decoded_img_width = tDecParam.tResult.u32Width;
//	decoded_img_height = tDecParam.tResult.u32Height;    

	//printf( "###### %d \n", (*(volatile unsigned char *) ((void*)tDecParam.pu8Display_addr[0])) );

	if (tDecParam.tResult.isDisplayOut ==0)
		return 0;
	else
		return (int)tDecParam.got_picture;
}


#if 0
pthread_t task_H264_dec;
pthread_t task_UDP_Receive;

OS_Q H264_decQ;
unsigned char data_dec[DEC_SIZE*20];

void API_H264_dec( unsigned char* buff, int size )
{
	unsigned char _buff[DEC_SIZE] = {0};
	
	memcpy( _buff, buff, size );
		
	if( OS_Q_Put( &H264_decQ, _buff, size ) )
		printf(" API_H264_dec if full \n");
}

void  vtk_TaskProcessEvent_H264_dec( void )
{
	int result = 0;
	int size = 0;
	unsigned char* msgH264_dec;
	unsigned char IDR =0;
	
	DECODE_OBJ_t VpeObj;
	DECODE_OBJ_t VpostObj;
	DECODE_OBJ_t H264Obj;
	
	OS_Q_Create( &H264_decQ, &data_dec, sizeof(data_dec) );
	
	if( InitVpost( &VpostObj ) )
		exit(-1);

	if ( InitVpe( &VpeObj  ) )	
		exit(-1);

	if( InitH264( &H264Obj ) < 0 )
		exit(-1);

	
	while( 1 )
	{	
		size = OS_Q_GetPtr( &H264_decQ,  (void*)&msgH264_dec );	//Get message

		if( IDR == 0 && msgH264_dec[0] == 0 && msgH264_dec[1] == 0 && msgH264_dec[2] == 0 && msgH264_dec[3] == 1 )
		{
			if( ( (msgH264_dec[4]) & ((1<<5)-1) ) == 7 && ( (msgH264_dec[16]) & ((1<<5)-1) ) == 8 && ( (msgH264_dec[24]) & ((1<<5)-1) ) == 5 )
				IDR = 1;
		}
		
		if( IDR )
		{
			result = DecodeH264( &H264Obj, (unsigned char*)msgH264_dec, size );
			
			if( result < 0 )
				printf( "frame decode FAIL!\n" );

			FormatConversion( &VpeObj, &VpostObj, &H264Obj, 320, 240, 320, 240 );
		}
		
		OS_Q_Purge( &H264_decQ );
	}
}


void vtk_TaskProcessEvent_UDP_Receive( void )
{

	int port = 25050;
	
	// set UDP server data structures ---------------------------
	struct sockaddr_in addr;
	int sd;
	int bytes;
	u_int32_t addr_len = sizeof( addr );
	u_int8_t udpbuffer[DEC_SIZE] = {0};
	
	sd = socket( PF_INET, SOCK_DGRAM, 0 );
	
	bzero( &addr, sizeof(addr) );
	addr.sin_family 		= AF_INET;
	addr.sin_addr.s_addr 	= INADDR_ANY;
	addr.sin_port 			= htons( port );
	
	if( bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
		perror("bind");
	
	while( 1 )
    	{
	        // UDP receive ---------------------------------------------
	        memset( udpbuffer, 0, sizeof(udpbuffer) );
			
	        bytes = recvfrom( sd, udpbuffer, sizeof(udpbuffer), 0, (struct sockaddr*)&addr, &addr_len );

		//printf("recvfrom bytes %d !\n" , bytes );

		API_H264_dec( udpbuffer, bytes );
    	}
}


void vtk_TaskInit_H264_dec( unsigned char priority )
{
	//create task
	if( pthread_create(&task_H264_dec,NULL,(void*)vtk_TaskProcessEvent_H264_dec, NULL) != 0 )
	{
		printf( "Create pthread error! \n" );
		exit(1);
	}

	if( pthread_create(&task_UDP_Receive,NULL,(void*)vtk_TaskProcessEvent_UDP_Receive, NULL) != 0 )
	{
		printf( "Create pthread error!n" );
		exit(1);
	}

	pthread_join( task_UDP_Receive, NULL );
}
#endif
