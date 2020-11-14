#include "../head/recver.h"

// ./recver 3500 bddserv1 1

long int delai;

int main(int argc, char *argv[]) {
	arg_check(argc, argv);
	serv_type = atoi(argv[3]);

//
//	Configure IPV4 & IPV6 listener
//

	int sockfd;
	char buff[STR_SIZE];
	socklen_t addrlen;
	struct sockaddr_in6 my_addr, client;
	addrlen = sizeof(struct sockaddr_in6);
	
	CHECK((sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)));
	memset(&my_addr, 0, sizeof(my_addr));	//mettre des 0 pour que valgrinou soit content
	memset(&client, 0, sizeof(client));	//(initialisation incomplete)
	my_addr.sin6_family = AF_INET6;
	my_addr.sin6_port = htons(atoi(argv[1]));
	my_addr.sin6_addr = in6addr_any;
	
	CHECK(setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &(int){0}, sizeof(int)));	//On désactive ipv6 only
	//CHECK(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)));	//On permet de réutiliser la socket
	CHECK(bind(sockfd, (struct sockaddr *) &my_addr, addrlen));
	
	while(1) {	//mettre un timer ou arreter avec un signal
		memset(buff, '\0',STR_SIZE);
		CHECK(recvfrom(sockfd, buff, STR_SIZE, 0, (struct sockaddr *) &client, &addrlen));	//ecoutes simulannees ?
		printf("recu : %s\n", buff);
		if (resolution(argv[2], buff, sockfd, client) != 0) return 1;
	}
	
	close(sockfd);	//pas besoin de check dans tous les cas on quitte ensuite.
    return 0;
}


//Fonction qui s'occupe de la resolution de site recu
int resolution(char *search_file, char *buff, int sockfd, struct sockaddr_in6 client) {
	char *message, *racine, *line;
	FILE *fd;
	
	racine = racine_extractor(buff);
	
	if ((message = malloc(STR_SIZE)) == NULL) return -1;
	memset(message, '\0',STR_SIZE);
	
	if ((fd = fopen(search_file, "r")) == NULL) return -1;
	if ((line = malloc(STR_SIZE)) == NULL) return -1;
	memset(line, '\0',STR_SIZE);
	while (getline(&line, &(size_t){STR_SIZE}, fd) > 0) {
		if (strncmp(racine, line, strlen(racine)) == 0) {
			if (msg_builder(&message, buff, "1", line) == -1) rerror("msg builder");	//remplir les args
		}
	}
	fclose(fd);
	free(racine);
	free(line);
	
	if (message[0] == '\0') {
		if (msg_builder(&message, buff, "-1", NULL) == -1) rerror("msg builder");
	}
	
	printf("repondu : %s\n", message);
	msleep(delai);
	CHECK_NOER(sendto(sockfd, message, strlen(message)+1, 0, (struct sockaddr *) &client, sizeof(struct sockaddr_in6)), -1);
	
	free(message);
	return 0;
}


