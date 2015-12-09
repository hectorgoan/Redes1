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
#include <netdb.h>		//Needed 4 the hostent structure
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//						DEFINE's SECTION
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
#define PORT 3550		//Port that'll be open in the remote node
#define MAXDATASIZE 100	//in bytes

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//						PROGRAM's BODY
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
int main(int argc, char const *argv[])
{

	int fd, numBytes;

	char buffer[MAXDATASIZE];	//Where we'll store received data

	struct hostent *he;	//Struct that'll receive remote node's info
	struct sockaddr_in server;	//Will store server's direction

	if (argc == 2)	//Our program only needs one argument (the ip) 
	{
		printf("Uso: %s <Direccion IP>\n", argv[1]);
		exit (-1);
	}

	if ((he = gethostbyname(argv[1]))==NULL)
	{
		printf("gethostbyname error\n");
		exit(-1);
	}

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket() error\n");
	}
	//Filling server info
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	printf("%d\n", server.sin_port);
	//DESDE AQUI COPYPASTE
	server.sin_addr= *((struct in_addr *)he->h_addr);	/*he->h_addr pasa la información de ``*he'' a "h_addr" */
	bzero(&(server.sin_zero),8);

	if(connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr))==-1)
	{
		/* llamada a connect() */
		printf("connect error\n");
		exit(-1);
	}
	if ((numBytes=recv(fd,buffer,MAXDATASIZE,0)) == -1)
	{
   		/* llamada a recv() */
		printf("Error en recv() \n");
		exit(-1);
	}
	buffer[numBytes]='\0';

	printf("Mensaje del Servidor: %s\n",buffer);
   /* muestra el mensaje de bienvenida del servidor =) */
   close(fd);   /* cerramos fd =) */


}
