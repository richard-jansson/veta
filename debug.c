/* 
 * Basic logging functionalities 
 */
#include<stdio.h>
#include<stdarg.h>
#include<assert.h>
#include<time.h>

#include"debug.h"

FILE *log_file=NULL;

void debug_init(char *path){
	log_file=fopen(path,"a");
	assert(log_file);
	fseek(log_file,0,SEEK_END);
}

void _uk_log(const char *file,int line,const char *fmt,...){
#ifdef DEBUG	
	// TODO a bit of waste here perhaps
	char timebuf[128];
	time_t t;
	struct tm *tmp;

	assert(log_file);

	t=time(NULL);
	tmp=localtime(&t);

	strftime(timebuf,sizeof(timebuf),"%Y:%M:%d %H:%M:%S",tmp);

	va_list args;

	va_start(args,fmt);
	fprintf(log_file,"%s %s:%i ",timebuf,file,line);
	vfprintf(log_file,fmt,args);
	fprintf(log_file,"\n");
	fflush(log_file);
	va_end(args);
#endif 
}

void debug_exit(){
	fclose(log_file);
}
