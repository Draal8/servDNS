#ifndef H_RECVER
	#define H_RECVER

#include "common.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "tourniquet.h"

noreturn void usage(char *str);
void arg_check(int argc, char *argv[]);

int resolution(char *search_file, char *buff, int sockfd,
	struct sockaddr_in6 client);
int msg_builder(char **old, char *recu, char *code, char *line);
char *racine_extractor(char *buff);


int serv_type = 0;
//0: non renseigné 1: serveur racine
//2: serveur de nom de rang 1 3: serveur de nom de rang 2

#endif

