/*
** Fichero: servidor.c
** Autores: 
** Héctor Gonzalo Andrés DNI 71038384D
** Néstor 
*/
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//			INCLUDE's, DEFINE's & GLOBAL VAR's SECTION
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include "mylib.h"
#include <signal.h>


#define PORT 5030
#define BACKLOG 10	//Defines the maximum lenght to wich the queue of
					//pending connections for serverfd may grow
int flag = 0;


//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
//						PROGRAM's BODY
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
void main(void)
{

	//Welcome message
	//system ("tput clear");	//Cleans terminal, disabled for debugging
	printf("Hola!, soy el servidor, vivo aquí\n");
	signal(SIGINT, INThandler);	//Will detect SIGINT (ctrl+c)
	
	//Variable declaration zone
	int serverfd = 0;	//Will store server's file descriptor
	int clientfd = 0;	//Will store client's file descriptor

	
    struct sockaddr_in serv_addr;	//Will store server's address info
    struct sockaddr_in client_addr;	//Will store client's address info

    char sendBuff[1025];	//Will be sent to the client
    time_t ticks;			//Will be used to build the response message

    int sin_size;    
    int resultadoBind;
    int resultadoSockOpt;
    int iSetOption = 1;



	/*	by using this we'll se all users printed in servers terminal
		it's just an example*/

	char *stringWithContentOfTextFile = ObtainStringFromTextFile();
	ObtainUsersFromString(stringWithContentOfTextFile);
    //*/
	int r=0;


    //Function's body starts here!
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)	//SOCK_STREAM TCP
    {
    	printf("Error en la llamada a socket()\n");
    	exit (-1);
    }

    if ((setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption))) == -1)	//Only avaliable in linux 2.7+
    {
    	printf("Error en la llamada a setsockopt()\n");
    	exit (-1);
    }
    


    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT); 

    

    resultadoBind = bind(serverfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
    if ((resultadoBind) == -1)
    {
    	printf("resultadoBind %d\n", resultadoBind);
    	printf("Error en la llamada a bind()\n");
    	exit (-1);
    }

    if ((listen(serverfd, BACKLOG)) == -1)
    {
    	printf("Error en la llamada a listen()\n");
    	exit (-1);
    }
     
    

    while(flag != 1)
    {
    	
    	/*
    	sin_size = sizeof(struct sockaddr_in);
    	printf("%d\n", sin_size);*/
    	printf("Conexión entrante\n");
    	if ((clientfd = accept(serverfd, (struct sockaddr*) &client_addr, &(sin_size))) == -1)
    	{
    		printf("Error al aceptar una llamada entrante\n");
    		exit (-1);
    	}
    	
        ticks = time(NULL);

        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        write(clientfd, sendBuff, strlen(sendBuff)); 

        close(clientfd);
        sleep(1);
    }	

	

}


//Other functions
void  INThandler(int sig)	
{
	//::::::::::::::::::::::::::::::::::::::::::::
	//This function handles when ctrl+c is pushed
	//::::::::::::::::::::::::::::::::::::::::::::
	printf("Adios!\n");
	flag = 1;
	exit (0); 
}

void ObtainUsersFromString (char *strWithUsers)
{
	//::::::::::::::::::::::::::::::::::::::::::::
	//This function obtains the users contained in
	//msg, separated by /
	//::::::::::::::::::::::::::::::::::::::::::::
	char *str2 = strWithUsers;
	char *str;
	memcpy(str, str2, sizeof(strWithUsers)/sizeof(char*));

	int r=0;
	int l=0;
	int i=0;
	l=ObtainNumberOfUsersFromString(str2);

	//printf("%d\n", l);

	const char s[2] = "/";
	char* users[l];
   	const char *token;
   	token = strtok(str, s);

   	while(token != NULL) 
   	{   		
  		//printf("%s\n", token);
  		users[i] = token;
   	   	token = strtok(NULL, s);
   	   	i++;
   	}

   	/*
   	for (r = 0; r < l; r++)	//it works---------------------------------en users tengo los usuarios--------------------------------
   	{
   		printf("%s\n", users[r]);
   	}*/


}

int ObtainNumberOfUsersFromString (char *strWithUsers)
{
	//::::::::::::::::::::::::::::::::::::::::::::
	//This function obtains the number of users 
	//contained in msg, separated by /
	//::::::::::::::::::::::::::::::::::::::::::::
	char *str = strWithUsers;
	
	int i=0;
	const char s[2] = "/";
   	const char *token;

   	token = strtok(str, s);

   	while(token != NULL) 
   	{

  		i++;  		
    
   	   	token = strtok(NULL, s);
   	}
   	
   	printf("%d\n", i);
   	return i;

}

char* ObtainStringFromTextFile (void)
{
	//::::::::::::::::::::::::::::::::::::::::::::::::::
	//This function extracts the content of usuarios.txt
	//and store it in a "string"
	//::::::::::::::::::::::::::::::::::::::::::::::::::
	char *stringToReturn = 0;
	char *stringToModify = 0;

	long length;
	FILE *f = fopen ("usuarios.txt", "rb");

	if (f != NULL)
	{
		fseek (f, 0, SEEK_END);
  		length = ftell (f);
  		fseek (f, 0, SEEK_SET);

  		stringToModify= malloc(length);

  		if (stringToModify)
  		{
  			fread(stringToModify, 1, length, f);
  		}

  		fclose(f);
	}

	stringToReturn = replaceStringAforStringB(stringToModify, "#", "/");
	stringToReturn = replaceStringAforStringB(stringToReturn, "\n", "/");


	return stringToReturn;
}


char *replaceStringAforStringB(char *str, char *a, char *b)
{
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::
	//This function replace the string a with b if possible
	//:::::::::::::::::::::::::::::::::::::::::::::::::::::
    char *buffer;
    char *p;

    if(!(p = strstr(str, a)))	//Does str contains a?
    {
    	return str;
    }
    

    buffer = (char*)malloc(strlen(str)+strlen(b)-strlen(a)+1);

    strncpy(buffer, str, p-str);
    
    sprintf(buffer+(p-str), "%s%s", b, p+strlen(a));

    if( buffer != (p=replaceStringAforStringB(buffer, a, b)) )
    {
    	free(buffer);
    }
          
      
    return p;
}
