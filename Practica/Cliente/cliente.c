#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 35400
#define BUFFERSIZE 141

void error(const char* msg);
int readCommand(const char* file, const int line, char* command);
void sendCommand(const int socket, struct sockaddr_in servAddr, char* command);
void str_cut(char* string, const char cutter);
ssize_t avSend(int socket, const void* buff, size_t n,
		       int flags, __CONST_SOCKADDR_ARG addr);
ssize_t avRecv(int socket, void* buff, size_t n,
			 int flags, __SOCKADDR_ARG addr);

int gIsTCP;

int main(int argc, char *argv[])
{
    if(argc < 4)
    {
        error("ERROR Bad number of arguments");
    }
    
    int sockfd;
    struct addrinfo hints;
    struct addrinfo* servAddrInfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    if(getaddrinfo(argv[1], NULL, &hints, &servAddrInfo))
    {
        error("ERROR solving host");
    }
    
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr = ((struct sockaddr_in*)servAddrInfo->ai_addr)->sin_addr;
    servAddr.sin_port = htons(PORT);
    freeaddrinfo(servAddrInfo);
    
    if(strcmp(argv[2], "TCP") == 0)
    {
        gIsTCP = 1;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd == -1)
        {
            error("ERROR: opening socket");
        }
    }
    else if(strcmp(argv[2], "UDP") == 0)
    {
        gIsTCP = 0;
        sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(sockfd == -1)
        {
            error("ERROR: opening socket");
        }
    }
    else
    {
        error("ERROR bad argument");
    }
    
    int val = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
            (char*)&val, sizeof(val)) == -1)
    {
        close(sockfd);
        error("Error in setsockopt");
    }
    
    if(gIsTCP)
    {
        if(connect(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1)
        {
            close(sockfd);
            error("ERROR: Unable to connect to remote");
        }
    }
    
    char command[150];
    int i = 0;
    int flag = 1;
    while(flag)
    {
        flag = readCommand(argv[3], i, command);
        if(flag)
        {
            sendCommand(sockfd, servAddr, command);
        }
        i++;
    }
    
    
    close(sockfd);
    return EXIT_SUCCESS;
}

void error(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int readCommand(const char* file, const int line, char* command)
{
    FILE* cmf = fopen(file, "r");
    if(!cmf)
    {
        error("ERROR: Open file");
    }
    
    int i;
    for(i = 0; i <= line; i++)
    {
        if (fgets(command, 150, cmf) == NULL)
        {
            fclose(cmf);
            return 0;
        }
    }
    char* pch;
    pch = strchr(command, '\r');
    if (pch != NULL)
    {
        strncpy(pch, "\0", 1);
    }
    
    fclose(cmf);
    return 1;
}

void sendCommand(const int socket, struct sockaddr_in servAddr, char* command)
{
    char buffer[BUFFERSIZE];
    size_t buffLen;
    char cmd[150];
    time_t tTime;
    struct tm* timeinfo;
    char sTime[50];
    
    if(strncmp(command, "FICHAR", 6) == 0)
    {
        strcpy(cmd, command);
        time(&tTime);
        timeinfo = localtime(&tTime);
        strftime(sTime, 50, "%d/%m/%G;%T", timeinfo);
        sprintf(cmd, "%s %s", command, sTime);
        avSend(socket, cmd, strlen(cmd) + 1, 0,
                (struct sockaddr*)&servAddr);
        if(buffLen = avRecv(socket, buffer, BUFFERSIZE, 0,
                (struct sockaddr*)&servAddr) == -1)
        {
            error("ERROR: Failed to receive");
        }
        buffer[BUFFERSIZE] = '\0';
        puts(buffer);
    }
    else if(strncmp(command, "LISTAR", 6) == 0)
    {
        avSend(socket, command, strlen(command) + 1, 0,
                (struct sockaddr*)&servAddr);
        while(1)
        {
            if(buffLen = avRecv(socket, buffer, BUFFERSIZE, 0,
                (struct sockaddr*)&servAddr) == -1)
            {
                error("ERROR: Failed to receive");
            }
            buffer[BUFFERSIZE] = '\0';
            if(strncmp(buffer, "\n", 1) != 0)
            {
                puts(buffer);
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        avSend(socket, command, strlen(command) + 1, 0,
                (struct sockaddr*)&servAddr);
        if(buffLen = avRecv(socket, buffer, BUFFERSIZE, 0,
                (struct sockaddr*)&servAddr) == -1)
        {
            error("ERROR: Failed to receive");
        }
        buffer[BUFFERSIZE] = '\0';
        puts(buffer);
    }
}

//Cut the string for the first second param: ("This is a example.", " ") -> "is a example."
void str_cut(char* string, const char cutter)
{
    char* pch;
    pch = strchr(string, cutter);
    if(pch == NULL)
    {
        error("NULL pointer");
    }
    strcpy(string, pch + 1);
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