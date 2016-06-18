#include<assert.h>
#include<string.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>


#include "keyboard_io.h"
#include"debug.h"
#include "veta.h"
#include "cell.h"

/* Private functions */
void _set_on_top(Display *dpy,Window window);
void _make_borderless(Display *dpy,Window win);
void draw_box(int w,int h,int x,int y,int r,int g,int b);
XColor _rgb2XColor(int r,int g,int b);
void _setupkeymap();
int _already_used(char *s);
int _is_modifier(KeyCode kc);
void _add_unique(char *s,KeySym ks,int mod,KeyCode keycode);
int _get_keycode_count(KeySym *keymap,int min,int max,int keysyms_per_keycode);
void _grabkeys();
void _ungrabkeys();
void draw_text_box(char *txt,int w,int h,int x,int y,rgb fg,rgb bg);
event_t *_get_event_from_keycode(int keycode);
/* End of private functions */

/* Callback functions / Events */
void (*onevent)(event_t *ev);
void (*onhaskeymap)(symbol *symbols,int n);
void (*onrender)();
/* End of callback funtions */ 


void ui_onevent(void(*callback)(event_t *ev)){
	onevent=callback;	
}

void ui_haskeymap(void(*callback)(symbol *symbols,int n)){
	onhaskeymap=callback;
}

void ui_render(void(*callback)()){
	onrender=callback;
}

event_t kbio_undefined={0,UNDEFINED,-1};
event_t keybindings[]={
	{24,SELECT_CELL,0},
	{25,SELECT_CELL,1},
	{26,SELECT_CELL,2},
	{27,SELECT_CELL,3},

	{38,SELECT_CELL,4},
	{39,SELECT_CELL,5},
	{40,SELECT_CELL,6},
	{41,SELECT_CELL,7},

	{52,SELECT_CELL,8},
	{53,SELECT_CELL,9},
	{54,SELECT_CELL,10},
	{55,SELECT_CELL,11},

	{50,RESET,-1}, // Shift 
	{49,QUIT,-1}, // Section
	{23,SWITCH_TREE,-1}}; // Tab

Display *dpy=NULL;
XSetWindowAttributes attr ;
int screen;
GC       gc;
XVisualInfo visualinfo ;
Window win=0,root,parent=0;
int x,y,_w,_h;
int lastX=0,lastY=0;
int running=1;
symbol *unique;
int n_unique=0,max_keysyms=0;
keyboardio kbio;
modifier_x11 *g_modifiers=NULL;
int g_modifier_count;

void ui_quit(){
	running=0;
}

void ui_init(int w,int h,int x,int y){
	_w=w;
	_h=h;
	dpy=XOpenDisplay("");
	screen=DefaultScreen(dpy);
	root=DefaultRootWindow(dpy);

	XMatchVisualInfo(dpy, screen, 32, TrueColor, &visualinfo);

	uk_log("Iniitializing UI");
	uk_log("Win @ (%i,%i,%i,%i)",x,y,w,h);
	
	 attr.colormap   = XCreateColormap( dpy, root, visualinfo.visual, AllocNone) ;
	 attr.event_mask = ExposureMask | KeyPressMask ;
	 attr.background_pixmap = None ;
	 attr.border_pixel      = 0 ;

	uk_log("root=%x\n",root);
	uk_log("%i %i %i %i\n",x,y,w,h);
	win=XCreateWindow(dpy,root,
						x,y,w,h,
		0,
		visualinfo.depth,
		InputOutput,
		visualinfo.visual,
		CWColormap|CWEventMask|CWBackPixmap|CWBorderPixel,
		&attr);
	gc = XCreateGC(dpy,win,0,0);

	uk_log("window = %x root = %x",win,root);

	_set_on_top(dpy,win);

	XMapWindow(dpy,win);
	XMoveWindow(dpy,win,x,y);

	// Try to make borderless
	_make_borderless(dpy,win);

	// Bind to what stuff?
	unsigned long eventmask;
	eventmask=StructureNotifyMask|SubstructureNotifyMask;
	uk_log("event mask = x%x",eventmask);
	XSelectInput(dpy,root,eventmask);

	XFlush(dpy);

	kbio.start=kbio.stop=kbio.n=0;
	_setupkeymap();

	_grabkeys();
}

