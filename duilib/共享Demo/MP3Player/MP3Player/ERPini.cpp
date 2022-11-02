
#include "stdafx.h"
#include <io.h>
#include "ERPini.h"

ERPIni::ERPIni()
{
    pini=0;
}
 

ERPIni::~ERPIni()
{
    if(pini)
    {
      ini_close(pini); 
    }

    pini=0;
}

void ERPIni::Save()
{
    ini_save(pini);
}

void ERPIni::SaveTo(const char *sFileName)
{
    ini_saveex(pini,sFileName);
}

void ERPIni::WriteInteger(const char *sec,const char *key,int value)
{
   ini_setex_int(pini,sec,key,value);
}

void ERPIni::WriteString(const char *sec,const char *key,const char *value)
{
   ini_setex(pini,sec,key,value,true);
}

int ERPIni::ReadInteger(const char *sec,const char *key,int default_value)
{
   const char *value;
   value = ini_getex(pini,sec,key);
   if(value)
     return ini_getex_int(pini,sec,key);
   else
     return default_value;
}

const char *ERPIni::ReadString(const char *sec,const char *key,const char *default_value)
{
	const char *value;
	value = ini_getex(pini,sec,key);
    if(value)
      return value;
	else
      return default_value;  
}


void ERPIni::Open(const char *sFileName)
{
	if(0==pini)
	{
        ini_exists(sFileName);
	    pini = ini_open(sFileName,1);
	}
}

void ERPIni::list_append_tailer(INI *pini,ini_item *item)
{
	item->next = NULL;
	if (!pini->tailer) 
	{
		pini->header = pini->tailer = item;
	} 
	else 
	{
		pini->tailer->next = item;
		pini->tailer = item;
	} 
	return;
}


void ERPIni::list_append(INI *pini,
        int type,
		char *key,
		char *value,
		char *section
		)
{
	ini_item *item = (ini_item *)malloc(sizeof(ini_item));
	item->type    = type;
	item->section = section;
	item->key     = key;
	item->value   = value;
	list_append_tailer(pini,item);
	pini->length += 1;

	return;
}

//查找对应的段节点section存不存在，并定位到最后一个节点
ini_item *ERPIni::list_searchexbysection(INI *pini,const char *sect)
{
	ini_item *temp=0;
	ini_item *p = pini->header;  
	ini_item *_ret = 0;
	
	if (!p) 
		return NULL;  
 
	//找到第一个具有section名字的段节点
	while (p) 
	{ 
		if ( p->type!= ini_line_value && p->type!= ini_line_sect)
		{
            p = p->next;
			continue; 
		}

		if ( p->type== ini_line_sect && 0==strcmp(p->value,sect) )
		{			
			temp = p;
			p = p->next;
			continue; 
		} 
 
		if ( p->type== ini_line_value && 0==strcmp(p->section,sect) )
		{  
			_ret = p; 
			break; 
		}  		
		
		p = p->next;
	}

	if(!_ret && temp)  
	  _ret =temp; 

	return _ret; 
}

ini_item *ERPIni::list_searchex(INI *pini,const char *sect, const char *key)
{
	ini_item *p = pini->header;  
	ini_item *_ret = NULL;
	
	if (!p) 
		return NULL;  
 

	//下面先比较key再比较section，主要是考虑section相同机率比较大
	while (p) 
	{
		if(p->type!=ini_line_value)
		{
			p = p->next;
			continue;
		} 

		if (0 != strcmp(p->key,key)) 
		{
			p = p->next;
			continue;
		}

		if ( 0!=strcmp(p->section,sect)) 
		{
			p = p->next;
			continue;
		} 

		_ret = p;
		break;
	}

	return _ret; 
}

int ERPIni::list_searchsection(INI *pini,const char *section,int auto_create)
{
	char *value;
	ini_item *p = pini->header;   
   
	while (p) 
	{  
		if(p->type== ini_line_sect  &&  0==strcmp(p->value,section) )
			break;
        if(p->type== ini_line_value &&  0==strcmp(p->section,section) )
			break; 
		p= p->next;
	}  

	if(!p) //如果没找到
	{
        if(auto_create)
		{                
			value = (char *)malloc(strlen(section)+1);
			strcpy(value,section); 
			list_append(pini,ini_line_sect,0,value,0); 
			return 1;
		}
		else
           return 0;
	}
	
	return 1;
}


