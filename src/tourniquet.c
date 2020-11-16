#include "../head/tourniquet.h"

//Fonction tourniquet pour sender() du fichier sender.c
char *tourniquet(FILE *fd) {
	size_t n = STR_SIZE;
	unsigned long int tmp, min = ULONG_MAX;
	char *serveur, *ping, *best_line, *adrs, *port, *saveptr, *aFree;
	FILE *fe;
	fe = fopen("time", "r+");
	
	if ((serveur = malloc(STR_SIZE)) == NULL)
		rerror("malloc");
	if ((best_line = malloc(STR_SIZE)) == NULL)
		rerror("malloc");
	
	//boucle pour trouver (ou non) le serveur dans time
	while (getline(&serveur, &n, fe) > 0) {
		aFree = token_master(serveur, &adrs, &ping, &port, &saveptr);
		tmp = atoll(ping);
		if (tmp < min && file_search(fd, port) == 0) {
			min = tmp;
			strcpy(best_line, serveur);
		}
		free(aFree);
	}
	fclose(fe);
	if (min == ULONG_MAX) {	//si il a pas été trouvé on renvoit NULL
		free(best_line);
		return NULL;
	}
	strcpy(serveur, best_line);
	aFree = token_master(serveur, &adrs, &ping, &port, &saveptr);
	snprintf(best_line, STR_SIZE, "%s | %s", adrs, port);
	free(serveur);
	free(aFree);
	return best_line;	//sinon on renvoit "adrs | port"
}

//Fonction tourniquet pour suite() du fichier sender.c
char *tourniquet_suite(char *buff, int flush) {
	static char **str = NULL;	//string static inspire de strtok
	static int max = 0, compt = 0;
	if (buff != NULL) {	//initialisation
		int i;
		if (str != NULL) {	//seul moyen de free str (static) sans mem leak
			for (i = 0; i < max; i++)
				free(str[i]);
			free(str);
		}
		char copy[STR_SIZE], *saveptr, *tmp;
		strncpy(copy, buff, STR_SIZE);
		compt = 0;
		max = 0;
		
		strtok_r(copy, "|", &saveptr);
		for (i = 0; i < 3; i++)
			strtok_r(NULL, "|", &saveptr);
		
		//on compte le nombre de serveurs a visiter
		for (i = 0; i < (int)strlen(buff); i++) {
			if (buff[i] == '|') max++;
		}
		max-=3;	//il y a deja 4 | de base donc au min on a 1 adresse a visiter
		if ((str = malloc(sizeof(*str) * max)) == NULL) rerror("malloc");
		for (i = 0; i < max; i++) {
			if ((str[i] = malloc(STR_SIZE)) == NULL) rerror("malloc");
			tmp = strtok_r(NULL, "|", &saveptr);
			strncpy(str[i], tmp, STR_SIZE);
		}
		return NULL;
	} else if (compt >= max || flush == 1) {	//free quand finit
		int i;
		if (str != NULL) {
			for (i = 0; i < max; i++) {
				if (str[i] != NULL) free(str[i]);
			}
		}
		return NULL;
	} else {	//utilisation classique
		int mini = -1, i;
		long long int recup, min = -1;
		char copy[STR_SIZE], *port, *saveptr;
		//on determine le serveur avec le ELO le plus bas
		for (i = 0; i < max; i++) {
			if (str[i] != NULL) {
				strncpy(copy, str[i], STR_SIZE);
				strtok_r(copy, ", ", &saveptr);
				strtok_r(NULL, ", ", &saveptr);
				port = strtok_r(NULL, ", ", &saveptr);
				recup = recup_temps(port);	
				if (min == -1 || min > recup) {
					min = recup;
					mini = i;
				}
			}
		}
		if (mini == -1) return NULL;
		char *retour;
		retour = str[mini];
		str[mini] = NULL;
		return retour;
	}
}

//Fonction qui retrouve le ELO ou non d'un serveur
long long int recup_temps(char *port) {
	FILE *fd;
	long long int temps = 0;
	size_t n = STR_SIZE;
	char *read, *port2, *saveptr, *horo, *aFree;
	if ((read = malloc(STR_SIZE)) == NULL) rerror("malloc");
	fd = fopen("time", "r");
	while (getline(&read, &n, fd) > 0) {
		aFree = token_master (read, NULL, &horo, &port2, &saveptr);
		if (strncmp(port, port2, strlen(port)) == 0) {
			temps = atoll(horo);
			free(aFree);
			free(read);
			fclose(fd);
			return temps;
		}
		free(aFree);
	}
	free(read);
	fclose(fd);
	return temps;
}


