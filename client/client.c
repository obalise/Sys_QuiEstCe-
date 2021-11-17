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
#include <assert.h>
#include <termios.h>

#define NBR_CARACTERES 80
#define NBR_PERSONNAGES 19


//Structure permettant d'envoyer d'un coup les informations importantes du client vers le serveur
typedef struct messageClientServeur {
    int type_message;                                             //0 Initialisation et 1 pour le message final une fois le jeu fini
    int resultat;  					                                     //1 si on trouve -> ce parametre est utile uniquement si le message est de type 1 (message final de fin de partie)
    pid_t pid;
    char identite_envoyeur [50];
} MessageClientServeur;


/*/Protorypes des fonctions/*/
int menu (char personnageselect[NBR_CARACTERES], char tableau[NBR_PERSONNAGES ][NBR_CARACTERES]);
void strcpy_pointeur(char *, char);
int launch_regex(char *, char *);
void affichagePersonnages(char tableau[NBR_PERSONNAGES ][NBR_CARACTERES]);
void arretCTRLC();
char** str_split(char* , const char);
void personnageTrouve();
void partieDejaCommence();
int attenteTouche ( void );


int main(void){

	/*/Gestion de tous les signaux reçus/*/
    signal(SIGINT, arretCTRLC);                                       //Prise en compte du CTRL+C 
    signal(SIGUSR1, personnageTrouve);                     //Signal reçu quand un adversaire trouve l'élève mystère avant nous
    signal(SIGUSR2, partieDejaCommence);				//Signal reçu quand on essaie de se connecter au serveur alors que le nombre maxi de joueurs est atteint

	/*/Création des variables nécessaires/*/
    int descW, descR;                                                                                         //Descripteurs
    char prenom[50];																							//Prénom de l'utilisateur de ce client
    char tableau[NBR_PERSONNAGES ][NBR_CARACTERES];			//Tableau qui contiendra la base de données utilisée
    char personnageselect[NBR_CARACTERES];										//Array qui contiendra le nom de l'élève mystère
    chdir("../pipe");																								 //On se place dans le bon répertoire pour l'utilisation des pipes

    /*/ On demande le nom du client, qui est tu ?/*/
    printf("\e[1;1H\e[2J"); 																							//Nettoie l'écran de la console
    printf("Bonjour et bienvenue dans le jeu \"Qui est-ce\" ?\n");
    printf("Quel est votre prenom ? (Exemples : Olivier, Arnaud...)\n");
    scanf("%s", prenom);

	/*/On formalise la réponse en utilsant une structure MessageClientServeur/*/
    MessageClientServeur *messageInitialisation = malloc(sizeof(MessageClientServeur));
    messageInitialisation->type_message = 0;
    messageInitialisation->resultat = 0;
    messageInitialisation->pid = getpid();
    strcpy(messageInitialisation->identite_envoyeur, prenom);

    /*/On envoie la structure de message à notre serveur à travers le pipe "main"/*/
    descW=open("main",O_WRONLY); 																					// on ouvre le pipe main en ecriture
    write(descW, messageInitialisation, sizeof(MessageClientServeur));
    close(descW); 																														// on ferme le descripteur
    sleep(1);
	
	/*/On attend la réponse du serveur qui nous donne la base de données et l'élève mystère/*/
    descR=open(prenom,O_RDONLY); 
    read(descR, tableau, sizeof(char)*NBR_PERSONNAGES*NBR_CARACTERES);
    read(descR, personnageselect, sizeof(char)*NBR_CARACTERES);
	close(descR);
    
    printf("\nBase de données et élève mystère chargés !\nAttente du lancement de la partie !\n");
	
	
	/*/On lance le menu qui contient toute la partie Jeu/*/
    int resultat = menu(personnageselect, tableau);

	/* NOTE :
	 *  Le résultat donné par menu ne peut être que 1, ce qui correspond a une partie gagné
	 *  On peut éventuellement penser à un système de pénalité qui se finirait par l'exclusion de la partie le joueur
	 *  Ici ce n'est pas pris en compte
	 */


	/*/On exploite le résultat de la partie donné par menu()/*/
    if (resultat == 1){
		//On refait un message avec notre structure...
        MessageClientServeur *messageFinal = malloc(sizeof(MessageClientServeur));
        messageFinal->type_message = 1;
        messageFinal->resultat = resultat;
        messageFinal->pid = getpid();
        strcpy(messageFinal->identite_envoyeur, prenom);

		//On l'envoie dans le pipe
        descW=open("main",O_WRONLY);
        write(descW, messageFinal,sizeof(MessageClientServeur));
        close(descW); 
        
        //On ferme ce client
        printf("\nFin du client de %s!\n", prenom);
        printf("Au revoir !\n");
        exit (1);
    }
}


