#include "debug.h"
#include <stdio.h>
#include <stdlib.h>

int CC(int code, const char *msg) {
	if(code < 0) {
		puts(msg);
		exit(1);
	}
	return code;
}

void *CP(void *ptr, const char *msg) {
	if(!ptr) {
		puts(msg);
		exit(1);
	}
	return ptr;
}

