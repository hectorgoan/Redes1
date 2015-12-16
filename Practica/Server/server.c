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
void handler_listarEventos(const int socket);
void handler_listar(const int socket, char* command);
void handler_fichar(const int socket, char* command);
void handler_adios(const int socket, char* command);

void str_cut(char* string, const char cutter);
int isUserValid(const char* user);
int isUserInEvent(char* idEvent, char* idUser);
int validateDateForEvent(char* idEvent, char* date);
time_t makeDateFromString(char* date);
void fichar(char* idEvent, char* idUser, char* date);
int isUserLogged(const int socket);
void areNecessaryFiles(void);
void signalHandler(int signal);
ssize_t avSend(int socket, const void* buff, size_t n,
		       int flags, __CONST_SOCKADDR_ARG addr);
ssize_t avRecv(int socket, void* buff, size_t n,
			 int flags, __SOCKADDR_ARG addr);

int gIsTCP;

int gLogged = 0;

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
    
    areNecessaryFiles();
    
    struct sigaction sh;
    memset(&sh, 0, sizeof(sh));
    sh.sa_handler = signalHandler;
    sigemptyset(&sh.sa_mask);
    sigaddset(&sh.sa_mask, SIGUSR1);
    sigaddset(&sh.sa_mask, SIGUSR2);
    sigaddset(&sh.sa_mask, SIGTERM);
    sigaddset(&sh.sa_mask, SIGINT);
    
    if(sigaction(SIGUSR1, &sh, NULL) == -1
       || sigaction(SIGUSR2, &sh, NULL) == -1
       || sigaction(SIGTERM, &sh, NULL) == -1
       || sigaction(SIGINT, &sh, NULL) == -1)
    {
        error("ERROR handling signals");
    }
  
    int sockfd, sckTCP;
    
    struct sockaddr_in servAddr, cliAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    memset(&cliAddr, 0, sizeof(servAddr));
    
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
    
    inet_ntop(AF_INET, &(cliAddr.sin_addr), hostname, sizeof(hostname));
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
                handler_listarEventos(socket);
                break;
            case 4: //FICHAR
                str_cut(buffer, ' ');
                handler_fichar(socket, buffer);
                break;
            case 5: //ADIOS
                str_cut(buffer, ' ');
                handler_adios(socket, buffer);
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
        error("ERROR: Open file");
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
    else if(strncmp(command, "LISTAR", 6) == 0
            && strncmp(command, "LISTAR EVENTOS", 14) != 0)
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
        gLogged = 1;
        send(socket, "CORRECTO\0", 9, 0);
    }
    else
    {
        send(socket, "ERROR Invalid user.\0", 20, 0);
    }
}

void handler_listarEventos(const int socket)
{
    if(!isUserLogged(socket))
    {
        return;
    }
    char* pch;
    char line[150];
    char lineCp[150];
    char idEvent[50];
    time_t tTime;
    struct tm* timeinfo;
    char sTime[50];
    
    FILE* events = fopen("./eventos.txt", "r");
    if(!events)
    {
        error("ERROR: Open file");
    }
    
    time(&tTime);
    timeinfo = localtime(&tTime);
    strftime(sTime, 50, "%d/%m/%G;%T", timeinfo);
    
    while(fgets(line, 150, events) != NULL)
    {
        strcpy(lineCp, line);
        pch = strtok(lineCp, "#");
        strcpy(idEvent, pch);
        if(validateDateForEvent(idEvent, sTime))
        {
            pch = strchr(line, '\r');
            if (pch != NULL)
            {
                strncpy(pch, "\0", 1);
            }
            send(socket, line, strlen(line) + 1, 0);
        }
    }
    send(socket, "\n\0", 2, 0);    
}

void handler_listar(const int socket, char* command)
{
    if(!isUserLogged(socket))
    {
        return;
    }

    char* pch;
    char line[150];
    char lineCp[150];
    char cmdUser[40];
    char cmdEvent[40];
    char user[40];
    char event[40];
    
    pch = strtok(command, " ");
    strcpy(cmdEvent, pch);
    pch = strtok(NULL, " ");
    strcpy(cmdUser, pch);
    
    FILE* f = fopen("./usuarios-eventos.txt", "r");
    if(!f)
    {
        error("ERROR: Open file");
    }
    
    while(fgets(line, 150, f) != NULL)
    {
        strcpy(lineCp, line);
        pch = strchr(lineCp, '\r');
        if(pch != NULL)
        {
            strncpy(pch, "\0", 1);
        }
        pch = strtok(line, "#");
        strcpy(user, pch);
        pch = strtok(NULL, "#");
        strcpy(event, pch);
        pch = strchr(event, '\r');
        if(pch != NULL)
        {
            strncpy(pch, "\0", 1);
        }
        
        if(strcmp(cmdUser, user) == 0 && strcmp(cmdEvent, event) == 0)
        {
            send(socket, lineCp, strlen(lineCp) + 1, 0);
        }
    }
    send(socket, "\n\0", 2, 0);
    
    fclose(f);
}

