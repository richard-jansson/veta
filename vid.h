// FIXME: refactor. X11.c and win.c accordingly
struct _vid_out_t {
	void (*onevent)(event_t *ev);
	void (*onclick)(int *x,int *y);
	void (*onrelease)(char *symbol,int *propagate,vkey key,void *data);
	void (*onhaskeymap)(symbol *symbols, int n):
	void (*onrender)();
	void (*draw_text_box)(char *txt,int w,int h,int x,rgb c1,rgb c2);
	void (*dump)(char **img);
} vidout;

vidout *initvo(const char *name);

void destroy_vo(vidout *vo);


void vo_screenshot(char *name);
