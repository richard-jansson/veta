#include<X11/Xlib.h>
#include<X11/Xutil.h>

#include"debug.h"

/* Private functions */
void _set_on_top(Display *dpy,Window window);
void _make_borderless(Display *dpy,Window win);
void _draw_box(int w,int h,int x,int y,XColor col);
XColor _rgb2XColor(int r,int g,int b);
/* End of private functions */


Display *dpy=NULL;
XSetWindowAttributes attr ;
int screen;
GC       gc;
Window win;
XVisualInfo visualinfo ;
Window win=0,root,parent=0;
int x,y,w,h;
int running=1;

void ui_init(){
	dpy=XOpenDisplay("");
	screen=DefaultScreen(dpy);
	root=DefaultRootWindow(dpy);

	XMatchVisualInfo(dpy, screen, 32, TrueColor, &visualinfo);

	uk_log("Iniitializing UI");

	w=h=480;
	
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

/*	GLXContext glcontext=glXCreateContext(dpy,&visualinfo,0,True);
	if(!glcontext){
		uk_log("No open gl support!");
		return 1;
	}
	glXMakeCurrent(dpy,win,glcontext);*/
	XMapWindow(dpy,win);

	XMoveWindow(dpy,win,x,y);


	// Try to make borderless
	_make_borderless(dpy,win);
}

void ui_loop(){
	XEvent ev;
	while(running){
		XNextEvent(dpy,&ev);
		switch(ev.type){
			case Expose:
				_draw_box(w,h,0,0,_rgb2XColor(128,128,128));
				break;
		}
	}
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

void _draw_box(int w,int h,int x,int y,XColor bg){
	GC bg_gc=XCreateGC(dpy,win,0,0);

	XAllocColor(dpy,attr.colormap,&bg);
	XSetForeground(dpy,bg_gc,bg.pixel);
	XSetFillStyle(dpy,bg_gc,FillSolid);
	XFillRectangle(dpy,win,bg_gc,x,y,w,h);
	XFlush(dpy);
}
