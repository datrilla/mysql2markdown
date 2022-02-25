 
/*
* fileencoding=UTF-8
* [C version]author:Datrilla
* email:598223433@qq.com
* archetype from:mysql_markdown[go version]
gcc -g mysql2markdown.c  -lrt -L/usr/lib64/mysql  -lmysqlclient  -o  mysql2markdown
*/
#include<stdio.h>
#include<string.h>
#include <mysql/mysql.h>
#include <stdint.h>
#include<time.h> 

typedef bool my_bool;
/*
gcc -g mysql2markdown.c  -lrt -L/usr/lib64/mysql  -lmysqlclient  -o  mysql2markdown
*/
#if 0
#define USEMACRO
#endif 

#ifdef  USEMACRO
#define Mdatabase  "dbname"
#define Muser      "user"
#define Mpsw       "psw"
#define Mport       3306
#define Mhost      "127.0.0.1"
#else
char* Mdatabase ;
char* Muser     ;
char* Mpsw      ;
int   Mport     ;
char* Mhost     ;
#endif 


#define EASY_BLOCKBIND_ERREXIT(ctx,sqlstr,stmt,bsets,bselect)  \
stmt= mysql_stmt_init(ctx); \
if((!stmt)) {  printf(" mysql_stmt_init() line:%d, out of memory\n",__LINE__); exit(1);} \
if (mysql_stmt_prepare(stmt, sqlstr, strlen(sqlstr))){ \
        printf("mysql_stmt_prepare() error %s[%s]\n",mysql_stmt_error(stmt),sqlstr);\
        exit(1);}\
if(NULL!=bsets){if (mysql_stmt_bind_param(stmt,bsets)){     \
    printf("mysql_stmt_bind_param()line:%d failed %s",__LINE__, mysql_stmt_error(stmt));\
    exit(1);} } \
if(NULL!=bselect){ if (mysql_stmt_bind_result(stmt,bselect)) {  \
    printf(" mysql_stmt_bind_result()line:%d  failed %s",__LINE__,  mysql_stmt_error(stmt));\
   exit(1); } }
 

int  GetFiledLength(MYSQL*mysql,const char*table,const char*filed)
{
        int len=0;
        int i; 
        if(filed==NULL)
          return len;
        if(mysql_query(mysql,table))
        {
               printf("%s not 1filed\n",table);
               return 0;
       }

       MYSQL_RES *tbl_cols=mysql_store_result(mysql);
       if(NULL==tbl_cols)
       {
               printf("%s not 2filed\n",table);
               return 0;
       } 
       
       unsigned int field_cnt = mysql_num_fields(tbl_cols); 
       for (i=0; i < field_cnt; ++i)
       {
           /* col describes i-th column of the table */
           MYSQL_FIELD *col = mysql_fetch_field_direct(tbl_cols, i);
           if((col->name!=NULL)&&(0==strcmp(col->name,filed)))
           {
              len=col->length;
              if(col->type==MYSQL_TYPE_VAR_STRING)
                printf("[SUCCESS]FOUND %s Column %d:%d [%s] STRING\n",filed, i,col->length, col->name); 
               else
                printf("[SUCCESS]FOUND %s Column %d:%d [%s] TYPE[%d]\n",filed, i,col->length, col->name,col->type); 
              
              break;
           }  
         /* printf("Column %d:len-%d[%s] TYPE[%d] STRING[%d] INT[%d] \n", 
          i,col->length, col->name,col->type,MYSQL_TYPE_VAR_STRING,MYSQL_TYPE_LONG); */
       }
       if(NULL!=tbl_cols)
       mysql_free_result(tbl_cols);
       return len;
}
#define VARCHAR_MALLOC(pmysql,tabname,filed,alen,xfiled)  \
alen=GetFiledLength(pmysql,tabname,filed);\
if(0==alen) alen=40;\
alen++; \
xfiled.var=calloc(sizeof(char),alen);\
if(NULL!=xfiled.var){xfiled.alen=alen;}


#define POINT_VARCHAR(bp,xfiled)\
bp.buffer_type = MYSQL_TYPE_VAR_STRING;\
bp.buffer = (char *)xfiled.var;\
bp.buffer_length=xfiled.alen-1;\
bp.length =&xfiled.len;/*mysql_stmt_execute()*/ \
bp.is_null=&xfiled.nil; \
bp.error  =&xfiled.err; \
bp.is_unsigned=0;



