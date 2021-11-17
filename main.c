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

//Structure permettant de recevoir d'un coup les informations importantes venant du client
typedef struct messageClientServeur {
	int type_message;         //0 Initialisation et 1 pour le message final une fois le jeu fini
	int resultat;  					  //0 si on n'a pas trouvé, 1 si on trouve -> ce parametre est utile uniquement si le message est de type 1 (message final de fin de partie)
	pid_t pid;
    char identite_envoyeur [50];
} MessageClientServeur;

/* NOTE :
 * Nous n'avons pas eu le temps de mettre en place le unit testing
 * La prochaine on utilisera le framework "Check" ou alors le framework "Criterion"
 * Plus bas voici un prototype de test "Check"
 */

/*
START_TEST (test_name)
{
  		//unit test code 
}
END_TEST*/

/*/Variables globales/*/
const char* filename = "eleve.txt";       //Nom du fichier base de données
int partieEnCours = 0;								  //0 si le nombre de joueurs n'a pas été atteint, 1 si le nombre de joueurs a été atteint
int nbrJoueur = 0;								

/*/Prototypes des fonctions/*/
void return_tableau(char tableau[NBR_PERSONNAGES][NBR_CARACTERES]);
void launch_regex();
void gestionNouveauClient(char prenom[50], char tableau[NBR_PERSONNAGES][NBR_CARACTERES ], char personnageselect[NBR_CARACTERES ],pid_t pid_client);
int gestionFinPartie(char listeClient [NBR_PERSONNAGES][NBR_CARACTERES], int listePidClient[NBR_PERSONNAGES], int dernierClient, char identite_envoyeur[50]);
void selection_aleatoire_perso(char tableau[NBR_PERSONNAGES][NBR_CARACTERES], char personnageselect[NBR_CARACTERES]);
void fin(int sig);
void clean_stdin(void);
int attenteTouche ( void ) ;
void menuServeur(char listeClient [NBR_PERSONNAGES][NBR_CARACTERES], int listePidClient[NBR_PERSONNAGES], int dernierClient, char tableau[NBR_PERSONNAGES][NBR_CARACTERES ], char personnageselect[NBR_CARACTERES ]);


