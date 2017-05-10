// FIXME make this into a generalized interface such as vid_out 
typedef struct _img_t {
	int w,h;
	char *path;
	FILE *fd;
	unsigned char *buf;
} img_t;

img_t * img_open(char *name,int w,int h);
// -1 on error
void img_getfmt(img_t *i,int *w,int *h);
void img_setpixel(img_t *img,int x,int y,rgb c);
void img_close(img_t *i);