ini_item *ERPIni::list_search(INI *pini,const char *key)
{
	ini_item *p = pini->header;
	char temp[VCHR_LEN];
	char *k = temp, *sect=temp;
	ini_item *_ret = NULL;
	
	if (!p)
		return NULL;

	strcpy(temp,key);
	for (; *k && SECT_DOT!=*k; k++);
	if (SECT_DOT==*k && k==sect)
		sect = ++k;
	else if (SECT_DOT == *k)
		*k++ = 0;
	else
		k = sect;
	
	if (k!=sect && (unsigned int)(k-sect)==strlen(key)) 
	{
		while (p) 
		{
			if (0 != strcmp(p->section,sect))
			{
				p = p->next;
				continue;
			}
			if (!p->next)
				return p;
			if (0 != strcmp(p->next->section,sect))
				return p;
			p = p->next;
		}
		return _ret;
	}
	
	//下面先比较key再比较section，主要是考虑section相同机率比较大
	while (p) 
	{ 
		if(p->type!=ini_line_value)
		{
			p = p->next;
			continue;
		} 

		if (0 != strcmp(p->key,k)) 
		{
			p = p->next;
			continue;
		}

		if (k!=sect && 0!=strcmp(p->section,sect)) 
		{
			p = p->next;
			continue;
		}
		
		if (k==sect && 0!=strcmp(p->section, pini->header->section)) 
		{
			p = p->next;
			continue;
		}
		_ret = p;
		break;
	}
	
	return _ret;
}


// 获取所在行字符串的类型,是SECTION还是KEY或者是注释或者是空白行
const int ERPIni::getlinetype(const char *pstr)
{	  
	int pos=0;
	int line_size= strlen(pstr);
	
	if(pstr==NULL)
	  return ini_line_other;

	if(line_size==0) 
		return ini_line_other; 

	//找到第一个不为空的字符
	for (1; ' '==*(pstr+pos); pos++); 
   
    if(pstr[pos]==INI_REFCHAR1 || pstr[pos]==INI_REFCHAR2)        return ini_line_comment;  //注释行
	if(pos<line_size-1 && pstr[pos]=='[' )                        return ini_line_sect; //section行
	if(pos==line_size-2 && pstr[pos]=='\r' || pstr[pos]=='\n')    return ini_line_blank;  //空白行
    
	for (1; pos<line_size; pos++)
	{
       if('='==*(pstr+pos))
		 return ini_line_value;//有效key行
	}

	return ini_line_other;   
}

void ERPIni::parseline(INI *pini,char *buffer,int type,char *sec)
{
    char *p;
	char *s;
	char sbuffer[LINE_LEN] ;
	char *section;
	char *key;
	char *value; 
	//跳过空格,回车符\r和换行符\n
	for (p=buffer; ' '==*p||'\t'==*p; p++);
        
	switch(type)
	{
		   case ini_line_sect:
			   {
					for (p++; ' '==*p||'\t'==*p; p++);
					
					for (s=p; ' '!=*p && '\t'!=*p && ']'!=*p;  p++);	
					
					*p = '\0';
					
					strcpy(sbuffer,s);
					key = 0;
					value = (char *)malloc(strlen(sbuffer)+1);
					strcpy(value,sbuffer);
					section = 0;
					list_append(pini,type,key,value,section);
					strcpy(sec,sbuffer);
			   }
		    	break;
		   case ini_line_value:
			   {
                    for (; ' '==*p||'\t'==*p; p++);
					
					for (s=p; ' '!=*p && '\t'!=*p && '='!=*p && ':'!=*p;  p++);
					
					*p = '\0';

					key = (char *)malloc(strlen(s)+1);
					strcpy(key,s);
					
					for (p++;   ' '==*p||'\t'==*p||'='==*p||':'==*p;   p++);

					for (s=p;   '#'!=*p && ';'!=*p && '\n'!=*p && '\r'!=*p;   p++);
					
					*p = '\0';
					for (p=s+strlen(s)-1; ' '==*p; *p=0,p--);

					value = (char *)malloc(strlen(s)+1);
					strcpy(value,s);
					
					section = (char *)malloc(strlen(sec)+1);
					strcpy(section,sec);
					
					list_append(pini,type,key,value,section);
			   }
			    break;
		   case ini_line_blank: 
		   case ini_line_comment:
			   {  
					for (s=p; '\t'!=*p && '\n'!=*p; p++);
					
					*p = '\0'; 
				    
					strcpy(sbuffer,s);
					key = 0;
					value = (char *)malloc(strlen(sbuffer)+1);
					strcpy(value,sbuffer); 
					
					section = (char *)malloc(strlen(sec)+1);
					strcpy(section,sec);
					
					list_append(pini,type,key,value,section); 
			   }
		    	break;
		   case ini_line_other: 
			    break;
	}	 
}


