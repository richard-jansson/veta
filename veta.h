#define WIDTH 640
#define HEIGHT 480

#define MOD_MAX 1

#define STATE_FILE "veta.state"
#define LOG_FILE "veta.log"

#define CELL_SIZE 12 
#define TREE_DEPTH 2

#define TOP_W WIDTH

#define CELL_TOT_W WIDTH
#define CELL_TOT_H HEIGHT - TOP_H
#define CELL_X 0 
#define CELL_Y TOP_H

#define FONT_SIZE 16

#define PARENT_MARGIN_TOP 0.15
#define PARENT_MARGIN_BOT 0.05

#define PARENT_MARGIN_LEFT 0.08
#define PARENT_MARGIN_RIGHT 0.03

#define PARENT_MARGIN_VER (PARENT_MARGIN_TOP + PARENT_MARGIN_BOT)
#define PARENT_MARGIN_HOR (PARENT_MARGIN_LEFT + PARENT_MARGIN_RIGHT)
#define TOP_H 0.01*HEIGHT

#define CELLS_W 4
#define CELLS_H 3

typedef struct _box {
	int x0,y0;
	int w,h;
}box;
