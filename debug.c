/*
 *	Copyright (C) 2016 Richard Jansson 
 *	
 *	This file is part of Veta.
 *	
 *	Veta is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	
 *	Veta is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with Veta.  If not, see <http://www.gnu.org/licenses/>.
*/
/* 
 * Basic logging functionalities 
 */
#include<stdio.h>
#include<stdarg.h>
#include<assert.h>
#include<time.h>
#include<sys/time.h>


#include"debug.h"

FILE *log_file=NULL;

void debug_init(char *path){
	log_file=fopen(path,"a");
	assert(log_file);
	fseek(log_file,0,SEEK_END);
}

long get_msec(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec*1000+tv.tv_usec/1000;
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
