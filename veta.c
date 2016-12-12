#ifdef WINDOWS 
#include<windows.h>
#include"winapi.h"
#endif 

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

cell *root;

/* Configuration options */
char *symbol_file=NULL; 
sym_mode_t symbol_mode;

#ifdef WINDOWS
sel_mode_t selection_mode=ZOOM;
//sel_mode_t selection_mode=HIGHLIGHT;
#else
sel_mode_t selection_mode=ZOOM;
#endif
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
//			ui_state=HUD_OFFLINE;
//			veta_render();
			refresh();
			break;
		case SELECT_CELL:
			uk_log("select cell %i",event->cell);
//			debug_print_tree(root);
			select_cell(root,event->cell);
			symbol *sym;
			if(NULL!=(sym=get_selected_symbol(root))){
				clear_selection(root);
				if(ui2_onselect_symbol(sym)) break;
				sendkey(sym->data,1,0);
			}
			refresh();
			break;
		case UNDEFINED:
			uk_log("got event UNDEFINED\n"); 
			break;
		default:
			uk_log("WARNING undefined event type");
			break;
	}
}

void veta_symbolsloaded(symbol *symbols,int n){
	uk_log("Keymap is loaded! %i keys",n);
	root=create_cells(symbols,n,CELL_SIZE,TREE_DEPTH,1);
	clear_selection(root);
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
			else bg=cell->color;

			draw_text_box(cell->symbol->name,ob->w,ob->h,ob->x0,ob->y0,fg,bg);
		}else{
		}
	} else {
//		draw_box(WIDTH,HEIGHT,0,0,0,0,0);
	}
		
	int innerWidth=(ob->w/CELLS_W)*0.9;
	int innerHeight=(ob->h/CELLS_H)*0.9;

	nb.w=(ob->w/CELLS_W)*0.9;
	nb.h=(ob->h/CELLS_H)*0.9;
	int pleft=0.1*(ob->w/CELLS_W);
	int ptop=0.1*(ob->h/CELLS_H);

	for(int i=0;i<cell->nchildren;i++){
		nb.x0=nb.w*(i%CELLS_W)+ob->x0+pleft;	
		nb.y0=nb.h*(i/CELLS_W)+ob->y0+ptop;
		
		render_cell(cell->children[i],&nb);
	}
	return 0;
}

void veta_render(){
	uk_log("veta_render");
	box b;
	b.x0=b.y0=0;
	b.w=WIDTH;
	b.h=HEIGHT;

	draw_box(WIDTH,HEIGHT,0,0,0,0,0);

	if(selection_mode==ZOOM){
		cell *deepest=get_deepest_cell(root);
		render_cell(deepest,&b);
	}else{
		render_cell(root,&b);
	}
	
	render_ui2();
#ifdef DEBUG
	uk_log("debug!");
	rgb black=(rgb){0,0,0};
	rgb white=(rgb){0xff,0xff,0xff};
	draw_text_box(BUILD,650,40,20,HEIGHT-45,white,black);
#endif
	uk_log("render done!");
}

void veta_click(int x,int y){
}

#ifdef WINDOWS 
int WINAPI WinMain(HINSTANCE h,HINSTANCE hprev,LPSTR cmdline,int n){
	debug_init(LOG_FILE);
	uk_log("windows build: %s",BUILD);

	int full_throttle=0;
	nCmdShow=n;

	hinstance=h;
	symbol_mode=LOAD;

	// FIXME temporary debug shit

#else
int main(int argc,char *argv[]){
	debug_init(LOG_FILE);
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
#endif

	state *st=readstate(STATE_FILE);

	// Set up callbacks 
	ui_onevent(veta_handleevent);
	ui_onclick(ui2_handle_click);
	ui_onrelease(ui2_handle_release);

	ui_haskeymap(veta_symbolsloaded);
	ui_render(veta_render);


// Set up low level graphics and load the keymap
	ui_init(WIDTH,HEIGHT,st->x,st->y);

	ui2_init();

	ui2_add_widgets();

	ui_loop(full_throttle);
	return 1;
}