void ui_loop(){
	Window root_return,parent,*children_return;
	unsigned int n_children;
	XEvent ev;
	event_t *v_event;
	while(running){
		XNextEvent(dpy,&ev);
		switch(ev.type){
			case KeyRelease:
				uk_log("KeyRelease %i",ev.xkey.keycode);
				v_event=_get_event_from_keycode(ev.xkey.keycode);
				onevent(v_event);
				onrender();
				XFlush(dpy);
				break;
			case DestroyNotify:
				uk_log("destroy window");
				running=0;
				break;
			case Expose:
/*				uk_log("Expose");
				_draw_box(_w,_h,0,0,_rgb2XColor(128,128,128));*/
				onrender();
				XFlush(dpy);
				break;
			case ReparentNotify:
				uk_log("Reparent win=%x par=%x",ev.xreparent.window,ev.xreparent.parent);
				parent=ev.xreparent.parent;
				break;
			case ConfigureNotify:
				// Let's start out with window
				for(Window cwin=win; cwin != 0; cwin=parent){
					XQueryTree(dpy,cwin,&root_return,&parent,&children_return,&n_children);
					if(parent == root){
						XWindowAttributes attr;
						XGetWindowAttributes(dpy,cwin,&attr);
						lastX=attr.x; lastY=attr.y;
						uk_log("window frame %x has pos (%i,%i)",lastX,lastY);
					}
				}
				break;
		}
	}	
	uk_log("ui_loop end!");
	writestate(STATE_FILE,lastX,lastY);
}
// Set x11 window on top
void _set_on_top(Display *dpy,Window window){
	XEvent ev;
	Atom net_wm_state = XInternAtom(dpy,"_NET_WM_STATE",False);
	Atom net_wm_state_above = XInternAtom(dpy,"_NET_WM_STATE_ABOVE",False);
	Window root=DefaultRootWindow(dpy);

	ev.xclient.type=ClientMessage;
	ev.xclient.serial=0;
	ev.xclient.send_event=True;
	ev.xclient.display=dpy;
	ev.xclient.window=window,
	ev.xclient.message_type=net_wm_state;
	ev.xclient.format=32;

	ev.xclient.data.l[0]=1;
	ev.xclient.data.l[1]=net_wm_state_above;
	ev.xclient.data.l[2]=0;
	ev.xclient.data.l[3]=0;
	ev.xclient.data.l[4]=0;

	XSendEvent(dpy,root,False,
                       SubstructureRedirectMask|SubstructureNotifyMask, &ev);

}
void _make_borderless(Display *dpy,Window win){
	struct {
		unsigned long flags;
		unsigned long functions;
		unsigned long decorations;
		long input_mode;
		unsigned long status;
	} mhints={(1L<<1),0,0,0,0};
	Atom wm_hints=XInternAtom(dpy,"_MOTIF_WM_HINTS",True);

	XChangeProperty(dpy,win,wm_hints,wm_hints,32,
						PropModeReplace,(unsigned char*)&mhints,sizeof(mhints)/sizeof(long));
}

XColor _rgb2XColor(int r,int g,int b){
	XColor c1;
	c1.red=r*256;
	c1.green=g*256;
	c1.blue=b*256;
	return c1;
}

