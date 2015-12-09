#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 35400
#define BUFFERSIZE 141

void error(const char* msg);

int main(int argc, char *argv[])
{
    if(argc < 3)
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
    
    sockfd = socket(AF_INET, SOCK_STREAM, NULL);
    if(sockfd == -1)
    {
        error("ERROR: opening socket.\n");
    }
    
    if(connect(sockfd, (const struct sockaddr*)&servAddr, sizeof(servAddr)) == -1)
    {
        error("ERROR: Unable to connect to remote.\n");
    }
    
    send(sockfd, "peaw", 4, NULL);
    
    
    close(sockfd);
    return 0;
}

void error(const char* msg)
{
    perror(msg);
    exit(1);
}