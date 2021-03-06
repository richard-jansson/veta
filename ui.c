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
/* Level 2 - User Interface ( UI ) 
 * 
 * This UI is based on functions in the platform specific code. In our current state
 * that would be x11. It would be trivial, really to implement these functions to 
 * work with other libraries / platforms. 
 * 
 * So the lvl. 2 User Interface is platform independent.
 * 
 * User interface to configure keybindings, try to keep as simple as possible!
 * 
 */
#include<stdlib.h>

#include "veta.h"
#include "debug.h"
#include "keyboard_io.h"
#include "ui.h"

void widget_set_visible(widget w,int v);

widget_t **widgets;
int n_widgets=0;

void standard_onrelease(widget_t *this,char *s,int *propagate,vkey key,void *pspecific){
	*propagate=1;
	uk_log("current widget has no associated onrelease function");
}
void standard_click(widget_t *this){
	uk_log("[%s] clicked",this->label);
}

void standard_draw(widget_t *this,int x,int y,int w,int h){
	draw_text_box(this->label,w,h,x,y,this->fg,this->bg);	
}

// Text draw don't give two cent about your coordinates
// He'll live on the center of attention or he'll die 
// trying.
void text_draw(widget_t *this,int x0,int y0,int w0,int h0){
	int x,y,w,h;
	w=WIDTH/2;
	h=HEIGHT/10;
	x=(WIDTH-w)/2;
	y=(HEIGHT-h)/2;
	draw_text_box(this->label,w,h,x,y,this->fg,this->bg);	
}

// By default the widgets are invisible
widget add_widget(char *label,
	void(*draw)(widget_t *this,int x,int y,int w,int h),
	void(*onclick)(widget_t *w),
	void(*onrelease)(widget_t *w,char *s,int *p,vkey key,void *pspecific))
{
	int n=n_widgets;
	rgb white=(rgb){255,255,255};

	widgets=realloc(widgets,sizeof(widget_t*)*(n+1));
	
	widgets[n]=malloc(sizeof(widget_t));
	widgets[n]->bg=get_color(n,0);
	widgets[n]->fg=white;

	widgets[n]->label=label;
	widgets[n]->onrelease=onrelease?onrelease:standard_onrelease;
	widgets[n]->draw=draw?draw:standard_draw;
	widgets[n]->onclick=onclick?onclick:standard_click;

	widgets[n]->visible=0;
	
	n_widgets++;
	if(label) uk_log("added %s = %i",label,(int)n); 
	return (widget)n;
}

/* Private */
//typedef enum {HUD_OFFLINE,CONF,CONF_DESC,CONF_BINDING} ui_state_t;
//ui_state_t ui_state;

int keyboard_grabbed=0;
/*
typedef struct box_t {
	int w,h;
	int x,y;
	int id;
	ui_state_t current_state;
	ui_state_t next_state;
} box_t;

box_t **boxes;
int n_boxes=0;
*/

extern int lastMX,lastMY;

int test_intersect(int i,int x,int y){
	widget_t *b=widgets[i];
	uk_log("{%i,%i,%i,%i} vs (%i,%i)\n",b->x,b->y,b->x+b->w,b->y+b->h,x,y);
	if( x < b->x ) return 1;
	if( y < b->y ) return 2;
	if( x > (b->x+b->w)) return 3;
	if( y > (b->y+b->h)) return 4;
	return 0;
}


void ui2_handle_release(char *s,int *propagate,vkey key,void *pspecific){
	void (*f)(widget_t *this,char *,int *,vkey key,void *data);
	int p;
	uk_log("UI2 got release %s",s);
	for(int i=0;i<n_widgets;i++) {
		if(widgets[i]->visible ){
			f=widgets[i]->onrelease;
			f(widgets[i],s,&p,key,pspecific);
			if(!p){
				uk_log("do not propagate!!");
				*propagate=0;
				return;
			}
		}
	}
	*propagate=1;
}

void draw_normal(){
	uk_log("draw_normal");
	rgb c=get_color(4,0);
	rgb white=(rgb){255,255,255};
	int w=WIDTH;
	int h=HEIGHT;

//	add_box(w/2,h/2,w/2,h/2,HUD_OFFLINE,CONF);
	int w0=w/8;
	int h0=h/16;
	int x0=w-w/8-w/16;
	int y0=h-h/16-h/32;
	draw_text_box("Configure",w0,h0,x0,y0,white,c);
}

void draw_conf(){
	rgb c=get_color(0,0);
	rgb c1=get_color(1,0);
	rgb c2=get_color(2,0);
	rgb white=(rgb){255,255,255};
	int qw=WIDTH/4,qh=HEIGHT/4;
	int w=qw*2;
	int h=qh*2;

	draw_box(w,h,qw,qh, c.r,c.g,c.b);	

//	draw_text_box("Description",w,h/2,qw,qh, white,c1);
	draw_text_box("Binding",w,h/2,qw,qh+h/2, white,c2);
}

void draw_conf_desc(){
	if( keyboard_grabbed == 0) {
//		grab_keyboard();
	}
	rgb c1=get_color(0,0);
	rgb c2=get_color(1,0);
	int w=WIDTH;
	int h=HEIGHT;
	int qw=WIDTH/4,qh=HEIGHT/4;
	rgb white=(rgb){255,255,255};
	w=qw*2;
	h=qh*2;
	draw_text_box("OK",qw/2,h/2,qw+w,qh,white,c2);
}
void draw_conf_binding(){
}

void *draw_functions[]={draw_normal,draw_conf,draw_conf_desc,draw_conf_binding};

void ui2_handle_click(int mx,int my){
	void (*f)(widget_t *this);
	int w=WIDTH/8;
	int h=HEIGHT/16; 
	int x=WIDTH-w; 
	int y0=HEIGHT-h;
	int y;


	refresh();

	uk_log("handle click!");

	for(int i=0;i<n_widgets;i++) {
		if(widgets[i]->visible){
			y=y0;
			y0-=h;
//  FIXME: code duplication create an intersect helper function!
			if( mx < x ) continue;
			if( my < y ) continue;
			if( mx > x + w ) continue;
			if( my > y + h ) continue;

			uk_log("ui2 handle click matches %s",widgets[i]->label);

			f=widgets[i]->onclick;
			f(widgets[i]);

			refresh();
		}
	}
}

/* 
 * FIXME: code duplication between render_ui2 and ui2_handle_click 
 */ 
void render_ui2(){
	void (*f)(widget_t *this,int x,int y,int w,int h);

	int w=WIDTH/8;
	int h=HEIGHT/16;
	int x0=WIDTH-w;
	int y0=HEIGHT-h;
	for(int i=0;i<n_widgets;i++) {
		if(widgets[i]->visible ){
			f=widgets[i]->draw;
			f(widgets[i],x0,y0,w,h);
			y0-=h;
		}
	}
}

void widget_set_visible(widget w,int v){
	widgets[(int)w]->visible=v?1:0;
	refresh();
}

void widget_set_label(widget w,char *label){
	widgets[w]->label=label;
}

void ui2_init(){
	widgets=(widget_t**)malloc(sizeof(widget_t*)*(n_widgets+1));
}

