typedef struct {
	int x,y,w,h;
	rgb bg,fg;
	int visible; // Visible means focus?? 
	void *onclick;
	void *onrelease;
	void *draw; // draw function 	
	char *label;
} widget_t;

typedef int widget;

void text_draw(widget_t *this,int x0,int y0,int w0,int h0);
