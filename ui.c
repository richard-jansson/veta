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
#include "cell.h"
#include "debug.h"


/* Private */
//typedef enum {HUD_OFFLINE,CONF,CONF_DESC,CONF_BINDING} ui_state_t;
ui_state_t ui_state;
cell *curr;

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

int intersect(i,x,y){
	box_t *b=boxes[i];
	uk_log("{%i,%i,%i,%i} vs (%i,%i)\n",b->x,b->y,b->x+b->w,b->y+b->h,x,y);
	if( x < boxes[i]->x ) return 1;
	if( y < boxes[i]->y ) return 2;
	if( x > (boxes[i]->x+boxes[i]->w)) return 3;
	if( y > (boxes[i]->y+boxes[i]->h)) return 4;
	return 0;
}

void ui2_handle_click(int x,int y){
	for(int i=0;i<n_boxes;i++){
		if(boxes[i]->current_state != ui_state) continue;
		int r=intersect(i,x,y);
		printf("%i",r);
		// A click event is blocked by the element
		if(!r){
			uk_log("box %i is hit! Going to state: %i",i,boxes[i]->next_state);
			ui_state = boxes[i]->next_state;
			render_ui2();
			return;
		}
	}
	
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
}
void draw_conf_binding(){
}

void *draw_functions[]={draw_normal,draw_conf,draw_conf_desc,draw_conf_binding};

void ui2_init(){
	ui_state=HUD_OFFLINE;

	boxes=(box_t**)malloc(sizeof(box_t*)*(n_boxes+1));

	int w=WIDTH;
	int h=HEIGHT;
	add_box(w/8,h/16,w-w/8-w/16,h-h/16-h/32,HUD_OFFLINE,CONF);

	int qw=WIDTH/4,qh=HEIGHT/4;
	w=qw*2;
	h=qh*2;

	add_box(w,h/2,qw,qh, CONF,CONF_DESC);
	add_box(w,h/2,qw,qh+h/2,CONF,CONF_BINDING);
}

void render_ui2(){
	void (*draw)(void);
	draw=draw_functions[ui_state];
	draw();
}
