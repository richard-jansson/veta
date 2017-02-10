#include "color.h"

// Default values 
#define CELLS_W 1
#define CELLS_H 1

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
