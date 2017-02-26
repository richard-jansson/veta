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
