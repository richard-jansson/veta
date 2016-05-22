#include <stdio.h>

#include "debug.h"
#include "conf.h"


state status;

void _initstate(){
	status.magic=MAGIC;
	status.x=0;
	status.y=0;
}

state *readstate(char *path){
	_initstate();

	FILE *fd=fopen(path,"r");
	if(!fd) return &status;

	fread((void*)&status,1,sizeof(status),fd);
	if(status.magic!=MAGIC){
		uk_log("Faulty magic in %s",path);
		_initstate();
		return &status;
	}
	fclose(fd);

	return &status;
}

void writestate(char *path,int x,int y){
	_initstate();
	status.x=x;
	status.y=y;

	FILE *fd=fopen(path,"w");
	if(!fd){
		uk_log("Couldn't open %s",path);
		return;
	}
	uk_log("writing x=%i y=%i",status.x,status.y);
	fwrite((void*)&status,1,sizeof(status),fd);

	fclose(fd);
}
