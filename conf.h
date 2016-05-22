#include<stdint.h>

#define MAGIC 0x45564f4c
typedef struct _state{
	uint32_t magic;
	int32_t x,y;
}state;

state *readstate(char *path);
void writestate(char *path,int x,int y);
