//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//						INCLUDE's SECTION
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
#include <stdio.h>
#include <netinet/in.h>	//Needed 4 inet_ntoa()
#include <arpa/inet.h>	//Needed 4 inet_ntoa()
#include <sys/types.h>	//Needed 4 socket(), bind(), connect(), listen(), accept(), send(), recv() & recvfrom()
#include <sys/socket.h>	//Needed 4 socket(), bind(), connect(), listen(), accept(), send(), recv(), recvfrom() & shutdown()
#include <unistd.h>		//Needed 4 close() & gethostname()
#include <stdlib.h>		//Needed 4 exit()
#include <strings.h>	//Needed 4 bzero()

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//						DEFINE's SECTION
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
#define PORT 3550	//Port that'll be opened
#define BACKLOG 2	//Maximum lenght to wich the queue of pending 
					//connections for fdServer may grow

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//						PROGRAM's BODY
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
void main(void)
{
	int fdServer;	//Server socket file descriptor 
	int fdClient;	//Client socket file descriptor

	struct sockaddr_in server;	//Server's direction info
	struct sockaddr_in client;	//Client's directioin info

	int sin_size = sizeof(struct sockaddr_in);

	if ((fdServer = socket(AF_INET, SOCK_STREAM, 0)) == -1)	//Obtaining TCP socket's file 
	{														//descriptor (and checking for errors)
		printf("Error en la creacción del descriptor de fichero del socket del server\n");
		exit(-1);
	}
	//Filling server info
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;	//Shouldn't it be 127.0.0.1 ???
	bzero(&(server.sin_zero), 8);

	//Let's bind
	if (bind(fdServer, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) == -1)
	{
		printf("Error en bind()\n");
		exit(-1);
	}

	//Let's listen
	if (listen(fdServer, BACKLOG) == -1)
	{
		printf("Error en listen()\n");
		exit(-1);
	}

	printf("Esperaré conexiones entrantes\n");
	while(1)
	{
		if ((fdClient = accept(fdServer, (struct sockaddr*)&client, &sin_size)) == -1)
		{
			printf("Error en accept\n");
			exit(-1);
		}
		printf("Conexión entrante desde %s\n", inet_ntoa(client.sin_addr));	//Will show client's IP

		send(fdClient, "Bienvenido a mi server salao! \n", 31, 0);

		close(fdClient);		

	}

}