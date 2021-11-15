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
#include <termios.h>
#include <time.h>
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
int partieEnCours = 0;
int nbrJoueur = 0;

/*Prototype*/
void return_tableau(char tableau[NBR_PERSONNAGES][NBR_CARACTERES]);
void launch_regex();
void gestionNouveauClient(char prenom[50], char tableau[NBR_PERSONNAGES][NBR_CARACTERES ], char personnageselect[NBR_CARACTERES ],pid_t pid_client);
int gestionFinPartie(char listeClient [NBR_PERSONNAGES][NBR_CARACTERES], int listePidClient[NBR_PERSONNAGES], int dernierClient, char identite_envoyeur[50]);
void selection_aleatoire_perso(char tableau[NBR_PERSONNAGES][NBR_CARACTERES], char personnageselect[NBR_CARACTERES]);
void fin(int sig);

void clean_stdin(void)
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}


int attenteTouche ( void ) 
{
  int ch;
  struct termios oldt, newt;
  tcgetattr ( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr ( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );
  return ch;
}

void menuServeur(char listeClient [NBR_PERSONNAGES][NBR_CARACTERES], int listePidClient[NBR_PERSONNAGES], int dernierClient, char tableau[NBR_PERSONNAGES][NBR_CARACTERES ], char personnageselect[NBR_CARACTERES ]){
	int i = 0;
	int sortieMenu =0;
	
	do{
		printf("\e[1;1H\e[2J");
		printf("\n********** Bienvenue dans le menu Serveur du jeu QUI EST-CE ? **********\n");
		printf("|  0 | Quitter le programme\n");
		printf("|  1 | Voir l'élève mystère actuel\n");
		printf("|  2 | Génération de l'élève mystère (deconnexion forcée des joueurs actuels)\n");
		printf("|  3 | Selection nombre de Client attendu\n");	
		printf("|  4 | Début attente des joueurs : %d joueurs prévus\n", nbrJoueur);	
		
		 scanf("%d", &i);
		 
		switch (i) {
		case 0:
			exit(0);
			break;
		case 1:	
			printf("Elève mystère est : %s", personnageselect);
			printf("\nAppuyer sur une touche pour continuer.\n");
			attenteTouche();
			attenteTouche();
			break;
			
		case 2:
			selection_aleatoire_perso(tableau, personnageselect);
			printf("\nGénération effectuée.\n");
			printf("Appuyer sur une touche pour continuer.\n");
			attenteTouche();
			attenteTouche();
			break;
			
		case 3:
			printf("Veuillez donnez un nombre entre 1 et 19: ");
			scanf("%d", &nbrJoueur);
			printf("Appuyer sur une touche pour continuer.\n");
			attenteTouche();
			attenteTouche();
			break;
			
		case 4:
			sortieMenu = 1;
			break;
		default:

			break;
		}
	}while(sortieMenu == 0);

}


int main(int argc, char *argv[], char *arge[])
{
	signal(SIGUSR1, SIG_IGN);
	signal(SIGALRM, fin);	 

    char tableau[NBR_PERSONNAGES][NBR_CARACTERES];
    char personnageselect[NBR_CARACTERES];
    
    int descR, descW, test;    
    
    pid_t pid, pid1;
	int status=1, sec=0;
    char prenom[50];
	char main[NBR_PERSONNAGES]= "./pipe/main";
	char listeClient [NBR_PERSONNAGES][NBR_CARACTERES];
	int listePidClient [NBR_PERSONNAGES];
	int dernierClient = 0;
	char * myArgv[3];	
	
	sec = atoi(argv[1]);	
	alarm(sec);
    
    
    //Création du pipe FIFO
    unlink(main);
    mkfifo(main,0666); 
    
    printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
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
    
	
	menuServeur(listeClient, listePidClient, dernierClient, tableau, personnageselect);

	printf("Attente des %d joueurs.\n", nbrJoueur);
	do{
		
		
		/*         */
		if(partieEnCours == 1){
			
			char passage[8] = "florent";
			for(int i = 0; i <= dernierClient-1; i++){	
				printf("[YAYAYAYAYA] : ");
				printf("Pid: %d\t", listePidClient[i]);
				printf("%s",listeClient[i]);
				
			descW=open(listeClient[i],O_WRONLY); //ouverture du pipe
			write(descW, passage, sizeof(char)*8);
			close(descW);
			printf("ARGH6\n");
			kill(listePidClient[i], SIGCHLD);
			printf("ARGH7\n");
			
			}
		}
		
		printf("ARGH\n");
		descR=open(main,O_RDONLY); //ouverture du pipe
		MessageClientServeur *messageRecu = malloc(sizeof(MessageClientServeur));
		read(descR, messageRecu, sizeof(MessageClientServeur)); 
		close(descR);
		
		if(messageRecu->type_message == 0){                                             
			strcpy(prenom, messageRecu->identite_envoyeur);
			test=(strcmp(prenom,"Julien")*strcmp(prenom,"Florent")*strcmp(prenom,"Adrien")*strcmp(prenom,"Olivier"));	

			if(test == 0){
				gestionNouveauClient(prenom, tableau, personnageselect, messageRecu->pid);
				printf("ARGH2\n");
				if(partieEnCours == 0){
				strcpy(listeClient[dernierClient], prenom);
				listePidClient[dernierClient] = messageRecu->pid;
				dernierClient++;
				nbrJoueur--;
				}
				
			}else{
				printf("Tentative de connexion d'une personne non autorisée !\n");
				kill(messageRecu->pid, SIGKILL);
				printf("Processus intrus assassiné avec un SIGKILL !\n");
			}	

		}else if(messageRecu->type_message == 1 && messageRecu->resultat == 1){
				printf("\n%s a trouvé l'élève caché, Bravo !\n", messageRecu->identite_envoyeur);
				
				char  gagnant[50];
				strcpy(gagnant, messageRecu->identite_envoyeur);
				
			    pid=fork();	
				if(pid == 0)
				{							
					myArgv[0]="home/isen/Sys_QuiEstCe-/socket";
					//myArgv[0]="/home/zeus/Bureau/Projet SYSEXP/Sys_QuiEstCe-/socket";
					myArgv[1]= gagnant;
					myArgv[2]= NULL;
					execv("/home/isen/Sys_QuiEstCe-/socket", myArgv);
					//execv("/home/zeus/Bureau/Projet SYSEXP/Sys_QuiEstCe-/socket", myArgv);
				}
				gestionFinPartie(listeClient, listePidClient, dernierClient, gagnant);            //Il faut envoyer à tous les clients un message disant qu'on a un gagnant et on fait le fork exec pour le socket
				status = 0;
		}
		
				if(nbrJoueur == 0){
			partieEnCours = 1;
			printf("ARGH5\n");
		}
		
		
	}while(status == 1);


 	return 0;
}

void selection_aleatoire_perso(char tableau[NBR_PERSONNAGES][NBR_CARACTERES], char personnageselect[NBR_CARACTERES])
{
	srand(time(NULL));
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

void gestionNouveauClient(char prenom[50], char tableau[NBR_PERSONNAGES][NBR_CARACTERES ], char personnageselect[NBR_CARACTERES ], pid_t pid_client)
{
	int pid2,descW;
    char chemin[9]= "./pipe/";
	
	//FORK ET CREATION DU PIPE CLIENT
	pid2=fork();
    if(pid2 == 0){
	//printf("on est dans le fils de %s\n",prenom);
	strcat(chemin,prenom);
	unlink(chemin);
	mkfifo(chemin,0666); 

	if (partieEnCours == 0){
		descW=open(chemin,O_WRONLY); //ouverture du pipe
		write(descW, tableau, sizeof(char)*NBR_PERSONNAGES*NBR_CARACTERES );
		write(descW, personnageselect, sizeof(char)*NBR_CARACTERES);
		close(descW);
		printf("ARGH3\n");
	}else if (partieEnCours == 1){
		kill(pid_client, SIGUSR2);
	}
	exit(0);
	}
}

void fin(int sig)
{
	printf("Fin du jeu\n");
	exit(0);
}

int gestionFinPartie(char listeClient [NBR_PERSONNAGES][NBR_CARACTERES], int listePidClient[NBR_PERSONNAGES], int dernierClient, char nomGagnant[50])
{
	for(int i = 0; i <= dernierClient-1; i++){
		
		printf("[GESTION FIN PARTIE] : ");
		printf("Pid: %d\t", listePidClient[i]);
		puts(listeClient[i]);
		
		kill(listePidClient[i], SIGUSR1);
				
	}
	return 0;
}
