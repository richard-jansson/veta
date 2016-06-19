#include<stdlib.h>

#include "veta.h"

#include "debug.h"
#include "cell.h"
#include "keyboard_io.h"
#include "ui.h"
#include "conf.h"

#include "render.h"

cell *root;

void veta_exit(){
	uk_log("veta exit!");
	writestate(STATE_FILE,lastX,lastY);
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
			clear_selection(root);
			veta_render();
			break;
		case SELECT_CELL:
			uk_log("select cell %i\n",event->cell);
			select_cell(root,event->cell);
			symbol *sym;
			if(NULL!=(sym=get_selected_symbol(root))){
				sendkey(sym->data,1,0);
//				uk_log("send symbol (%s)",symbol->name);
				clear_selection(root);
			}
			veta_render();
			break;
		case UNDEFINED:
			uk_log("got event UNDEFINED\n"); 
			break;
	}
}

void veta_symbolsloaded(symbol *symbols,int n){
	uk_log("Keymap is loaded! %i keys",n);
	root=create_cells(symbols,n,CELL_SIZE,TREE_DEPTH,1);
	clear_selection(root);
}

int render_cell(cell *cell,void *data){
	box *ob=(box *)data;
	box nb;
//	uk_log("Cell: n=%i children=%i level=%i\n",cell->nchildren,cell->level);
//	uk_log("render cell level=%i!",cell->level);
//	uk_log("render at [%i,%i,%i,%i]",b->x0,b->y0,b->w,b->h);
	int r=random()%255;
	int g=random()%255;
	int b=random()%255;
	if(cell->level>1){
		if(cell->symbol->name){
			rgb fg={255,255,255};
			rgb bg={r,g,b};
			// FIXME this should handle an arbitrary level
			int is_selected=cell->selected || (cell->parent && cell->parent->selected);
			bg=is_selected?cell->color_selected:cell->color;
//			bg=cell->selected?cell->color_selected:(rgb){0,0,255};
//			uk_log("%i %i %i",bg.r,bg.g,bg.b);

			draw_text_box(cell->symbol->name,ob->w,ob->h,ob->x0,ob->y0,fg,bg);
		}else{
			draw_box(ob->w,ob->h,ob->x0,ob->y0,r,g,b);
		}
//		uk_log("[%i,%i,%i,%i]\n",ob->w,ob->h,ob->x0,ob->y0);
	} else {
//		draw_box(WIDTH,HEIGHT,0,0,0,0,0);
	}

	nb.w=ob->w/CELLS_W;
	nb.h=ob->h/CELLS_H;

	for(int i=0;i<cell->nchildren;i++){
		nb.x0=nb.w*(i%CELLS_W)+ob->x0;	
		nb.y0=nb.h*(i/CELLS_W)+ob->y0;
//		uk_log("%i,%i",nb.x0,nb.y0);
		
		render_cell(cell->children[i],&nb);
	}
	return 0;
}


void veta_render(){
	box b;
	b.x0=b.y0=0;
	b.w=WIDTH;
	b.h=HEIGHT;
//	recurse_cells(root,render_cell,&b);
	draw_box(WIDTH,HEIGHT,0,0,0,0,0);
	render_cell(root,&b);
}

int main(){
	atexit(veta_exit);
	debug_init(LOG_FILE);

	state *st=readstate(STATE_FILE);

	// Set up callbacks 
	ui_onevent(veta_handleevent);
	ui_haskeymap(veta_symbolsloaded);
	ui_render(veta_render);

//	set_draw_text_box(r_render_cell);

	ui_init(WIDTH,HEIGHT,st->x,st->y);

	ui_loop();
	return 1;
}
