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
/* This is the logic for the ui. So ui is library / helper functions for the 
  graphical user interface. And this would be what acctually does things. */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "veta.h"

#include "keyboard_io.h"
#include "ui.h"

#include "debug.h"

// Our state 
int selectcell=0;
symbol *selected;
char *label=NULL;

// Our widgets
widget w_configure;
widget w_select;
widget w_description,w_binding,w_done;
widget w_label; // Edit  label 
widget w_map; //  Edit mapping


cell *curr;

char *string_clear(char *s){
	if(s) free(s);
	s=malloc(1);
	*s='\0';
	return s;
}

char *string_append(char *s,char *appendix){
	int l=strlen(s)+strlen(appendix);

	s=realloc(s,l);
	strcat(s,appendix);
	
	return  s;
}

char *string_backspace(char *s){
	int l=strlen(s);
	if(l==0) return s;
	s[l-1]='\0';
	return s;
}

void done_click(widget_t *this){
	widget_set_visible(w_configure,1);

	widget_set_visible(w_description,0);
	widget_set_visible(w_binding,0);
	widget_set_visible(w_done,0);
	
	selected=NULL;
}

// Click on configure -> Go to select cell
void configure_click(widget_t *this){
	widget_set_visible(w_configure,0);

	widget_set_visible(w_select,1);
	
	selectcell=1;
}

void desc_input(widget_t *this){
	uk_log("Changing description");
}

// If returns true do not propagate signal
// select cell
// when we have a selected cell go to -> {binding,description}
int ui2_onselect_symbol(symbol *sym){
	if(selectcell){
		uk_log("%s selected",sym->name); 
		selectcell=0;
		selected=sym;
		widget_set_visible(w_select,0);

		widget_set_visible(w_description,1);
		widget_set_visible(w_binding,1);
		widget_set_visible(w_done,1);
		render_ui2();
		return  1;
	}
	return 0;
}

// Go to edit label
void desc_click(widget *this){
	uk_log("description was clicked");

	widget_set_visible(w_description,0);
	widget_set_visible(w_binding,0);

	widget_set_visible(w_label,1);

// move name to label, that we work on
	if(selected->name){
		if(label) free(label);
		label=malloc(strlen(selected->name));
		*label='\0';
		strcat(label,selected->name);
	}
//	label=selected->name;

	grabkeyboard();
}

// Go to map
void binding_click(widget *this){
	uk_log("binding was clicked");

	widget_set_visible(w_description,0);
	widget_set_visible(w_binding,0);

	widget_set_visible(w_map,1);
	grabkeyboard();
}

// Do you worry about feature creep, me  I don't!
// TODO: make it so that we can use strings here 
// TODO: make here so that we can call system("whatever %s"); based on this
// TODO: make it so that there is generic output stuff
void map_onrelease(widget_t *this,char *s,int *propagate,vkey  key,void *pspecific){
	if(!this->visible){
		*propagate=1;
		return;
	}
	uk_log("REMAPPING  %s\n",selected->name); 
	log_platformspecific(selected->data);
	log_platformspecific(pspecific);

	selected->data=pspecific;


	this->visible=0;

	widget_set_visible(w_description,1);	
	widget_set_visible(w_binding,1);
	widget_set_visible(w_done,1);
	*propagate=0;

	ungrabkeyboard();
}

void label_onrelease(widget_t *this,char *s,int *propagate,vkey  key,void *pspecific){
	if( key  == BACKSPACE) {
		label=string_backspace(label);
		return;	
	}
// To get enter or escape or whatever
	if(!isprint(*s) || key == ENTER) {
		ungrabkeyboard();

// Back to S. III
		widget_set_visible(w_label,0);

		widget_set_visible(w_description,1);
		widget_set_visible(w_binding,1);

// I hate C from the bottom of my heart, the subtle bugs created by this 
// ancient beast creeps in in early stages of development. Just like a 
// a Succubus it leads you to believe you've done something clever and nice,
// while you in fact sawed your fucking leg of.
		if(selected->name) {
			free(selected->name);
		}
		selected->name=malloc(strlen(selected->name)+1);
		selected->name[0]='\0';
		strcat(selected->name,label);

		render_ui2();
		return;
	}

	label=string_append(label,s);

	render_ui2();
	
// Stop the signal from propagating, we're eating this!
	*propagate=0;
}
/*
 onclick  				I -> II
 onselectcell			II -> III
 onclick					III -> {IV,V,VI}
 onenter					IV -> III
 onbinding				V		-> III
*/

void ui2_add_widgets(){
	label=string_clear(label);
// I 
	w_configure=add_widget("Configure",NULL,configure_click,NULL);

// II 
	w_select=add_widget("Select cell",NULL,NULL,NULL);

// III 
//	w_cellname=add_widget("N/A",NULL,NULL,NULL);
	w_description=add_widget("Description",NULL,desc_click,NULL);
	w_binding=add_widget("Binding",NULL,binding_click,NULL);
	w_done=add_widget("Done",NULL,done_click,NULL);

// IV 
	w_label=add_widget(label,text_draw,NULL,label_onrelease);

// V 
	w_map=add_widget("MAP",text_draw,NULL,map_onrelease);

	widget_set_visible(w_configure,1);
}
