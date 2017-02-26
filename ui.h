// Public functions
void ui_init(int w,int h,int x,int y);
void ui_loop();

// Public variables 
extern int lastX,lastY;

// Private functions
//void set_on_top(Display *dpy,Window window);


void ui_onevent(void(*onevent)(event_t *ev));
void ui_haskeymap(void(*has_keymap)(symbol *keymap,int n));
void ui_quit();
void ui_render();
void refresh();
void log_platformspecific(void  *data);

void widget_set_visible(widget w,int v);
void grabkeyboard();
void ungrabkeyboard();
widget add_widget(char *label,
	void(*draw)(widget_t *this,int x,int y,int w,int h),
	void(*onclick)(widget_t *w),
	void(*onrelease)(widget_t *w,char *s,int *p,vkey key,void *pspecific));
