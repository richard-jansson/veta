#include<stdint.h>
#include<stdlib.h>

// Constants for RGB - YUV conversion
#define C_1 1.370705
#define C_2 0.698001
#define C_3 0.337633
#define C_4 1.732446

#define MAX_U 0.436
#define MAX_V 0.615

#define SEED 0x45564f4c

#define STUCK_TRESHOLD 500

typedef struct _rgb {
	uint8_t r,g,b;
} rgb;

typedef struct _yuv {
	double y,u,v;
} yuv;

void init_color(int n_colors);
void destroy_color();
void print_yuv(yuv c);
void print_rgb(rgb c);

void generate_colors(int static_component,double dist_treshold);
rgb get_color(int n,int bright);

void yuv2rgb(rgb *rgb,yuv yuv);
