#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <regex.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#define PORT 8080


typedef struct messageClientServeurSock {
	int type_message;         //2 Initialisation et 3 pour le message final une fois le jeu fini
	int resultat;  					  //0 si on n'a pas trouvé, 1 si on trouve -> ce parametre est utile uniquement si le message est de type 3 (message final de fin de partie)
    char identite_envoyeur [50];
} MessageClientServeurSock;


int main(int argc, char const *argv[])
{
	int sock = 0, status = 1, valread;
	struct sockaddr_in serv_addr;
	char prenom[50];
	char *hello = "Hello from socket client";
	char buffer[1024] = {0};
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	
	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}
	
	while(status)
	{
		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		{	
			printf("\n Waiting Connection \n");
			//return -1;
		} else {
			status = 0;
		}	
	}
	
	printf("Bonjour bienvenu de le jeu Qui est-ce ?\n");
	printf("Quel est votre Prenom?\n");
	scanf("%s", prenom);
	
	MessageClientServeurSock *messageInitialisation = malloc(sizeof(MessageClientServeurSock));
	messageInitialisation->type_message = 2;
	messageInitialisation->resultat = 0;
	strcpy(messageInitialisation->identite_envoyeur, prenom);
	
	send(sock , messageInitialisation , sizeof(MessageClientServeurSock) , 0 );
	
	
	//send(sock , hello , strlen(hello) , 0 );

	
	printf("Hello message sent\n");
	valread = read( sock , buffer, 1024);
	printf("%s\n",buffer );
	return 0;
}

