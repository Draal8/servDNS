#ifndef H_COMMON
	#define H_COMMON

	#ifndef _GNU_SOURCE
		#define _GNU_SOURCE
	#endif

//basics
#include <stdlib.h>
#include <stdio.h>
//essentiels pour la fonction rerror et donc le controle d'erreur
#include <unistd.h>
#include <stdnoreturn.h> //compiler en C11
#include <errno.h>
#include <string.h>

//utile pour msleep
#include <time.h>

//macro de controle d'erreur
#define CHECK(op) do { if (op == -1) rerror(#op);} while(0)
#define CHECK_NOER(op, erno) do { if (op == -1) return(erno);} while(0)
#define STR_SIZE 4096

//code pas content : code crasher !
noreturn void rerror(char *str);

int msleep(long int ms);

#endif