int ERPIni::ini_exists(const char *fname)
{ 
	if(_access(fname, 0 ) == -1)
	{   
	     FILE *fp =fopen(fname,"w");
		 if(fp)
         {
			 fclose(fp);
			 return 1;
		 }
		 return 0;
	}   
    return 1;
}

INI *ERPIni::ini_open(const char *fname ,int auto_create)
{
	INI *pini;
	FILE *fp;
	char section[SECT_LEN];
	char buffer[LINE_LEN];

	if(auto_create)
      if(!ini_exists(fname))
		  return 0;

	fp= fopen(fname,"r");
	
	if (!fp)
		return NULL;
	pini = (INI *)malloc(sizeof(INI));
	pini->fname = (char *)malloc(strlen(fname)+1);
	strcpy(pini->fname,fname);
	pini->length = 0;
	pini->header = pini->tailer = NULL;
	
	section[0]='\0';

	while (!feof(fp)) 
	{
		if (!fgets(buffer,LINE_LEN,fp))
			break;
 
		parseline(pini,buffer,getlinetype(buffer),section);
	}

	fclose(fp);
	
	return pini;
}

INI *ERPIni::ini_create(const char *fname)
{
	FILE *fp = fopen(fname,"w+");
	INI *pini;
	
	if (!fp) return NULL;
	
	pini = (INI *)malloc(sizeof(INI));
	pini->fname = (char *)malloc(strlen(fname));
	strcpy(pini->fname,fname);
	pini->length = 0;
	pini->header = pini->tailer = NULL;
	
	fclose(fp);
	
	return pini;
}

int ERPIni::ini_saveex(INI *pini,const char *filename)
{
    FILE *fp;
	ini_item *p;
	char *sect = NULL;
	char buffer[LINE_LEN];
	
	p = pini->header;
	if (!p) return 0;
	fp = fopen(filename,"w+");
	if (!fp) return 0;
	
	while (p) 
	{
		switch(p->type)
		{   
		   case   ini_line_sect:
			 {
               	    sect = p->value;
					buffer[0] = '[';
					strcpy(buffer+1,sect);
					strcat(buffer,"]\n");
					fputs(buffer,fp);
			 }
			 break;
		   case   ini_line_value:
			   { 
					strcpy(buffer,p->key);
					strcat(buffer,"\t= ");
					strcat(buffer,p->value);
					strcat(buffer,"\n");
					fputs(buffer,fp);
			  }
			  break;
		   case   ini_line_blank:
			   {  
				   strcpy(buffer,p->value); 
				   fputs(buffer,fp);	   
			   }
			   break;
	       case   ini_line_comment:
			   {   
				   strcpy(buffer,p->value); 
				   strcat(buffer,"\n");
				   fputs(buffer,fp);	   
			  }
			  break; 
		   case ini_line_other:
			   break;
		}		
		p = p->next;
	}
	
	fclose(fp);
	
	return 1;
}

int ERPIni::ini_save(INI *pini)
{
	return ini_saveex(pini,pini->fname);
}

void ERPIni::ini_close(INI *pini)
{
	ini_item *p = pini->header;
	ini_item *temp;

	if (!p)
	{
        free(pini->fname);
	    free(pini); 
		return;
	}

	while (p)
	{
		free(p->key);
		free(p->value);
		free(p->section);
		temp = p;
		p = p->next;
		free(temp);
	}

	free(pini->fname);
	free(pini); 
        
	return;
}

char *ERPIni::ini_get(INI *pini,const char *key,char *value)
{
	ini_item *item;
	
	item = list_search(pini,key);
	*value = 0;
	if (item) 
		strcpy(value,item->value);
	
	return value;
}

char *ERPIni::ini_getex(INI *pini,const char *section,const char *key)
{
	ini_item *item;
	
	item = list_searchex(pini,section,key); 
	if (item) 
		return item->value; 
	else
		return 0;
}


