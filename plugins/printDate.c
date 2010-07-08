#include <stdio.h>
#include <stdlib.h>
char *callPlugin(char *ret, size_t max){
	printf("lol\n");
	strcpy(ret, "t\0");
	return ret;
}
