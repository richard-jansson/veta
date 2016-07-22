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

#include"veta.h"
#include "debug.h"


typedef struct {
	int x,y,w,h;
	rgb bg,fg;
	int visible; // Visible means focus?? 
	void *onclick;
	void *onrelease;
	void *draw; // draw function 	
	char *label;
} widget_t;

typedef int widget;

void widget_set_visible(widget w,int v);


widget w_configure,w_select;

widget_t **widgets;
int n_widgets=0;

void standard_onrelease(widget_t *this){
}
void standard_click(widget_t *this){
	uk_log("[%s] clicked",this->label);
}

void standard_draw(widget_t *this,int x,int y,int w,int h){
	uk_log("s draw!");
	draw_text_box(this->label,w,h,x,y,this->fg,this->bg);	
}

widget add_widget(char *label,
				void(*draw)(widget_t *this,int x,int y,int w,int h),
				void(*onclick)(widget_t *w),
				void(*onrelease)(widget_t *w)
				)
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
	
	n_widgets++;
	return (widget)n;
}

/* Private */
//typedef enum {HUD_OFFLINE,CONF,CONF_DESC,CONF_BINDING} ui_state_t;
ui_state_t ui_state;
cell *curr;


char *conf_desc="Hello world";
int keyboard_grabbed=0;

typedef struct box_t {
	int w,h;
	int x,y;
	int id;
	ui_state_t current_state;
	ui_state_t next_state;
} box_t;

box_t **boxes;
int n_boxes=0;

extern int lastMX,lastMY;
/*
int add_box(int w,int h,int x,int y,ui_state_t s1,ui_state_t s2){
	int n=n_boxes;
	boxes=realloc(boxes,sizeof(box_t*)*(n+1));

	boxes[n]=malloc(sizeof(box_t));
	boxes[n]->w=w;
	boxes[n]->h=h;
	boxes[n]->x=y;
	boxes[n]->y=y;
	boxes[n]->current_state=s1;
	boxes[n]->next_state=s2;

	uk_log("add box {%i,%i, %i,%i}",w,h,x,y);

	n_boxes++;
}
*/

int test_intersect(int i,int x,int y){
	widget_t *b=widgets[i];
	uk_log("{%i,%i,%i,%i} vs (%i,%i)\n",b->x,b->y,b->x+b->w,b->y+b->h,x,y);
	if( x < b->x ) return 1;
	if( y < b->y ) return 2;
	if( x > (b->x+b->w)) return 3;
	if( y > (b->y+b->h)) return 4;
	return 0;
}


void ui2_handle_release(char *s,int *propagate){
	*propagate=ui_state!=CONF_DESC && ui_state!=CONF_BINDING; 
	uk_log("UI2 got release %s",s);
}


void draw_normal(){
	uk_log("draw_normal");
	rgb c=get_color(4,0);
	rgb white=(rgb){255,255,255};
	int w=WIDTH;
	int h=HEIGHT;

//	add_box(w/2,h/2,w/2,h/2,HUD_OFFLINE,CONF);
	draw_text_box("Configure",w/8,h/16,w-w/8-w/16,h-h/16-h/32,white,c);
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


	draw_text_box("Description",w,h/2,qw,qh, white,c1);
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
	draw_text_box(conf_desc,w,h/2,qw,qh,white,c1);
	draw_text_box("OK",qw/2,h/2,qw+w,qh,white,c2);
}
void draw_conf_binding(){
}

void *draw_functions[]={draw_normal,draw_conf,draw_conf_desc,draw_conf_binding};

void ui2_init_old(){
	ui_state=HUD_OFFLINE;

	widgets=(widget_t**)malloc(sizeof(widget_t*)*(n_widgets+1));

	int w=WIDTH;
	int h=HEIGHT;

	int qw=WIDTH/4,qh=HEIGHT/4;
	w=qw*2;
	h=qh*2;


}

void ui2_handle_click(int mx,int my){
	void (*f)(widget_t *this);
	int w=WIDTH/8;
	int h=HEIGHT/16; 
	int x=WIDTH-w; 
	int y=HEIGHT-h;

	uk_log("click");
	for(int i=0;i<n_widgets;i++) {
		if(widgets[i]->visible ){
			uk_log("%i is visible (%i,%i) (%i,%i,%i,%i)",i,mx,my,x,y,w,h);
			if( mx < x ) break;
			if( my < y ) break;
			if( mx > x + w ) break;
			if( my > y + h ) break;
			f=widgets[i]->onclick;
			f(widgets[i]);
		}
		y-=h;
	}
}
void render_ui2(){
	uk_log("render ui2");
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
}


void configure_click(widget_t *this){
	widget_set_visible(w_configure,0);
	widget_set_visible(w_select,1);

	render_ui2();
}

void ui2_init(){
	widgets=(widget_t**)malloc(sizeof(widget_t*)*(n_widgets+1));

	w_configure=add_widget("Configure",NULL,configure_click,NULL);
	w_select=add_widget("Select cell",NULL,NULL,NULL);

	widget_set_visible(w_configure,1);
}