void draw_box(int w,int h,int x,int y,int r,int g,int b){
	XColor bg=_rgb2XColor(r,g,b);
	GC bg_gc=XCreateGC(dpy,win,0,0);

	XAllocColor(dpy,attr.colormap,&bg);
	
	XSetForeground(dpy,bg_gc,bg.pixel);
	XSetFillStyle(dpy,bg_gc,FillSolid);
	XFillRectangle(dpy,win,bg_gc,x,y,w,h);
	XFlush(dpy);
}
void _get_unique_keycodes(KeySym *keymap,int min,int max,int keysyms_per_keycode,int pass){
	// We really shouldn't skip all modifiers after 3
	int num=0;
	for(int modifier=0;modifier<keysyms_per_keycode && modifier < MOD_MAX;modifier++){
		// We don't need caps lock since shift will do the same
//		if(modifier == 1 ) continue;
		for(int kcode=min;kcode<=max;kcode++){
			KeySym ks=keymap[(kcode-min)*keysyms_per_keycode+modifier];
			if( ks != NoSymbol ){
				char *s=XKeysymToString(ks);
				// TODO: perhaps skipping XF86 and SUN is too a harsh design decision
				if(!_already_used(s) && strncmp("XF86",s,4) && strncmp("SUN",s,3)  
						&& strncmp("KP_",s,3)){

					if(pass==0 && _is_modifier(kcode)) _add_unique(s,ks,modifier,kcode);
					if(pass==1 && !_is_modifier(kcode)) _add_unique(s,ks,modifier,kcode);
				}
			}
		}
	}
}
void _setupkeymap(){
	uk_log("setup keymap");
	int min,max,keysyms_per_keycode;
	KeySym *keymap;
	XModifierKeymap *modifiers;
	modifiers=XGetModifierMapping(dpy);
	
//	_setup_modifiers(modifiers);

	XDisplayKeycodes(dpy,&min,&max);
	keymap = XGetKeyboardMapping(dpy,min,max-min+1,&keysyms_per_keycode);

	int n_keysyms=_get_keycode_count(keymap,min,max,keysyms_per_keycode);
	uk_log("got %i keysyms",n_keysyms);
//	_print_keycodes(keymap,min,max,keysyms_per_keycode);
	unique=malloc(n_keysyms*sizeof(symbol));
	_get_unique_keycodes(keymap,min,max,keysyms_per_keycode,0);
	_get_unique_keycodes(keymap,min,max,keysyms_per_keycode,1);
	uk_log("got %i unique",n_unique);

	onhaskeymap(unique,n_unique);
}
int _already_used(char *s){
	for(int i=0;i<n_unique;i++)
		if(!strcmp(unique[i].name,s)){
			return 1;
		}
	return 0;
}
int _is_modifier(KeyCode kc){
//	assert(g_modifiers!=NULL);
	for(int i=0;i<g_modifier_count;i++) if(g_modifiers[i].kc==kc) return 1;
	return 0;
}
void _add_unique(char *s,KeySym ks,int mod,KeyCode keycode){
//	assert(n_unique < max_keysyms);
	unique[n_unique].name=malloc(strlen(s));
	memcpy(unique[n_unique].name,s,strlen(s)+1);
	
	symbol_x11 *d=malloc(sizeof(symbol_x11));
	d->ks=ks;
	d->modifier=mod;
	d->keycode=keycode;

	unique[n_unique].mode=_is_modifier(keycode)?TOGGLE:NORMAL;
	unique[n_unique++].data=d;
}
int _get_keycode_count(KeySym *keymap,int min,int max,int keysyms_per_keycode){
	int count_keysyms=0;
	for(int kcode=min;kcode<=max;kcode++){
		for(int keysym=0;keysym<keysyms_per_keycode;keysym++){
			KeySym ks=keymap[(kcode-min)*keysyms_per_keycode+keysym];
			if(ks!=NoSymbol) count_keysyms++;
		}
	}
	max_keysyms=count_keysyms;
	return count_keysyms;
}
void _grabkeys(){
	Window root = DefaultRootWindow(dpy);
	// Shift, Lock, ctrl
	unsigned int modifiers=0b100;
	uk_log("Grabbing keys");
	for(int i=0;i<sizeof(keybindings)/sizeof(event_t);i++){
//		uk_log("modifiers = 0x%x\n",modifiers);
		XGrabKey(dpy,keybindings[i].keycode,0,root,False,GrabModeAsync,GrabModeAsync);
		XGrabKey(dpy,keybindings[i].keycode,1,root,False,GrabModeAsync,GrabModeAsync);
		XGrabKey(dpy,keybindings[i].keycode,2,root,False,GrabModeAsync,GrabModeAsync);
		XGrabKey(dpy,keybindings[i].keycode,4,root,False,GrabModeAsync,GrabModeAsync);
//		XGrabKey(dpy,keybindings[i].keycode,8,root,False,GrabModeAsync,GrabModeAsync);
//		XGrabKey(dpy,keybindings[i].keycode,16,root,False,GrabModeAsync,GrabModeAsync);
	}
	XFlush(dpy);
}
void _ungrabkeys(){
	uk_log("Ungrabbing keys");
	Window root = DefaultRootWindow(dpy);
	for(int i=0;i<sizeof(keybindings)/sizeof(event_t);i++){
		XUngrabKey(dpy,keybindings[i].keycode,0,root);
/*		XUngrabKey(dpy,keybindings[i].keycode,1,root);
		XUngrabKey(dpy,keybindings[i].keycode,2,root);
		XUngrabKey(dpy,keybindings[i].keycode,4,root);*/
//		XUngrabKey(dpy,keybindings[i].keycode,8,root);
//		XUngrabKey(dpy,keybindings[i].keycode,16,root);
	}
	XFlush(dpy);
}
event_t *_get_event_from_keycode(int keycode){
	for(int i=0;i<sizeof(keybindings)/sizeof(event_t);i++){
		if(keybindings[i].keycode==keycode) return &keybindings[i];
	}
	return &kbio_undefined;
}

void draw_text_box(char *txt,int w,int h,int x,int y,rgb c1,rgb c2){
	XColor fg=_rgb2XColor(c1.r,c1.g,c1.b);
	XColor bg=_rgb2XColor(c2.r,c2.g,c2.b);
	assert(txt);

	draw_box(w,h,x,y,c2.r,c2.g,c2.b);

	GC fg_gc=XCreateGC(dpy,win,0,0);
	XAllocColor(dpy,attr.colormap,&fg);
	XSetForeground(dpy,fg_gc,fg.pixel);

	uk_log("Draw text!!!!");
	x+=w/4;
	y+=h/2;
	XDrawString(dpy,win,fg_gc,x,y,txt,strlen(txt));
	XFlush(dpy);


/*	SDL_Surface *line = TTF_RenderText_Shaded(font,txt,fg,bg);
	if(!line) return;
	SDL_Rect fontdst={
				x+(w-line->w)/2,
				y+(h-line->h)/2,
				line->w,
				line->h};
	SDL_BlitSurface(line,NULL,window,&fontdst); */
}