int menu(char personnageselect[NBR_CARACTERES], char tableau[NBR_PERSONNAGES][NBR_CARACTERES]) {

    int i = 0;
    char chaine_recherche[NBR_CARACTERES];
    int status = 0;
    char elu[NBR_PERSONNAGES];

    char index_token[] = "";
    strcpy(index_token, personnageselect);
    char **tokens;
    tokens = str_split(index_token, ':');
    if (tokens) {
        strcpy(elu, *(tokens) + 0);
        free(tokens);
    } else {
        printf("Program Error\n");
        exit(0);
    }
    
    do {
        //Menu global       
        printf("\e[1;1H\e[2J");
        printf("\n********** Bienvenue dans le menu du jeu QUI EST-CE ? **********\n");
        printf("|  0 | Quitter le programme\n");
        printf("|  1 | Saisie caracteristique\n");
        printf("|  2 | Proposition du personnage mystere (penalite -> 3sec)\n");
        printf("|  3 | Affiche tableau\n");

        scanf("%d", &i);
        switch (i) {
            case 0:
                exit(0);
                break;
            case 1:
                printf("\e[1;1H\e[2J");                             //Nettoie l'écran de la console
                //Menu des caracteristiques
                printf("|  1 | Couleur des yeux\n");
                printf("|  2 | Couleur des cheveux\n");
                printf("|  3 | Regularité à l'apéro\n");

                scanf("%d", &i);
                switch (i) {
                    case 0:
                        break;
                    case 1:
                        //Menu des yeux
                        printf("|  1 | Vert\n");
                        printf("|  2 | Lunettes\n");
                        printf("|  3 | Marron\n");
                        printf("|  4 | Bleu\n");
                        printf("|  5 | Noisette\n");

                        scanf("%d", &i);
                        switch (i) {
                            case 0:
                                break;
                            case 1:
                                strcpy(chaine_recherche, "vert");
                                break;
                            case 2:
                                strcpy(chaine_recherche, "lunettes");
                                break;
                            case 3:
                                strcpy(chaine_recherche, "marron");
                                break;
                            case 4:
                                strcpy(chaine_recherche, "bleu");
                                break;
                            case 5:
                                strcpy(chaine_recherche, "noisette");
                                break;
                            default:
                                printf("Invalide !\n");
                                break;
                        }
                        printf("Appuyer sur une touche pour continuer.\n");
						attenteTouche();

                        break;
                    case 2:
                        //Menu des cheveux
                        printf("|  1 | Chatain\n");
                        printf("|  2 | Grisonant\n");
                        printf("|  3 | Brun\n");
                        printf("|  4 | Blond\n");
                        printf("|  5 | Boucle\n");

                        scanf("%d", &i);
                        switch (i) {
                            case 0:

                                break;
                            case 1:
                                strcpy(chaine_recherche, "chatain");
                                break;
                            case 2:
                                strcpy(chaine_recherche, "grisonant");
                                break;
                            case 3:
                                strcpy(chaine_recherche, "brun");
                                break;
                            case 4:
                                strcpy(chaine_recherche, "blond");
                                break;
                            case 5:
                                strcpy(chaine_recherche, "boucle");
                                break;
                            default:
                                printf("Invalide\n");
                                break;
                        }
                        printf("Appuyer sur une touche pour continuer.\n");
						attenteTouche();

                        break;
                    case 3:
                        //Menu de la regularite à l'apéro
                        printf("|  1 | Faible\n");
                        printf("|  2 | Moyen\n");
                        printf("|  3 | Fort\n");
                        printf("|  4 | Alcoolique\n");
                        printf("|  5 | Inexistant\n");

                        scanf("%d",&i);
                        switch(i)
                        {
                            case 0: break;
                            case 1:
                                strcpy(chaine_recherche,"faible");
                                break;
                            case 2:
                                strcpy(chaine_recherche,"moyen");
                                break;
                            case 3:
                                strcpy(chaine_recherche,"fort");
                                break;
                            case 4:
                                strcpy(chaine_recherche,"alcoolique");
                                break;
                            case 5:
                                strcpy(chaine_recherche,"inexistant");
                                break;
                            default:
                                printf ("Invalide !\n");
                                break;
                        }
                        printf("Appuyer sur une touche pour continuer.\n");
						attenteTouche();

                        break;
                    default:
                        printf ("Invalide !\n");
                        printf("Appuyer sur une touche pour continuer.\n");
						attenteTouche();

                        break;
                }

                status = launch_regex(personnageselect, chaine_recherche);
                if ((status == 0) || (status == 1)) {
                    if (status == 0) {
                        printf("Votre personnage possède bien cette caractéristique.\n");
                        printf("Appuyer sur une touche pour continuer.\n");
						attenteTouche();

                    }
                    if (status == 1) {
                        printf("Votre personnage n'a pas cette caractéristique.\n");
                        printf("Appuyer sur une touche pour continuer.\n");
						attenteTouche();

                    }
                } else {
                    printf("Erreur comparaison Regex avec la caractéristique !\n");
                    printf("Appuyer sur une touche pour continuer.\n");
					attenteTouche();

                    exit(0);
                }
                break;
            case 2:
				printf("\e[1;1H\e[2J");
				
                printf("Quel est le nom (MAJUSCULE) ou le prenom (minuscule) de l'élève mystère ?\n");
                scanf("%s", chaine_recherche);
                status = launch_regex(elu, chaine_recherche);

                if ((status == 0) || (status ==1)) {    
                    if (status == 0) {
                        printf("Personnage trouvé %s ! C'est gagné !\n", elu);
                        printf("Appuyer sur une touche pour continuer.\n");
						attenteTouche();
                        return 1;
                    }
                    if (status == 1) {
						printf("Pénalité ! Vous devez attendre 3 secondes avant de pouvoir reprendre la partie !\n");
                        sleep(3);

                    }
                } else {
                    printf("Erreur comparaison Regex avec l'élève mystère !\n");
                    printf("Appuyer sur une touche pour continuer.\n");
					attenteTouche();

                    exit(0);
                }
                break;
            case 3:
				printf("\e[1;1H\e[2J"); 
                affichagePersonnages(tableau);
                printf("\nAppuyer sur une touche pour continuer.\n");
				attenteTouche();
				attenteTouche();          //Ce double attenteTouche() est la solution bête et méchante que j'ai trouvé, affichagePersonnages() donne déjà un caractère à la fin de son exécution du à l'utilisation de puts je présume
                break;
            default:
                break;
        }

    }
    while (i != 0);
    return 2;
}