//Fonction qui construit le message a envoyer
int msg_builder(char **old, char *recu, char *code, char *line) {
	int rt;
	char *racine, *address, *port, *saveptr, *line_cpy;
	if (line != NULL) { //on sort racine, port et adresse de la ligne lue
		if ((line_cpy = malloc(STR_SIZE)) == NULL) rerror("malloc");
		memset(line_cpy, '\0',STR_SIZE);
		strcpy(line_cpy, line);	//no overflow since they have the same length
	
		racine = strtok_r(line_cpy, " | ", &saveptr);
		address = strtok_r(NULL, " | ", &saveptr);
		port = strtok_r(NULL, " | ", &saveptr);
		if (port[strlen(port)-1] == '\n') port[strlen(port)-1] = '\0';
		
		if (*old[0] == '\0') {
			rt = snprintf(*old, STR_SIZE, "%s |%s| %s, %s, %s", recu, code, racine, address, port);
			if (rt < 0 || rt >= STR_SIZE) return -1;
		} else {
			if (strlen(*old)+strlen(racine)+strlen(address)+strlen(port)+8 > STR_SIZE) return -1;
			strncat(*old, " | ", STR_SIZE-strlen(*old));
			strncat(*old, racine, STR_SIZE-strlen(*old));
			strncat(*old, ", ", STR_SIZE-strlen(*old));
			strncat(*old, address, STR_SIZE-strlen(*old));
			strncat(*old, ", ", STR_SIZE-strlen(*old));
			strncat(*old, port, STR_SIZE-strlen(*old));
			strncat(*old, "\n", STR_SIZE-strlen(*old));
		}
		free(line_cpy);
	} else {
		if (*old[0] == '\0') {
			rt = snprintf(*old, STR_SIZE, "%s |%s|\n", recu, code);
			if (rt < 0 || rt >= STR_SIZE) return -1;
		} else {
			if (strlen(*old)+6 > STR_SIZE) return -1;
			strncat(*old, " |", STR_SIZE-strlen(*old));
			strncat(*old, code, STR_SIZE-strlen(*old));
			strncat(*old, "|", STR_SIZE-strlen(*old));
			strncat(*old, "\n", STR_SIZE-strlen(*old));
		}
	}
	return 0;
}


//Fonction qui extrait la racine a rechercher dans le fichier repertoriant les adresses
char *racine_extractor(char *buff) {
	int i = 0, j = 0, end;
	char *saveptr, *buff_cpy, *racine, *site;
	
	if ((racine = malloc(STR_SIZE)) == NULL) rerror("malloc");
	if ((buff_cpy = malloc(STR_SIZE)) == NULL) rerror("malloc");
	strcpy(buff_cpy, buff);	//no overflow since they have the same length
	
	site = strtok_r(buff_cpy, " | ", &saveptr); //on balance l'id
	site = strtok_r(NULL, " | ", &saveptr);	//on balance l'horodatage
	site = strtok_r(NULL, " | ", &saveptr);
	
	end = strlen(site);
	
	if (serv_type < 3) {
		while (i < end && site[i] != '.') { i++; }
		i++;
	}
	if (serv_type < 2) {
		while (i < end && site[i] != '.') { i++; }
	}
	for (; i < end && site[i] != '\n'; i++) {
		racine[j] = site[i];
		j++;
	}
	racine[j] = '\0';
	
	free(buff_cpy);
	return racine;
}


void arg_check(int argc, char *argv[]) {
	if (argc < 4 || argc > 5) {
		usage("bad number of argument (4 ou 5)\n\n");
	}
	int tmp;
	tmp = atoi(argv[1]);
	if (tmp < 1024 || tmp > 65535) {
		usage("error argv[1] : port en dehors de la plage [1024,65535]\n\n");
	}
	
	struct stat s;	
	CHECK(stat(argv[2], &s));
	if (!S_ISREG(s.st_mode)) {
		usage("error argv[2] : not a regular file\n\n");
	}
	
	tmp = atoi(argv[3]);
	if (tmp < 1 || tmp > 3) {
		usage("error argv[3] : not in values {1,2,3}\n\n");
	}
	
	if (argc == 5)
		delai = atol(argv[4]);
	else {
		time_t t;
		srand((unsigned) time(&t));
		delai = rand()%20+1;
	}
}


noreturn void usage(char *str) {
	char str2[] = "./recver port bddserv1 type\nport est le port du serveur\nbddserv1 est un fichier qui contient des lignes (ex: '.fr | 127.0.0.1 | 3501'). Il s'agit des adresses des serveurs de nom inférieurs\ntype correspond a la profondeur du serveur\n";
	write(STDERR_FILENO, str, strlen(str));
	write(STDERR_FILENO, str2, strlen(str2));
	exit(EXIT_FAILURE);
}

