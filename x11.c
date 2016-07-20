#include<assert.h>
#include<string.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>

#include "keyboard_io.h"
#include"debug.h"
#include "veta.h"
#include "cell.h"

#define FORMAT "%s %i %i %i\n"

int winX=0,winY=0;
int lastMX=0,lastMY=0;
void get_last_click(int *x,int *y){
	*x=lastMX;
	*y=lastMY;
}
/* Global variables */
FILE *sym_file=NULL;

/* Global */ 

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
void (*onclick)(int x,int y);
void (*onhaskeymap)(symbol *symbols,int n);
void (*onrender)();
/* End of callback funtions */ 


void ui_onevent(void(*callback)(event_t *ev)){
	onevent=callback;	
}

void ui_onclick(void(*callback)(int x,int y)){
	onclick=callback;
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
Window win=0,root,win_parent=0;
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
	winX=x;
	winY=y;
	
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
	_set_sticky(dpy,win);

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

void get_win_pos(Window *win,int *x,int *y){
	int X=0;
	int Y=0;
	int i=0;
	unsigned int n_children;
	Window root_return,*children_return,parent;
	
	for(Window cwin=win; cwin != 0; cwin=parent){
				// I'm sure this is cheating!
					XQueryTree(dpy,cwin,&root_return,&parent,&children_return,&n_children);


					// Don't ask. This is pure magic!
					// Probably it only works on Ubuntu
					// there are for some 2 windows around my window 
					// until I can see the  actual position of the window. 
					// Milage may vary. 
						XWindowAttributes attr;
						XGetWindowAttributes(dpy,cwin,&attr);
						X+=attr.x; 
						Y+=attr.y;
					i++;
		}

		*x=X;
		*y=Y;
}

void ui_loop(){
	Window root_return,*children_return,parent;
	unsigned int n_children;
	XEvent ev;
	event_t *v_event;
	int i;
	XWindowAttributes winattr;
	int x,y;
	while(running){
		XNextEvent(dpy,&ev);
		switch(ev.type){
			case ButtonRelease:
			case ButtonPress:
				lastMX=ev.xbutton.x;
				lastMY=ev.xbutton.y;
				get_win_pos(win,&x,&y);
				//  112 just because ... 
				onclick(lastMX-x-112,lastMY-y);
				break;
			case KeyRelease:
				uk_log("KeyRelease %i",ev.xkey.keycode);
				v_event=_get_event_from_keycode(ev.xkey.keycode);
				onevent(v_event);
				onrender();
				XFlush(dpy);
				break;
			case DestroyNotify:
				uk_log("destroy window");
//				uk_log("subwindow = %i\n",ev.subwindow);
				//  Destroy notify here makes the program close whenever you open 
				// A new window on my machine. Wonder why?
//				running=0;
				break;
			case Expose:
/*				uk_log("Expose");
				_draw_box(_w,_h,0,0,_rgb2XColor(128,128,128));*/
				onrender();
				XFlush(dpy);
				break;
			case ReparentNotify:
				uk_log("Reparent win=%x par=%x",ev.xreparent.window,ev.xreparent.parent);
				if(ev.xreparent.parent!=0){	
					win_parent=ev.xreparent.parent;
				}
				break;
			case ConfigureNotify:
				get_win_pos(win,&lastX,&lastY);
				// Let's start out with window
/*				i=0;	
				for(Window cwin=win; cwin != 0; cwin=parent){
				// I'm sure this is cheating!
					winX=ev.xconfigure.x;
					winY=ev.xconfigure.y;
					XQueryTree(dpy,cwin,&root_return,&parent,&children_return,&n_children);


					// Don't ask. This is pure magic!
					// Probably it only works on Ubuntu
					// there are for some 2 windows around my window 
					// until I can see the  actual position of the window. 
					// Milage may vary. 
					if(i ==  2){
						XWindowAttributes attr;
						XGetWindowAttributes(dpy,cwin,&attr);
						lastX=attr.x; lastY=attr.y;
						uk_log("windows moved to (%i,%i)",lastX,lastY);
					}
					i++;
				}
				*/
				break;
		}
	}	
	uk_log("ui_loop end!");
	writestate(STATE_FILE,lastX,lastY);
}
void _set_sticky(Display *dpy,Window window){
	XEvent ev;
	Atom net_wm_state = XInternAtom(dpy,"_NET_WM_STATE",False);
	Atom net_wm_state_sticky = XInternAtom(dpy,"_NET_WM_STATE_STICKY",False);
	Window root=DefaultRootWindow(dpy);

	ev.xclient.type=ClientMessage;
	ev.xclient.serial=0;
	ev.xclient.send_event=True;
	ev.xclient.display=dpy;
	ev.xclient.window=window,
	ev.xclient.message_type=net_wm_state;
	ev.xclient.format=32;

	ev.xclient.data.l[0]=1;
	ev.xclient.data.l[1]=net_wm_state_sticky;
	ev.xclient.data.l[2]=0;
	ev.xclient.data.l[3]=0;
	ev.xclient.data.l[4]=0;

	XSendEvent(dpy,root,False,
                       SubstructureRedirectMask|SubstructureNotifyMask, &ev);

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
//	XFillRectangle(dpy,win,bg_gc,x0,y0,w0,h0);
	XFillRectangle(dpy,win,bg_gc,x,y,w,h);
	XFlush(dpy);
}
void _get_unique_keycodes(KeySym *keymap,int min,int max,int keysyms_per_keycode,int pass){
	// We really shouldn't skip all modifiers after 3
	int num=0;

	if(symbol_mode==DUMP){
		uk_log("Opening %s\n",symbol_file);
		sym_file=fopen(symbol_file,"w+");
		if(!sym_file){
		}
		assert(sym_file);
		fprintf(sym_file,"# x11\n");
		fprintf(sym_file,"#\n");
		fprintf(sym_file,"# line starting with  # is a comment\n");
		fprintf(sym_file,"# Only edit below this line!\n");
	}

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
	if(symbol_mode==LOAD){
		char *line=NULL;
		size_t len=0;
		int read=0;

		sym_file=fopen(symbol_file,"r");
		assert(sym_file);
		int  n_keysyms=0;
		while((read=getline(&line,&len,sym_file)) != -1){
			if(*line=='#') continue;
			n_keysyms++;
		}	
		unique=malloc(n_keysyms*sizeof(symbol));
		assert(unique);
		uk_log("we have %i keysysm in %s\n",n_keysyms,symbol_file);
	
		fseek(sym_file,0,SEEK_SET);
		while((read=getline(&line,&len,sym_file)) != -1){
			if(*line=='#') continue;
			char name[128];
			int ks,mod,keycode;
			sscanf(line,FORMAT,&name,&ks,&mod,&keycode);
			uk_log(FORMAT,name,ks,mod,keycode);
			_add_unique(name,ks,mod,keycode);
		}

		onhaskeymap(unique,n_unique);
		return;
	}
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
	if(symbol_mode==DUMP){
//§		fprintf(sym_file,"[%s]\t%i\t%i\t%i\n",s,(int)ks,mod,keycode);
		fprintf(sym_file,FORMAT,s,(int)ks,mod,keycode);
	}
//	uk_log("[%s]\t%i\t%i\t%i",s,ks,mod,keycode);
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

// Shouldn't be here 
// if it is function must be renamed
	XGrabButton(dpy,0,0,root,False, 0x0,GrabModeAsync,GrabModeAsync,None,None);

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

void sendkey(void *s,int press_and_release,int toggled){
	symbol_x11 *sym=(symbol_x11 *)s;
	KeyCode modifier;
	KeySym modifier_sym;
	assert(dpy);

	kbio.lastsent=sym->keycode;

	ungrabkeys();
	XFlush(dpy);

	uk_log("send keysym 0x%x keycode %i %s modifier=%i",
			(unsigned int)sym->ks,
			(int)sym->keycode,
			XKeysymToString(sym->ks),
			sym->modifier);

	XTestFakeKeyEvent(dpy,sym->keycode,1,CurrentTime); 
	XFlush(dpy);
	XTestFakeKeyEvent(dpy,sym->keycode,0,CurrentTime+100); 
	XFlush(dpy);

	XFlush(dpy);

	grabkeys();
}

void draw_text_box(char *txt,int w,int h,int x,int y,rgb c1,rgb c2){
	XColor fg=_rgb2XColor(c1.r,c1.g,c1.b);
	XColor bg=_rgb2XColor(c2.r,c2.g,c2.b);
	XColor white=_rgb2XColor(255,255,255);
	assert(txt);

	int x0,y0,w0,h0;
	x0=x;
	y0=y;
	w0=w;
	h0=h;

	draw_box(w,h,x,y,c2.r,c2.g,c2.b);
//	draw_box(w0,h0,x0,y0,c2.r,c2.g,c2.b);

	GC fg_gc=XCreateGC(dpy,win,0,0);
	GC white_gc=XCreateGC(dpy,win,0,0);
	XAllocColor(dpy,attr.colormap,&white);
	
	XSetForeground(dpy,white_gc,white.pixel);
	XFlush(dpy);
	XDrawRectangle(dpy,win,white_gc,x,y,w,h);

	XSetForeground(dpy,fg_gc,fg.pixel);
	XFlush(dpy);

//	uk_log("Draw text!!!!");

/* Please, pick one! 
	char **fonts;
	int n_fonts;
	fonts=XListFonts(dpy,"*",2000,&n_fonts);
	for(int i=0;i<n_fonts;i++){
//		printf("%s\n",fonts[i]);

		uk_log("font = %s\n",fonts[i]);
	}
u*/
//j	char *font_name="-misc-fixed-medium-r-normal--18-120-100-100-c-90-iso8859-1";
//	char *font_name="-adobe-avant garde gothic-book-o-normal--0-0-0-0-p-0-iso8859-1";
//	char *font_name="-adobe-avantgarde-medium-i-normal--0-0-0-0-p-0-iso8859-1";
	char *font_name=FONT;
	Font font=XLoadFont(dpy,font_name);

	XSetFont(dpy,white_gc,font);
	
	XDrawString(dpy,win,white_gc,x+w*0.2,y+=h0/2,txt,strlen(txt));
//	XTextItem ti[]={{txt,strlen(txt),1,font}};

//	XDrawText(dpy,win,white_gc,x+w*0.2,y+=h0/2,,0);

	XFlush(dpy);

/*	
	Perhaps you want it in SDL Instead!

	SDL_Surface *line = TTF_RenderText_Shaded(font,txt,fg,bg);
	if(!line) return;
	SDL_Rect fontdst={
				x+(w-line->w)/2,
				y+(h-line->h)/2,
				line->w,
				line->h};
	SDL_BlitSurface(line,NULL,window,&fontdst); */
}
void grabkeys(){
	Window root = DefaultRootWindow(dpy);
	// Shift, Lock, ctrl
	unsigned int modifiers=0b100;
	uk_log("Grabbing keys");
	/* Of what is going on here. I have genuienly no idea! It just works for me */
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
void ungrabkeys(){
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
