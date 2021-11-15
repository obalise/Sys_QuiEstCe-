#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <regex.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
//#include <check.h>

#define NBR_CARACTERES 80
#define NBR_PERSONNAGES 19

typedef struct messageClientServeur {
	int type_message;         //0 Initialisation et 1 pour le message final une fois le jeu fini
	int resultat;  					  //0 si on n'a pas trouvé, 1 si on trouve -> ce parametre est utile uniquement si le message est de type 1 (message final de fin de partie)
	pid_t pid;
    char identite_envoyeur [50];
} MessageClientServeur;

/*
START_TEST (test_name)
{
  		//unit test code 
}
END_TEST*/

const char* filename = "eleve.txt";

/*Prototype*/
void return_tableau(char tableau[NBR_PERSONNAGES][NBR_CARACTERES]);
void launch_regex();
int gestionNouveauClient(char prenom[50], char tableau[NBR_PERSONNAGES][NBR_CARACTERES ], char personnageselect[NBR_CARACTERES ]);
int gestionFinPartie(char listeClient [NBR_PERSONNAGES][NBR_CARACTERES], int listePidClient[NBR_PERSONNAGES], int dernierClient, char identite_envoyeur[50]);
void selection_aleatoire_perso(char tableau[NBR_PERSONNAGES][NBR_CARACTERES], char personnageselect[NBR_CARACTERES]);
void fin(int sig);

int main(int argc, char *argv[], char *arge[])
{
	signal(SIGUSR1, SIG_IGN);
	
	//INITIALISATION ?
    char tableau[NBR_PERSONNAGES][NBR_CARACTERES];
    char personnageselect[NBR_CARACTERES];
    
    //int fd_serveur_socket[2][2];
    
    int descR, test, pid, pid1;
	int status=1, sec=0;
    char prenom[50];
	char main[NBR_PERSONNAGES]= "./pipe/main";
	char listeClient [NBR_PERSONNAGES][NBR_CARACTERES];
	int listePidClient [NBR_PERSONNAGES];
	int dernierClient = 0;
	char * myArgv[3];	
	
	sec = atoi(argv[1]);	

	alarm(sec);
    signal(SIGALRM, fin);	 
    
    //Création du pipe FIFO
    unlink(main);
    mkfifo(main,0666); 
    
    return_tableau(tableau);
    selection_aleatoire_perso(tableau, personnageselect); //Pour l'instant on a un seul personnage pour tous les clients, sinon on prend cette ligne et on la met plus bas dans le if(test==0)
    
	pid1=fork();	
	if(pid1 == 0)
	{
		myArgv[0]="home/isen/Sys_QuiEstCe-/stats";
        myArgv[1]="/home/isen/Sys_QuiEstCe-/pipe/main";
		myArgv[2]=NULL;
        execv("/home/isen/Sys_QuiEstCe-/stats", myArgv);
	}
    wait(NULL); // on attende la fin du processus fils
    
	do{
		printf("Je suis en train d'attendre un message d'un client quelconque...\n");
		descR=open(main,O_RDONLY); //ouverture du pipe
		MessageClientServeur *messageRecu = malloc(sizeof(MessageClientServeur));
		read(descR, messageRecu, sizeof(MessageClientServeur)); 
		close(descR);
		
		if(messageRecu->type_message == 0){                                              //Il s'agit du cas ou le message recu est de type initialisation
			printf("Client: %s\n", messageRecu->identite_envoyeur);
			strcpy(prenom, messageRecu->identite_envoyeur);
			test=(strcmp(prenom,"Julien")*strcmp(prenom,"Florent")*strcmp(prenom,"Adrien")*strcmp(prenom,"Olivier"));	

			if(test == 0){
				gestionNouveauClient(prenom, tableau, personnageselect);

				strcpy(listeClient[dernierClient], prenom);
				listePidClient[dernierClient] = messageRecu->pid;
				dernierClient++;
				
			}else{
				printf("Tentative de connexion d'une personne non autorisée !\n");
			}	

		}else if(messageRecu->type_message == 1 && messageRecu->resultat == 1){
				printf("\n%s a trouvé l'élève caché, Bravo !\n", messageRecu->identite_envoyeur);
				
				char  gagnant[50];
				strcpy(gagnant, messageRecu->identite_envoyeur);
				
			/*	pid=fork();	
				if(pid == 0)
				{							
					myArgv[0]="home/isen/Sys_QuiEstCe-/socket";
					myArgv[1]= gagnant;
					myArgv[2]= NULL;
					execv("/home/isen/Sys_QuiEstCe-/socket", myArgv);
				}*/
				
				gestionFinPartie(listeClient, listePidClient, dernierClient, gagnant);            //Il faut envoyer à tous les clients un message disant qu'on a un gagnant et on fait le fork exec pour le socket
				status = 0;
		}
	}while(status == 1);

	//serveur(tableau, personnageselect, sec);

 	return 0;
}

