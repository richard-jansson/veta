#include<stdlib.h>

#include "veta.h"

#include "debug.h"
#include "cell.h"
#include "keyboard_io.h"
#include "ui.h"
#include "conf.h"

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
			break;
		case SELECT_CELL:
			uk_log("select cell %i\n",event->cell);
			break;
		case UNDEFINED:
			uk_log("got event UNDEFINED\n"); 
			break;
	}

}

void veta_symbolsloaded(symbol *symbols,int n){
	uk_log("Keymap is loaded! %i keys",n);
	root=create_cells(symbols,n,CELL_SIZE,TREE_DEPTH,1);
}

int main(){
	atexit(veta_exit);
	debug_init(LOG_FILE);

	state *st=readstate(STATE_FILE);

	// Set up callbacks 
	ui_onevent(veta_handleevent);
	ui_haskeymap(veta_symbolsloaded);

	ui_init(WIDTH,HEIGHT,st->x,st->y);

	ui_loop();
	return 1;
}
