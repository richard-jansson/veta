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
/* This is platform specific */ 

#ifdef X11
#include<X11/Xlib.h>

typedef struct _modifier_x11 {
	int modifier;
	KeyCode kc;
	KeySym ksym;
}modifier_x11;

// TODO: perhaps we should remove this extra complexity. Since I suppose that on all systems the data could be decoded into a number / keycode 
typedef struct _symbol_x11 {
// TODO: remove ks, since it's only used when printing log msg
	KeySym ks;
	KeyCode keycode;
	int modifier;
	int is_modifier;
}symbol_x11;


int is_modifier(KeyCode keycode);
KeyCode get_modifier_keycode(int modifier);
KeySym get_modifier_keysym(int modifier);
#endif 

#ifdef WINDOWS
typedef struct _symbol_win {
	char c;
}symbol_win;

// FIXME: implement modifier win
typedef struct _modifier_win {
	int a;
}modifier_win;
#endif

typedef struct _keyboardio{
//	SDL_mutex *mutex;
	int start,stop,n;
	int keycodes[128];
//	Display *dpy;
//	SDL_Thread *thread;
	int lastsent;
} keyboardio;

typedef enum {UNDEFINED,SELECT_CELL,RESET,QUIT,SWITCH_TREE} eventtype;

typedef struct _event{
	int keycode;
	eventtype type;
	int cell;
	int modifiers;
} event_t;

keyboardio * init_keyboardio();
int get_event(event_t *event);
void sendkey(void *sym,int press_and_release,int toggled);


char *get_select_cell_name(int n);