void handler_fichar(const int socket, char* command)
{
    if(!isUserLogged(socket))
    {
        return;
    }
    char cmdEvent[40];
    char cmdUser[40];
    char date[40];
    char* pch;
    
    pch = strtok(command, " ");
    strcpy(cmdEvent, pch);
    pch = strtok(NULL, " ");
    strcpy(cmdUser, pch);
    pch = strtok(NULL, " ");
    strcpy(date, pch);
    
    char log[150];
    time_t tTime;
    time(&tTime);
    if(isUserInEvent(cmdEvent, cmdUser) && validateDateForEvent(cmdEvent, date))
    {
        fichar(cmdEvent, cmdUser, date);
        send(socket, "CORRECTO\0", 9, 0);
        sprintf(log, "CORRECT \"FICHAR\" from %s in event %s at %s",
                cmdUser,
                cmdEvent,
                (char*)ctime(&tTime));
        logg(log);
    }
    else
    {
        send(socket, "ERROR Invalid user or date.\0", 28, 0);
        sprintf(log, "ERROR \"FICHAR\" from %s in event %s at %s",
                cmdUser,
                cmdEvent,
                (char*)ctime(&tTime));
        logg(log);
    }
}

void handler_adios(const int socket, char* command)
{
    if(!isUserLogged(socket))
    {
        return;
    }
    if(isUserValid(command))
    {
        gLogged = 0;
        send(socket, "CORRECTO\0", 9, 0);
    }
    else
    {
        send(socket, "ERROR Invalid user.\0", 20, 0);
    }
    close(socket);
}

int isUserValid(const char* user)
{
    FILE* users = fopen("./usuarios.txt", "r");
    if(!users)
    {
        error("ERROR: Open file");
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
        error("ERROR: Open file");
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
        error("ERROR: Open file");
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
        error("ERROR: Open file");
    }
    
    fputs(record, f);
    fclose(f);
}

int isUserLogged(const int socket)
{
    if(gLogged == 1)
    {
        return 1;
    }
    else
    {
        send(socket, "ERROR You need to be login first.\0", 34, 0);
        return 0;
    }
}

void areNecessaryFiles(void)
{
    FILE* f;
    f = fopen("./fichajes.txt", "r");
    if(!f)
    {
        error("ERROR: Open file");
    }
    fclose(f);
    
    f = fopen("./usuarios.txt", "r");
    if(!f)
    {
        error("ERROR: Open file");
    }
    fclose(f);
    
    f = fopen("./usuarios-eventos.txt", "r");
    if(!f)
    {
        error("ERROR: Open file");
    }
    fclose(f);
}

void signalHandler(int signal)
{
    switch (signal)
    {
	case SIGUSR1:
	    printf("He recibido la señal SIGUSR1\n");
	    break;
        case SIGUSR2:
	    printf("He recibido la señal SIGUSR2\n");
	    break;
	case SIGINT:
		printf("He recibido la señal SIGINT\n");
		break;
        case SIGTERM:
    	printf("He recibido la señal SIGTERM\n");	    
    }
    printf("Fin de ejecucion\n");
	exit(EXIT_SUCCESS);
}

ssize_t avSend(int socket, const void* buff, size_t n,
		       int flags, __CONST_SOCKADDR_ARG addr)
{
    socklen_t len = sizeof(__CONST_SOCKADDR_ARG);
    if(gIsTCP)
    {
        return send(socket, buff, n, flags);
    }
    else
    {
        return sendto(socket, buff, n, flags, addr, len);
    }
}

ssize_t avRecv(int socket, void * buff, size_t n,
			 int flags, __SOCKADDR_ARG addr)
{
    socklen_t len = sizeof(__CONST_SOCKADDR_ARG);
    if(gIsTCP)
    {
        return recv(socket, buff, n, flags);
    }
    else
    {
        return recvfrom(socket, buff, n, flags, addr, &len);
    }
}