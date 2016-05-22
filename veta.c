#include<X11/Xlib.h>
#include<X11/Xutil.h>

Display *dpy=NULL;
XSetWindowAttributes attr ;
Screen *screen;
Window win;
XVisualInfo visualinfo ;
Window win=0,root,parent=0;

int main(){

// B: save state 
// C: logging functionallity 

	int x,y,w,h;
	x=y=0;
	w=h=480;

	dpy=XOpenDisplay("");
	root=DefaultRootWindow(dpy);

	win=XCreateWindow(dpy,root,
						x,y,w,h,
		0,
		visualinfo.depth,
		InputOutput,
		visualinfo.visual,
		CWColormap|CWEventMask|CWBackPixmap|CWBorderPixel,
		&attr);

	XMapWindow(dpy,win);

	int running=1;
	XEvent ev;
	while(running){
		XNextEvent(dpy,&ev);
		switch(ev.type){
			case Expose:
//				XFillRectangle(dpy,win,blackgc,0,0,w,h);				
				XFlush(dpy);
				break;
		}
	}


	return 1;
}
