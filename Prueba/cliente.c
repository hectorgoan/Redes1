/*
** Fichero: cliente.c
** Autores: 
** Héctor Gonzalo Andrés DNI 71038384D
** Néstor 
*/
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//				INCLUDE's & DEFINE's SECTION
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include "mylib.h"

#define PORT 5030
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//						PROGRAM's BODY
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
int main(int argc, char const *argv[])
{
	//Welcome message
	printf("Hola!, soy el cliente, vivo aquí\n");

	//Variable declaration zone
	int sockfd = 0;	//Used to store sock's file descriptor
	int n = 0;		//Used to read recvBuff
    char recvBuff[1024];
    struct sockaddr_in serv_addr;	//Used to store server's address info

    //Function's body
    if(argc != 2)
    {
        printf("Error en los argumentos, asegúrese de indicar el puerto");
        return (-1);
    } 
    printf("Usando la dirección: %s ",argv[1]);
    printf("y el puerto %d\n", PORT);

    
    memset(recvBuff, '0',sizeof(recvBuff));
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error en la llamada a socket() \n");
        return (-1);
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    //Filling serv_addr info
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); 

    //Converts the IP (character string) into a network address structure (AF_INET type) and then copies it to (&serv_addr.sin_addr)
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)	
    {
        printf("Error en inet_pton()\n");
        return (-1);
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("Error en connect() \n");
       return (-1);
    } 

    //Reading the buffer
    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("Error en fputs()\n");
            return (-1);
        }
    } 

    if(n < 0)
    {
        printf("Error de lectura en el buffer \n");
        return (-1);
    } 

    return 0;

}
