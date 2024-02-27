#ifndef DEBUG_H
#define DEBUG_H

#include <stdlib.h>

int CC(int code, const char *msg) {
	if(code < 0) {
		perror(msg);
		exit(1);
	}
	return code;
}

void *CP(void *ptr, const char *msg) {
	if(!ptr) {
		perror(msg);
		exit(1);
	}
	return ptr;
}


#endif
