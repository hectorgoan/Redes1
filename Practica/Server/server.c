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

void handler_hola(const int socket, char* command);
void handler_fichar(const int socket, char* command);

void str_cut(char* string, const char cutter);
int isUserValid(const char* user);
int isUserInEvent(char* idEvent, char* idUser);
int validateDateForEvent(char* date);

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
    size_t buffLen;
    
    time_t tTime;
    
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
    
    time(&tTime);
    
    sprintf(log, "Startup from %s: %s. Port %u at %s",
            hostname,
            ip,
            ntohs(cliAddr.sin_port),
            (char*)ctime(&tTime));
    logg(log);
    
    if(buffLen = recv(socket, buffer, BUFFERSIZE, 0) == -1)
    {
        error("ERROR: Failed to receive.");
    }
    
    buffer[BUFFERSIZE] = '\0';
    
    switch (validateCmd(buffer, NULL))
    {
        case 1: //HOLA
            str_cut(buffer, ' ');
            handler_hola(socket, buffer);
            break;
        case 2: //LISTAR
            break;
        case 3: //LISTAR EVENTOS
            break;
        case 4: //FICHAR
            handler_fichar(socket, buffer);
            break;
        case 5: //ADIOS
            break;
        default: //ERROR
            send(socket, "ERROR Invalid command.\n", 23, 0);
    }
    close(socket);
}

void logg(const char* string)
{
    FILE* log = fopen("./fichajes.log", "a");
    if(!log)
    {
        error("ERROR: Open file.");
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

void handler_hola(const int socket, char* command)
{
    if(isUserValid(command))
    {
        send(socket, "CORRECTO\n", 9, 0);
    }
    else
    {
        send(socket, "ERROR Invalid user.\n", 20, 0);
    }
}

void handler_fichar(const int socket, char* command)
{
    char idEvent[40];
    char idUser[40];
    char date[40];
    char* pch;
    
    char baseCommand[BUFFERSIZE];
    strcpy(baseCommand, command);
    
    pch = strtok(command, " ");
    strcpy(idEvent, pch);
    
    str_cut(command, ' ');
    pch = strtok(command, " ");
    strcpy(idUser, pch);
    
    strcpy(command, baseCommand);
    str_cut(command, ' ');
    str_cut(command, ' ');
    strcpy(date, command);
    
    if(isUserInEvent(idEvent, idUser) && validateDateForEvent(date))
    {
        
    }
}

int isUserValid(const char* user)
{
    FILE* users = fopen("./usuarios.txt", "r");
    if(!users)
    {
        error("ERROR: Open file.");
        return -1;
    }
    
    char* pch;
    char line[150];
    
    while(fgets(line, 150, users) != NULL)
    {
        pch = strtok(line, "#");
        if(strcmp(pch, user) == 0)
        {
            fclose(users);
            return 1;
        }
    }
    
    fclose(users);
    return 0;
}


//Cut the string for the first second param: ("This is a example.", " ") -> "is a example."
void str_cut(char* string, const char cutter)
{
    char* pch;
    pch = strchr(string, cutter);
    strcpy(pch, string);
}

int isUserInEvent(char* idEvent, char* idUser)
{
    FILE* file = fopen("usuarios-eventos.txt", "r");
    if(!file)
    {
        error("ERROR: Open file.");
        return -1;
    }
    
    char* pch;
    char line[150];
    
    char user[40];
    char Event[40];
    
    while(fgets(line, 150, file) != NULL)
    {
        pch = strtok(line, "#");
        strcpy(user, pch);
        pch = strtok(NULL, "#");
        strcpy(idEvent, pch);
        
        if(strcmp(idEvent, Event) == 0 && strcmp(user, idUser) == 0)
        {
            fclose(file);
            return 1;
        }
    }
    
    fclose(file);
    return 0;
}

int validateDateForEvent(char* date)
{
    // 01/12/2015 14:00:00
    struct tm* parsedTime;
    int year, month;
    
    sscanf(date, "%d/%d/%d %d:%d:%d",
            parsedTime->tm_mday,
            &month,
            &year,
            parsedTime->tm_hour,
            parsedTime->tm_min, 
            parsedTime->tm_sec);
    
    parsedTime->tm_year = year - 1900;
    parsedTime->tm_mon = month -1;
    
    FILE* events = fopen("./eventos.txt", "r");
    if(!events)
    {
        error("ERROR: Open file.");
        return -1;
    }
    //Incomplete
    
    
    
    fclose(events);
    if(difftime())
}