char *ERPIni::ini_getex(INI *pini,const char *section,const char *key,char *value)
{
	ini_item *item;
	
	item = list_searchex(pini,section,key);
	*value = 0;
	if (item) 
		strcpy(value,item->value);
	
	return value;
}


int ERPIni::ini_get_int(INI *pini,const char *key)
{
	char value[VINT_LEN];
	return atoi(ini_get(pini,key,value));
}

int ERPIni::ini_getex_int(INI *pini,const char *section,const char *key)
{
    char value[VINT_LEN];
	return atoi(ini_getex(pini,section,key,value));
}

int ERPIni::ini_setex(INI *pini,const char *section,const char *key,const char *value,int auto_create)
{
	ini_item *item;
	char *temp;
	 
	item = list_searchex(pini,section,key);
	if (!item ) 
	{
		if(auto_create)
		{
          ini_appendex(pini,section,key,value); 
		}
	}
	else
	{
		temp = item->value;
		item->value = (char *)malloc(strlen(value)+1);
		strcpy(item->value,value);
		free(temp);
	}
	
	return 1;
}

int ERPIni::ini_set(INI *pini,const char *key,const char *value)
{
	ini_item *item;
	char *temp;
	
	item = list_search(pini,key);
	if (!item) return 0;
	temp = item->value;
	item->value = (char *)malloc(strlen(value)+1);
	strcpy(item->value,value);
	free(temp);
	
	return 1;
}

int ERPIni::ini_set_int(INI *pini,const char *key,int value)
{
	char buffer[VINT_LEN];
	sprintf(buffer,"%d\0",value);
	return ini_set(pini,key,buffer);
}

int ERPIni::ini_setex_int(INI *pini,const char *section,const char *key,int value)
{
	char buffer[VINT_LEN];
	sprintf(buffer,"%d\0",value);
	return ini_setex(pini,section,key,buffer,1);
}


int ERPIni::ini_appendex(INI *pini,const char *section, const char *key,const char *value)
{
  ini_item *item, *sear, *temp;
	char *k, *v, *s;
	 
	list_searchsection(pini,section,1);

	sear = list_searchex(pini,section,key);
	if (sear && 0==strcmp(sear->key,key))
		return 0;

	//分配一个节点
	item = (ini_item *)malloc(sizeof(ini_item));
	item->type  = ini_line_value;
	k = (char *)malloc(strlen(key)+1);
	strcpy(k ,key);
	item->key   = k;
	v = (char *)malloc(strlen(value)+1);
	strcpy(v,value);
	item->value = v;
	  
	if(sear) 
	{
		//更新节点信息
		s = (char *)malloc(strlen(sear->section)+1);
		strcpy(s,sear->section);
		temp = sear->next;
		sear->next = item;
		item->next = temp;
	} 
	else 
	{  
		sear =list_searchexbysection(pini,section);

		if(sear)
		{ 
			if(sear->type==ini_line_sect)
			{
               s = (char *)malloc(strlen(sear->value)+1);
			   strcpy(s,sear->value); 
			   list_append_tailer(pini,item);
			}
			else
			{
               s = (char *)malloc(strlen(sear->section)+1);
			   strcpy(s,sear->section);   
			   temp = sear->next;
			   sear->next = item;
			   item->next = temp;
			}			
		}
		else
		{
			 
			//找不到则插入到尾部 
			s = (char *)malloc(strlen(section)+1);
			strcpy(s,section);
			list_append_tailer(pini,item);
		}
	}

	item->section = s;
	pini->length += 1;
	
	return 1;
}

