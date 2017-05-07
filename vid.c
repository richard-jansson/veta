// FIXME integrate into interface 

#include <stdio.h>

#include"color.h"
//#include"vidout.h"
#include"img.h"

int _frame = 0;


// FIXME remove upon cleanup
rgb getpixel(int x,int y);

// FIXME: should be integrated into vo and take no parameters
void vo_screenshot(char *garbage,int w,int h){
	// potential overflow?
	char name[256];

	sprintf(name,"%i",_frame++);

	img_t *img=img_open(name,w,h);
	for(int y=w;y;y--){
		for (int x = h;x;x--) img_setpixel(img,x, y, getpixel(x, y));
	}
}