int launch_regex(char *elu, char *p_chaine_recherche){
	/* Permet de comparer l'élève mystère au nom rentré par le joueur 
	 *  Retourne vrai ou faux
	 */
	 
    regex_t regex;
    int reti;
    char msgbuf[100];
    int status = 0;

/* Compile regular expression */
    reti = regcomp(&regex, p_chaine_recherche, 0);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

/* Execute regular expression */
    reti = regexec(&regex, elu, 0, NULL, 0);
    if (!reti) {
       // puts("Match");
        status = 0;
    }
    else if (reti == REG_NOMATCH) {
      //  puts("No match");
        status = 1;
    }
    else {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Echec Regex: %s\n", msgbuf);
        status = 2;
        exit(1);
    }

/* Free memory allocated to the pattern buffer by regcomp() */
    regfree(&regex);
    return status;
}


void affichagePersonnages(char tableau[NBR_PERSONNAGES ][NBR_CARACTERES]){
	/* Fonction permettant au joueur de voir la liste des élèves et de leurs caractéristiques
	 *  C'est fonctionnel mais peu esthétique, on continue d'avoir les séparateurs ':' de la base de données
	 *  Il faudrait refaire cette fonction en utilisant des tokens pour avoir un affichage plus plaisaint
	 */

    printf("Liste des Personnages et de leurs caractéristiques: \n");
    for(int i = 0; i < NBR_PERSONNAGES ; i++)
    {
        printf("%d -> ", i );
        puts(tableau[i]);
    }

}

char** str_split(char* a_str, const char a_delim) {
    char **result = 0;
    size_t count = 0;
    char *tmp = a_str;
    char *last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;
    /* Count how many elements will be extracted. */
    while (*tmp) {
        if (a_delim == *tmp) {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }
    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);
    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;
    result = malloc(sizeof(char *) * count);
    if (result) {
        size_t idx = 0;
        char *token = strtok(a_str, delim);
        while (token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    return result;
}

void personnageTrouve() {
	/* Cas où un adversaire trouve la réponse avant nous. 
	 */
    printf("\nUn de vos adversaires vous a devancé et a trouvé l'élève caché !\nLa carte Olimex vous dévoilera de qui il s'agit !\n");
	printf("\nAppuyer sur une touche pour continuer.\n");
	attenteTouche();
	exit(0);
}


void partieDejaCommence(){
	/*Cas où le nombre de joueur max est atteint sur le serveur et qu'on ne peut plus se connecter
	 */
	printf("\nLa partie a déjà commencé ! Impossible de participer pour le moment, réessayer plus tard s'il vous plaît.\n");
	exit(0);
}


int attenteTouche ( void ) {
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


void arretCTRLC(){
	//Sert sans doute à rien, c'est un reste de notre première itération ou les clients étaient des fils du serveur.
	exit(0);
}
