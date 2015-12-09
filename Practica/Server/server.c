//#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define PORT 35400
#define BUFFERSIZE 141

void error(const char* msg);
void daemon(void);
void session(int socket, struct sockaddr_in cliAddr);

int main(void)
{    
    setpgrp();
    
    switch(fork())
    {
        case -1:
            error("ERROR en fork.");
            exit(1);
            break;
        case 0:
            daemon();
            exit(0);
            break;
        default:
            exit(0);
    }
}

void error(const char* msg)
{
    perror(msg);
    exit(1);
}


void daemon(void)
{  
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    
    struct sigaction sa = {.sa_handler = SIG_IGN};
    sigaction(SIGCHLD, &sa, NULL);
    
    int sockfd, sckTCP;
    
    struct sockaddr_in servAddr, cliAddr;
    bzero(&servAddr, sizeof(servAddr));
    bzero(&cliAddr, sizeof(servAddr));
    
    sockfd = socket(AF_INET, SOCK_STREAM, SOCK_NONBLOCK);
    if(sockfd == -1)
    {
        error("ERROR de apertura de socket");
    }
    
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(PORT);
    if(bind(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1)
    {
        error("ERROR al enlazar el socket.");
    }
   
    listen(sockfd, 1024);
    
    while(1)
    {
        socklen_t cliLen = sizeof(cliAddr);
        sckTCP = accept(sockfd, (struct sockaddr*)&cliAddr, &cliLen);
        if(sckTCP == -1)
        {
            error("ERROR al aceptar conexion.");
        }
        switch(fork())
        {
            case -1:
                error("ERROR en fork.");
                break;
            case 0:
                close(sockfd);
                session(sckTCP, cliAddr);
                exit(0);
                break;
            default:
                close(sckTCP);
        }
    }
    close(sockfd);
    return 0;
}

void session(int socket, struct sockaddr_in cliAddr)
{
    char buffer[BUFFERSIZE];
    bzero(buffer, BUFFERSIZE);
    
    int buffLen;
    long lTime;
    
    char hostname[128];
    
    FILE* log = fopen("./fichajes.log", "a");
    
    getnameinfo((struct sockaddr*)&cliAddr,
                 sizeof(cliAddr),
                 hostname,
                 sizeof(hostname),
                 NULL,
                 0,
                 NULL);
    
    inet_ntop(AF_INET, &(cliAddr.sin_addr), hostname, sizeof(hostname));
    
    time(&lTime);
    
    fprintf(log, "Startup from %s port %u at %s",
            hostname,
            ntohs(cliAddr.sin_port),
            (char*)ctime(&lTime));
    
    if(buffLen = recv(socket, buffer, BUFFERSIZE, NULL) == -1)
    {
        error("ERROR al recibir.");
    }
    
    buffer[BUFFERSIZE] = '\0';
    
    fprintf(log, "Mensaje del cliente: %s\n", buffer);
    
    fclose(log);
    close(socket);
}