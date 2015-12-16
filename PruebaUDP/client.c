//::::::::::::::::::::::::::::::::::::::://
//       INCLUDES, DEFINES & HEADERS
//::::::::::::::::::::::::::::::::::::::://

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>


#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to send data
 
void manejaErrores(char *s);
void manejaPaqueteRecibido(char* datos);
 
//::::::::::::::::::::::::::::::::::::::://
//          PROGRAM'S BODY
//::::::::::::::::::::::::::::::::::::::://
int main(int argc, char const *argv[])
{
    //-----------------------------------//
    //      	FIRST TEST
    //-----------------------------------//
	if(argc != 2)
    {
        printf("Error en los argumentos, asegúrese de indicar el puerto");
        return (-1);
    } 
    printf("Usando la dirección: %s ",argv[1]);
    printf("y el puerto %d\n", PORT);
    
    //-----------------------------------//
    //      VARIABLE DECLARATION
    //-----------------------------------//
    struct sockaddr_in server_si;
    int sock_fd;
    int i;
    int sock_len=sizeof(server_si);
    char buf[BUFLEN];
    char message[BUFLEN];

    //-----------------------------------//
    //      		BODY
    //-----------------------------------//
 
    if ( (sock_fd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        manejaErrores("Error en socket()");
    }
 
    memset((char *) &server_si, 0, sizeof(server_si));
    server_si.sin_family = AF_INET;
    server_si.sin_port = htons(PORT);
     
    if (inet_aton(argv[1] , &server_si.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
 
    while(1)
    {
        printf("Enter message : ");
        gets(message);
         
        //send the message
        if (sendto(sock_fd, message, strlen(message) , 0 , (struct sockaddr *) &server_si, sock_len)==-1)
        {
            manejaErrores("Error en sendto()");
        }
         
        //receive a reply and print it
        //clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFLEN);
        //try to receive some data, this is a blocking call
        if (recvfrom(sock_fd, buf, BUFLEN, 0, (struct sockaddr *) &server_si, &sock_len) == -1)
        {
            manejaErrores("Error en recvfrom()");
        }
         
        manejaPaqueteRecibido(buf);
    }
 
    close(sock_fd);
    return 0;
}

void manejaErrores(char *msgToShow)
{
    //::::::::::::::::::::::::::::::::::::::::::::::::
    //This function handles errors (thanks mr.obious)
    //::::::::::::::::::::::::::::::::::::::::::::::::
    perror(msgToShow);
    exit(1);
}
void manejaPaqueteRecibido(char* datos)
{
    //:::::::::::::::::::::::::::::::::::::::::::::::::
    //This function handles when a package is received
    //:::::::::::::::::::::::::::::::::::::::::::::::::
    printf("\n----\n");
    printf("Datos: %s\n" , datos);
    printf("-----\n");

}