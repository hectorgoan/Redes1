#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void custom_handler(int mySignal);

int main(int argc, char const *argv[])
{
	int pidhijo, pidpapa;	

	int pid;
	setpgrp();
	pid = fork();
	switch(pid)
	{
		case -1:	//problem with fork
			printf("Algun problema ocurrió en fork()\n");
		break;
		
		case 0:		//I'm the child (a daemon wannabee)
		
			if (signal(SIGUSR1, custom_handler) == SIG_ERR) 
			{
				perror("error en la señal SIGUSR1");
				exit(EXIT_FAILURE);
			}
			if (signal(SIGUSR2, custom_handler) == SIG_ERR) 
			{
				perror("error en la señal SIGUSR2");
				exit(EXIT_FAILURE);
			}
			if (signal(SIGTERM, custom_handler) == SIG_ERR) 
			{
				perror("error en SIGTERM");
				exit(EXIT_FAILURE);
			}
			if (signal(SIGINT, custom_handler) == SIG_ERR) 
			{
				perror("error en SIGINT");
				exit(EXIT_FAILURE);
			}

			pidhijo = getpid();

			printf("Soy el hijo, de pid %d\n", pidhijo);

			while(1)
			{
				//Simulates what the child'll be doing
			}	

			printf("Soy el hijo y he acabado\n");

		break;
		
		default:	//I AM YOUR FATHER

			pidpapa = getpid();
			printf("Soy el padre, de pid %d\n", pidpapa);

			sleep(5);	//SIMULATES WHAT THE FATHER'LL BE DOING

			printf("Soy el padre y he acabado\n");

		break;
	}

	return 0;
}

void custom_handler(int mySignal)
{
	switch (mySignal)
	{
	case SIGUSR1:
	    printf("He recibido la señal SIGUSR1\n");
	    printf("Fin de la ejecucion\n");
	    break;
    case SIGUSR2:
	    printf("He recibido la señal SIGUSR2\n");
	    printf("Fin de la ejecucion\n");
	    break;
	case SIGINT:
		printf("He recibido la señal SIGINT\n");
		printf("Fin de la ejecucion\n");
		break;
    case SIGTERM:
    	printf("He recibido la señal SIGTERM\n");
	    printf("Fin de ejecucion\n");
	    
	    exit(EXIT_SUCCESS);
    }
}