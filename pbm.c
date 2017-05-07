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


// For reference of the pbm or netpbm graphics format I would of course refer to wikipedia. 
//
// https://en.wikipedia.org/wiki/Netpbm_format#File_format_description
//
// A much more comprehensive support are of course the man pages.
//
// man netpbm 
//

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"color.h"

// The interface we are "implementing"
#include"img.h"

img_t* img_open(char *name,int w,int h){
	img_t *ret = malloc(sizeof(img_t));
	if (!ret) return _uk_ret("malloc failed.");

// Set to real values once set up 
	ret->w=ret->h=-1;

	char *path;
	path=malloc(strlen(name)+4);
	*path='\0';
	strcat(path,name);
	// PPM - gives us 24 bit binary image 
	strcat(path,".ppm");

	char cwd = getcwd();

	_uk_log("Creating screenshot in: %s", cwd);

	FILE *fd=fopen(name,"w");
	if(!fd) return _uk_ret("fopen failed");

	ret->buf=malloc(w*h*3);
	if(!ret->buf) return _uk_ret("malloc failed");

	// Ready 
	ret->w=w;
	ret->h=h;
}

// Values will be -1 upon error 
void img_getfmt(img_t *i,int *w,int *h){
	*w=i->w;
	*h=i->h;
}

// FIXME: optimize this code...
void img_setpixel(img_t *i,int x, int y,rgb c){
	char *buf = i->buf;
	int o = (i->w*y + x) * 3;

	*(buf + o + 0) = c.r;
	*(buf + o + 1) = c.g;
	*(buf + o + 2) = c.b;
}

// protected, for internal use only 
void _img_writeheader(img_t *i){
	fprintf(i->fd,"P6 %i %i %i\n",i->w,i->h,255);
}
// The actual image is written out at img_close
void img_close(img_t *i) {
	if (!i) return;
	if (i->w == -1) return;
	if (i->fd) fclose(i->fd);
	if (i->buf && i->fd) {
		_img_writeheader(i);
		fwrite(i->buf, i->w * 3, i->h, i->fd);
	}
	if (i->buf) free(i->buf);
	if (i->fd) fclose(i->fd);
	free(i);
}