int ERPIni::ini_append(INI *pini,const char *key,const char *value)
{
	ini_item *item, *sear, *temp;
	char buffer[LINE_LEN];
	char *k, *v, *s, *p;
	


	strcpy(buffer,key);
	if (NULL == (p=strchr(buffer,SECT_DOT))) 
	{
		if (pini->header)
			strcpy(buffer,pini->header->section);
		else	
			strcpy(buffer,DEF_SECT_ROOT);
		strcat(buffer,".");
		strcat(buffer,key);
	} 
	else if (p == buffer) 
	{
		strcpy(buffer,pini->header->section);
		strcat(buffer,key);
	} 
	else if ((unsigned int)(p+1-buffer) == strlen(buffer))
		return 0;




	p = strchr(buffer,SECT_DOT);
	sear = list_search(pini,buffer);
	if (sear && 0==strcmp(sear->key,p+1))
		return 0;
	
	//分配一个节点值
	item = (ini_item *)malloc(sizeof(ini_item));
	item->type  = ini_line_value;
	k = (char *)malloc(strlen(key)+1);
	strcpy(k,p+1);
	item->key = k;
	v = (char *)malloc(strlen(value)+1);
	strcpy(v,value);
	item->value = v;
	

	*(p+1) = 0;
	if (sear) 
	{
		//更新节点信息
		s = (char *)malloc(strlen(sear->section)+1);
		strcpy(s,sear->section);
		temp = sear->next;
		sear->next = item;
		item->next = temp;
	} 
	else 
	{
		//查找对应的段节点section存不存在,插入一个Section节点，并插入当前节点到尾部
		*p = 0;  
		list_searchsection(pini,buffer,1);
		
		sear =list_searchexbysection(pini,buffer);

		if(sear)
		{
			if(sear->type==ini_line_sect)
			{
               s = (char *)malloc(strlen(sear->value)+1);
			   strcpy(s,sear->value);  
			}
			else
			{
               s = (char *)malloc(strlen(sear->section )+1);
			   strcpy(s,sear->section);   
			}

			temp = sear->next;
			sear->next = item;
			item->next = temp;
		}
		else
		{ 
			s = (char *)malloc(strlen(buffer)+1);
			strcpy(s,buffer);
			list_append_tailer(pini,item);
		}
	}

	item->section = s;
	pini->length += 1;
	
	return 1;
}

int ERPIni::ini_append_int(INI *pini,const char *key,int value)
{
	char buffer[VINT_LEN];
	sprintf(buffer,"%d\0",value);
	return ini_append(pini,key,buffer);
}

int ERPIni::ini_appendex_int(INI *pini,const char *section, const char *key,int value)
{
  char buffer[VINT_LEN];
	sprintf(buffer,"%d\0",value);
	return ini_appendex(pini,section,key,buffer);
}

int ERPIni::ini_removesection(INI *pini,const char *section)
{
  ini_item *item,*temp;
	ini_item *p = pini->header;  
	int head_destoryed=0;

	if (!p) 
		return 1;  
  
	temp = p;
	while (p) 
	{ 
		if ( 0!=strcmp(p->section,section)) 
		{  
			if(head_destoryed)
			{
				 pini->header = p;
                 head_destoryed=0;
			}
			temp = p;
			p = p->next;  
			continue;
		}
 
		if(!head_destoryed && p==pini->header)
		{
           pini->header=p->next; 
		   head_destoryed=1;
		}
		
		temp->next = p->next;
		item = p;
		p = p->next;  
		
		free(item->section);
		free(item->key);
		free(item->value);
		free(item);
		item=0;
		pini->length -= 1;
	}  
	
	return 1;
}

int ERPIni::ini_removeex(INI *pini,const char *section, const char *key)
{
    ini_item *item, *temp, *p;
	
	item = list_searchex(pini,section,key);
	if (!item) return 0;
	p = pini->header;
	if (p && !p->next) 
	{
		if (p != item) return 0;
		free(p->section);
		free(p->key);
		free(p->value);
		free(item);
		pini->header = pini->tailer = NULL;
		pini->length -= 1;
		return 1;
	}
	while (p && p->next) 
	{
		if (p->next == item)
			break;
		p = p->next;
	}
	if (!p || !p->next)
		return 0;

	temp = p->next;
	p->next = temp->next;
	free(temp->section);
	free(temp->key);
	free(temp->value);
	free(temp);
	pini->length -= 1;
	
	return 1;
}

int ERPIni::ini_remove(INI *pini,const char *key)
{
	ini_item *item, *temp, *p;
	
	item = list_search(pini,key);
	if (!item) return 0;
	p = pini->header;
	if (p && !p->next) 
	{
		if (p != item) return 0;
		free(p->section);
		free(p->key);
		free(p->value);
		free(item);
		pini->header = pini->tailer = NULL;
		pini->length -= 1;
		return 1;
	}
	while (p && p->next) 
	{
		if (p->next == item)
			break;
		p = p->next;
	}
	if (!p || !p->next)
		return 0;

	temp = p->next;
	p->next = temp->next;
	free(temp->section);
	free(temp->key);
	free(temp->value);
	free(temp);
	pini->length -= 1;
	
	return 1;
}
 