/* 
 * Try to find the modifiers which grabs as many modifiers as possible for a given keycode.
 * Whenever an X11 request files the Xlib is nice enough to kill our process. To work around
 * this we have to execute the request in a different process and then see if the process is
 * killed or exits normaly 
 */
#include<stdlib.h>
#include<stdio.h>

#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#include<X11/Xlib.h>
#include<X11/Xutil.h>

int _test_grabkey(int kc,int mod){
	Display *dpy=XOpenDisplay("");
	Window root=DefaultRootWindow(dpy);

	XEvent ev;

	unsigned long eventmask;
	eventmask=StructureNotifyMask|SubstructureNotifyMask|ExposureMask;

	XSelectInput(dpy,root,eventmask);

	XGrabKey(dpy,kc,mod,root,False,GrabModeAsync,GrabModeAsync);
	XFlush(dpy);

	// Since XNextEvent is nonblocking and the request is apparently not processed until it's run
	// we'll have to send some sort of event to the X server or simply wait for it to be generated
	// in some other way.
	ev.xclient.type=ClientMessage;
	ev.xclient.serial=0;
	ev.xclient.send_event=True;
	ev.xclient.display=dpy;
	ev.xclient.window=root;
	ev.xclient.message_type=0;
	ev.xclient.format=32;

	XSendEvent(dpy,root,False,SubstructureRedirectMask|SubstructureNotifyMask,&ev);
	XFlush(dpy);

	XNextEvent(dpy,&ev);
}

int _testkey(int kc,int mod){
	pid_t pid=fork();
	int status;
	if(pid){
		waitpid(pid,&status,0);  
		if(WIFEXITED(status)){
			return !WEXITSTATUS(status);
		}
		if(WIFSIGNALED(status)){
			return 0;
		}
	}else{
	// Close stderr since we are not interested in the errors for this process
		close(2);
		_test_grabkey(kc,mod);
		exit(0);
	}
}

// get all the modifiers which it's possible to bind to for a specific keycode (kc)
int get_modifiers(int keycode){
	int mod=0;
	
	int modifiers=0;
	for(int i=0;i<8;i++){
		if(_testkey(keycode,mod)) modifiers|=mod;
		mod=!mod?1:mod*2;
	}
	
	return modifiers;
}
