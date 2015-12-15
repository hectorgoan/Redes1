#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char const *argv[])
{
	int pidhijo, pidpapa;	

	int pid;
	pid = fork();

	switch(pid)
	{
		case -1:	//problem with fork
			printf("Algun problema ocurrió en fork()\n");
		break;
		
		case 0:		//I'm the child (a daemon wannabee)
		
			signal(SIGINT, SIG_IGN);	//Will ignore ctrl+c, only way to kill him is by kill-9
			int papapid = getppid();
			kill(papapid, SIGKILL);
			pidhijo = getpid();

			printf("Soy el hijo, de pid %d\n", pidhijo);

			sleep(25);	//SIMULATES WHAT THE PARENT'LL BE DOING		

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
