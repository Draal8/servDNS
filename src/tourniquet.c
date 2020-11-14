#include "../head/tourniquet.h"

/*int main() {
	static long int trans_id = 1;
	unsigned long long int tim;
	char *msg;
	struct timeval horodatage;
	
	if ((msg = malloc(STR_SIZE)) == NULL) return 1;
	memset(msg, '\0',STR_SIZE);
	CHECK_NOER(gettimeofday(&horodatage, NULL), 1);
	tim = horodatage.tv_sec*1000 + horodatage.tv_usec/1000;
	
	snprintf(msg, STR_SIZE, "127.0.0.1 | %llu | 3501", tim);
	register_time(msg);
	
	return 0;
}*/

char *tourniquet(FILE *fd) {
	size_t n = STR_SIZE;
	unsigned long int tmp, min = ULONG_MAX; // 1 sec de delai c'est enorme
	char *serveur, *ping, *best_line, *adrs, *port, *saveptr, *aFree;
	FILE *fe;
	fe = fopen("time", "r+");
	
	if ((serveur = malloc(STR_SIZE)) == NULL)
		rerror("malloc");
	if ((best_line = malloc(STR_SIZE)) == NULL)
		rerror("malloc");
	
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
	if (min == ULONG_MAX) {
		free(best_line);
		return NULL;
	}
	strcpy(serveur, best_line);
	aFree = token_master(serveur, &adrs, &ping, &port, &saveptr);
	snprintf(best_line, STR_SIZE, "%s | %s", adrs, port);
	free(serveur);
	free(aFree);
	return best_line;
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

//Ecrit en mode append dans le fichier 'time' les temps mis par les serveurs pour repondre
char *token_master (char *buff, char **adrs, char **horodatage, char **port, char **saveptr) {
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

int register_time(char *buff, char *serv, int crash) {
	if (buff == NULL || serv == NULL) rerror("register_time NULL args");
	int i = 0;
	size_t n = 0;
	long long int interval;
	char *adrs, *horo_str, *port, to_write[STR_SIZE], *saveptr, *aFree, *aFree2;
	struct timeval horodatage;
	FILE *fd;
	
	
	
	aFree = token_master(buff, NULL, &horo_str, NULL, &saveptr);
	//sleep(1); utiliser nanosleep a la place
	CHECK_NOER(gettimeofday(&horodatage, NULL), -1);
	interval = horodatage.tv_sec*1000 + horodatage.tv_usec/1000;
	
	if (crash == 0) {
		if (horo_str != NULL)
			interval -= atoll (horo_str);
		interval = pow(interval, 2);	//peut-etre mettre au carre est un peu fort
		//on met la difference au carré deja pour regler les problemes de changement de jour
		//et pour accentuer l'importance du ping dans le choix de serveur et se rapprocher de la realite
		interval += horodatage.tv_sec*1000 + horodatage.tv_usec/1000;
		//on ne stock pas la difference de temps comme on l'imaginerais mais on lui donne un systeme de elo relatif 
		//au temps ; on prendra le min a reveiller. Ainsi on l'info sur un timeout a reessayer, un ping court ou long,
		//ou une erreur serveur et permet de faire tourner les serveur comme un briquet en soiree
	} else if (crash == 2) {
		interval += 10000000; //on rajoute 10²ms au elo
	} else {
		interval += 1000000000;	//on rajoute 100²ms au elo
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
		if (serveur[0] != '\n' && strcmp(adrs2, adrs) == 0 && strncmp(port2, port, strlen(port2)-1) == 0) {
			replace_line(i, to_write);
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
