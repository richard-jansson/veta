#include<jansson.h>

#include "debug.h"
#include "cell.h"
#include "jsonconf.h"

// Platform specific
#include "keyboard_io.h"

int has_symbols=0;
json_t *conf_root,*json_symbols;

void conf_init(const char *config_file){
	json_t *json;
	json_error_t error;

	conf_root=json_load_file(config_file,0,&error);
	if(!conf_root){
		printf("couldn't open %s\n",config_file);
		uk_log("Failed to open conf.json: %s",error.text);
		conf_root=json_object();
	}

	if(!json_is_object(conf_root)){
		uk_log("root node is not an object\n");
		exit(1);
	}
	
	size_t index,i2,i3;
	const char *key;
	json_t *value,*v2,*v3;

	// Stunningly beautiful use of the preprocessor
	json_object_foreach(conf_root,key,value) {
		uk_log("Reading got key %s",key);
		if(json_is_array(value) && !strncmp("symbols",key,7)) {
			int i=0;
			uk_log("got symbols");
			has_symbols=1;
			json_array_foreach(value,i2,v2){
				if(json_is_string(v2)){
//					printf("%s ",json_string_value(v2));
					if(!((i+1)%4)) printf("\t");
					if(!(++i%16)) printf("\n");
				}
			}
		}
	}
	printf("\n");
}


int conf_get_int(char *key,int def){
	json_t *integer;
	integer=json_object_get(conf_root,key);
	if(!json_is_integer(integer)){
		uk_log("configuration error %s is supposed to be an integer",key);
		return def;
	}
	if(!integer) return def;
	return json_integer_value(integer);
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

//		printf("%s:%i\n",unique[i].name,pspecific->keycode);
	}
	printf("set symbols on root");
	json_object_set_new(conf_root,"symbols",json_symbols);

}

void conf_save(const char *path){
	uk_log("saving config file to %s",path);

	if(json_dump_file(conf_root,path,JSON_INDENT(0))==-1){
		uk_log("json dump error!");
	}
}