int file_search(FILE *fd, char *port) {
	size_t n = STR_SIZE;
//l'adresse ne change pas donc au bout d'un moment ... mais c'est pas plus complique
	char *portloc, *saveptr, *serveur, *aFree;
	if ((serveur = malloc(STR_SIZE)) == NULL) rerror("malloc");
	rewind(fd);	//yaaa it's rewind time
	while (getline(&serveur, &n, fd) > 0) {
		aFree = token_master(serveur, NULL, &portloc, NULL, &saveptr);
		if (portloc != NULL && port != NULL && strlen(portloc) > 1 && strlen(port) > 1 && strcmp(portloc, port) == 0) {
			free(aFree);
			free(serveur);
			return 0;
		}
		free(aFree);
	}
	free(serveur);
	return 1;
}


char *token_master (char *buff, char **adrs, char **horodatage, char **port,
char **saveptr) {
	char *copy;
	if ((copy = malloc(STR_SIZE)) == NULL) rerror("malloc");
	strncpy(copy, buff, STR_SIZE);
	if (adrs != NULL) *adrs = strtok_r(copy, " | ", saveptr);
	else strtok_r(copy, " | ", saveptr);
	if (horodatage != NULL) *horodatage = strtok_r(NULL, " | ", saveptr);
	else strtok_r(copy, " | ", saveptr);
	if (port != NULL) *port = strtok_r(NULL, " | ", saveptr);
	else strtok_r(copy, " | ", saveptr);
	return copy;
}

//Ecrit en mode append dans le fichier 'time' les temps mis par les serveurs pour repondre
int register_time(char *buff, char *serv, int crash) {
	if (buff == NULL || serv == NULL) rerror("register_time NULL args");
	int i = 0;
	size_t n = 0;
	long long int interval;
	char *adrs, *horo_str, *port, to_write[STR_SIZE], *saveptr, *aFree, *aFree2;
	struct timeval horodatage;
	FILE *fd;
	
	aFree = token_master(buff, NULL, &horo_str, NULL, &saveptr);
	CHECK_NOER(gettimeofday(&horodatage, NULL), -1);
	interval = horodatage.tv_sec*1000 + horodatage.tv_usec/1000;	//en ms
	
	if (crash == 0) {
		if (horo_str != NULL)
			interval -= atoll (horo_str);
		interval = pow(interval, 2);
/*peut-etre mettre au carre est un peu fort on met la difference au carré pour
accentuer l'importance du ping dans le choix de serveur et se rapprocher de
la realite*/
		interval += horodatage.tv_sec*1000 + horodatage.tv_usec/1000;
/*on ne stock pas la difference de temps comme on l'imaginerais mais on lui
donne un systeme de elo relatif au temps ; on prendra le min a reveiller.
Ainsi on l'info sur un timeout a reessayer, un ping court ou long,
ou une erreur serveur et permet de faire tourner les serveur
comme un briquet en soiree*/
	} else if (crash == 2) {
		interval += 100; //on rajoute 10²ms au elo
	} else {
		interval += 10000;	//on rajoute 100²ms au elo
	}
	free(aFree);
	aFree = token_master(serv, &adrs, &port, NULL, &saveptr);
	if (port[strlen(port)-1] == '\n')
		snprintf(to_write, STR_SIZE, "%s | %llu | %s", adrs, interval, port);
	else
		snprintf(to_write, STR_SIZE, "%s | %llu | %s\n", adrs, interval, port);
	
	char *adrs2, *horo_str2, *port2, *saveptr2, *serveur = NULL;
	fd = fopen("time", "r+");
	while (getline(&serveur, &n, fd) > 0) {	//on cherche le serveur
		aFree2 = token_master(serveur, &adrs2, &horo_str2, &port2, &saveptr2);
		if (serveur[0] != '\n' && strcmp(adrs2, adrs) == 0 &&
		strncmp(port2, port, strlen(port2)-1) == 0) {
			replace_line(i, to_write);
			//si il est trouve, remplacer une ligne est une operation speciale
			free(aFree);
			free(serveur);
			free(aFree2);
			fclose(fd);
			return 0;
		}
		free(aFree2);
		i++;
	}
	free(aFree);
	free(serveur);
	
	fwrite(to_write, strlen(to_write), 1, fd);
	fclose(fd);
	
	return 0;
}

int replace_line(int nbline, char *to_put) {
	size_t n = STR_SIZE;
	char *str;
	str = malloc(STR_SIZE);
	FILE *fd, *fe;
	fd = fopen("time", "r");
	fe = fopen(".time", "w");
	
	int i;
	for (i = 0; i < nbline; i++) {
		getline(&str, &n, fd);
		fwrite(str, strlen(str), 1, fe);
	}
	getline(&str, &n, fd);	//ligne a supprimer
	
	fwrite(to_put, strlen(to_put), 1, fe);
	
	while (!feof(fd)) {
		memset(str, '\0', STR_SIZE);
		fread(str, (size_t) STR_SIZE, 1, fd);
		fwrite(str, strlen(str), 1, fe);
	}
	
	fclose(fd);
	fclose(fe);
	remove("time");
	rename(".time", "time");
	
	free(str);
	return 0;
}