#define POINT_INTEGER(bp,xfiled)\
bp.buffer_type = MYSQL_TYPE_LONG;\
bp.buffer = (char *)&xfiled.i;\
bp.buffer_length=sizeof(int);\
bp.length =&xfiled.len;/*mysql_stmt_execute()*/ \
bp.is_null=&xfiled.nil; \
bp.error  =&xfiled.err; \
bp.is_unsigned=0;

void escapeChar( char *var,int alen)
{ 
        char*piter=var; 
        for(;(piter!=NULL)&&(piter-var<alen)&&((*piter)!='\0'); piter++)
        {
                if(*piter=='|')
                {
                        *piter='\040'; 
                }else if(*piter=='\n')
                {
                        *piter='\040';
                       
                }else if(piter-var+2<alen)
                {
                        if(((*piter)=='\\')&& 
                            ( *(piter+1)=='|'))
                        {
                                (*piter)='\040';
                                *(piter+1)='\040';
                                piter++;
                        }
                } 
        } 
}

int main(int argc,char**argv)
{
 #ifndef  USEMACRO
       { 
                int  bitcheck=0; 
                int  bitmask=0x0F; 
                int opt; 
                Mport=3306;
                #include<getopt.h>
                while ((opt = getopt(argc, argv, "vVh:u:p:P:D:")) != -1) 
                {
                        switch (opt) {  
                        case 'v':  
                                break; 
                        case 'V':   
                                break; 
                        case 'h':
                                bitcheck=(bitcheck|(1<<0));
                                Mhost=optarg;
                                break;
                        case 'u':
                                bitcheck=(bitcheck|(1<<1));
                                Muser=optarg;
                                break;
                        case 'p':
                                bitcheck=(bitcheck|(1<<2));
                                Mpsw=optarg;
                                break;
                        case 'P':
                                Mport=atoi(optarg);
                                break;
                        case 'D':
                                bitcheck=(bitcheck|(1<<3));
                                Mdatabase=optarg;
                                break;
                        default:
                                printf("optopt = %c\n", (char)optopt);
                                printf("opterr = %d\n", opterr);
                                 
                        }
                }
                printf("Usage:\n%s -hhost -uUser -pPsw -Pport -Ddatabase \n",argv[0]);
                printf("%s -h%s -u%s -p%s -P%u -D%s \n",argv[0], Mhost,
                 Muser,Mpsw,Mport,Mdatabase); 
                if(bitcheck==bitmask)
                {
                        printf("Usage:\n SUCCESS pass \n",argv[0]);
                }else
                {
                        printf("Usage:\n%s -hhost -uUser -pPsw -Pport -Ddatabase \n",argv[0]);
                        exit(0);
                }
               
    } 
#endif




 mysql_library_init(0, NULL, NULL);
 mysql_thread_init();

MYSQL *mysql=NULL;
MYSQL **ctx=&mysql;

if(NULL!=ctx)
{
        mysql_close(*ctx);  
        *ctx = NULL;  
}

*ctx = mysql_init(*ctx);  /*NULL-new;POINT-set*/
int sec =30;
 mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT,&sec);

if (!mysql_real_connect(mysql, Mhost, Muser, Mpsw, Mdatabase, Mport, NULL, 0)) 
  {   
                printf("Failed to connect to database:IP[%s]:%u User[%s] PSW[%s] DB[%s] code[%d] failed: %s.", 
                Mhost,Mport, Muser, Mpsw, Mdatabase, mysql_errno(mysql), mysql_error(mysql));
                 
            if(NULL!=ctx)
             {
                     mysql_close(*ctx);  
                *ctx = NULL;  
             }
             return -1;  
       }  
 mysql_query(mysql,"set names utf8mb4");
typedef struct
{
                union 
                {
                        char                 *var; 
                        int                  i;
                }               ;
                my_bool        err;
                unsigned long  len;
                my_bool        nil;
                unsigned long  alen;
}xxfiled; 
FILE*fp=NULL;
struct tbhead{
  xxfiled name;
  xxfiled comment;
  xxfiled table_schema;
};
struct tbhead tab; 
             
int alen;

VARCHAR_MALLOC(*ctx,"select  * from  information_schema.tables limit 1","TABLE_NAME",alen,tab.name) 

VARCHAR_MALLOC(*ctx,"select  * from  information_schema.tables limit 1","TABLE_COMMENT",alen,tab.comment) 

