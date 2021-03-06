#include "../head/common.h"

//fonction gloable pour crash en cas d'erreur non récupérable
noreturn void rerror(char *str) {
	if (errno != 0) {
		perror(str);
	} else {
		write(STDERR_FILENO, str, strlen(str));
		write(STDERR_FILENO, "\n", 1);
	}
    exit(EXIT_FAILURE);
}

//sleep masi version millisecondes
int msleep(long int ms) {
	struct timespec t;
	t.tv_sec = ms / 1000;
	t.tv_nsec = (ms % 1000) * 1000000;
	return nanosleep(&t, NULL);
}


