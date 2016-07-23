/* This is platform specific */ 

/* TODO: remove SDL references */
#include<X11/Xlib.h>
#include<SDL/SDL_thread.h>

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
} event_t;

keyboardio * init_keyboardio();
int get_event(event_t *event);
void sendkey(void *sym,int press_and_release,int toggled);
int is_modifier(KeyCode keycode);

KeyCode get_modifier_keycode(int modifier);
KeySym get_modifier_keysym(int modifier);

char *get_select_cell_name(int n);