VARCHAR_MALLOC(*ctx,"select  * from  information_schema.tables limit 1","TABLE_SCHEMA",alen,tab.table_schema) 
struct colbody
{
     xxfiled   OrdinalPosition;
     xxfiled   ColumnName;
     xxfiled   ColumnComment;
     xxfiled   ColumnType;
     xxfiled   ColumnKey;
     xxfiled   IsNullable;
     xxfiled   Extra;
     xxfiled   ColumnDefault;
}; 
 
struct colbody info;
VARCHAR_MALLOC(*ctx,"select  * FROM information_schema.columns limit 1","COLUMN_NAME",alen,info.ColumnName) 
VARCHAR_MALLOC(*ctx,"select  * FROM information_schema.columns limit 1","COLUMN_TYPE",alen,info.ColumnType) 
VARCHAR_MALLOC(*ctx,"select  * FROM information_schema.columns limit 1","COLUMN_KEY",alen,info.ColumnKey) 
VARCHAR_MALLOC(*ctx,"select  * FROM information_schema.columns limit 1","IS_NULLABLE",alen,info.IsNullable) 
VARCHAR_MALLOC(*ctx,"select  * FROM information_schema.columns limit 1","EXTRA",alen,info.Extra) 
VARCHAR_MALLOC(*ctx,"select  * FROM information_schema.columns limit 1","COLUMN_COMMENT",alen,info.ColumnComment) 
VARCHAR_MALLOC(*ctx,"select  * FROM information_schema.columns limit 1","COLUMN_DEFAULT",alen,info.ColumnDefault) 

 struct timespec         tp; 
 struct tm               cur;
 clock_gettime(CLOCK_REALTIME,&tp);
 localtime_r(&tp.tv_sec, &cur); 


char *filename=NULL;
alen=strlen(Mdatabase)+strlen("YYYYMMDD+0800")+strlen(".md")+1;
filename=calloc(sizeof(char),alen);
snprintf(filename,alen,"%s%04.4d%02d%02d%c%02d00.md",Mdatabase,
cur.tm_year+1900,
cur.tm_mon+1,
cur.tm_mday,cur.tm_gmtoff>=0?'+':'-',
cur.tm_gmtoff/3600);


fp=fopen(filename,"w+");

fprintf(fp,"## %s tables message\n",
Mdatabase);
 
   const char SqlTables[] = "SELECT table_name,table_comment FROM information_schema.tables WHERE table_schema=? and TABLE_TYPE='BASE TABLE'";
   const char SqlTableColumn[] = "SELECT ORDINAL_POSITION,COLUMN_NAME,COLUMN_TYPE,COLUMN_KEY,IS_NULLABLE,EXTRA,COLUMN_COMMENT,COLUMN_DEFAULT FROM information_schema.columns WHERE table_schema=? AND table_name=? ORDER BY ORDINAL_POSITION ASC ";

  
        MYSQL_BIND          hget[2]; 
        MYSQL_BIND          hset[1];  
        MYSQL_BIND          tget[8]; 
        MYSQL_BIND          tset[2];  
        MYSQL_STMT *stmttab=NULL; 
        MYSQL_STMT *stmtcol=NULL; 



POINT_VARCHAR(hget[0],tab.name)
POINT_VARCHAR(hget[1],tab.comment)
POINT_VARCHAR(hset[0],tab.table_schema)


int i,t,ret;
i=0;
POINT_INTEGER(tget[i],info.OrdinalPosition);i++; 
POINT_VARCHAR(tget[i],info.ColumnName)   ;i++;  
POINT_VARCHAR(tget[i],info.ColumnType)   ;i++;  
POINT_VARCHAR(tget[i],info.ColumnKey)    ;i++; 
POINT_VARCHAR(tget[i],info.IsNullable)   ;i++;  
POINT_VARCHAR(tget[i],info.Extra)        ;i++;
POINT_VARCHAR(tget[i],info.ColumnComment);i++; 
POINT_VARCHAR(tget[i],info.ColumnDefault);i++; 

POINT_VARCHAR(tset[0],tab.table_schema);
POINT_VARCHAR(tset[1],tab.name);

snprintf(tab.table_schema.var,tab.table_schema.alen-1,"%s",Mdatabase);
tab.table_schema.len=strlen(tab.table_schema.var);
tab.table_schema.nil=0;

EASY_BLOCKBIND_ERREXIT(*ctx,SqlTables,stmttab,hset,hget) 
;
EASY_BLOCKBIND_ERREXIT(*ctx,SqlTableColumn,stmtcol,tset,tget) 


