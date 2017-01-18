/*
 *	Copyright 2016 
 *	
 *	This file is part of Veta.
 *	
 *	Veta is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	
 *	Veta is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with Veta.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ver.h"
/* FIXME: Before including veta.h you have to include cell.h not sure if this makes sense */
#include "cell.h"
#include "ui2.h"

#define SCALE 1

#define ASPECT_HOR 800
#define ASPECT_VER 600

#define FONT_SIZE  48* SCALE

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
typedef enum  {NONE,LOAD,DUMP} sym_mode_t;
typedef enum  {ZOOM,HIGHLIGHT} sel_mode_t;

extern sym_mode_t symbol_mode;


/*  UI */
typedef enum {BACKSPACE,ENTER,ANY} vkey; 

void draw_text_box(char *txt,int w,int h,int x,int y,rgb fg,rgb bg);
void draw_box(int w,int h,int x,int y,int r,int g,int b);
//extern ui_state_t ui_state;
void ui_onclick(void(*callback)(int x,int y));
void ui_onrelease(void(*callback)(char *s,int *p,vkey key,void *data));

/* UI lvl 2 */ 
void render_ui2();
void ui2_handle_click(int x,int y);
void ui2_handle_release(char *s,int *propagate,vkey key,void *platform_specific);
void ui2_init();
void ui_onrelase(void(*callback)(char *s,int *p));
int ui2_onselect_symbol(symbol *sym);
void ui2_add_widgets();

/* VETA */ 
void veta_render();

/* Conf  */
void writestate(char *path,int x,int y);

/* getmodifiers */
int get_modifiers(int keycode);
