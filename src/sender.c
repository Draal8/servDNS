#include "../head/sender.h"

// ./sender bddclient bddsites

int choix_serv(FILE *fd, char **buff);

int nb_racines = 0;

int main(int argc, char *argv[]) {
	arg_check(argc, argv);
	int i;
	size_t n = STR_SIZE;
	char *buff, *site/*, *serveur*/;
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
			while (sender(site, buff) != 0) {
				free (buff);
				if (i > nb_racines) {
					printf ("le site est introuvable\n");
					break;
				}
				
				buff = parcours_serv_racine(fd);
				if (buff == NULL) break;
				i++;
			}
		}
		if (buff != NULL) free(buff);
		//a cause du break parfois on a un double free ici sans la condition
	}
	if (errno != 0) rerror("read");
	free(site);
	
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
	time.tv_usec = 5000;	//d'apres l'horodatage un echange dure moins d'une milli-seconde donc 5 c'est safe mais faut ptet le descendre

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
	CHECK(sendto(sockfd, message, strlen(message)+1, 0, (struct sockaddr *) &dest, addrlen));
	free(message);
	
	recvfrom(sockfd, buff, STR_SIZE, 0, (struct sockaddr *) &retour, &addrlen);
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
		register_time(buff, adrs_port, 1);
		return -1;
	}
	printf("reponse : %s\n", buff);
	
	for (i = 0; i < (int)strlen(buff); i++) {
		if (buff[i] == '-' && i < (int)strlen(buff)-1 && buff[i+1] == '1')
			a = 1;
	}
	if (a == 0) register_time(buff, adrs_port, 0);
	
	rt = 0;
	while (rt == 0) {
		rt = suite(sockfd, buff, site, &dest, &retour);
		if (rt == -1) {
			register_time(buff, adrs_port, 2);
			return -1;
		}
		else if (rt == 1) printf("Adresse resolue\n");
		else {
			char tmp_str[STR_SIZE];
			snprintf(tmp_str, STR_SIZE, "127.0.0.1 | %d", ntohs(dest.sin6_port));
			register_time(buff, tmp_str, 0);
		}
	}
	
	return 0;
}

int suite(int sockfd, char *buff, char *site, struct sockaddr_in6 *dest, struct sockaddr_in6 *retour) {
	int i;
	char copy[STR_SIZE], *message, *adrs, *port, *saveptr, *tmp, *saveptr2;
	socklen_t addrlen;
	
	strcpy(copy, buff);
	adrs = strtok_r(copy, "|", &saveptr);
	for (i = 0; i < 3; i++)
		adrs = strtok_r(NULL, "|", &saveptr);
	if (strlen(adrs) == 2 && adrs[0] == '-' && adrs[1] == '1') {
		return -1;
	}
	while ((tmp = strtok_r(NULL, "|", &saveptr)) != NULL) {
		strcpy(copy, tmp);
		adrs = strtok_r(copy, ", ", &saveptr2);
		if (strncmp(adrs, site, strlen(adrs)) == 0) return 1;
		adrs = strtok_r(NULL, ", ", &saveptr2);
		port = strtok_r(NULL, ", ", &saveptr2);
		
		dest->sin6_port = htons(atoi(port));
		addrlen = sizeof(struct sockaddr_in6);
		//normallement on ferait la meme chose pour l'addresse mais inutile car on reste en local
		//dest->sin6_addr = adrs;
		message = msg_builder(site);
		printf("message : %s\n", message);
		CHECK_NOER(sendto(sockfd, message, strlen(message)+1, 0, (struct sockaddr *) dest, addrlen), 1);
		free(message);
		
		buff[0] = '\0';
		recvfrom(sockfd, buff, STR_SIZE, 0, (struct sockaddr *) retour, &addrlen);
	}
	if (buff[0] == '\0') return -1;
	printf("reponse : %s\n", buff);
	return 0;
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
}


noreturn void usage(char *str) {
	char str2[] = "./sender bddclient bddsites\n\nbddclient est un fichier qui contient des lignes (ex: '127.0.0.1 | 3500'). Il s'agit de l'adresse (IPV4 ou IPV6) et du numéro de port des serveurs racines\nbddsites est un fichier qui contient des lignes (ex: 'www.unistra.fr'). Il s'agit des noms a résoudre\n";
	write(STDERR_FILENO, str, strlen(str));
	write(STDERR_FILENO, str2, strlen(str2));
	exit(EXIT_FAILURE);
}

