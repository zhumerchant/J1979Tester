#ifndef ERP_INI_H
#define ERP_INI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  

#define LINE_LEN 2000
#define SECT_LEN 64	
#define VINT_LEN 64	
#define VCHR_LEN 64

#define DEF_SECT_ROOT "Root"//默认的根节点段名	  
#define SECT_DOT '@'     //段名与关键字之间的分割符


#define INI_REFCHAR1 ';' 
#define INI_REFCHAR2 '#'

//每行的类型 0 =Section 1= 有效key值行， 2=空白行 ,3= 注释行;或者#开头 
enum  {
  ini_line_sect=0,
  ini_line_value=1,
  ini_line_blank=2,
  ini_line_comment,
  ini_line_other
};


typedef struct ini_item_ {
	int                type;       //类型         
	char	           *section;   //段名
	char	           *key;       //关键字
	char	           *value;     //值 
	struct ini_item_	*next;  //下一个节点

} ini_item;

typedef struct {
	char			*fname;      //文件名
	struct ini_item_	*header; //头节点
	struct ini_item_	*tailer; //尾节点
	int			 length;   //总长度
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
		 * ini_open	: 打开ini文件
		 * 		  注意：ini必须以回车换行结束！
		 * @fname	:ini文件名
		*/
		INI *ini_open(const char *fname,int auto_create);

		int ini_exists(const char *fname);

 
		/********************************************************
		 * ini_create	: 创建一个ini文件
		 * 		  
		 * @fname	: ini文件名
		*/
		INI *ini_create(const char *fname);

		/********************************************************
		 * ini_save	: 保存ini文件
		 * 		  
		 * @pini	: ini指针
		*/
		int ini_save(INI *pini);

		/*
		* ini_saveex: 保存ini数据到文件中
		* @pini     : ini指针
		* @filename : 文件名
		*/
		int ini_saveex(INI *pini,const char *filename);

		/********************************************************
		 * ini_close	: 关闭ini文件 * 		
		 * @pini	    : ini指针
		*/
		void ini_close(INI *pini);

		/********************************************************
		 * ini_get	: 取ini节点值
		 * 		  
		 * @pini	: INI指针
		 * @key		: key
		 * @value	: 值
		*/
		char *ini_get(INI *pini,const char *key,char *value);

		/********************************************************
		 * ini_getex	: 取ini节点值
		 * 		  
		 * @pini	: INI指针
		 * @section : 段名
		 * @key		: key
		 * @value	: 值
		*/
		char *ini_getex(INI *pini,const char *section,const char *key,char *value);

		char *ini_getex(INI *pini,const char *section,const char *key);


		/********************************************************
		 * ini_get_int	: 取整型值
		 * 		   
		 * @pini	: INI指针
		 * @key		: key
		*/
		int ini_get_int(INI *pini,const char *key);

		/********************************************************
		 * ini_get_int	: 取整型值
		 * 	
		 * @pini	: INI指针
		 * @section : 段名
		 * @key		: key
		*/
		int ini_getex_int(INI *pini,const char *section,const char *key);

		/********************************************************
		 * ini_set	: 设置ini节点值
		 * 		  
		 * @pini	: INI指针
		 * @key		: key
		 * @value	: 值
		*/
		int ini_set(INI *pini,const char *key,const char *value);

		/********************************************************
		 * ini_setex   : 设置文件值
		 * @section    : 段名		 
		 * @pini	   : INI文件句柄
		 * @key		   : 关键字
		 * @value	   : 值
		 * @auto_create: 不存在时是否自动创建节点
		*/
		int ini_setex(INI *pini,const char *section,const char *key,const char *value,int auto_create);

		/********************************************************
		 * ini_set_int	: 设置ini值为整型
		 * 		  
		 * @pini	: INI指针
		 * @key		: key
		 * @value	: 整型值
		*/
		int ini_set_int(INI *pini,const char *key,int value);

		/********************************************************
		 * ini_setex_int	: 设置ini值为整型
		 * 		  
		 * @pini	: INI指针
		 * @section : 段名
		 * @key		: key
		 * @value	: 整型值
		*/
		int ini_setex_int(INI *pini,const char *section, const char *key,int value);

		/********************************************************
		 * ini_append	: 新增一个节点
		 *
		 * @pini	: INI指针
		 * @key		: key
		 * @value	: 值
		*/
		int ini_append(INI *pini,const char *key,const char *value);

		int ini_appendex(INI *pini,const char *section, const char *key,const char *value);

		/********************************************************
		 * ini_append_int:新增一个整型
		 * 		   
		 * @pini	: INI指针
		 * @key		: key
		 * @value	: 值
		*/
		int ini_append_int(INI *pini,const char *key,int value);

		/********************************************************
		 * ini_appendex_int:新增一个整型
		 * 		   
		 * @pini	: INI指针
		 * @section : 段名
		 * @key		: key
		 * @value	: 值
		*/
		int ini_appendex_int(INI *pini,const char *section, const char *key,int value);

		/********************************************************
		 * ini_remove	: 删除节点 
		 * 		 
		 * @pini	: INI指针
		 * @key		: key
		*/
		int ini_remove(INI *pini,const char *key);

		/********************************************************
		 * ini_removeex	: 删除节点函数
		 * 	
		 * @pini	: INI指针
		 * @section :段名
		 * @key     :关键字名
		*/
		int ini_removeex(INI *pini,const char *section, const char *key);

		/********************************************************
		 * ini_remove	: 删除所有区段中的节点
		 * 	 
		 * @pini	  : INI指针
		 * @section   : 段名
		*/
		int ini_removesection(INI *pini,const char *section);
};

#endif//ALAN_LIBINI_H
