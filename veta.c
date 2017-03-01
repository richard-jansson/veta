/*
 *	Copyright (C) 2016 Richard Jansson 
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
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>

#include "veta.h"
#include "debug.h"
#include "keyboard_io.h"
#include "ui.h"
#include "conf.h"
#include "render.h"

#include "jsonconf.h"

#define CONFIG_PATH "conf.json"

cell *root_cell=NULL;

/* Configuration options */
char *symbol_file=NULL; 
sym_mode_t symbol_mode;
sel_mode_t selection_mode=ZOOM;
/* end of configuration options */

void usage(char *cmd){
	printf("Usage: %s \n",cmd);	
	printf("\t--dump-symbols [file]\t dump symbols to file\n");
	printf("\t--load-symbols [file]\t load symbols from  file\n");
	printf("\t--zoom\t Select cells by zooming\n");
	printf("\t--highlight\t Select cells by highlighting\n");
	exit(1);
}

void veta_exit(){
	uk_log("veta exit!");

	conf_save_position(lastX,lastY);

	debug_exit();
}

void veta_handleevent(event_t *event){
	switch(event->type){
		case QUIT:
			uk_log("Quit");
			ui_quit();
			break;
		case RESET:
			uk_log("Reset");
			clear_selection(root_cell);
			veta_render();
			break;
		case SELECT_CELL:
			uk_log("select cell %i\n",event->cell);
			select_cell(root_cell,event->cell);
			symbol *sym;
			if(NULL!=(sym=get_selected_symbol(root_cell))){
				clear_selection(root_cell);
				if(ui2_onselect_symbol(sym)) break;
				sendkey(sym->data,1,0);
			}
			veta_render();
			break;
		case UNDEFINED:
			uk_log("got event UNDEFINED\n"); 
			break;
	}
}

void veta_symbolsloaded(symbol *symbols,int n){
	uk_log("Symbols are loaded got %i symbols",n);

	conf_save_symbols("platform_symbols.json",symbols,n);

	int cellsize=conf_get_int("n_columns",CELLS_W)*conf_get_int("n_rows",CELLS_H);

	root_cell=create_cells(symbols,n,cellsize,TREE_DEPTH,1);
	clear_selection(root_cell);
}

int render_cell(cell *cell,void *data){
	assert(cell != NULL);
	assert(data != NULL);
	box *ob=(box *)data;
	box nb;
	assert(cell);

	if(cell->level>1){
		if(cell->symbol->name){
			rgb fg={255,255,255};
			rgb bg={0,0,0};
			// FIXME this should handle an arbitrary level
			int is_selected=cell->selected || (cell->parent && cell->parent->selected);
			if(selection_mode==HIGHLIGHT) bg=is_selected?cell->color_selected:cell->color;
			else if(ui2_is_selected(cell->symbol)) bg=cell->color_selected;
			else bg=cell->color;

			draw_text_box(cell->symbol->name,ob->w,ob->h,ob->x0,ob->y0,fg,bg);
		}else{
		}
	} else {
//		draw_box(WIDTH,HEIGHT,0,0,0,0,0);
	}
		
	int n_cols=conf_get_int("n_columns",CELLS_W);
	int n_rows=conf_get_int("n_rows",CELLS_H);


	int innerWidth=(ob->w/n_cols)*0.9;
	int innerHeight=(ob->h/n_rows)*0.9;

	nb.w=(ob->w/n_cols)*0.9;
	nb.h=(ob->h/n_rows)*0.9;
	int pleft=0.1*(ob->w/n_cols);
	int ptop=0.1*(ob->h/n_rows);

	for(int i=0;i<cell->nchildren ;i++){
		nb.x0=nb.w*(i%n_cols)+ob->x0+pleft;	
		nb.y0=nb.h*(i/n_cols)+ob->y0+ptop;
		
		render_cell(cell->children[i],&nb);
	}
	return 0;
}

void veta_render(){
	box b;
	b.x0=b.y0=0;
	b.w=WIDTH;
	b.h=HEIGHT;

	draw_box(WIDTH,HEIGHT,0,0,0,0,0);

	if(selection_mode==ZOOM){
		cell *deepest=get_deepest_cell(root_cell);
		render_cell(deepest,&b);
	}else{
		render_cell(root_cell,&b);
	}
	
	render_ui2();
#ifdef DEBUG
	rgb black=(rgb){0,0,0};
	rgb white=(rgb){0xff,0xff,0xff};
	draw_text_box(BUILD,650,40,20,HEIGHT-45,black,black);
#endif
}

void veta_click(int x,int y){
}

int main(int argc,char *argv[]){
	debug_init(LOG_FILE);
	
	conf_init(CONFIG_PATH,veta_symbolsloaded);

	uk_log("build: %s",BUILD);
	int full_throttle=0;

	symbol_mode=LOAD;
	symbol_file=SYMBOLS_FILE;

	for(int i=1;i<argc;i++){
		if(!strcmp("--dump-symbols",argv[i])){
			symbol_mode=DUMP;	
			i++;
			if(i>=argc) usage(argv[0]);
			symbol_file=argv[i];
			uk_log("Dumping to %s\n",symbol_file);
		}else if(!strcmp("--load-symbols",argv[i])){
			symbol_mode=LOAD;	
			i++;
			if(i>=argc) usage(argv[0]);
			symbol_file=argv[i];
		}else if(!strcmp("--zoom",argv[i])){
			selection_mode=ZOOM;
		}else if(!strcmp("--highlight",argv[i])){
			selection_mode=HIGHLIGHT;
		}else if(!strcmp("--graphicsopt",argv[i])){
			full_throttle=1;
		}else{
			usage(argv[0]);
		}
	}

	// Set up callbacks 
	ui_onevent(veta_handleevent);
	ui_onclick(ui2_handle_click);
	ui_onrelease(ui2_handle_release);

	ui_haskeymap(veta_symbolsloaded);
	ui_render(veta_render);

	int x,y;
	conf_read_position(&x,&y);

	ui_init(conf_get_int("width",WIDTH),conf_get_int("height",HEIGHT),x,y);

	ui2_init();

	ui2_add_widgets();

	ui_loop(full_throttle);

	return 1;
}
