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

// ./sender 0.0.0.0 3500 "test"
// l'adresse peut-etre passee avec des "" ou sans

int main(int argc, char *argv[]) {
	int sockfd;
	socklen_t addrlen;
	struct sockaddr_in dest;
	
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	dest.sin_family = AF_INET;
	dest.sin_port = htons(atoi(argv[2]));
	addrlen = sizeof(struct sockaddr_in);
	inet_pton(AF_INET, argv[1], &dest.sin_addr);
	sendto(sockfd, argv[3], strlen(argv[3]), 0, (struct sockaddr *) &dest, addrlen);
	
    return 0;
}
