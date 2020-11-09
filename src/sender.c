#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdnoreturn.h> //compiler en C11
#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/time.h>

void arg_check(int argc, char *argv[]);

#define CHECK(op) do { if (op == -1) rerror(#op);} while(0)
#define CHECK_NOER(op, erno) do { if (op == -1) return(erno);} while(0)
#define STR_SIZE 4096

noreturn void rerror(char *str) {
	if (errno != 0) {
		perror(str);
	} else {
		write(STDERR_FILENO, str, strlen(str));
		write(STDERR_FILENO, "\n", 1);
	}
    exit(EXIT_FAILURE);
}

// ./sender bddclient bddsites

void arg_check(int argc, char *argv[]);
noreturn void usage(char *str);
int choix_serv(FILE *fd, char **buff);
int sender(char *site, char *buff);
char *msg_builder(char *site);

int main(int argc, char *argv[]) {
	arg_check(argc, argv);
	size_t n = STR_SIZE;
	char *buff, *site;
	if ((buff = malloc(STR_SIZE)) == NULL) rerror("malloc buff");
	if ((site = malloc(STR_SIZE)) == NULL) rerror("malloc buff");
	FILE *fd, *fe;
	fd = fopen(argv[1], "r");
	fe = fopen(argv[2], "r");
	
	getline(&buff, &n, fd);
	while (getline(&site, &n, fe) > 0) {
		
		if (site[0] != '\0' && site[0] != '\n') {
			int a;
			if((a = sender(site, buff)) != 0)
				return a;
		}
	}
	if (errno != 0) rerror("read");
	
    return 0;
}

int sender(char *site, char *adrs_port) {
	int sockfd;
	char buff[STR_SIZE], copy[STR_SIZE], *message, *adrs, *port, *saveptr;
	socklen_t addrlen;
	struct sockaddr_in6 dest;
	struct sockaddr_in6 retour;
	
	strcpy(copy, adrs_port);
	adrs = strtok_r(copy, " | ", &saveptr);
	port = strtok_r(NULL, " | ", &saveptr);
	
	message = msg_builder(site);
	
	CHECK_NOER((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)), 1);
	dest.sin6_family = AF_INET6;
	dest.sin6_port = htons(atoi(port));
	dest.sin6_addr = in6addr_any;
	CHECK(setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &(int){0}, sizeof(int)));
	
	addrlen = sizeof(struct sockaddr_in6);
	CHECK_NOER(inet_pton(AF_INET6, adrs, &dest.sin6_addr), 2);
	message[strlen(message)-1] = '\0';
	printf("message : %s\n", message);
	CHECK_NOER(sendto(sockfd, message, strlen(message)+1, 0, (struct sockaddr *) &dest, addrlen), 3);
	
	
	CHECK_NOER(recvfrom(sockfd, buff, STR_SIZE, 0, (struct sockaddr *) &retour, &addrlen), 4);
	printf("buff : %s\n", buff);
	
	int i;
	char *tmp, *saveptr2;
	strcpy(copy, buff);
	adrs = strtok_r(copy, "|", &saveptr);
	for (i = 0; i < 3; i++)
		adrs = strtok_r(NULL, "|", &saveptr);
	if (strlen(adrs) == 2 && adrs[0] == '-' && adrs[1] == '1')
		return -1;
	while ((tmp = strtok_r(NULL, "|", &saveptr)) != NULL) {
		strcpy(copy, tmp);
		adrs = strtok_r(copy, ", ", &saveptr2);
		adrs = strtok_r(NULL, ", ", &saveptr2);
		port = strtok_r(NULL, ", ", &saveptr2);
		
		dest.sin6_port = htons(atoi(port));
		//normallement on ferait la meme chose pour l'addresse
		CHECK_NOER(sendto(sockfd, message, strlen(message)+1, 0, (struct sockaddr *) &dest, addrlen), 3);
		CHECK_NOER(recvfrom(sockfd, buff, STR_SIZE, 0, (struct sockaddr *) &retour, &addrlen), 4);
	}
	printf("reponse : %s\n", buff);
	
	strcpy(copy, buff);
	adrs = strtok_r(copy, "|", &saveptr);
	for (i = 0; i < 3; i++)
		adrs = strtok_r(NULL, "|", &saveptr);
	if (strlen(adrs) == 2 && adrs[0] == '-' && adrs[1] == '1')
		return -1;
	while ((tmp = strtok_r(NULL, "|", &saveptr)) != NULL) {
		strcpy(copy, tmp);
		adrs = strtok_r(copy, ", ", &saveptr2);
		adrs = strtok_r(NULL, ", ", &saveptr2);
		port = strtok_r(NULL, ", ", &saveptr2);
		
		dest.sin6_port = htons(atoi(port));
		//normallement on ferait la meme chose pour l'addresse
		CHECK_NOER(sendto(sockfd, message, strlen(message)+1, 0, (struct sockaddr *) &dest, addrlen), 3);
		CHECK_NOER(recvfrom(sockfd, buff, STR_SIZE, 0, (struct sockaddr *) &retour, &addrlen), 4);
	}
	printf("reponse : %s\n", buff);
	
	free(message);
	return 0;
}

char *msg_builder(char *site) {
	static long int trans_id = 1;
	unsigned long long int tim;
	int rt;
	char *msg;
	struct timeval horodatage;
	
	if ((msg = malloc(STR_SIZE)) == NULL) return NULL;
	memset(msg, '\0',STR_SIZE);
	CHECK_NOER(gettimeofday(&horodatage, NULL), NULL);
	tim = horodatage.tv_sec*1000 + horodatage.tv_usec/1000;
	
	rt = snprintf(msg, STR_SIZE, "%ld | %llu | %s", trans_id, tim, site);
	
	if (rt < 0 || rt >= STR_SIZE) return NULL;
	return msg;
}

void arg_check(int argc, char *argv[]) {
	if (argc != 3) {
		usage("bad number of argument (3)\n\n");
	}
		
	struct stat s;	
	CHECK(stat(argv[1], &s));
	if (!S_ISREG(s.st_mode)) {
		usage("error argv[1] : not a regular file\n\n");
	}
	
	CHECK(stat(argv[2], &s));
	if (!S_ISREG(s.st_mode)) {
		usage("error argv[2] : not a regular file\n\n");
	}
}

noreturn void usage(char *str) {
	char str2[] = "./sender bddclient bddsites\n\nbddclient est un fichier qui contient des lignes (ex: '127.0.0.1 | 3500'). Il s'agit de l'adresse (IPV4 ou IPV6) et du numéro de port des serveurs racines\nbddsites est un fichier qui contient des lignes (ex: 'www.unistra.fr'). Il s'agit des noms a résoudre\n";
	write(STDERR_FILENO, str, strlen(str));
	write(STDERR_FILENO, str2, strlen(str2));
	exit(EXIT_FAILURE);
}

