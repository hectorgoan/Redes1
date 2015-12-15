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
#define PORT 8888   //The port on which to listen for incoming data
 
void manejaErrores(char *s);
void manejaPaqueteRecibido(char *ip, int puerto, char* datos);
void INThandler(int sig);

//::::::::::::::::::::::::::::::::::::::://
//          PROGRAM'S BODY
//::::::::::::::::::::::::::::::::::::::://
int main(void)
{
    //-----------------------------------//
    //      VARIABLE DECLARATION
    //-----------------------------------//
    struct sockaddr_in server_si, client_si;
     
    int sock_fd;
    int i;
    int sock_len = sizeof(client_si);
    int recv_len;
    char buf[BUFLEN];

    //----------------------------------//
    //             BODY
    //----------------------------------//     
    signal(SIGINT, INThandler); //Will detect SIGINT (ctrl+c)

    if ((sock_fd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        manejaErrores("Error en socket()");
    }
     
    
    memset((char *) &server_si, 0, sizeof(server_si));
     
    server_si.sin_family = AF_INET;
    server_si.sin_port = htons(PORT);
    server_si.sin_addr.s_addr = htonl(INADDR_ANY);
     
    
    if( bind(sock_fd , (struct sockaddr*)&server_si, sizeof(server_si) ) == -1)
    {
        manejaErrores("Error en bind()");
    }
    
    while(1)
    {
        printf("Esperando a recibir datos...");
        fflush(stdout); //Cleaning purposes
         
        
        if ((recv_len = recvfrom(sock_fd, buf, BUFLEN, 0, (struct sockaddr *) &client_si, &sock_len)) == -1)
        {
            manejaErrores("Error en recvfrom()");
        }
         
        manejaPaqueteRecibido(inet_ntoa(client_si.sin_addr), ntohs(client_si.sin_port), buf);
        
        //The server is just sending back the same message that he've received
        if (sendto(sock_fd, buf, recv_len, 0, (struct sockaddr*) &client_si, sock_len) == -1)
        {
            manejaErrores("Error en sendto()");
        }
    }
 
    close(sock_fd);
    return 0;
}
void  INThandler(int sig)   
{
    //::::::::::::::::::::::::::::::::::::::::::::
    //This function handles when ctrl+c is pushed
    //::::::::::::::::::::::::::::::::::::::::::::
    printf("\n ctrl+c pulsado \n Adios!\n");

    exit (0); 
}

void manejaErrores(char *msgToShow)
{
    //::::::::::::::::::::::::::::::::::::::::::::::::
    //This function handles errors (thanks mr.obious)
    //::::::::::::::::::::::::::::::::::::::::::::::::
    perror(msgToShow);
    exit(1);
}
void manejaPaqueteRecibido(char *ip, int puerto, char* datos)
{
    //:::::::::::::::::::::::::::::::::::::::::::::::::
    //This function handles when a package is received
    //:::::::::::::::::::::::::::::::::::::::::::::::::
    printf("\n-------------------\n");
    printf("Paquete recibido, ip: %s puerto: %d\n", ip, puerto);
    printf("Datos: %s\n" , datos);
    printf("--------------------\n");
}
