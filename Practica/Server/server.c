#define _XOPEN_SOURCE /* glibc2 needs this */

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
void handler_listar(const int socket, char* command);
void handler_fichar(const int socket, char* command);

void str_cut(char* string, const char cutter);
int isUserValid(const char* user);
int isUserInEvent(char* idEvent, char* idUser);
int validateDateForEvent(char* idEvent, char* date);
time_t makeDateFromString(char* date);
void fichar(char* idEvent, char* idUser, char* date);

int main(void)
{
    setpgid(0, 0);
    /*switch(fork())
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
    }*/
    daemonFn();
}

void error(const char* msg)
{
    perror(msg);
    exit(1);
}


void daemonFn(void)
{
    //fclose(stdin);
    //fclose(stdout);
    //fclose(stderr);
    
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
        
        /*switch(fork())
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
        }*/
        session(sckTCP, cliAddr);
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
    
    int end = 0;
    while(!end)
    {
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
                str_cut(buffer, ' ');
                handler_listar(socket, buffer);
                break;
            case 3: //LISTAR EVENTOS
                str_cut(buffer, ' ');
                break;
            case 4: //FICHAR
                str_cut(buffer, ' ');
                handler_fichar(socket, buffer);
                break;
            case 5: //ADIOS
                str_cut(buffer, ' ');
                close(socket);
                end = 1;
                break;
            default: //ERROR
                send(socket, "ERROR Invalid command.\0", 23, 0);
        }
    }
}

void logg(const char* string)
{
    FILE* log = fopen("./fichajes.log", "a");
    if(!log)
    {
        error("ERROR: Open file.");
        return;
    }
    
    fprintf(log, "%s", string);
    
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
    else if(strncmp(command, "LISTAR", 6) == 0)
    {
        if(result != NULL)
        {
            result = "LISTAR";
        }
        return 2;
    }
    else if(strncmp(command, "LISTAR EVENTOS", 14) == 0)
    {
        if(result != NULL)
        {
            result = "LISTAR EVENTOS";
        }
        return 3;
    }
    else if(strncmp(command, "FICHAR", 6) == 0)
    {
        if(result != NULL)
        {
            result = "FICHAR";
        }
        return 4;
    }
    else if(strncmp(command, "ADIOS", 5) == 0)
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
        send(socket, "CORRECTO\0", 9, 0);
    }
    else
    {
        send(socket, "ERROR Invalid user.\0", 20, 0);
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
    pch = strtok(NULL, " ");
    strcpy(idUser, pch);
    pch = strtok(NULL, " ");
    strcpy(date, pch);
    
    char log[50];
    time_t tTime;
    time(&tTime);
    if(isUserInEvent(idEvent, idUser) && validateDateForEvent(idEvent, date))
    {
        fichar(idEvent, idUser, date);
        send(socket, "CORRECTO\0", 9, 0);
        sprintf(log, "CORRECT \"FICHAR\" from %s in event %s at %s",
                idUser,
                idEvent,
                (char*)ctime(&tTime));
        logg(log);
    }
    else
    {
        send(socket, "ERROR Invalid user or date.\0", 28, 0);
        sprintf(log, "ERROR \"FICHAR\" from %s in event %s at %s",
                idUser,
                idEvent,
                (char*)ctime(&tTime));
        logg(log);
    }
}

void handler_listar(const int socket, char* command)
{
    
}

int isUserValid(const char* user)
{
    FILE* users = fopen("./usuarios.txt", "r");
    if(!users)
    {
        error("ERROR: Open file.");
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
    if(pch == NULL)
    {
        error("NULL pointer.");
    }
    strcpy(string, pch + 1);
}

int isUserInEvent(char* idEvent, char* idUser)
{
    char* pch;
    char line[150];
    
    char user[40];
    char event[40];
    
    FILE* file = fopen("usuarios-eventos.txt", "r");
    if(!file)
    {
        error("ERROR: Open file.");
    }
    
    while(fgets(line, 150, file) != NULL)
    {
        pch = strtok(line, "#");
        strcpy(user, pch);
        pch = strtok(NULL, "#");
        strcpy(event, pch);
        pch = strchr(event, '\r');
        if(pch != NULL)
        {
            strncpy(pch, "\0", 1);
        }
        
        if(strcmp(idEvent, event) == 0 && strcmp(idUser, user) == 0)
        {
            fclose(file);
            return 1;
        }
    }
    
    fclose(file);
    return 0;
}

int validateDateForEvent(char* idEvent, char* date)
{
    char* pch;
    char line[150];
    
    FILE* events = fopen("./eventos.txt", "r");
    if(!events)
    {
        error("ERROR: Open file.");
    }
    
    while(fgets(line, 150, events) != NULL)
    {
        pch = strtok(line, "#");
        if(strcmp(pch, idEvent) == 0)
        {
            break;
        }
    }
    
    if(feof(events))
    {
        fclose(events);
        return 0;
    }
    fclose(events);
    
    char initDate[25];
    char endDate[25];

    //pch = strtok(line, "#");
    pch = strtok(NULL, "#");
    pch = strtok(NULL, "#");
    strcpy(initDate, pch);
    pch = strtok(NULL, "#");
    strcpy(endDate, pch);
    pch = strchr(endDate, '\r');
    if (pch != NULL)
    {
        strncpy(pch, "\0", 1);
    }
    
    time_t ParseDate = makeDateFromString(date);
    
    return difftime(ParseDate, makeDateFromString(initDate)) > 0
           && difftime(makeDateFromString(endDate), ParseDate) > 0 ?
                1 : 0;
}

time_t makeDateFromString(char* date)
{
    char* pch;
    pch = strchr(date, ' ');
    if(pch != NULL)
    {
        strncpy(pch, ";", 1);
    }
    // 21/12/2015 14:00:00
    struct tm parsedTime;
    strptime(date, "%d/%m/%Y;%H:%M:%S", &parsedTime);
    return mktime(&parsedTime);
}

void fichar(char* idEvent, char* idUser, char* date)
{
    char record[150];
    strcpy(record, idEvent);
    strcat(record, "#");
    strcat(record, idUser);
    strcat(record, "#");
    strcat(record, date);
    
    FILE* f = fopen("./fichajes.txt", "a+");
    if(!f)
    {
        error("ERROR: Open file.");
    }
    
    fputs(record, f);
    fclose(f);
}