#include<jansson.h>
#include<string.h>

#include "debug.h"
#include "cell.h"
#include "jsonconf.h"

// Platform specific
#include "keyboard_io.h"

symbol *symbols;
int has_symbols=0,n_symbols=0,n;
json_t *conf_root,*json_symbols;

/* 
 * Iterate over all symbols that can be sent. 
 *
 * This function should be made to work on other attributes as well. 
 */
void _iterate_json_symbol_pairs(json_t *conf_root,void(*callback)(char *,json_t *)){
	size_t index,i2,i3;
	const char *key,*u;
	json_t *value,*v2,*v3,*v;

	json_object_foreach(conf_root,key,value) {
		if(json_is_array(value) && !strncmp("symbols",key,7)) {
			int i=0;
			has_symbols=1;
			json_array_foreach(value,i2,v2){
				if(json_is_object(v2)){
					json_object_foreach(v2,u,v){
						callback(u,v);
					}
				}
			}
		}
	}
}

void _countkeys(char *s,json_t *w){
	n_symbols++;
}
void _addkeys(char * s,json_t *w){
	// memory leak for sure!
//	symbols[n]=(symbol *)malloc(sizeof(symbol));
	symbols[n].name=malloc(strlen(s)+1);

	symbol_x11 *data=(symbol_x11 *)malloc(sizeof(symbol_x11));
	data->keycode=json_integer_value(w);
	symbols[n].data=(void*)data;

	strncpy(symbols[n].name,s,strlen(s)+1);


	n++;
}

void conf_init(const char *config_file,void (*onhaskeymap)(symbol *,int n)){
	json_t *json;
	json_error_t error;

	conf_root=json_load_file(config_file,0,&error);
	if(!conf_root){
		uk_log("Failed to open conf.json: %s",error.text);
		conf_root=json_object();
	}

	if(!json_is_object(conf_root)){
		uk_log("root node is not an object\n");
		exit(1);
	}
	
	_iterate_json_symbol_pairs(conf_root,_countkeys);
	symbols=malloc(sizeof(symbol)*n_symbols);
	_iterate_json_symbol_pairs(conf_root,_addkeys);

	for(int i=0;i<n_symbols;i++){
		uk_log("L: %s",symbols[i].name);
	}
		
	onhaskeymap(symbols,n_symbols);
}

float _conf_get_float(json_t *root, char *key,float def){
	json_t *real;
	real=json_object_get(root,key);
	if(!json_is_real(real)){
		uk_log("configuration error %s is supposed to be an real",key);
		return def;
	}
	if(!real) return def;
	return json_real_value(real);
}
float conf_get_float(char *key,float def){
	return _conf_get_float(conf_root,key,def);
}
int _conf_get_int(json_t *root, char *key,int def){
	json_t *integer;
//	uk_log("looking for key %s",key);
	integer=json_object_get(root,key);
	if(!json_is_integer(integer)){
		uk_log("configuration error %s is supposed to be an integer",key);
		return def;
	}
	if(!integer) return def;
	return json_integer_value(integer);
}
int conf_get_keybinding(char *event,int def){	
	const char *key,*u;
	json_t *value,*v2,*v3,*v;
	json_object_foreach(conf_root,key,value) {
		if(json_is_object(value) && !strncmp("keybindings",key,11)) {
			int t=_conf_get_int(value,event,def);
			uk_log("%s = %i",event,t);
			return t;
		}
	}
	return def;
}

int conf_get_int(char *key,int def){
	return _conf_get_int(conf_root,key,def);
}

char *conf_get_string(char *key,char *def){
	json_t *string;
	string=json_object_get(conf_root,key);
	if(!json_is_string(string)){
		uk_log("configuration error %s is supposed to be a string",key);
		return def;
	}
	if(!string) return def;
	return json_string_value(string);
}

void conf_read_position(int *x,int *y){
	json_t *pos,*jx,*jy;
	pos=json_object_get(conf_root,"position");
	if(!pos) {
		*x=0;
		*y=0;
		return;
	}
	jx=json_object_get(pos,"x");
	jy=json_object_get(pos,"y");

	*x=json_integer_value(jx);
	*y=json_integer_value(jy);
}

void conf_save_position(int x,int y){
	json_t *position,*jx,*jy;

	position=json_object();

	jx=json_integer(x);
	jy=json_integer(y);

	json_object_set_new(position,"x",jx);
	json_object_set_new(position,"y",jy);

	json_object_set_new(conf_root,"position",position);
}

// Memory leak ahead! 
void conf_save_symbols(const char *path,symbol *unique,int n){
	if(has_symbols) return;

	json_t *sym, *keycode;

	json_symbols=json_array();

	for(int i=0;i<n;i++){
		symbol_x11 *pspecific;
		pspecific=unique[i].data;

		sym=json_object();
		keycode=json_integer(pspecific->keycode);

		json_object_set_new(sym,unique[i].name,keycode);

		json_array_append_new(json_symbols,sym);
	}
	json_object_set_new(conf_root,"symbols",json_symbols);

}

void conf_save(const char *path){
	uk_log("saving config file to %s",path);

	if(json_dump_file(conf_root,path,JSON_INDENT(0))==-1){
		uk_log("json dump error!");
	}
}

