#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define PORT 35400
#define BUFFERSIZE 141

void error(const char* msg);
void daemonFn(void);
void session(int socket, struct sockaddr_in cliAddr);

void logg(const char* string);
int validateCmd(const char* command, char* result);
void handler_hola(const char* command);
void str_cut(char* string, char cutter);

int main(void)
{
    setpgid(0, 0);
    switch(fork())
    {
        case -1:
            error("ERROR en fork.");
            exit(1);
            break;
        case 0:
            daemonFn();
            exit(0);
            break;
        default:
            sleep(60);
            exit(0);
    }
}

void error(const char* msg)
{
    perror(msg);
    exit(1);
}


void daemonFn(void)
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
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
    
    socklen_t cliLen = sizeof(cliAddr);
    while(1)
    {
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
    return;
}

void session(int socket, struct sockaddr_in cliAddr)
{
    char buffer[BUFFERSIZE];
    bzero(buffer, BUFFERSIZE);
    
    size_t buffLen;
    long lTime;
    
    char hostname[128];
    char* ip;
    char log[128];
    
    getnameinfo((struct sockaddr*)&cliAddr,
                 sizeof(cliAddr),
                 hostname,
                 sizeof(hostname),
                 NULL,
                 0,
                 0);
    
    inet_ntop(AF_INET, &(cliAddr. sin_addr), hostname, sizeof(hostname));
    ip = inet_ntoa(cliAddr.sin_addr);
    
    time(&lTime);
    
    sprintf(log, "Startup from %s: %s. Port %u at %s",
            hostname,
            ip,
            ntohs(cliAddr.sin_port),
            (char*)ctime(&lTime));
    logg(log);
    
    if(buffLen = recv(socket, buffer, BUFFERSIZE, 0) == -1)
    {
        error("ERROR al recibir.");
    }
    
    buffer[BUFFERSIZE] = '\0';
    
    switch (validateCmd(buffer, NULL))
    {
        case 1: //HOLA
            str_cut(buffer, " ");
            handler_hola(buffer);
            break;
        case 2: //LISTAR
            break;
        case 3: //LISTAR EVENTOS
            break;
        case 4: //FICHAR
            break;
        case 5: //ADIOS
            break;
        default: //ERROR
            send(socket, "ERROR Invalid command.\n", 23, NULL);
    }
    close(socket);
}

void logg(const char* string)
{
    FILE* log = fopen("./fichajes.log", "a");
    if(log == NULL)
    {
        error("ERROR file open.");
        return;
    }
    
    fprintf(log, "%s\n", string);
    
    fclose(log);
}

int validateCmd(const char* command, char* result)
{
    if(strncmp(command, "HOLA", 4) == 0)
    {
        if(result != NULL)
        {
            result = "HOLA";
        }
        return 1;
    }
    else if(strncmp(command, "LISTAR", 6))
    {
        if(result != NULL)
        {
            result = "LISTAR";
        }
        return 2;
    }
    else if(strncmp(command, "LISTAR EVENTOS", 14))
    {
        if(result != NULL)
        {
            result = "LISTAR EVENTOS";
        }
        return 3;
    }
    else if(strncmp(command, "FICHAR", 6))
    {
        if(result != NULL)
        {
            result = "FICHAR";
        }
        return 4;
    }
    else if(strncmp(command, "ADIOS", 5))
    {
        if(result != NULL)
        {
            result = "ADIOS";
        }
        return 5;
    }
    else
    {
        result = "";
        return 0;
    }
}

void handler_hola(const char* command)
{
    if(isUserValid(command))
    {
        send(socket, "CORRECTO\n", 9, NULL);
    }
    else
    {
        send(socket, "ERROR Invalid user.\n", 20, NULL);
    }
}

int isUserValid(const char* user)
{
    FILE* users = fopen("./usuarios.txt", "r");
    if(users == NULL)
    {
        error("ERROR file open.");
        return -1;
    }
    
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    
    while((read = getline(&line, &len, users)) != -1)
    {
        if(strcmp(read, user) == 0)
        {
            return 1;
        }
    }
    
    fclose(users);
    if(line)
    {
        free(line);
    }
    return 0;
}


//Cut the string for the first second param: ("This is a example.", " ") -> "is a example."
void str_cut(char* string, char cutter)
{
    char* pch;
    pch = strchr(string, cutter);
    strncpy(pch, string);
}