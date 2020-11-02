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

void arg_check(int argc, char *argv[]);

#define CHECK(op) do { if (op == -1) rerror(#op);} while(0)
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

// ./sender ::1 3500 "unistra.fr"
// l'adresse peut-etre passee avec des "" ou sans

void recevoir();

int main(int argc, char *argv[]) {
	if (argc != 4) rerror("bad number of arguments");
	int sockfd;
	char buff[STR_SIZE];
	socklen_t addrlen;
	struct sockaddr_in6 dest;
	struct sockaddr_in6 retour;
	
	CHECK((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)));
	dest.sin6_family = AF_INET6;
	dest.sin6_port = htons(atoi(argv[2]));
	
	/*printf("port : %i\n", getsockname(sockfd, (struct sockaddr *) &dest, &addrlen));*/
	addrlen = sizeof(struct sockaddr_in6);
	CHECK(inet_pton(AF_INET6, argv[1], &dest.sin6_addr));
	CHECK(sendto(sockfd, argv[3], strlen(argv[3]), 0, (struct sockaddr *) &dest, addrlen));
	
	CHECK(recvfrom(sockfd, buff, STR_SIZE, 0, (struct sockaddr *) &retour, &addrlen));
	printf("buff : %s\n", buff);
    return 0;
}

/*void arg_check(int argc, char *argv[]) {
	if (argc != 4) rerror("bad number of arguments");
	int i, nbPoints = 0, end = strlen(argv[i]);
	for (i = 0; i < end; i++) {
		if (argv[i] == '.') nbPoints++;
	}
	if (i == end) rerror("not an adress");
}*/
