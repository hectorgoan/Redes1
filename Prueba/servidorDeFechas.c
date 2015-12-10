//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//				INCLUDE's & DEFINE's SECTION
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 

#define PORT 5030

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//						PROGRAM's BODY
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
void main(void)
{
	//Welcome message
	//system ("tput clear");
	printf("Hola!, soy el servidor, vivo aquí\n");
	
	//Variable declaration zone
	int serverfd = 0;	//Will store server's file descriptor
	int clientfd = 0;	//Will store client's file descriptor
    struct sockaddr_in serv_addr;	//Will store server's address info
    struct sockaddr_in client_addr;	//Will store client's address info

    char sendBuff[1025];	//Will be sent to the client
    time_t ticks;			//Will be used to build the response message

    int sin_size;

    //Function's body
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
    	printf("Error en la llamada a socket()\n");
    	exit (-1);
    }


    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT); 

    bind(serverfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(serverfd, 10); 

    while(1)
    {

    	sin_size = sizeof(struct sockaddr_in);
    	if ((clientfd = accept(serverfd, (struct sockaddr*)&client_addr, &(sin_size))) == -1)
    	{
    		printf("Error al aceptar una llamada entrante\n");
    		exit (-1);
    	}

        ticks = time(NULL);

        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        write(clientfd, sendBuff, strlen(sendBuff)); 

        close(clientfd);
        sleep(1);
    }	

	

}