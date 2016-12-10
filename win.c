/*
 * Platform specific code for the windows interface
 *
 */

#include <windows.h>

#include "veta.h"
#include "keyboard_io.h"

// public 
int lastX=0,lastY=0;

// private
int running;

/* Starts the ui backend code */
void ui_init(int w,int h,int x,int y){
	running=0;	
}
void ui_quit(){
	running=0;
}
void ui_loop(int full_throttle){
	while(running){
	}
}

/* platform specific keyboard functions */
void sendkey(void *s,int press_and_release,int toggled){
}
void grabkeyboard(){
}
void ungrabkeyboard(){
}

/* Drawing helpers */
void draw_box(int w,int h,int x,int y,int r,int g,int b){
}
void draw_text_box(char *txt,int w,int h,int x,int y,rgb c1,rgb c2){
}
void refresh(){
}

/* Configure callback functions */
void (*onevent)(event_t *ev);
void (*onclick)(int x,int y);
void (*onrelease)(char *symbol,int *propagate,vkey key,void *data);
void (*onhaskeymap)(symbol *symbols,int n);
void (*onrender)();
void ui_onevent(void(*callback)(event_t *ev)){
	onevent=callback;	
}
void ui_onclick(void(*callback)(int x,int y)){
	onclick=callback;
}
void ui_onrelease(void(*callback)(char *s,int *p,vkey key,void *data)){
	onrelease=callback;
}
void ui_haskeymap(void(*callback)(symbol *symbols,int n)){
	onhaskeymap=callback;
}
void ui_render(void(*callback)()){
	onrender=callback;
}
/* debug */
void log_platformspecific(void  *data){
}
