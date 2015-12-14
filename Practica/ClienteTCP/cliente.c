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
void sendCommand(const int socket, char* command);
void str_cut(char* string, const char cutter);

int main(int argc, char *argv[])
{
    if(argc < 4)
    {
        error("ERROR Bad number of arguments.\n");
    }
    
    int sockfd, sckTCP;
    struct hostent* server;
    struct addrinfo hints;
    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    
    struct addrinfo* addressInfo;
    bzero(&addressInfo, sizeof(addressInfo));
    
    
    if(getaddrinfo(argv[1], NULL, &hints, &addressInfo) != 0)
    {
        error("ERROR solving host.\n");
    }
    
    struct sockaddr_in servAddr, myAddr;
    bzero(&servAddr, sizeof(servAddr));
    bzero(&myAddr, sizeof(servAddr));
    
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_addr = ((struct sockaddr_in*)addressInfo->ai_addr)->sin_addr;
    freeaddrinfo(addressInfo);
    servAddr.sin_port = htons(PORT);
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        error("ERROR: opening socket.\n");
    }
    
    if(connect(sockfd, (const struct sockaddr*)&servAddr, sizeof(servAddr)) == -1)
    {
        error("ERROR: Unable to connect to remote.\n");
    }
    
    
    char command[150];
    int i = 0;
    int flag = 1;
    while(flag)
    {
        flag = readCommand(argv[3], i, command);
        if(flag)
        {
            sendCommand(sockfd, command);
        }
        i++;
    }
    
    
    close(sockfd);
    return 0;
}

void error(const char* msg)
{
    perror(msg);
    exit(1);
}

int readCommand(const char* file, const int line, char* command)
{
    FILE* cmf = fopen(file, "r");
    if(!cmf)
    {
        error("ERROR: Open file.");
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
    fclose(cmf);
    return 1;
}

void sendCommand(const int socket, char* command)
{
    char buffer[BUFFERSIZE];
    size_t buffLen;
    
    send(socket, command, strlen(command), 0);
    if(buffLen = recv(socket, buffer, BUFFERSIZE, 0) == -1)
    {
        error("ERROR: Failed to receive.");
    }
    buffer[BUFFERSIZE] = '\0';
    puts(buffer);
}

//Cut the string for the first second param: ("This is a example.", " ") -> "is a example."
void str_cut(char* string, const char cutter)
{
    char* pch;
    pch = strchr(string, cutter);
    strcpy(pch, string);
}