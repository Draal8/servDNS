#ifndef H_TOURNIQUET
	#define H_TOURNIQUET

#include "common.h"

#include <math.h>
#include <limits.h>
#include <sys/time.h>

char *token_master (char *buff, char **adrs, char **horodatage, char **port, char **saveptr);
char *tourniquet(FILE *fd);
int file_search(FILE *fd, char *port);
int register_time(char *buff, char *serv, int crash);
int replace_line(int nbline, char *to_put);

#endif

