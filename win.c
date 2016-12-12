/*
 * Platform specific code for the windows interface
 *
 * A good reference for GDI code:
 * http://zetcode.com/gui/winapi/gdi/
 *
 */

#include <windows.h>

#include <assert.h>

#include "debug.h"
#include "winapi.h"

#include "veta.h"
#include "keyboard_io.h"

// public 
int lastX=0,lastY=0;
HINSTANCE hinstance;
int nCmdShow;

// Configure keybindings
event_t kbio_undefined={0,UNDEFINED,-1};
event_t keybindings[]={
	{'Q',SELECT_CELL,0},
	{'W',SELECT_CELL,1},
	{'E',SELECT_CELL,2},
	{'R',SELECT_CELL,3},

	{'A',SELECT_CELL,4},
	{'S',SELECT_CELL,5},
	{'D',SELECT_CELL,6},
	{'F',SELECT_CELL,7},

	{'Z',SELECT_CELL,8},
	{'X',SELECT_CELL,9},
	{'Z',SELECT_CELL,10},
	{'V',SELECT_CELL,11},

	{160,RESET,-1}, // Shift 
	{192,QUIT,-1}, // Section
	{0,SWITCH_TREE,-1}}; 

// private
int lastsent;
symbol *symbol_list;
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
	int count='Z'-'A'+1;
	symbol_list=malloc(sizeof(symbol)*count);

	for(int i=0;i<count;i++){
		char *name=malloc(2);
		name[0]='A'+i;
		name[1]='\0';

		symbol_list[i].name=name;

		symbol_list[i].mode=TOGGLE;
		symbol_list[i].toggled=0;

		symbol_win *s=malloc(sizeof(symbol_win));
		s->c='A'+i;
		symbol_list[i].data=s;

	}

	onhaskeymap(symbol_list,count);
}

event_t *_get_event_from_keycode(int keycode){
	for(int i=0;i<sizeof(keybindings)/sizeof(event_t);i++){
		if(keybindings[i].keycode==keycode) return &keybindings[i];
	}
	return &kbio_undefined;
}

LRESULT CALLBACK _keyboard_hook(int n,WPARAM w,LPARAM l){
	PKBDLLHOOKSTRUCT p;
	int propagate=1;
	event_t *ev;
	if(n==HC_ACTION){
		switch(w){
			case WM_KEYDOWN:	
			case WM_KEYUP:
				p=(PKBDLLHOOKSTRUCT)l;

				propagate=1;
				uk_log("event");
				ev=_get_event_from_keycode(p->vkCode);

				uk_log("last=%i vkcode=%i",toupper(lastsent),p->vkCode);	

				if(toupper(lastsent)==p->vkCode && w==WM_KEYDOWN && ev->type!=UNDEFINED){
					uk_log("not receiving keys veta sent");
					return CallNextHookEx(NULL,n,w,l);
				}
				if(toupper(lastsent)==p->vkCode && w==WM_KEYUP && ev->type!=UNDEFINED){
					uk_log("not receiving keys veta sent");
					lastsent=-1;
					return CallNextHookEx(NULL,n,w,l);
				}

				if(w==WM_KEYUP && ev->type!=UNDEFINED){
					uk_log("get event type");
					onevent(ev);
				}
				if(ev->type!=UNDEFINED){
					uk_log("event is defined not sending on");
					propagate=0;
				}
				uk_log("keyup done");
				break;
			default:
				break;
		}
	}

	return propagate?CallNextHookEx(NULL,n,w,l):1;
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

	// Keyboard grabbing 
	SetWindowsHookEx(WH_KEYBOARD_LL,_keyboard_hook,0,0);

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
	symbol_win *d=(symbol_win *)s;
	assert(d);

	lastsent=d->c;

// Send the key
	keybd_event(d->c,0,0,0);	
	keybd_event(d->c,0,KEYEVENTF_KEYUP,0);
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

	DeleteObject(brush);
}
void draw_text_box(char *txt,int w,int h,int x,int y,rgb c1,rgb c2){
	RECT r={x,y,x+w,y+h};

	draw_box(w,h,x,y,c2.r,c2.g,c2.b);

	SetBkColor(_hdc,RGB(c2.r,c2.g,c2.b));

	HPEN pen=CreatePen(PS_SOLID,1,RGB(255,255,255));
	SelectObject(_hdc,pen);

	DrawText(_hdc,txt,-1,&r,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	DeleteObject(pen);
//	TextOutW(_hdc,x,y,txt,strlen(txt));
}
void refresh(){
	InvalidateRect(_win,NULL,TRUE);
}

/* debug */
void log_platformspecific(void  *data){
}
