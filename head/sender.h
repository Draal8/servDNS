#ifndef H_SENDER
	#define H_SENDER

#include "common.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "tourniquet.h"

noreturn void usage(char *str);
void arg_check(int argc, char *argv[]);

int sender(char *site, char *buff);
int suite(int sockfd, char *buff, char *site, struct sockaddr_in6 *dest,
	struct sockaddr_in6 *retour);
char *msg_builder(char *site);
char *parcours_serv_racine(FILE *fd);
void call_register_time(char *buff, char *serveur);

#endif

