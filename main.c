#ifndef _XOPEN_SOURCE
	#define _XOPEN_SOURCE	//pour compiler sur ma machine, turing ignorera ceci
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdnoreturn.h> //compiler en C11
#include <errno.h>
#include <string.h>

#define CHECK(op) do { if (op == -1) rerror(#op);} while(0)

noreturn void rerror(char *str) {
	if (errno != 0) {
		perror(str);
	} else {
		write(STDOUT_FILENO, str, strlen(str));
		write(STDOUT_FILENO, "\n", 1);
	}
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    return 0;
}
