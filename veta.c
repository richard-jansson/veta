#include<stdlib.h>

#include "debug.h"
#include "ui.h"

void veta_exit(){
	debug_exit();
}

int main(){
	atexit(veta_exit);
	debug_init("veta.log");
	ui_init();

	ui_loop();
	return 1;
}
