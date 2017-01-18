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
#include "color.h"

#define CELLS_W 4
#define CELLS_H 3

#define TREE_DEPTH 2

// FIXME depth hard coded to two
#define CELLS_N (CELLS_W*CELLS_H*CELLS_W*CELLS_H)

typedef enum {NORMAL,TOGGLE} mode;

typedef struct _symbol{
	char *name;
	void *data; // Platform specific data 
	mode mode;
	int toggled;
} symbol;

typedef struct _cell{
	int level;
	int nchildren;
	int selected;
	int n;
	int depth; 
	int cellsize;
	struct _cell **children;
	struct _cell *parent;
	rgb color,color_selected;
	symbol *symbol;
}cell;

cell *create_cells(symbol *symbols,int n_symbols,int cellsize,int depth,int first);
// If callback returns no-zero the iteration will stop
void recurse_cells(cell *curr,int (*callback)(cell *cell,void *data),void *data);
void destroy_cells(cell *curr);
int select_cell(cell *curr, int cell);
void clear_selection(cell *root);
symbol *get_selected_symbol(cell *root);

cell *get_deepest_cell(cell *root);
