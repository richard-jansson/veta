void conf_save_symbols(const char *path,symbol *unique,int n);
void conf_read_position(int *x,int *y);
void conf_save_position(int x,int y);
void conf_init(const char *config_file,void (*onhaskeymap)(symbol *,int n));
void conf_save(const char *path);
int conf_get_int(char *key,int def);
float conf_get_float(char *key,float def);
char *conf_get_string(char *key,char *def);
