#include "ui2.h"

#define SCALE 1

#define ASPECT_HOR 800
#define ASPECT_VER 600

#define FONT_SIZE  48* SCALE

#define WIDTH ASPECT_HOR * SCALE	
#define HEIGHT ASPECT_VER * SCALE
#define WIDTH 800
#define HEIGHT 600

#define MOD_MAX 1

#define STATE_FILE "veta.state"
#define LOG_FILE "veta.log"
#define SYMBOLS_FILE "symbols"

#define  FONT "-misc-fixed-bold-r-normal--18-120-100-100-c-90-iso8859-1"

#define CELL_SIZE 12 
#define TREE_DEPTH 2

#define TOP_W WIDTH

#define CELL_TOT_W WIDTH
#define CELL_TOT_H HEIGHT - TOP_H
#define CELL_X 0 
#define CELL_Y TOP_H


#define PARENT_MARGIN_TOP 0.1
#define PARENT_MARGIN_BOT 0.1

#define PARENT_MARGIN_LEFT 0.1
#define PARENT_MARGIN_RIGHT 0.1

#define PARENT_MARGIN_VER (PARENT_MARGIN_TOP + PARENT_MARGIN_BOT)
#define PARENT_MARGIN_HOR (PARENT_MARGIN_LEFT + PARENT_MARGIN_RIGHT)


#define CELLS_W 4
#define CELLS_H 3

typedef struct _box {
	int x0,y0;
	int w,h;
}box;

extern char *symbol_file; 
typedef enum  {NONE,LOAD,DUMP} smode_t;
extern smode_t symbol_mode;


/*  UI */

extern ui_state_t ui_state;
