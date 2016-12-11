/*
 * Platform specific code for the windows interface
 *
 */

#include <windows.h>

#include "debug.h"
#include "winapi.h"

#include "veta.h"
#include "keyboard_io.h"

// public 
int lastX=0,lastY=0;
HINSTANCE hinstance;
int nCmdShow;

// private
int running;
HWND _win;
MSG _msg;
const char _classname[] = "vetawinclass";
PAINTSTRUCT _ps;
HDC _hdc;

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
LRESULT CALLBACK _win_callback(HWND win,UINT msg,WPARAM w,LPARAM l){
	switch(msg){
		case WM_PAINT:
			_hdc=BeginPaint(_win,&_ps);
			onrender();	
			EndPaint(_win,&_ps);
			break;
		default:
			break;
	}
	return DefWindowProc(win,msg,w,l);
}

void _setupkeymap(){
// FIXME: get the symbols from the current layout?
	symbol *symbol_list=malloc(sizeof(symbol)*26*2);
	for(int i=0;i<26;i++){
		symbol_list[i].name=malloc(2);
		symbol_list[i].name[0]='a'+i;
		symbol_list[i].name[0]='\0';
		symbol_list[i].data=NULL;
		symbol_list[i].mode=TOGGLE;
		symbol_list[i].toggled=0;
	}
	for(int i=0;i<26;i++){
		symbol_list[i+25].name=malloc(2);
		symbol_list[i+25].name[0]='A'+i;
		symbol_list[i+25].name[0]='\0';
		symbol_list[i+25].data=NULL;
		symbol_list[i+25].mode=TOGGLE;
		symbol_list[i+25].toggled=0;
	}
	for(int i=0;i<10;i++){
		symbol_list[i+51].name=malloc(2);
		symbol_list[i+51].name[0]='0'+i;
		symbol_list[i+51].name[0]='\0';
		symbol_list[i+51].data=NULL;
		symbol_list[i+51].mode=TOGGLE;
		symbol_list[i+51].toggled=0;
	}

	onhaskeymap(symbol_list,26*2+10);
}

/* Starts the ui backend code */
void ui_init(int w,int h,int x,int y){
	running=1;	

	WNDCLASSEX wc;
	
// Create window class
	wc.cbSize=sizeof(WNDCLASSEX);
	wc.style=0;
	wc.lpfnWndProc = _win_callback;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=hinstance;
	wc.hIcon = LoadIcon(NULL,"IDI_APPLICATION");
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _classname;
	wc.hIconSm=LoadIcon(NULL,"IDI_APPLICATION");

	if(!RegisterClassEx(&wc)){
		printf("Couldn't register class\n");
	}

// Create actual window
	_win=CreateWindowEx(WS_EX_CLIENTEDGE|CS_HREDRAW|CS_VREDRAW,
				_classname,
				"Veta",
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,CW_USEDEFAULT,WIDTH,HEIGHT,
				NULL,NULL,hinstance,NULL);


	if(!_win) printf("Couldn't create window");

	_setupkeymap();

	ShowWindow(_win,nCmdShow);
	UpdateWindow(_win);

}
void ui_quit(){
	running=0;
}
void ui_loop(int full_throttle){
	MSG msg;
	while(GetMessage(&msg,NULL,0,0) > 0 && running){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

/* platform specific keyboard functions */
void sendkey(void *s,int press_and_release,int toggled){
	uk_log("sendkey not implemented yet");
}
void grabkeyboard(){
	uk_log("grabkeyboard not implemented yet");
}
void ungrabkeyboard(){
	uk_log("ungrabkeyboard not implemented yet");
}

/* Drawing helpers */

void draw_box(int w,int h,int x,int y,int r,int g,int b){

	HPEN pen=CreatePen(PS_NULL,1,RGB(r,g,b));
	
	HBRUSH brush=CreateSolidBrush(RGB(r,g,b));
	
	SelectObject(_hdc,brush);

	Rectangle(_hdc,x,y,x+w,y+h);
}
void draw_text_box(char *txt,int w,int h,int x,int y,rgb c1,rgb c2){
	draw_box(w,h,x,y,c2.r,c2.g,c2.b);
	uk_log("draw_text_box not implemented");
}
void refresh(){
	uk_log("refresh");
}

/* debug */
void log_platformspecific(void  *data){
}