if (mysql_stmt_execute(stmttab)){ 
         printf("mysql_stmt_execute()line:%d failed %s",__LINE__,  mysql_stmt_error(stmttab)); 
         exit(1);}  
if(0!=mysql_stmt_store_result(stmttab)){ 
        printf("mysql_stmt_store_result()line:%d failed %s\n",__LINE__,  mysql_stmt_error(stmttab)); 
         exit(1);} 

#define MMCLEAR(name)  memset(name.var,0,name.alen);

for(i=0;;i++)
{
        MMCLEAR(tab.name); 
        MMCLEAR(tab.comment); 
         ret=mysql_stmt_fetch(stmttab); 
         if(1==ret)
         {
                printf("%d err \n ",__LINE__);  
                  break;
         }else if(MYSQL_NO_DATA==ret)
         {
                 printf("database[%s] line: %d No more rows\n",tab.table_schema.var,__LINE__);
                 break;  
         }else if(MYSQL_DATA_TRUNCATED==ret)
         {
                printf("%d MYSQL_DATA_TRUNCATED\n",__LINE__);  
         }  
fprintf(fp,"#### %d、%s \n%s\n",
i+1,tab.name.var,tab.comment.var);
fprintf(fp,"\n| 序号 | 名称 | 类型 | 键 | 为空 | 额外 | 默认值 | 描述 |\n" 
                        "| :--: | :--: | :--: | :--: | :--: | :--: | :--: | :--: |\n");



        if (mysql_stmt_execute(stmtcol)){ 
         printf("mysql_stmt_execute()line:%d failed %s",__LINE__,  mysql_stmt_error(stmtcol)); 
         exit(1);}  
        if(0!=mysql_stmt_store_result(stmtcol)){ 
        printf("mysql_stmt_store_result()line:%d failed %s\n",__LINE__,  mysql_stmt_error(stmtcol)); 
         exit(1);} 
        for(t=0;;t++){
MMCLEAR(info.ColumnName) ;
MMCLEAR(info.ColumnType) ;
MMCLEAR(info.ColumnKey) ;
MMCLEAR(info.IsNullable) ;
MMCLEAR(info.Extra) ;
MMCLEAR(info.ColumnComment) ;
MMCLEAR(info.ColumnDefault) ;
         ret=mysql_stmt_fetch(stmtcol); 
         if(1==ret)
         {
                printf("%d err  \n",__LINE__);  
                  break;
         }else if(MYSQL_NO_DATA==ret)
         {
                printf("[%s][%s] line: %d No more rows\n",tab.name.var,tab.name.var,__LINE__);
                 break;  
         }else if(MYSQL_DATA_TRUNCATED==ret)
         {
                printf("%d MYSQL_DATA_TRUNCATED\n",__LINE__);  
         }   
         escapeChar(info.ColumnComment.var,info.ColumnComment.len);
         
         fprintf(fp,    "| %d | `%s` | %s | %s | %s | %s | %s | %s |\n",
            info.OrdinalPosition.i,
                                info.ColumnName.var, 
                                info.ColumnType.var,
                                info.ColumnKey.var,
                                info.IsNullable.var,
                                info.Extra.var,
                                info.ColumnDefault.var,
                                
                                info.ColumnComment.var);

        }
        fprintf(fp,"\n\n");
        mysql_stmt_free_result(stmtcol);
}
mysql_stmt_free_result(stmttab);
        if(stmtcol)mysql_stmt_close(stmtcol); 
        stmtcol=NULL;
                if(stmttab)mysql_stmt_close(stmttab); 
        stmttab=NULL;

fclose(fp);
fp=NULL;

#define MMFREE(name)   if(NULL!=name.var){free(name.var);name.var=NULL;}




MMFREE(tab.name); 
MMFREE(tab.comment); 
MMFREE(tab.table_schema); 
MMFREE(info.ColumnName) ;
MMFREE(info.ColumnType) ;
MMFREE(info.ColumnKey) ;
MMFREE(info.IsNullable) ;
MMFREE(info.Extra) ;
MMFREE(info.ColumnComment) ;
MMFREE(info.ColumnDefault) ;

if(NULL!=ctx)
{
        mysql_close(*ctx);  
        *ctx = NULL;  
}
  mysql_thread_end();
mysql_library_end();
printf("SUCCESS finish ,please check file[%s]\n",filename);
free(filename);
filename=NULL;
return 0;

}
