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
typedef int widget;

typedef struct {
	int x,y,w,h;
	rgb bg,fg;
	int visible; // Visible means focus?? 
	void *onclick;
	void *onrelease;
	void *draw; // draw function 	
	char *label;
} widget_t;


void text_draw(widget_t *this,int x0,int y0,int w0,int h0);
void widget_set_label(widget w,char *label);
