typedef int widget;

typedef struct {
	int x,y,w,h;
	rgb bg,fg;
	int visible; // Visible means focus?? 
	void *onclick;
	void *onrelease;
	void *draw; // draw function 	
	char *label;
} widget_t;


void text_draw(widget_t *this,int x0,int y0,int w0,int h0);
void widget_set_label(widget w,char *label);
