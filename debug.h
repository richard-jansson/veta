#include <stdarg.h>

void _uk_log(const char *file,int line,const char *fmt,...);
void debug_init(char *path);
void debug_exit();

#define uk_log(fmt,...) \
	_uk_log(__FILE__,__LINE__,(const char *)fmt,##__VA_ARGS__)
