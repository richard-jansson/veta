#include<stdlib.h>
#include<assert.h>
#include<math.h>

#include"debug.h"
#include"cell.h"

//key *currentkey,*selected_key;
symbol *current_symbol,*selected_symbol;
cell *deepest_selected_cell=NULL;

int prev_lvl_selected=-1;
int max_symbols=0,symbols_added=0;

int cell_id = 0;

cell *create_recurse(int lvl,int cellsizen,int depth,int childn,cell *parent){
	int cellsize=CELLS_W*CELLS_H;
	cell *curr=NULL;
	curr=malloc(sizeof(cell));
	curr->children=malloc(sizeof(cell*)*cellsize);
	curr->nchildren=0;
	curr->symbol=NULL;
	curr->level=lvl;
	curr->selected=0;
	curr->depth=depth;
	curr->cellsize=cellsize;
	curr->parent=parent;

	if(lvl==depth){
		// TODO this isn't really right
		if(symbols_added == max_symbols){
			if(curr && curr->children) free(curr->children);
			free(curr);
			return NULL; 
		}
		curr->symbol=current_symbol;
		curr->n=childn;
		current_symbol++;	
		symbols_added++;
		return curr;
	}

	for(int i=0;i<cellsize;i++){
		curr->children[i]=create_recurse(lvl+1,cellsize,depth,i,curr);
		if(curr->children[i]) curr->nchildren++;
	}
	return curr;
}

int _print_cell(cell *cell,void *data){
	symbol *sym=cell->symbol;
	if(sym!=NULL) uk_log("{name=%s mode=%i toggled=%i}",sym->name,sym->mode,sym->toggled);
	return 0;
}

void print_structure(cell *root){
	recurse_cells(root,_print_cell,NULL);
}

int _print_color(cell *cell,void *data){
	print_rgb(cell->color);
	return 0;
}

int _colorize(cell *cell,void *data){
	int *cell_n=(int*)data;
	
	cell->color=get_color(*cell_n,0);
	cell->color_selected=get_color(*cell_n,1);

	(*cell_n)++;
	return 0;
}

int count_cells(cell *cell, void *data){
	int *n_cells=(int*)data;
	(*n_cells)++;
	return 0;
}

void colorize(cell *root){
	int n_cells=0;

	recurse_cells(root,count_cells,(void*)&n_cells);

	uk_log("Counted cells and got %i",n_cells);
	init_color(n_cells);
	int n=0;
	recurse_cells(root,_colorize,(void*)&n);
}

cell *create_cells(symbol *symbols,int n_symbols,int cellsize,int  depth,int first){
//	int depth = (int)ceil(log(n_symbols)/log(cellsize));
	cell *root;
	if(first) current_symbol=symbols;
	max_symbols=n_symbols;

	root=create_recurse(0,cellsize,depth,0,NULL);

	colorize(root);

	return root;
}

void _recurse_cells(cell *curr,int (*callback)(cell *cell, void *data),void *data){
	if(callback(curr,data)) return;
	for(int i=0;i<curr->nchildren;i++){
		_recurse_cells(curr->children[i],callback,data);
	}
}

void recurse_cells(cell *curr,int (*callback)(cell *cell,void *data),void *data){
	_recurse_cells(curr,callback,data);
}

// Perhaps this should use the generalized recurse cell function above
void destroy_cells(cell *curr){
	for(int i=0;i<curr->nchildren;i++){
		destroy_cells(curr->children[i]);		
	}
	free(curr->children);
	free(curr);
	
	destroy_color();
}

int _select_cell(cell *curr,void *data){
	if(curr->selected && curr->level > prev_lvl_selected){
		deepest_selected_cell=curr;		
		prev_lvl_selected=curr->level;
	}
	return 0;
}

int _clear_selection(cell *curr,void *data){
	curr->selected=0;
	return 0;
}

int select_cell(cell *root, int cell){
	if(cell<0 || cell >= root->nchildren) return 1;

	recurse_cells(root,_select_cell,NULL);
	
	if(!deepest_selected_cell){
		deepest_selected_cell=root;
		root->selected=1;
		prev_lvl_selected=0;
	}

	if(deepest_selected_cell->nchildren){
		deepest_selected_cell->children[cell]->selected=1;
	}

	return 0;
}

int _get_selected_symbol(cell *curr,void *data){
	if(curr->selected && curr->symbol){
		selected_symbol=curr->symbol;
		return 1;
	}
	return 0;
}

symbol *get_selected_symbol(cell *root){
	selected_symbol=NULL;
	recurse_cells(root,_get_selected_symbol,NULL);
	return selected_symbol;
}

void clear_selection(cell *root){
	recurse_cells(root,_clear_selection,NULL);
	deepest_selected_cell=NULL;
	prev_lvl_selected=-1;
}


// I've omitted to make this work at an arbitrary level. According to my mental specification.
cell *get_deepest_cell(cell *root){
	cell *deepest=root;
	for(int i=0;i<root->nchildren;i++){
		if(root->children[i] && root->children[i]->selected) deepest=root->children[i];
	}
	return deepest;
}
