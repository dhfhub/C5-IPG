
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>

#include <time.h>   
#include <sys/types.h>   
#include <sys/socket.h>   
#include <sys/ioctl.h>   
                  
#include <netinet/in.h>    
#include <netinet/if_ether.h>   
#include <net/if.h>   
#include <net/if_arp.h>   
#include <arpa/inet.h>     

#include <errno.h>

#include <stdarg.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../task_survey/task_survey.h"
#include "logdoc.h"


static char logfile_name[MAX_PATH_LENGTH];
static int	logfile_line;

FILE* logfile = NULL;

/****************************************************************************************************************************************
 * @fn      OpenLogFile()
 *
 * @brief   ��logDoc�ļ���task_debug_sbu��򿪸��ļ���д���¼
 *
 * @param   none
 *
 * @return  0 - ok, other - err
 ***************************************************************************************************************************************/
int OpenLogFile(void)
{	
	if( ( logfile = fopen(logfile_name,"a+") ) == NULL )
	{
		eprintf( "open logfile error:%s\n",strerror(errno) );
		return -1;
	}
	return 0;
}

/****************************************************************************************************************************************
 * @fn      CloseLogFile()
 *
 * @brief   �ر�logDoc�ļ���task_debug_sbuд���¼�󣬻�رո��ļ�
 *
 * @param   none
 *
 * @return  0 - ok, other - err
 ***************************************************************************************************************************************/
int CloseLogFile(void)
{	
	fclose( logfile );
	return 0;
}

/****************************************************************************************************************************************
 * @fn      InitialLogFileLines()
 *
 * @brief   �õ�logDoc���ĵ���������logDoc�ĵ������ڣ����Զ����ɸ��ļ��������ڣ�������õ����ĵ���Ч�ļ�¼��������main��ʼ��ʱ����
 *
 * @param   none
 *
 * @return  ��Ч��¼����
 ***************************************************************************************************************************************/
int InitialLogFileLines( void )
{
	char buff[MAX_PATH_LENGTH] = {'\0'};
	FILE *file = NULL;
	
	snprintf(logfile_name, MAX_PATH_LENGTH, "%smnt/nand1-2/business.log",getenv("HOME"));

	logfile_line = 0;

	if( (file=fopen(logfile_name,"a+")) == NULL )
	{
		eprintf( "parse error:%s\n",strerror(errno) );
		return -1;
	}

	while( fgets(buff,MAX_PATH_LENGTH,file) != NULL )
	{
		logfile_line++;
	}
	
	fclose( file );

	printf("log file lines = %d\n", logfile_line);
	return 0;
}

/****************************************************************************************************************************************
 * @fn      JustifyLogFileLength()
 *
 * @brief   �����ļ�����
 *
 * @param   none
 *
 * @return  ��Ч��¼����
 ***************************************************************************************************************************************/
int JustifyLogFileLength( void )
{
	char buff[MAX_PATH_LENGTH] = {'\0'};
	char logfile_temp[MAX_PATH_LENGTH];	
	
	FILE *file_temp = NULL;
	FILE *file_log = NULL;
	
	int logfile_line_temp;

	if( logfile_line >= MAX_FILE_LINES )
	{
		//	open log file
		if( (file_log = fopen( logfile_name, "r" )) == NULL )
		{
			eprintf( "open logfile error:%s\n",strerror(errno) );
			return -1;
		}

		snprintf(logfile_temp, MAX_PATH_LENGTH, "%s.tmp",logfile_name);
		
		//	open temp file and empty file
		if( (file_temp = fopen( logfile_temp, "a+" )) == NULL )
		{
			eprintf( "open logfile temp error:%s\n",strerror(errno) );
			return -1;
		}

		//move top
		fseek( file_log, 0, SEEK_SET);
		fseek( file_temp, 0, SEEK_SET);

		logfile_line_temp = 0;
		//read line
		while( fgets(buff, MAX_PATH_LENGTH, file_log) != NULL )
		{
			logfile_line_temp++;
			if( logfile_line_temp > (MAX_FILE_LINES/2) )
			{
				// write temp file
				fprintf( file_temp, "%s", buff );
			}
		}
		// ���¿�ʼ����
		logfile_line = 0;
			
		fclose(file_log);
		fclose(file_temp);
		
		// ɾ���ɵ�log file
		if( unlink(logfile_name) < 0 )
		{
			eprintf("unlink error !\n");
		}
		// ������ʱ�ļ�Ϊlog file����
		if( rename( logfile_temp, logfile_name ) < 0 )
		{
			eprintf("rename error !\n");			
		}
	}
	return logfile_line;
}

/****************************************************************************************************************************************
 * @fn      write_rec_to_log_doc()
 *
 * @brief   д��һ����¼���ĵ�
 *
 * @param   prec_str - д���¼���ַ���ָ��
 *
 * @return  -1 - err����Ч��¼����
 ***************************************************************************************************************************************/
int write_rec_to_log_doc( char* prec_str )
{
	if( logfile != NULL )
	{
		fprintf( logfile, "%04d %s", logfile_line+1, prec_str );
		// line increase
		logfile_line++;
		return logfile_line;
	}
	else
		return -1;
}


/****************************************************************************************************************************************
 * @fn      read_rec_fm_log_doc()
 *
 * @brief   ��ȡlog���ļ�¼����
 *
 * @param   index 	- ��¼������
 * @param   pDat 	- ��¼����ָ��
 *
 * @return  0 - ok, other - err
 ***************************************************************************************************************************************/
int read_rec_fm_log_doc( int index, char* pDat )
{
	return 0;
}


