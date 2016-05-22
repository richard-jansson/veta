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
