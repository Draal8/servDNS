#include "../head/sender.h"

// ./sender bddclient bddsites

int choix_serv(FILE *fd, char **buff);

int nb_racines = 0;
int timeout = 18000;

int main(int argc, char *argv[]) {
	arg_check(argc, argv);
	int i, a;
	size_t n = STR_SIZE;
	char *buff, *parcours, *site/*, *serveur*/;
	//if ((buff = malloc(STR_SIZE)) == NULL) rerror("malloc buff");
	if ((site = malloc(STR_SIZE)) == NULL) rerror("malloc buff");
	FILE *fd, *fe;
	fd = fopen(argv[1], "r");
	fe = fopen(argv[2], "r");
	
	//getline(&buff, &n, fd);
	while (getline(&site, &n, fe) > 0) {
		buff = parcours_serv_racine(fd);
		if (site[0] != '\0' && site[0] != '\n') {
			i = 0;
			//printf("buff : %s\n", buff);
			while ((a = sender(site, buff)) != 0) {
				if (i > nb_racines) {
					printf ("le site est introuvable\n");
					break;
				}
				
				parcours = parcours_serv_racine(fd);
				if (parcours == NULL) break;
				i++;
			}
			free(buff);
		}
	}
	if (errno != 0) rerror("read");
	free(site);
	free(buff);
	
	fclose(fd);
	fclose(fe);
    return 0;
}


int sender(char *site, char *adrs_port) {
	int sockfd, rt, i, a = 0;
	char buff[STR_SIZE], copy[STR_SIZE], *message, *adrs, *port, *saveptr;
	socklen_t addrlen;
	struct sockaddr_in6 dest, retour;
	struct timeval time;
	time.tv_sec = 0;
	time.tv_usec = timeout;
	//j'ai mis un rand qui se balade sur [1,20]

	strcpy(copy, adrs_port);
	adrs = strtok_r(copy, " | ", &saveptr);
	port = strtok_r(NULL, " | ", &saveptr);
	
	message = msg_builder(site);
	
	CHECK((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)));
	memset(&dest, 0, sizeof(dest));	//mettre des 0 pour que valgrinou soit content
	memset(&retour, 0, sizeof(retour));	//(initialisation incomplete)
	dest.sin6_family = AF_INET6;
	dest.sin6_port = htons(atoi(port));
	dest.sin6_addr = in6addr_any;
	CHECK(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(struct timeval)));
	CHECK(setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &(int){0}, sizeof(int)));
	
	addrlen = sizeof(struct sockaddr_in6);
	CHECK(inet_pton(AF_INET6, adrs, &dest.sin6_addr));
	printf("message : %s\n", message);
	
	i = 0;
	do {
	CHECK(sendto(sockfd, message, strlen(message)+1, 0, (struct sockaddr *) &dest, addrlen));
	
	recvfrom(sockfd, buff, STR_SIZE, 0, (struct sockaddr *) &retour, &addrlen);
	i++;
	} while ((errno == EAGAIN || errno == EWOULDBLOCK) && i < 3);
	
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
		errno = 0;
		printf("timeout\n");
		register_time(buff, adrs_port, 1);
		free(message);
		return -1;
	}
	printf("reponse : %s\n", buff);
	
	for (i = 0; i < (int)strlen(buff); i++) {
		if (buff[i] == '-' && i < (int)strlen(buff)-1 && buff[i+1] == '1') {
			register_time(buff, adrs_port, 2);
			a = 1;
			free(message);
			return 1;
		}
	}
	if (a == 0) register_time(buff, adrs_port, 0);
	
	rt = suite(sockfd, buff, site, &dest, &retour);
	
	if (rt == 1) {
		printf("Adresse resolue\n");
		free(message);
		return 0;
	} else {
		free(message);
		return 1;
	}
}

