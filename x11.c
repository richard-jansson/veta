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
int x,y,_w,_h;
int lastX=0,lastY=0;
int running=1;

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
}

void ui_loop(){
	Window root_return,parent,*children_return;
	unsigned int n_children;
	XEvent ev;
	while(running){
		XNextEvent(dpy,&ev);
		switch(ev.type){
			case DestroyNotify:
					uk_log("destroy window");
					running=0;
					break;
			case Expose:
				uk_log("Expose");
				_draw_box(_w,_h,0,0,_rgb2XColor(128,128,128));
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
	
	uk_log("fill rectangle d=(%i,%i,%i,%i) c=(%i,%i,%i) p=%i",
			_w,_h,x,y,
			bg.red,bg.green,bg.blue,
			bg.pixel);
	
	XSetForeground(dpy,bg_gc,bg.pixel);
	XSetFillStyle(dpy,bg_gc,FillSolid);
	XFillRectangle(dpy,win,bg_gc,x,y,w,h);
	XFlush(dpy);
}
