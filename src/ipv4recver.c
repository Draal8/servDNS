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

//serveur racine

void arg_check(int argc, char *argv);
void domtab_create();
void domaine_create(char *racine, char *addr, int port);
char *resolution(char *buff);
void tab_null();
int search_domain(char *str);

#define STR_SIZE 2048
#define CHECK(op) do { if (op == -1) rerror(#op);} while(0)

noreturn void rerror(char *str) {
	if (errno != 0) {
		perror(str);
	} else {
		write(STDERR_FILENO, str, strlen(str));
		write(STDERR_FILENO, "\n", 1);
	}
    exit(EXIT_FAILURE);
}

typedef struct {
	char *racine;
	char *addr;
	int port;
} domaine;

domaine *dom_tab[50];
int lport = 3500;	//entre 1024 et 5000

int main() {
	int sockfd;
	char buff[STR_SIZE];
	socklen_t addrlen;
	struct sockaddr_in my_addr;
	struct sockaddr_in client;
	
	CHECK((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(lport);
	CHECK(inet_pton(AF_INET, "0.0.0.0", &my_addr.sin_addr.s_addr));
	addrlen = sizeof(struct sockaddr_in);
	CHECK(bind(sockfd, (struct sockaddr *) &my_addr, addrlen));
	
	domtab_create();
	
	char *msg;
	while(1) {
		memset(buff, '\0',STR_SIZE);
		
		CHECK(recvfrom(sockfd, buff, STR_SIZE, 0, (struct sockaddr *) &client, &addrlen));	//en faire un thread ou un processus pour continuer a écouter meme si on est en plein traitement voire faire plusieurs ecoutes simultanement
		
		if((msg = resolution(buff)) == NULL); //renvoyer un message d'erreur
		else {
			printf("%s\n", msg); 
			free(msg);
		}
	}
	
	close(sockfd);	//pas besoin de check dans tous les cas on quitte ensuite.
	
    return 0;
}

char *resolution(char *buff) {
	int end = strlen(buff), i = end, j = 0, id;
	char racine[STR_SIZE], *address, temp[10];
	while (i >= 0 && buff[i] != '.') { i--;	}
	i++;
	for (;i < end; i++) {
		racine[j] = buff[i];
		j++;
	}
	racine[j] = '\0';
	if ((id = search_domain(racine)) == -1) return NULL;
	
	if ((address = malloc(STR_SIZE * sizeof(*address))) == NULL)
		rerror("address malloc");
	address[0] = '\0';
	strcat(address, dom_tab[id]->addr);
	strcat(address, " | ");
	sprintf(temp, "%d", dom_tab[id]->port);
	strcat(address, temp);
	return address;
}

int search_domain(char *str) {
	int i = 0;
	while (i < 50 && dom_tab[i] != NULL && strcmp(str, dom_tab[i]->racine)) { i++; }
	if (i < 50 && dom_tab[i] != NULL && !strcmp(str, dom_tab[i]->racine)) { return i; }
	return -1;
}

void domtab_create() {
	tab_null();
	domaine_create("fr", "0.0.0.0", 3501);
	//domaine_create("fr", "0::0", 3501);
	domaine_create("com", "0.0.0.0", 3502);
	//domaine_create("com", "0::0", 3502);
	domaine_create("eu", "0.0.0.0", 3503);
	//domaine_create("eu", "0::0", 3503);
	domaine_create("net", "0.0.0.0", 3504);
	//domaine_create("net", "0::0", 3504);
	domaine_create("org", "0.0.0.0", 3505);
	//domaine_create("org", "0::0", 3505);
}

void domaine_create(char *racine, char *addr, int port) {
	int i = 0;
	while(i < 50 && dom_tab[i] != NULL) i++;
	if (i != 50) {
		if ((dom_tab[i] = malloc(sizeof(domaine))) == NULL)
			rerror("domaine malloc");
		if ((dom_tab[i]->addr = malloc(STR_SIZE)) == NULL)
			rerror("address malloc");
		if ((dom_tab[i]->racine = malloc(20)) == NULL)
			rerror("racine malloc");
		strcpy(dom_tab[i]->racine, racine);
		strcpy(dom_tab[i]->addr, addr);
		dom_tab[i]->port = port;
	}
}

void tab_null() {
	int i;
	for(i = 0; i < 50; i++) {
		dom_tab[i] = NULL;
	}
}

