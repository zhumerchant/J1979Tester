#ifndef ERP_INI_H
#define ERP_INI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  

#define LINE_LEN 2000
#define SECT_LEN 64	
#define VINT_LEN 64	
#define VCHR_LEN 64

#define DEF_SECT_ROOT "Root"//Ĭ�ϵĸ��ڵ����	  
#define SECT_DOT '@'     //������ؼ���֮��ķָ��


#define INI_REFCHAR1 ';' 
#define INI_REFCHAR2 '#'

//ÿ�е����� 0 =Section 1= ��Чkeyֵ�У� 2=�հ��� ,3= ע����;����#��ͷ 
enum  {
  ini_line_sect=0,
  ini_line_value=1,
  ini_line_blank=2,
  ini_line_comment,
  ini_line_other
};


typedef struct ini_item_ {
	int                type;       //����         
	char	           *section;   //����
	char	           *key;       //�ؼ���
	char	           *value;     //ֵ 
	struct ini_item_	*next;  //��һ���ڵ�

} ini_item;

typedef struct {
	char			*fname;      //�ļ���
	struct ini_item_	*header; //ͷ�ڵ�
	struct ini_item_	*tailer; //β�ڵ�
	int			 length;   //�ܳ���
} INI;
  
class ERPIni
{
private:
	INI *pini; 	 
	
	void list_append_tailer(INI *pini, ini_item *item);
	void list_append(INI *pini, int type, char *key,	char *value,char *section);
	ini_item *list_searchexbysection(INI *pini, const char *sect);
	ini_item *list_searchex(INI *pini, const char *sect, const char *key);
	int list_searchsection(INI *pini, const char *section,int auto_create);
	ini_item *list_search(INI *pini, const char *key);
	const int getlinetype( const char *pstr);
	void parseline(INI *pini,char *buffer,int type,char *sec);
public:
	ERPIni(); 
	~ERPIni();
	void Open(const char *sFileName);
	void Save();
	void SaveTo(const char *sFileName);
  void WriteInteger(const char *sec,const char *key,int value);
	void WriteString(const char *sec,const char *key,const char *value);
  int ReadInteger(const char *sec,const char *key,int default_value);
	const char *ReadString(const char *sec,const char *key,const char *default_value);    
private:
		/********************************************************
		 * ini_open	: ��ini�ļ�
		 * 		  ע�⣺ini�����Իس����н�����
		 * @fname	:ini�ļ���
		*/
		INI *ini_open(const char *fname,int auto_create);

		int ini_exists(const char *fname);

 
		/********************************************************
		 * ini_create	: ����һ��ini�ļ�
		 * 		  
		 * @fname	: ini�ļ���
		*/
		INI *ini_create(const char *fname);

		/********************************************************
		 * ini_save	: ����ini�ļ�
		 * 		  
		 * @pini	: iniָ��
		*/
		int ini_save(INI *pini);

		/*
		* ini_saveex: ����ini���ݵ��ļ���
		* @pini     : iniָ��
		* @filename : �ļ���
		*/
		int ini_saveex(INI *pini,const char *filename);

		/********************************************************
		 * ini_close	: �ر�ini�ļ� * 		
		 * @pini	    : iniָ��
		*/
		void ini_close(INI *pini);

		/********************************************************
		 * ini_get	: ȡini�ڵ�ֵ
		 * 		  
		 * @pini	: INIָ��
		 * @key		: key
		 * @value	: ֵ
		*/
		char *ini_get(INI *pini,const char *key,char *value);

		/********************************************************
		 * ini_getex	: ȡini�ڵ�ֵ
		 * 		  
		 * @pini	: INIָ��
		 * @section : ����
		 * @key		: key
		 * @value	: ֵ
		*/
		char *ini_getex(INI *pini,const char *section,const char *key,char *value);

		char *ini_getex(INI *pini,const char *section,const char *key);


		/********************************************************
		 * ini_get_int	: ȡ����ֵ
		 * 		   
		 * @pini	: INIָ��
		 * @key		: key
		*/
		int ini_get_int(INI *pini,const char *key);

		/********************************************************
		 * ini_get_int	: ȡ����ֵ
		 * 	
		 * @pini	: INIָ��
		 * @section : ����
		 * @key		: key
		*/
		int ini_getex_int(INI *pini,const char *section,const char *key);

		/********************************************************
		 * ini_set	: ����ini�ڵ�ֵ
		 * 		  
		 * @pini	: INIָ��
		 * @key		: key
		 * @value	: ֵ
		*/
		int ini_set(INI *pini,const char *key,const char *value);

		/********************************************************
		 * ini_setex   : �����ļ�ֵ
		 * @section    : ����		 
		 * @pini	   : INI�ļ����
		 * @key		   : �ؼ���
		 * @value	   : ֵ
		 * @auto_create: ������ʱ�Ƿ��Զ������ڵ�
		*/
		int ini_setex(INI *pini,const char *section,const char *key,const char *value,int auto_create);

		/********************************************************
		 * ini_set_int	: ����iniֵΪ����
		 * 		  
		 * @pini	: INIָ��
		 * @key		: key
		 * @value	: ����ֵ
		*/
		int ini_set_int(INI *pini,const char *key,int value);

		/********************************************************
		 * ini_setex_int	: ����iniֵΪ����
		 * 		  
		 * @pini	: INIָ��
		 * @section : ����
		 * @key		: key
		 * @value	: ����ֵ
		*/
		int ini_setex_int(INI *pini,const char *section, const char *key,int value);

		/********************************************************
		 * ini_append	: ����һ���ڵ�
		 *
		 * @pini	: INIָ��
		 * @key		: key
		 * @value	: ֵ
		*/
		int ini_append(INI *pini,const char *key,const char *value);

		int ini_appendex(INI *pini,const char *section, const char *key,const char *value);

		/********************************************************
		 * ini_append_int:����һ������
		 * 		   
		 * @pini	: INIָ��
		 * @key		: key
		 * @value	: ֵ
		*/
		int ini_append_int(INI *pini,const char *key,int value);

		/********************************************************
		 * ini_appendex_int:����һ������
		 * 		   
		 * @pini	: INIָ��
		 * @section : ����
		 * @key		: key
		 * @value	: ֵ
		*/
		int ini_appendex_int(INI *pini,const char *section, const char *key,int value);

		/********************************************************
		 * ini_remove	: ɾ���ڵ� 
		 * 		 
		 * @pini	: INIָ��
		 * @key		: key
		*/
		int ini_remove(INI *pini,const char *key);

		/********************************************************
		 * ini_removeex	: ɾ���ڵ㺯��
		 * 	
		 * @pini	: INIָ��
		 * @section :����
		 * @key     :�ؼ�����
		*/
		int ini_removeex(INI *pini,const char *section, const char *key);

		/********************************************************
		 * ini_remove	: ɾ�����������еĽڵ�
		 * 	 
		 * @pini	  : INIָ��
		 * @section   : ����
		*/
		int ini_removesection(INI *pini,const char *section);
};

#endif//ALAN_LIBINI_H
