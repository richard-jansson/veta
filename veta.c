#include<stdlib.h>

#include "veta.h"

#include "debug.h"
#include "ui.h"
#include "conf.h"

void veta_exit(){
	uk_log("veta exit!");
	writestate(STATE_FILE,lastX,lastY);
	debug_exit();
}

int main(){
	atexit(veta_exit);
	debug_init(LOG_FILE);

	state *st=readstate(STATE_FILE);
	ui_init(WIDTH,HEIGHT,st->x,st->y);

	ui_loop();
	return 1;
}