/*/Notre main prend en argument un nombre de secondes, il détermine le temps durant lequel le serveur tournera/*/
int main(int argc, char *argv[], char *arge[])
{
	//Gestion des signals reçus
	signal(SIGUSR1, SIG_IGN);
	signal(SIGALRM, fin);	 

	/*/Création des variables nécessaires/*/
    char tableau[NBR_PERSONNAGES][NBR_CARACTERES];
    char personnageselect[NBR_CARACTERES];
    int descR, test, partieFini;    
    pid_t pid, pid1;
	int sec=0;
    char prenom[50];
	char main[NBR_PERSONNAGES]= "./pipe/main";
	char listeClient [NBR_PERSONNAGES][NBR_CARACTERES];
	int listePidClient [NBR_PERSONNAGES];
	int dernierClient = 0;
	char * myArgv[3];	
	sec = atoi(argv[1]);	
	
	//Mise en place de l'alarme pour éteindre le programme au bout des x sec donner en paramètres lors du lancement
	alarm(sec);
    
    
    /*/Création du pipe FIFO, il permettra de prendre les inputs de tous les clients/*/
    unlink(main);
    mkfifo(main,0666); 
    
    printf("\e[1;1H\e[2J"); //Nettoie l'écran de la console
    
    
    return_tableau(tableau);                                                            //On charge le tableau avec la base de données  
    selection_aleatoire_perso(tableau, personnageselect);       //On selectionne l'élève mystère une première fois
    
    /* NOTE :
     * Une partie statistiques était initialement prévu
     * Voici le fork+exec permettant de lancer le programme stats.c
     * Pas gérer ici
     */
	pid1=fork();	
	if(pid1 == 0)
	{
		myArgv[0]="home/isen/Sys_QuiEstCe-/stats";
        myArgv[1]="/home/isen/Sys_QuiEstCe-/pipe/main";
		myArgv[2]=NULL;
        execv("/home/isen/Sys_QuiEstCe-/stats", myArgv);
	}
    wait(NULL); 

	/*/Cette boucle while continue à l'infini tant que l'admin server n'arrête pas le programme depuis le menu, ou avec CTRL+C/*/
	while(1){
		
		//Menu Serveur permet de set le nombre de joueurs max ainsi que de regénérer un élève mystère
		menuServeur(listeClient, listePidClient, dernierClient, tableau, personnageselect);

		printf("Partie lancée : Attente des %d joueurs.\n", nbrJoueur);
		
		partieEnCours = 0;

		//Tant que le nombre de joueurs max n'est pas atteint on continue d'attendre des messages de types initialisation venant des client
		while (nbrJoueur != 0){
			descR=open(main,O_RDONLY); //ouverture du pipe
			MessageClientServeur *messageRecu = malloc(sizeof(MessageClientServeur));
			read(descR, messageRecu, sizeof(MessageClientServeur)); 
			close(descR);
			
			if(messageRecu->type_message == 0){                                             
				strcpy(prenom, messageRecu->identite_envoyeur);
				test=(strcmp(prenom,"Julien")*strcmp(prenom,"Florent")*strcmp(prenom,"Adrien")*strcmp(prenom,"Olivier")*strcmp(prenom,"Vincent")*strcmp(prenom,"Arnaud")*strcmp(prenom,"Anakin")*strcmp(prenom,"Mathieu")*strcmp(prenom,"Sabrina")*strcmp(prenom,"Guillaume")*strcmp(prenom,"Thomas")*strcmp(prenom,"Remi")*strcmp(prenom,"Frederic")*strcmp(prenom,"Maxime")*strcmp(prenom,"Eddy")*strcmp(prenom,"Emmanuel")*strcmp(prenom,"Yoni")*strcmp(prenom,"Claude")*strcmp(prenom,"Romain"));	

				if(test == 0){
					gestionNouveauClient(prenom, tableau, personnageselect, messageRecu->pid);
					
					strcpy(listeClient[dernierClient], prenom);
					listePidClient[dernierClient] = messageRecu->pid;
					dernierClient++;
					nbrJoueur--;
					
					printf("Nouveau Joueur connecté : Prenom %s, PID %d\n", messageRecu->identite_envoyeur, messageRecu->pid);

				}else{
					printf("Tentative de connexion d'une personne non autorisée !\n");
					kill(messageRecu->pid, SIGKILL);
					printf("Processus intrus assassiné avec un SIGKILL !\n");
				}	

			}
		}
		
		
		partieEnCours = 1;
		
		printf("\nTous les joueurs se sont connectés.\n");
		
		partieFini = 1;
		
		//Les joueurs sont connectés, on attend maintenant un message de fin venant du client gagnant
		while (partieFini == 1){
			descR=open(main,O_RDONLY); //ouverture du pipe
			MessageClientServeur *messageRecu = malloc(sizeof(MessageClientServeur));
			read(descR, messageRecu, sizeof(MessageClientServeur)); 
			close(descR);
			
			if(messageRecu->type_message == 0){                                             
				strcpy(prenom, messageRecu->identite_envoyeur);
				test=(strcmp(prenom,"Julien")*strcmp(prenom,"Florent")*strcmp(prenom,"Adrien")*strcmp(prenom,"Olivier"));	

				if(test == 0){
					gestionNouveauClient(prenom, tableau, personnageselect, messageRecu->pid);
					
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
			}
			partieFini = 0;
		}
		printf("Appuyer sur une touche pour continuer.\n");
		attenteTouche();
		attenteTouche();
	}

 	return 0;
}

void selection_aleatoire_perso(char tableau[NBR_PERSONNAGES][NBR_CARACTERES], char personnageselect[NBR_CARACTERES])
{
	/* Permet de selectionné une ligne aléatoire du tableau contenant la base de données
	 * Cette ligne contient l'élève mystère et ses caractèristiques
	 */
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
	/* Permet d'envoyer au client les données nécessaires
	 * Gere l'arrivé d'un client alors que la partie a commencé
	 */
	int pid2,descW;
    char chemin[9]= "./pipe/";
	
	//FORK ET CREATION DU PIPE CLIENT
	pid2=fork();
    if(pid2 == 0){
	//printf("on est dans le fils de %s\n",prenom);
	strcat(chemin,prenom);
	
	if(unlink(chemin) < 0);

	if(mkfifo(chemin,0666) < 0);


	if (partieEnCours == 0){
		descW=open(chemin,O_WRONLY); //ouverture du pipe
		write(descW, tableau, sizeof(char)*NBR_PERSONNAGES*NBR_CARACTERES );
		write(descW, personnageselect, sizeof(char)*NBR_CARACTERES);
		close(descW);
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
	/* Envoie un SIGUSR1 à chaque client pour qu'il sache que quelqu'un a déjà trouvé la solution
	 */
	for(int i = 0; i <= dernierClient-1; i++){
	
		kill(listePidClient[i], SIGUSR1);
				
	}
	return 0;
}


int attenteTouche ( void ) 
{
	/* Fonction que Florent VILLENEUVE a volé sur internet de manière complétement éhonté
	 *  Elle sert dans les menus à attendre avant de retourner au "menu global" (premier switch case)
	 *  Sinon le joueur n'aurait pas le temps de voir ce qu'il s'affiche
	 */
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
		printf("|  4 | Début attente des joueur(s) : %d joueurs prévus\n", nbrJoueur);	
		
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
