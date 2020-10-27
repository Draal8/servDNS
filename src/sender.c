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

// ./sender 0.0.0.0 3500 "test"
// l'adresse peut-etre passee avec des "" ou sans

void recevoir();

int main(int argc, char *argv[]) {
	if (argc != 4) rerror("bad number of arguments");
	int sockfd;
	socklen_t addrlen;
	struct sockaddr_in dest;
	
	CHECK((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(atoi(argv[2]));
	addrlen = sizeof(struct sockaddr_in);
	CHECK(inet_pton(AF_INET, argv[1], &dest.sin_addr));
	CHECK(sendto(sockfd, argv[3], strlen(argv[3]), 0, (struct sockaddr *) &dest, addrlen));
	CHECK(sendto(sockfd, "0.0.0.0 | 3499", strlen("0.0.0.0 | 3499"), 0, (struct sockaddr *) &dest, addrlen));
	
	recevoir();
	
    return 0;
}

void recevoir() {
	int sockfd;
	char buff[STR_SIZE];
	socklen_t addrlen;
	struct sockaddr_in my_addr;
	struct sockaddr_in client;
	
	CHECK((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(3499);
	CHECK(inet_pton(AF_INET, "0.0.0.0", &my_addr.sin_addr.s_addr));
	addrlen = sizeof(struct sockaddr_in);
	CHECK(bind(sockfd, (struct sockaddr *) &my_addr, addrlen));
	
	memset(buff, '\0',STR_SIZE);
		
	CHECK(recvfrom(sockfd, buff, STR_SIZE, 0, (struct sockaddr *) &client, &addrlen));
	printf("buff : %s\n", buff);
}

/*void arg_check(int argc, char *argv[]) {
	if (argc != 4) rerror("bad number of arguments");
	int i, nbPoints = 0, end = strlen(argv[i]);
	for (i = 0; i < end; i++) {
		if (argv[i] == '.') nbPoints++;
	}
	if (i == end) rerror("not an adress");
}*/
