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
// Public functions
void ui_init(int w,int h,int x,int y);
void ui_loop();

// Public variables 
extern int lastX,lastY;

// Private functions
//void set_on_top(Display *dpy,Window window);


void ui_onevent(void(*onevent)(event_t *ev));
void ui_haskeymap(void(*has_keymap)(symbol *keymap,int n));
void ui_quit();
void ui_render();
void refresh();
void log_platformspecific(void  *data);

void widget_set_visible(widget w,int v);
void grabkeyboard();
void ungrabkeyboard();
widget add_widget(char *label,
	void(*draw)(widget_t *this,int x,int y,int w,int h),
	void(*onclick)(widget_t *w),
	void(*onrelease)(widget_t *w,char *s,int *p,vkey key,void *pspecific));