void selection_aleatoire_perso(char tableau[NBR_PERSONNAGES][NBR_CARACTERES], char personnageselect[NBR_CARACTERES])
{
	srand(getpid());
	int numerolignealeatoire = (rand() % NBR_PERSONNAGES);
	printf("Le nombre aléatoire entre 0 et 18 est %d!\n", numerolignealeatoire);
	strcpy(personnageselect, tableau[numerolignealeatoire]);
	printf("Ligne choisie est : %s", personnageselect);
	
}

void return_tableau(char tableau[NBR_PERSONNAGES][NBR_CARACTERES]){
	
	errno = 0;
	
    FILE* in_file = NULL;

    chdir("./BaseDeDonne");

    in_file = fopen(filename, "r+");
    char ligne[NBR_CARACTERES];
    int i = 0;

    if (in_file == NULL)
    {
		printf("Echec ouverture fichier !\n");
		printf("Errno: %d \n", errno);
		exit(0);
	}
	else{
        while(fgets(ligne, NBR_CARACTERES , in_file) != NULL) {
           // printf("%s", ligne);
            strcpy(tableau[i],ligne);
            i++;
            //puts(&tableau[i]);
        }
        // On l'écrit dans le fichier
        fclose(in_file);

	chdir("../.");
    }
    printf("Sortie du fichier\n");
}

int gestionNouveauClient(char prenom[50], char tableau[NBR_PERSONNAGES][NBR_CARACTERES ], char personnageselect[NBR_CARACTERES ])
{
	int pid2,descW;
    char chemin[9]= "./pipe/";
	
	//FORK ET CREATION DU PIPE CLIENT
	pid2=fork();
    if(pid2 == 0){
	printf("on est dans le fils de %s\n",prenom);
	strcat(chemin,prenom);
	unlink(chemin);
	mkfifo(chemin,0666); 

	descW=open(chemin,O_WRONLY); //ouverture du pipe
	write(descW, tableau, sizeof(char)*NBR_PERSONNAGES*NBR_CARACTERES );
	write(descW, personnageselect, sizeof(char)*NBR_CARACTERES);
	close(descW);
	
	exit(0);
	}
	return 74;
}

void fin(int sig)
{
	printf("Fin du jeu\n");
	exit(0);
}

int gestionFinPartie(char listeClient [NBR_PERSONNAGES][NBR_CARACTERES], int listePidClient[NBR_PERSONNAGES], int dernierClient, char nomGagnant[50])
{
	//kill(0, SIGUSR1);
	int descW;
	chdir ("./pipe/");
	printf("[GESTION FIN PARTIE] : ");
	
	for(int i = 0; i <= dernierClient-1; i++){
		
		printf("[GESTION FIN PARTIE] : ");
		printf("Pid: %d\t", listePidClient[i]);
		puts(listeClient[i]);
		
		kill(listePidClient[i], SIGUSR1);
				
		descW=open(listeClient[i],O_WRONLY); 
		
		write(descW, nomGagnant, sizeof(char)*50);
		
		close(descW);	
	}
	return 0;
}