int suite(int sockfd, char *old_buff, char *site, struct sockaddr_in6 *dest, struct sockaddr_in6 *retour) {
	if (atteint(old_buff, site) == 0) {
		return 1;
	}
	int ret = 0, i, a = 0;
	char copy[STR_SIZE], *tmp, *adrs, *port, *saveptr, *message, *buff, adrs_port[STR_SIZE];
	socklen_t addrlen;
	if ((buff = malloc(STR_SIZE)) == NULL) rerror("malloc");
	
	//aFree = token_master(old_buff, &adrs, &horo, &port, &saveptr);
	tourniquet_suite(old_buff, 0);
	while ((tmp = tourniquet_suite(NULL, 0)) != NULL && ret != 1) {
		//printf("tmp : %s\n", tmp);
		strcpy(copy, tmp);
		strtok_r(copy, ", ", &saveptr);	//on balance la racine
		adrs = strtok_r(NULL, ", ", &saveptr);
		port = strtok_r(NULL, ", ", &saveptr);
		snprintf(adrs_port, STR_SIZE, "%s | %s", adrs, port);
		if (port[strlen(port)-1] == '\n')
			port[strlen(port)-1] ='\0';

		dest->sin6_port = htons(atoi(port));
		//dest->sin6_addr = adrs;
		//normallement on ferait la meme chose pour l'addresse mais inutile car on reste en local
		addrlen = sizeof(struct sockaddr_in6);
		
		if ((message = msg_builder(site)) == NULL) rerror("msg_builder");
		i = 0;
		do {
			CHECK(sendto(sockfd, message, strlen(message)+1, 0, (struct sockaddr *) dest, addrlen));
			//free(message);
				
			recvfrom(sockfd, buff, STR_SIZE, 0, (struct sockaddr *) retour, &addrlen);
			i++;
		} while ((errno == EAGAIN || errno == EWOULDBLOCK) && i < 3);
	
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			errno = 0;
			printf("timeout\n");
			register_time(buff, adrs_port, 1);
			continue;
		}
		printf("reponse : %s\n", buff);
		
		for (i = 0; i < (int)strlen(buff); i++) {
			if (buff[i] == '-' && i < (int)strlen(buff)-1 && buff[i+1] == '1') {
				register_time(buff, adrs_port, 2);
				a = 1;
			}
		}
		if (a == 1) continue;
		register_time(buff, adrs_port, 0);
		free(tmp);
		ret = suite(sockfd, buff, site, dest, retour);
	}
	tourniquet_suite(NULL, 1);
	free(buff);
	free(message);
	return ret;
}

int atteint(char *buff, char *site) {
	int i;
	char copy[STR_SIZE], *lien, *saveptr;
	strncpy(copy, buff, STR_SIZE);
	strtok_r(copy, "|", &saveptr);
	for (i = 0; i < 3; i++) {
		strtok_r(NULL, "|", &saveptr);
	}
	lien = strtok_r(NULL, ", ", &saveptr);
	if (lien == NULL || strlen(lien) < strlen(site)-1)
		return 1;
	return strncmp(lien, site, strlen(lien));
}

char *parcours_serv_racine(FILE *fd) {
	size_t n = STR_SIZE;
	char *serveur;
	if ((serveur = malloc(STR_SIZE)) == NULL)
		rerror("malloc");
	
	if (getline(&serveur, &n, fd) > 2) {
		return serveur;
	} else {
		free(serveur);
		return tourniquet(fd);
	}
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
	msg[strlen(msg)-1] = '\0';
	return msg;
}


void arg_check(int argc, char *argv[]) {
	if (argc < 3 || argc > 4) {
		usage("bad number of argument (3 ou 4)\n\n");
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
	
	size_t n = STR_SIZE;
	char *str;
	if ((str = malloc(STR_SIZE)) == NULL) rerror("malloc");
	FILE *fd;
	fd = fopen(argv[1], "r");
	while (getline(&str, &n, fd) > 0) nb_racines++;
	fclose(fd);
	fd = fopen("time", "w");
	fclose(fd);
	free(str);
	
	if (argc == 4) {
		timeout = atoi(argv[3]);
	}
}


noreturn void usage(char *str) {
	char str2[] = "./sender bddclient bddsites\n\nbddclient est un fichier qui contient des lignes (ex: '127.0.0.1 | 3500'). Il s'agit de l'adresse (IPV4 ou IPV6) et du numéro de port des serveurs racines\nbddsites est un fichier qui contient des lignes (ex: 'www.unistra.fr'). Il s'agit des noms a résoudre\n";
	write(STDERR_FILENO, str, strlen(str));
	write(STDERR_FILENO, str2, strlen(str2));
	exit(EXIT_FAILURE);
}

