#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex.h>
#define PORT 8080

typedef struct messageServeurServeurSock {
	int type_message;         //2 Initialisation et 3 pour le message final une fois le jeu fini
	int resultat;  					  //0 si on n'a pas trouvé, 1 si on trouve -> ce parametre est utile uniquement si le message est de type 3(message final de fin de partie)
    char identite_envoyeur [50];
} MessageServeurServeurSock;

typedef struct messageClientServeurSock {
	int type_message;         //2 Initialisation et 1 pour le message final une fois le jeu fini
	int resultat;  					  //0 si on n'a pas trouvé, 1 si on trouve -> ce parametre est utile uniquement si le message est de type 3 (message final de fin de partie)
    char identite_envoyeur [50];
} MessageClientServeurSock;

int main(int argc, char const *argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	char *hello = "Hello from socket server";	
	
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
												&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
	
	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address,
								sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
					(socklen_t*)&addrlen))<0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	
	MessageClientServeurSock *messageInitialisation = malloc(sizeof(MessageClientServeurSock));
	valread = read( new_socket , messageInitialisation, 1024);
	
	printf("%d  %d  %s\n",messageInitialisation->type_message, messageInitialisation->resultat, messageInitialisation->identite_envoyeur );
	send(new_socket , hello , strlen(hello) , 0 );
	printf("Hello message sent\n");
	
	
	int descW;
	
	chdir("./pipe"); 
	descW = open("main",O_WRONLY); // on ouvre le pipe main en ecriture
    write(descW, messageInitialisation, sizeof(MessageServeurServeurSock));
    close(descW); // on ferme le descripteur
	 
	return 0;
}

