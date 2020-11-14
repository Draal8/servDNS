#ifndef H_TOURNIQUET
	#define H_TOURNIQUET

#include "common.h"

#include <math.h>
#include <limits.h>
#include <sys/time.h>

char *token_master (char *buff, char **adrs, char **horodatage, char **port, char **saveptr);
char *tourniquet(FILE *fd);
char *tourniquet_suite(char *buff, int flush);
int file_search(FILE *fd, char *port);
int register_time(char *buff, char *serv, int crash);
int replace_line(int nbline, char *to_put);
long long int recup_temps(char *port);

#endif

