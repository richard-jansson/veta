/* This is platform specific */ 

#ifdef X11
#include<X11/Xlib.h>

typedef struct _modifier_x11 {
	int modifier;
	KeyCode kc;
	KeySym ksym;
}modifier_x11;

typedef struct _symbol_x11 {
// TODO: remove ks, since it's only used when printing log msg
	KeySym ks;
	KeyCode keycode;
	int modifier;
	int is_modifier;
}symbol_x11;


int is_modifier(KeyCode keycode);
KeyCode get_modifier_keycode(int modifier);
KeySym get_modifier_keysym(int modifier);
#endif 

#ifdef WINDOWS
typedef struct _symbol_win {
	char c;
}symbol_win;

typedef struct _modifier_win {
}modifier_win;
#endif

typedef struct _keyboardio{
//	SDL_mutex *mutex;
	int start,stop,n;
	int keycodes[128];
//	Display *dpy;
//	SDL_Thread *thread;
	int lastsent;
} keyboardio;

typedef enum {UNDEFINED,SELECT_CELL,RESET,QUIT,SWITCH_TREE} eventtype;

typedef struct _event{
	int keycode;
	eventtype type;
	int cell;
	int modifiers;
} event_t;

keyboardio * init_keyboardio();
int get_event(event_t *event);
void sendkey(void *sym,int press_and_release,int toggled);


char *get_select_cell_name(int n);
