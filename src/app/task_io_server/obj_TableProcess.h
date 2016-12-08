/**
  ******************************************************************************
  * @file    obj_TableProcess.h
  * @author  czb
  * @version V00.01.00
  * @date    2016.5.31
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  * <h2><center>&copy; COPYRIGHT 2016 V-Tec</center></h2>
  ******************************************************************************
  */ 

#ifndef _obj_TableProcess_H
#define _obj_TableProcess_H

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <pthread.h>
#include <netinet/in.h>    
#include <netinet/if_ether.h>   
#include <net/if.h>   
#include <net/if_arp.h>   
#include <arpa/inet.h>     

#define BUFF_ONE_RECORD_LEN			200		// lzh_20160601
#define BUFF_ONE_KEY_NAME_LEN		200		// lzh_20160601

// lzh_20160601_e

// Define Object Property-------------------------------------------------------

// KEYֵ�û�������ṹ��pdatΪmalloc�õ�������ָ��
typedef struct
{
	int				len;			// ����������
	unsigned char*	pdat;			// ������ָ��
}one_vtk_dat;

// һ��������ݽṹ
typedef struct
{
	// key name zone
	int				keyname_cnt;	// ����key name����
	int*			pkeyname_len;	// ����key name��������
	one_vtk_dat* 	pkeyname;		// ����key name����ָ��
	
	// record zone
	int				record_cnt;		// ���ļ�¼����
	one_vtk_dat*	precord;		// ����¼������ȥָ��	
	// lzh_20160601_s
	pthread_mutex_t	lock;			// �����������ݱ�����
	// lzh_20160601_e	
} one_vtk_table;



// Define Object Function - Public----------------------------------------------

//����һ�����ļ�
one_vtk_table*  load_vtk_table_file( const char* ptable_file_name);	
//�ͷ�һ�����ļ�
int free_vtk_table_file_buffer( one_vtk_table* ptable);


//����һ�����м�ֵ��ƥ������м�¼
one_vtk_table* search_vtk_table_with_key_value( one_vtk_table* ptable, unsigned char* key_name, unsigned char* key_value, int whole_word_only);	
//�õ�һ������һ����¼��¼��Ϣ���Ӽ�ֵ��ƥ���offsetƫ������ʼ��ȡ
one_vtk_dat* get_one_vtk_record( one_vtk_table* ptable, unsigned char* key_name, unsigned char* key_value, int index);	
//�õ�һ����¼���ֶ��ַ�����Ϣ: char* pstring, int* plen(Ϊҵ��buf����󳤶ȣ�������ɺ���д) 
int get_one_record_string( one_vtk_dat* prec, int key_index, char* pstring, int* plen );
int set_one_record_string( one_vtk_dat* prec, int key_index, char* pstring );

//�õ�һ������ָ��һ����¼���ֶ��ַ�����Ϣ: char* pstring, int* plen(Ϊҵ��buf����󳤶ȣ�������ɺ���д) 
int get_one_table_record_string( one_vtk_table* ptable, int rec_idx, unsigned char* key_name, char* pstring, int* plen);

//ɾ�����ͷ�һ����
int release_one_vtk_tabl( one_vtk_table* ptable);	


// lzh_20160601_s
// ��ӡһ����
int printf_one_table( one_vtk_table* ptable );
// ����һ����ʱ��
one_vtk_table* create_one_vtk_table( int keyname_cnt, one_vtk_dat* pkeyname, int* pkeyvalue_len );
// ����һ����¼�������
int add_one_vtk_record( one_vtk_table* ptable, one_vtk_dat* precord );
//�õ�һ������һ����¼��¼��Ϣ
one_vtk_dat* get_one_vtk_record_without_keyvalue( one_vtk_table* ptable, int index );
// �õ�keyname�ڱ��е�ƫ������ֵ
int get_keyname_index( one_vtk_table* ptable, unsigned char *keyname );

// lzh_20160601_e

// Define Object Function - Private---------------------------------------------
// ����һ�����ļ�
int save_vtk_table_file_buffer( one_vtk_table* ptable, int start_index, int write_num, char* ptable_file_name );
int getNumberCount(int num);

#endif
