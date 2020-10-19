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

// ./recv 3500

int main(int argc, char *argv[]) {
	int sockfd;
	char buff[1024];
	socklen_t addrlen;
	struct sockaddr_in my_addr;
	struct sockaddr_in client;
	
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(atoi(argv[1]));
	inet_pton(AF_INET, "0.0.0.0", &my_addr.sin_addr.s_addr);
	
	addrlen = sizeof(struct sockaddr_in);	
	
	memset(buff, '\0',1024);
	bind(sockfd, (struct sockaddr *) &my_addr, addrlen);
	recvfrom(sockfd, buff, 1024, 0, (struct sockaddr *) &client, &addrlen);
	printf("%s", buff);
	close(sockfd);
	
    return 0;
}
